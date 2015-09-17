/*****************************************************************************
 *
 * \file
 *
 * \brief System Control InterFace(SCIF) driver.
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
#include "scif_uc3l.h"


/*! \name SCIF Writable Bit-Field Registers
 */
//! @{

typedef union
{
  unsigned long                 oscctrl0;
  avr32_scif_oscctrl0_t         OSCCTRL0;
} u_avr32_scif_oscctrl0_t;

typedef union
{
  unsigned long                 oscctrl32;
  avr32_scif_oscctrl32_t         OSCCTRL32;
} u_avr32_scif_oscctrl32_t;

typedef union
{
  unsigned long                 dfll0conf;
  avr32_scif_dfll0conf_t        DFLL0CONF;
} u_avr32_scif_dfll0conf_t;

typedef union
{
  unsigned long                 dfll0ssg;
  avr32_scif_dfll0ssg_t         DFLL0SSG;
} u_avr32_scif_dfll0ssg_t;

#if (UC3L0128 || UC3L0256 || UC3L3_L4)
typedef union
{
  unsigned long                 pll0;
  avr32_scif_pll0_t              PLL0;
} u_avr32_scif_pll0_t;
#endif

//! @}


/**
 ** Interrupt Functions
 **/
// Implemented as inline in scif_uc3l.h


/**
 ** Power and Clocks Status Functions
 **/
// Implemented as inline in scif_uc3l.h


/**
 ** OSC0/OSC1 Functions
 **/

long int scif_start_osc(scif_osc_t osc, const scif_osc_opt_t *opt, bool wait_for_ready)
{
//# Implementation note: this code doesn't consider the osc input parameter
//# because UC3L devices only implement OSC0.
  u_avr32_scif_oscctrl0_t   u_avr32_scif_oscctrl0 = {AVR32_SCIF.oscctrl0};


#ifdef AVR32SFW_INPUT_CHECK
  // Check that the input frequency is in the supported frequency range.
  if( (opt->freq_hz < SCIF_EXT_CRYSTAL_MIN_FREQ_HZ)
      || (opt->freq_hz > SCIF_EXT_CRYSTAL_MAX_FREQ_HZ))
  {
    return -1;
  }
  // Check : for OSC0/OSC1, only 2 modes are supported
  if( (opt->mode != SCIF_OSC_MODE_EXT_CLK)
      && (opt->mode != SCIF_OSC_MODE_2PIN_CRYSTAL))
  {
    return -1;
  }
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
#endif  // AVR32SFW_INPUT_CHECK

  // Configure & start OSC0.
  u_avr32_scif_oscctrl0.OSCCTRL0.mode = opt->mode;
  u_avr32_scif_oscctrl0.OSCCTRL0.gain = opt->gain;
  u_avr32_scif_oscctrl0.OSCCTRL0.startup = opt->startup;
  u_avr32_scif_oscctrl0.OSCCTRL0.oscen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
  // Write
  AVR32_SCIF.oscctrl0 = u_avr32_scif_oscctrl0.oscctrl0;
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
//# Implementation note: this code doesn't consider the osc input parameter
//# because UC3L devices only implement OSC0.
  return((AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_OSC0RDY_MASK)>>AVR32_SCIF_PCLKSR_OSC0RDY_OFFSET);
}


long int scif_stop_osc(scif_osc_t osc)
{
//# Implementation note: this code doesn't consider the osc input parameter
//# because UC3L devices only implement OSC0.
  unsigned long temp = AVR32_SCIF.oscctrl0;
  temp &= ~AVR32_SCIF_OSCCTRL0_OSCEN_MASK;

  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
  // Stop OSC0.
  AVR32_SCIF.oscctrl0 = temp;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}


