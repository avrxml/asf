/*****************************************************************************
 *
 * \file
 *
 * \brief PWM driver for AVR32 UC3 with PWM module version above 4.0.0.
 *
 * This file defines a useful set of functions for the PWM interface on AVR32
 * devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "pwm4.h"

int pwm_write_protect_hw_lock(int group)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  switch(group)
  {
    case 0:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG0_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS0_YES << AVR32_PWM_WPSR_WPHWS0)) return PWM_FAILURE;
      break;
    case 1:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG1_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS1_YES << AVR32_PWM_WPSR_WPHWS1)) return PWM_FAILURE;
      break;
    case 2:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG2_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS2_YES << AVR32_PWM_WPSR_WPHWS2)) return PWM_FAILURE;
      break;
    case 3:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG3_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS3_YES << AVR32_PWM_WPSR_WPHWS3)) return PWM_FAILURE;
      break;
    case 4:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG4_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS4_YES << AVR32_PWM_WPSR_WPHWS4)) return PWM_FAILURE;
      break;
    case 5:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG5_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_HWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPHWS5_YES << AVR32_PWM_WPSR_WPHWS5)) return PWM_FAILURE;
      break;
  }
  return PWM_SUCCESS;
}

int pwm_write_protect_sw_lock(int group)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  switch(group)
  {
    case 0:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG0_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS0_YES << AVR32_PWM_WPSR_WPSWS0)) return PWM_FAILURE;
      break;
    case 1:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG1_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS1_YES << AVR32_PWM_WPSR_WPSWS1)) return PWM_FAILURE;
      break;
    case 2:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG2_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS2_YES << AVR32_PWM_WPSR_WPSWS2)) return PWM_FAILURE;
      break;
    case 3:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG3_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS3_YES << AVR32_PWM_WPSR_WPSWS3)) return PWM_FAILURE;
      break;
    case 4:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG4_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS4_YES << AVR32_PWM_WPSR_WPSWS4)) return PWM_FAILURE;
      break;
    case 5:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG5_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWENA << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != (AVR32_PWM_WPSR_WPSWS5_YES << AVR32_PWM_WPSR_WPSWS5)) return PWM_FAILURE;
      break;
  }
  return PWM_SUCCESS;
}

int pwm_write_protect_sw_unlock(int group)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  switch(group)
  {
    case 0:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG0_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
    case 1:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG1_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
    case 2:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG2_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
    case 3:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG3_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
    case 4:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG4_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
    case 5:
      pwm->wpcr = (AVR32_PWM_WPCR_WPKEY_KEY   << AVR32_PWM_WPCR_WPKEY) |
	          AVR32_PWM_WPCR_WPRG5_MASK                            |
	          (AVR32_PWM_WPCR_WPCMD_SWDIS << AVR32_PWM_WPCR_WPCMD);
      if (pwm->wpsr != 0) return PWM_FAILURE;
      break;
  }
  return PWM_SUCCESS;
}

U32 pwm_write_protect_status(void)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  U32 status;
  status = pwm->wpsr;
  if (status&AVR32_PWM_WPSR_WPVS_MASK)
    return ((status&AVR32_PWM_WPSR_WPVSRC_MASK)>>AVR32_PWM_WPSR_WPVSRC_OFFSET);
  else
    return PWM_NO_WRITE_PROTECT_VIOLATION;
}

int pwm_init(pwm_opt_t *opt)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (opt == 0 ) // Null pointer.
    return PWM_INVALID_INPUT;

  // First Unlock Register
  pwm_write_protect_sw_unlock(0);
  pwm_write_protect_sw_unlock(1);
  pwm_write_protect_sw_unlock(2);
  pwm_write_protect_sw_unlock(3);

  // Disable interrupt.
  if (global_interrupt_enabled) Disable_global_interrupt();
  pwm->idr1 = ((1 << (AVR32_PWM_LINES_MSB + 1)) - 1) << AVR32_PWM_IDR1_CHID0_OFFSET;
  if (opt->fault_detection_activated)
    pwm->idr1 |= ((1 << (AVR32_PWM_LINES_MSB + 1)) - 1) << AVR32_PWM_IDR1_FCHID0_OFFSET;
  pwm->isr1;
  // Check if synchronous channel ...
  if (opt->sync_channel_activated)
  {
    pwm->idr2 = (1 << AVR32_PWM_IDR2_UNRE_OFFSET) | (1 << AVR32_PWM_IDR2_WRDY_OFFSET);
    if (opt->sync_update_channel_mode==PWM_SYNC_UPDATE_AUTO_WRITE_AUTO_UPDATE)
    pwm->idr2 |= (1 << AVR32_PWM_IDR2_ENDTX_OFFSET) | (1 << AVR32_PWM_IDR2_TXBUFE_OFFSET);
    pwm->isr2;
  }
  if (global_interrupt_enabled) Enable_global_interrupt();

  // Set PWM mode register.
  pwm->clk =
    ((opt->diva)<<AVR32_PWM_DIVA_OFFSET) |
    ((opt->divb)<<AVR32_PWM_DIVB_OFFSET) |
    ((opt->prea)<<AVR32_PWM_PREA_OFFSET) |
    ((opt->preb)<<AVR32_PWM_PREB_OFFSET) |
    ((opt->cksel)<<AVR32_PWM_CLKSEL_OFFSET)
    ;

  // Set PWM Sync register
  pwm->SCM.updm = opt->sync_update_channel_mode;
  int i;
  for (i=0;i<PWM_OOV_LINES;i++)
  {
    pwm->scm     |= ((opt->sync_channel_select[i])<<(AVR32_PWM_SCM_SYNC0_OFFSET+i));
  }

  return PWM_SUCCESS;
}

int pwm_channel_init( unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (pwm_channel == 0) // Null pointer.
    return PWM_INVALID_ARGUMENT;
  if (channel_id > AVR32_PWM_LINES_MSB) // Control input values.
    return PWM_INVALID_INPUT;

  pwm->channel[channel_id].cmr= pwm_channel->cmr;   // Channel mode.
  pwm->channel[channel_id].cdty= pwm_channel->cdty; // Duty cycle, should be < CPRD.
  pwm->channel[channel_id].cprd= pwm_channel->cprd; // Channel period.
  pwm->channel[channel_id].dt= pwm_channel->dt; // Channel period.
  return PWM_SUCCESS;
}

int pwm_start_channels(unsigned long channels_bitmask)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  if (channels_bitmask & ~((1 << (AVR32_PWM_LINES_MSB + 1)) - 1))
    return PWM_INVALID_INPUT;

  //pwm->wpsr; // Clear Fault register
  pwm->ena = channels_bitmask; // Enable channels.

  return PWM_SUCCESS;
}

int pwm_stop_channels(unsigned long channels_bitmask)
{
  if (channels_bitmask & ~((1 << (AVR32_PWM_LINES_MSB + 1)) - 1))
    return PWM_INVALID_INPUT;

  AVR32_PWM.dis = channels_bitmask; // Disable channels.

  return PWM_SUCCESS;
}

int pwm_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (channel_id > AVR32_PWM_LINES_MSB)
     return PWM_INVALID_INPUT;

  AVR32_PWM.isr1;                                    // Acknowledgement and clear previous register state.
  while (!(AVR32_PWM.isr1 & (1 << channel_id)));     // Wait until the last write has been taken into account.
  pwm->channel[channel_id].cdtyupd= pwm_channel->cdtyupd; // Channel update Duty Cycle
  pwm->channel[channel_id].cprdupd= pwm_channel->cprdupd; // Channel update Period
  pwm->channel[channel_id].dtupd= pwm_channel->dtupd;     // Channel update Dead Time

  return PWM_SUCCESS;
}

int pwm_interrupt_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (channel_id > AVR32_PWM_LINES_MSB)
     return PWM_INVALID_INPUT;

  pwm->channel[channel_id].cdtyupd= pwm_channel->cdtyupd; // Channel update Duty Cycle
  pwm->channel[channel_id].cprdupd= pwm_channel->cprdupd; // Channel update Period
  pwm->channel[channel_id].dtupd= pwm_channel->dtupd;     // Channel update Dead Time

  return PWM_SUCCESS;
}

int pwm_output_override_setup(pwm_output_override_opt_t *opt)
{
  if (opt == 0 ) // Null pointer.
    return PWM_INVALID_INPUT;

  AVR32_PWM.oov = 0; // Clear output override value register
  int i;
  for (i=0;i<PWM_OOV_LINES;i++)
  {
      AVR32_PWM.oov  = ((opt->oov[0][i]<<(AVR32_PWM_OOVH0_OFFSET+i))|(opt->oov[1][i]<<(AVR32_PWM_OOVL0_OFFSET+i)));
  }

  return PWM_SUCCESS;
}

int pwm_output_override_write(pwm_output_override_opt_t *opt)
{
  if (opt == 0 ) // Null pointer.
    return PWM_INVALID_INPUT;

  U32 tmp=0;
  int i;
  for (i=0;i<PWM_OOV_LINES;i++)
  {
        if (opt->os[0][i])
          tmp |= (1<<(AVR32_PWM_OSH0_OFFSET+i));
        else
          tmp &= ~(1<<(AVR32_PWM_OSH0_OFFSET+i));
        if (opt->os[1][i])
          tmp |= (1<<(AVR32_PWM_OSL0_OFFSET+i));
        else
          tmp &= ~(1<<(AVR32_PWM_OSL0_OFFSET+i));
  }
  AVR32_PWM.os  = tmp;
  return PWM_SUCCESS;
}

int pwm_update_period_value(unsigned int value)
{
  AVR32_PWM.scup = value;
  return PWM_SUCCESS;
}

int pwm_synchronous_prepare_update(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (channel_id > AVR32_PWM_LINES_MSB)
     return PWM_INVALID_INPUT;

  AVR32_PWM.isr1;                                    // Acknowledgement and clear previous register state.
  while (!(AVR32_PWM.isr1 & (1 << channel_id)));     // Wait until the last write has been taken into account.
  pwm->channel[channel_id].cdtyupd= pwm_channel->cdtyupd; // Channel update Duty Cycle
  pwm->channel[channel_id].cprdupd= pwm_channel->cprdupd; // Channel update Period
  return PWM_SUCCESS;
}

int pwm_update_manual_write_manual_update(void)
{
  AVR32_PWM.scuc = (1<<AVR32_PWM_SCUC_UPDULOCK_OFFSET);
  return PWM_SUCCESS;
}

int pwm_update_manual_write_auto_update(void)
{
  while(!AVR32_PWM.ISR2.wrdy);
  return PWM_SUCCESS;
}

int pwm_update_auto_write_auto_update_check_ready(void)
{
  while(!AVR32_PWM.ISR2.wrdy);
  return PWM_SUCCESS;
}

int pwm_update_auto_write_auto_update_check_eot(void)
{
  while(!AVR32_PWM.ISR2.endtx);
  return PWM_SUCCESS;
}

int pwm_fault_lines_polarity_setup(U8 line,
                                    U8 polarity,
                                    U8 mode,
                                    U8 filter)
{
  if ((line > 8 ) ||
      (polarity > 1 ) ||
      (mode > 1 )||
      (filter > 1 ))
    return PWM_INVALID_INPUT;

  if (polarity)
    AVR32_PWM.fmr  |=  (polarity<<(AVR32_PWM_FPOL0_OFFSET+line));
  else
    AVR32_PWM.fmr  &= ~(polarity<<(AVR32_PWM_FPOL0_OFFSET+line));

  if (mode)
    AVR32_PWM.fmr  |=  (mode<<(AVR32_PWM_FMOD0_OFFSET+line));
  else
    AVR32_PWM.fmr  &= ~(mode<<(AVR32_PWM_FMOD0_OFFSET+line));

  if (filter)
    AVR32_PWM.fmr  |=  (filter<<(AVR32_PWM_FFIL0_OFFSET+line));
  else
    AVR32_PWM.fmr  &= ~(filter<<(AVR32_PWM_FFIL0_OFFSET+line));

  return PWM_SUCCESS;
}

int pwm_fault_lines_level_setup(U8 line,
                                 U8 channel_high,
                                 U8 channel_low)
{
  if ((line > 8 ) ||
      (channel_high > 1 ) ||
      (channel_low > 1 ))
    return PWM_INVALID_INPUT;

  if (channel_high)
    AVR32_PWM.fpv  |=  (channel_high<<(AVR32_PWM_FPVH0_OFFSET+line));
  else
    AVR32_PWM.fpv  &= ~(channel_high<<(AVR32_PWM_FPVH0_OFFSET+line));

  if (channel_low)
    AVR32_PWM.fpv  |=  (channel_low<<(AVR32_PWM_FPVL0_OFFSET+line));
  else
    AVR32_PWM.fpv  &= ~(channel_low<<(AVR32_PWM_FPVL0_OFFSET+line));

  return PWM_SUCCESS;
}

int pwm_fault_lines_enable(U8 line,
                            U8 channel)
{
  if (channel > 4 ) // Null pointer.
    return PWM_INVALID_INPUT;

  if (channel)
    AVR32_PWM.fpe1  |=  (line<<(AVR32_PWM_FPE1_OFFSET+8*channel));
  else
    AVR32_PWM.fpe1  &= ~(line<<(AVR32_PWM_FPE1_OFFSET+8*channel));

  return PWM_SUCCESS;
}

int pwm_get_fault_status(void)
{
  return AVR32_PWM.fsr;
}

void pwm_clr_fault_status(void)
{
  AVR32_PWM.fcr = 0xffff;
}

// CV & CVM & CTR & CPR
int pwm_compare_match_setup(int line,pwm_compare_opt_t *pwm_compare_t)
{
  if (line > AVR32_PWM_CMP_NUM)
    return PWM_INVALID_INPUT;

  // configure Compare PWM Counter Value
  AVR32_PWM.comp[line].cmp0v   =
                    ((pwm_compare_t->compare_pwm_counter_value<<AVR32_PWM_CMP0V_CV_OFFSET) |
                     (pwm_compare_t->compare_mode<<AVR32_PWM_CMP0V_CVM_OFFSET));

  // configure Compare Trigger
  AVR32_PWM.comp[line].cmp0m            =
                    (((pwm_compare_t->compare_pwm_period_counter_value)<< AVR32_PWM_CMP0M_CTR_OFFSET)  |
                    ((pwm_compare_t->compare_period_counter_value)     << AVR32_PWM_CMP0M_CPR_OFFSET)  |
                    ((pwm_compare_t->compare_pwm_update_period)        << AVR32_PWM_CMP0M_CUPR_OFFSET) |
                    ((pwm_compare_t->compare_status)                   << AVR32_PWM_CMP0M_CEN_OFFSET));

  // configure Comparison Period
  return PWM_SUCCESS;
}

// CVUPD & CVMUPD & CTRUPD & CPRUPD
int pwm_compare_match_reload_setup(int line,pwm_compare_opt_t *pwm_compare_t)
{
  if (line > AVR32_PWM_CMP_NUM)
    return PWM_INVALID_INPUT;

  // configure Compare PWM Counter Value
  AVR32_PWM.comp[line].cmp0vupd   =
                    ((pwm_compare_t->compare_pwm_counter_value<<AVR32_PWM_CMP0VUPD_CVUPD_OFFSET) |
                     (pwm_compare_t->compare_mode<<AVR32_PWM_CMP0VUPD_CVMUPD_OFFSET));

  // configure Compare Trigger
  AVR32_PWM.comp[line].cmp0mupd  =
                    (((pwm_compare_t->compare_pwm_period_counter_value)<< AVR32_PWM_CMP0MUPD_CTRUPD_OFFSET)  |
                    ((pwm_compare_t->compare_period_counter_value)     << AVR32_PWM_CMP0MUPD_CPRUPD_OFFSET)  |
                    ((pwm_compare_t->compare_pwm_update_period)        << AVR32_PWM_CMP0MUPD_CUPRUPD_OFFSET) |
                    ((pwm_compare_t->compare_status)                   << AVR32_PWM_CMP0MUPD_CENUPD_OFFSET));

  return PWM_SUCCESS;
}

int pwm_compare_event_enable(int line, int event_line)
{
  if (line > AVR32_PWM_CMP_NUM)
    return PWM_INVALID_INPUT;

  // Enable Event Line
  AVR32_PWM.elxmr[line] = event_line;

  return PWM_SUCCESS;
}

int pwm_compare_event_disable(int line, int event_line)
{
  if (line > AVR32_PWM_CMP_NUM)
    return PWM_INVALID_INPUT;

  // Disable Event Line
  AVR32_PWM.elxmr[line] &= (~event_line);

  return PWM_SUCCESS;
}
