/**
 * \file
 *
 * \brief Task to manage USB Device feature
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "conf_app_display.h"
#include "conf_usb.h"
#include "app_usb.h"
#include "app_microsd.h"

/**
 * \ingroup usb_task_group
 * \defgroup usb_task_group_implementation USB task implementation
 * @{
 */

/* ! Link to USB icon */
extern struct gfx_mono_bitmap bitmap_usb;
/* ! Virtual Port is open, if true */
static bool app_usb_cdc_open = false;
/* ! Notify that the USB rectangle must be toggled */
static bool app_usb_rec_toggle = false;

void app_usb_task(void)
{
	static bool sw0_released = true;
	static bool usb_running = false;
	static bool cdc_running = false;
	static bool toggle = true;

	if (sw0_released && ioport_pin_is_low(GPIO_PUSH_BUTTON_0)) {
		/* A new press has been done */
		sw0_released = false;
		/* Switch USB state */
		if (usb_running) {
			udc_stop();
			gfx_mono_draw_filled_rect(DISPLAY_USB_STA_POS_X,
					DISPLAY_USB_STA_POS_Y,
					DISPLAY_USB_STA_SIZE_X,
					DISPLAY_USB_STA_SIZE_Y,
					GFX_PIXEL_CLR);
			app_microsd_start();
		} else {
			/* Stop FatFS on uSD card if enabled */
			app_microsd_stop();
			stdio_usb_init(); /* Start USB */
			gfx_mono_generic_put_bitmap(&bitmap_usb,
					DISPLAY_USB_STA_POS_X,
					DISPLAY_USB_STA_POS_Y);
		}

		usb_running = !usb_running;
		cdc_running = true;
	} else {
		/* Wait switch release */
		sw0_released = ioport_pin_is_high(GPIO_PUSH_BUTTON_0);
	}

	if (!usb_running) {
		return;
	}

	/* USB MSC task */
	while (udi_msc_process_trans()) {
	}

	if (app_usb_cdc_open && !cdc_running) {
		printf("\x0CSensor data logs:\r\n");
		cdc_running = true;
	}

	if (!app_usb_cdc_open && cdc_running) {
		cdc_running = false;
	}

	/* Toggle USB icon */
	if ((app_usb_cdc_open && app_usb_rec_toggle) ||
			(toggle && app_usb_rec_toggle)) {
		app_usb_rec_toggle = false;
		toggle = !toggle;
		gfx_mono_draw_rect(DISPLAY_USB_STACDC_POS_X,
				DISPLAY_USB_STACDC_POS_Y,
				DISPLAY_USB_STACDC_SIZE_X,
				DISPLAY_USB_STACDC_SIZE_Y,
				toggle ? GFX_PIXEL_SET : GFX_PIXEL_CLR);
	}
}

void app_usb_sof_action(void)
{
	static uint16_t counter = 0;
	if (++counter == 500) {
		counter = 0;
		app_usb_rec_toggle = true;
	}
}

void app_usb_cdc_set_dtr(bool b_enable)
{
	app_usb_cdc_open = b_enable;
}

bool app_usb_cdc_is_open(void)
{
	return app_usb_cdc_open;
}

/* ! @} */
