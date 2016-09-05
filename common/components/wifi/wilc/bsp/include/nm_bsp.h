/**
 *
 * \file
 *
 * \brief This module contains NMC1500 BSP APIs declarations.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
/**@defgroup  BSPDefine Defines
 * @ingroup nm_bsp
 * @{
 */
#ifndef _NM_BSP_H_
#define _NM_BSP_H_

#define NMI_API
/*!< 
*        Attribute used to define memory section to map Functions in host memory.
*/

#ifdef __MCF964548__
#define CONST code
#else
#define CONST const
#endif
/*!< 
*     Used for code portability.
*/

/*!
 * @typedef      void (*tpfNmBspIsr) (void);
 * @brief           Pointer to function.
 *                     Used as a data type of ISR function registered by \ref nm_bsp_register_isr
 * @return         None
 */
typedef void (*tpfNmBspIsr)(void);



#ifndef NULL
#define NULL ((void*)0)
#endif
/*!< 
*    Void Pointer to '0' in case of NULL is not defined. 
*/


#define BSP_MIN(x,y) ((x)>(y)?(y):(x))
/*!< 
*     Computes the minimum of \b x and \b y.
*/

 //@}

/**@defgroup  DataT  DataTypes
 * @ingroup nm_bsp
 * @{
 */
 
  /*!
 * @ingroup DataTypes
 * @typedef      unsigned char	uint8;
 * @brief        Range of values between 0 to 255
 */
typedef unsigned char	uint8;

 /*!
 * @ingroup DataTypes
 * @typedef      unsigned short	uint16;
 * @brief        Range of values between 0 to 65535
 */
typedef unsigned short	uint16;

 /*!
 * @ingroup Data Types
 * @typedef      unsigned long	uint32;
 * @brief        Range of values between 0 to 4294967295
 */ 
typedef unsigned long	uint32;


  /*!
 * @ingroup Data Types
 * @typedef      signed char		sint8;
 * @brief        Range of values between -128 to 127
 */
typedef signed char		sint8;

 /*!
 * @ingroup DataTypes
 * @typedef      signed short	sint16;
 * @brief        Range of values between -32768 to 32767
 */
typedef signed short	sint16;

  /*!
 * @ingroup DataTypes
 * @typedef      signed long		sint32;
 * @brief        Range of values between -2147483648 to 2147483647
 */

typedef signed long		sint32;
 //@}

#ifndef CORTUS_APP


