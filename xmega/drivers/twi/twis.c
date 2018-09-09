/**
 * \file *********************************************************************
 *
 * \brief
 *      XMEGA TWI slave driver source file.
 *
 *      This file contains the function implementations the XMEGA TWI slave
 *      driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA TWI slave module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is
 *      discouraged, in some occasions the operator makes it possible to write
 *      pretty clean and neat code. In this driver, the construct is used to
 *      set or not set a configuration bit based on a boolean input parameter,
 *      such as the "some_parameter" in the example above.
 *
 * \par Application note:
 *      AVR1308: Using the XMEGA TWI
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 *      Microchip Technology Inc: http://www.microchip.com \n
 *
 * $Revision: 2660 $
 * $Date: 2009-08-11 12:28:58 +0200 (Tue, 11 Aug 2009) $  \n
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "twis.h"


/*! \brief Initalizes TWI slave driver structure.
 *
 *  Initialize the instance of the TWI Slave and set the appropriate values.
 *
 *  \param twi                  The TWI_Slave_t struct instance.
 *  \param module               Pointer to the TWI module.
 *  \param processDataFunction  Pointer to the function that handles incoming data.
 */
void TWI_SlaveInitializeDriver(TWI_Slave_t *twi,
                               TWI_t *module,
                               void (*processDataFunction) (void))
{
	twi->interface = module;
	twi->Process_Data = processDataFunction;
	twi->bytesReceived = 0;
	twi->bytesSent = 0;
	twi->status = TWIS_STATUS_READY;
	twi->result = TWIS_RESULT_UNKNOWN;
	twi->abort = false;
}


/*! \brief Initialize the TWI module.
 *
 *  Enables interrupts on address recognition and data available.
 *  Remember to enable interrupts globally from the main application.
 *
 *  \param twi        The TWI_Slave_t struct instance.
 *  \param address    Slave address for this module.
 *  \param intLevel   Interrupt level for the TWI slave interrupt handler.
 */
void TWI_SlaveInitializeModule(TWI_Slave_t *twi,
                               uint8_t address,
                               TWI_SLAVE_INTLVL_t intLevel)
{
	twi->interface->SLAVE.CTRLA = intLevel |
	                              TWI_SLAVE_DIEN_bm |
	                              TWI_SLAVE_APIEN_bm |
	                              TWI_SLAVE_ENABLE_bm;
	twi->interface->SLAVE.ADDR = (address<<1);
}


/*! \brief Common TWI slave interrupt service routine.
 *
 *  Handles all TWI transactions and responses to address match, data reception,
 *  data transmission, bus error and data collision.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveInterruptHandler(TWI_Slave_t *twi)
{
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;

	/* If bus error. */
	if (currentStatus & TWI_SLAVE_BUSERR_bm) {
		twi->bytesReceived = 0;
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_BUS_ERROR;
		twi->status = TWIS_STATUS_READY;
	}

	/* If transmit collision. */
	else if (currentStatus & TWI_SLAVE_COLL_bm) {
		twi->bytesReceived = 0;
		twi->bytesSent = 0;
		twi->result = TWIS_RESULT_TRANSMIT_COLLISION;
		twi->status = TWIS_STATUS_READY;
	}

	/* If address match. */
	else if ((currentStatus & TWI_SLAVE_APIF_bm) &&
	        (currentStatus & TWI_SLAVE_AP_bm)) {

		TWI_SlaveAddressMatchHandler(twi);
	}

	/* If stop (only enabled through slave read transaction). */
	else if (currentStatus & TWI_SLAVE_APIF_bm) {
		TWI_SlaveStopHandler(twi);
	}

	/* If data interrupt. */
	else if (currentStatus & TWI_SLAVE_DIF_bm) {
		TWI_SlaveDataHandler(twi);
	}

	/* If unexpected state. */
	else {
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_FAIL);
	}
}

