/**
 * \file
 *
 * \brief System Control InterFace (SCIF) driver.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "scif_uc3d.h"

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#ifndef AVR32_SCIF_UNLOCK_KEY_VALUE
#define AVR32_SCIF_UNLOCK_KEY_VALUE                         0x000000AA
#endif
#ifndef AVR32_SCIF_OSCCTRL_OSCEN_DISABLE
#define AVR32_SCIF_OSCCTRL_OSCEN_DISABLE                    0x00000000
#endif
#ifndef AVR32_SCIF_OSCCTRL_OSCEN_ENABLE
#define AVR32_SCIF_OSCCTRL_OSCEN_ENABLE                     0x00000001
#endif
#ifndef AVR32_SCIF_OSCCTRL0_GAIN_G0
#define AVR32_SCIF_OSCCTRL0_GAIN_G0                         0x00000000
#define AVR32_SCIF_OSCCTRL0_GAIN_G1                         0x00000001
#define AVR32_SCIF_OSCCTRL0_GAIN_G2                         0x00000002
#define AVR32_SCIF_OSCCTRL0_GAIN_G3                         0x00000003
#endif
#ifndef AVR32_SCIF_OSCCTRL32_OSC32EN_DISABLE
#define AVR32_SCIF_OSCCTRL32_OSC32EN_DISABLE                0x00000000
#endif

/*! \name SCIF Writable Bit-Field Registers
 */
//! @{
typedef union
{
  unsigned long                 oscctrl[1];
  avr32_scif_oscctrl_t          OSCCTRL[1];
} u_avr32_scif_oscctrl_t;

typedef union
{
  unsigned long                 pll[2];
  avr32_scif_pll_t              PLL[2];
} u_avr32_scif_pll_t;

typedef union
{
  unsigned long                 oscctrl32;
  avr32_scif_oscctrl32_t         OSCCTRL32;
} u_avr32_scif_oscctrl32_t;
//! @}


//----- PRIVATE FUNCTIONS

/*! \brief Wait for a status high in the Power and Clocks status register.
 *
 * \param statusMask Mask field of the status to poll [INPUT]
 *
 * \return Status.
 *   \retval 0   Status is high.
 *   \retval <0  SCIF_POLL_TIMEOUT Timeout expired before the status was high.
 */
static long int scif_pclksr_statushigh_wait(unsigned long statusMask)
{
  unsigned int  timeout = SCIF_POLL_TIMEOUT;

  while(!(AVR32_SCIF.pclksr & statusMask))
  {
    if(--timeout == 0)
      return -1;
  }
  return PASS;
}

unsigned long scif_read_gplp(unsigned long gplp)
{
  return AVR32_SCIF.gplp[gplp];
}


void scif_write_gplp(int gplp, unsigned long value)
{
  SCIF_UNLOCK(AVR32_SCIF_GPLP + 4*gplp);
  AVR32_SCIF.gplp[gplp] = value;
}

/**
 ** Power and Clocks Status Functions
 **/
// Implemented as inline in scif_uc3d.h


/**
 ** OSC0/OSC1 Functions
 **/

long int scif_start_osc(scif_osc_t osc, const scif_osc_opt_t *opt, bool wait_for_ready)
{
  u_avr32_scif_oscctrl_t   u_avr32_scif_oscctrl;

#ifdef AVR32SFW_INPUT_CHECK
  // Check that the input frequency is in the supported frequency range.
  if( (opt->freq_hz < SCIF_EXT_CRYSTAL_MIN_FREQ_HZ)
      || (opt->freq_hz > SCIF_EXT_CRYSTAL_MAX_FREQ_HZ))
  {
    return -1;
  }
  // Check : for OSC0, only 2 modes are supported
  if( (opt->mode != SCIF_OSC_MODE_EXT_CLK)
      && (opt->mode != SCIF_OSC_MODE_2PIN_CRYSTAL))
  {
    return -1;
  }

  if (osc == SCIF_OSC0)
  {
    // Check that the startup value is in the supported range.
    if(opt->startup > (unsigned char)AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC)
    {
      return -1;
    }
    // Check that the gain value is in the supported range.
    if(opt->gain > AVR32_SCIF_OSCCTRL0_GAIN_G3)
    {
      return -1;
    }
  }
  else
  {
     return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK
  // Read Register
  u_avr32_scif_oscctrl.OSCCTRL[osc] = AVR32_SCIF.OSCCTRL[osc] ;
  // Modify: Configure & start OSC0.
  u_avr32_scif_oscctrl.OSCCTRL[osc].mode = opt->mode;
  u_avr32_scif_oscctrl.OSCCTRL[osc].gain = opt->gain;
  u_avr32_scif_oscctrl.OSCCTRL[osc].startup = opt->startup;
  u_avr32_scif_oscctrl.OSCCTRL[osc].oscen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL + 4*osc);
  // Write Back
  AVR32_SCIF.OSCCTRL[osc] = u_avr32_scif_oscctrl.OSCCTRL[osc];
  AVR32_LEAVE_CRITICAL_REGION( );

  if(true == wait_for_ready)
  {
		  // Wait until OSC0 is stable and ready to be used.
		  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_OSC0RDY_MASK))
			return -1;
  }
  return PASS;
}