#ifdef __cplusplus
extern "C"{
#endif

/** \defgroup BSPAPI Function
 *   @ingroup nm_bsp
 */


/** @defgroup NmBspInitFn nm_bsp_init
 *  @ingroup BSPAPI
 *   Initialize BSP
 */
 /**@{*/
/*!
 * @fn           sint8 nm_bsp_init(void);
 * @brief        Initialization for bsp (\e Board \e Support \e Package) such as Reset and Chip Enable Pins for WINC, delays, register ISR, enable/disable IRQ for WINC, ...etc.
 *                   You must use this function in the head of your application to enable WINC and Host Driver communicate each other.
 * @warning      Missing use will lead to unavailability of host communication.
 *  @note        Implementation of this function is host dependent.
 * @return       The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

 */
sint8 nm_bsp_init(void);
 /**@}*/

 
 /** @defgroup NmBspDeinitFn nm_bsp_deinit
 *    @ingroup BSPAPI
 *   	 De-initialization for bsp (\e Board \e Support \e Package)
 */
 /**@{*/
/*!
 * @fn           sint8 nm_bsp_deinit(void);
 * @pre          Initialize \ref nm_bsp_init first
 * @warning      Missing use may lead to unknown behavior in case of soft reset.
 * @note         Implementation of this function is host dependent.
 * @see          nm_bsp_init               
 * @return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

 */
sint8 nm_bsp_deinit(void);
 /**@}*/

 
/** @defgroup NmBspResetFn  nm_bsp_reset
*     @ingroup BSPAPI
*      Reinitialization for bsp (\e Board \e Support \e Package) by resetting NMC1500 SoC by setting \ref CHIP_EN and \ref RESET_N signals low, 
 *     \ref CHIP_EN high then \ref RESET_N high
*/
/**@{*/
 /*!
 * @fn           void nm_bsp_reset(void);
 * @brief     
 * @param [in]   None
 * @pre          Initialize \ref nm_bsp_init first
 * @note         Implementation of this function is host dependent and called by HIF layer.
 * @see          nm_bsp_init    
 * @return       None

 */
void nm_bsp_reset(void);
 /**@}*/

 
/** @defgroup NmBspSleepFn nm_bsp_sleep
*     @ingroup BSPAPI
*     Sleep in units of milliseconds.
*    This function used by HIF Layer according to different situations. 
*/
/**@{*/
/*!
 * @fn           void nm_bsp_sleep(uint32);
 * @brief   
 * @param [in]   u32TimeMsec
 *               Time unit in milliseconds
 * @pre          Initialize \ref nm_bsp_init first
 * @warning      Maximum value must nor exceed 4294967295 milliseconds which is equal to 4294967.295 seconds.
 * @note         Implementation of this function is host dependent.
 * @see           nm_bsp_init               
 * @return       None
 */
void nm_bsp_sleep(uint32 u32TimeMsec);
/**@}*/

  
/** @defgroup NmBspRegisterFn nm_bsp_register_isr
*     @ingroup BSPAPI
*   Register ISR (\e Interrupt \e Service \e Routine) in the initialization of HIF (\e Host \e Interface) Layer.
*/
/**@{*/
/*!
 * @fn           void nm_bsp_register_isr(tpfNmBspIsr);
 * @param [in]   pfIsr
 *               Pointer to ISR handler in HIF
 * @warning      Make sure that ISR for IRQ pin for WINC is disabled by default in your implementation.
 * @note         Implementation of this function is host dependent and called by HIF layer.
  * @see        tpfNmBspIsr
 * @return       None

 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr);
/**@}*/

  
/** @defgroup NmBspInterruptCtrl nm_bsp_interrupt_ctrl
*     @ingroup BSPAPI
*    Synchrnonous enable/disable interrupts fucntion
*/
/**@{*/
/*!
 * @fn           void nm_bsp_interrupt_ctrl(uint8);
 * @brief        Enable/Disable interrupts
 * @param [in]   u8Enable
 *               '0' disable interrupts. '1' enable interrupts 
 * @see          tpfNmBspIsr           
 * @note         Implementation of this function is host dependent and called by HIF layer.
 * @return       None

 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable);
  /**@}*/

#ifdef __cplusplus
}
#endif

#endif

#ifdef WIN32
#include "nm_bsp_win32.h"
#endif

#ifdef __K20D50M__
#include "nm_bsp_k20d50m.h"
#endif

#ifdef __MSP430FR5739__
#include "bsp_msp430fr5739.h"
#endif

#ifdef _FREESCALE_MCF51CN128_
#include "bsp/include/nm_bsp_mcf51cn128.h"
#endif

#ifdef __MCF964548__
#include "bsp/include/nm_bsp_mc96f4548.h"
#endif

#ifdef __APP_APS3_CORTUS__
#include "nm_bsp_aps3_cortus.h"
#endif

#if (defined __SAMD21J18A__) || (defined __SAMD21G18A__)
#include "bsp/include/nm_bsp_samd21.h"
#endif

#if (defined __SAM4S16C__) || (defined __SAM4SD32C__)
#include "bsp/include/nm_bsp_sam4s.h"
#endif

#ifdef __SAMG53N19__
#include "bsp/include/nm_bsp_samg53.h"
#endif

#ifdef __SAMG55J19__
#include "bsp/include/nm_bsp_samg55.h"
#endif

#ifdef __SAMV71Q21__
#include "bsp/include/nm_bsp_samv71.h"
#endif

#ifdef CORTUS_APP
#include "crt_iface.h"
#endif

#ifdef NRF51 
#include "nm_bsp_nrf51822.h"
#endif

#ifdef _ARDUINO_UNO_
#include <bsp/include/nm_bsp_arduino_uno.h>
#endif


#ifdef _NM_BSP_BIG_END
#define NM_BSP_B_L_32(x) \
((((x) & 0x000000FF) << 24) + \
(((x) & 0x0000FF00) << 8)  + \
(((x) & 0x00FF0000) >> 8)   + \
(((x) & 0xFF000000) >> 24))
#define NM_BSP_B_L_16(x) \
((((x) & 0x00FF) << 8) + \
(((x)  & 0xFF00) >> 8))
#else
#define NM_BSP_B_L_32(x)  (x)
#define NM_BSP_B_L_16(x)  (x)
#endif


#endif	/*_NM_BSP_H_*/
