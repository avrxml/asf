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
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "contiki.h"
#include "at24mac602.h"
#include "at24mac602-arch.h"
/*---------------------------------------------------------------------------*/
/**
 * \brief      Init the AT24MAC602 EEPROM
 * \retval 0   Success
 * \retval -1  Fail
 */
int
at24mac602_init(void)
{
  return at24mac602_arch_init();
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Read the EUI-64 from the AT24MAC602 EEPROM
 * \param eui  Pointer to an EUI-64 struct to fill out with the EUI-64 read.
 * \retval 0   Success
 * \retval -1  Fail
 */
int
at24mac602_read_eui64(eui64_t *eui)
{
  if(at24mac602_arch_read_eui64(eui) == -1) {
    printf("failed to read EUI-64 from EEPROM\n");
    eui->u8[0] = 0;
    eui->u8[1] = 1;
    eui->u8[2] = 2;
    eui->u8[3] = 3;
    eui->u8[4] = 4;
    eui->u8[5] = 5;
    eui->u8[6] = 6;
    eui->u8[7] = 7;
    return -1;
  }
  reset_i2c();
  at24mac602_arch_init();
 
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Read the 128-bit serial number from the EEPROM.
 * \param serial    pointer to serial number structure that will contain the serial number after reading
 * \retval 0   Success
 * \retval -1  Fail
 *            
 *            
 */
int
at24mac602_read_serial128(at24mac602_serialnumber_t *serial)
{
  if(at24mac602_arch_read_serial128(serial) == -1) {
    printf("failed to read serial128 from EEPROM\n");
    return -1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
