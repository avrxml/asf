/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel USART driver module.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

typedef void*  xComPortHandle;

typedef enum
{
	serCOM1,
	serCOM2,
	serCOM3,
	serCOM4,
	serCOM5,
	serCOM6,
	serCOM7,
	serCOM8
} eCOMPort;

typedef enum
{
	serNO_PARITY,
	serODD_PARITY,
	serEVEN_PARITY,
	serMARK_PARITY,
	serSPACE_PARITY
} eParity;

typedef enum
{
	serSTOP_1,
	serSTOP_2
} eStopBits;

typedef enum
{
	serBITS_5,
	serBITS_6,
	serBITS_7,
	serBITS_8
} eDataBits;

typedef enum
{
	ser50,
	ser75,
	ser110,
	ser134,
	ser150,
	ser200,
	ser300,
	ser600,
	ser1200,
	ser1800,
	ser2400,
	ser4800,
	ser9600,
	ser19200,
	ser38400,
	ser57600,
	ser115200
} eBaud;


/*!
 * \brief Init the serial port.
 * \param UsartId The identifier of the Usart to init.
 * \param ulWantedBaud The required baudrate.
 * \param uxRxQueueLength The length of the Rx buffer (if 0, rx is not supported).
 * \param uxTxQueueLength The length of the Tx buffer (if 0, tx is not supported).
 * \return xComPortHandle Handler on the COM port.
 */
xComPortHandle xUsartInit( eCOMPort UsartId, unsigned portLONG ulWantedBaud,
                           unsigned portBASE_TYPE uxRxQueueLength,
                           unsigned portBASE_TYPE uxTxQueueLength);


/*!
 * \brief Get char from Usart.
 * \param pxPort The Usart handle to get the char from
 * \param pcRxedChar The rxed char(output)
 * \param xBlockTime The max time to wait for a rxed char
 */
signed portBASE_TYPE xUsartGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime );


/*!
 * \brief Put char to Usart.
 * \param pxPort The Usart handle to put the char to
 * \param cOutChar The char to transmit
 * \param xBlockTime The max time to wait for getting the right to transmit the char.
 */
signed portBASE_TYPE xUsartPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );


/*!
 * \brief Put a string to Usart.
 * \param pxPort The Usart handle to put the string to
 * \param pcString The string to transmit
 * \param usStringLength The number of char to transmit.
 */
unsigned portSHORT usUsartPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength );

/*!
 * \brief Close the serial port
 * \param xPort The handle of the usart to close
 */
void vSerialClose( xComPortHandle xPort );

/*
 * USART0 interrupt service routine.
 */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vUSART0_ISR( void );

#endif

