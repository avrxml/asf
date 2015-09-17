/**
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * \defgroup group_dsplib_dataget Data Get
 * \ingroup group_dsplib_tools
 * \brief This program is used to get any type of data from a COM or LPT port.
 *
 * \section usage Usage
 * \code DataGet [-l number] [-b number] [-f filename] [-h] [port [baud_rate [bits_per_char [parity [stopbit]]]]] \endcode
 * - The \b -l number argument is used to refresh the output file every number lines retrieved.
 * - The \b -b number argument is used to refresh the output file every number bytes retrieved.
 * - The \b -f filename argument is used to store the output data into the filename file.
 * If this argument is not specified the result will be printed on the standard output.
 * - The \b -h argument permits to display the help.
 * - The \b port argument specifies the identifier of the port to use.
 * - The \b baud_rate argument specifies the baud rate of the transmission.
 * - The \b bits_per_char argument specifies the number of bits per characters.
 * - The \b parity argument specifies the parity of the transmission. (0 for no parity, 1 for even parity and 2 for odd parity)
 * - The \b stopbit argument specifies the format of the stop bit. (1 for 1 stop bit, 2 for 1.5 stop bit and 3 for 2 stop bits)
 *
 * \section information Information
 * All the files related to this module are located under /avr32/services/dsp/dsplib/utils/program/data_get\n
 * In this directory you can find the source code and the binary of this module.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
