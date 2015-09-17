/*****************************************************************************
 *
 * \file
 *
 * \brief PWM driver for AVR32 UC3 with PWM module version above 4.0.0.
 *
 * This file defines a useful set of functions for the PWM interface on AVR32
 * devices.
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


#ifndef _PWM_H_
#define _PWM_H_

/**
 * \defgroup group_avr32_drivers_pwm PWM - Pulse Width Modulation (>v4.0.0)
 *
 * Pulse Width Modulation (PWM) Software Driver for modules above revision >v4.0.0.
 * This driver provides an API to get access to the main features of the PWM controller.
 *
 * \{
 */

#include <avr32/io.h>

#include "compiler.h"

//! Local Definition for the number of PWM Lines
#define AVR32_PWM_LINES_MSB                    4

//! Value returned by function when it completed successfully.
#define PWM_SUCCESS                            0

//! Value returned by function when it was unable to complete successfully
//! for some unspecified reason.
#define PWM_FAILURE                           -1

//! Value returned by function when the input parameters are out of range.
#define PWM_INVALID_INPUT                      1

//! Value returned by function when the channel number is invalid.
#define PWM_INVALID_ARGUMENT                   1

//! Operate PWM channel in left aligned mode.
#define PWM_MODE_LEFT_ALIGNED                  0

//! Operate PWM channel in center aligned mode.
#define PWM_MODE_CENTER_ALIGNED                1

//! PWM channel starts output low level.
#define PWM_POLARITY_LOW                       0

//! PWM channel starts output high level.
#define PWM_POLARITY_HIGH                      1

//! PWM channel write in CUPDx updates duty cycle at the next period start event.
#define PWM_UPDATE_DUTY                        0

//! PWM channel write in CUPDx updates period at the next period start event.
#define PWM_UPDATE_PERIOD                      1

//! PWM Output Override Value Low Level
#define PWM_OOV_VALUE_LOW                      0

//! PWM Output Override Value High Level
#define PWM_OOV_VALUE_HIGH                     1

//! PWM Output Override Control OFF
#define PWM_OOV_CTRL_OFF                       0

//! PWM Output Override Control ON
#define PWM_OOV_CTRL_ON                        1

//! PWM Ouptut Override Number of Lines: PWM0,1,2,3 => 4
#define PWM_OOV_LINES                          4

//! PWM Protection Key
#define PWM_WPKEY                              0x50574D

//! PWM No Write To Protected Register Status
#define PWM_NO_WRITE_PROTECT_VIOLATION         0xFFFFFFFF

//! PWM Sync channel ON
#define PWM_SYNC_CHANNEL_ON                    1

//! PWM Sync channel OFF
#define PWM_SYNC_CHANNEL_OFF                   1

//! PWM Manual Write And Manual Update Period, Duty Cycle and Event Method
#define PWM_SYNC_UPDATE_MANUAL_WRITE_MANUAL_UPDATE       0

//! PWM Manual Write And Automatic Update Period, Duty Cycle and Event Method
#define PWM_SYNC_UPDATE_MANUAL_WRITE_AUTO_UPDATE         1

//! PWM Automatic Write And Automatic Update Period, Duty Cycle and Event Method
#define PWM_SYNC_UPDATE_AUTO_WRITE_AUTO_UPDATE           2

//! PWM Update Lock State
#define PWM_UPDLOCK_TRIGGER                              1

//! PWM Number of Fault Lines
#define PWM_FAULT_LINES                                  8

//! PWM Fault Input Active Low State
#define PWM_FAULT_INPUT_ACTIVE_LOW                       0

//! PWM Fault Input Active High State
#define PWM_FAULT_INPUT_ACTIVE_HIGH                      1

//! PWM Fault Input Filtering Option OFF
#define PWM_FAULT_INPUT_FILTER_OFF                       0

//! PWM Fault Input Filtering Option ON
#define PWM_FAULT_INPUT_FILTER_ON                        1


//! PWM Output Low Level when fault detection
#define PWM_OUTPUT_LOW_WHEN_FAULT_DETECTION              0

//! PWM Output High Level when fault detection
#define PWM_OUTPUT_HIGH_WHEN_FAULT_DETECTION             1

//! PWM Fault Input Not Used
#define PWM_FAULT_INPUT_NOT_USED                         0

//! PWM Fault Input Used
#define PWM_FAULT_INPUT_USED                             1

