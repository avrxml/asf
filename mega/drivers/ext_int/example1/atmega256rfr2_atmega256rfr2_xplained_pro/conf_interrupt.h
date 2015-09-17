/**
 * \file
 *
 * \brief Chip-specific Interrupt configuration
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef CONF_INTERRUPT_H
#define CONF_INTERRUPT_H

/* External Interrupt INT0 */
#define CONFIG_EXT_INT0      EXT_INT0_PIN
/* External Interrupt INT1 */
#define CONFIG_EXT_INT1      EXT_INT1_PIN
/* External Interrupt INT2 */
#define CONFIG_EXT_INT2      EXT_INT2_PIN
/* External Interrupt INT3 */
#define CONFIG_EXT_INT3      EXT_INT3_PIN
/* External Interrupt INT4 */
#define CONFIG_EXT_INT4      EXT_INT4_PIN
/* External Interrupt INT5 */
#define CONFIG_EXT_INT5      EXT_INT5_PIN
/* External Interrupt INT6 */
#define CONFIG_EXT_INT6      EXT_INT6_PIN
/* External Interrupt INT7 */
#define CONFIG_EXT_INT7      EXT_INT7_PIN

/* External PC Interrupt PCINT0 */
#define CONFIG_PC_INT0      PC_INT0_PIN
/* External PC Interrupt PCINT1 */
#define CONFIG_PC_INT1      PC_INT1_PIN
/* External PC Interrupt PCINT2 */
#define CONFIG_PC_INT2      PC_INT2_PIN
/* External PC Interrupt PCINT3 */
#define CONFIG_PC_INT3      PC_INT3_PIN
/* External PC Interrupt PCINT4 */
#define CONFIG_PC_INT4      PC_INT4_PIN
/* External PC Interrupt PCINT5 */
#define CONFIG_PC_INT5      PC_INT5_PIN
/* External PC Interrupt PCINT6 */
#define CONFIG_PC_INT6      PC_INT6_PIN
/* External PC Interrupt PCINT7 */
#define CONFIG_PC_INT7      PC_INT7_PIN
/* External PC Interrupt PCINT8 */
#define CONFIG_PC_INT8      PC_INT8_PIN

#endif