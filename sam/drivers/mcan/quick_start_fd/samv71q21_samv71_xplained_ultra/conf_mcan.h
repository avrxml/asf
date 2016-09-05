/**
 * \file
 *
 * \brief SAM Control Area Network Driver Configuration Header
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_MCAN_H_INCLUDED
#define CONF_MCAN_H_INCLUDED

/*
 * Below is the message RAM setting, it will be stored in the system RAM.
 * Please adjust the message size according to your application.
 */
/** Range: 1..64 */
#define CONF_MCAN0_RX_FIFO_0_NUM         4
/** Range: 1..64 */
#define CONF_MCAN0_RX_FIFO_1_NUM         4
/** Range: 1..64 */
#define CONF_MCAN0_RX_BUFFER_NUM         4
/** Range: 1..16 */
#define CONF_MCAN0_TX_BUFFER_NUM         2
/** Range: 1..16 */
#define CONF_MCAN0_TX_FIFO_QUEUE_NUM     2
/** Range: 1..32 */
#define CONF_MCAN0_TX_EVENT_FIFO         4
/** Range: 1..128 */
#define CONF_MCAN0_RX_STANDARD_ID_FILTER_NUM     4
/** Range: 1..64 */
#define CONF_MCAN0_RX_EXTENDED_ID_FILTER_NUM     2
/** Range: 1..64 */
#define CONF_MCAN1_RX_FIFO_0_NUM         4
/** Range: 1..64 */
#define CONF_MCAN1_RX_FIFO_1_NUM         4
/** Range: 1..64 */
#define CONF_MCAN1_RX_BUFFER_NUM         4
/** Range: 1..16 */
#define CONF_MCAN1_TX_BUFFER_NUM         2
/** Range: 1..16 */
#define CONF_MCAN1_TX_FIFO_QUEUE_NUM     2
/** Range: 1..32 */
#define CONF_MCAN1_TX_EVENT_FIFO         4
/** Range: 1..128 */
#define CONF_MCAN1_RX_STANDARD_ID_FILTER_NUM     4
/** Range: 1..64 */
#define CONF_MCAN1_RX_EXTENDED_ID_FILTER_NUM     2

/** The value should be 8/12/16/20/24/32/48/64. */
#define CONF_MCAN_ELEMENT_DATA_SIZE         64

/*
 * The setting of the nominal bit rate is based on the PCK5 which is 30M which you can
 * change. Below is the default configuration. The
 * time quanta is 30MHz / (2+1) =  10MHz. And each bit is (1 + NTSEG1 + 1 + NTSEG2 + 1) = 20 time
 * quanta which means the bit rate is 10MHz/20=500KHz.
 */
/** Nominal bit Baud Rate Prescaler */
#define CONF_MCAN_NBTP_NBRP_VALUE    2
/** Nominal bit (Re)Synchronization Jump Width */
#define CONF_MCAN_NBTP_NSJW_VALUE    3
/** Nominal bit Time segment before sample point */
#define CONF_MCAN_NBTP_NTSEG1_VALUE  10
/** Nominal bit Time segment after sample point */
#define CONF_MCAN_NBTP_NTSEG2_VALUE  7

/*
 * The setting of the data bit rate is based on the PCK5 which is 30M which you can
 * change. Below is the default configuration. The
 * time quanta is 30MHz / (0+1) =  30MHz. And each bit is (1 + FTSEG1 + 1 + FTSEG2 + 1) = 16 time
 * quanta which means the bit rate is 30MHz/15=2MHz.
 */
/** Data bit Baud Rate Prescaler */
#define CONF_MCAN_FBTP_FBRP_VALUE    0
/** Data bit (Re)Synchronization Jump Width */
#define CONF_MCAN_FBTP_FSJW_VALUE    3
/** Data bit Time segment before sample point */
#define CONF_MCAN_FBTP_FTSEG1_VALUE  9
/** Data bit Time segment after sample point */
#define CONF_MCAN_FBTP_FTSEG2_VALUE  3

#endif
