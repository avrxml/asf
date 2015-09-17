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

#include "contiki.h"
#include "leds.h"
#include "asf.h"
#include "rtc_count.h" //rtc
#include "rtc_count_interrupt.h"
#include "system_interrupt_features.h"
#include "tc_interrupt.h"
#include "rtimer-arch.h"

void sleep_callback(struct rtimer *t, void *ptr);
void rtimer_adjust_ticks(uint32_t );

uint32_t sleep_count;
static struct tc_module tc_instance;
//static struct rtimer sleep_tmr;
#define TC45_WAIT_BUSY() while(REG_TC4_STATUS & TC_STATUS_SYNCBUSY)
/* irq bits - valid in INTENSET, INTENCLR, INTFLAGS */
#define TC_INT_OVF (1 << 0)
#define TC_INT_ERR (1 << 1)
#define TC_INT_MC0 (1 << 4)
#define TC_INT_MC1 (1 << 5)
#define TC_INT_ALL (TC_INT_OVF | TC_INT_ERR | TC_INT_MC0 | TC_INT_MC1)
#define RTIMER_SYNC_VALUE 10
#if RTIMER_SECOND == 32768
#define RTIMER_COMPENSATION_TICKS       6
#else /* RTIMER_SECOND == 32768 */
#define RTIMER_COMPENSATION_TICKS       0
#endif /* RTIMER_SECOND == 32768 */
/*---------------------------------------------------------------------------*/
void _TC4_Handler(void);
/* RTC test */
void rtc_overflow_callback(void);
void configure_rtc_count(void);
void configure_rtc_callbacks(void);
#define RTC_SECOND    1000UL

//! [rtc_module_instance]
struct rtc_module rtc_instance;
//! [rtc_module_instance]

void rtc_overflow_callback(void)
{
	//! [overflow_act]
	/* Do something on RTC overflow here */
	//ENERGEST_ON(ENERGEST_TYPE_IRQ);	
	rtc_count_disable(&rtc_instance);
	//ENERGEST_OFF(ENERGEST_TYPE_IRQ);
	
	//! [overflow_act]
}
//! [callback]

//! [initialize_rtc]
void configure_rtc_count(void)
{
	//! [init_conf]
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);
	//! [init_conf]

	//! [set_config]
	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_32;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	config_rtc_count.clear_on_match      = true;
	config_rtc_count.continuously_update = true;
	//! [set_config]
	//! [init_rtc]
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
	
	//! [init_rtc]

	//! [enable]
	
	//! [enable]
}
//! [initialize_rtc]

//! [setup_callback]
void configure_rtc_callbacks(void)
{
	//! [reg_callback]
	rtc_count_register_callback(
	&rtc_instance, rtc_overflow_callback, RTC_COUNT_CALLBACK_OVERFLOW);
	//! [reg_callback]
	//! [en_callback]
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);
	//! [en_callback]
}

static void
rtimer_irq_callback( struct tc_module *const module_inst)
{
	//ENERGEST_ON(ENERGEST_TYPE_IRQ);
  // rtimer_expired=1;
	rtimer_run_next();
	//ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
void _TC4_Handler(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

	/* clear all interrupt flags and disable interrupts */
	REG_TC4_INTFLAG = TC_INT_ALL;
	REG_TC4_INTENCLR = TC_INT_ALL;

	/* check for, and run, any pending rtimers */
	//rtimer_run_next();

	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
void
rtimer_arch_init(void)
{
#define TIMER         TC4

  struct tc_config cfg;
  tc_get_config_defaults(&cfg);

  cfg.clock_source = GCLK_GENERATOR_5;
  cfg.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
  cfg.run_in_standby = false;
  cfg.counter_size = TC_COUNTER_SIZE_32BIT;

  int ret = tc_init(&tc_instance, TIMER, &cfg);
  tc_register_callback(&tc_instance, rtimer_irq_callback, TC_CALLBACK_CC_CHANNEL0);
  tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
  system_interrupt_set_priority(_tc_interrupt_get_interrupt_vector(_tc_get_inst_index(TC4)),SYSTEM_INTERRUPT_PRIORITY_LEVEL_1);//tc4 priority  
  tc_enable(&tc_instance);
  /* RTC Init */
  configure_rtc_count();
  configure_rtc_callbacks();
  //  printf("----------------------- rtimer_arch_init ret %02x\n", ret);
  ret = ret;  // dummy
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
rtimer_arch_now(void)
{
  rtimer_clock_t t = tc_get_count_value(&tc_instance);
  //  printf("rtimer_arch_now: t 0x%08x\n", t);
  return t;
}
/*---------------------------------------------------------------------------*/
void
rtimer_arch_schedule(rtimer_clock_t t)
{
	uint32_t expiry = t + RTIMER_SYNC_VALUE- RTIMER_COMPENSATION_TICKS;
	
	
	if(RTIMER_CLOCK_LT(expiry, rtimer_arch_now())) {
		/* too soon, run now instead */
		printf("rtimer: schedule too soon, executing now! %lu %lu\n",expiry,(uint32_t)rtimer_arch_now());
		rtimer_run_next();
		return;
	}

	///* set compare match on time t */
	//TC45_WAIT_BUSY();
	//REG_TC4_COUNT32_CC0 = expiry;
	///* enable interrupts */
	//REG_TC4_INTENSET = TC_INT_MC0;
	tc_set_compare_value(&tc_instance,TC_COMPARE_CAPTURE_CHANNEL_0,expiry);
}

/*---------------------------------------------------------------------------*/

void
rtimer_arch_sleep(rtimer_clock_t howlong)
{
	/*Step 1:  Calculate the sleep count from the ticks received */
	sleep_count = ( howlong * RTC_SECOND ) / RTIMER_ARCH_SECOND;
	/*Step 2: Enable RTC and set the calculated time period */
	rtc_count_enable(&rtc_instance);
	rtc_count_set_period(&rtc_instance, sleep_count); //rtc
	/* Step 3: Enable IRQ, since this function caled in IRQ context and other intterupts will be disabled by now */
	//cpu_irq_enable();
	/* Step 4: Set the sleep mode and put the MCU to sleep */
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
    ENERGEST_OFF(ENERGEST_TYPE_CPU);
    ENERGEST_ON(ENERGEST_TYPE_LPM);	
	system_sleep();
    ENERGEST_OFF(ENERGEST_TYPE_LPM);
    ENERGEST_ON(ENERGEST_TYPE_CPU);	
	//cpu_irq_disable();
	/* Step 5: Once woke, wake Transceiver up */
	//wake_from_sleep(); // TRX Wakeup
	/* Step 6: Adjust the timer ticks - both rtimer and etimer */
	rtimer_adjust_ticks(sleep_count);
    return;
}

/*---------------------------------------------------------------------------*/

void 
rtimer_adjust_ticks(uint32_t howlong)
{
	howlong = (howlong * RTIMER_ARCH_SECOND)/RTC_SECOND;
	uint32_t ticks = rtimer_arch_now();
	tc_set_count_value(&tc_instance, ticks + howlong);
	/* Adjust other timer ticks */
	uint32_t longhowlong = CLOCK_CONF_SECOND;
	longhowlong *= howlong;
	clock_adjust_ticks(longhowlong/RTIMER_ARCH_SECOND);
	rtimer_run_next();
}

/*---------------------------------------------------------------------------*/
