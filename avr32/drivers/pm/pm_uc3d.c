/**
 * \file
 *
 * \brief Power Manager(PM) driver interface.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "pm_uc3d.h"


#ifndef AVR32_PM_VERSION_RESETVALUE
  #error Cannot use this software module with the current device.
#else
  #if AVR32_PM_VERSION_RESETVALUE < 0x412
    #error Cannot use this software module with the current device
  #endif
#endif

/*! \name PM Writable Bit-Field Registers
 */
//! @{

typedef union
{
  unsigned long                 cfdctrl;
  avr32_pm_cfdctrl_t            CFDCTRL;
} u_avr32_pm_cfdctrl_t;

typedef union
{
  unsigned long                 cpusel;
  avr32_pm_cpusel_t             CPUSEL;
} u_avr32_pm_cpusel_t;

typedef union
{
  unsigned long                 pbasel;
  avr32_pm_pbasel_t             PBASEL;
} u_avr32_pm_pbasel_t;

typedef union
{
  unsigned long                 pbbsel;
  avr32_pm_pbbsel_t             PBBSEL;
} u_avr32_pm_pbbsel_t;

//! @}


// Forward declarations
static pm_divratio_t pm_find_divratio(unsigned long ref_freq_hz, unsigned long target_freq_hz);


/**
 ** Clock Functions
 **/

long pm_set_mclk_source(pm_clk_src_t src)
{
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected MCCTRL register
  PM_UNLOCK(AVR32_PM_MCCTRL);
  AVR32_PM.mcctrl = src;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long pm_config_mainclk_safety(bool cfd, bool ocp, bool final)
{
  u_avr32_pm_cfdctrl_t u_avr32_pm_cfdctrl = {AVR32_PM.cfdctrl};

  // Check if the CFDCTRL register is read-only.
  if(AVR32_PM.cfdctrl & AVR32_PM_CFDCTRL_SFV_MASK)
    return -1;

  // Modify
  u_avr32_pm_cfdctrl.CFDCTRL.cfden = cfd;
  u_avr32_pm_cfdctrl.CFDCTRL.ocpen = ocp;
  u_avr32_pm_cfdctrl.CFDCTRL.sfv = final;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected CFDCTRL register
  PM_UNLOCK(AVR32_PM_CFDCTRL);
  // Write back
  AVR32_PM.cfdctrl = u_avr32_pm_cfdctrl.cfdctrl;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long pm_set_clk_domain_div(pm_clk_domain_t clock_domain, pm_divratio_t divratio)
{
  u_avr32_pm_cpusel_t u_avr32_pm_cpusel = {AVR32_PM.cpusel};

//# Implementation note: the CPUSEL and PBASEL and PBBSEL registers all have the
//# same structure.

//# Implementation note: the ckSEL registers are contiguous and memory-mapped in
//# that order: CPUSEL, HSBSEL, PBASEL, PBBSEL.

#ifdef AVR32SFW_INPUT_CHECK
  // Check the divratio
  if((divratio > PM_CPUSEL_DIVRATIO_MAX)||(divratio < 0))
    return -1;
#endif

  // ckSEL must not be written while SR.CKRDY is 0.
  while(!(AVR32_PM.sr & AVR32_PM_SR_CKRDY_MASK));

  // Modify
  u_avr32_pm_cpusel.CPUSEL.cpudiv= 1;
  u_avr32_pm_cpusel.CPUSEL.cpusel = divratio;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected ckSEL register
  PM_UNLOCK(AVR32_PM_CPUSEL + clock_domain*sizeof(avr32_pm_cpusel_t));
  // Update
  *(&(AVR32_PM.cpusel) + clock_domain)= u_avr32_pm_cpusel.cpusel;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long pm_disable_clk_domain_div(pm_clk_domain_t clock_domain)
{
  u_avr32_pm_cpusel_t u_avr32_pm_cpusel = {AVR32_PM.cpusel};

//# Implementation note: the CPUSEL and PBASEL and PBBSEL registers all have the
//# same structure.

//# Implementation note: the ckSEL registers are contiguous and memory-mapped in
//# that order: CPUSEL, HSBSEL, PBASEL, PBBSEL.

  // ckSEL must not be written while SR.CKRDY is 0.
  while(!(AVR32_PM.sr & AVR32_PM_SR_CKRDY_MASK));

  // Modify
  u_avr32_pm_cpusel.CPUSEL.cpudiv= DISABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected ckSEL register
  PM_UNLOCK(AVR32_PM_CPUSEL + clock_domain*sizeof(avr32_pm_cpusel_t));
  // Update
  *(&(AVR32_PM.cpusel) + clock_domain)= u_avr32_pm_cpusel.cpusel;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

void pm_set_all_cksel(unsigned long main_clock_f_hz, unsigned long cpu_f_hz,
                      unsigned long pba_f_hz, unsigned long pbb_f_hz)
{
  pm_divratio_t div_ratio;


  // Find the divratio to apply to reach the target cpu_f frequency.
  div_ratio = pm_find_divratio(main_clock_f_hz, cpu_f_hz);
  // Apply the division ratio for the CPU clock domain.
  if(PM_CKSEL_DIVRATIO_ERROR == div_ratio)
    pm_disable_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_CPU);
  else
    pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_CPU, div_ratio);

  // Find the divratio to apply to reach the target pba_f frequency.
  div_ratio = pm_find_divratio(main_clock_f_hz, pba_f_hz);
  // Apply the division ratio for the PBA clock domain.
  if(PM_CKSEL_DIVRATIO_ERROR == div_ratio)
    pm_disable_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBA);
  else
    pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBA, div_ratio);

  // Find the divratio to apply to reach the target pbb_f frequency.
  div_ratio = pm_find_divratio(main_clock_f_hz, pbb_f_hz);
  // Apply the division ratio for the PBB clock domain.
  if(PM_CKSEL_DIVRATIO_ERROR == div_ratio)
    pm_disable_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBB);
  else
    pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBB, div_ratio);
}

