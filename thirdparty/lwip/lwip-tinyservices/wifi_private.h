/**
 *
 * \file
 *
 * \brief Private definitions.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef WIFI_PRIVATE_H_INCLUDED
#define WIFI_PRIVATE_H_INCLUDED

static inline uint16_t _wifi_get16_be(uint8_t *p)
{
	return p[1] | (((uint16_t) p[0]) << 8);
}

static inline void _wifi_put16_be(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t) (v>>8);
	p[1] = (uint8_t) v;
}

static inline uint16_t _wifi_get16_le(uint8_t *p)
{
	return p[0] | (((uint16_t) p[1]) << 8);
}

static inline void _wifi_put16_le(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t) v;
	p[1] = (uint8_t) (v>>8);
}

static inline uint32_t _wifi_get32_be(uint8_t *p)
{
    return p[3] | (((uint32_t)p[2]) << 8) | (((uint32_t)p[1]) << 16) | (((uint32_t)p[0]) << 24);
}

static inline void _wifi_put32_be(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >>  8);
    p[3] = (uint8_t)v;
}

static inline uint32_t _wifi_get32_le(uint8_t *p)
{
	return p[0] | (((uint32_t)p[1]) << 8) | (((uint32_t)p[2]) << 16) | (((uint32_t)p[3]) << 24);
}

static inline void _wifi_put32_le(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >>  8);
	p[2] = (uint8_t)(v >> 16);
	p[3] = (uint8_t)(v >> 24);
}

#endif /* WIFI_PRIVATE_H_INCLUDED */
