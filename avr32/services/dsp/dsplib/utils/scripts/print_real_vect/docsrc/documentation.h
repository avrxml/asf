/**
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

/*!
 * \defgroup group_dsplib_printrealvect Real Vector Visualizer
 * \ingroup group_dsplib_tools
 * \brief This script is used to visualize a fixed-length data buffer from the serial port formatted as follow:
 * \code 1.      0.265445
	2.      -0.025633
	... \endcode
 * All others pattern lines will be avoided and not included in the visualization.
 * This script is useful to visualize example's results. In a nutshell, it permits to visualize data printed with
 * the dsp16_debug_print_vect and dsp32_debug_print_vect functions.\n
 * This script is a combination of three main programs (\ref group_dsplib_dataget, \ref group_dsplib_dataextract and \ref group_dsplib_dataprint)
 *
 * \section usage Usage
 * The following variables can be modified in the script:
 * - \b COM_PORT: the serial port identifier used (i.e.: COM1).
 * - \b COM_BAUDRATE: the baud rate of the transmission (i.e.: 9600).
 *
 * Then, once launched, the script will wait until a reset has been performed on the target
 * (which means it will wait until the beginning of the transmission to be sure data are well synchronized).
 *
 * \section information Information
 * All the files related to this module are located under /avr32/services/dsp/dsplib/utils/scripts/print_real_vect\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
