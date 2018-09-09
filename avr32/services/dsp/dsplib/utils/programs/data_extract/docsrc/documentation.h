/**
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