long int scif_configure_osc_crystalmode(scif_osc_t osc, unsigned int fcrystal)
{
//# Implementation note: this code doesn't consider the osc input parameter
//# because UC3L devices only implement OSC0.
  u_avr32_scif_oscctrl0_t   u_avr32_scif_oscctrl0 = {AVR32_SCIF.oscctrl0};

  // Configure the oscillator mode to crystal and set the gain according to the
  // crystal frequency. Gain G3 (for power optimization) is unused and if 
  // required will need to be set manually.
  u_avr32_scif_oscctrl0.OSCCTRL0.mode = SCIF_OSC_MODE_2PIN_CRYSTAL;
  u_avr32_scif_oscctrl0.OSCCTRL0.gain = (fcrystal < 12000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G0 :
                                        (fcrystal < 16000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G1 :
                                        AVR32_SCIF_OSCCTRL0_GAIN_G2;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
  // Write
  AVR32_SCIF.oscctrl0 = u_avr32_scif_oscctrl0.oscctrl0;
  AVR32_LEAVE_CRITICAL_REGION( );
  // Add here after support for OSC1 for devices that implement OSC1.

  return PASS;
}

long int scif_configure_osc_extmode(scif_osc_t osc)
{
  u_avr32_scif_oscctrl0_t   u_avr32_scif_oscctrl0 = {AVR32_SCIF.oscctrl0};

      // Read Register
    u_avr32_scif_oscctrl0.OSCCTRL0 = AVR32_SCIF.OSCCTRL0 ;
    // Modify : Configure the oscillator mode to crystal and set the gain according to the
    // crystal frequency.
    u_avr32_scif_oscctrl0.OSCCTRL0.mode = SCIF_OSC_MODE_EXT_CLK;
    AVR32_ENTER_CRITICAL_REGION( );
    // Unlock the write-protected OSCCTRL0 register
    SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
    // Write Back
    AVR32_SCIF.OSCCTRL0 = u_avr32_scif_oscctrl0.OSCCTRL0;
    AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}

long int scif_enable_osc(scif_osc_t osc, unsigned int startup, bool wait_for_ready)
{
//# Implementation note: this code doesn't consider the osc input parameter
//# because UC3L devices only implement OSC0.
  u_avr32_scif_oscctrl0_t   u_avr32_scif_oscctrl0 = {AVR32_SCIF.oscctrl0};


  // Configure the oscillator startup and enable the osc.
  u_avr32_scif_oscctrl0.OSCCTRL0.startup = startup;
  u_avr32_scif_oscctrl0.OSCCTRL0.oscen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL0 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
  // Write
  AVR32_SCIF.oscctrl0 = u_avr32_scif_oscctrl0.oscctrl0;
  AVR32_LEAVE_CRITICAL_REGION( );

  if(true == wait_for_ready)
  {
    // Wait until OSC0 is stable and ready to be used.
    if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_OSC0RDY_MASK))
      return -1;
  }

  return PASS;
}


/**
 ** OSC32 Functions
 **/

long int scif_start_osc32(const scif_osc32_opt_t *opt, bool wait_for_ready)
{
  u_avr32_scif_oscctrl32_t  u_avr32_scif_oscctrl32 = {AVR32_SCIF.oscctrl32};


#ifdef AVR32SFW_INPUT_CHECK
  // Check that the input frequency is in the supported frequency range.
  if( (opt->freq_hz < SCIF_EXT_CRYSTAL_MIN_FREQ_HZ)
      || (opt->freq_hz > SCIF_EXT_CRYSTAL_MAX_FREQ_HZ))
  {
    return -1;
  }
  // Check : for OSC32, 3 modes are supported
  if( (opt->mode != SCIF_OSC_MODE_EXT_CLK)
      && (opt->mode != SCIF_OSC_MODE_2PIN_CRYSTAL)
      && (opt->mode != SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR)
    )
  {
    return -1;
  }
  // Check that the startup value is in the supported range.
  if(opt->startup > (unsigned char)AVR32_SCIF_OSCCTRL32_STARTUP_524288_RCOSC)
  {
    return -1;
  }
#endif  // AVR32SFW_INPUT_CHECK

  // Note: RC32K is automatically output on PA20 upon reset, which is the pin
  // for XOUT32_2 => we must disable this output before setting-up the OSC32K
  // (in case the OSC32 is connected to XIN32_2/XOUT32_2).
  scif_disable_rc32out();

  // Configure & start OSC32.
  u_avr32_scif_oscctrl32.OSCCTRL32.mode = opt->mode;
  u_avr32_scif_oscctrl32.OSCCTRL32.pinsel = opt->pinsel;
  u_avr32_scif_oscctrl32.OSCCTRL32.en32k = opt->en32k;
  u_avr32_scif_oscctrl32.OSCCTRL32.en1k = opt->en1k;
  u_avr32_scif_oscctrl32.OSCCTRL32.startup = opt->startup;
  u_avr32_scif_oscctrl32.OSCCTRL32.osc32en = ENABLE;
  // Note: the OSCCTRL32 register is protected by a lock. To safely unlock then
  // write in the register, perform this operation inside a critical region.
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected OSCCTRL32 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL32);
  // write
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
  temp &= ~AVR32_SCIF_OSCCTRL32_OSC32EN_MASK;

  // Note: the OSCCTRL32 register is protected by a lock.
  // Unlock the write-protected OSCCTRL32 register
  SCIF_UNLOCK(AVR32_SCIF_OSCCTRL32);
  // Stop OSC32.
  AVR32_SCIF.oscctrl32 = temp;

  return PASS;
}



