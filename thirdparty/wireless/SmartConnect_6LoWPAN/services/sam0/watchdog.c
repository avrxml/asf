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

#include "watchdog.h"
#include "asf.h"
#include "wdt.h"
#include "leds.h"
//#include "instance_wdt.h"
/*---------------------------------------------------------------------------*/
/*
 * Note: for some reason, a device must have rebooted manually
 * at least once for the WDT to be able to reboot the device either
 * by watchdog_reboot() or through expiry.
 */

/* Watchdog timeout, in milliseconds */
#define WDT_PERIOD_17         WDT_PERIOD_512CLK
#define WDT_PERIOD_34         WDT_PERIOD_1024CLK
#define WDT_PERIOD_68         WDT_PERIOD_2048CLK
#define WDT_PERIOD_125        WDT_PERIOD_4096CLK
#define WDT_PERIOD_250        WDT_PERIOD_8192CLK
#define WDT_PERIOD_500        WDT_PERIOD_16384CLK

/* WDT count clear register and magic byte (write anything but this == reset) */
#define WDT_CLEAR              *((uint8_t *)0x40001008U)
#define WDT_CLEAR_MAGIC        0xA5

/* WDT states */
enum wdt_state {
  NOT_INITED  = 0,
  ACTIVE      = 1,
  INACTIVE    = 2,
};
static volatile int status = NOT_INITED;
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initiate the Watchdog timer, but does not start it
 */
void
watchdog_init(void)
{
  if(status == NOT_INITED) {
    struct wdt_conf wdt;
	wdt_get_config_defaults(&wdt);
    wdt.always_on            = false;
    wdt.clock_source         = GCLK_GENERATOR_4;    /* Use the 32.768 kHz clock */
    wdt.timeout_period       = WDT_PERIOD_16384CLK;      /* WDT expiry time */
    //wdt.window_period        = WDT_PERIOD_NONE;
   // wdt.early_warning_period = WDT_PERIOD_NONE;
    wdt_set_config(&wdt);
    status = INACTIVE;
  }
}


/*---------------------------------------------------------------------------*/
/**
 * \brief      Reset the Watchdog timer count
 */
void
watchdog_periodic(void)
{
  if(status == ACTIVE) {
    WDT_CLEAR = WDT_CLEAR_MAGIC;
  }
  // wdt_reset_count();
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Reboot the device.
 */
void
watchdog_reboot(void)
{
  /* ensure WDT is on, no matter if it has been init'ed */
  if(status == NOT_INITED) {
    watchdog_init();
  }
  if(status == INACTIVE) {
    watchdog_start();
  }

  /* writing anything but 0xA5 to this register results in immmediate reboot */
  WDT_CLEAR = 0;
  WDT_CLEAR = 0;
  WDT_CLEAR = 0;
  WDT_CLEAR = 0;
}
/*---------------------------------------------------------------------------*/
