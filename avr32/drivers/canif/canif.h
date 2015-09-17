/*****************************************************************************
 *
 * \file
 *
 * \brief CANIF for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _CANIF_H_
#define _CANIF_H_

/**
 * \defgroup group_avr32_drivers_canif CANIF - Control Area Network Interface
 *
 * CANIF is a 32-bit interface for CAN channels.
 *
 * \{
 */

//_____ I N C L U D E S ________________________________________________________
#include "avr32/io.h"
#include "compiler.h"
#include "preprocessor.h"
#include "conf_can.h"

//_____ D E F I N I T I O N S __________________________________________________

//  These defines are missing from or wrong in the toolchain header file
#ifndef AVR32_PM_AWEN_CANIF0WEN_MASK
// Optional #undef AVR32_PM_AWEN_CANIF0WEN_MASK if the define values is wrong.
#define AVR32_PM_AWEN_CANIF0WEN_MASK  0x02
#endif

//  These defines are missing from or wrong in the toolchain header file
#ifndef AVR32_PM_AWEN_CANIF1WEN_MASK
// Optional #undef AVR32_PM_AWEN_CANIF1WEN_MASK if the define values is wrong.
#define AVR32_PM_AWEN_CANIF1WEN_MASK  0x04
#endif

typedef struct
{
  union{
    struct{
      U32   id          : 32;
      U32   id_mask     : 32;
    };
    struct{
      U32                   : 1;
      U32   rtr_bit         : 1;
      U32   ide_bit         : 1;
      U32   id_bit          : 29;
      U32                   : 1;
      U32   rtr_mask_bit    : 1;
      U32   ide_mask_bit    : 1;
      U32   id_mask_bit     : 29;
    };
  };
 Union64 data;
} can_msg_t;


#if (CAN_AUTOBAUD == true)
#error 'Autobaud feature not supported'
#else
#define CANIF_bit_timing(ch)  (canif_fixed_baudrate(ch))
#endif


#define BRP_MIN     1       //! Prescaler of FOSC (TQ generation)
#define BRP_MAX     64
#define NTQ_MIN     8       //! Number of TQ in one CAN bit
#define NTQ_MAX     25
#define PRS_MIN     1       //! Propagation segment
#define PRS_MAX     8
#define PHS1_MIN    2       //! Phase segment 1
#define PHS1_MAX    8
#define PHS2_MIN    2       //! Phase segment 2
#define PHS2_MAX    8
#define SJW_MIN     1       //! Synchro jump width
#define SJW_MAX     4

#define STATUS_CLEARED          0x00
#define MOB_NOT_COMPLETED       0x00   //! MOB Status
#define MOB_TX_COMPLETED        (AVR32_CANIF_MOBSR_TXOK_MASK)
#define MOB_RX_COMPLETED        (AVR32_CANIF_MOBSR_RXOK_MASK)
#define MOB_RX_COMPLETED_DLCW   ((AVR32_CANIF_MOBSR_RXOK_MASK)|(AVR32_CANIF_MOBSR_DLCW_MASK))
#define MOB_ACK_ERROR           (AVR32_CANIF_AERR_MASK)
#define MOB_FORM_ERROR          (AVR32_CANIF_FERR_MASK)
#define MOB_CRC_ERROR           (AVR32_CANIF_CERR_MASK)
#define MOB_STUFF_ERROR         (AVR32_CANIF_SERR_MASK)
#define MOB_BIT_ERROR           (AVR32_CANIF_BERR_MASK)
#define MOB_NOT_REACHED         ((AVR32_CANIF_AERR_MASK)| \
                                 (AVR32_CANIF_FERR_MASK)| \
                                 (AVR32_CANIF_CERR_MASK)| \
                                 (AVR32_CANIF_SERR_MASK)| \
                                 (AVR32_CANIF_BERR_MASK))
#define MOB_DISABLE             0xFF
#define NO_MOB                  0xff
#define DATA                    0
#define REMOTE                  1
#define STD                     0
#define EXTD                    1
#define RTR_BIT                 30
#define IDE_BIT                 29


//_____ M A C R O S ____________________________________________________________



