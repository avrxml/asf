/*****************************************************************************
 *
 * \file
 *
 * \brief Timer/Counter driver for AVR32 UC3.
 *
 * AVR32 Timer/Counter driver module.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _TC_H_
#define _TC_H_

/**
 * \defgroup group_avr32_drivers_tc TC - Timer/Counter
 *
 * Driver for the TC (Timer/Counter). The TC includes three identical 16-bit Timer/Counter channels.
 * Each channel can be independently programmed to perform a wide range of functions including frequency measurement,
 * event counting, interval measurement, pulse generation, delay timing and pulse width modulation.
 *
 * \{
 */

#include <avr32/io.h>


//! TC driver functions return value in case of invalid argument(s).
#define TC_INVALID_ARGUMENT                     (-1)

//! Number of timer/counter channels.
#define TC_NUMBER_OF_CHANNELS                   (sizeof(((avr32_tc_t *)0)->channel) / sizeof(avr32_tc_channel_t))

/*! \name External Clock Signal 0 Selection
 */
//! @{
#define TC_CH0_EXT_CLK0_SRC_TCLK0               AVR32_TC_TC0XC0S_TCLK0
#define TC_CH0_EXT_CLK0_SRC_NO_CLK              AVR32_TC_TC0XC0S_NO_CLK
#define TC_CH0_EXT_CLK0_SRC_TIOA1               AVR32_TC_TC0XC0S_TIOA1
#define TC_CH0_EXT_CLK0_SRC_TIOA2               AVR32_TC_TC0XC0S_TIOA2
//! @}

/*! \name External Clock Signal 1 Selection
 */
//! @{
#define TC_CH1_EXT_CLK1_SRC_TCLK1               AVR32_TC_TC1XC1S_TCLK1
#define TC_CH1_EXT_CLK1_SRC_NO_CLK              AVR32_TC_TC1XC1S_NO_CLK
#define TC_CH1_EXT_CLK1_SRC_TIOA0               AVR32_TC_TC1XC1S_TIOA0
#define TC_CH1_EXT_CLK1_SRC_TIOA2               AVR32_TC_TC1XC1S_TIOA2
//! @}

/*! \name External Clock Signal 2 Selection
 */
//! @{
#define TC_CH2_EXT_CLK2_SRC_TCLK2               AVR32_TC_TC2XC2S_TCLK2
#define TC_CH2_EXT_CLK2_SRC_NO_CLK              AVR32_TC_TC2XC2S_NO_CLK
#define TC_CH2_EXT_CLK2_SRC_TIOA0               AVR32_TC_TC2XC2S_TIOA0
#define TC_CH2_EXT_CLK2_SRC_TIOA1               AVR32_TC_TC2XC2S_TIOA1
//! @}

/*! \name Event/Trigger Actions on Output
 */
//! @{
#define TC_EVT_EFFECT_NOOP                      AVR32_TC_NONE
#define TC_EVT_EFFECT_SET                       AVR32_TC_SET
#define TC_EVT_EFFECT_CLEAR                     AVR32_TC_CLEAR
#define TC_EVT_EFFECT_TOGGLE                    AVR32_TC_TOGGLE
//! @}

/*! \name RC Compare Trigger Enable
 */
//! @{
#define TC_NO_TRIGGER_COMPARE_RC                0
#define TC_TRIGGER_COMPARE_RC                   1
//! @}

/*! \name Waveform Selection
 */
//! @{
#define TC_WAVEFORM_SEL_UP_MODE                 AVR32_TC_WAVSEL_UP_NO_AUTO
#define TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER      AVR32_TC_WAVSEL_UP_AUTO
#define TC_WAVEFORM_SEL_UPDOWN_MODE             AVR32_TC_WAVSEL_UPDOWN_NO_AUTO
#define TC_WAVEFORM_SEL_UPDOWN_MODE_RC_TRIGGER  AVR32_TC_WAVSEL_UPDOWN_AUTO
//! @}

/*! \name TIOA or TIOB External Trigger Selection
 */
//! @{
#define TC_EXT_TRIG_SEL_TIOA                    1
#define TC_EXT_TRIG_SEL_TIOB                    0
//! @}

/*! \name External Event Selection
 *
 * \attention See \ref tc_attention_eevt "this note" for important details about
 * this setting.
 */
//! @{
#define TC_EXT_EVENT_SEL_TIOB_INPUT             AVR32_TC_EEVT_TIOB_INPUT
#define TC_EXT_EVENT_SEL_XC0_OUTPUT             AVR32_TC_EEVT_XC0_OUTPUT
#define TC_EXT_EVENT_SEL_XC1_OUTPUT             AVR32_TC_EEVT_XC1_OUTPUT
#define TC_EXT_EVENT_SEL_XC2_OUTPUT             AVR32_TC_EEVT_XC2_OUTPUT
//! @}

