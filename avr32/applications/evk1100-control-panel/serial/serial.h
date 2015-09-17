/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel USART driver module.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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