//! CANRAMB Register Access
#define CANIF_get_ram_add(ch)		         ( AVR32_CANIF.channel[ch].canramb )
#define CANIF_set_ram_add(ch,add)		     { AVR32_CANIF.channel[ch].canramb = add; }

//! CANCTRL Register Access
#define CANIF_set_reset(ch)     		     { AVR32_CANIF.channel[ch].canctrl = (1<<AVR32_CANIF_CANCTRL_INIT_OFFSET); }
#define CANIF_clr_reset(ch)     		     { AVR32_CANIF.channel[ch].canctrl = (0<<AVR32_CANIF_CANCTRL_INIT_OFFSET); }
#define CANIF_enable(ch)    		         { AVR32_CANIF.channel[ch].canctrl |= (1<<AVR32_CANIF_CANCTRL_CEN_OFFSET); }
#define CANIF_disable(ch)   		         { AVR32_CANIF.channel[ch].canctrl &= ~(1<<AVR32_CANIF_CANCTRL_CEN_OFFSET); }
#define CANIF_full_abort(ch)                 { CANIF_disable(ch) }
#define CANIF_send_overload(ch)              { AVR32_CANIF.channel[ch].canctrl |= (1<<AVR32_CANIF_CANCTRL_OVRQ_OFFSET); }
#define CANIF_enable_wakeup(ch)              { AVR32_CANIF.channel[ch].canctrl |= (1<<AVR32_CANIF_CANCTRL_WKEN_OFFSET); }
#define CANIF_disable_wakeup(ch)             { AVR32_CANIF.channel[ch].canctrl &= ~(1<<AVR32_CANIF_CANCTRL_WKEN_OFFSET); }

//! CANIFSR Register Access
#define CANIF_channel_enable_status(ch)      ( (AVR32_CANIF.channel[ch].cansr&AVR32_CANIF_CANSR_CES_MASK) >> AVR32_CANIF_CANSR_CES_OFFSET )
#define CANIF_channel_overload_status(ch)    ( (AVR32_CANIF.channel[ch].cansr&AVR32_CANIF_CANSR_OVS_MASK) >> AVR32_CANIF_CANSR_OVS_OFFSET )
#define CANIF_channel_receive_status(ch)     ( (AVR32_CANIF.channel[ch].cansr&AVR32_CANIF_CANSR_RS_MASK) >> AVR32_CANIF_CANSR_RS_OFFSET )
#define CANIF_channel_transmit_status(ch)    ( (AVR32_CANIF.channel[ch].cansr&AVR32_CANIF_CANSR_TS_MASK) >> AVR32_CANIF_CANSR_TS_OFFSET )

//! CANIFCFG Register Access
#define CANIF_CHANNEL_MODE_NORMAL            0
#define CANIF_CHANNEL_MODE_LISTENING         1
#define CANIF_CHANNEL_MODE_LOOPBACK          2
#define CANIF_get_channel_mode(ch)     	     ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_CMODE_MASK)>> AVR32_CANIF_CANCFG_CMODE_OFFSET) )
#define CANIF_set_channel_mode(ch,mode)      {  AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_CMODE_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (mode<<AVR32_CANIF_CANCFG_CMODE_OFFSET); }
#define CANIF_get_overrun_mode(ch)     	     ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_OVRM_MASK)>> AVR32_CANIF_CANCFG_OVRM_OFFSET) )
#define CANIF_set_overrun_mode(ch)           { AVR32_CANIF.channel[ch].cancfg |= (1<<AVR32_CANIF_CANCFG_OVRM_OFFSET); }
#define CANIF_clr_overrun_mode(ch)           { AVR32_CANIF.channel[ch].cancfg &= ~(1<<AVR32_CANIF_CANCFG_OVRM_OFFSET); }
#define CANIF_get_phs1(ch)     		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_PHS1_MASK)>> AVR32_CANIF_CANCFG_PHS1_OFFSET) )
#define CANIF_set_phs1(ch,phs1)              { AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_PHS1_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (phs1<<AVR32_CANIF_CANCFG_PHS1_OFFSET); }
#define CANIF_get_phs2(ch)     		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_PHS2_MASK)>> AVR32_CANIF_CANCFG_PHS2_OFFSET) )
#define CANIF_set_phs2(ch,phs2)              { AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_PHS2_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (phs2<<AVR32_CANIF_CANCFG_PHS2_OFFSET); }
#define CANIF_get_pres(ch)     		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_PRES_MASK)>> AVR32_CANIF_CANCFG_PRES_OFFSET) )
#define CANIF_set_pres(ch,pres)              { AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_PRES_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (pres<<AVR32_CANIF_CANCFG_PRES_OFFSET); }
#define CANIF_get_prs(ch)     		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_PRS_MASK)>> AVR32_CANIF_CANCFG_PRS_OFFSET) )
#define CANIF_set_prs(ch,prs)                { AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_PRS_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (prs<<AVR32_CANIF_CANCFG_PRS_OFFSET); }
#define CANIF_get_sjw(ch)     		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_SJW_MASK)>> AVR32_CANIF_CANCFG_SJW_OFFSET) )
#define CANIF_set_sjw(ch,sjw)                { AVR32_CANIF.channel[ch].cancfg &= ~(AVR32_CANIF_CANCFG_SJW_MASK); \
                                             AVR32_CANIF.channel[ch].cancfg |= (sjw<<AVR32_CANIF_CANCFG_SJW_OFFSET); }