long pm_wait_for_clk_ready(void)
{
  unsigned int timeout = PM_POLL_TIMEOUT;
  while (!(AVR32_PM.sr & AVR32_PM_SR_CKRDY_MASK))
  {
    if(--timeout == 0)
      return -1;
  }
  return PASS;
}

/*! \brief Compute the division ratio field of the PM CPU/PBxSEL registers to apply
 *         to get a target frequency from a main clock source frequency.
 *
 * \param ref_freq_hz The main clock source frequency (Hz)
 * \param target_freq_hz The target frequency (Hz)
 *
 * \return the division ratio CPUSEL
 *   \retval PM_CKSEL_DIVRATIO_ERROR  no div ratio to apply
 *   \retval the div ratio enum value
 */
static pm_divratio_t pm_find_divratio(unsigned long ref_freq_hz, unsigned long target_freq_hz)
{
  int div_ratio;

  div_ratio = ref_freq_hz/target_freq_hz;
  if(div_ratio == 1)
    return(PM_CKSEL_DIVRATIO_ERROR);
  else
  {
    // div ratio field value so that rel_freq_hz = target_freq_hz*2pow(divratio+1)
    return((pm_divratio_t)(ctz(div_ratio)-1));
  }
}


/**
 ** Module Functions
 **/

long pm_enable_module(unsigned long module)
{
  unsigned long domain = module>>5;
//# Implementation note: the ckMASK registers are contiguous and memory-mapped
//# in that order: CPUMASK, HSBMASK, PBAMASK, PBBMASK.
  unsigned long *regptr = (unsigned long*)(&(AVR32_PM.cpumask) + domain);
  unsigned long regvalue;


  // Read
  regvalue = *regptr;
  // Modify
  regvalue |= (1<<(module%32));
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected ckMASK register
  PM_UNLOCK(AVR32_PM_CPUMASK + domain*sizeof(unsigned long));
  // Write
  *regptr = regvalue;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long pm_disable_module(unsigned long module)
{
  unsigned long domain = module>>5;
//# Implementation note: the ckMASK registers are contiguous and memory-mapped
//# in that order: CPUMASK, HSBMASK, PBAMASK, PBBMASK.
  volatile unsigned long *regptr = (volatile unsigned long*)(&(AVR32_PM.cpumask) + domain);
  unsigned long regvalue;


  // Read
  regvalue = *regptr;
  // Modify
  regvalue &= ~(1<<(module%32));
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected ckMASK register
  PM_UNLOCK(AVR32_PM_CPUMASK + domain*sizeof(unsigned long));
  // Write
  *regptr = regvalue;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}



/**
 ** Sleep Functions
 **/
// Implemented as inline in pm_uc3x.h



/**
 ** Reset Functions
 **/
// Implemented as inline in pm_uc3x.h



/**
 ** Interrupt Functions
 **/
// Implemented as inline in pm_uc3x.h



/**
 ** Misc Functions
 **/
// Implemented as inline in pm_uc3x.h