/**
 ** DFLL Control Functions
 **/

//! The different DFLL0 modes
typedef enum
{
  SCIF_DFLL0_MODE_OPENLOOP = 0,
  SCIF_DFLL0_MODE_CLOSEDLOOP
} scif_dfll_mode_t;

long int scif_dfll0_openloop_start(const scif_dfll_openloop_conf_t *pdfllconfig)
{
  u_avr32_scif_dfll0conf_t  u_avr32_scif_dfll0conf = {AVR32_SCIF.dfll0conf};


#ifdef AVR32SFW_INPUT_CHECK
  if((pdfllconfig->fine >> AVR32_SCIF_DFLL0CONF_FINE_SIZE))
    return -1;
  if((pdfllconfig->coarse >> AVR32_SCIF_DFLL0CONF_COARSE_SIZE))
    return -1;
#endif

  // Enable the DFLL0: DFLL0CONF.EN=1
  u_avr32_scif_dfll0conf.DFLL0CONF.en = ENABLE;
#if (UC3L0128 || UC3L0256 || UC3L3_L4)
  u_avr32_scif_dfll0conf.DFLL0CONF.ccen = ENABLE;
#endif
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Set the DFLL0 to operate in open mode: DFLL0CONF.MODE=0
  u_avr32_scif_dfll0conf.DFLL0CONF.mode = SCIF_DFLL0_MODE_OPENLOOP;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Write DFLL0CONF.COARSE & DFLL0CONF.FINE
  u_avr32_scif_dfll0conf.DFLL0CONF.coarse = pdfllconfig->coarse;
  u_avr32_scif_dfll0conf.DFLL0CONF.fine = pdfllconfig->fine;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  return PASS;
}


