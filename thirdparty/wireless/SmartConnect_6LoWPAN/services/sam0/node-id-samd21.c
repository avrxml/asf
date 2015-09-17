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

#include <stdio.h>
#include <string.h>
#include "node-id.h"
#include "contiki-conf.h"
#include "at24mac602.h"

unsigned short node_id = 0;
unsigned char node_mac[8];
/*---------------------------------------------------------------------------*/
#define _DEBUG_ 0
#if _DEBUG_
#include <stdio.h>
#define PRINTF(...)       printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
/* If device ID is stored in flash */
#define NODEID_BASEADDR              (0x0003FF80)
static const uint8_t magic_bytes[] = {0xde, 0xad, 0xbe, 0xef};
/*---------------------------------------------------------------------------*/
/* 
 * This fallback is used if the REB233-XPro supplied EEPROM fails. It first tries
 * to use any MAC address stored in flash, and if that fails too, resorts to a
 * default MAC address.
 */
static int
fallback(void)
{
  int i;
  uint8_t *user_mem;

  /* check that the magic bytes exists */
  user_mem = (uint8_t *) NODEID_BASEADDR;
  for(i = 0; i < 4; i++) {
    if(*user_mem != magic_bytes[i]) {
      PRINTF("node-id: No stored MAC found. Using default.\n");
      node_mac[0] = 0x02;
      node_mac[1] = 0x12;
      node_mac[2] = 0x00;
      node_mac[3] = 0x74;
      node_mac[4] = 0x00;
      node_mac[5] = 0x01;
      node_mac[6] = 0x02;
      node_mac[7] = 0x03;
      return -1;
    }
    user_mem += 1;
  }

  /* copy stored node MAC */  
  PRINTF("node-id: Using MAC stored in flash.\n");
  for(i = 0; i < 8; i++) {
    node_mac[i] = *user_mem;
    user_mem += 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
node_id_restore(void)
{
  eui64_t eui;
   /* read out the EUI-64 from the EEPROM on Radio */
  if(at24mac602_init() == -1) {
    fallback();
    goto FINISH_UP;
  }
  if(at24mac602_read_eui64(&eui) == -1) {
    fallback();
    goto FINISH_UP;
  } else {
    int i;

    /* init and read went ok, use EUI-64 for MAC */  
    for(i = 0; i < 8; i++) {
      node_mac[i] = eui.u8[i];
    }
  }

FINISH_UP:
  /* set node ID */
  node_id = node_mac[7] | (node_mac[6] << 2);
  /*printf(" Node MAC is %x.%x.%x.%x.%x.%x.%x.%x.\n", 
    node_mac[0], node_mac[1], node_mac[2], node_mac[3], node_mac[4], node_mac[5],
    node_mac[6], node_mac[7]);
  printf("Node ID is %x\n", node_id);*/
}
/*---------------------------------------------------------------------------*/
void
node_id_burn(unsigned short id)
{
  /* This is not implemented - burning is currently done from the command line */
  PRINTF("node-id: node_id_burn() not implemented. See 'make help'\n");
}
/*---------------------------------------------------------------------------*/
