/**************************************************************************
 *
 * \file
 *
 * \brief Management of the config file access.
 *
 * This file manages the accesses to the config files.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

//!
//! @brief This function replace a (or many) line(s) in a file.
//!        Syntax of the line should be : \verbatim field=value\r\n \endverbatim
//!
//! @param filename Input. The filename to write into.
//! @param ac       Input. The argument counter. For this command, should be 1.
//! @param av       Input. The argument vector.
//!
//! @return short : 0 if success or -1 else
//!
signed short config_file_set_value(char * filename, int ac, signed char *av[]);

//!
//! @brief get module config, by parsing config file.
//!
//! @param filename     Input. file to read into.
//! @param pcField      Input. field to find in file.
//! @param pcValue      Output. value read for this field, as string.
//!
//! @return short       0 if success, -1 otherwise.
//!
signed short config_file_get_value(char * filename, char * pcField, char *pcValue);

#endif // __CONFIG_H__