//! PWM Fault Mode Level
#define PWM_FAULT_MODE_LEVEL                             0

//! PWM Fault Mode Toggle
#define PWM_FAULT_MODE_TGL                               1

//! PWM Fault Output Lines: PWM0,1,2,3 => 4
#define PWM_FAULT_OUTPUT                                 4

//! PWM Clock Input is Master Clock
#define PWM_CKSEL_MCK					  												0

//! PWM Clock Input is Generic Clock
#define PWM_CKSEL_GCLK					  												1

//! PWM Compare UP
#define PWM_COMPARE_MODE_UP                            0

//! PWM Compare DOWN
#define PWM_COMPARE_MODE_DOWN                          1

//! PWM Compare ON
#define PWM_COMPARE_ON                                 1

//! PWM Compare OFF
#define PWM_COMPARE_OFF                                0

#define PWM_EVENT_LINE0                                (1<<AVR32_PWM_EL0MR_CSEL0_OFFSET)

#define PWM_EVENT_LINE1                                (1<<AVR32_PWM_EL0MR_CSEL1_OFFSET)

#define PWM_EVENT_LINE2                                (1<<AVR32_PWM_EL0MR_CSEL2_OFFSET)

#define PWM_EVENT_LINE3                                (1<<AVR32_PWM_EL0MR_CSEL3_OFFSET)

#define PWM_EVENT_LINE4                                (1<<AVR32_PWM_EL0MR_CSEL4_OFFSET)

#define PWM_EVENT_LINE5                                (1<<AVR32_PWM_EL0MR_CSEL5_OFFSET)

#define PWM_EVENT_LINE6                                (1<<AVR32_PWM_EL0MR_CSEL6_OFFSET)

#define PWM_EVENT_LINE7                                (1<<AVR32_PWM_EL0MR_CSEL7_OFFSET)

//! Input parameters when initializing a PWM channel.
typedef struct
{
  //! CLKB divide factor.
  unsigned int divb;

  //! CLKA divide factor.
  unsigned int diva;

  //! Divider input clock B.
  unsigned int preb;

  //! Divider input clock A.
  unsigned int prea;

  //! Clock Input Selection.
  unsigned int cksel;

  //! Fault Detection Activation.
  unsigned char fault_detection_activated;

  //! Synchronous Channel Activation.
  unsigned char sync_channel_activated;

  //! Synchronous channel Mode.
  unsigned char sync_update_channel_mode;

  //! Synchronous Channel Select.
  unsigned char sync_channel_select[PWM_OOV_LINES];
} pwm_opt_t;


typedef struct
{
  //! Output Selection Register
  unsigned char os[2][PWM_OOV_LINES];

  //! Output Value Register
  unsigned char oov[2][PWM_OOV_LINES];

} pwm_output_override_opt_t;

typedef struct
{
  //! Compare Match PWM Counter Value
  U32 compare_pwm_counter_value;

  //! Compare Match  Mode
  unsigned char compare_mode;

  //! Compare Match PWM Period Value
  U32 compare_pwm_period_counter_value;

  //! Compare Match Period Value
  U8 compare_period_counter_value;

  //! Compare Update Period Value
  U8 compare_pwm_update_period;

  //! Compare Status
  U8 compare_status;

} pwm_compare_opt_t;


/*! \brief This function initialize the PWM controller (mode register) and disable the interrupt.
 * \param opt PWM Channel structure parameter
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_init(pwm_opt_t *opt);

/*! \brief Initialize a specific PWM channel.
 * \param channel_id The channel identifier mask
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS, PWM_INVALID_INPUT or PWM_INVALID_ARGUMENT
 */
