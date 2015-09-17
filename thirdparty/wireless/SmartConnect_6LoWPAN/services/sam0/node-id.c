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

#define SAMR21_ADDR0 (uint16_t *)0x0080A00C
#define SAMR21_ADDR1 (uint16_t *)0x0080A040
#define SAMR21_ADDR2 (uint16_t *)0x0080A044
#define SAMR21_ADDR3 (uint16_t *)0x0080A048

/* If device ID is stored in flash */
#define NODEID_BASEADDR              (0x0003FF80)
static const uint8_t magic_bytes[] = {0xde, 0xad, 0xbe, 0xef};
/*---------------------------------------------------------------------------*/
void
node_id_restore(void)
{
  if(node_id != 0) {
    node_mac[6] = node_id >> 8;
    node_mac[7] = node_id & 0xff;;
    return;
  }
  /* set node ID */
  node_id = node_mac[7] | (node_mac[6] << 2);

  node_mac[0] = (*SAMR21_ADDR0) >> 24;
  node_mac[1] = (*SAMR21_ADDR0) >> 16;
  node_mac[2] = (*SAMR21_ADDR0) >> 8;
  node_mac[3] = (*SAMR21_ADDR0) & 0xff;
  node_mac[4] = (*SAMR21_ADDR1) >> 24;
  node_mac[5] = (*SAMR21_ADDR1) >> 16;
  node_mac[6] = (*SAMR21_ADDR1) >> 8;
  node_mac[7] = (*SAMR21_ADDR1) & 0xff;
  PRINTF("Node MAC is %u.%u.%u.%u.%u.%u.%u.%u.\n", 
    node_mac[0], node_mac[1], node_mac[2], node_mac[3], node_mac[4], node_mac[5],
    node_mac[6], node_mac[7]);
  PRINTF("Node ID is %u\n", node_id);
}
/*---------------------------------------------------------------------------*/
void
node_id_burn(unsigned short id)
{
  /* This is not implemented - burning is currently done from the command line */
  PRINTF("node-id: node_id_burn() not implemented. See 'make help'\n");
}
/*---------------------------------------------------------------------------*/