/*! \brief TWI address match interrupt handler.
 *
 *  Prepares TWI module for transaction when an address match occurs.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveAddressMatchHandler(TWI_Slave_t *twi)
{
	/* If application signalling need to abort (error occured). */
	if (twi->abort) {
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
		twi->abort = false;
	} else {
		twi->status = TWIS_STATUS_BUSY;
		twi->result = TWIS_RESULT_UNKNOWN;

		/* Disable stop interrupt. */
		uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
		twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

		twi->bytesReceived = 0;
		twi->bytesSent = 0;

		/* Send ACK, wait for data interrupt. */
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
	}
}


/*! \brief TWI stop condition interrupt handler.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveStopHandler(TWI_Slave_t *twi)
{
	/* Disable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

	/* Clear APIF, according to flowchart don't ACK or NACK */
	uint8_t currentStatus = twi->interface->SLAVE.STATUS;
	twi->interface->SLAVE.STATUS = currentStatus | TWI_SLAVE_APIF_bm;

	TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);

}


/*! \brief TWI data interrupt handler.
 *
 *  Calls the appropriate slave read or write handler.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveDataHandler(TWI_Slave_t *twi)
{
	if (twi->interface->SLAVE.STATUS & TWI_SLAVE_DIR_bm) {
		TWI_SlaveWriteHandler(twi);
	} else {
		TWI_SlaveReadHandler(twi);
	}
}


/*! \brief TWI slave read interrupt handler.
 *
 *  Handles TWI slave read transactions and responses.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveReadHandler(TWI_Slave_t *twi)
{
	/* Enable stop interrupt. */
	uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
	twi->interface->SLAVE.CTRLA = currentCtrlA | TWI_SLAVE_PIEN_bm;

	/* If free space in buffer. */
	if (twi->bytesReceived < TWIS_RECEIVE_BUFFER_SIZE) {
		/* Fetch data */
		uint8_t data = twi->interface->SLAVE.DATA;
		twi->receivedData[twi->bytesReceived] = data;

		/* Process data. */
		twi->Process_Data();

		twi->bytesReceived++;

		/* If application signalling need to abort (error occured),
		 * complete transaction and wait for next START. Otherwise
		 * send ACK and wait for data interrupt.
		 */
		if (twi->abort) {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
			TWI_SlaveTransactionFinished(twi, TWIS_RESULT_ABORTED);
			twi->abort = false;
		} else {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
		}
	}
	/* If buffer overflow, send NACK and wait for next START. Set
	 * result buffer overflow.
	 */
	else {
		twi->interface->SLAVE.CTRLB = TWI_SLAVE_ACKACT_bm |
		                              TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
	}
}


/*! \brief TWI slave write interrupt handler.
 *
 *  Handles TWI slave write transactions and responses.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
void TWI_SlaveWriteHandler(TWI_Slave_t *twi)
{
	/* If NACK, slave write transaction finished. */
	if ((twi->bytesSent > 0) && (twi->interface->SLAVE.STATUS &
	                             TWI_SLAVE_RXACK_bm)) {

		twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
		TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);
	}
	/* If ACK, master expects more data. */
	else {
		if (twi->bytesSent < TWIS_SEND_BUFFER_SIZE) {
			uint8_t data = twi->sendData[twi->bytesSent];
			twi->interface->SLAVE.DATA = data;
			twi->bytesSent++;

			/* Send data, wait for data interrupt. */
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_RESPONSE_gc;
		}
		/* If buffer overflow. */
		else {
			twi->interface->SLAVE.CTRLB = TWI_SLAVE_CMD_COMPTRANS_gc;
			TWI_SlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
		}
	}
}


/*! \brief TWI transaction finished function.
 *
 *  Prepares module for new transaction.
 *
 *  \param twi    The TWI_Slave_t struct instance.
 *  \param result The result of the transaction.
 */
void TWI_SlaveTransactionFinished(TWI_Slave_t *twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIS_STATUS_READY;
}
