/******************************************************************************* 
*   $FILE:  QdebugSetting.h
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/
/*  License
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
*   
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*   
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*   
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*   
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*   
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//---------- Do not edit --------------------
// Project Constants
// Values from 0xF000->0xFFFF are reserved for Atmel Kits
// values from 0x0000->0xEFFF are available for other projects
#define 	QT8				    0xF001
#define 	QT16				0xF002
#define 	QM64				0xF003
#define     DUMMY               0x1111

// Interface constans
#define 	TWI				    1
#define 	SPI1W				2
#define 	SPI2W				3
#define 	UART				4
#define     BitBangSPI          5

//---------- Edit Interface conf in touch_config.h -------------
#if (DEF_QTOUCH_INTERFACE_TYPE == 0)
#define	QDEBUG_SPI				/* If using on-chip SPI peripheral*/
#elif (DEF_QTOUCH_INTERFACE_TYPE == 1)
#define QDEBUG_TWI				/* If using on-chip TWI peripheral */
#elif (DEF_QTOUCH_INTERFACE_TYPE == 2)
#define QDEBUG_SPI_BB		/* If using Bit Bang SPI */
#else
#error 'Debug interface options should be 0 to 2'
#endif

//---------- Edit Project Info -------------

#if defined(QDEBUG_SPI)
#define		INTERFACE           SPI2W
#elif defined(QDEBUG_TWI)
#define		INTERFACE           TWI
#elif defined(QDEBUG_SPI_BB)
#define     INTERFACE           BitBangSPI
#endif


