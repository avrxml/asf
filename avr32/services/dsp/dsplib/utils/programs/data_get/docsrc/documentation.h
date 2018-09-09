/**
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
