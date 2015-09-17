/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/* 
 * The AT24MAC602 is a serial EEPROM on the REB233 X-Pro board containing, among
 * other things, a 64-bit MAC address. This driver is intended to in the simplest
 * possible way read out this MAC address for use with the Thingsquare firmware.
 */
#ifndef _AT24MAC602_H_
#define _AT24MAC602_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "contiki.h"
/*---------------------------------------------------------------------------*/
/* An EUI-64 identifier */
typedef union {
  uint8_t     u8[8];
  uint16_t    u16[4];
  uint32_t    u32[2];
} eui64_t;

/* A 128-bit, globally unique, serial number */
typedef union {
  uint8_t     u8[16];
  uint16_t    u16[8];
  uint32_t    u32[4];
} at24mac602_serialnumber_t;

/* Atmel IEEE OUI (OUI is the first three bytes in the EUI-64) */
#define EUI64_ATMEL_OUI_1     0xFC
#define EUI64_ATMEL_OUI_2     0xC2
#define EUI64_ATMEL_OUI_3     0x3D
/*---------------------------------------------------------------------------*/
int at24mac602_init(void);
int at24mac602_read_eui64(eui64_t *eui);
int at24mac602_read_serial128(at24mac602_serialnumber_t *serial);
/*---------------------------------------------------------------------------*/
#endif  /* _AT24MAC602_H_ */
