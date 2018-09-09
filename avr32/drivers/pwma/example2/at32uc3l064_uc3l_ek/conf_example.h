/**
 * \file
 *
 * \brief Example Configuration 
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


#ifndef CONF_EXAMPLE_H_
#define CONF_EXAMPLE_H_

//PWMA configuration options
#  define EXAMPLE_PWMA_GCLK_ID             AVR32_SCIF_GCLK_PWMA
#  define EXAMPLE_PWMA_GCLK_SOURCE         GENCLK_SRC_CLK_PBA
#  define EXAMPLE_PWMA_GCLK_FREQUENCY      12000000
#  define EXAMPLE_PWMA_OUTPUT_FREQUENCY    1000000
#  define EXAMPLE_PWMA_CHANNEL_ID          9
#  define EXAMPLE_PWMA_PIN                 AVR32_PWMA_9_PIN
#  define EXAMPLE_PWMA_FUNCTION            AVR32_PWMA_9_FUNCTION
#  define ERROR_LED                        LED0

//Analog Comparator options
#  define EXAMPLE_AC_GCLK_ID               AVR32_SCIF_GCLK_ACIFB
#  define EXAMPLE_AC_GCLK_SRC              GENCLK_SRC_CLK_PBA
#  define EXAMPLE_AC_GCLK_FREQUENCY        10000
#  define EXAMPLE_AC_STARTUP_CYCLES        6
#  define EXAMPLE_AC_NB_CHANNELS           1
#  define EXAMPLE_ACIFB_CHANNEL            3
#  define EXAMPLE_ACIFBP_PIN               AVR32_ACIFB_ACBP_1_PIN
#  define EXAMPLE_ACIFBP_FUNCTION          AVR32_ACIFB_ACBP_1_FUNCTION
#  define EXAMPLE_ACIFBN_PIN               AVR32_ACIFB_ACBN_1_PIN
#  define EXAMPLE_ACIFBN_FUNCTION          AVR32_ACIFB_ACBN_1_FUNCTION
#  define EXAMPLE_ACIFB_CHANNEL_MASK       AVR32_ACIFB_SR_ACCS3_MASK
#  define EXAMPLE_ACIFB_INTERRUPT_PRIORITY 0
#  define EXAMPLE_ACIFB_INTERRUPT_MASK     AVR32_ACIFB_ACINT3_MASK



#endif /* CONF_EXAMPLE_H_ */
