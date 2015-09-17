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

#ifndef _SCIF_UC3C_H_
#define _SCIF_UC3C_H_

/**
 * \defgroup group_avr32_drivers_scif CPU - SCIF - System Control Interface
 *
 * Driver for the SCIF (System Control Interface).
 * The SCIF controls the Oscillators, PLL, Generic Clocks, BODs, the voltage regulators and general purpose
 * low power registers.
 *
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"

#ifndef AVR32_SCIF_OSCCTRL1
#define AVR32_SCIF_OSCCTRL1                                0x00000028
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif  //end AVR32_SCIF_OSCCTRL1

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#ifdef AVR32_SCIF_101_H_INCLUDED
#define AVR32_SCIF_OSCCTRL0_STARTUP_0_RCOSC                 0x00000000
#define AVR32_SCIF_OSCCTRL0_STARTUP_128_RCOSC               0x00000002
#define AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC             0x00000006
#define AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC              0x00000003
#define AVR32_SCIF_OSCCTRL0_STARTUP_4096_RCOSC              0x00000004
#define AVR32_SCIF_OSCCTRL0_STARTUP_64_RCOSC                0x00000001
#define AVR32_SCIF_OSCCTRL0_STARTUP_8192_RCOSC              0x00000005
#define AVR32_SCIF_OSCCTRL1_STARTUP_0_RCOSC                 0x00000000
#define AVR32_SCIF_OSCCTRL1_STARTUP_128_RCOSC               0x00000002
#define AVR32_SCIF_OSCCTRL1_STARTUP_16384_RCOSC             0x00000006
#define AVR32_SCIF_OSCCTRL1_STARTUP_2048_RCOSC              0x00000003
#define AVR32_SCIF_OSCCTRL1_STARTUP_4096_RCOSC              0x00000004
#define AVR32_SCIF_OSCCTRL1_STARTUP_64_RCOSC                0x00000001
#define AVR32_SCIF_OSCCTRL1_STARTUP_8192_RCOSC              0x00000005
#define AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC                0x00000000
#define AVR32_SCIF_OSCCTRL32_STARTUP_128_RCOSC              0x00000001
#define AVR32_SCIF_OSCCTRL32_STARTUP_131072_RCOSC           0x00000005
#define AVR32_SCIF_OSCCTRL32_STARTUP_16384_RCOSC            0x00000003
#define AVR32_SCIF_OSCCTRL32_STARTUP_262144_RCOSC           0x00000006
#define AVR32_SCIF_OSCCTRL32_STARTUP_524288_RCOSC           0x00000007
#define AVR32_SCIF_OSCCTRL32_STARTUP_65536_RCOSC            0x00000004
#define AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC             0x00000002
#define AVR32_SCIF_OSCCTRL0_MODE_CRYSTAL                    0x00000001
#define AVR32_SCIF_OSCCTRL0_MODE_EXTCLK                     0x00000000
#define AVR32_SCIF_OSCCTRL0_GAIN_G0                         0x00000000
#define AVR32_SCIF_OSCCTRL0_GAIN_G1                         0x00000001
#define AVR32_SCIF_OSCCTRL0_GAIN_G2                         0x00000002
#define AVR32_SCIF_OSCCTRL0_GAIN_G3                         0x00000003
#define AVR32_SCIF_OSCCTRL1_GAIN_G0                         0x00000000
#define AVR32_SCIF_OSCCTRL1_GAIN_G1                         0x00000001
#define AVR32_SCIF_OSCCTRL1_GAIN_G2                         0x00000002
#define AVR32_SCIF_OSCCTRL1_GAIN_G3                         0x00000003
#define AVR32_SCIF_UNLOCK_KEY_VALUE                         0x000000AA
#define AVR32_SCIF_OSCCTRL_OSCEN_ENABLE                     0x00000001
#define AVR32_SCIF_OSCCTRL_OSCEN_DISABLE                    0x00000000
#define AVR32_SCIF_OSCCTRL32_OSC32EN_ENABLE                 0x00000001
#define AVR32_SCIF_OSCCTRL32_OSC32EN_DISABLE                0x00000000
#endif


//! Device-specific data

//!< External crystal/clock min frequency (in Herz)
#define SCIF_EXT_CRYSTAL_MIN_FREQ_HZ  4000000
//!< External crystal/clock max frequency (in Herz)
#define SCIF_EXT_CRYSTAL_MAX_FREQ_HZ  20000000

//! The different clock source for the generic clocks
typedef enum
{
  SCIF_GCCTRL_SLOWCLOCK  = AVR32_SCIF_GC_USES_CLK_SLOW,
  SCIF_GCCTRL_OSC32K  = AVR32_SCIF_GC_USES_CLK_32,
  SCIF_GCCTRL_OSC0  = AVR32_SCIF_GC_USES_OSC0,
  SCIF_GCCTRL_OSC1  = AVR32_SCIF_GC_USES_OSC1,
  SCIF_GCCTRL_PLL0  = AVR32_SCIF_GC_USES_PLL0,
  SCIF_GCCTRL_PLL1  = AVR32_SCIF_GC_USES_PLL1,
  SCIF_GCCTRL_RC8M  = AVR32_SCIF_GC_USES_RCOSC8,
  SCIF_GCCTRL_CPUCLOCK  = AVR32_SCIF_GC_USES_CLK_CPU,
  SCIF_GCCTRL_HSBCLOCK  = AVR32_SCIF_GC_USES_CLK_HSB,
  SCIF_GCCTRL_PBACLOCK  = AVR32_SCIF_GC_USES_CLK_PBA,
  SCIF_GCCTRL_PBBCLOCK  = AVR32_SCIF_GC_USES_CLK_PBB,
  SCIF_GCCTRL_PBCCLOCK  = AVR32_SCIF_GC_USES_CLK_PBC,
  SCIF_GCCTRL_OSCSEL_INVALID
} scif_gcctrl_oscsel_t;



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

//! The different plls
typedef enum
{
  SCIF_PLL0  = 0,
  SCIF_PLL1  = 1
} scif_pll_t;

//! The different oscillator modes
typedef enum
{
  SCIF_OSC_MODE_EXT_CLK = 0,            // For both OSC0 & OSC32
  SCIF_OSC_MODE_2PIN_CRYSTAL = 1,       // For both OSC0 & OSC32
  SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR = 2, // For OSC32 only
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

//! PLL0/PLL1 startup options.
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

//! OSC32 startup options.
typedef struct
{
  //! The input frequency (from the external clock or the crystal).
  unsigned long   freq_hz;
  //! Set the oscillator mode.
  scif_osc_mode_t mode;
  //! Specify the oscillator startup time.
  unsigned char   startup;
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
} scif_gclk_opt_t;


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

/*! \brief Enable external clock.
 *
 * \param osc The oscillator to configure [INPUT]
 *
 * \return Status.
 *   \retval 0   Oscillator successfully started
 *   \retval <0  Error starting the oscillator.
 */
