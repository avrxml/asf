/**
* \file
*
* \brief PRIME Stack Version Configuration.
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
#include "conf_prime.h"

#ifdef __GNUC__
const char FWVS_VERSION[32]  __attribute__((section (".frwvrs"))) = PRIME_FW_VERSION ;
const char FWVS_MODEL[16]    __attribute__((section (".frwvrs"))) = PRIME_FW_MODEL ;
const char FWVS_VENDOR[16]   __attribute__((section (".frwvrs"))) = PRIME_FW_VENDOR ;
#endif

#ifdef __ICCARM__
#pragma location = ".frwvrs"
__root const char FWVS_VENDOR[16] = PRIME_FW_VENDOR ;
#pragma location = ".frwvrs"
__root const char FWVS_MODEL[16] = PRIME_FW_MODEL ;
#pragma location = ".frwvrs"
__root const char FWVS_VERSION[32] = PRIME_FW_VERSION ;
#endif
