/**
 * \file
 *
 * \brief Sleep controller driver
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef SLEEP_H
#define SLEEP_H

#include "compiler.h"

/**
 * \section mega_sleep_quickstart_section Quick Start Guide
 * See \ref mega_sleep_quickstart
 *
 * @{
 */

#if defined(__ICCAVR__) || defined(__DOXYGEN__)
# include <intrinsics.h>
/* ! Macro for issuing the sleep instruction. */
# define sleep_enter() __sleep()
#elif defined(__GNUC__)
# include <avr/sleep.h>
# define sleep_enter() sleep_cpu()
#else
# error Unsupported compiler.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*  \brief Sleep Mode Select */
enum SLEEP_SMODE_enum {
	SLEEP_SMODE_IDLE = (0x00 << SM0), /* Idle mode */
	SLEEP_SMODE_ADC_NOISE_REDUCTION      = (0x01 << SM0), /* ADC Noise
	                                                        * Reduction */
	SLEEP_SMODE_PDOWN        = (0x02 << SM0), /* Power-down Mode */
	SLEEP_SMODE_PSAVE = (0x03 << SM0), /* Power-save Mode */
	SLEEP_SMODE_STDBY = (0x06 << SM0), /* Standby Mode */
	SLEEP_SMODE_ESTDBY = (0x07 << SM0), /* Extended Standby Mode */
};

#ifdef __cplusplus
}
#endif

 /*! @} */

/**
 * \page mega_sleep_quickstart Quick Start Guide for the MEGA Sleep Driver
 *
 * This is the quick start guide for the \ref sleep_group "Sleep Driver", with
 * step-by-step instructions on how to configure and use the driver for a
 * specific use case.
 *
 * The section described below can be copied into, e.g. the main application
 * loop or any other function that will need to control and execute different
 * sleep modes on the device.
 *
 * \section mega_sleep_quickstart_basic Basic usage of the sleep driver
 * This use case will prepare the device to enter the Power Down sleep mode and
 * then enter the sleep mode. After waking up it will disable sleep.
 *
 * \section mega_sleep_basic_usage Usage steps
 * \subsection mega_sleep_basic_usage_code Example code
 * Add to, e.g., the main loop in the application C-file:
 * \code
	sleep_set_mode(SLEEP_SMODE_PDOWN_gc);
	sleep_enable();
	sleep_enter();
	sleep_disable();
\endcode
 *
 * \subsection Workflow
 * -# Set what sleep mode to use.
 *  - \code sleep_set_mode(SLEEP_SMODE_PDOWN_gc); \endcode
 * -# Enable that the device are allowed to go to sleep:
 *  - \code sleep_enable(); \endcode
 *  - \note This function has to be called in order for the device to go to
 *  sleep. This is a safety feature to stop the device to go to sleep
 *  unintentionally, even though it is possible to have this enabled at all
 *  times
 *  it is recommended to enable sleep mode only when you intend to go to sleep
 *  within a few clock cycles.
 * -# Enter sleep mode:
 *  - \code sleep_enter(); \endcode
 *  - \attention Make sure to enable global interrupt and the interrupt you
 *  plan to use as wake-up source for your device, do also pay special
 *  attention to what wake-up sources are available for the different sleep
 *  modes. Failing to enable interrupts may result in indefinite sleep until
 *  power is cycled!
 * -# When the device is woken from sleep it will execute the interrupt handler
 *  related to the wakeup-source (interrupt source) and continue on the next
 *  line of code after the \ref sleep_enter() call. Make sure to disable sleep
 *  when waking up.
 *  - \code sleep_disable(); \endcode
 *
 *  \subsection Sleep Modes
 *  Possible sleep modes depend on the device that is used. Please refer to the
 *  device datasheet and header file to find these definitions.
 */
 

#endif /* SLEEP_H */
