/**
 * \file
 *
 * \brief Settings for ADC and DAC test.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CLASSB_ADC_DAC_H_
#define CLASSB_ADC_DAC_H_

#include <asf.h>

/**
 * \defgroup adc_dac  Analog I/O Test
 *
 * \brief Self-diagnostic test for the ADC, DAC and analog multiplexer.
 *
 * A plausibility check is done to make sure that ADC, DAC and analog
 * multiplexer work correctly. The test is implemented in
 * \ref classb_analog_io_test() and consists of the following steps:
 * -# the DAC is configured to output one of five known values.
 * -# the ADC is set up to measure the DAC with all of its channels and the
 * results compared with the expected value.
 * -# repeat for the remaining DAC values.
 *
 * If the measured values deviate from the expected values more than the
 * threshold configured with \ref CLASSB_ADC_DEV, the test calls the error
 * handler \ref CLASSB_ERROR_HANDLER_ANALOG().
 *
 * The test checks one ADC module and one DAC module at a time, i.e., it has to
 * be repeated until all modules are tested. Further, the ADC module to test
 * must be able to read from the DAC module it is tested together with.
 *
 * \note
 *  - Interrupts should be disabled during this test.
 * @{
 */

/**
 * \name Internal settings
 *
 * @{
 */

/**
 * \brief Maximum allowed absolute deviation for test measurements.
 *
 * Error offset for the ADC is +-2mV, which corresponds to +-0x40 when the
 * reference is 1V and  \c TOP is \c 2047.
 */
#define CLASSB_ADC_DEV      0x40

/** @} */

void classb_analog_io_test(DAC_t *dacptr, ADC_t *adcptr);

/** @} */

#endif /* CLASSB_ADC_DAC_H_ */