bool scif_is_osc_ready(scif_osc_t osc)
{
  if (osc == SCIF_OSC0)
  {
    return((AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_OSC0RDY_MASK)>>AVR32_SCIF_PCLKSR_OSC0RDY_OFFSET);
  }
  else
  {
    return false;
  }
}


long int scif_stop_osc(scif_osc_t osc)
{
  unsigned long temp;

  temp = AVR32_SCIF.oscctrl[osc];
  temp &= ~(AVR32_SCIF_OSCCTRL_OSCEN_MASK);
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL + 4*osc);
  // Stop OSC0.
  AVR32_SCIF.oscctrl[osc] = temp;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}


long int scif_configure_osc_crystalmode(scif_osc_t osc, unsigned int fcrystal)
{
  u_avr32_scif_oscctrl_t   u_avr32_scif_oscctrl;

  if (osc == SCIF_OSC0)
  {
    // Read Register
    u_avr32_scif_oscctrl.OSCCTRL[SCIF_OSC0] = AVR32_SCIF.OSCCTRL[SCIF_OSC0] ;
    // Modify : Configure the oscillator mode to crystal and set the gain according to the
    // crystal frequency. Gain G3 (for power optimization) is unused and if required will
    // need to be set manually.
    u_avr32_scif_oscctrl.OSCCTRL[SCIF_OSC0].mode = SCIF_OSC_MODE_2PIN_CRYSTAL;
    u_avr32_scif_oscctrl.OSCCTRL[SCIF_OSC0].gain = (fcrystal < 12000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G0 :
                                                   (fcrystal < 16000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G1 :
                                                   AVR32_SCIF_OSCCTRL0_GAIN_G2;
    AVR32_ENTER_CRITICAL_REGION( );
    // Unlock the write-protected OSCCTRL0 register
    SCIF_UNLOCK(AVR32_SCIF_OSCCTRL);
    // Write Back
    AVR32_SCIF.OSCCTRL[SCIF_OSC0] = u_avr32_scif_oscctrl.OSCCTRL[SCIF_OSC0];
    AVR32_LEAVE_CRITICAL_REGION( );
  }
  else
  {
    return FAIL;
  }

  return PASS;
}

long int scif_configure_osc_extmode(scif_osc_t osc)
{
  u_avr32_scif_oscctrl_t   u_avr32_scif_oscctrl;

      // Read Register
    u_avr32_scif_oscctrl.OSCCTRL[osc] = AVR32_SCIF.OSCCTRL[osc] ;
    // Modify : Configure the oscillator mode to crystal and set the gain according to the
    // crystal frequency.
    u_avr32_scif_oscctrl.OSCCTRL[osc].mode = SCIF_OSC_MODE_EXT_CLK;
    AVR32_ENTER_CRITICAL_REGION( );
    // Unlock the write-protected OSCCTRL0 register
    SCIF_UNLOCK(AVR32_SCIF_OSCCTRL + 4*osc);
    // Write Back
    AVR32_SCIF.OSCCTRL[osc] = u_avr32_scif_oscctrl.OSCCTRL[osc];
    AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long int scif_enable_osc(scif_osc_t osc, unsigned int startup, bool wait_for_ready)
{

  u_avr32_scif_oscctrl_t   u_avr32_scif_oscctrl;

  // Read Register
  u_avr32_scif_oscctrl.OSCCTRL[osc] = AVR32_SCIF.OSCCTRL[osc] ;
  // Modify: Configure the oscillator startup and enable the osc.
  u_avr32_scif_oscctrl.OSCCTRL[osc].startup = startup;
  u_avr32_scif_oscctrl.OSCCTRL[osc].oscen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL + 4*osc);
  // Write Back
  AVR32_SCIF.OSCCTRL[osc] = u_avr32_scif_oscctrl.OSCCTRL[osc];
  AVR32_LEAVE_CRITICAL_REGION( );

  if(true == wait_for_ready)
  {
		  // Wait until OSC0 is stable and ready to be used.
		  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_OSC0RDY_MASK))
			return -1;
  }

  return PASS;
}

