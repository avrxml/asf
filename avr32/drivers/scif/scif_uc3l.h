/*****************************************************************************
 *
 * \file
 *
 * \brief System Control InterFace(SCIF) driver interface.
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

#ifndef _SCIF_UC3L_H_
#define _SCIF_UC3L_H_

/**
 * \defgroup group_avr32_drivers_scif CPU - SCIF - System Control Interface
 *
 * Driver for the SCIF (System Control Interface). The SCIF controls the oscillators,
 * Generic Clocks, BODs, VREG, Temperature Sensor, and Backup Registers.
 *
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#if defined(AVR32_SCIF_100_H_INCLUDED) || defined(AVR32_SCIF_102_H_INCLUDED)
// Optional #undef AVR32_SCIF_GC_NO_DIV_CLOCK if the define values is wrong.
#define AVR32_SCIF_GC_NO_DIV_CLOCK       AVR32_GC_NO_DIV_CLOCK
#endif

#define AVR32_SCIF_OSCCTRL0_OSCEN_ENABLE      0x00000001  //! Enable value for OSCCTRL0.OSCEN
#define AVR32_SCIF_OSCCTRL0_OSCEN_DISABLE     0x00000000  //! Disable value for OSCCTRL0.OSCEN
#define AVR32_SCIF_OSCCTRL32_OSC32EN_ENABLE   0x00000001  //! Enable value for OSCCTRL32.OSC32EN
#define AVR32_SCIF_OSCCTRL32_OSC32EN_DISABLE  0x00000000  //! Disable value for OSCCTRL32.OSC32EN


//! Device-specific data

//!< External crystal/clock min frequency (in Herz)
#define SCIF_EXT_CRYSTAL_MIN_FREQ_HZ  4000000UL
//!< External crystal/clock max frequency (in Herz)
#define SCIF_EXT_CRYSTAL_MAX_FREQ_HZ  20000000UL

//! The different clock source for the generic clocks
typedef enum
{
#if UC3L0 || UC3L0128 || UC3L0256
  SCIF_GCCTRL_SLOWCLOCK  = AVR32_SCIF_GC_USES_CLK_SLOW,
  SCIF_GCCTRL_OSC32K  = AVR32_SCIF_GC_USES_CLK_32,
  SCIF_GCCTRL_DFLL0  = AVR32_SCIF_GC_USES_DFLL0,
  SCIF_GCCTRL_OSC0  = AVR32_SCIF_GC_USES_OSC0,
  SCIF_GCCTRL_RC120M  = AVR32_SCIF_GC_USES_RC120M,
  SCIF_GCCTRL_CPUCLOCK  = AVR32_SCIF_GC_USES_CLK_CPU,
  SCIF_GCCTRL_HSBCLOCK  = AVR32_SCIF_GC_USES_CLK_HSB,
  SCIF_GCCTRL_PBACLOCK  = AVR32_SCIF_GC_USES_CLK_PBA,
  SCIF_GCCTRL_PBBCLOCK  = AVR32_SCIF_GC_USES_CLK_PBB,
  SCIF_GCCTRL_RC32K  = AVR32_SCIF_GC_USES_RC32K,
  SCIF_GCCTRL_CLK1K  = AVR32_SCIF_GC_USES_CLK_1K,
#else
  SCIF_GCCTRL_SLOWCLOCK  = AVR32_SCIF_GCCTRL_OSCSEL_RCSYS,
  SCIF_GCCTRL_OSC32K  = AVR32_SCIF_GCCTRL_OSCSEL_OSC32K,
  SCIF_GCCTRL_DFLL0  = AVR32_SCIF_GCCTRL_OSCSEL_DFLL0,
  SCIF_GCCTRL_OSC0  = AVR32_SCIF_GCCTRL_OSCSEL_OSC0,
  SCIF_GCCTRL_RC120M  = AVR32_SCIF_GCCTRL_OSCSEL_RC120M,
  SCIF_GCCTRL_CPUCLOCK  = AVR32_SCIF_GCCTRL_OSCSEL_CLK_CPU,
  SCIF_GCCTRL_HSBCLOCK  = AVR32_SCIF_GCCTRL_OSCSEL_CLK_HSB,
  SCIF_GCCTRL_PBACLOCK  = AVR32_SCIF_GCCTRL_OSCSEL_CLK_PBA,
  SCIF_GCCTRL_PBBCLOCK  = AVR32_SCIF_GCCTRL_OSCSEL_CLK_PBB,
  SCIF_GCCTRL_RC32K  = AVR32_SCIF_GCCTRL_OSCSEL_RC32K,
  SCIF_GCCTRL_CLK1K  = AVR32_SCIF_GCCTRL_OSCSEL_CLK_1K,
#endif
#if (UC3L0128 || UC3L0256)
  SCIF_GCCTRL_PLL0  = AVR32_SCIF_GC_USES_PLL0,
#endif
#if (UC3L3_L4)
  SCIF_GCCTRL_PLL0  = AVR32_SCIF_GCCTRL_OSCSEL_PLL0,
#endif
  SCIF_GCCTRL_OSCSEL_INVALID
} scif_gcctrl_oscsel_t;

//! The min DFLL output frequency
#if (UC3L0128 || UC3L0256)
#define SCIF_DFLL_MINFREQ_KHZ         20000
#define SCIF_DFLL_MINFREQ_HZ          20000000UL
#else
#define SCIF_DFLL_MINFREQ_KHZ         40000
#define SCIF_DFLL_MINFREQ_HZ          40000000UL
#endif

//! The max DFLL output frequency
#define SCIF_DFLL_MAXFREQ_KHZ         150000
#define SCIF_DFLL_MAXFREQ_HZ          150000000UL

//! The RCSYS slow clock frequency
#define SCIF_SLOWCLOCK_FREQ_HZ        AVR32_SCIF_RCOSC_FREQUENCY
#define SCIF_SLOWCLOCK_FREQ_KHZ       (SCIF_SLOWCLOCK_FREQ_HZ/1000)

//! The RC32K slow clock frequency
#define SCIF_RC32K_FREQ_HZ            32768
#define SCIF_RC32K_FREQ_KHZ           (SCIF_RC32K_FREQ_HZ/1000)

//! The RC120M frequency
#define SCIF_RC120M_FREQ_HZ           120000000UL
#define SCIF_RC120M_FREQ_KHZ          120000

//! The OSC32 frequency
#define SCIF_OSC32_FREQ_HZ            32768

//! The timeguard used for polling in ticks.
#define SCIF_POLL_TIMEOUT             100000

//! Define "not supported" for the chosen implementation.
#define SCIF_NOT_SUPPORTED            (-10000)


//! The different oscillators
typedef enum
{
  SCIF_OSC0  = 0,
  SCIF_OSC1  = 1
} scif_osc_t;

//! The different oscillator modes
typedef enum
{
  SCIF_OSC_MODE_EXT_CLK = 0,            // For both OSC0 & OSC32
  SCIF_OSC_MODE_2PIN_CRYSTAL = 1,       // For both OSC0 and OSC32
  SCIF_OSC_MODE_NOT_SUPPORTED_1 = 2,
  SCIF_OSC_MODE_NOT_SUPPORTED_2 = 3,
  SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR = 4, // For OSC32 only
  SCIF_OSC_MODE_NOT_SUPPORTED_3 = 5,
  SCIF_OSC_MODE_NOT_SUPPORTED_4 = 6,
  SCIF_OSC_MODE_NOT_SUPPORTED_5 = 7
} scif_osc_mode_t;

//! OSC0/OSC1 startup options.
typedef struct
{
  //! The input frequency (from the external clock or the crystal).
  unsigned long   freq_hz;
  //! Set the oscillator mode.
  scif_osc_mode_t mode;
  //! Specify the oscillator startup time.
  unsigned char   startup;
  //! Gain for the oscillator
  unsigned char   gain;
} scif_osc_opt_t;

#if (UC3L0128 || UC3L0256 || UC3L3_L4)
//! PLL0 startup options.
typedef struct
{
  //!  Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
  unsigned char   pll_freq;
  //! 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
  unsigned char   pll_wbwdisable;
  //! Divide the PLL output frequency by 2. (this settings does not change the FVCO value)
  unsigned char   pll_div2;
  //! PLL MUL in the PLL formula
  unsigned int    mul;
  //! PLL DIV in the PLL formula
  unsigned int    div;
  //! PLL lockcount
  unsigned int    lockcount;
  //! Specify the oscillator input.
  unsigned char   osc;
} scif_pll_opt_t;
#endif

//! OSC32 startup options.
typedef struct
{
  //! Set the oscillator mode.
  scif_osc_mode_t mode;
  //! Specify the oscillator startup time.
  unsigned char   startup;
  //! Select pins used for the 32kHz Crystal
  bool            pinsel;
  //! 1kHz output enable/disable
  bool            en1k;
  //! 32kHz output enable/disable
  bool            en32k;
} scif_osc32_opt_t;


//! Generic clock generation settings.
typedef struct
{
  //! The input clock source to use for the generic clock
  scif_gcctrl_oscsel_t clock_source;
  //! The division factor to apply to the clock src.
  unsigned int divider;
  //! Enable/disable the generic clock divisor
  unsigned int diven;
  //! Oscillator external crystal(or external clock) frequency (board dependant)
  // (to fill-in if the clock_source is set to OSCn).
  unsigned long extosc_f;
} scif_gclk_opt_t;


//! DFLL open-loop mode settings.
typedef struct
{
  unsigned int  fine;
  unsigned int  coarse;
} scif_dfll_openloop_conf_t;

//! DFLL closed-loop mode settings.
typedef struct
{
  //! The coarse value (= (fDFLL - SCIF_DFLL_MINFREQ_KHZ)*255/(SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ))
  unsigned int    coarse;
  //! The fmul value (= (fDFLL*2^16)/fref) - imul*2^16)
  unsigned int    fmul;
  //! The imul value (= (fDFLL)/fref)
  unsigned int    imul;
  //! Fine maximum step size during fine adjustment
  unsigned int    finemaxstep;
  //! Coarse maximum step size during coarse adjustment
  unsigned int    coarsemaxstep;
} scif_dfll_closedloop_conf_t;

//! DFLL SSG settings.
typedef struct
{
  //! Enable/disable the pseudo random
  unsigned int    use_random;
  //! SSG amplitude
  unsigned int    amplitude;
  //! SSG step size
  unsigned int    step_size;
} scif_dfll_ssg_conf_t;


//! Unlock SCIF register macro
#define SCIF_UNLOCK(reg)  (AVR32_SCIF.unlock = (AVR32_SCIF_UNLOCK_KEY_VALUE << AVR32_SCIF_UNLOCK_KEY_OFFSET)|(reg))

/*! \name Interrupt Functions
 */
