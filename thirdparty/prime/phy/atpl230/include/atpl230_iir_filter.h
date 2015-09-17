/**
 * \file
 *
 * \brief ATPL230 Physical layer - IIR Filter
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#ifndef ATPL230IIRFILTER_H_INCLUDE
#define ATPL230IIRFILTER_H_INCLUDE

#include "compiler.h"

/**
 * \weakgroup phy_plc_group
 * @{
 */

/* \name IIR Filter definitions */
/* @{ */
#define LENGTH_ROW_DATA_FILTER_IIR      2
#define NUM_ROWS_DATA_FILTER_IIR        40
#define LENGTH_DATA_FILTER_IIR (NUM_ROWS_DATA_FILTER_IIR * LENGTH_ROW_DATA_FILTER_IIR)

#define LENGTH_ROW_DATA_CHIRP   4
#define NUM_ROWS_DATA_CHIRP     65
#define LENGTH_DATA_CHIRP       (NUM_ROWS_DATA_CHIRP * LENGTH_ROW_DATA_CHIRP)

#define LENGTH_ROW_DATA_ANGLE_REAL_IMAG_COMP    2
#define NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP      97
#define LENGTH_DATA_ANGLE_REAL_IMAG_COMP (NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP * LENGTH_ROW_DATA_ANGLE_REAL_IMAG_COMP)

#define LENGTH_ROW_DATA_OFFSET_CORRECTION       4
#define NUM_ROWS_DATA_OFFSET_CORRECTION         16
#define LENGTH_DATA_OFFSET_CORRECTION          (NUM_ROWS_DATA_OFFSET_CORRECTION * LENGTH_ROW_DATA_OFFSET_CORRECTION)
/* @} */

/* @} */

#endif  /* ATPL230IIRFILTER_H_INCLUDE */
