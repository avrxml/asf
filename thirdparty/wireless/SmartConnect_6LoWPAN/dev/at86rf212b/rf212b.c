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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "leds.h"
#include "rtimer.h"
#include "netstack.h"
#include "net/packetbuf.h"
#include "rf212b-const.h"
#include "rf212b-config.h"
#include "rf212b-arch.h"
#include "rf212b.h"
#include "trx_access.h"
#include "delay.h"
#include "system_interrupt.h"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


/*---------------------------------------------------------------------------*/
PROCESS(rf212_radio_process, "RF212 radio driver");
static int  on(void);
static int  off(void);
#if !NULLRDC_CONF_802154_AUTOACK_HW
static void radiocore_hard_recovery(void);
#endif
static void rf_generate_random_seed(void);
static void flush_buffer(void);
static uint8_t flag_transmit = 0;
#if NULLRDC_CONF_802154_AUTOACK_HW
static uint8_t ack_status = 0;
#endif
static volatile int radio_is_on = 0;
static volatile int pending_frame = 0;
static volatile int sleep_on = 0;
/*---------------------------------------------------------------------------*/
static int rf212_init(void);
static int rf212_prepare(const void *payload, unsigned short payload_len);
static int rf212_transmit(unsigned short payload_len);
static int rf212_send(const void *data, unsigned short len);
static int rf212_read(void *buf, unsigned short bufsize);
static int rf212_channel_clear(void);
static int rf212_receiving_packet(void);
static int rf212_pending_packet(void);
static int rf212_on(void);
static int rf212_off(void);
int rf212_sleep(void);

const struct radio_driver rf212_radio_driver =
{
  rf212_init,
  rf212_prepare,
  rf212_transmit,
  rf212_send,
  rf212_read,
  rf212_channel_clear,
  rf212_receiving_packet,
  rf212_pending_packet,
  rf212_on,
  rf212_off,
  rf212_sleep,
};
/*---------------------------------------------------------------------------*/
/* convenience macros */
//#define RF212_STATUS()                    rf233_status()
#define RF212_STATUS()                      rf212_status()
//#define RF212_COMMAND(c)                  rf212_arch_write_reg(RF212_REG_TRX_STATE, c)
#define RF212_COMMAND(c)                  trx_reg_write(RF212_REG_TRX_STATE, c)
/* each frame has a footer consisting of LQI, ED, RX_STATUS added by the radio */
#define FOOTER_LEN                        3   /* bytes */
#define MAX_PACKET_LEN                    127 /* bytes, excluding the length (first) byte */

/* when transmitting, time to allow previous transmission to end before drop */
#define PREV_TX_TIMEOUT                   (10 * RTIMER_SECOND/1000)
/*---------------------------------------------------------------------------*/
#define _DEBUG_                 0
#define DEBUG_PRINTDATA       0     /* print frames to/from the radio; requires DEBUG == 1 */
#if _DEBUG_
#define PRINTF(...)       printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define BUSYWAIT_UNTIL(cond, max_time)                                  \
  do {                                                                  \
    rtimer_clock_t t0;                                                  \
    t0 = RTIMER_NOW();                                                  \
    while(!(cond) && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time)));   \
  } while(0)
/*---------------------------------------------------------------------------*/
/**
 * \brief      Get radio channel
 * \return     The radio channel
 */
