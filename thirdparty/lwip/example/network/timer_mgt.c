/*****************************************************************************
 *
 * \file
 *
 * \brief Timer management functions for standalone lwIP example.
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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


#include "tc.h"
#include "intc.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "lwip/ip_frag.h"
#include "netif/etharp.h"

#if UC3A
#define LWIP_TC                 (&AVR32_TC)
#define LWIP_TC_IRQ_GROUP       AVR32_TC_IRQ_GROUP
#define LWIP_TC_IRQ             AVR32_TC_IRQ0
#elif UC3C
#define LWIP_TC                 (&AVR32_TC0)
#define LWIP_TC_IRQ_GROUP       AVR32_TC0_IRQ_GROUP
#define LWIP_TC_IRQ             AVR32_TC0_IRQ0
#else
#error "This example is not ported on that part."
#endif

#define TC_CHANNEL              0


/* Keeps track of the elapsed # ms.... 64-bit, so up to ~584942417 years */
volatile U64 tc_tick = 0;


/*! \brief TC interrupt.
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = LWIP_TC_IRQ_GROUP, 1
__interrupt
#endif
static void tc_irq(void)
{
  // Increment the ms seconds counter
  tc_tick++;

  // Clear the interrupt flag. This is a side effect of reading the TC SR.
  tc_read_sr(LWIP_TC, TC_CHANNEL);

}

/*! \brief Private function to safely retrieve the timer tick
 */
static U64 get_current_timer_tick(void)
{
	U64 temp;
	bool global_interrupt_enabled = Is_global_interrupt_enabled();

	/* Ensure access to 64-bit variable is not interrupted */
	if (global_interrupt_enabled) Disable_global_interrupt();
	temp = tc_tick;
	if (global_interrupt_enabled) Enable_global_interrupt();

	return temp;
}


/*! \brief Initialization function of the display. Generates a
 * timer interrupt every 1ms for maintaining the lwIP timers
 *
 * \param fcpu_hz CPU frequency.
 */
void init_timer(U32 fcpu_hz)
{
	volatile avr32_tc_t *tc = LWIP_TC;

	// Options for waveform genration.
	static const tc_waveform_opt_t WAVEFORM_OPT =
	{
	  .channel  = TC_CHANNEL,                        // Channel selection.

	  .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
	  .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
	  .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
	  .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

	  .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
	  .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
	  .acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
	  .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

	  .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
	  .enetrg   = false,                             // External event trigger enable.
	  .eevt     = 0,                                 // External event selection.
	  .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
	  .cpcdis   = false,                             // Counter disable when RC compare.
	  .cpcstop  = false,                             // Counter clock stopped with RC compare.

	  .burst    = false,                             // Burst signal selection.
	  .clki     = false,                             // Clock inversion.
	  .tcclks   = TC_CLOCK_SOURCE_TC3                // Internal source clock 3, connected to fPBA / 8.
	};

	/* Enable the compare interrupt */
	static const tc_interrupt_t TC_INTERRUPT =
	{
	  .etrgs = 0,
	  .ldrbs = 0,
	  .ldras = 0,
	  .cpcs  = 1,
	  .cpbs  = 0,
	  .cpas  = 0,
	  .lovrs = 0,
	  .covfs = 0
	};

	Disable_global_interrupt();

	// The INTC driver has to be used only for GNU GCC for AVR32.
	#if defined (__GNUC__)
	// Initialize interrupt vectors.
	INTC_init_interrupts();

	// Register the RTC interrupt handler to the interrupt controller.
	INTC_register_interrupt(&tc_irq, LWIP_TC_IRQ, AVR32_INTC_INT0);
	#endif

	// Initialize the timer/counter waveform.
	tc_init_waveform(tc, &WAVEFORM_OPT);

	// Set the compare triggers to get an interrupt every 1 ms.
	// We want: (1/(fPBA/8)) * RC = 0.001 s, hence RC = (fPBA/8) / 1000
	tc_write_rc(tc, TC_CHANNEL, (fcpu_hz / 8) / 1000); // Set RC value.

	tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);

	// Start the timer/counter.
	tc_start(tc, TC_CHANNEL);

	Enable_global_interrupt();
}


/*! \brief Manages the required lwIP timers when operating
 * in standalone mode. Timer tick is 1ms
 */
void manage_timers(void)
{
	static U64 last_arp_time = 0;
	static U64 last_tcp_time = 0;
	static U64 last_ip_reass_time = 0;
#if defined(DHCP_USED)
	static U64 last_dhcp_fine_time = 0;
	static U64 last_dhcp_coarse_time = 0;
#endif
	U64 cur_time;
	static U64 last_time;
	U64 force_arp = false, force_tcp = false, force_ip_reass = false;
#if defined(DHCP_USED)
	U64 force_dhcp_fine = false, force_dhcp_coarse = false;
#endif

	/* Get the current time and see if any timer has expired */
	cur_time = get_current_timer_tick();
	if(cur_time < last_time)
	{
		if((cur_time +(0xFFFFFFFFFFFFFFFFULL - last_arp_time)) >= ARP_TMR_INTERVAL)
			force_arp = true;
		if((cur_time +(0xFFFFFFFFFFFFFFFFULL - last_tcp_time)) >= TCP_TMR_INTERVAL)
			force_tcp = true;
		if((cur_time +(0xFFFFFFFFFFFFFFFFULL - last_ip_reass_time)) >= IP_TMR_INTERVAL)
			force_ip_reass = true;
#if defined(DHCP_USED)
		if((cur_time +(0xFFFFFFFFFFFFFFFFULL - last_dhcp_fine_time)) >= DHCP_FINE_TIMER_MSECS)
			force_dhcp_fine = true;
		if((cur_time +(0xFFFFFFFFFFFFFFFFULL - last_dhcp_coarse_time)) >= DHCP_COARSE_TIMER_MSECS)
			force_dhcp_coarse = true;
#endif
	}
	last_time = cur_time;
	if ((cur_time - last_arp_time >= ARP_TMR_INTERVAL)
		|| (force_arp == true)) {
		etharp_tmr();
		last_arp_time = cur_time;
	}
	if ((cur_time - last_tcp_time >= TCP_TMR_INTERVAL)
		|| (force_tcp == true)) {
		tcp_tmr();
		last_tcp_time = cur_time;
	}
	if ((cur_time - last_ip_reass_time >= IP_TMR_INTERVAL)
		|| (force_ip_reass == true)){
		ip_reass_tmr();
		last_ip_reass_time = cur_time;
	}
#if defined(DHCP_USED)
	if ((cur_time - last_dhcp_fine_time >= DHCP_FINE_TIMER_MSECS)
		|| (force_dhcp_fine == true)){
		dhcp_fine_tmr();
		last_dhcp_fine_time = cur_time;
	}
	if ((cur_time - last_dhcp_coarse_time >= DHCP_COARSE_TIMER_MSECS)
		|| (force_dhcp_coarse == true)){
		dhcp_coarse_tmr();
		last_dhcp_coarse_time = cur_time;
	}
#endif
}

