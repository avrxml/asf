/*****************************************************************************
 *
 * \file
 *
 * \brief CAN Software Stack for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2009 - 2012 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

//_____ I N C L U D E S ________________________________________________________
#include "avr32/io.h"
#include "can.h"
#include "compiler.h"
#include "preprocessor.h"
#include "intc.h"
#include "canif.h"
#include "delay.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ F U N C T I O N S ______________________________________________________

//! Dynamic Mob Allocation State Vector for Channel 0
U32  can_mob_alloc_vector0 = 0;

//! Dynamic Mob Allocation State Vector for Channel 1
U32  can_mob_alloc_vector1 = 0;

//! Output parameters.
static struct
{
  void (*can_msg_callback_channel0) (U8,U8);
  void (*can_msg_callback_channel1) (U8,U8);
} can_lib_params =
{
  .can_msg_callback_channel0         = NULL,
  .can_msg_callback_channel1         = NULL
};

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can0_int_tx_handler(void)
{
  U8 handle;
  handle = CANIF_mob_get_mob_txok(0);
  if (handle != 0x20)
  {
    CANIF_mob_clear_txok_status(0,handle);
    CANIF_mob_clear_status(0,handle); //   and reset MOb status
  }
  can_lib_params.can_msg_callback_channel0(handle,CAN_STATUS_COMPLETED);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can0_int_rx_handler(void)
{
  U8 handle;
  handle = CANIF_mob_get_mob_rxok(0) ;
  if (handle != 0x20)
  {
    CANIF_mob_clear_rxok_status(0,handle);
    CANIF_mob_clear_status(0,handle); //   and reset MOb status
  }
  can_lib_params.can_msg_callback_channel0(handle,CAN_STATUS_COMPLETED);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can0_int_busoff_handler(void)
{
    CANIF_clr_interrupt_status(0);
    can_lib_params.can_msg_callback_channel0(0xFF,CAN_STATUS_BUSOFF);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can0_int_cerr_handler(void)
{
  CANIF_clr_interrupt_status(0);
  can_lib_params.can_msg_callback_channel0(0xFF,CAN_STATUS_ERROR);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can0_int_wakeup_handler(void)
{
  CANIF_clr_interrupt_status(0);
  can_lib_params.can_msg_callback_channel0(0xFF,CAN_STATUS_WAKEUP);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can1_int_tx_handler(void)
{
  U8 handle;
  handle = CANIF_mob_get_mob_txok(1) ;
  if (handle != 0x20)
  {
    CANIF_mob_clear_txok_status(1,handle);
    CANIF_mob_clear_status(1,handle); //   and reset MOb status
  }
  can_lib_params.can_msg_callback_channel1(handle,CAN_STATUS_COMPLETED);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can1_int_rx_handler(void)
{
  U8 handle;
  handle = CANIF_mob_get_mob_rxok(1) ;
  if (handle != 0x20)
  {
    CANIF_mob_clear_rxok_status(1,handle);
    CANIF_mob_clear_status(1,handle); //   and reset MOb status
  }
  can_lib_params.can_msg_callback_channel1(handle,CAN_STATUS_COMPLETED);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can1_int_busoff_handler(void)
{
    CANIF_clr_interrupt_status(1);
    can_lib_params.can_msg_callback_channel1(0xFF,CAN_STATUS_BUSOFF);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can1_int_cerr_handler(void)
{
  CANIF_clr_interrupt_status(1);
  can_lib_params.can_msg_callback_channel1(0xFF,CAN_STATUS_ERROR);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void can1_int_wakeup_handler(void)
{
  CANIF_clr_interrupt_status(1);
  can_lib_params.can_msg_callback_channel1(0xFF,CAN_STATUS_WAKEUP);
}

U8 can_enable_interrupt(U8 ch)
{
  if ((ch > 1))
        return  CAN_CMD_REFUSED;

  if (ch==0)
  {
    INTC_register_interrupt(&can0_int_tx_handler, AVR32_CANIF_TXOK_IRQ_0, CAN0_INT_TX_LEVEL);
    INTC_register_interrupt(&can0_int_rx_handler, AVR32_CANIF_RXOK_IRQ_0, CAN0_INT_RX_LEVEL);
    INTC_register_interrupt(&can0_int_busoff_handler, AVR32_CANIF_BUS_OFF_IRQ_0, CAN0_INT_BOFF_LEVEL);
    INTC_register_interrupt(&can0_int_cerr_handler, AVR32_CANIF_ERROR_IRQ_0, CAN0_INT_ERR_LEVEL);
    INTC_register_interrupt(&can0_int_wakeup_handler, AVR32_CANIF_WAKE_UP_IRQ_0, CAN0_INT_WAKE_UP_LEVEL);
    CANIF_enable_interrupt(ch);
  }
  else if (ch == 1)
  {
    INTC_register_interrupt(&can1_int_tx_handler, AVR32_CANIF_TXOK_IRQ_1, CAN1_INT_TX_LEVEL);
    INTC_register_interrupt(&can1_int_rx_handler, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
    INTC_register_interrupt(&can1_int_busoff_handler, AVR32_CANIF_BUS_OFF_IRQ_1, CAN1_INT_BOFF_LEVEL);
    INTC_register_interrupt(&can1_int_cerr_handler, AVR32_CANIF_ERROR_IRQ_1, CAN1_INT_ERR_LEVEL);
    INTC_register_interrupt(&can1_int_wakeup_handler, AVR32_CANIF_WAKE_UP_IRQ_1, CAN1_INT_WAKE_UP_LEVEL);
    CANIF_enable_interrupt(ch);
  }

  return  CAN_CMD_ACCEPTED;
}

U8 can_init(U8 ch,
            U32 can_msg_ram_add,
            U8 operating_mode,
            void (*can_msg_callback_channel) (U8 handle, U8 event))
{
   if ( ch > 1)
         return  CAN_CMD_REFUSED;

   // Initialize CAN channel
   CANIF_set_reset(ch);
   while(CANIF_channel_enable_status(ch));
   CANIF_clr_reset(ch);

   CANIF_set_ram_add(ch,(unsigned long) can_msg_ram_add);
   if ((CANIF_bit_timing(ch))==0) return (0);
   switch(operating_mode)
   {
    case CANIF_CHANNEL_MODE_NORMAL:
      CANIF_set_channel_mode(ch,0);
      CANIF_clr_overrun_mode(ch);
      break;
    case CANIF_CHANNEL_MODE_LISTENING:
      CANIF_set_channel_mode(ch,1);
      CANIF_set_overrun_mode(ch);
      break;
    case CANIF_CHANNEL_MODE_LOOPBACK:
      CANIF_set_channel_mode(ch,2);
      CANIF_clr_overrun_mode(ch);
      break;
   }
   canif_clear_all_mob(ch,NB_MOB_CHANNEL);
   CANIF_enable(ch);

#if (CAN_AUTOBAUD == true)
#error 'Autobaud feature not supported'
#else
/* 
 * The maximum delay time to wait is the time to transmit 128-bits 
 * (CAN extended frame at baudrate speed configured by the user).
 * - 10x bits number of previous undetected message,
 * - 128x bits MAX length,
 * - 3x bits of interframe.
 */
