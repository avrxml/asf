/**
 * @file sio_handler.c
 *
 * @brief RF4CE Serial API
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef API_PARSER_H
#define API_PARSER_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

/** \page api RF4Control API Parser
 *    - \ref group_api_parser_rf4ce
 */

/**
 * \defgroup group_api_parser_rf4ce RF4Control API Parser Module
 * This module is responsible for encoding and decoding the serial bytes to/from
 * NCP into NWK or ZRC APIs to provide seamless working for application similar
 * to stack running in same processor or on
 * NCP.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

void serial_api_init(void);

/* bool nwk_task(void); */

#define RF4CONTROL_PID                         (2)

/* MAC Protocol Identifier Length */
#define RF4CONTROL_PID_LEN                     (1)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* SERIAL_API_H */
/* ! @} */
/* EOF */
