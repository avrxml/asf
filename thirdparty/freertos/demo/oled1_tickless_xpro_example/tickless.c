/**
 * \file
 *
 * \brief FreeRTOS demo application tick setup function and tickless function
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

#include <asf.h>
#include <conf_demo.h>

/**
 * \defgroup freertos-v7_4_2_sam_d20_tickless_group FreeRTOS tickless
 *
 * The tickless feature of FreeRTOS allows the device to sleep for peroids
 * longer than the predefined OS tick rate. This is useful when no tasks
 * are ready to execute for more than one OS tick. The benefit
 * of this is lower power consumption.
 *
 * The tickless feature is implemented using a timer, configured with the
 * desired timeout value, to wake the device. The same timer is also used to
 * generate the system tick, so that time is kept in the optimal way,
 * eliminating drift in most cases. If some other source wakes the device
 * before the sleep period is complete, but after more than one OS tick,
 * there will be a slight drift, as the timers count value must be corrected.
 *
 * @{
 */

//! \name Timer Constants
//@{

//! Frequency of timer
#define TIMER_HZ                            ( configCPU_CLOCK_HZ )

//! Value per os tick of timer
#define TIMER_RELOAD_VALUE_ONE_TICK         ( TIMER_HZ / configTICK_RATE_HZ )

//!  Maximum value of timer
#define TIMER_MAX_COUNT                     ( 0xffffffff )

//! Maximum possible suppressed ticks with timer
#define TIMER_MAX_POSSIBLE_SUPPRESSED_TICKS ( TIMER_MAX_COUNT / TIMER_RELOAD_VALUE_ONE_TICK )

//@}

//! \name Global instance and prototypes
//@{

//! Timer instance
static struct tc_module tc;

//! External declaration of freeRTOS SysTick handler
extern void xPortSysTickHandler( void );

//! Function for setting up timer
void vPortSetupTimerInterrupt(void);

//! Prototype for empty_callback for sleep timer
static void empty_callback(struct tc_module *const module_inst);

//! Global variable to control tickless operation
bool volatile tickless_enable = false;
bool volatile tickless_mode = false;
//@}

/*
 * \brief Empty callback
 *
 * An empty callback, just to wake the device.
 */
static void empty_callback(struct tc_module *const module_inst)
{
	// Empty, just to wake the device
}

/* 
 * \brief Initialize and start timer for tick
 *
 * Function that sets up a timer to use for os tick. The same timer is also
 * used as the sleep timer.
 * The timer runs at 48MHz, i.e. with no prescaler on GCLK0. Wavegen function
 * Match Frequency is chosen to reload the count register on every CC0 match.
 * 8 bit counter mode must not be chosen.
 * The function is weakly defined in freeRTOS, and redefined here.
 */
void vPortSetupTimerInterrupt(void)
{
	// Struct for configuring TC
	struct tc_config tcconf;
	// Set up configuration values
	tc_get_config_defaults(&tcconf);
	tcconf.counter_size    = TC_COUNTER_SIZE_32BIT;
	tcconf.run_in_standby  = true;
	tcconf.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	tcconf.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;

	// Initialize the TC
	tc_init(&tc, TICK_TC, &tcconf);

	// Register and enable callback for freeRTOS tick handler
	tc_register_callback(&tc, (tc_callback_t) xPortSysTickHandler, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);

	// Set top value equal to one os tick
	tc_set_top_value(&tc, TIMER_RELOAD_VALUE_ONE_TICK);

	// Enable the timer
	tc_enable(&tc);
}

/*
 * \brief Configure sleep timer and sleep
 *
 * Function to configure timer for sleep, and calculate time slept.
 */ 
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
	// Are we running tickless now?
	if (!tickless_enable) return;
	if(!tickless_mode)
	{
		SysTick->CTRL &=  ~0x01;
		NVIC_DisableIRQ(SysTick_IRQn);
		vPortSetupTimerInterrupt();
		tickless_mode=true;
		return ;
	}

	// Reconfigure the timer to act as sleep timer
	tc_disable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
	tc_unregister_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
	tc_register_callback(&tc, empty_callback, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);

	// Check that the offset is not greater than the range of the timer
	if (xExpectedIdleTime > TIMER_MAX_POSSIBLE_SUPPRESSED_TICKS)
	{
		xExpectedIdleTime = TIMER_MAX_POSSIBLE_SUPPRESSED_TICKS;
	}

	// Set sleep time, -1 because we want to wake up before the last tick
	tc_set_top_value(&tc, (xExpectedIdleTime - 1) * TIMER_RELOAD_VALUE_ONE_TICK);

	// Clear overflow interrupt flag
	tc.hw->COUNT32.INTFLAG.reg = TC_INTFLAG_OVF;

	// Check if we still should sleep
	if (eTaskConfirmSleepModeStatus() == eAbortSleep)
	{
		// Reset the timer to act as SysTick
		tc_disable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_unregister_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_register_callback(&tc, (tc_callback_t) xPortSysTickHandler, TC_CALLBACK_CC_CHANNEL0);
		tc_enable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_set_top_value(&tc, TIMER_RELOAD_VALUE_ONE_TICK);
	}
	else
	{
		if (xExpectedIdleTime > 0)
		{
			// Data sync barrier before sleep
			__asm volatile ("dsb");
			// Go to sleep
			__asm volatile ("wfi");

			// If OVF interrupt flag is set, we know the timer has wrapped
			if (tc.hw->COUNT32.INTFLAG.reg & TC_INTFLAG_OVF)
			{
				vTaskStepTick(xExpectedIdleTime - 1);
			}
			// We do not know how long we've slept
			else
			{
				// Calculate from Counter how long we've slept
				// Reset counter to less than one os tick
				// This might result in a tiny drift in time.
				uint32_t count_val = tc_get_count_value(&tc);
				vTaskStepTick(count_val / TIMER_RELOAD_VALUE_ONE_TICK);
				tc_set_count_value(&tc, count_val % TIMER_RELOAD_VALUE_ONE_TICK);
			}
		}
		// Reset the timer to act as SysTick
		tc_disable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_unregister_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_register_callback(&tc, (tc_callback_t) xPortSysTickHandler, TC_CALLBACK_CC_CHANNEL0);
		tc_enable_callback(&tc, TC_CALLBACK_CC_CHANNEL0);
		tc_set_top_value(&tc, TIMER_RELOAD_VALUE_ONE_TICK);

		// Make sure that the counter hasn't passed the CC before callback was registered
		if ( tc_get_count_value(&tc) > TIMER_RELOAD_VALUE_ONE_TICK )
		{
			// If so, reload count value, and step one tick */
			tc_set_count_value(&tc, tc_get_count_value(&tc) % TIMER_RELOAD_VALUE_ONE_TICK);
			vTaskStepTick(1);
		}
	}
}