#define DELAY_HZ         (BAUDRATE_HZ/141.0)   /*Compute Maximum delay time*/
#define DELAY            (1000000 / DELAY_HZ)  /*Compute Delay in µs*/
   delay_us(DELAY);
   if(!CANIF_channel_enable_status(ch)) {
            return CAN_CMD_REFUSED;
   }
#endif

#ifdef CAN_LIB_UNDER_INTERRUPT
   switch(ch)
   {
    case 0:
        can_lib_params.can_msg_callback_channel0     = can_msg_callback_channel;
        break;
    case 1:
        can_lib_params.can_msg_callback_channel1     = can_msg_callback_channel;
        break;
   }
    can_enable_interrupt(ch);
#endif

   return CAN_CMD_ACCEPTED;
}


U8 can_mob_alloc(U8 ch)
{
  if ((ch > 1))
        return  CAN_CMD_REFUSED;

    if(ch==0)
    {
          int i;
          for (i=0;i<NB_MOB_CHANNEL;i++)
          {
            if (!((can_mob_alloc_vector0>>i)&0x01))
            {
              can_mob_alloc_vector0|=(1<<i);
              CANIF_clr_mob(0,i);
              return i;
            }
          }
          return CAN_CMD_REFUSED;
    }
    else{
          int i;
          for (i=0;i<NB_MOB_CHANNEL;i++)
          {
            if (!((can_mob_alloc_vector1>>i)&0x01))
            {
              can_mob_alloc_vector1|=(1<<i);
              CANIF_clr_mob(1,i);
              return i;
            }
          }
          return CAN_CMD_REFUSED;
    }
}

