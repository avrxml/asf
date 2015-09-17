/**
 * \file
 *
 * \brief Watchdog timer self-diagnostic routine.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "classb_wdt_test.h"
#include "error_handler.h"
#include <wdt.h>
#include <rtc32.h>
#include <sysclk.h>

/**
 * \addtogroup classb_wdt
 *
 * @{
 */

/**
 * \name Non-initialized Variables
 *
 * These variables need to be preserved between resets, and are therefore not
 * initialized upon device reset.
 *
 * @{
 */

/** WDT test state. */
NO_INIT volatile classb_preinit_teststate_t classb_wdt_teststate;

/** Number of RTC periods in a WDT period. */
NO_INIT volatile uint16_t  classb_rtc_count;

/** @} */

/**
 * \name Internal functions
 *
 * @{
 */

/**
 * \brief Check if RTC32 is busy synchronizing
 *
 * \retval true  Is busy
 * \retval false Is ready
 */
static __always_inline bool rtc32_is_busy(void)
{
	return RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm;
}

/** Compatibility macro for RTC32 */
#define rtc_is_busy() rtc32_is_busy()

/**
 * \brief Delay for \a us microseconds
 *
 * \param us number of microseconds to busy wait
 */
static void udelay(uint16_t us)
{
	uint32_t count;

	/* Approximate the number of loop iterations needed. */
	count = sysclk_get_cpu_hz() / 1000000;
	count *= us;
	count /= 6;

	while (count--) {
		asm("");
	}
}

/**
 * \brief RTC32 Initialization.
 *
 * This function initializes the VBAT module and enables the oscillator used by
 * the RTC32.
 */
static void vbat_init(void)
{
	/* Reset the battery backup system */
	CCP = CCP_IOREG_gc;
	VBAT.CTRL = VBAT_RESET_bm;

	/* Enable access to VBAT */
	VBAT.CTRL |= VBAT_ACCEN_bm;

	/* Choose 1024Hz oscillator output and enable crystal oscillator failure
	 * monitor
	 */
	VBAT.CTRL |= VBAT_XOSCFDEN_bm | VBAT_XOSCSEL_bm;
	/* This delay is needed to give the voltage in the backup system some
	 * time to stabilize before we turn on the oscillator. If we do not
	 * have this delay we may get a failure detection.
	 */
	udelay(200);

	/* Enable the crystal oscillator */
	VBAT.CTRL |= VBAT_XOSCEN_bm;

	/* Wait until the crystal oscillator ready flag is set */
	while (!(VBAT.STATUS & VBAT_XOSCRDY_bm)) {
	}
}

/** @} */

/** @} */

/**
 * \brief Perform self-diagnostic test of watchdog timer (WDT).
 *
 * Basically this functions tests the following:
 * -# that WDT issues a system reset after timeout.
 * -# the WDT timing and that it can be reset.
 * -# that WDT issues a system reset if it is untimely reset in window mode.
 *
 * If any of these tests fail, \ref CLASSB_ERROR_HANDLER_WDT() is called.
 * By default the device will then simply hang.
 *
 * The expected (error-free) execution flow is as follows:
 * -# Power-on or external reset has occurred.
 * -# Check that WDT can issue a system reset. Set test state 1 then issue a
 * system reset.
 * -# Check that WDT can be reset. Set test state 2 then issue a system reset.
 * -# Check that window mode works correctly. Set test state 3 then issue a
 * system reset.
 * -# Set up WDT according to settings, i.e., CLASSB_ACTIONS_1_WDT(). Set test
 * state "ok" and continue to main().
 *
 * The reset cause is used to decide which actions to take. The user can
 * configure how to process a reset caused by the watchdog when the test is in
 * the "ok" state, i.e., \ref CLASSB_ACTIONS_2_WDT(), or what to do for
 * brown-out or software reset, i.e., \ref CLASSB_ACTIONS_3_WDT().
 *
 * The real time counter (RTC) is used as an independent clock source, which is
 * necessary for checking the timing of the oscillator used by the WDT.
 *
 * \note This test is executed before the main function. The method to achieve
 * this depends on the specific compiler that is used.
 */
