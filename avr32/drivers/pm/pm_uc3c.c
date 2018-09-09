/*****************************************************************************
 *
 * \file
 *
 * \brief Power Manager(PM) driver interface.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "pm_uc3c.h"

#ifndef AVR32_PM_VERSION_RESETVALUE
  #error Cannot use this software module with the current device.
#else
  #if AVR32_PM_VERSION_RESETVALUE < 0x400
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

typedef union
{
  unsigned long                 pbcsel;
  avr32_pm_pbcsel_t             PBCSEL;
} u_avr32_pm_pbcsel_t;

//! @}


/**
 ** Clock Functions
 **/

long pm_set_mclk_source(pm_clk_src_t src)
{
  // Unlock the write-protected MCCTRL register
  AVR32_ENTER_CRITICAL_REGION( );
  PM_UNLOCK(AVR32_PM_MCCTRL);
  AVR32_PM.mcctrl = src;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long pm_config_mainclk_safety(bool cfd, bool final)
{
  u_avr32_pm_cfdctrl_t u_avr32_pm_cfdctrl = {AVR32_PM.cfdctrl};

  // Check if the CFDCTRL register is read-only.
  if(AVR32_PM.cfdctrl & AVR32_PM_CFDCTRL_SFV_MASK)
    return -1;

  // Unlock the write-protected CFDCTRL register
  AVR32_ENTER_CRITICAL_REGION( );
  // Modify
  u_avr32_pm_cfdctrl.CFDCTRL.cfden = cfd;
  u_avr32_pm_cfdctrl.CFDCTRL.sfv = final;
  // Write back
  PM_UNLOCK(AVR32_PM_CFDCTRL);
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
  if(!(AVR32_PM.sr & AVR32_PM_SR_CKRDY_MASK))
    return -1;

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

//# Implementation note: the CPUSEL and PBASEL and PBBSEL and PBCSEL registers all have the
//# same structure.

//# Implementation note: the ckSEL registers are contiguous and memory-mapped in
//# that order: CPUSEL, HSBSEL, PBASEL, PBBSEL, PBCSEL.

  // ckSEL must not be written while SR.CKRDY is 0.
  if(!(AVR32_PM.sr & AVR32_PM_SR_CKRDY_MASK))
    return -1;

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
  PM_UNLOCK(AVR32_PM_CPUMASK + domain*sizeof(avr32_pm_cpumask_t));
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
  PM_UNLOCK(AVR32_PM_CPUMASK + domain*sizeof(avr32_pm_cpumask_t));
  // Write
  *regptr = regvalue;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}



/**
 ** Sleep Functions
 **/
// Implemented as inline in pm_uc3c.h



/**
 ** Reset Functions
 **/
// Implemented as inline in pm_uc3c.h



/**
 ** Interrupt Functions
 **/
// Implemented as inline in pm_uc3c.h



/**
 ** Misc Functions
 **/
// Implemented as inline in pm_uc3c.h