U8 can_mob_free(U8 ch, U8 handle)
{
  if ((ch > 1)||
      (handle > (NB_MOB_CHANNEL-1)))
        return  CAN_CMD_REFUSED;
  switch(ch)
  {
    case 0:
      can_mob_alloc_vector0 &=  (~(1<<handle));
      break;
    case 1:
      can_mob_alloc_vector1 &=  (~(1<<handle));
      break;
  }
  return CAN_CMD_ACCEPTED;
}

U8 can_tx( U8 ch,
           U8 handle,
           U8 dlc,
           U8 req_type,
           const can_msg_t *can_msg)
{
    if ((ch > 1) ||
        (handle > (NB_MOB_CHANNEL-1)) ||
        (dlc > 8))
        return  CAN_CMD_REFUSED;

    if (can_msg->ide_bit){
             CANIF_set_ext_id(ch,
                            handle,
                            can_msg->id);

             CANIF_set_ext_idmask(ch,
                                handle,
                                can_msg->id_mask);
    }
    else {
             CANIF_set_std_id(ch,
                            handle,
                            can_msg->id);
             CANIF_set_std_idmask(ch,
                                handle,
                                can_msg->id_mask);
    }
    CANIF_mob_clr_dlc(ch,handle);
    CANIF_mob_set_dlc(ch,handle,dlc);
    if (req_type == CAN_REMOTE_FRAME){
            CANIF_set_rtr(ch,handle);
            CANIF_set_rtrmask(ch,handle);
            CANIF_mob_set_automode(ch,handle);
    }
    CANIF_set_data(ch,handle,((can_msg_t *)can_msg)->data.u64);
    CANIF_config_tx(ch,handle);
    CANIF_mob_enable(ch,handle);
#ifdef CAN_LIB_UNDER_INTERRUPT
    CANIF_mob_enable_interrupt(ch,handle);
#endif
    return CAN_CMD_ACCEPTED;
}

U8 can_rx( U8 ch,
           U8 handle,
           U8 req_type,
           const can_msg_t *can_msg)
{
    if ((ch > 1) ||
        (handle > (NB_MOB_CHANNEL-1)))
        return  CAN_CMD_REFUSED;
    if (can_msg->ide_bit){
    	CANIF_set_ext_id(ch,
                      handle,
                      can_msg->id);
        CANIF_set_ext_idmask(ch,
                          handle,
                          can_msg->id_mask);
    }
    else {
    	CANIF_set_std_id(ch,
                      handle,
                      can_msg->id);
        CANIF_set_std_idmask(ch,
                          handle,
                          can_msg->id_mask);
    }
    if (req_type == CAN_REMOTE_FRAME){
            CANIF_set_rtr(ch,handle);
            CANIF_set_rtrmask(ch,handle);
            CANIF_mob_set_automode(ch,handle);
            CANIF_set_data(ch,handle,((can_msg_t *)can_msg)->data.u64);
    }
    CANIF_config_rx(ch,handle);
    CANIF_mob_enable(ch,handle);
#ifdef CAN_LIB_UNDER_INTERRUPT
    CANIF_mob_enable_interrupt(ch,handle);
#endif
    return CAN_CMD_ACCEPTED;
}

Union64 can_get_mob_data( U8 ch ,
                           U8 handle)
{
    return ((CANIF_mob_get_ptr_data(ch,handle)->data));
}

U8 can_get_mob_dlc( U8 ch ,
                    U8 handle)
{
    return (CANIF_mob_get_dlc(ch,handle));
}

U32 can_get_mob_id( U8 ch ,
                    U8 handle)
{
    return (CANIF_get_ext_id(ch,handle));
}

U8 can_mob_get_status( U8 ch,
                       U8 handle)
{
    U8 status;
    if ((ch > 1)||
        (handle > (NB_MOB_CHANNEL-1)) )
        return  CAN_CMD_REFUSED;

    status = CANIF_get_interrupt_error_status(ch);
    if (status!=0)
      return CAN_STATUS_ERROR;

    status = CANIF_mob_get_status(ch,handle);
    if ( (status & MOB_RX_COMPLETED) ||
         (status & MOB_TX_COMPLETED) ||
         (status & MOB_RX_COMPLETED_DLCW) )
         return CAN_STATUS_COMPLETED;

    return CAN_STATUS_NOT_COMPLETED;
}

void can_clear_status (U8 ch,
                       U8 mob)
{
  CANIF_mob_clear_status(ch,mob);
}