/*! \name Edge Selection
 */
//! @{
#define TC_SEL_NO_EDGE                          AVR32_TC_EEVTEDG_NO_EDGE
#define TC_SEL_RISING_EDGE                      AVR32_TC_EEVTEDG_POS_EDGE
#define TC_SEL_FALLING_EDGE                     AVR32_TC_EEVTEDG_NEG_EDGE
#define TC_SEL_EACH_EDGE                        AVR32_TC_EEVTEDG_BOTH_EDGES
//! @}

/*! \name Burst Signal Selection
 */
//! @{
#define TC_BURST_NOT_GATED                      AVR32_TC_BURST_NOT_GATED
#define TC_BURST_CLK_AND_XC0                    AVR32_TC_BURST_CLK_AND_XC0
#define TC_BURST_CLK_AND_XC1                    AVR32_TC_BURST_CLK_AND_XC1
#define TC_BURST_CLK_AND_XC2                    AVR32_TC_BURST_CLK_AND_XC2
//! @}

/*! \name Clock Invert
 */
//! @{
#define TC_CLOCK_RISING_EDGE                    0
#define TC_CLOCK_FALLING_EDGE                   1
//! @}

/*! \name Clock Selection
 */
//! @{
#define TC_CLOCK_SOURCE_TC1                     AVR32_TC_TCCLKS_TIMER_CLOCK1
#define TC_CLOCK_SOURCE_TC2                     AVR32_TC_TCCLKS_TIMER_CLOCK2
#define TC_CLOCK_SOURCE_TC3                     AVR32_TC_TCCLKS_TIMER_CLOCK3
#define TC_CLOCK_SOURCE_TC4                     AVR32_TC_TCCLKS_TIMER_CLOCK4
#define TC_CLOCK_SOURCE_TC5                     AVR32_TC_TCCLKS_TIMER_CLOCK5
#define TC_CLOCK_SOURCE_XC0                     AVR32_TC_TCCLKS_XC0
#define TC_CLOCK_SOURCE_XC1                     AVR32_TC_TCCLKS_XC1
#define TC_CLOCK_SOURCE_XC2                     AVR32_TC_TCCLKS_XC2
//! @}


//! Timer/counter interrupts.
typedef struct
{
  unsigned int                 :24;

  //! External trigger interrupt.
  unsigned int etrgs           : 1;

  //! RB load interrupt.
  unsigned int ldrbs           : 1;

  //! RA load interrupt.
  unsigned int ldras           : 1;

  //! RC compare interrupt.
  unsigned int cpcs            : 1;

  //! RB compare interrupt.
  unsigned int cpbs            : 1;

  //! RA compare interrupt.
  unsigned int cpas            : 1;

  //! Load overrun interrupt.
  unsigned int lovrs           : 1;

  //! Counter overflow interrupt.
  unsigned int covfs           : 1;
} tc_interrupt_t;

//! Parameters when initializing a timer/counter in capture mode.
typedef struct
{
  //! Channel to initialize.
  unsigned int channel            ;

  unsigned int                 :12;

  //! RB loading selection:\n
  //!   - \ref TC_SEL_NO_EDGE;\n
  //!   - \ref TC_SEL_RISING_EDGE;\n
  //!   - \ref TC_SEL_FALLING_EDGE;\n
  //!   - \ref TC_SEL_EACH_EDGE.
  unsigned int ldrb            : 2;

  //! RA loading selection:\n
  //!   - \ref TC_SEL_NO_EDGE;\n
  //!   - \ref TC_SEL_RISING_EDGE;\n
  //!   - \ref TC_SEL_FALLING_EDGE;\n
  //!   - \ref TC_SEL_EACH_EDGE.
  unsigned int ldra            : 2;

  unsigned int                 : 1;

  //! RC compare trigger enable:\n
  //!   - \ref TC_NO_TRIGGER_COMPARE_RC;\n
  //!   - \ref TC_TRIGGER_COMPARE_RC.
  unsigned int cpctrg          : 1;

  unsigned int                 : 3;

  //! TIOA or TIOB external trigger selection:\n
  //!   - \ref TC_EXT_TRIG_SEL_TIOA;\n
  //!   - \ref TC_EXT_TRIG_SEL_TIOB.
  unsigned int abetrg          : 1;

  //! External trigger edge selection:\n
  //!   - \ref TC_SEL_NO_EDGE;\n
  //!   - \ref TC_SEL_RISING_EDGE;\n
  //!   - \ref TC_SEL_FALLING_EDGE;\n
  //!   - \ref TC_SEL_EACH_EDGE.
  unsigned int etrgedg         : 2;

  //! Counter clock disable with RB loading:\n
  //!   - \c false;\n
  //!   - \c true.
  unsigned int ldbdis          : 1;

  //! Counter clock stopped with RB loading:\n
  //!   - \c false;\n
  //!   - \c true.
  unsigned int ldbstop         : 1;

  //! Burst signal selection:\n
  //!   - \ref TC_BURST_NOT_GATED;\n
  //!   - \ref TC_BURST_CLK_AND_XC0;\n
  //!   - \ref TC_BURST_CLK_AND_XC1;\n
  //!   - \ref TC_BURST_CLK_AND_XC2.
  unsigned int burst           : 2;

  //! Clock invert:\n
  //!   - \ref TC_CLOCK_RISING_EDGE;\n
  //!   - \ref TC_CLOCK_FALLING_EDGE.
  unsigned int clki            : 1;

  //! Clock selection:\n
  //!   - \ref TC_CLOCK_SOURCE_TC1;\n
  //!   - \ref TC_CLOCK_SOURCE_TC2;\n
  //!   - \ref TC_CLOCK_SOURCE_TC3;\n
  //!   - \ref TC_CLOCK_SOURCE_TC4;\n
  //!   - \ref TC_CLOCK_SOURCE_TC5;\n
  //!   - \ref TC_CLOCK_SOURCE_XC0;\n
  //!   - \ref TC_CLOCK_SOURCE_XC1;\n
  //!   - \ref TC_CLOCK_SOURCE_XC2.
  unsigned int tcclks          : 3;
} tc_capture_opt_t;