#define SCIF_DFLL_COARSE_MAX  (AVR32_SCIF_COARSE_MASK >> AVR32_SCIF_COARSE_OFFSET)
#define SCIF_DFLL_FINE_MAX    (AVR32_SCIF_FINE_MASK >> AVR32_SCIF_FINE_OFFSET)
#define SCIF_DFLL_FINE_HALF   (1 << (AVR32_SCIF_DFLL0CONF_FINE_SIZE-1))
long int scif_dfll0_openloop_start_auto(unsigned long TargetFreqkHz)
{
  scif_dfll_openloop_conf_t Dfll0Conf;
  unsigned long             Coarse;
  unsigned long             Fine;
  unsigned long             CoarseFreq;
  unsigned long             DeltaFreq;


#ifdef AVR32SFW_INPUT_CHECK
  if((TargetFreqkHz < SCIF_DFLL_MINFREQ_KHZ) || (TargetFreqkHz > SCIF_DFLL_MAXFREQ_KHZ))
    return -1;
#endif

  //**
  //** Dynamically compute the COARSE and FINE values.
  //**
  // Fdfll = (Fmin+(Fmax-Fmin)*(COARSE/0xFF))*(1+X*(FINE-0x100)/0x1FF)=CoarseFreq*(1+X*(FINE-0x100)/0x1FF)

  // Compute the COARSE value.
  Coarse = ((TargetFreqkHz - SCIF_DFLL_MINFREQ_KHZ)*SCIF_DFLL_COARSE_MAX)/(SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ);
  // Compute the COARSE DFLL frequency.
  CoarseFreq = SCIF_DFLL_MINFREQ_KHZ + (((SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ)/SCIF_DFLL_COARSE_MAX)*Coarse);
  // Compute the coarse error.
  DeltaFreq = TargetFreqkHz - CoarseFreq;
  // Compute the FINE value.
  // Fine = ((DeltaFreq*SCIF_DFLL_FINE_MAX)*10/CoarseFreq) + SCIF_DFLL_FINE_HALF;
  // Theoretical equation don't work on silicon: the best was to use X=5/2 to
  // find FINE, then do FINE/4.
  Fine = ((DeltaFreq*SCIF_DFLL_FINE_MAX)*2/CoarseFreq*5) + SCIF_DFLL_FINE_HALF;
  Fine >>=2;

  Dfll0Conf.coarse = Coarse;
  Dfll0Conf.fine = Fine;
  return(scif_dfll0_openloop_start(&Dfll0Conf));
}

long int scif_dfll0_openloop_updatefreq(const scif_dfll_openloop_conf_t *pdfllconfig)
{
  u_avr32_scif_dfll0conf_t  u_avr32_scif_dfll0conf = {AVR32_SCIF.dfll0conf};


#ifdef AVR32SFW_INPUT_CHECK
  if((pdfllconfig->fine >> AVR32_SCIF_DFLL0CONF_FINE_SIZE))
    return -1;
#endif

  // It is assumed that the DFLL is enabled and operates in open-loop mode.

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Write DFLL0CONF.COARSE & DFLL0CONF.FINE
  u_avr32_scif_dfll0conf.DFLL0CONF.coarse = pdfllconfig->coarse;
  u_avr32_scif_dfll0conf.DFLL0CONF.fine = pdfllconfig->fine;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  return PASS;
}


long int scif_dfll0_openloop_stop(void)
{
  u_avr32_scif_dfll0conf_t  u_avr32_scif_dfll0conf = {AVR32_SCIF.dfll0conf};


  // Before disabling the DFLL, the output freq of the DFLL should be set to a
  // minimum: set COARSE to 0x00.

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Write DFLL0CONF.COARSE
  u_avr32_scif_dfll0conf.DFLL0CONF.coarse = 0;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Disable the DFLL
  u_avr32_scif_dfll0conf.DFLL0CONF.en = 0;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  return PASS;
}


long int scif_dfll0_ssg_enable(scif_dfll_ssg_conf_t *pssg_conf)
{
  u_avr32_scif_dfll0ssg_t  u_avr32_scif_dfll0ssg = {AVR32_SCIF.dfll0ssg};


  u_avr32_scif_dfll0ssg.DFLL0SSG.en = ENABLE;
  u_avr32_scif_dfll0ssg.DFLL0SSG.prbs = pssg_conf->use_random;
  u_avr32_scif_dfll0ssg.DFLL0SSG.amplitude = pssg_conf->amplitude;
  u_avr32_scif_dfll0ssg.DFLL0SSG.stepsize = pssg_conf->step_size;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0SSG);
  AVR32_SCIF.dfll0ssg = u_avr32_scif_dfll0ssg.dfll0ssg;
  AVR32_LEAVE_CRITICAL_REGION( );

  return PASS;
}


