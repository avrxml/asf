/**
 * \file
 *
 * \brief Example Configuration 
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
