/**
 * \file
 *
 * \brief QTouch Debug Setting
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \name Constants
 * --- Do not edit ---
 * @{
 */
/**
 * \name Project Constants
 * Values from 0xF000->0xFFFF are reserved for Atmel Kits.
 * Values from 0x0000->0xEFFF are available for other projects.
 * @{
 */
#define QT8     0xF001
#define QT16    0xF002
#define QM64    0xF003
#define TOUCH_KIT_ID_XMEGA_C3_XPLAINED 0xF00D
//! @}
/*! \name Interface constants.
 */
//! @{
#define TWI         1
#define SPI1W       2
#define SPI2W       3
#define UART        4
#define BitBangSPI  5
//! @}
//! @}

//---------- Edit Project Info -------------

/*! \name Select the type of interface to use for the debug protocol.
 * \brief Comment out the interface not used.
 * Only one interface should be active.
 * @{
 */
//#define QDEBUG_SPI
//#define QDEBUG_TWI
#define QDEBUG_SPI_BB

#if defined(QDEBUG_SPI)
#define INTERFACE           SPI2W
#elif defined(QDEBUG_TWI)
#define INTERFACE           TWI
#elif defined(QDEBUG_SPI_BB)
#define INTERFACE           BitBangSPI
#endif
//! @}

/*! \name Set up project info.
 * @{
 */
#if BOARD == ATXMEGA128A1_QT600
# define PROJECT_ID          QT16
#elif BOARD == XMEGA_C3_XPLAINED
# define PROJECT_ID          TOUCH_KIT_ID_XMEGA_C3_XPLAINED
#endif
//! @}