long int scif_dfll0_closedloop_start(const scif_dfll_closedloop_conf_t *pdfllconfig)
{
  u_avr32_scif_dfll0conf_t  u_avr32_scif_dfll0conf = {AVR32_SCIF.dfll0conf};
  volatile unsigned long    tempo;


#ifdef AVR32SFW_INPUT_CHECK
  if((pdfllconfig->coarse >> AVR32_SCIF_DFLL0CONF_COARSE_SIZE))
    return -1;

  if( (pdfllconfig->finemaxstep >> AVR32_SCIF_DFLL0STEP_FSTEP_SIZE)
   || (pdfllconfig->coarsemaxstep >> AVR32_SCIF_DFLL0STEP_CSTEP_SIZE) )
    return -1;
#endif

  // Enable the DFLL0: DFLL0CONF.EN=1
  u_avr32_scif_dfll0conf.DFLL0CONF.en = ENABLE;
#if (UC3L0128 || UC3L0256 || UC3L3_L4)
  u_avr32_scif_dfll0conf.DFLL0CONF.ccen = ENABLE;
#endif
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Set the maxstep values
  tempo = ((pdfllconfig->coarsemaxstep << AVR32_SCIF_DFLL0STEP_CSTEP_OFFSET)&AVR32_SCIF_DFLL0STEP_CSTEP_MASK)
          | ((pdfllconfig->finemaxstep << AVR32_SCIF_DFLL0STEP_FSTEP_OFFSET)&AVR32_SCIF_DFLL0STEP_FSTEP_MASK);
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0STEP);
  AVR32_SCIF.dfll0step = tempo;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Set the fmul
  AVR32_ENTER_CRITICAL_REGION( );
#if AVR32_SCIF_H_VERSION < 110
  SCIF_UNLOCK(AVR32_SCIF_DFLL0FMUL);
  AVR32_SCIF.dfll0fmul = (pdfllconfig->imul << AVR32_SCIF_DFLL0FMUL_FMUL_OFFSET)&AVR32_SCIF_DFLL0FMUL_FMUL_MASK;
#else
  SCIF_UNLOCK(AVR32_SCIF_DFLL0MUL);
  AVR32_SCIF.dfll0mul = ((pdfllconfig->fmul << AVR32_SCIF_DFLL0MUL_FMUL_OFFSET)&AVR32_SCIF_DFLL0MUL_FMUL_MASK)
    | ((pdfllconfig->imul << AVR32_SCIF_DFLL0MUL_IMUL_OFFSET)&AVR32_SCIF_DFLL0MUL_IMUL_MASK);
#endif
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Set the DFLL0 to operate in closed-loop mode: DFLL0CONF.MODE=1
  u_avr32_scif_dfll0conf.DFLL0CONF.mode = SCIF_DFLL0_MODE_CLOSEDLOOP;
  u_avr32_scif_dfll0conf.DFLL0CONF.coarse = pdfllconfig->coarse;
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = u_avr32_scif_dfll0conf.dfll0conf;
  AVR32_LEAVE_CRITICAL_REGION( );

  // Wait for PCLKSR.DFLL0RDY is high
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0RDY_MASK))
    return -1;

  // Wait until the DFLL is locked on Fine value, and is ready to be selected as
  // clock source with a highly accurate output clock.
  if(scif_pclksr_statushigh_wait(AVR32_SCIF_PCLKSR_DFLL0LOCKF_MASK))
    return -1;

  return PASS;
}