//! @{

/*! \brief Enable SCIF interrupts.
 *
 * \param mask the interrupts to enable.
 */
__always_inline static void scif_enable_interrupts(unsigned long mask)
{
  AVR32_SCIF.ier = mask;
}

/*! \brief Disable SCIF interrupts.
 *
 * \param mask the interrupts to disable.
 */
__always_inline static void scif_disable_interrupts(unsigned long mask)
{
  AVR32_SCIF.idr = mask;
}

/*! \brief Read the SCIF currently enabled interrupts.
 *
 * \return The enabled interrupts.
 */
__always_inline static unsigned long scif_get_enabled_interrupts(void)
{
  return(AVR32_SCIF.imr);
}

/*! \brief Read the interrupt status of the SCIF.
 *
 * \return The interrupts which have been triggered.
 */
__always_inline static unsigned long scif_get_interrupts_status(void)
{
  return(AVR32_SCIF.isr);
}

/*! \brief Clear raised interrupts from the SCIF.
 *
 * \param mask The interrupts to clear.
 */
__always_inline static void scif_clear_interrupts_status(unsigned long mask)
{
  AVR32_SCIF.icr = mask;
}

//! @}


/*! \name Power and Clocks Status Functions
 */
//! @{

/*! \brief Read the Power and Clocks Status of the SCIF.
 *
 * \return The content of the PCLKSR register
 */
