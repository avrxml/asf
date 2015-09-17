/**
 * \file
 *
 * \brief Power Manager(PM) driver interface.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _PM_UC3D_H_
#define _PM_UC3D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <avr32/io.h>
#include "compiler.h"


//! Device-specific data
//! The clock sources of the power manager.
typedef enum
{
  PM_CLK_SRC_SLOW = AVR32_PM_MCSEL_SLOW,
  PM_CLK_SRC_OSC0 = AVR32_PM_MCSEL_OSC0,
  PM_CLK_SRC_RC120M = 4, //AVR32_PM_MCSEL_RC120M,
  PM_CLK_SRC_PLL0 = 2, //AVR32_PM_MCSEL_PLL0,
  PM_CLK_SRC_PLL1 = 3, // AVR32_PM_MCSEL_PLL1,
  PM_CLK_SRC_RCRIPOSC = AVR32_PM_MCSEL_CRIPOSC,
  PM_CLK_SRC_INVALID
} pm_clk_src_t;

//! The clock domains of the power manager.
typedef enum
{
  PM_CLK_DOMAIN_0 = AVR32_PM_CLK_GRP_CPU,
  PM_CLK_DOMAIN_1 = AVR32_PM_CLK_GRP_HSB,
  PM_CLK_DOMAIN_2 = AVR32_PM_CLK_GRP_PBA,
  PM_CLK_DOMAIN_3 = AVR32_PM_CLK_GRP_PBB,
  PM_CLK_DOMAIN_INVALID
} pm_clk_domain_t;


//! The possible synchronous clock division ratio.
typedef enum
{
  PM_CKSEL_DIVRATIO_2 = 0,  // Divide the main clock by 2^1
  PM_CKSEL_DIVRATIO_4,      // Divide the main clock by 2^2
  PM_CKSEL_DIVRATIO_8,      // Divide the main clock by 2^3
  PM_CKSEL_DIVRATIO_16,     // Divide the main clock by 2^4
  PM_CKSEL_DIVRATIO_32,     // Divide the main clock by 2^5
  PM_CKSEL_DIVRATIO_64,     // Divide the main clock by 2^6
  PM_CKSEL_DIVRATIO_128,    // Divide the main clock by 2^7
  PM_CKSEL_DIVRATIO_256,    // Divide the main clock by 2^8
  PM_CKSEL_DIVRATIO_ERROR
} pm_divratio_t;

//! The maximum synchronous clock division ratio for each clock domain.
typedef enum
{
  PM_CPUSEL_DIVRATIO_MAX = AVR32_PM_CPUSEL_CPUSEL_MASK >> AVR32_PM_CPUSEL_CPUSEL_OFFSET,
  PM_HSBSEL_DIVRATIO_MAX = AVR32_PM_HSBSEL_HSBSEL_MASK >> AVR32_PM_HSBSEL_HSBSEL_OFFSET,
  PM_PBASEL_DIVRATIO_MAX = AVR32_PM_PBASEL_PBSEL_MASK >> AVR32_PM_PBASEL_PBSEL_OFFSET,
  PM_PBBSEL_DIVRATIO_MAX = AVR32_PM_PBBSEL_PBSEL_MASK >> AVR32_PM_PBBSEL_PBSEL_OFFSET
} pm_divratio_max_t;

//! The timeguard used for polling (expressed in ticks).
#define PM_POLL_TIMEOUT 100000

//! Define "not supported" for the power manager features.
#define PM_NOT_SUPPORTED (-10000)


//! Unlock PM register macro
#define PM_UNLOCK(reg)  (AVR32_PM.unlock = (unsigned long)(AVR32_PM_UNLOCK_KEY_VALUE << AVR32_PM_UNLOCK_KEY_OFFSET)|(reg))


/*! \name Clock Functions
 */
//! @{

/*! \brief Set the main clock.
 *
 * \param src The clock to use as the main clock.
 *
 * \warning The input clock to use as the main clock must be enabled before calling
 *          this function, otherwise a deadlock will occur.
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 An error occurred when trying to set the main clock.
 */
extern long pm_set_mclk_source(pm_clk_src_t src);

/*! \brief Configure the main clock safety mechanisms.
 *
 * \note Refer to the section Clock Failure Detector of the PM chapter in the datasheet
 *
 * \warning The Critical Path Oscillator (CRIPOSC) must be enabled before
 *          enabling the Over Clock Protection mechanism.
 *
 * \param cfd Enable/disable the Clock Failure Detection mechanism
 * \param ocp Enable/disable the Over Clock Protection mechanism
 * \param final If true, make this configuration definitive
 *
 * \return Status.
 *   \retval =0 Success.
 *   \retval <0 An error occurred.
 */
extern long pm_config_mainclk_safety(bool cfd, bool ocp, bool final);

/*! \brief Set the division ratio for a clock domain.
 *
 * \param clock_domain The clock domain to alter.
 * \param divratio The division ratio to set.
 *
 * \warning Care should be taken that each new frequency of the synchronous clocks
 *          does not exceed the maximum frequency for each clock domain.
 *
 * \return Status.
 *   \retval =0 Success.
 *   \retval <0 An error occurred.
 */
extern long pm_set_clk_domain_div(pm_clk_domain_t clock_domain, pm_divratio_t divratio);

/*! \brief Disable the division ratio for a clock domain.
 *
 * \param clock_domain The clock domain to alter.
 *
 * \warning Care should be taken that each new frequency of the synchronous clocks
 *          does not exceed the maximum frequency for each clock domain.
 *
 * \return Status.
 *   \retval =0 Success.
 *   \retval <0 An error occurred.
 */
