/**
* Copyright (c) 2015 Atmel Corporation and 2012 – 2013, Thingsquare, http://www.thingsquare.com/. All rights reserved. 
*  
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution.
* 
* 3. Neither the name of Atmel nor the name of Thingsquare nor the names of its contributors may be used to endorse or promote products derived 
* from this software without specific prior written permission.  
* 
* 4. This software may only be redistributed and used in connection with an 
* Atmel microcontroller or Atmel wireless product.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* 
* 
*/

#include "contiki.h"
#include "ksz8851snl.h"
#include "ksz8851snl_reg.h"
#include "ksz8851snl-ip64-driver.h"
#include "board.h"
#include "compiler.h"
#include "port.h"
#include "spi.h"
#include "delay.h"
#include "ip64.h"
#include "ip64-eth.h"

#include "net/rime/rime.h"

#include <string.h>
#include <stdio.h>

PROCESS(ksz8851snl_ip64_driver_process, "KSZ8851SNL IP64 driver");

/*---------------------------------------------------------------------------*/
static void
init(void)
{
  uint8_t eui64[8];
  uint8_t macaddr[6];

  /* Assume that linkaddr_node_addr holds the EUI64 of this device. */
  memcpy(eui64, &linkaddr_node_addr, sizeof(eui64));

  /* Mangle the EUI64 into a 48-bit Ethernet address. */
  memcpy(&macaddr[0], &eui64[0], 3);
  memcpy(&macaddr[3], &eui64[5], 3);

  /* In case the OUI happens to contain a broadcast bit, we mask that
     out here. */
  macaddr[0] = (macaddr[0] & 0xfe);

  /* Set the U/L bit, in order to create a locally administered MAC address */
  macaddr[0] = (macaddr[0] | 0x02);

  memcpy(ip64_eth_addr.addr, macaddr, sizeof(macaddr));

  printf("Ethernet Address %02x:%02x:%02x:%02x:%02x:%02x\n", 
         macaddr[0], macaddr[1], macaddr[2],
         macaddr[3], macaddr[4], macaddr[5]);
  ksz8851snl_init();
  /* Write MAc address in ksz8851snl registers */
  //
  ksz8851_reg_write(REG_MAC_ADDR_0, (macaddr[4] << 8) | macaddr[5]);
  ksz8851_reg_write(REG_MAC_ADDR_2, (macaddr[2] << 8) | macaddr[3]);
  ksz8851_reg_write(REG_MAC_ADDR_4, (macaddr[0] << 8) | macaddr[1]);
   
  process_start(&ksz8851snl_ip64_driver_process, NULL);
}

#if 0
int
ksz8851snl_send(const uint8_t *data, uint16_t datalen)
{
  int txmir;
  int i = 0;
  printf("ksz8851snl_send %p %d\n", data, datalen);
  for(i = 0; i < datalen-1 && data[i] != NULL; i++)
  {
	  printf("%x",data[i]);
  }
  //printf("ksz8851snl_send %p %d\n", data, datalen);
  /* TX step1: check if TXQ memory size is available for transmit. */
  txmir = ksz8851_reg_read(REG_TX_MEM_INFO) & TX_MEM_AVAILABLE_MASK;
  if (txmir < datalen + 8) {
    printf("ksz8851snl_update: TX not enough memory in queue: %d required %d\n",
           txmir, datalen + 8);
    return -1;
  }

  /* TX step2: disable all interrupts. */
  ksz8851_reg_write(REG_INT_MASK, 0);

  /*  printf("ksz8851snl_update: TX start packet transmit len %d\n",
      datalen);*/

  /* TX step3: enable TXQ write access. */
  ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_START);

  /* TX step4-8: perform FIFO write operation. */
  ksz8851_fifo_write_begin(datalen);
  ksz8851_fifo_write((uint8_t*)data, datalen);

  /* TX step9-10: pad with dummy data to keep dword alignment. */
  ksz8851_fifo_write_end(datalen & 3);

  /* TX step12: disable TXQ write access. */
  ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);

  /* TX step12.1: enqueue frame in TXQ. */
  ksz8851_reg_setbits(REG_TXQ_CMD, TXQ_ENQUEUE);

  /* RX step13: enable INT_RX flag. */
  ksz8851_reg_write(REG_INT_MASK, INT_RX);

  return datalen;
}

