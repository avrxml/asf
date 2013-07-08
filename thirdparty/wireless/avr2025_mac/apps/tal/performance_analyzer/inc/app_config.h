/**
 * \file app_config.h
 *
 * \brief These are application-specific resources which are used
 *        in the application in addition to the underlaying stack
 * - Performance Analyzer application.
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2012, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* === Includes ============================================================= */

#include "stack_config.h"
#include "user_interface.h"
#include <conf_board.h>
#include "board.h"

/* === Macros =============================================================== */

/* Version of the software */
#define MAJOR_NUMBER        4
#define MINOR_NUMBER        0

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#define DEFAULT_CHANNEL         (1)
#else
#define DEFAULT_CHANNEL         (21)
#endif
#define DEFAULT_PAN_ID          (0xCAFE)
#define DST_PAN_ID              (DEFAULT_PAN_ID)
#define SRC_PAN_ID              (DEFAULT_PAN_ID)
#define DEFAULT_ADDR            (0xFFFF)
#define DST_SHORT_ADDR          (0xFFFF)

/* Frame overhead due to selected address scheme incl. FCS */
#if (DST_PAN_ID == SRC_PAN_ID)
#define FRAME_OVERHEAD          (11)
#else
#define FRAME_OVERHEAD          (13)
#endif

#define FRAME_OVERHEAD_SRC_IEEE_ADDR (FRAME_OVERHEAD + 6)
#define FRAME_OVERHEAD_DST_IEEE_ADDR (FRAME_OVERHEAD + 6)

#define OFFSET_FOR_SRC_IEEE_ADDR    (7)

#define DEFAULT_SCAN_DURATION   8

#if (LED_COUNT >= 3)
#define STATUS_LED              LED0
#define TX_LED                  LED1
#define RX_LED                  LED2
#elif (LED_COUNT >= 2)
#define STATUS_LED              LED0
#define TX_LED                  LED0
#define RX_LED                  LED1
#else
#define STATUS_LED              LED0
#define TX_LED                  LED0
#define RX_LED                  LED0
#endif

/* === Types ================================================================ */

#define NUMBER_OF_APP_TIMERS        (4)

#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + \
	NUMBER_OF_TOTAL_STACK_TIMERS)

/* Additional buffers used by the application */
#define NUMBER_OF_LARGE_APP_BUFS    (12)
#define NUMBER_OF_SMALL_APP_BUFS    (0)

#define TOTAL_NUMBER_OF_LARGE_BUFS  (NUMBER_OF_LARGE_APP_BUFS +	\
	NUMBER_OF_LARGE_STACK_BUFS)
#define TOTAL_NUMBER_OF_SMALL_BUFS  (NUMBER_OF_SMALL_APP_BUFS +	\
	NUMBER_OF_SMALL_STACK_BUFS)

#define TOTAL_NUMBER_OF_BUFS        (TOTAL_NUMBER_OF_LARGE_BUFS + \
	TOTAL_NUMBER_OF_SMALL_BUFS)

/* Offset of IEEE address storage location within EEPROM */
#define EE_IEEE_ADDR                (0)

/* Macro to enable the feature of counting wrong CRC packets */
#if ((TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B) || \
	(TAL_TYPE == AT86RF231) || (TAL_TYPE == AT86RF233))
#define CRC_SETTING_ON_REMOTE_NODE
#endif

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_CONFIG_H */
/* EOF */