extern int pwm_channel_init(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Start PWM channels.
 * \param channels_bitmask A bit-mask with set bits indicating channels to start.
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_start_channels(unsigned long channels_bitmask);

/*! \brief Stop PWM channels.
 * \param channels_bitmask A bit-mask with set bits indicating channels to stop.
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_stop_channels(unsigned long channels_bitmask);

/*! \brief Update channel register CPRDx or CDTYx with the polling method. This function uses the CUPDx register
 * as a double buffer for the period or the duty cycle. Only the first 20 bits of cupd are significant.
 * \param channel_id The channel identifier (0 to max channel-1)
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Update channel register CPRDx or CDTYx with the interrupt method. This function uses the CUPDx register
 * as a double buffer for the period or the duty cycle. Only the first 20 bits of cupd are significant.
 * \param channel_id The channel identifier (0 to max channel-1)
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_interrupt_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Output Override Configuration. This function uses the OS and OOV registers
 *  \param opt Output Override Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_output_override_setup(pwm_output_override_opt_t *opt);

/*! \brief Output Override Set. This function uses the OS and OOV registers
 *  \param opt Output Override Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_output_override_write(pwm_output_override_opt_t *opt);

/*! \brief Hardware Write Protect Lock Function. This function uses the WPMR register
 *  \param group
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_write_protect_hw_lock(int group);

/*! \brief Software Write Protect Lock Function. This function uses the WPMR register
 *  \param group
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_write_protect_sw_lock(int group);

/*! \brief Software Write Protect Unlock Function. This function uses the WPMR register
 *  \param group
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_write_protect_sw_unlock(int group);

/*! \brief Write Protect Status Function. This function uses the WPSR register
 *  \return PWM_NO_WRITE_PROTECT_VIOLATION or Write Protection Violation Source
 */
extern U32 pwm_write_protect_status(void);

/*! \brief Define Update Period Value For Update Of Synchronous Channel
 *  \param value  Update Period Value
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_period_value(unsigned int value);

/*! \brief Define Update Period Value For Update Of Synchronous Channel after a first init
 *  \param value  Update Period Value
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_period_value_update(unsigned int value);

/*! \brief Return Current Counter of Update Period Value
 *  \return Value of Counter
 */
extern unsigned int pwm_get_update_period_counter_value(void);

/*! \brief Prepare a Synchronous Channel, User should call this function for all the synchronous channel
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_synchronous_prepare_update(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Synchronous Update with Manual Write and Manual Update Method
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_manual_write_manual_update(void);

/*! \brief Synchronous Update with Manual Write and Auto Update Method
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_manual_write_auto_update(void);

/*! \brief Synchronous Update with Manual Write and Auto Update Method
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_auto_write_auto_update_check_ready(void);

/*! \brief Synchronous Update with Manual Write and Auto Update Method
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_update_auto_write_auto_update_check_eot(void);

/*! \brief Fault Lines Level Polarity Function
 *  \param line      Line
 *  \param polarity  Polarity
 *  \param mode      Mode
 *  \param filter    Filter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_fault_lines_polarity_setup(U8 line,
                                           U8 polarity,
                                           U8 mode,
                                           U8 filter);

/*! \brief Fault Lines Level Setup Function
 *  \param line Line
 *  \param channel_high Channel High
 *  \param channel_low  Channel Low
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_fault_lines_level_setup(U8 line,
                                        U8 channel_high,
                                        U8 channel_low);

/*! \brief Fault Lines Enable function
 *  \param line    Line
 *  \param channel Channel
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_fault_lines_enable(U8 line,
                                   U8 channel);
/*! \brief Get Fault Status
 *  \return Value of FSR register
 */
extern int pwm_get_fault_status(void);

/*! \brief Return Fault Status
 */
extern void pwm_clr_fault_status(void);

/*! \brief Compare Match Setup
 *  \param line Compare Line to be used
 *  \param pwm_compare_t Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_match_setup(int line,pwm_compare_opt_t *pwm_compare_t);

/*! \brief Compare Match Reload Setup
 *  \param line Compare Line to be used
 *  \param pwm_compare_t Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_match_reload_setup(int line,pwm_compare_opt_t *pwm_compare_t);

/*! \brief Fault Lines Enable function
 *  \param line Compare Line to be used
 *  \param pwm_compare_t Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_update_period_value(int line,pwm_compare_opt_t *pwm_compare_t);

/*! \brief Compare Reload Update Period
 *  \param line Compare Line to be used
 *  \param pwm_compare_t Parameter
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_reload_update_period_value(int line,pwm_compare_opt_t *pwm_compare_t);

/*! \brief Compare Match Enable
 *  \param line Compare Line to be used
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_enable(int line);

/*! \brief Compare Match Disable
 *  \param line Compare Line to be used
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_disable(int line);

/*! \brief Compare Event Enable
 *  \param line Compare Line to be used
 *  \param event_line Event Line to be used
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_event_enable(int line, int event_line);

/*! \brief Compare Event Disable
 *  \param line Compare Line to be used
 *  \param event_line Event Line to be used
 *  \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_compare_event_disable(int line, int event_line);

/**
 * \}
 */

#endif  // _PWM_H_