__always_inline static unsigned long scif_get_pclk_status(void)
{
  return(AVR32_SCIF.pclksr);
}

//! @}


/*! \name OSC0/OSC1 Functions
 */
//! @{

/*! \brief Configure and start an OSC0/OSC1 oscillator.
 *
 * \param osc The oscillator to start [INPUT]
 * \param opt The configuration of the oscillator [INPUT]
 * \param wait_for_ready Wait for the oscillator to be stable before return [INPUT]
 *
 * \note To avoid an infinite loop, this function checks the osc0 ready flag
 *  SCIF_POLL_TIMEOUT times.
 *
 * \return Status.
 *   \retval 0   Oscillator start successful.
 *   \retval <0  Error starting the oscillator.
 */
extern long int scif_start_osc(scif_osc_t osc, const scif_osc_opt_t *opt, bool wait_for_ready);

/*! \brief Is an oscillator stable and ready to be used as clock source?
 *
 * \param osc The oscillator [INPUT]
 *
 * \return Status.
 *   \retval true oscillator stable and ready
 *   \retval false oscillator not enabled or not ready.
 */
extern bool scif_is_osc_ready(scif_osc_t osc);

/*! \brief Stop an oscillator.
 *
 * \param osc The oscillator to stop
 *
 * \return Status.
 *   \retval 0 Oscillator successfully stopped.
 *   \retval <0 An error occurred when stopping the oscillator.
 */
