/**
 * \file  timeout.c
 *
 * \brief Timeout service for XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <conf_timeout.h>

/* Check if RTC32 is defined, otherwise use RTC as default */
#if defined(CLOCK_SOURCE_RTC32)
  #include <rtc32.h>
#else
  #include <rtc.h>
#endif

/** \brief Timeout timekeeping data */
struct timeout_struct {
	/**
	 * Current count-down value. Counts down for every tick.
	 * Will be considered as expired when it reaches 0, and
	 * may then be reloaded with period.
	 */
	uint16_t count;

	/**
	 * Period between expires. Used to reload count.
	 * If 0, the count won't be reloaded.
	 */
	uint16_t period;
};

/** Array of configurable timeout timekeeping data */
static struct timeout_struct timeout_array[TIMEOUT_COUNT];

/** Bitmask of active timeouts */
static uint8_t timeout_active;

/** Bitmask of expired timeouts */
static uint8_t timeout_expired;

/**
 * \brief Callback function for RTC compare interrupt handler
 *
 * The function executes when the RTC compare interrupt occurs and loop
 * through all timeout channels. The timeout_array[channel_index] which
 * contains the remaining ticks before timeout is decremented and the timeout
 * active/expired masks are updated.
 */
static void tick_handler(uint32_t time)
{
	uint8_t i;

	/* Loop through all timeout channels */
	for (i = 0; i < TIMEOUT_COUNT; i++) {
		/* Skip processing on current channel if not active */
		if (!(timeout_active & (1 << i))) {
			continue;
		}

		/* Decrement current channel with one tick */
		timeout_array[i].count--;

		/* Skip further processing on current channel if not expired */
		if (timeout_array[i].count) {
			continue;
		} else {
			/* Update expired bit mask with current channel */
			timeout_expired |= 1 << i;

			/* If Periodic timer, reset timeout counter to period
			 * time */
			if (timeout_array[i].period) {
				timeout_array[i].count
					= timeout_array[i].period;
			}
			/* If not periodic timeout, set current channel to
			 * in-active */
			else {
				timeout_active &= ~(1 << i);
			}
		}
	}
	/* Reset RTC before next tick */
	rtc_set_time(0);
	rtc_set_alarm(TIMEOUT_COMP);
}

/**
 * \brief Initialize timeout
 *
 * Initializes timeout counter for desired tick rate and starts it. The device
 * interrupt controller should be initialized prior to calling this function,
 * and global interrupts must be enabled.
 *
 * \note If the service is configured to use the asynchronous RTC32 module,
 *       there are restrictions on the timeout period that can be used - see
 *       to \ref rtc32_min_alarm_time for details.
 */
void timeout_init(void)
{
	rtc_init();
	rtc_set_callback(tick_handler);
	rtc_set_time(0);
	rtc_set_alarm(TIMEOUT_COMP);
}

/**
 * \brief Start periodic timeout with a specific start timeout
 *
 * \param id            \ref timeout_id_t
 * \param period        Time period in number of ticks
 * \param offset        Time to first timeout in number of ticks
 */
void timeout_start_offset(timeout_id_t id, uint16_t period, uint16_t offset)
{
	/* Check that ID within the TIMEOUT_COUNT range */
	if (id < TIMEOUT_COUNT) {
		/* Disable interrupts before tweaking the bitmasks */
		irqflags_t flags;
		flags = cpu_irq_save();

		/* Update timeout struct with offset and period */
		timeout_array[id].count = offset;
		timeout_array[id].period = period;

		/* Set current timeout channel bitmasks to active and not
		 * expired */
		timeout_active |= 1 << id;
		timeout_expired &= ~(1 << id);

		/* Restore interrupts */
		cpu_irq_restore(flags);
	}
}

/**
 * \brief Start singleshot timeout
 *
 * \param id       \ref timeout_id_t
 * \param timeout  Timeout in number of ticks
 */
void timeout_start_singleshot(timeout_id_t id, uint16_t timeout)
{
	timeout_start_offset(id, 0, timeout);
}

/**
 * \brief Start periodic timeout
 *
 * \param id      \ref timeout_id_t
 * \param period  Time period in number of ticks
 */
void timeout_start_periodic(timeout_id_t id, uint16_t period)
{
	timeout_start_offset(id, period, period);
}

/**
 * \brief Test and clear expired flag for running timeout
 *
 * \param id      \ref timeout_id_t
 * \retval true   Timer have expired; clearing expired flag
 * \retval false  Timer still running
 */
bool timeout_test_and_clear_expired(timeout_id_t id)
{
	/* Check that ID within the TIMEOUT_COUNT range */
	if (id < TIMEOUT_COUNT) {
		irqflags_t flags;

		/* Check if timeout has expired */
		if (timeout_expired & (1 << id)) {
			flags = cpu_irq_save();
			timeout_expired &= ~(1 << id);
			cpu_irq_restore(flags);
			return true;
		}
	}

	return false;
}

/**
 * \brief Stop running timeout
 *
 * \param id \ref timeout_id_t
 */
void timeout_stop(timeout_id_t id)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	timeout_active &= ~(1 << id);
	cpu_irq_restore(flags);
}
