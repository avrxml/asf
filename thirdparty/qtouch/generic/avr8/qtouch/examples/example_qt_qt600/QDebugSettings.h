/**
 * \file
 *
 * \brief QTouch Debug Setting
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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