extern long int scif_stop_osc(scif_osc_t osc);

/*! \brief Configure an oscillator in crystal mode
 *
 * \param osc The oscillator to configure [INPUT]
 * \param fcrystal Crystal frequency (Hz) [INPUT]
 *
 * \return Status.
 *   \retval 0   Oscillator successfully configured.
 *   \retval <0  Error configuring the oscillator.
 */
extern long int scif_configure_osc_crystalmode(scif_osc_t osc, unsigned int fcrystal);

/*! \brief Configure an external clock as input clock
 *
 * \param osc The external clock to configure [INPUT]
 *
 * \return Status.
 *   \retval 0   External clock successfully configured.
 *   \retval <0  Error configuring the external clock.
 */
extern long int scif_configure_osc_extmode(scif_osc_t osc);

/*! \brief Enable an oscillator with a given startup time.
 *
 * \param osc The oscillator to configure [INPUT]
 * \param startup Oscillator startup time (one of AVR32_SCIF_OSCCTRLx_STARTUP_x_RCOSC) [INPUT]
 * \param wait_for_ready Wait for the oscillator to be stable before return [INPUT]
 *
 * \return Status.
 *   \retval 0   Oscillator successfully started
 *   \retval <0  Error starting the oscillator.
 */
extern long int scif_enable_osc(scif_osc_t osc, unsigned int startup, bool wait_for_ready);

#if (UC3L0128 || UC3L0256 || UC3L3_L4)
//! @}


/*! \name PLL0 Functions
 */
//! @{

/*!
 * \brief This function will setup PLL0.
 * \param opt The configuration of the PLL [INPUT]
 * \return Status.
 *   \retval 0   PLL successfully configured
 *   \retval <0  Error.
 */
extern long int scif_pll0_setup(const scif_pll_opt_t *opt);

/*!
 * \brief This function will enable PLL0.
 * \return Status.
 *   \retval 0   PLL successfully enabled
 *   \retval <0  Error starting the PLL.
 */
extern long int scif_pll0_enable(void);

/*!
 * \brief This function will disable PLL0.
 * \return Status.
 *   \retval 0   PLL successfully disabled
 *   \retval <0  Error disabling the PLL.
 */
extern long int scif_pll0_disable(void);


/*!
 * \brief This function will wait until the PLL0 is locked
 * \return Status.
 *   \retval 0   OK
 *   \retval <0  Error
 */
extern long int scif_wait_for_pll0_locked(void);

//! @}

#endif

/*! \name OSC32 Functions
 */
//! @{

/*! \brief Configure and start the OSC32 oscillator.
 *
 * \param opt The configuration of the oscillator [INPUT]
 * \param wait_for_ready Wait for the oscillator to be stable before return [INPUT]
 *
 * \note To avoid an infinite loop, this function checks the osc32 ready flag
 *       SCIF_POLL_TIMEOUT times.
 *
 * \return Status.
 *   \retval 0   Oscillator start successful.
 *   \retval <0  Error starting the oscillator.
 */
extern long int scif_start_osc32(const scif_osc32_opt_t *opt, bool wait_for_ready);

/*! \brief Is OSC32 stable and ready to be used as clock source?
 *
 * \return Status.
 *   \retval true oscillator stable and ready
 *   \retval false oscillator not enabled or not ready.
 */
__always_inline static bool scif_is_osc32_ready(void)
{
  return((AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_OSC32RDY_MASK)>>AVR32_SCIF_PCLKSR_OSC32RDY_OFFSET);
}

/*! \brief Enable the 1kHz output of the OSC32 oscillator
 *
 */
__always_inline static void scif_osc32_1kout_ena(void)
{
  AVR32_SCIF.oscctrl32 |= AVR32_SCIF_EN1K_MASK;
}

/*! \brief Disable the 1kHz output of the OSC32 oscillator
 *
 */
__always_inline static void scif_osc32_1kout_dis(void)
{
  AVR32_SCIF.oscctrl32 &= ~AVR32_SCIF_EN1K_MASK;
}

/*! \brief Enable the 32kHz output of the OSC32 oscillator
 *
 */