#define CANIF_get_sm(ch)      		         ( ((AVR32_CANIF.channel[ch].cancfg & AVR32_CANIF_CANCFG_SM_MASK)>> AVR32_CANIF_CANCFG_SM_OFFSET) )
#define CANIF_set_sm(ch,sm)                  { AVR32_CANIF.channel[ch].cancfg |= (sm<<AVR32_CANIF_CANCFG_SM_OFFSET); }
#define CANIF_conf_bt(ch)                    { CANIF_set_sjw(ch,BAUDRATE##_SJW);  \
                                             CANIF_set_prs(ch,BAUDRATE##_PRS);    \
                                             CANIF_set_pres(ch,BAUDRATE##_PRES);  \
                                             CANIF_set_phs2(ch,BAUDRATE##_PHS2);  \
                                             CANIF_set_phs1(ch,BAUDRATE##_PHS1); }

//! CANIFFC Register Access
#define CANIF_get_error_mode(ch)             ( ((AVR32_CANIF.channel[ch].canfc & AVR32_CANIF_CANFC_EMODE_MASK) >> AVR32_CANIF_CANFC_EMODE_OFFSET) )
#define CANIF_get_tec(ch)                    ( ((AVR32_CANIF.channel[ch].canfc & AVR32_CANIF_CANFC_TEC_MASK) >> AVR32_CANIF_CANFC_TEC_OFFSET) )
#define CANIF_get_rec(ch)                    ( ((AVR32_CANIF.channel[ch].canfc & AVR32_CANIF_CANFC_REC_MASK) >> AVR32_CANIF_CANFC_REC_OFFSET) )

//! CANIFIER Register Access
#define CANIF_enable_wakeup_interrupt(ch)    { AVR32_CANIF.channel[ch].canier = AVR32_CANIF_CANIER_WKUPIM_MASK; }
#define CANIF_enable_interrupt(ch)           { AVR32_CANIF.channel[ch].canier = AVR32_CANIF_CANIER_TXOKIM_MASK | \
																				AVR32_CANIF_CANIER_RXOKIM_MASK | \
																				AVR32_CANIF_CANIER_CERRIM_MASK | \
																				AVR32_CANIF_CANIER_BOFFIM_MASK | \
																				AVR32_CANIF_CANIER_WKUPIM_MASK; }

//! CANIFIDR Register Access
#define CANIF_disable_wakeup_interrupt(ch)   { AVR32_CANIF.channel[ch].canidr = AVR32_CANIF_CANIDR_WKUPIM_MASK; }
#define CANIF_disable_interrupt(ch)          { AVR32_CANIF.channel[ch].canidr = AVR32_CANIF_CANIDR_TXOKIM_MASK | \
																				AVR32_CANIF_CANIDR_RXOKIM_MASK | \
																				AVR32_CANIF_CANIDR_CERRIM_MASK | \
																				AVR32_CANIF_CANIDR_BOFFIM_MASK; }

//! CANIFISCR Register Access
#define CANIF_get_interrupt_status(ch)          ( AVR32_CANIF.channel[ch].canisr)
#define CANIF_get_interrupt_error_status(ch)    ( AVR32_CANIF.channel[ch].canisr & 0x3F)
#define CANIF_get_interrupt_lastmob_selected(ch)( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_LSMOB_MASK)>> AVR32_CANIF_CANISR_LSMOB_OFFSET ))
#define CANIF_get_interrupt_wakeup_status(ch)   ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_WKUP_MASK)>> AVR32_CANIF_CANISR_WKUP_OFFSET ))
#define CANIF_get_interrupt_berr_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_BERR_MASK)>> AVR32_CANIF_CANISR_BERR_OFFSET ))
#define CANIF_get_interrupt_serr_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_SERR_MASK)>> AVR32_CANIF_CANISR_SERR_OFFSET ))
#define CANIF_get_interrupt_cerr_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_CERR_MASK)>> AVR32_CANIF_CANISR_CERR_OFFSET ))
#define CANIF_get_interrupt_ferr_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_FERR_MASK)>> AVR32_CANIF_CANISR_FERR_OFFSET ))
#define CANIF_get_interrupt_aerr_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_AERR_MASK)>> AVR32_CANIF_CANISR_AERR_OFFSET ))
#define CANIF_get_interrupt_boff_status(ch)     ( ((AVR32_CANIF.channel[ch].canisr & AVR32_CANIF_CANISR_BOFF_MASK)>> AVR32_CANIF_CANISR_BOFF_OFFSET ))
#define CANIF_clr_interrupt_status(ch)          {  AVR32_CANIF.channel[ch].caniscr = CANIF_get_interrupt_status(ch); }

//! MOBCTRL Register Access
#define CANIF_get_mobctrl(ch,mob)            (((unsigned volatile long*)&(AVR32_CANIF.channel[ch].mobctrl))[mob*3])
#define CANIF_set_mobctrl(ch,mob,val)        (((unsigned volatile long*)&(AVR32_CANIF.channel[ch].mobctrl))[mob*3]=val)
#define CANIF_mobctrl(ch,mob)                (((unsigned volatile long*)&(AVR32_CANIF.channel[ch].mobctrl))[mob*3])
#define CANIF_mob_clr_automode(ch,mob)       { CANIF_set_mobctrl(ch,mob,CANIF_get_mobctrl(ch,mob)&~(1<<AVR32_CANIF_AM_OFFSET));}
#define CANIF_mob_set_automode(ch,mob)       { CANIF_set_mobctrl(ch,mob,CANIF_get_mobctrl(ch,mob)| (1<<AVR32_CANIF_AM_OFFSET));}
#define CANIF_mob_clr_dlc(ch,mob)            { CANIF_set_mobctrl(ch,mob,CANIF_get_mobctrl(ch,mob)&~(0x0F<<AVR32_CANIF_DLC_OFFSET));}
#define CANIF_mob_set_dlc(ch,mob,dlc)        { CANIF_set_mobctrl(ch,mob,(CANIF_get_mobctrl(ch,mob)| (dlc<<AVR32_CANIF_DLC_OFFSET))); }
#define CANIF_mob_get_dlc(ch,mob)            ( (CANIF_get_mobctrl(ch,mob)&AVR32_CANIF_DLC_MASK)>>AVR32_CANIF_DLC_OFFSET )
#define CANIF_mob_clr_dir(ch,mob)            { CANIF_set_mobctrl(ch,mob,CANIF_get_mobctrl(ch,mob)&~(1<<AVR32_CANIF_DIR_OFFSET));}
#define CANIF_mob_set_dir(ch,mob)            { CANIF_set_mobctrl(ch,mob,CANIF_get_mobctrl(ch,mob)| (1<<AVR32_CANIF_DIR_OFFSET));}
#define CANIF_mob_get_dir(ch,mob)            ( (CANIF_get_mobctrl(ch,mob)&AVR32_CANIF_DIR_MASK) >> AVR32_CANIF_DIR_OFFSET )

//! MOBSR Register Access
#define CANIF_mob_get_status(ch,mob)         (((unsigned volatile long*) &(AVR32_CANIF.channel[ch].mobsr))[mob*3])
#define CANIF_mob_set_status(ch,mob,val)     {((unsigned volatile long*) &(AVR32_CANIF.channel[ch].mobscr))[mob*3]=val;}
#define CANIF_mob_clear_status(ch,mob)       {CANIF_mob_set_status(ch,mob,0x0F)}

//! MOBSCH Register Access
#define CANIF_mob_get_mob_free(ch)    	     ((AVR32_CANIF.channel[ch].mobsch&AVR32_CANIF_MOBSCH_MAV_MASK)>>AVR32_CANIF_MOBSCH_MAV_OFFSET)
#define CANIF_mob_get_mob_rxok(ch)    	     ((AVR32_CANIF.channel[ch].mobsch&AVR32_CANIF_MOBSCH_MRXOK_MASK)>>AVR32_CANIF_MOBSCH_MRXOK_OFFSET)
#define CANIF_mob_get_mob_txok(ch)    	     ((AVR32_CANIF.channel[ch].mobsch&AVR32_CANIF_MOBSCH_MTXOK_MASK)>>AVR32_CANIF_MOBSCH_MTXOK_OFFSET)

//! MOBER Register Access
#define CANIF_mob_enable(ch,mob)             {AVR32_CANIF.channel[ch].mober = 1<<mob;}

//! MOBDR Register Access
#define CANIF_mob_disable(ch,mob)   	     {AVR32_CANIF.channel[ch].mobdr = 1<<mob;}

//! MOBESR Register Access
#define CANIF_mob_enable_status(ch,mob)      ((AVR32_CANIF.channel[ch].mobesr >> mob)&1)

//! MOBIER Register Access
#define CANIF_mob_enable_interrupt(ch,mob)   {AVR32_CANIF.channel[ch].mobier = 1<<mob;}

//! MOBIDR Register Access
#define CANIF_mob_disable_interrupt(ch,mob)  {AVR32_CANIF.channel[ch].mobidr = 1<<mob;}

//! MRXISCR Register Access
#define CANIF_mob_clear_rxok_status(ch,mob)  {AVR32_CANIF.channel[ch].mrxiscr = 1<<mob;}

//! MRXISR Register Access
#define CANIF_mob_get_rxok_status(ch,mob)    ((AVR32_CANIF.channel[ch].mrxisr >> mob)& 1)

//! MTXISCR Register Access
#define CANIF_mob_clear_txok_status(ch,mob)  {AVR32_CANIF.channel[ch].mtxiscr = 1<<mob;}

//! MTXISR Register Access
#define CANIF_mob_get_txok_status(ch,mob)    ((AVR32_CANIF.channel[ch].mtxisr >> mob)& 1)

#define CANIF_SIZE_OF_CANIF_MSG              (sizeof(can_msg_t))
#define CANIF_config_tx(ch,mob)              {CANIF_mob_set_dir(ch,mob) }
#define CANIF_config_rx(ch,mob)              {CANIF_mob_clr_dir(ch,mob) }

#define CANIF_mob_allocate(ch)               (CANIF_mob_get_mob_free(ch))
#define CANIF_mob_free_get_addr_data(ch)     (CANIF_SIZE_OF_CANIF_MSG*CANIF_mob_get_mob_free(ch)+CANIF_get_ram_add(ch))
#define CANIF_mob_free_get_ptr_data(ch)      ((can_msg_t *)(CANIF_SIZE_OF_CANIF_MSG*CANIF_mob_get_mob_free(ch)+CANIF_get_ram_add(ch)))
#define CANIF_mob_free(ch,mob)               {CANIF_mob_disable(ch,mob)}
#define CANIF_mob_get_addr_data(ch,mob)      ((CANIF_SIZE_OF_CANIF_MSG*mob+CANIF_get_ram_add(ch)))
#define CANIF_mob_get_ptr_data(ch,mob)       ((can_msg_t *)(CANIF_SIZE_OF_CANIF_MSG*mob+CANIF_get_ram_add(ch)))

#define CANIF_set_idemask(ch,mob)            {(CANIF_mob_get_ptr_data(ch,mob))->ide_mask_bit = 1;}
#define CANIF_clr_idemask(ch,mob)            {(CANIF_mob_get_ptr_data(ch,mob))->ide_mask_bit = 0);}
#define CANIF_get_idemask(ch,mob)            ((CANIF_mob_get_ptr_data(ch,mob))->ide_mask_bit)
#define CANIF_set_rtrmask(ch,mob)            {(CANIF_mob_get_ptr_data(ch,mob))->rtr_mask_bit = 1;}
#define CANIF_clr_rtrmask(ch,mob)            {(CANIF_mob_get_ptr_data(ch,mob))->rtr_mask_bit = 0;}
#define CANIF_get_rtrmask(ch,mob)            ((CANIF_mob_get_ptr_data(ch,mob))->rtr_mask_bit)
#define CANIF_set_ide(ch,mob)                {(CANIF_mob_get_ptr_data(ch,mob))->ide_bit = 1;}
#define CANIF_clr_ide(ch,mob)                {(CANIF_mob_get_ptr_data(ch,mob))->ide_bit = 0;}
#define CANIF_get_ide(ch,mob)                ((CANIF_mob_get_ptr_data(ch,mob))->ide_bit)
#define CANIF_set_rtr(ch,mob)                {(CANIF_mob_get_ptr_data(ch,mob))->rtr_bit = 1;}
#define CANIF_clr_rtr(ch,mob)                {(CANIF_mob_get_ptr_data(ch,mob))->rtr_bit = 0;}
#define CANIF_get_rtr(ch,mob)                ((CANIF_mob_get_ptr_data(ch,mob))->rtr_bit )

#define CANIF_set_ext_id(ch,mob,_id)         {(CANIF_mob_get_ptr_data(ch,mob))->id = (1<<IDE_BIT)|(_id);}
#define CANIF_get_ext_id(ch,mob)             ((CANIF_mob_get_ptr_data(ch,mob))->id & 0x1FFFFFFF )
#define CANIF_set_std_id(ch,mob,_id)         {(CANIF_mob_get_ptr_data(ch,mob))->id = (_id);}
#define CANIF_get_std_id(ch,mob)             ((CANIF_mob_get_ptr_data(ch,mob))->id & 0x000007FF )

#define CANIF_set_ext_idmask(ch,mob,mask)    {(CANIF_mob_get_ptr_data(ch,mob))->id_mask = mask;}
#define CANIF_set_std_idmask(ch,mob,mask)    {(CANIF_mob_get_ptr_data(ch,mob))->id_mask = mask;}

#define CANIF_clr_mob(ch,mob)                {(CANIF_mob_get_ptr_data(ch,mob))->id = 0; \
                                             (CANIF_mob_get_ptr_data(ch,mob))->id_mask = 0; \
                                             (CANIF_mob_get_ptr_data(ch,mob))->data.u64 = 0;}

#define CANIF_set_data(ch,mob,_data)         {(CANIF_mob_get_ptr_data(ch,mob))->data.u64 = _data;}

/*! \brief Gets the mob status.
 * \param ch              CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 * \param mob             Mob number
 * \return uint8_t        MOB_RX_COMPLETED or MOB_TX_COMPLETED or MOB_RX_COMPLETED_DLCW
 *                        MOB_NOT_COMPLETED or CANIF.ISR register value.
 */
extern uint8_t canif_get_mob_status(uint8_t ch, uint8_t mob);

/*! \brief Config Baudrate with fixed baudrate
 * \param ch              CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 * \return 1              Status completed
 */
extern uint8_t canif_fixed_baudrate(uint8_t ch);

/*! \brief Clear all mob of a channel
 * \param ch              CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 * \param nb_mob          Number of MOBs to clear
 */
extern void canif_clear_all_mob(uint8_t ch, uint8_t nb_mob);

/*! \brief Get the first MOB free
 * \param ch              CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 * \return uint8_t        First available MOB or NO_MOB
 */
extern uint8_t canif_get_mob_free(uint8_t ch);
//______________________________________________________________________________

/**
 * \}
 */

#endif // _CANIF_H_