static int pending_frame = 0;

int
ksz8851snl_read(uint8_t *buffer, uint16_t bufsize)
{
  int len, status;

  if (0 == pending_frame) {
    /* RX step1: read interrupt status for INT_RX flag. */
    status = ksz8851_reg_read(REG_INT_STATUS);
    if (!(status & INT_RX)) {
      return 0;
    }

    /* RX step2: disable all interrupts. */
    ksz8851_reg_write(REG_INT_MASK, 0);

    /* RX step3: clear INT_RX flag. */
    ksz8851_reg_setbits(REG_INT_STATUS, INT_RX);

    /* RX step4-5: check for received frames. */
    pending_frame = ksz8851_reg_read(REG_RX_FRAME_CNT_THRES) >> 8;
    if (0 == pending_frame) {
      /* RX step24: enable INT_RX flag. */
      ksz8851_reg_write(REG_INT_MASK, INT_RX);
      return 0;
    }
  }

  /*  printf("pending_frame %d\n", pending_frame);*/
  if(pending_frame > 0) {

    /* RX step6: get RX packet status. */
    status = ksz8851_reg_read(REG_RX_FHR_STATUS);
    if (((status & RX_VALID) == 0) || (status & RX_ERRORS)) {
      ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_CMD_FREE_PACKET);
      pending_frame -= 1;
      printf("ksz8851snl_update: RX packet error!\n");
    } else {
      /* RX step7: read frame length. */
      len = ksz8851_reg_read(REG_RX_FHR_BYTE_CNT) & RX_BYTE_CNT_MASK;

      /* RX step8: Drop packet if len is invalid or no descriptor available. */
      if (0 == len) {
        ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_CMD_FREE_PACKET);
        pending_frame -= 1;
        printf("ksz8851snl_update: RX bad len!\n");
      } else {
           //printf("ksz8851snl_update: RX start packet receive len=%d\n",len);

        /* RX step9: reset RX frame pointer. */
        ksz8851_reg_clrbits(REG_RX_ADDR_PTR, ADDR_PTR_MASK);

        /* RX step10: start RXQ read access. */
        ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_START);

        /* Remove CRC and update pbuf length. */
        len -= 4;

        if(len > bufsize) {
          len = bufsize;
        }

        /* RX step11-17: start FIFO read operation. */
        ksz8851_fifo_read(buffer, len);


        /* RX step21: end RXQ read access. */
        ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);

        /* RX step22-23: update frame count to be read. */
        pending_frame -= 1;

        /* RX step24: enable INT_RX flag if transfer complete. */
        if (0 == pending_frame) {
          ksz8851_reg_write(REG_INT_MASK, INT_RX);
        }

        return len;
      }
    }
  }
  return 0;
}
#endif

/*---------------------------------------------------------------------------*/
static int
output(uint8_t *packet, uint16_t len)
{
  ksz8851snl_send(packet, len);
  return len;
}
int
ksz8851snl_send(const uint8_t *data, uint16_t datalen)
{
  int txmir;

  //printf("ksz8851snl_send %p %d\n", data, datalen);
  /* TX step1: check if TXQ memory size is available for transmit. */
  txmir = ksz8851_reg_read(REG_TX_MEM_INFO) & TX_MEM_AVAILABLE_MASK;
  if (txmir < datalen + 8) {
    printf("ksz8851snl_update: TX not enough memory in queue: %d required %d\n",
           txmir, datalen + 8);
    return -1;
  }

  /* TX step2: disable all interrupts. */
  ksz8851_reg_write(REG_INT_MASK, 0);

  /*  printf("ksz8851snl_update: TX start packet transmit len %d\n",
      datalen);*/

  /* TX step3: enable TXQ write access. */
  ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_START);

  /* TX step4-8: perform FIFO write operation. */
  ksz8851_fifo_write_begin(datalen);
  ksz8851_fifo_write((uint8_t*)data, datalen);

  /* TX step9-10: pad with dummy data to keep dword alignment. */
  ksz8851_fifo_write_end(datalen & 3);

  /* TX step12: disable TXQ write access. */
  ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);

  /* TX step12.1: enqueue frame in TXQ. */
  ksz8851_reg_setbits(REG_TXQ_CMD, TXQ_ENQUEUE);

  /* RX step13: enable INT_RX flag. */
  ksz8851_reg_write(REG_INT_MASK, INT_RX);

  return datalen;
}

