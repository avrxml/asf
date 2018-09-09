/**
 * \file
 *
 * \brief ATPL230 Physical layer - Version
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

#ifndef ATPL230VERSION_H_INCLUDE
#define ATPL230VERSION_H_INCLUDE

#include "parts.h"

/**
 * \weakgroup phy_plc_group
 * @{
 */

/* \name Version information for ATPL230 */
/* @{ */
#define ATPL230_COPYMSG            "Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries."
#define ATPL230_VERSION_STR        "23.00.01.02"
#define ATPL230_VERSION_NUM        0x23000102

#if SAM4CP16_0
#define ATPL230_PRODID             "SAM4CP16B "
#elif SAM4CMP16_0
#define ATPL230_PRODID             "SAM4CMP16C"
#elif SAM4CMS16_0
#define ATPL230_PRODID             "SAM4CMS16C"
#elif SAM4C16_0
#define ATPL230_PRODID             "SAM4C16C  "
#elif SAM4S
#define ATPL230_PRODID             "ATPL230   "
#else
# error Unsupported chip type
#endif
/* @} */

/* @} */

#endif  /* ATPL230VERSION_H_INCLUDE */