__always_inline static void scif_osc32_32kout_ena(void)
{
  AVR32_SCIF.oscctrl32 |= AVR32_SCIF_EN32K_MASK;
}

/*! \brief Disable the 32kHz output of the OSC32 oscillator
 *
 */
__always_inline static void scif_osc32_32kout_dis(void)
{
  AVR32_SCIF.oscctrl32 &= ~AVR32_SCIF_EN32K_MASK;
}

/*! \brief Stop the OSC32 oscillator.
 *
 * \return Status.
 *   \retval 0 Oscillator successfully stopped.
 *   \retval <0 An error occurred when stopping the oscillator.
 */
extern long int scif_stop_osc32(void);

//! @}


/*! \name DFLL Control Functions
 */
//! @{

/*! \brief Configure and start the DFLL0 in open loop mode.
 *
 * \param pdfllconfig The DFLL parameters in open loop mode [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 configured and started successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_openloop_start(const scif_dfll_openloop_conf_t *pdfllconfig);

/*! \brief Automatic configuration and start of the DFLL0 in open loop mode
 *
 * \param TargetFreqkHz The DFLL target frequency (in kHz) [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 configured and started successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_openloop_start_auto(unsigned long TargetFreqkHz);

/*! \brief Update the frequency of the DFLL0 in open loop mode.
 *
 * \param pdfllconfig The DFLL parameters in open loop mode [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 frequency updated successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_openloop_updatefreq(const scif_dfll_openloop_conf_t *pdfllconfig);

/*! \brief Automatic configuration to update the frequency of the DFLL0 in open loop mode.
 *
 * \param TargetFreq The DFLL target frequency (in kHz) [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 frequency updated successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_openloop_updatefreq_auto(unsigned long TargetFreq);

/*! \brief Stop the DFLL0 in open loop mode.
 *
 * \return Status.
 *   \retval 0   DFLL0 successfully stopped.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_openloop_stop(void);

/*! \brief Configure and enable the SSG reference generic clock
 *
 * \note The frequency of the SSG reference clock should be higher than the CLK_DFLLIF_REF
 *       to ensure that the DFLLIF can lock.
 *
 * \param pgc_conf The settings for the generic clock [INPUT]
 *
 * \return Status.
 *   \retval 0   SSG Generic clock configured and started successfully.
 *   \retval <0  Error.
 */
#if UC3L0 || UC3L0128 || UC3L0256
#define scif_dfll0_ssg_gc_enable(pgc_conf) scif_start_gclk(AVR32_SCIF_GCLK_DFLL0_SSG, pgc_conf)
#else
#define scif_dfll0_ssg_gc_enable(pgc_conf) scif_start_gclk(AVR32_SCIF_GCLK_DFLLIF_SSG, pgc_conf)
#endif

/*! \brief Configure and enable the SSG
 *
 * \note The SSG reference generic clock must have previously been enabled.
 *
 * \param pssg_conf The settings for the SCIF.DFLL0SSG register [INPUT]
 *
 * \return Status.
 *   \retval 0   SSG Generic clock configured and started successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_ssg_enable(scif_dfll_ssg_conf_t *pssg_conf);

/*! \brief Configure and enable the closed-loop mode main reference generic clock
 *
 * \param pgc_conf The settings for the generic clock [INPUT]
 *
 * \return Status.
 *   \retval 0   Main reference generic clock configured and started successfully.
 *   \retval <0  Error.
 */
#if UC3L0 || UC3L0128 || UC3L0256
#define scif_dfll0_closedloop_mainref_gc_enable(pgc_conf) scif_start_gclk(AVR32_SCIF_GCLK_DFLL0_REF, pgc_conf)
#else
#define scif_dfll0_closedloop_mainref_gc_enable(pgc_conf) scif_start_gclk(AVR32_SCIF_GCLK_DFLLIF_REF, pgc_conf)
#endif

