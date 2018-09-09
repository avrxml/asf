/**
 * \file
 *
 * \brief Chip-specific Interrupt configuration
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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