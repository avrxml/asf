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

#include "asf.h"


static volatile uint32_t ticks;
static volatile uint32_t seconds;
/* sleepseconds is the number of seconds sleeping since startup, available globally */
long sleepseconds;
static struct tc_module tc_instance;
void configure_tc3(void);
void _TC3_Handler(void);
#define TC3_WAIT_BUSY() while(REG_TC3_STATUS & TC_STATUS_SYNCBUSY)//rtc
/*---------------------------------------------------------------------------*/
static void
clock_irq_callback( struct tc_module *const module_inst)
{ 
 // ENERGEST_ON(ENERGEST_TYPE_IRQ);

  ticks++;
  //port_pin_toggle_output_level(PIN_PA23);
  if((ticks % CLOCK_SECOND) == 0) {
    seconds++;
   //energest_flush();
    //    printf("seconds, ticks %d\n", tc_get_count_value(&tc_instance));
  }
  if(etimer_pending()) {
    etimer_request_poll();
  }

  //ENERGEST_OFF(ENERGEST_TYPE_IRQ);
  
}
/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
#define TIMER_PERIOD  UINT16_MAX
#define TIMER         TC3
  
  struct tc_config cfg;
  tc_get_config_defaults(&cfg);

  cfg.clock_source = GCLK_GENERATOR_5;
  cfg.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
  cfg.run_in_standby = false;

  cfg.counter_16_bit.compare_capture_channel[0] = TIMER_PERIOD;
  tc_init(&tc_instance, TIMER, &cfg);
  /*  tc_register_callback(&tc_instance, clock_irq_callback, TC_CALLBACK_OVERFLOW);*/
  tc_register_callback(&tc_instance, clock_irq_callback, TC_CALLBACK_CC_CHANNEL0);
  /*  tc_register_callback(&tc_instance, clock_irq_callback, TC_CALLBACK_CC_CHANNEL1);
      tc_register_callback(&tc_instance, clock_irq_callback, TC_CALLBACK_ERROR);*/
  /*  tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);*/
  tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
  /*  tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL1);
      tc_enable_callback(&tc_instance, TC_CALLBACK_ERROR);*/
  
  tc_enable(&tc_instance);

 
}
/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  //  printf("0x%04x\n", tc_get_count_value(&tc_instance));
  //  printf("clock_time ticks %d\n", ticks);
  clock_time_t t;

  system_interrupt_enter_critical_section();
  t = ticks;
  system_interrupt_leave_critical_section();
  return t;
}
/*---------------------------------------------------------------------------*/
unsigned long 
clock_seconds(void)
{
  return seconds;
}
/*---------------------------------------------------------------------------*/
/**
 * Adjust the system current clock time.
 * \param dt   How many ticks to add
 *
 * Typically used to add ticks after an MCU sleep
 * clock_seconds will increment if necessary to reflect the tick addition.
  * Leap ticks or seconds can (rarely) be introduced if the ISR is not blocked.
 */
void
clock_adjust_ticks(clock_time_t howmany)
{
	ticks += howmany;
	while(howmany >= CLOCK_SECOND) {
		howmany -= CLOCK_SECOND;
		seconds++;
		sleepseconds++;
	}
}
/*---------------------------------------------------------------------------*/

//! [module_inst]
struct tc_module tc3_instance;
//! [module_inst]
/*---------------------------------------------------------------------------*/
void _TC3_Handler(void)//void TCC0_Handler(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

	ticks++;
	if((ticks % CLOCK_SECOND) == 0) {
		seconds++;                                                                                                                                                                                                                                                                                                                                                                                        
		energest_flush();
		//    printf("seconds, ticks %d\n", tc_get_count_value(&tc_instance));
	}
	if(etimer_pending()) {
		etimer_request_poll();
	}

	ENERGEST_OFF(ENERGEST_TYPE_IRQ); 
}
/*---------------------------------------------------------------------------*/

static void configure_tc3_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&tc3_instance,(tc_callback_t)_TC3_Handler,TC_CALLBACK_CC_CHANNEL0);
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tc_enable_callback(&tc3_instance, TC_CALLBACK_CC_CHANNEL0);
	//! [setup_enable_callback]
}

void configure_tc3(void)
{
	
	
	REG_GCLK_GENDIV = 1 << 8 | 2;
	REG_GCLK_GENCTRL =
	1 << 21 | /* run in stdby */
	1 << 16 | /* enable */
	5 << 8 |  /* XOSC32 is source */
	2 << 0;   /* output to GCLKx */

	/* enable GCLKx for TC0/TC1, clocked at 32.768 kHz */
	REG_GCLK_CLKCTRL = 1 << 14 | 2 << 8 | 0x0013;

	TC3_WAIT_BUSY();

	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config_tc.counter_16_bit.compare_capture_channel[0] = 0x0101;
	//! [setup_change_config]

	//! [setup_change_config_pwm]
	//config_tc.pwm_channel[0].enabled = true;
	//config_tc.pwm_channel[0].pin_out = PWM_OUT_PIN;
	//config_tc.pwm_channel[0].pin_mux = PWM_OUT_MUX;
	//! [setup_change_config_pwm]

	//! [setup_set_config]
	//tc_init(&tc_instance, PWM_MODULE, &config_tc);
	tc_init(&tc3_instance, TC3, &config_tc);
	//! [setup_set_config]
	
	//! [setup_enable]
	tc_enable(&tc3_instance);
	//! [setup_enable]
	
	configure_tc3_callbacks();
}

//rtc