//! Parameters when initializing a timer/counter in waveform mode.
typedef struct
{
  //! Channel to initialize.
  unsigned int channel            ;

  //! Software trigger effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int bswtrg          : 2;

  //! External event effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int beevt           : 2;

  //! RC compare effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int bcpc            : 2;

  //! RB compare effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int bcpb            : 2;

  //! Software trigger effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int aswtrg          : 2;

  //! External event effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int aeevt           : 2;

  //! RC compare effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int acpc            : 2;

  //! RA compare effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
  unsigned int acpa            : 2;

  unsigned int                 : 1;

  //! Waveform selection:\n
  //!   - \ref TC_WAVEFORM_SEL_UP_MODE;\n
  //!   - \ref TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER;\n
  //!   - \ref TC_WAVEFORM_SEL_UPDOWN_MODE;\n
  //!   - \ref TC_WAVEFORM_SEL_UPDOWN_MODE_RC_TRIGGER.
  unsigned int wavsel          : 2;

  //! External event trigger enable:\n
  //!   - \c false;\n
  //!   - \c true.
  unsigned int enetrg          : 1;

  //! External event selection:\n
  //!   - \ref TC_EXT_EVENT_SEL_TIOB_INPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC0_OUTPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC1_OUTPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC2_OUTPUT.
  //! \attention See \ref tc_attention_eevt "this note" for important details
  //! about this setting.
  unsigned int eevt            : 2;

  //! External event edge selection:\n
  //!   - \ref TC_SEL_NO_EDGE;\n
  //!   - \ref TC_SEL_RISING_EDGE;\n
  //!   - \ref TC_SEL_FALLING_EDGE;\n
  //!   - \ref TC_SEL_EACH_EDGE.
  unsigned int eevtedg         : 2;

  //! Counter clock disable with RC compare:\n
  //!   - \c false;\n
  //!   - \c true.
  unsigned int cpcdis          : 1;

  //! Counter clock stopped with RC compare:\n
  //!   - \c false;\n
  //!   - \c true.
  unsigned int cpcstop         : 1;

  //! Burst signal selection:\n
  //!   - \ref TC_BURST_NOT_GATED;\n
  //!   - \ref TC_BURST_CLK_AND_XC0;\n
  //!   - \ref TC_BURST_CLK_AND_XC1;\n
  //!   - \ref TC_BURST_CLK_AND_XC2.
  unsigned int burst           : 2;

  //! Clock invert:\n
  //!   - \ref TC_CLOCK_RISING_EDGE;\n
  //!   - \ref TC_CLOCK_FALLING_EDGE.
  unsigned int clki            : 1;

  //! Clock selection:\n
  //!   - \ref TC_CLOCK_SOURCE_TC1;\n
  //!   - \ref TC_CLOCK_SOURCE_TC2;\n
  //!   - \ref TC_CLOCK_SOURCE_TC3;\n
  //!   - \ref TC_CLOCK_SOURCE_TC4;\n
  //!   - \ref TC_CLOCK_SOURCE_TC5;\n
  //!   - \ref TC_CLOCK_SOURCE_XC0;\n
  //!   - \ref TC_CLOCK_SOURCE_XC1;\n
  //!   - \ref TC_CLOCK_SOURCE_XC2.
  unsigned int tcclks          : 3;
} tc_waveform_opt_t;


