/**
 * @file sio_handler.c
 *
 * @brief RF4CE Serial API
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