long int scif_dfll0_closedloop_configure_and_start( const scif_gclk_opt_t *gc_dfllif_ref_opt,
                                                    unsigned long long target_freq_hz,
                                                    bool enable_ssg)
{
  scif_dfll_closedloop_conf_t DfllConfig;
  int gc_source_clock_freq_hz;

  // This function only supports the following source clocks for the CLK_DFLLIF_REF generic clock:
  // SCIF_GCCTRL_SLOWCLOCK (aka RCSYS), SCIF_GCCTRL_OSC32K, SCIF_GCCTRL_RC32K,
  // SCIF_GCCTRL_OSC0, SCIF_GCCTRL_RC120M, SCIF_GCCTRL_CLK1K.
  if(SCIF_GCCTRL_SLOWCLOCK == gc_dfllif_ref_opt->clock_source)
    gc_source_clock_freq_hz = SCIF_SLOWCLOCK_FREQ_HZ;
  else if((SCIF_GCCTRL_OSC32K == gc_dfllif_ref_opt->clock_source) || (SCIF_GCCTRL_RC32K == gc_dfllif_ref_opt->clock_source))
    gc_source_clock_freq_hz = SCIF_RC32K_FREQ_HZ;
  else if(SCIF_GCCTRL_OSC0 == gc_dfllif_ref_opt->clock_source)
    gc_source_clock_freq_hz = gc_dfllif_ref_opt->extosc_f;
  else if(SCIF_GCCTRL_RC120M == gc_dfllif_ref_opt->clock_source)
    gc_source_clock_freq_hz = SCIF_RC120M_FREQ_HZ;
  else if(SCIF_GCCTRL_CLK1K == gc_dfllif_ref_opt->clock_source)
    gc_source_clock_freq_hz = 1000;
  else
    return -1;

  // Configure and start the DFLL main reference generic clock (CLK_DFLLIF_REF).
  if(scif_dfll0_closedloop_mainref_gc_enable(gc_dfllif_ref_opt))
    return(-1);

  // Configure the DFLL.
  // The coarse value (= (dfll_f - SCIF_DFLL_MINFREQ_KHZ)*255/(SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ))
  DfllConfig.coarse = ((unsigned long long)(target_freq_hz - SCIF_DFLL_MINFREQ_HZ)*255)/(SCIF_DFLL_MAXFREQ_HZ - SCIF_DFLL_MINFREQ_HZ);

  // imul = (fDFLL)/fref,
  // fmul = (fDFLL*2^16)/fref - imul*2^16,
  // with fref being the frequency of the DFLL main reference generic clock
  // and fDFLL being the target frequency of the DFLL
  DfllConfig.imul = ((unsigned long long)target_freq_hz)/gc_source_clock_freq_hz;
  if(0 == gc_dfllif_ref_opt->diven)
  {
    DfllConfig.fmul = ((unsigned long long)target_freq_hz<<16)/gc_source_clock_freq_hz - ((unsigned long long)(DfllConfig.imul)<<16);
  }
  else
    DfllConfig.fmul = ((((unsigned long long)target_freq_hz<<16)/gc_source_clock_freq_hz - ((unsigned long long)(DfllConfig.imul)<<16))<<1)*(1+gc_dfllif_ref_opt->divider);

  // The fine and coarse maxstep values
  DfllConfig.finemaxstep = 0x0000004;
  DfllConfig.coarsemaxstep = 0x0000004;

  // Dithering disabled.

  // Configure and start the DFLL0 in closed loop mode.
  if(scif_dfll0_closedloop_start(&DfllConfig))
    return -1;

  // TODO: Future implementation note: add use of the generic clock CLK_DFLLIF_DITHER
  // as a reference for the SSG feature.
  if(true == enable_ssg)
  {
  ;
  }
  return PASS;
}

/**
 ** Calibration Functions
 **/


/**
 ** Critical Path Oscillator Functions
 **/


#if ( UC3L0128 || UC3L0256  || UC3L3_L4 )

/**
 ** PLL0 Functions
 **/
long int scif_pll0_setup(const scif_pll_opt_t *opt)
{

  u_avr32_scif_pll0_t   u_avr32_scif_pll0;

  // Read Register
  u_avr32_scif_pll0.PLL0 = AVR32_SCIF.PLL0 ;
  // Modify Configuration
  u_avr32_scif_pll0.PLL0.pllosc  = opt->osc;
  u_avr32_scif_pll0.PLL0.pllopt  = opt->pll_freq | (opt->pll_div2 << 1) | (opt->pll_wbwdisable << 2);
  u_avr32_scif_pll0.PLL0.plldiv   = opt->div;
  u_avr32_scif_pll0.PLL0.pllmul  = opt->mul;
  u_avr32_scif_pll0.PLL0.pllcount= opt->lockcount;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected PLL0 register
  SCIF_UNLOCK(AVR32_SCIF_PLL0);
  // Write Back
  AVR32_SCIF.PLL0 = u_avr32_scif_pll0.PLL0;
  AVR32_LEAVE_CRITICAL_REGION( );

  return 0;
}

