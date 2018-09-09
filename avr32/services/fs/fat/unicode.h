/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Unicode Service.
 *
 * This file defines a useful set of functions for decode string unicode from ASCII, UTF8, UTF16
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _UNICODE_H_
#define _UNICODE_H_

//_____ D E F I N I T I O N S _______________________________________________

#define UNI_MAX_HEADER_SIZE   3
#define UNI_MAX_UTF8_SIZE     3

//! \name Define a ID for each text format
//! PS: This defines numbers are extract of ID3 norms
//! @{
#define UNI_TYPE_UNKNOWN      0xFF
#define UNI_TYPE_ASCII        0x00
#define UNI_TYPE_UTF16LE      0x01 // unicode endianess known: little-endian
#define UNI_TYPE_UTF16BE      0x02 // unicode endianess known: big-endian
#define UNI_TYPE_UTF8         0x03
//! @}


//! Detect the text format via a header (3 bytes)
//!
//! @param  header      header to analyze (array of 3B)
//!
//! @return text format identifier
//!
uint8_t    unicode_header_scan( uint8_t* header );

//! Return the header corresponding at text format identifier
//!
//! @param  txt_format  UNI_TYPE_UTF8, UNI_TYPE_UTF16BE, UNI_TYPE_UTF16LE, UNI_TYPE_ASCII
//! @param  header      header to fill (array of 3B)
//!
//! @return the size of header
//!
uint8_t    unicode_header_get( uint8_t* header, uint8_t txt_format );

//! Conversion UTF-8 to unicode
//!
//! @param  utf8     array with utf8 codes (array of 3B max)
//! @param  unicode  pointer used to store the character unicode decoded
//!
//! @return the size of UTF8 decoded
//!
uint8_t    utf8_to_unicode( uint8_t* utf8, uint16_t* unicode );


#endif  // _UNICODE_H_