/*! \brief Reads timer/counter interrupt settings.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 The interrupt enable configuration organized according to \ref tc_interrupt_t.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_get_interrupt_settings(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Enables various timer/counter interrupts.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 * \param bitfield        The interrupt enable configuration.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_configure_interrupts(volatile avr32_tc_t *tc, unsigned int channel, const tc_interrupt_t *bitfield);

/*! \brief Selects which external clock to use and how to configure it.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 * \param ext_clk_sig_src External clock signal selection:
 *   \arg \c TC_CH0_EXT_CLK0_SRC_TCLK0;
 *   \arg \c TC_CH0_EXT_CLK0_SRC_NO_CLK;
 *   \arg \c TC_CH0_EXT_CLK0_SRC_TIOA1;
 *   \arg \c TC_CH0_EXT_CLK0_SRC_TIOA2;
 *   \arg \c TC_CH1_EXT_CLK1_SRC_TCLK1;
 *   \arg \c TC_CH1_EXT_CLK1_SRC_NO_CLK;
 *   \arg \c TC_CH1_EXT_CLK1_SRC_TIOA0;
 *   \arg \c TC_CH1_EXT_CLK1_SRC_TIOA2;
 *   \arg \c TC_CH2_EXT_CLK2_SRC_TCLK2;
 *   \arg \c TC_CH2_EXT_CLK2_SRC_NO_CLK;
 *   \arg \c TC_CH2_EXT_CLK2_SRC_TIOA0;
 *   \arg \c TC_CH2_EXT_CLK2_SRC_TIOA1.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_select_external_clock(volatile avr32_tc_t *tc, unsigned int channel, unsigned int ext_clk_sig_src);

/*! \brief Sets options for timer/counter capture initialization.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param opt             Options for capture mode.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_init_capture(volatile avr32_tc_t *tc, const tc_capture_opt_t *opt);

/*! \brief Sets options for timer/counter waveform initialization.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param opt             Options for waveform generation.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 *
 * \anchor tc_attention_eevt
 * \attention If the external event selection, \ref tc_waveform_opt_t::eevt
 * "EEVT", is set to \ref TC_EXT_EVENT_SEL_TIOB_INPUT or the equivalent value 0,
 * TIOB becomes an input even if external event triggers
 * are \e not enabled (\ref tc_waveform_opt_t::enetrg "ENETRG").\n
 * Therefore, unless it is the intention to use TIOB as an external event trigger,
 * \ref tc_waveform_opt_t::eevt "EEVT" should be set to any other value than
 * \ref TC_EXT_EVENT_SEL_TIOB_INPUT to allow normal operation of TIOB.
 */
extern int tc_init_waveform(volatile avr32_tc_t *tc, const tc_waveform_opt_t *opt);

/*! \brief Starts a timer/counter.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_start(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Stops a timer/counter.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_stop(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Performs a software trigger: the counter is reset and the clock is started.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval 0 Success.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_software_trigger(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Asserts a SYNC signal to generate a software trigger and reset all channels.
 *
 * \param tc              Pointer to the TC instance to access.
 */
extern void tc_sync_trigger(volatile avr32_tc_t *tc);

/*! \brief Start all TC channels simultaneously.
 *
 * \param tc              Pointer to the TC instance to access.
 */
extern void tc_sync_start(volatile avr32_tc_t *tc);

/*! \brief Reads the status register.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 Status register value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_read_sr(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Reads the channel's TC counter and returns the value.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 TC counter value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_read_tc(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Reads the channel's RA register and returns the value.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 RA register value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_read_ra(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Reads the channel's RB register and returns the value.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 RB register value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_read_rb(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Reads the channel's RC register and returns the value.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 *
 * \retval >=0 RC register value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_read_rc(volatile avr32_tc_t *tc, unsigned int channel);

/*! \brief Writes a value to the channel's RA register.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 * \param value           Value to write to the RA register.
 *
 * \retval >=0 Written value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_write_ra(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value);

/*! \brief Writes a value to the channel's RB register.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 * \param value           Value to write to the RB register.
 *
 * \retval >=0 Written value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_write_rb(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value);

/*! \brief Writes a value to the channel's RC register.
 *
 * \param tc              Pointer to the TC instance to access.
 * \param channel         The TC instance channel to access.
 * \param value           Value to write to the RC register.
 *
 * \retval >=0 Written value.
 * \retval TC_INVALID_ARGUMENT Invalid argument(s).
 */
extern int tc_write_rc(volatile avr32_tc_t *tc, unsigned int channel, unsigned short value);

/**
 * \}
 */

#endif  // _TC_H_