extern long pm_disable_clk_domain_div(pm_clk_domain_t clock_domain);

/*! \brief Enable or Disable the division ratio for each clock domain depending
 *         on the main source clock frequency and each clock domain target frequency.
 *
 * \param main_clock_f_hz The main source clock frequency
 * \param cpu_f_hz The target CPU clock domain frequency
 * \param pba_f_hz The target PBA clock domain frequency
 * \param pbb_f_hz The target PBB clock domain frequency
 *
 * \warning Care should be taken that each new frequency of the synchronous clocks
 *          does not exceed the maximum frequency for each clock domain.
 *
 */
extern void pm_set_all_cksel( unsigned long main_clock_f_hz, unsigned long cpu_f_hz,
                              unsigned long pba_f_hz, unsigned long pbb_f_hz );

/*! \brief Wait actively for the clock settings to be effective.
 *
 * \note To avoid an infinite loop, this function checks the clock ready flag
 *  PM_POLL_TIMEOUT times.
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 Unable to reach a clock ready status within the polling limit.
 */
extern long pm_wait_for_clk_ready(void);

//! @}


/*! \name Module Functions
 */
//! @{

/*! \brief Enable the clock of a module.
 *
 * \param module The module to clock (use one of the defines in the part-specific
 * header file under "toolchain folder"/avr32/inc(lude)/avr32/; depending on the
 * clock domain, look for the sections "CPU clocks", "HSB clocks", "PBx clocks")
 *
 * \return Status.
 *   \retval 0  Success.
 *   \retval <0 An error occurred.
 */
extern long pm_enable_module(unsigned long module);

/*! \brief Disable the clock of a module.
 *
 * \param module The module to shut down (use one of the defines in the part-specific
 * header file under "toolchain folder"/avr32/inc(lude)/avr32/; depending on the
 * clock domain, look for the sections "CPU clocks", "HSB clocks", "PBx clocks")
 *
 * \return Status.
 *   \retval 0  Success.
 *   \retval <0 An error occurred.
 */
extern long pm_disable_module(unsigned long module);


//! @}

/*! \name Sleep Functions
 */
//! @{

/*! \brief Sets the MCU in the specified sleep mode
 *
 * \note Precautions should be taken before entering sleep mode. Refer to the
 *  datasheet chapter 'Power Manager'.
 *
 * \param sleep_mode The sleep mode index
 */
#define SLEEP(sleep_mode)  {__asm__ __volatile__ ("sleep "STRINGZ(sleep_mode));}

/*! \brief Returns MCU wake cause
 *
 * \return The MCU wake cause which can be masked with the
 *         \c AVR32_PM_WCAUSE_x_MASK bit-masks to isolate specific causes.
 */
__always_inline static unsigned long pm_get_wake_cause(void)
{
  return AVR32_PM.wcause;
}

/*! \brief Enable one or several asynchronous wake-up source.
 *
 * \param awen_mask Mask of asynchronous wake-up sources (use one of the defines
 *  AVR32_PM_AWEN_xxxxWEN_MASK in the part-specific header file under
 *  "toolchain folder"/avr32/inc(lude)/avr32/)
 */
__always_inline static void pm_asyn_wake_up_enable(unsigned long awen_mask)
{
  AVR32_PM.awen |= awen_mask;
}

/*! \brief Disable one or several asynchronous wake-up sources
 *
 * \param awen_mask Mask of asynchronous wake-up sources (use one of the defines
 *  AVR32_PM_AWEN_xxxxWEN_MASK in the part-specific header file under
 *  "toolchain folder"/avr32/inc(lude)/avr32/)
 */
__always_inline static void pm_asyn_wake_up_disable(unsigned long awen_mask)
{
  AVR32_PM.awen &= ~awen_mask;
}

//! @}



/*! \name Reset Functions
 */
//! @{

/*! \brief Returns MCU last reset cause
 *
 * \return The MCU last reset cause which can be masked with the
 *         \c AVR32_PM_RCAUSE_x_MASK bit-masks to isolate specific causes.
 */
__always_inline static unsigned long pm_get_reset_cause(void)
{
  return AVR32_PM.rcause;
}

//! @}



/*! \name Interrupt Functions
 */
//! @{

/*! \brief Enable power manager interrupts.
 *
 * \param mask the interrupts to enable.
 *
 */
__always_inline static void pm_enable_interrupts(unsigned long mask)
{
  AVR32_PM.ier |= mask;
}

/*! \brief Disable power manager interrupts.
 *
 * \param mask the interrupts to disable.
 *
 */
__always_inline static void pm_disable_interrupts(unsigned long mask)
{
  AVR32_PM.idr |= mask;
}

/*! \brief Read the enabled power manager interrupts.
 *
 * \return mask of the enabled interrupts.
 */
__always_inline static unsigned long pm_get_enabled_interrupts(void)
{
  return(AVR32_PM.imr);
}

/*! \brief Read the interrupts status of the power manager.
 *
 * \return mask of the interrupts that have been triggered.
 */
__always_inline static unsigned long pm_get_interrupts_status(void)
{
  return(AVR32_PM.isr);
}

/*! \brief Clear raised interrupts from the power manager.
 *
 * \param mask The interrupts to clear.
 */
__always_inline static void pm_clear_interrupt_status(unsigned long mask)
{
  AVR32_PM.icr |= mask;
}

//! @}



/*! \name Misc Functions
 */
//! @{

/*! \brief Get the PM status
 *
 * \return The content of the PM Status register.
 */
__always_inline static unsigned long pm_get_status(void)
{
  return AVR32_PM.sr;
}

//! @}

#ifdef __cplusplus
}
#endif

#endif  // _PM_UC3D_H_