classb_wdt_test(void)
{
	/* This variable is used to count RTC periods */
	register uint16_t counter;

	/* Check reset cause.
	 *
	 * If a power-on, external or debug reset has occurred, the first part
	 * of the test is executed. This checks that WDT issues a system reset
	 * on timeout.
	 */
	if (RST.STATUS & (RST_PORF_bm | RST_EXTRF_bm | RST_PDIRF_bm)) {
		/* First, clear reset flags */
		RST.STATUS = (RST_PORF_bm | RST_EXTRF_bm | RST_PDIRF_bm);
		/* Set the next state of the test */
		classb_wdt_teststate = TEST_WDT_1;

		/* Configure the RTC, which is used as an independent time
		 * source. In this section we are going to measure the number of
		 * RTC periods before the WDT timeout issues a system reset.
		 */
#ifdef RTC32
		/* If we use a device with RTC32 module, the VBAT system must be
		 * initialized before we use the RTC module. This sets a 1024 Hz
		 * RTC32 from the 32.768 kHz crystal oscillator
		 */
		vbat_init();
#else
		/* Use the RTC with 1024 Hz from internal 32.768 kHz RC
		 * oscillator
		 */
		OSC.CTRL |= OSC_RC32KEN_bm;
		while (!(OSC.STATUS & OSC_RC32KRDY_bm)) {
		}
		CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
#endif
		/* In order to have a good estimate, CLASSB_WDT_RTC_PER should
		 * be small.
		 */
		RTC_TEST.PER = CLASSB_WDT_RTC_PER;
		/* Wait until RTC is ready. */
		while (rtc_is_busy()) {
		}
		/* Start RTC timer */
		RTC_TEST.CTRL = RTC_TEST_START_bm;

		/* WDT Configuration:
		 * First, write to Configuration Change Protection register
		 */
		CCP = CCP_IOREG_gc;
		/* Then enable WDT with a configurable period. This requires
		 * setting the Change Enabled bit.
		 */
		WDT.CTRL = WDT_ENABLE_bm | CLASSB_WDT_PER | WDT_CEN_bm;

		/* Count number of RTC periods until a WDT timeout.
		 * There is a configurable maximum number of RTC periods before
		 * an error in the WDT is assumed.
		 * Recall that the CLASSB_WDT_RTC_PER gives the value of the
		 * register that is compared against the timer value, so the
		 * real period is CLASSB_WDT_RTC_PER+1
		 */
		classb_rtc_count = 0;
		counter = CLASSB_WDT_MAX / (CLASSB_WDT_RTC_PER+1);
		while (counter--) {
			classb_rtc_count++;
			while (!(RTC_TEST.INTFLAGS & RTC_TEST_OVFIF_bm)) {
			}
			RTC_TEST.INTFLAGS = RTC_TEST_OVFIF_bm;
		}
		/* This should only be executed if there is an error in the WDT,
		 * i.e., if the WDT did not timeout before the maximum number of
		 * RTC periods was exceeded.
		 */
		classb_wdt_teststate = FAULT_WDT;
	}

	/* If a watchdog reset has occurred, this is not the first iteration. */
	else if (RST.STATUS & RST_WDRF_bm) {
		/* First, clear the flag for WDT reset. */
		RST.STATUS = RST_WDRF_bm;

		/* Handle the different test stages. */
		switch (classb_wdt_teststate) {
		/* Test WDT timing and that WDT can be reset */
		case TEST_WDT_1:
			/* Assume watchdog fault for now. */
			classb_wdt_teststate = FAULT_WDT;

			/* Make sure that the estimated WDT period is also
			 * higher than expected
			 */
			if (classb_rtc_count >= (CLASSB_WDT_MIN
					/ (CLASSB_WDT_RTC_PER + 1))) {
				/* The real time counter is used as independent
				 * time source.
				 */
#ifdef RTC32
				/* If we use a device with RTC32 module, the
				 * VBAT system must be initialized before we use
				 * the RTC module.
				 * This sets a 1024 Hz RTC32 from the 32.768 kHz
				 * crystal oscillator
				 */
				vbat_init();
#else
				/* Use the RTC with 1024 Hz from internal
				 * 32.768 kHz RC oscillator
				 */
				CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc
						| CLK_RTCEN_bm;
#endif
				/* Set RTC period */
				RTC_TEST.PER = CLASSB_WDT_RTC_PER;
				/* Wait until RTC is ready. */
				while (rtc_is_busy()) {
				}
				/* Start RTC timer */
				RTC_TEST.CTRL = RTC_TEST_START_bm;

				/* WDT Configuration
				 * Enable WDT with configurable period.
				 * First, write to Configuration Change
				 * Protection register.
				 */
				CCP = CCP_IOREG_gc;
				/* Then enable WDT with desired period, which
				 * requires setting the Change Enabled bit.
				 */
				WDT.CTRL = WDT_ENABLE_bm | CLASSB_WDT_PER
						| WDT_CEN_bm;

				/* Wait approximately 0.75 * T_WDT, where T_WDT
				 * is the estimated WDT period, before a WDT
				 * reset. This checks that the WDT does not
				 * expires earlier than expected.
				 */
				counter  = classb_rtc_count;
				counter += (classb_rtc_count >> 1);
				counter >>= 1;
				while (counter--) {
					while (!(RTC_TEST.INTFLAGS
							& RTC_TEST_OVFIF_bm)) {
					}
					RTC_TEST.INTFLAGS = RTC_TEST_OVFIF_bm;
				}
				wdt_reset();
				/* Wait again approximately 0.75 * T_WDT */
				counter  = classb_rtc_count;
				counter += (classb_rtc_count >> 1);
				counter >>= 1;
				while (counter--) {
					while (!(RTC_TEST.INTFLAGS
							& RTC_TEST_OVFIF_bm)) {
					}
					RTC_TEST.INTFLAGS = RTC_TEST_OVFIF_bm;
				}

				/* This should only occur if WDT reset worked,
				 * otherwise there would have been a system
				 * reset before this point (this would be
				 * executed approximately 1.5*T_WDT after the
				 * WDT was initially set).
				 * Note that in that case the state would still
				 * be |FAULT_WDT|. Set next state and wait for
				 * the WDT to issue a system reset.
				 * That should approximately happen in approx
				 * 0.25*T_WDT.
				 */
				classb_wdt_teststate = TEST_WDT_2;
				/* Wait approx 0.5*T_WDT */
				counter = (classb_rtc_count >> 1);
				while (counter--) {
					while (!(RTC_TEST.INTFLAGS
							& RTC_TEST_OVFIF_bm)) {
					}
					RTC_TEST.INTFLAGS = RTC_TEST_OVFIF_bm;
				}
			}
			/* Set error flag if WDT has not issued a reset. */
			classb_wdt_teststate = FAULT_WDT;
			break;

		/* Test WDT with window mode. */
		case TEST_WDT_2:
			/* Assume watchdog fault for now. */
			classb_wdt_teststate = FAULT_WDT;

			/* The real time counter is used as an independent time
			 * source.
			 */
#ifdef RTC32
			/* If we use a device with RTC32 module, the VBAT system
			 * must be initialized before we use the RTC module This
			 * sets a 1024 Hz RTC32 from the 32.768 kHz crystal
			 * oscillator
			 */
			vbat_init();
#else
			/* Use the RTC with 1024 Hz from internal 32.768 kHz RC
			 * oscillator
			 */
			CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
#endif
			/* Set RTC period */
			RTC_TEST.PER = CLASSB_WDT_RTC_PER;
			/* Wait until RTC is ready. */
			while (rtc_is_busy()) {
			}
			/* Start RTC timer */
			RTC_TEST.CTRL = RTC_TEST_START_bm;

			/* Enable WDT with window mode enabled.
			 * Set Normal period (when WDT can be reset)
			 */
			CCP = CCP_IOREG_gc;
			WDT.CTRL =  WDT_ENABLE_bm | CLASSB_WDT_PER | WDT_CEN_bm;
			/* Wait until WDT Synchronized */
			while (WDT.STATUS & WDT_SYNCBUSY_bm) {
			}
			/* Set Closed period (when WDT cannot be reset) */
			CCP = CCP_IOREG_gc;
			WDT.WINCTRL =  WDT_WEN_bm | CLASSB_WDT_WPER
					| WDT_WCEN_bm;
			/* Wait until WDT Synchronized */
			while (WDT.STATUS & WDT_SYNCBUSY_bm) {
			}

			/* Set next test state and do a reset of the WDT
			 * immediately.  This should issue a system reset with
			 * any window mode settings.
			 */
			classb_wdt_teststate = TEST_WDT_3;
			wdt_reset();

			/* If there was a problem with the WDT in window mode,
			 * this would be executed.  Wait for 0.25 * T_WDT just
			 * to avoid timing issues with reset vs execution of
			 * instructions then set fault state if WDT module has
			 * not reset the device as expected.
			 */
			counter = classb_rtc_count >> 2;
			while (counter--) {
				while (!(RTC_TEST.INTFLAGS
						& RTC_TEST_OVFIF_bm)) {
				}
				RTC_TEST.INTFLAGS = RTC_TEST_OVFIF_bm;
			}
			/* Set error flag if WDT has not issued a reset. */
			classb_wdt_teststate = FAULT_WDT;
			break;

		/* After the test the WDT should be left enabled for the main
		 * application.
		 */
		case TEST_WDT_3:
			/* WDT configuration for the main application: WDT in
			 * normal mode
			 */
			CCP = CCP_IOREG_gc;
			WDT.CTRL =  WDT_ENABLE_bm | CLASSB_WDT_PER | WDT_CEN_bm;
			/* Wait until WDT Synchronized */
			while (WDT.STATUS & WDT_SYNCBUSY_bm) {
			}
			/* Set Closed period (when WDT cannot be reset) */
			CCP = CCP_IOREG_gc;
			WDT.WINCTRL = WDT_WEN_bm | CLASSB_WDT_WPER
					| WDT_WCEN_bm;
			/* Wait until WDT Synchronized */
			while (WDT.STATUS & WDT_SYNCBUSY_bm) {
			}
			classb_error = CLASSB_ERROR_NONE;

			/* The test of the WDT is finished and there was no
			 * error
			 */
			classb_wdt_teststate = TEST_WDT_OK;
			break;

		/* Handle WDT reset under normal operation. If the program
		 * reaches this point is because the main application did not
		 * update the WDT correctly and this led to system reset. Note
		 * that the WDT needs to be setup again.
		 */
		case TEST_WDT_OK:
			/* Configurable actions */
			CLASSB_ACTIONS_WDT_RUNTIME_FAILURE();
			break;

		/* Otherwise assume error */
		default:
			classb_wdt_teststate = FAULT_WDT;
			break;
		}
	/* Handle other reset reasons, i.e. software or brown-out. */
	} else {
		/* Configurable actions */
		CLASSB_ACTIONS_WDT_OTHER_FAILURE();
	}

	/* Actions to take if there was an error.
	 * The test would be on fault state because:
	 * - WDT could not be reset
	 * - WDT did not issue a system reset on either timeout or untimely
	 *   reset (window mode)
	 */
	if (classb_wdt_teststate == FAULT_WDT) {
		CLASSB_ERROR_HANDLER_WDT();
	}

#if defined(__ICCAVR__)
	/* IAR has a low level pre-init prototype that needs a return value */
	return 1;
#endif
}
