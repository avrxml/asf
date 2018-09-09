/*****************************************************************************
 *
 * \file
 *
 * \brief QDEC driver for AVR32 UC3.
 *
 * AVR32 QDEC driver module.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "qdec.h"


int qdec_get_interrupt_settings(volatile avr32_qdec_t *qdec)
{
  return qdec->imr;
}


int qdec_configure_interrupts(volatile avr32_qdec_t *qdec, const qdec_interrupt_t *bitfield)
{

  AVR32_ENTER_CRITICAL_REGION( );
  // Enable the appropriate interrupts.
  qdec->ier            =     bitfield->ovr << AVR32_QDEC_IER_OVR_OFFSET |
                             bitfield->dirinv << AVR32_QDEC_IER_DIRINV_OFFSET |
                             bitfield->idexerr << AVR32_QDEC_IER_IDXERR_OFFSET |
                             bitfield->rcro << AVR32_QDEC_IER_RCRO_OFFSET |
                             bitfield->pcro << AVR32_QDEC_IER_PCRO_OFFSET |
                             bitfield->cap << AVR32_QDEC_IER_CAP_OFFSET |
                             bitfield->cmp << AVR32_QDEC_IER_CMP_OFFSET |
                             bitfield->qepi << AVR32_QDEC_IER_QEPI_OFFSET;

  // Disable the appropriate interrupts.
  qdec->idr            =     (~bitfield->ovr & 1) << AVR32_QDEC_IER_OVR_OFFSET |
                             (~bitfield->dirinv & 1) << AVR32_QDEC_IER_DIRINV_OFFSET |
                             (~bitfield->idexerr & 1) << AVR32_QDEC_IER_IDXERR_OFFSET |
                             (~bitfield->rcro & 1) << AVR32_QDEC_IER_RCRO_OFFSET |
                             (~bitfield->pcro & 1) << AVR32_QDEC_IER_PCRO_OFFSET |
                             (~bitfield->cap & 1) << AVR32_QDEC_IER_CAP_OFFSET |
                             (~bitfield->cmp & 1) << AVR32_QDEC_IER_CMP_OFFSET |
                             (~bitfield->qepi & 1) << AVR32_QDEC_IER_QEPI_OFFSET;

  AVR32_LEAVE_CRITICAL_REGION( );

  return 0;
}

int qdec_init_quadrature_decoder_mode(volatile avr32_qdec_t *qdec, const qdec_quadrature_decoder_opt_t *opt)
{
  qdec->cf = 0;
  qdec->cf            =     1 << AVR32_QDEC_CF_QDEC_OFFSET |
                            opt->idxe << AVR32_QDEC_CF_IDXE_OFFSET |
                            opt->pcce << AVR32_QDEC_CF_PCCE_OFFSET |
                            opt->rcce << AVR32_QDEC_CF_RCCE_OFFSET |
                            opt->evtrge << AVR32_QDEC_CF_EVTRGE_OFFSET |
                            opt->phsinva << AVR32_QDEC_CF_PHSINVA_OFFSET |
                            opt->phsinvb << AVR32_QDEC_CF_PHSINVB_OFFSET |
                            opt->idxinv << AVR32_QDEC_CF_IDXINV_OFFSET |
                            opt->idxphs << AVR32_QDEC_CF_IDXPHS_OFFSET |
                            opt->filten << AVR32_QDEC_CF_FILTEN_OFFSET ;
  // Start QDEC Clock by default
  qdec->ctrl          |=     1 << AVR32_QDEC_CTRL_CLKEN_OFFSET;
  return 0;

}

int qdec_init_timer_mode(volatile avr32_qdec_t *qdec, const qdec_timer_opt_t *opt)
{
  qdec->cf = 0;
  qdec->cf            =     0 << AVR32_QDEC_CF_QDEC_OFFSET |
                            opt->pcce << AVR32_QDEC_CF_PCCE_OFFSET |
                            opt->rcce << AVR32_QDEC_CF_RCCE_OFFSET |
                            opt->evtrge << AVR32_QDEC_CF_EVTRGE_OFFSET |
                            opt->filten << AVR32_QDEC_CF_FILTEN_OFFSET |
                            opt->tsdir << AVR32_QDEC_CF_TSDIR_OFFSET |
                            opt->upd << AVR32_QDEC_CF_UPD_OFFSET ;
  // Start QDEC Clock by default
  qdec->ctrl          |=     1 << AVR32_QDEC_CTRL_CLKEN_OFFSET;
   return 0;
}

int qdec_software_trigger(volatile avr32_qdec_t *qdec)
{

  qdec->ctrl           |= 1 << AVR32_QDEC_CTRL_SWTRG_OFFSET ;
  return 0;
}

int qdec_stop(volatile avr32_qdec_t *qdec)
{
  // Stop QDEC Clock by default
  qdec->ctrl          &=     ~(1 << AVR32_QDEC_CTRL_CLKEN_OFFSET);
  return 0;
}

unsigned short int qdec_read_rc(volatile avr32_qdec_t *qdec)
{
   return qdec->CNT.rc;
}

unsigned short int qdec_read_pc(volatile avr32_qdec_t *qdec)
{
   return qdec->CNT.pc;
}

unsigned short int qdec_write_rc_cnt(volatile avr32_qdec_t *qdec,unsigned short int value)
{
  qdec->CNT.rc = value;
  return 0;
}

unsigned short int qdec_write_pc_cnt(volatile avr32_qdec_t *qdec,unsigned short int value)
{
  qdec->CNT.pc = value;
  return qdec->CNT.pc;
}

unsigned short int qdec_write_rc_top(volatile avr32_qdec_t *qdec, unsigned short int value)
{
  qdec->TOP.rctop = value;

  return qdec->TOP.rctop;
}

unsigned short int qdec_write_pc_top(volatile avr32_qdec_t *qdec, unsigned short int value)
{
  qdec->TOP.pctop = value;

  return qdec->TOP.pctop;
}

unsigned short int qdec_write_rc_cmp(volatile avr32_qdec_t *qdec, unsigned short int value)
{
  qdec->CMP.rccmp = value;

  return qdec->CMP.rccmp;
}

unsigned short int qdec_write_pc_cmp(volatile avr32_qdec_t *qdec, unsigned short int value)
{
  qdec->CMP.pccmp = value;

  return qdec->CMP.pccmp;
}

unsigned short int qdec_read_rc_cap(volatile avr32_qdec_t *qdec)
{
   return qdec->CAP.rccap;
}

unsigned short int qdec_read_pc_cap(volatile avr32_qdec_t *qdec)
{
   return qdec->CAP.pccap;
}