static int pending_frame = 0;

int
ksz8851snl_read(uint8_t *buffer, uint16_t bufsize)
{
  int len, status;

  if (0 == pending_frame) {
    /* RX step1: read interrupt status for INT_RX flag. */
    status = ksz8851_reg_read(REG_INT_STATUS);
    if (!(status & INT_RX)) {
      return 0;
    }

    /* RX step2: disable all interrupts. */
    ksz8851_reg_write(REG_INT_MASK, 0);

    /* RX step3: clear INT_RX flag. */
    ksz8851_reg_setbits(REG_INT_STATUS, INT_RX);

    /* RX step4-5: check for received frames. */
    pending_frame = ksz8851_reg_read(REG_RX_FRAME_CNT_THRES) >> 8;
    if (0 == pending_frame) {
      /* RX step24: enable INT_RX flag. */
      ksz8851_reg_write(REG_INT_MASK, INT_RX);
      return 0;
    }
  }

  /*  printf("pending_frame %d\n", pending_frame);*/
  if(pending_frame > 0) {

    /* RX step6: get RX packet status. */
    status = ksz8851_reg_read(REG_RX_FHR_STATUS);
    if (((status & RX_VALID) == 0) || (status & RX_ERRORS)) {
      ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_CMD_FREE_PACKET);
      pending_frame -= 1;
      printf("ksz8851snl_update: RX packet error!\n");
    } else {
      /* RX step7: read frame length. */
      len = ksz8851_reg_read(REG_RX_FHR_BYTE_CNT) & RX_BYTE_CNT_MASK;

      /* RX step8: Drop packet if len is invalid or no descriptor available. */
      if (0 == len) {
        ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_CMD_FREE_PACKET);
        pending_frame -= 1;
        printf("ksz8851snl_update: RX bad len!\n");
      } else {
        //printf("ksz8851snl_update: RX start packet receive len=%d\n",len);
        /* RX step9: reset RX frame pointer. */
        ksz8851_reg_clrbits(REG_RX_ADDR_PTR, ADDR_PTR_MASK);

        /* RX step10: start RXQ read access. */
        ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_START);

        /* Remove CRC and update pbuf length. */
        len -= 4;

        if(len > bufsize) {
          len = bufsize;
        }

        /* RX step11-17: start FIFO read operation. */
        ksz8851_fifo_read(buffer, len);


        /* RX step21: end RXQ read access. */
        ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);

        /* RX step22-23: update frame count to be read. */
        pending_frame -= 1;

        /* RX step24: enable INT_RX flag if transfer complete. */
        if (0 == pending_frame) {
          ksz8851_reg_write(REG_INT_MASK, INT_RX);
        }

        return len;
      }
    }
  }
  return 0;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ksz8851snl_ip64_driver_process, ev, data)
{
  static int len;
  static struct etimer e;
  PROCESS_BEGIN();

  while(1) {
    etimer_set(&e, 1);
    PROCESS_WAIT_EVENT();
    len = ksz8851snl_read(ip64_packet_buffer, ip64_packet_buffer_maxlen);
    if(len > 0) {
      IP64_INPUT(ip64_packet_buffer, len);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
const struct ip64_driver ksz8851snl_ip64_driver = {
  init,
  output
};
/*---------------------------------------------------------------------------*/