extern long int scif_enable_extosc(scif_osc_t osc);

//! @}


/*! \name PLL0/PLL1 Functions
 */
//! @{

/*!
 * \brief This function will setup a PLL.
 * \param pll The PLL to configure [INPUT]
 * \param opt The configuration of the PLL [INPUT]
 * \return Status.
 *   \retval 0   PLL successfully started
 *   \retval <0  Error starting the PLL.
 */
extern long int scif_pll_setup(scif_pll_t pll, const scif_pll_opt_t *opt);

/*!
 * \brief This function will enable a PLL.
 * \param pll The PLL to configure [INPUT]
 * \return Status.
 *   \retval 0   PLL successfully started
 *   \retval <0  Error starting the PLL.
 */
extern long int scif_pll_enable(scif_pll_t pll);


/*!
 * \brief This function will disable a PLL.
 * \param pll The PLL to configure [INPUT]
 * \return Status.
 *   \retval 0   PLL successfully started
 *   \retval <0  Error starting the PLL.
 */
extern long int scif_pll_disable(scif_pll_t pll);


/*!
 * \brief This function will wait for PLL locked
 * \param pll The PLL to configure [INPUT]
 * \return Status.
 *   \retval 0   PLL successfully started
 *   \retval <0  Error starting the PLL.
 */
extern long int scif_wait_for_pll_locked(scif_pll_t pll);

//! @}

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


/*! \brief Stop the OSC32 oscillator.
 *
 * \return Status.
 *   \retval 0 Oscillator successfully stopped.
 *   \retval <0 An error occurred when stopping the oscillator.
 */
extern long int scif_stop_osc32(void);

//! @}


/*! \name Calibration Functions
 */
//! @{

/*! \brief Enables the 1.8V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod18_enable_irq(void);


/*! \brief Disables the 1.8V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod18_disable_irq(void);


/*! \brief Clears the 1.8V Brown-Out Detector interrupt flag.
 *
 */
extern void scif_bod18_clear_irq(void);


/*! \brief Gets the 1.8V Brown-Out Detector interrupt flag.
 *
 * \retval 0 No BOD interrupt.
 * \retval 1 BOD interrupt pending.
 */