long int scif_pll0_enable(void)
{

  u_avr32_scif_pll0_t   u_avr32_scif_pll0;

  // Read Register
  u_avr32_scif_pll0.PLL0 = AVR32_SCIF.PLL0 ;
  // Modify Configuration
  u_avr32_scif_pll0.PLL0.pllen = ENABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected PLL0 register
  SCIF_UNLOCK(AVR32_SCIF_PLL0);
  // Write Back
  AVR32_SCIF.PLL0 = u_avr32_scif_pll0.PLL0;
  AVR32_LEAVE_CRITICAL_REGION( );

  return 0;
}

long int scif_pll0_disable(void)
{

  u_avr32_scif_pll0_t   u_avr32_scif_pll0;

  // Read Register
  u_avr32_scif_pll0.PLL0 = AVR32_SCIF.PLL0 ;
  // Modify Configuration
  u_avr32_scif_pll0.PLL0.pllen = DISABLE;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected PLL0 register
  SCIF_UNLOCK(AVR32_SCIF_PLL0);
  // Write Back
  AVR32_SCIF.PLL0 = u_avr32_scif_pll0.PLL0;
  AVR32_LEAVE_CRITICAL_REGION( );

  return 0;
}

long int scif_wait_for_pll0_locked(void)
{
  // Wait until PLL0 is stable and ready to be used.
  while(!(AVR32_SCIF.pclksr & AVR32_SCIF_PCLKSR_PLLLOCK0_MASK));

  return 0;
}

#endif // #if ( UC3L0128 || UC3L0256 )

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

  temp &= ~AVR32_SCIF_RC120MCR_EN_MASK;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected RC120MCR register
  SCIF_UNLOCK(AVR32_SCIF_RC120MCR);
  AVR32_SCIF.rc120mcr = temp;
  AVR32_LEAVE_CRITICAL_REGION( );
}



/**
 ** 32kHz internal RCosc (RC32K) Functions
 **/
void scif_start_rc32k(void)
{
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected RC32KCR register
  SCIF_UNLOCK(AVR32_SCIF_RC32KCR);
  AVR32_SCIF.rc32kcr = AVR32_SCIF_RC32KCR_EN_MASK;
  AVR32_LEAVE_CRITICAL_REGION( );
}

void scif_stop_rc32k(void)
{
  unsigned long temp = AVR32_SCIF.rc32kcr;

  temp &= ~AVR32_SCIF_RC32KCR_EN_MASK;
  AVR32_ENTER_CRITICAL_REGION( );
  // Unlock the write-protected RC32KCR register
  SCIF_UNLOCK(AVR32_SCIF_RC32KCR);
  AVR32_SCIF.rc32kcr = temp;
  AVR32_LEAVE_CRITICAL_REGION( );
}

void scif_disable_rc32out(void)
{
  unsigned long temp;
  AVR32_ENTER_CRITICAL_REGION( );
  temp = AVR32_PM.ppcr & (~AVR32_PM_PPCR_FRC32_MASK);
  // Un-force the RC32 signal from being output on the dedicated pin (PA20).
  AVR32_PM.unlock = 0xAA000000 | AVR32_PM_PPCR;
  AVR32_PM.ppcr = temp;
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
    AVR32_SCIF.gcctrl[gclk] |= (AVR32_SCIF_GCCTRL_CEN_MASK);

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
    AVR32_SCIF.gcctrl[gclk] |= (AVR32_SCIF_GCCTRL_CEN_MASK);

  return PASS;
}

/**
 ** Backup Registers Functions
 **/


/**
 ** Misc
 **/

/*! \brief Wait for a status high in the Power and Clocks status register.
 *
 * \param statusMask Mask field of the status to poll [INPUT]
 *
 * \return Status.
 *   \retval 0   Status is high.
 *   \retval <0  SCIF_POLL_TIMEOUT Timeout expired before the status was high.
 */
long int scif_pclksr_statushigh_wait(unsigned long statusMask)
{
  unsigned int  timeout = SCIF_POLL_TIMEOUT;

  while(!(AVR32_SCIF.pclksr & statusMask))
  {
    if(--timeout == 0)
      return -1;
  }
  return PASS;
}
