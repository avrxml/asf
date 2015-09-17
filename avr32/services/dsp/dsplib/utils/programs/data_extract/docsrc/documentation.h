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
 * \defgroup group_dsplib_dataextract Data Extraction
 * \ingroup group_dsplib_tools
 * \brief This program is used to extract data from a buffer and print them with the following format on the standard output.
 * \code 0.1545211
	-0.5892455
	... \endcode
 * It parses every line of the input buffer and check if it matches with the given pattern (gave in argument to this program).
 * If it does, it will extract the specified data from the buffer and print it, else it will ignore the line.
 *
 * \section usage Usage
 * \code DataExtract input_buffer_file_path pattern n_arg \endcode
 * - The \b input_buffer_file_path argument is the file path of the input buffer.
 * - The \b pattern argument is the pattern used to parse the line. This pattern is uses the same syntax as the printf function.
 * Example: if you want to parse a complex number, you will have to use the following pattern: "%f + %fi".
 * - The \b n_arg argument specifies the index of the number to extract form the previously defined pattern.
 *
 * Usage example:\n
 * To extract the 2nd number of a file which lines are defined as follow: integer float integer
 * \code DataExtract ./buffer.txt "%i %f %i" 2 \endcode
 *
 * \section information Information
 * All the files related to this module are located under /avr32/services/dsp/dsplib/utils/program/data_extract\n
 * In this directory you can find the source code and the binary of this module.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