/*! \brief Configure and start the DFLL0 in closed loop mode.
 *
 * \note The main reference generic clock must have previously been started.
 *
 * \param pdfllconfig The DFLL parameters in closed loop mode [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 configured and started successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_closedloop_start(const scif_dfll_closedloop_conf_t *pdfllconfig);

/*! \brief Depending on the target frequency, compute the DFLL configuration parameters
 *         and start the DFLL0 in closed loop mode.
 *
 * \note Configures and enables the generic clock CLK_DFLLIF_REF to serve as the main reference.
 *
 * \note This function only supports the following source clocks for the CLK_DFLLIF_REF generic clock:
 *       SCIF_GCCTRL_SLOWCLOCK (aka RCSYS), SCIF_GCCTRL_OSC32K, SCIF_GCCTRL_RC32K,
 *       SCIF_GCCTRL_OSC0, SCIF_GCCTRL_RC120M, SCIF_GCCTRL_CLK1K.
 *
 * \param gc_dfllif_ref_opt The settings for the CLK_DFLLIF_REF generic clock [INPUT]
 * \param target_freq_hz The target frequency (in Hz) [INPUT]
 * \param enable_ssg Enable/disable the SSG feature [INPUT]
 *
 * \return Status.
 *   \retval 0   DFLL0 configured and started successfully.
 *   \retval <0  Error.
 */
extern long int scif_dfll0_closedloop_configure_and_start( const scif_gclk_opt_t *gc_dfllif_ref_opt,
                                                    unsigned long long target_freq_hz,
                                                    bool enable_ssg);

/*! \brief Configure and enable the generic clock used by the closed-loop mode dithering stage and by the SSG.
 *
 * \param pgc_conf The settings for the generic clock [INPUT]
 *
 * \return Status.
 *   \retval 0   Dithering & SSG reference generic clock configured and started successfully.
 *   \retval <0  Error.
 */
#define scif_dfll0_closedloop_dither_gc_enable(pgc_conf)  scif_dfll0_ssg_gc_enable(pgc_conf)



//! @}


/*! \name Calibration Functions
 */
//! @{


//! @}


/*! \name 120MHz RCosc Functions
 */
//! @{

/*! \brief Start the 120MHz internal RCosc (RC120M) clock
 */
extern void scif_start_rc120M(void);

/*! \brief Stop the 120MHz internal RCosc (RC120M) clock
 */
extern void scif_stop_rc120M(void);

//! @}


/*! \name 32kHz internal RCosc (RC32K) Functions
 */
//! @{

/*! \brief Start the 32kHz internal RCosc (RC32K) clock
 */
extern void scif_start_rc32k(void);

/*! \brief Stop the 32kHz internal RCosc (RC32K) clock
 */
extern void scif_stop_rc32k(void);

/*! \brief Un-force the RC32 signal from being output on the dedicated pin (PA20)
 */
extern void scif_disable_rc32out(void);

//! @}


/*! \name Generic Clock Functions
 */
//! @{

/*! \brief Setup and start a generic clock.
 *
 * \param gclk The generic clock number to setup and start (cf. datasheet)
 * \param opt The settings for the generic clock.
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 An error occurred.
 */
extern long int scif_start_gclk(unsigned int gclk, const scif_gclk_opt_t *opt);

/*! \brief Stop a generic clock.
 *
 * \param gclk The generic clock number to stop.
 *
 * \note To avoid an infinite loop, this function checks the Clock enable flag
 *       SCIF_POLL_TIMEOUT times.
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 Unable to stop generic clock.
 */
extern long int scif_stop_gclk(unsigned int gclk);

/*! \brief Setup a generic clock.
 *
 * \param gclk generic clock number (0 for gc0...)
 * \param clk_src The input clock source to use for the generic clock
 * \param diven Generic clock divisor enable
 * \param divfactor Generic clock divisor
 *
 * \note If the generic clock is already enabled, this function will disable it,
 *       apply the configuration then restart the generic clock.
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 An error occurred.
 */
extern long int scif_gc_setup(unsigned int gclk, scif_gcctrl_oscsel_t clk_src, unsigned int diven, unsigned int divfactor);

/*! \brief Enable a generic clock.
 *
 * \param gclk generic clock number (0 for gc0...)
 *
 * \return Status.
 *   \retval 0 Success.
 *   \retval <0 An error occurred.
 */
extern long int scif_gc_enable(unsigned int gclk);

//! @}


/*! \name Backup Registers Functions
 */
//! @{


//! @}


/*! \name Miscellaneous Functions
 */
//! @{

/*! \brief Wait for a status high in the Power and Clocks status register.
 *
 * \param statusMask Mask field of the status to poll [INPUT]
 *
 * \return Status.
 *   \retval 0   Status is high.
 *   \retval <0  SCIF_POLL_TIMEOUT Timeout expired before the status was high.
 */
long int scif_pclksr_statushigh_wait(unsigned long statusMask);

//! @}

#ifdef __cplusplus
}
#endif

/**
 * \}
 */

#endif  // _SCIF_UC3L_H_