int
rf_get_channel(void)
{
  //return (int) (rf212_arch_read_reg(RF212_REG_TRX_STATUS) & PHY_CC_CCA_CHANNEL);
  uint8_t channel;
  channel=trx_reg_read(RF212_REG_PHY_CC_CCA) & PHY_CC_CCA_CHANNEL;
  //printf("rf233 channel%d\n",channel);
  return (int)channel;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Set radio channel
 * \param ch   The radio channel
 * \retval -1  Fail: channel number out of bounds
 * \retval 0   Success
 */
int
rf_set_channel(uint8_t ch)
{
  uint8_t temp;
  PRINTF("RF212: setting channel %u\n", ch);
  /*if(ch > 10 || ch < 1) {
    return -1;
  }*/
 // rf212_arch_write_reg(RF212_REG_CC_CTRL_1, 0x00);
  /* read-modify-write to conserve other settings */
  temp = trx_reg_read(RF212_REG_PHY_CC_CCA);
  //temp &=~ PHY_CC_CCA_CHANNEL;
  temp |= ch;
  trx_reg_write(RF212_REG_PHY_CC_CCA, temp);
    return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Get transmission power
 * \return     The transmission power
 */
int
rf212_get_txp(void)
{
  PRINTF("RF212: get txp\n");
  return trx_reg_read(RF212_REG_PHY_TX_PWR_CONF) & PHY_TX_PWR_TXP;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Set transmission power
 * \param txp  The transmission power
 * \retval -1  Fail: transmission power out of bounds
 * \retval 0   Success
 */
int
rf212_set_txp(uint8_t txp)
{
  PRINTF("RF212: setting txp %u\n", txp);
  if(txp > TXP_M17) {
    /* undefined */
    return -1;
  }

  trx_reg_write(RF212_REG_PHY_TX_PWR_CONF, txp);
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Init the radio
 * \return     Returns success/fail
 * \retval 0   Success
 */
static int
rf212_init(void)
{
  volatile uint8_t regtemp;
   uint8_t radio_state;  /* don't optimize this away, it's important */
  //uint8_t temp;
  PRINTF("RF212: init.\n");

  /* init SPI and GPIOs, wake up from sleep/power up. */
  //rf212_arch_init();
  trx_spi_init();
 
  /* reset will put us into TRX_OFF state */
  /* reset the radio core */
  port_pin_set_output_level(AT86RFX_RST_PIN, false);
  delay_cycles_ms(1);
  port_pin_set_output_level(AT86RFX_RST_PIN, true);
  
  port_pin_set_output_level(AT86RFX_SLP_PIN, false); /*wakeup from sleep*/

  /* before enabling interrupts, make sure we have cleared IRQ status */
  regtemp = trx_reg_read(RF212_REG_IRQ_STATUS);
  printf("After wake from sleep\n");
  radio_state = rf212_status();
  printf("After arch read reg: state 0x%04x\n", radio_state);
 
  /* Assign regtemp to regtemp to avoid compiler warnings */
  regtemp = regtemp;
if(radio_state == STATE_P_ON) {
	trx_reg_write(RF212_REG_TRX_STATE, TRXCMD_TRX_OFF);
	}  
  trx_irq_init((FUNC_PTR)rf212_interrupt_poll);
  ENABLE_TRX_IRQ();  
  system_interrupt_enable_global();
  /* Configure the radio using the default values except these. */
  trx_reg_write(RF212_REG_TRX_CTRL_1,      RF212_REG_TRX_CTRL_1_CONF);
  trx_reg_write(RF212_REG_PHY_CC_CCA,      RF212_REG_PHY_CC_CCA_CONF);
  trx_reg_write(RF212_REG_PHY_TX_PWR_CONF, RF212_REG_PHY_TX_PWR_CONF);
  //temp = rf212_arch_read_reg(RF212_REG_TRX_CTRL_2);
  trx_reg_write(RF212_REG_TRX_CTRL_2, RF212_REG_TRX_CTRL_2_CONF);
  trx_reg_write(RF212_REG_IRQ_MASK,        RF212_REG_IRQ_MASK_CONF);
#if HW_CSMA_FRAME_RETRIES
  trx_bit_write(SR_MAX_FRAME_RETRIES, 3);
  trx_bit_write(SR_MAX_CSMA_RETRIES, 4);
#else  
  trx_bit_write(SR_MAX_FRAME_RETRIES, 0);
  trx_bit_write(SR_MAX_CSMA_RETRIES, 7);
#endif  
  SetPanId(IEEE802154_CONF_PANID);
  rf_generate_random_seed();
  /* start the radio process */
  process_start(&rf212_radio_process, NULL);
  return 0;
}

/*
 * \brief Generates a 16-bit random number used as initial seed for srand()
 *
 */
static void rf_generate_random_seed(void)
{
	uint16_t seed = 0;
	uint8_t cur_random_val = 0;

	/*
	 * We need to disable TRX IRQs while generating random values in RX_ON,
	 * we do not want to receive frames at this point of time at all.
	 */
	ENTER_TRX_REGION();

	do 
	{
		trx_reg_write(RF212_REG_TRX_STATE, TRXCMD_TRX_OFF);
		
	} while (TRXCMD_TRX_OFF != rf212_status());

	do
	{
		/* Ensure that PLL has locked and receive mode is reached. */
		trx_reg_write(RF212_REG_TRX_STATE, TRXCMD_PLL_ON);
		
	} while (TRXCMD_PLL_ON != rf212_status());
	do
	{
		trx_reg_write(RF212_REG_TRX_STATE, TRXCMD_RX_ON);
		
	} while (TRXCMD_RX_ON != rf212_status());	

	/* Ensure that register bit RX_PDT_DIS is set to 0. */
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);

	/*
	 * The 16-bit random value is generated from various 2-bit random
	 * values.
	 */
	for (uint8_t i = 0; i < 8; i++) {
		/* Now we can safely read the 2-bit random number. */
		cur_random_val = trx_bit_read(SR_RND_VALUE);
		seed = seed << 2;
		seed |= cur_random_val;
		delay_us(1); /* wait that the random value gets updated */
	}

	do
	{
		/* Ensure that PLL has locked and receive mode is reached. */
		trx_reg_write(RF212_REG_TRX_STATE, TRXCMD_TRX_OFF);		
	} while (TRXCMD_TRX_OFF != rf212_status());
	/*
	 * Now we need to clear potential pending TRX IRQs and
	 * enable the TRX IRQs again.
	 */
	trx_reg_read(RF212_REG_IRQ_STATUS);
	trx_irq_flag_clr();
	LEAVE_TRX_REGION();

	/* Set the seed for the random number generator. */
	srand(seed);
}

/* Put the Radio in sleep mode */

int
rf212_sleep(void)
{
	int status;
	/* Check whether we're already sleeping */
	if (!sleep_on) {
		//printf("\r\n goto sleep %d",rf233_get_channel());
		//delay_ms(1);
		sleep_on = 1;
		/* Turn off the Radio */
		status = rf212_off();
		/* Set the SLP_PIN to high */
		if(status == 0) {
			goto_sleep();
		}
	}
	
	return 0;
	
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      prepare a frame and the radio for immediate transmission 
 * \param payload         Pointer to data to copy/send
 * \param payload_len     length of data to copy
 * \return     Returns success/fail, refer to radio.h for explanation
 */
static int
rf212_prepare(const void *payload, unsigned short payload_len)
{
#if DEBUG_PRINTDATA
  int i;
#endif  /* DEBUG_PRINTDATA */
  uint8_t templen;
  uint8_t radio_status;
  uint8_t data[130];

#if USE_HW_FCS_CHECK
  /* Add length of the FCS (2 bytes) */
  templen = payload_len + 2;
#else   /* USE_HW_FCS_CHECK */
  /* FCS is assumed to already be included in the payload */
  templen = payload_len;
#endif  /* USE_HW_FCS_CHECK */
data[0] = templen;
memcpy(&data[1],payload,templen);
#if DEBUG_PRINTDATA
  PRINTF("RF212 prepare (%u/%u): 0x", payload_len, templen);
  for(i = 0; i < templen; i++) {
    PRINTF("%02x", *(uint8_t *)(payload + i));
  }
  PRINTF("\n");
#endif  /* DEBUG_PRINTDATA */

  PRINTF("RF212: prepare %u\n", payload_len);
  if(payload_len > MAX_PACKET_LEN) {
    PRINTF("RF212: error, frame too large to tx\n");
    return RADIO_TX_ERR;
  }

  /* check that the FIFO is clear to access */
  radio_status = rf212_status();
  #if NULLRDC_CONF_802154_AUTOACK_HW
  if(radio_status == STATE_BUSY_RX_AACK || radio_status == STATE_BUSY_TX_ARET) {
	  PRINTF("RF212B: TRX buffer unavailable: prep when %s\n", radio_status == STATE_BUSY_RX_AACK ? "rx" : "tx");
  #else
   if(radio_status == STATE_BUSY_RX || radio_status == STATE_BUSY_TX) {
	   PRINTF("RF212B: TRX buffer unavailable: prep when %s\n", radio_status == STATE_BUSY_RX? "rx" : "tx");
  #endif
    return RADIO_TX_ERR;
  }

  /* Write packet to TX FIFO. */
  PRINTF("RF212 len = %u\n", payload_len);
  trx_frame_write((uint8_t *)data, templen+1);
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Transmit a frame already put in the radio with 'prepare'
 * \param payload_len    Length of the frame to send
 * \return     Returns success/fail, refer to radio.h for explanation
 */
static int
rf212_transmit(unsigned short payload_len)
{
  uint8_t status_now;//,temp;
  PRINTF("RF212: tx %u\n", payload_len);

  /* prepare for TX */
  status_now = rf212_status();
  #if NULLRDC_CONF_802154_AUTOACK_HW
  if(status_now == STATE_BUSY_RX_AACK || status_now == STATE_BUSY_TX_ARET) {
  #else
  if(status_now == STATE_BUSY_RX || status_now == STATE_BUSY_TX) {
  #endif
    PRINTF("RF212: collision, was receiving\n");
    /* NOTE: to avoid loops */
    return RADIO_TX_ERR;
    // return RADIO_TX_COLLISION;
  }
  if(status_now != STATE_PLL_ON) {
    /* prepare for going to state TX, should take max 80 us */
    //RF212_COMMAND(TRXCMD_PLL_ON);
	trx_reg_write(RF212_REG_TRX_STATE,0x09);
	do 
   {
	   status_now = trx_bit_read(0x01, 0x1F, 0);
   } while (status_now == 0x1f);
    //BUSYWAIT_UNTIL(RF212_STATUS() == STATE_PLL_ON, 1 * RTIMER_SECOND/1000);
  }

  if(rf212_status() != STATE_PLL_ON) {
    /* failed moving into PLL_ON state, gracefully try to recover */
    PRINTF("RF212: failed going to PLLON\n");
    RF212_COMMAND(TRXCMD_PLL_ON);   /* try again */
    if(rf212_status() != STATE_PLL_ON) {
      /* give up and signal big fail (should perhaps reset radio core instead?) */
      PRINTF("RF212: graceful recovery (in tx) failed, giving up. State: 0x%02X\n", RF212_STATUS());
      return RADIO_TX_ERR;
    }
  }

  /* perform transmission */
  ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);
#if NULLRDC_CONF_802154_AUTOACK_HW
  RF212_COMMAND(TRXCMD_TX_ARET_ON);
#endif  
  RF212_COMMAND(TRXCMD_TX_START);
  flag_transmit=1;

#if !NULLRDC_CONF_802154_AUTOACK_HW  
  BUSYWAIT_UNTIL(RF212_STATUS() == STATE_BUSY_TX, RTIMER_SECOND/2000);
  BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_TX, 10 * RTIMER_SECOND/1000);
  #if (DATA_RATE==BPSK_20||BPSK_40||OQPSK_SIN_RC_100)
  BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_TX, 10 * RTIMER_SECOND/1000);
  BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_TX, 10 * RTIMER_SECOND/1000);
  BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_TX, 10 * RTIMER_SECOND/1000);
  #endif
#endif  
  ENERGEST_OFF(ENERGEST_TYPE_TRANSMIT);
  ENERGEST_ON(ENERGEST_TYPE_LISTEN);
#if !NULLRDC_CONF_802154_AUTOACK_HW  
  if(RF212_STATUS() != STATE_PLL_ON) {
    /* something has failed */
    printf("RF212: radio fatal err after tx\n");
    radiocore_hard_recovery();
	return RADIO_TX_ERR;
  }
 // printf("#RTIMER_SECOND");
  RF212_COMMAND(TRXCMD_RX_ON);
#else
	BUSYWAIT_UNTIL(ack_status == 1, 10 * RTIMER_SECOND/1000);
	if((ack_status))
	{
	//	printf("\r\nrf233 sent\r\n ");
		ack_status=0;
	//	printf("\nACK received");
		return RADIO_TX_OK;
	}
	else
	{
	//	printf("\nNOACK received");		
		return RADIO_TX_NOACK;
	}
	
#endif  

 PRINTF("RF212: tx ok\n");
  return RADIO_TX_OK;
}
void
goto_sleep(void)
{
	port_pin_set_output_level(AT86RFX_SLP_PIN, true);
}
void
wake_from_sleep(void)
{
  /* 
   * Triggers a radio state transition - assumes that the radio already is in
   * state SLEEP or DEEP_SLEEP and SLP_TR pin is low. Refer to datasheet 6.6.
   * 
   * Note: this is the only thing that can get the radio from state SLEEP or 
   * state DEEP_SLEEP!
   */
  port_pin_set_output_level(AT86RFX_SLP_PIN, false);
}

/*---------------------------------------------------------------------------*/
/**
 * \brief      Send data: first prepares, then transmits
 * \param payload         Pointer to data to copy/send
 * \param payload_len     length of data to copy
 * \return     Returns success/fail, refer to radio.h for explanation
 */
static int
rf212_send(const void *payload, unsigned short payload_len)
{
  PRINTF("RF212: send %u\n", payload_len);
  if(rf212_prepare(payload, payload_len) == RADIO_TX_ERR) {
    return RADIO_TX_ERR;
  } 
  return rf212_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      read a received frame out of the radio buffer 
 * \param buf         pointer to where to copy received data
 * \param bufsize     Maximum size we can copy into bufsize
 * \return     Returns length of data read (> 0) if successful
 * \retval -1  Failed, was transmitting so FIFO is invalid
 * \retval -2  Failed, rx timed out (stuck in rx?)
 * \retval -3  Failed, too large frame for buffer
 * \retval -4  Failed, CRC/FCS failed (if USE_HW_FCS_CHECK is true)
 */
static int
rf212_read(void *buf, unsigned short bufsize)
{
  
  uint8_t ed;       /* frame metadata */
  uint8_t frame_len = 0;
  uint8_t len = 0;
  int rssi;
#if DEBUG_PRINTDATA
  uint8_t tempreadlen;
#endif  /* DEBUG_PRINTDATA */

  if(pending_frame == 0) {
    return 0;
  }
  pending_frame = 0;

 /* check that data in FIFO is valid */
 //radio_state = RF212_STATUS();
  //if(radio_state == STATE_BUSY_TX) {
    ///* data is invalid, bail out */
    //PRINTF("RF212: read while in BUSY_TX ie invalid, dropping.\n");
    //return -1;
  //}
  //if(radio_state == STATE_BUSY_RX) {
    ///* still receiving - data is invalid, wait for it to finish */
    //PRINTF("RF212: read while BUSY_RX, waiting.\n");
    //BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_RX, 10 * RTIMER_SECOND/1000);
	//#if (DATA_RATE==BPSK_20||BPSK_40||OQPSK_SIN_RC_100)
	//BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_RX, 10 * RTIMER_SECOND/1000);
	//BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_RX, 10 * RTIMER_SECOND/1000);
	//BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_RX, 10 * RTIMER_SECOND/1000);
	//BUSYWAIT_UNTIL(RF212_STATUS() != STATE_BUSY_RX, 10 * RTIMER_SECOND/1000);
	//#endif
    //if(RF212_STATUS() == STATE_BUSY_RX) {
      //PRINTF("RF212: timed out, still BUSY_RX, dropping.\n");
      //return -2;
    //}
  //} 

  /* get length of data in FIFO */
  trx_frame_read(&frame_len, 1);
#if DEBUG_PRINTDATA
  tempreadlen = frame_len;
#endif  /* DEBUG_PRINTDATA */
  if(frame_len == 1) {
    frame_len = 0;
  }

  len = frame_len;
#if USE_HW_FCS_CHECK
  /* FCS has already been stripped */
  len = frame_len - 2;
#endif  /* USE_HW_FCS_CHECK */

  if(frame_len == 0) {
    return 0;
  }
  if(len > bufsize) {
    /* too large frame for the buffer, drop */
    PRINTF("RF212: too large frame for buffer, dropping (%u > %u).\n", frame_len, bufsize);
    flush_buffer();
    return -3;
  }
  PRINTF("RF212 read %u B\n", frame_len);

  /* read out the data into the buffer, disregarding the length and metadata bytes */
    trx_sram_read(1,(uint8_t *)buf, len);
#if DEBUG_PRINTDATA
  {
    int k;
    PRINTF("RF212: Read frame (%u/%u): ", tempreadlen, frame_len);
    for(k = 0; k < frame_len; k++) {
      PRINTF("%02x", *((uint8_t *)buf + k));
    }
    PRINTF("\n");
  }
#endif  /* DEBUG_PRINTDATA */

  /* 
   * Energy level during reception, ranges from 0x00 to 0x53 (=83d) with a
   * resolution of 1dB and accuracy of +/- 5dB. 0xFF means invalid measurement.
   * 0x00 means <= RSSI(base_val), which is -91dBm (typ). See datasheet 12.7.
   * Ergo, real RSSI is (ed-91) dBm or less.
   */
  #define RSSI_OFFSET       (91)
  ed = trx_reg_read(RF212_REG_PHY_ED_LEVEL);
  rssi = (int) ed - RSSI_OFFSET;
  packetbuf_set_attr(PACKETBUF_ATTR_RSSI, rssi);
  flush_buffer();

//#if USE_HW_FCS_CHECK
  //{
    //uint8_t crc_ok;   /* frame metadata */
    //crc_ok = rf212_arch_read_reg(RF212_REG_PHY_RSSI) & PHY_RSSI_CRC_VALID;
    //if(crc_ok == 0) {
      ///* CRC/FCS fail, drop */
      //PRINTF("RF212: CRC/FCS fail, dropping.\n");
      //flush_buffer();
      //return -4;
    //}
  //}
//#endif  /* USE_HW_FCS_CHECK */ 

  return len;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      perform a clear channel assessment 
 * \retval >0  Channel is clear
 * \retval 0   Channel is not clear
 */
static int
rf212_channel_clear(void)
{
  uint8_t regsave;
  int was_off = 0;
  
  if(RF212_STATUS() != STATE_RX_ON) {
    /* CCA can only be performed in RX state */
    was_off = 1;
    RF212_COMMAND(TRXCMD_RX_ON);
  }

  /* request a CCA, storing the channel number (set with the same reg) */
  regsave = trx_reg_read(RF212_REG_PHY_CC_CCA);
  regsave |= PHY_CC_CCA_DO_CCA | PHY_CC_CCA_MODE_CS_OR_ED;
  trx_reg_write(RF212_REG_PHY_CC_CCA, regsave);
  
  BUSYWAIT_UNTIL(trx_reg_read(RF212_REG_TRX_STATUS) & TRX_CCA_DONE,
      RTIMER_SECOND / 1000);
  regsave = trx_reg_read(RF212_REG_TRX_STATUS);

  /* return to previous state */
  if(was_off) {
    RF212_COMMAND(TRXCMD_TRX_OFF);
  }
  #if NULLRDC_CONF_802154_AUTOACK_HW 
  else{
	  RF212_COMMAND(TRXCMD_RX_AACK_ON);
  }
  #endif
  /* check CCA */
  if((regsave & TRX_CCA_DONE) && (regsave & TRX_CCA_STATUS)) {
    PRINTF("RF212: CCA 1\n");
    return 1;
  }
  PRINTF("RF212: CCA 0\n");
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      check whether we are currently receiving a frame 
 * \retval >0  we are currently receiving a frame 
 * \retval 0   we are not currently receiving a frame 
 */
static int
rf212_receiving_packet(void)
{
  uint8_t trx_state;
  trx_state=rf212_status();
  #if NULLRDC_CONF_802154_AUTOACK_HW
  if(trx_state == STATE_BUSY_RX_AACK) {
  #else 
  if(trx_state == STATE_BUSY_RX) {
  #endif
  
    PRINTF("RF22B: Receiving frame\n");
    return 1;
  }
  PRINTF("RF212B: not Receiving frame\n");
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      check whether we have a frame awaiting processing 
 * \retval >0  we have a frame awaiting processing 
 * \retval 0   we have not a frame awaiting processing 
 */
static int
rf212_pending_packet(void)
{
  PRINTF("RF212: Frame %spending\n", pending_frame ? "" : "not ");
  return pending_frame;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      switch the radio on to listen (rx) mode 
 * \retval 0   Success
 */
static int
rf212_on(void)
{
  PRINTF("RF212: on\n");
  on();
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      switch the radio off 
 * \retval 0   Success
 */
static int
rf212_off(void)
{
  PRINTF("RF212: off\n");
  off();
  return 0;
}


void SetIEEEAddr(uint8_t *ieee_addr)
{
	uint8_t *ptr_to_reg = ieee_addr;
	//for (uint8_t i = 0; i < 8; i++) {
	trx_reg_write((0x2b), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x2a), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x29), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x28), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x27), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x26), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x25), *ptr_to_reg);
	ptr_to_reg++;
	trx_reg_write((0x24), *ptr_to_reg);
	ptr_to_reg++;
	//}
}
void SetPanId(uint16_t panId)
{
	uint8_t *d = (uint8_t *)&panId;

	trx_reg_write(0x22, d[0]);
	trx_reg_write(0x23, d[1]);
}
void SetShortAddr(uint16_t addr)
{
	uint8_t *d = (uint8_t *)&addr;
	trx_reg_write(0x20, d[0]);
	trx_reg_write(0x21, d[1]);
}

/*---------------------------------------------------------------------------*/
/* switch the radio on */
static int
on(void)
{
  uint8_t state_now = RF212_STATUS();
  if(state_now != STATE_PLL_ON && state_now != STATE_TRX_OFF 
#if NULLRDC_CONF_802154_AUTOACK_HW
  && state_now != STATE_TX_ARET_ON
#endif
  ) {
    /* fail, we need the radio transceiver to be in either of those states */
    return -1;
  }

  /* go to RX_ON state */
  ENERGEST_ON(ENERGEST_TYPE_LISTEN);
  #if NULLRDC_CONF_802154_AUTOACK_HW
  RF212_COMMAND(TRXCMD_RX_AACK_ON);
  #else
  RF212_COMMAND(TRXCMD_RX_ON); 
  #endif
  radio_is_on = 1;
  return 0;
}
/*---------------------------------------------------------------------------*/
/* switch the radio off */
static int
off(void)
{
  #if NULLRDC_CONF_802154_AUTOACK_HW
  if(rf212_status() != STATE_RX_AACK_ON ) {
  #else
  if(rf212_status() != STATE_RX_ON) {
  #endif
    /* fail, we need the radio transceiver to be in this state */
    return -1;
  }

  /* turn off the radio transceiver */
  ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  RF212_COMMAND(TRXCMD_TRX_OFF);
  radio_is_on = 0;
  return 0;
}
/*---------------------------------------------------------------------------*/
/* used for indicating that the interrupt wasn't able to read SPI and must be serviced */
static volatile int interrupt_callback_wants_poll = 0;
/* used as a blocking semaphore to indicate that we are currently servicing an interrupt */
static volatile int interrupt_callback_in_progress = 0;

/**
 * \brief      Radio RF212 process, infinitely awaits a poll, then checks radio
 *             state and handles received data.
 */
PROCESS_THREAD(rf212_radio_process, ev, data)
{
  int len;
  PROCESS_BEGIN();
  PRINTF("RF212: started.\n");

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
    PRINTF("RF212: polled.\n");

    if(interrupt_callback_wants_poll) {
      rf212_interrupt_poll();
    }

    packetbuf_clear();
    // packetbuf_set_attr(PACKETBUF_ATTR_TIMESTAMP, last_packet_timestamp);
    len = rf212_read(packetbuf_dataptr(), PACKETBUF_SIZE);
    if(len > 0) {
      packetbuf_set_datalen(len);
      NETSTACK_RDC.input();
    } else {
      PRINTF("RF212: error while reading: %d\n", len);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      RF212 radio process poll function, to be called from the
 *             interrupt handler to poll the radio process
 * \retval 0   success
 */
int
rf212_interrupt_poll(void)
{
  volatile uint8_t irq_source;
  irq_source = trx_reg_read(RF212_REG_IRQ_STATUS);
	 if(irq_source & IRQ_TRX_DONE) {
		 
		 if(flag_transmit==1)
		 {
			 flag_transmit=0;
			 interrupt_callback_in_progress = 0;
			 #if NULLRDC_CONF_802154_AUTOACK_HW
			//printf("Status %x",trx_reg_read(RF233_REG_TRX_STATE) & TRX_STATE_TRAC_STATUS);
			if(!(trx_reg_read(RF212_REG_TRX_STATE) & TRX_STATE_TRAC_STATUS))
			ack_status = 1;
			 RF212_COMMAND(TRXCMD_RX_AACK_ON);
			 #endif			 
			 return 0;
		 }
  if(interrupt_callback_in_progress) {
    /* we cannot read out info from radio now, return here later (through a poll) */
    interrupt_callback_wants_poll = 1;
    process_poll(&rf212_radio_process);
    PRINTF("RF212: irq but busy, returns later.\n");
    return 0;
  }

  interrupt_callback_wants_poll = 0;
  interrupt_callback_in_progress = 1;

  
    /* we have started receiving a frame, len can be read */
    pending_frame = 1;
    process_poll(&rf212_radio_process);
  }

#if 0
  /* Note, these are not currently in use but here for completeness. */
  if(irq_source & IRQ_TRX_DONE) {
    /* End of transmitted or received frame.  */
  }
  if(irq_source & IRQ_TRXBUF_ACCESS_VIOLATION) {
    /* 
     * Access violation on the shared TX/RX FIFO. Possible causes:
     *  - buffer underrun while transmitting, ie not enough data in FIFO to tx
     *  - reading too fast from FIFO during rx, ie not enough data received yet
     *  - haven't read last rxed frame when next rx starts, but first is possible
     *    to read out, with possible corruption - check FCS
     *  - writing frames larger than 127 B to FIFO (len byte)
     */
    PRINTF("RF212-arch: access violation.\n");
  }
  if(irq_source & IRQ_BAT_LOW) {
    /* Battery low */
  }
  if(irq_source & IRQ_RX_ADDRESS_MATCH) {
    /* receiving frame address match */
  }
  if(irq_source & IRQ_CCA_ED_DONE) {
    /* CCA/ED done */
  }
  if(irq_source & IRQ_PLL_UNLOCK) {
    /* PLL unlock */
  }
  if(irq_source & IRQ_PLL_LOCK) {
    /* PLL lock */
  }
#endif

  interrupt_callback_in_progress = 0;
  return 0;
}

#if !NULLRDC_CONF_802154_AUTOACK_HW
/*---------------------------------------------------------------------------*/
/* 
 * Hard, brute reset of radio core and re-init due to it being in unknown,
 * unexpected, or locked state from which we cannot recover in the usual places.
 * Does a full reset and re-init.
 */
static void
radiocore_hard_recovery(void)
{ 
	printf("hardcore recovery\n");//
  rf212_init();
}
#endif
/*---------------------------------------------------------------------------*/
/* 
 * Crude way of flushing the Tx/Rx FIFO: write the first byte as 0, indicating
 * a zero-length frame in the buffer. This is interpreted by the driver as an
 * empty buffer.
 */
static void
flush_buffer(void)
{
  /* NB: tentative untested implementation */
  uint8_t temp = 0;
  trx_frame_write(&temp, 1);
}
uint8_t rf212_status()
{
	return (trx_reg_read(RF212_REG_TRX_STATUS) & TRX_STATUS);
}
/*---------------------------------------------------------------------------*/
