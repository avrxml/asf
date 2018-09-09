/**
 * \file
 *
 * \brief Oscillator Frequency detection
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _FREQ_DETECT_H_
#define _FREQ_DETECT_H_

/**
 * \defgroup group_avr32_services_freq_detect External oscillator frequency detection
 *
 * A software service to estimate an oscillator frequency using a known oscillator (external or internal) as a reference.
 *
 * \{
 */

/*! \name External routine used to detect frequency
 */
//! \{
  //! Callback used to reset the counter of the reference's oscillator.
extern void freq_detect_iface_ref_cnt_reset(void);
  //! Callback used to read the counter value of the reference's oscillator.
extern int freq_detect_iface_ref_cnt_value(void);
  //! Callback used to reset the counter of the target oscillator.
extern void freq_detect_iface_target_cnt_reset(void);
  //! Callback used to read the counter value of the target oscillator.
extern int freq_detetc_iface_target_cnt_value(void);
//! \}

/*! \brief Detects the frequency
 *
 * \return The frequency in Hz or 0 if no detection.
 *
 */
uint32_t freq_detect_start(void);

/**
 * \}
 */

#endif //_FREQ_DETECT_H_