long int scif_enable_extosc(scif_osc_t osc)
{

  u_avr32_scif_oscctrl_t   u_avr32_scif_oscctrl;

  // Read Register
  u_avr32_scif_oscctrl.OSCCTRL[osc] = AVR32_SCIF.OSCCTRL[osc] ;
  // Modify : Enable the osc.
  u_avr32_scif_oscctrl.OSCCTRL[osc].oscen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL + 4*osc);
  // Write Back
  AVR32_SCIF.OSCCTRL[osc] = u_avr32_scif_oscctrl.OSCCTRL[osc];
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

/**
 ** PLL0/PLL1 Functions
 **/
long int scif_pll_setup(scif_pll_t pll, const scif_pll_opt_t *opt)
{

  u_avr32_scif_pll_t   u_avr32_scif_pll;

  // Read Register
  u_avr32_scif_pll.PLL[pll] = AVR32_SCIF.PLL[pll] ;
  // Modify Configuration
  u_avr32_scif_pll.PLL[pll].pllosc  = opt->osc;
  u_avr32_scif_pll.PLL[pll].pllopt  = opt->pll_freq | (opt->pll_div2 << 1) | (opt->pll_wbwdisable << 2);
  u_avr32_scif_pll.PLL[pll].plldiv  = opt->div;
  u_avr32_scif_pll.PLL[pll].pllmul  = opt->mul;
  u_avr32_scif_pll.PLL[pll].pllcount= opt->lockcount;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected PLL0 register
  SCIF_UNLOCK(AVR32_SCIF_PLL + 4*pll);
  // Write Back
  AVR32_SCIF.PLL[pll] = u_avr32_scif_pll.PLL[pll];
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long int scif_pll_enable(scif_pll_t pll)
{

  u_avr32_scif_pll_t   u_avr32_scif_pll;

    // Read Register
    u_avr32_scif_pll.PLL[pll] = AVR32_SCIF.PLL[pll] ;
    // Modify Configuration
    u_avr32_scif_pll.PLL[pll].pllen = ENABLE;
    AVR32_ENTER_CRITICAL_REGION( );
    // Unlock the write-protected PLL0 register
    SCIF_UNLOCK(AVR32_SCIF_PLL + 4*pll);
    // Write Back
    AVR32_SCIF.PLL[pll] = u_avr32_scif_pll.PLL[pll];
    AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long int scif_pll_disable(scif_pll_t pll)
{

  u_avr32_scif_pll_t   u_avr32_scif_pll;

  // Read Register
  u_avr32_scif_pll.PLL[pll] = AVR32_SCIF.PLL[pll] ;
  // Modify Configuration
  u_avr32_scif_pll.PLL[pll].pllen = DISABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected PLL0 register
  SCIF_UNLOCK(AVR32_SCIF_PLL + 4*pll);
  // Write Back
  AVR32_SCIF.PLL[pll] = u_avr32_scif_pll.PLL[pll];
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long int scif_wait_for_pll_locked(scif_pll_t pll)
{

  if (pll == SCIF_PLL0)
  {
      // Wait until PLL0 is stable and ready to be used.
      while(!(AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_PLL0_LOCK_MASK));
  }
  else
  {
      // Wait until PLL1 is stable and ready to be used.
      while(!(AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_PLL1_LOCK_MASK));
  }

  return PASS;
}

/**
 ** OSC32 Functions
 **/

long int scif_start_osc32(const scif_osc32_opt_t *opt, bool wait_for_ready)
{
  u_avr32_scif_oscctrl32_t  u_avr32_scif_oscctrl32;


#ifdef AVR32SFW_INPUT_CHECK
  // Check that the input frequency is in the supported frequency range.
  if( (opt->freq_hz < SCIF_EXT_CRYSTAL_MIN_FREQ_HZ)
      || (opt->freq_hz > SCIF_EXT_CRYSTAL_MAX_FREQ_HZ))
  {
    return -1;
  }
  // Check : for OSC0/OSC1, only 2 modes are supported
  if( (opt->mode < SCIF_OSC_MODE_EXT_CLK)
      || (opt->mode > SCIF_OSC_MODE_2PIN_CRYSTAL)
      || (opt->mode == SCIF_OSC_MODE_NOT_SUPPORTED_1)
      || (opt->mode == SCIF_OSC_MODE_NOT_SUPPORTED_2) )
  {
    return -1;
  }
  // Check that the startup value is in the supported range.
  if(opt->startup > (unsigned char)AVR32_SCIF_OSCCTRL32_STARTUP_524288_RCOSC)
  {
    return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // Read Register
  u_avr32_scif_oscctrl32.oscctrl32 = AVR32_SCIF.oscctrl32 ;

  // Modify : Configure & start OSC32.
  u_avr32_scif_oscctrl32.OSCCTRL32.mode = opt->mode;
  u_avr32_scif_oscctrl32.OSCCTRL32.startup = opt->startup;
  u_avr32_scif_oscctrl32.OSCCTRL32.osc32en = ENABLE;

  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL32 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL32);

  // Write Back
  AVR32_SCIF.oscctrl32 = u_avr32_scif_oscctrl32.oscctrl32;
  AVR32_LEAVE_CRITICAL_REGION( );

  if(true == wait_for_ready)
  {
    // Wait until OSC32 is stable and ready to be used.
    if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_OSC32RDY_MASK))
      return -1;
  }

  return PASS;
}


long scif_stop_osc32()
{
  unsigned long temp = AVR32_SCIF.oscctrl32;
  temp &= ~AVR32_SCIF_OSCCTRL32_OSC32EN_MASK;;

  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL32 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL32);

  // Stop OSC32.
  AVR32_SCIF.oscctrl32 = temp;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

/**
 ** 120MHz RCosc Functions
 **/

void scif_start_rc120M(void)
{
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected RC120MCR register
  SCIF_UNLOCK(AVR32_SCIF_RC120MCR);
  AVR32_SCIF.rc120mcr = AVR32_SCIF_RC120MCR_EN_MASK;
  AVR32_LEAVE_CRITICAL_REGION( );
}

void scif_stop_rc120M(void)
{
  unsigned long temp = AVR32_SCIF.rc120mcr;

  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected RC120MCR register
  SCIF_UNLOCK(AVR32_SCIF_RC120MCR);
  temp &= ~AVR32_SCIF_RC120MCR_EN_MASK;
  AVR32_SCIF.rc120mcr = temp;
  AVR32_LEAVE_CRITICAL_REGION( );
}


/**
 ** Generic Clock Functions
 **/

long int scif_start_gclk(unsigned int gclk, const scif_gclk_opt_t *opt)
{
#ifdef AVR32SFW_INPUT_CHECK
  // Check that the generic clock number is correct
  if( gclk > AVR32_SCIF_GCLK_NUM )
  {
    return -1;
  }
  // Check that the clock source for the generic clock is correct.
  if(( opt->clock_source >= SCIF_GCCTRL_OSCSEL_INVALID ) || ( opt->clock_source < 0 ))
  {
    return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // If the generic clock is already enabled, return an error.
  if(AVR32_SCIF.gcctrl[gclk] & AVR32_SCIF_GCCTRL_CEN_MASK)
    return -1;

  // Configure & start the generic clock.
  AVR32_SCIF.gcctrl[gclk] = ((opt->divider << AVR32_SCIF_GCCTRL_DIV_OFFSET)&AVR32_SCIF_GCCTRL_DIV_MASK)
                            |((opt->diven << AVR32_SCIF_GCCTRL_DIVEN_OFFSET)&AVR32_SCIF_GCCTRL_DIVEN_MASK)
                            |((opt->clock_source << AVR32_SCIF_GCCTRL_OSCSEL_OFFSET)&AVR32_SCIF_GCCTRL_OSCSEL_MASK)
                            |(AVR32_SCIF_GCCTRL_CEN_MASK);

  return PASS;
}


long int scif_stop_gclk(unsigned int gclk)
{
  unsigned int  timeout = SCIF_POLL_TIMEOUT;

#ifdef AVR32SFW_INPUT_CHECK
  // Check that the generic clock number is correct
  if( gclk > AVR32_SCIF_GCLK_NUM )
  {
    return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // Stop the generic clock.
  AVR32_SCIF.gcctrl[gclk] &= ~AVR32_SCIF_GCCTRL_CEN_MASK;

  // Wait until the generic clock is actually stopped.
  while(AVR32_SCIF.gcctrl[gclk] & AVR32_SCIF_GCCTRL_CEN_MASK)
  {
    if(--timeout == 0)
      return -1;
  }

  return PASS;
}


long int scif_gc_setup(unsigned int gclk, scif_gcctrl_oscsel_t clk_src, unsigned int diven, unsigned int divfactor)
{
  bool restart_gc = false;


  // Change the division factor to conform to the equation: fgclk = fsrc/divfactor = fsrc/(2*(div+1))
  divfactor = (divfactor>>1) -1;

#ifdef AVR32SFW_INPUT_CHECK
  // Check that the generic clock number is correct
  if( gclk > AVR32_SCIF_GCLK_NUM )
  {
    return -1;
  }
  // Check that the clock source for the generic clock is correct.
  if(( clk_src >= SCIF_GCCTRL_OSCSEL_INVALID ) || ( clk_src < 0 ))
  {
    return -1;
  }
  // Check that the required division factor is correct.
  if(diven)
  {
    if(divfactor >= (1<<AVR32_SCIF_GCCTRL_DIV_SIZE))
      return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // If the generic clock is already enabled, disable it before changing its setup.
  if(AVR32_SCIF.gcctrl[gclk] & AVR32_SCIF_GCCTRL_CEN_MASK)
  {
    restart_gc = true;
    if(scif_stop_gclk(gclk) < 0)
      return -1;  // Could not stop the generic clock.
  }

  // Setup the generic clock.
  AVR32_SCIF.gcctrl[gclk] = ((divfactor << AVR32_SCIF_GCCTRL_DIV_OFFSET)&AVR32_SCIF_GCCTRL_DIV_MASK)
                            |((diven << AVR32_SCIF_GCCTRL_DIVEN_OFFSET)&AVR32_SCIF_GCCTRL_DIVEN_MASK)
                            |((clk_src << AVR32_SCIF_GCCTRL_OSCSEL_OFFSET)&AVR32_SCIF_GCCTRL_OSCSEL_MASK);

  // Restart the gc if it previously was enabled.
  if(true == restart_gc)
    AVR32_SCIF.gcctrl[gclk] |= AVR32_SCIF_GCCTRL_CEN_MASK ;

  return PASS;
}


long int scif_gc_enable(unsigned int gclk)
{
#ifdef AVR32SFW_INPUT_CHECK
  // Check that the generic clock number is correct
  if( gclk > AVR32_SCIF_GCLK_NUM )
  {
    return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // If the generic clock is already enabled, do nothing.
  if(!(AVR32_SCIF.gcctrl[gclk] & AVR32_SCIF_GCCTRL_CEN_MASK))
    AVR32_SCIF.gcctrl[gclk] |= AVR32_SCIF_GCCTRL_CEN_MASK;

  return PASS;

}

void scif_bod18_enable_irq(void)
{
  AVR32_SCIF.ier = AVR32_SCIF_IER_BODDET_MASK ;
}

void scif_bod18_disable_irq(void)
{
  Bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  AVR32_SCIF.idr = AVR32_SCIF_IDR_BODDET_MASK;
  AVR32_SCIF.isr;
  if (global_interrupt_enabled) Enable_global_interrupt();
}

void scif_bod18_clear_irq(void)
{
  AVR32_SCIF.icr = AVR32_SCIF_ICR_BODDET_MASK;
}

unsigned long scif_bod18_get_irq_status(void)
{
  return ((AVR32_SCIF.isr & AVR32_SCIF_ISR_BODDET_MASK) != 0);
}

unsigned long scif_bod18_get_irq_enable_bit(void)
{
  return ((AVR32_SCIF.imr & AVR32_SCIF_IMR_BODDET_MASK) != 0);
}

unsigned long scif_bod18_get_level(void)
{
  return (AVR32_SCIF.bod & AVR32_SCIF_BOD_LEVEL_MASK) >> AVR32_SCIF_BOD_LEVEL_OFFSET;
}