extern unsigned long scif_bod18_get_irq_status(void);


/*! \brief Gets the 1.8V Brown-Out Detector interrupt enable status.
 *
 * \retval 0 BOD interrupt disabled.
 * \retval 1 BOD interrupt enabled.
 */
extern unsigned long scif_bod18_get_irq_enable_bit(void);


/*! \brief Gets the triggering threshold of the 1.8V Brown-Out Detector.
 *
 */
extern unsigned long scif_bod18_get_level(void);


/*! \brief Enables the 3.3V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod33_enable_irq(void);


/*! \brief Disables the 3.3V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod33_disable_irq(void);


/*! \brief Clears the 3.3V Brown-Out Detector interrupt flag.
 *
 */
extern void scif_bod33_clear_irq(void);


/*! \brief Gets the 3.3V Brown-Out Detector interrupt flag.
 *
 * \retval 0 No BOD interrupt.
 * \retval 1 BOD interrupt pending.
 */
extern unsigned long scif_bod33_get_irq_status(void);


/*! \brief Gets the 3.3V Brown-Out Detector interrupt enable status.
 *
 * \retval 0 BOD interrupt disabled.
 * \retval 1 BOD interrupt enabled.
 */
extern unsigned long scif_bod33_get_irq_enable_bit(void);


/*! \brief Gets the triggering threshold of the 3.3V Brown-Out Detector.
 *
 */
extern unsigned long scif_bod33_get_level(void);

/*! \brief Enables the 5.0V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod50_enable_irq(void);


/*! \brief Disables the 5.0V Brown-Out Detector interrupt.
 *
 */
extern void scif_bod50_disable_irq(void);


/*! \brief Clears the 5.0V Brown-Out Detector interrupt flag.
 *
 */
extern void scif_bod50_clear_irq(void);


/*! \brief Gets the 5.0V Brown-Out Detector interrupt flag.
 *
 * \retval 0 No BOD interrupt.
 * \retval 1 BOD interrupt pending.
 */
extern unsigned long scif_bod50_get_irq_status(void);


/*! \brief Gets the 5.0V Brown-Out Detector interrupt enable status.
 *
 * \retval 0 BOD interrupt disabled.
 * \retval 1 BOD interrupt enabled.
 */
extern unsigned long scif_bod50_get_irq_enable_bit(void);


/*! \brief Gets the triggering threshold of the 5.0V Brown-Out Detector.
 *
 */
extern unsigned long scif_bod50_get_level(void);

//! @}


/*! \name Critical Path Oscillator Functions
 */
//! @{


//! @}

/*! \name 8MHz RCosc Functions
 */
//! @{

/*! \brief Start the 8MHz internal RCosc (RC8M) clock
 */
extern void scif_start_rc8M(void);

/*! \brief Stop the 8MHz internal RCosc (RC8M) clock
 */
extern void scif_stop_rc8M(void);


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
/*!
 * \brief Read the content of the SCIF GPLP registers
 * \param gplp GPLP register index (0,1,... depending on the number of GPLP registers for a given part)
 *
 * \return The content of the chosen GPLP register.
 */
extern unsigned long scif_read_gplp(unsigned long gplp);


/*!
 * \brief Write into the SCIF GPLP registers
 * \param gplp GPLP register index (0,1,... depending on the number of GPLP registers for a given part)
 * \param value Value to write
 */
extern void scif_write_gplp(int gplp, unsigned long value);

//! @}


/*! \name Temperature Sensor Management Functions
 */
//! @{


/*! \brief Enable the temperature sensor
 *
 * \note Due to register unlock process, this function needs to be atomic. So if
 *       there's risk that call is split by any interrupts, disable them first.
 *
 */
__always_inline static void scif_temperature_sensor_enable(void)
{
  // Unlock the write-protected TSENS register
  SCIF_UNLOCK(AVR32_SCIF_TSENS);
  AVR32_SCIF.tsens = AVR32_SCIF_TSENS_EN_MASK;
}

/*! \brief Disable the temperature sensor.
 *
 * \note Due to register unlock process, this function needs to be atomic so if
 *       there's risk that call is split by an interrupt, disable them first.
 *
 */
__always_inline static void scif_temperature_sensor_disable(void)
{
  // Unlock the write-protected TSENS register
  SCIF_UNLOCK(AVR32_SCIF_TSENS);
  AVR32_SCIF.tsens = ~AVR32_SCIF_TSENS_EN_MASK;
}

//! @}


#ifdef __cplusplus
}
#endif

/**
 * \}
 */

#endif  // _SCIF_UC3C_H_
