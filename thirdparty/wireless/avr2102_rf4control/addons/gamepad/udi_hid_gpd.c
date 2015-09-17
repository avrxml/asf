/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) gamepad interface.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_hid.h"
#include "udi_hid_gpd.h"
#include <string.h>

/**
 * \ingroup udi_hid_gamepad_group
 * \defgroup udi_hid_gamepad_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */

bool udi_hid_gpd_enable(void);
void udi_hid_gpd_disable(void);
bool udi_hid_gpd_setup(void);
uint8_t udi_hid_gpd_getsetting(void);

/* ! Global structure which contains standard UDI interface for UDC */
UDC_DESC_STORAGE udi_api_t udi_api_hid_gpd = {
	.enable = (bool (*)(void))udi_hid_gpd_enable,
	.disable = (void (*)(void))udi_hid_gpd_disable,
	.setup = (bool (*)(void))udi_hid_gpd_setup,
	.getsetting = (uint8_t (*)(void))udi_hid_gpd_getsetting,
	.sof_notify = NULL,
};
/* @} */

/**
 * \ingroup udi_hid_gamepad_group
 * \defgroup udi_hid_gamepad_group_internal Implementation of UDI HID gamepad
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Internal defines and variables to manage HID gamepad
 */
/* @{ */

/* ! Size of report for standard HID gamepad */
#define UDI_HID_GPD_REPORT_SIZE  4

/* ! To store current rate of HID gamepad */
static uint8_t udi_hid_gpd_rate;
/* ! To store current protocol of HID gamepad */
static uint8_t udi_hid_gpd_protocol;
/* ! To store report feedback from USB host */
static uint8_t udi_hid_gpd_report_set;
/* ! To signal if a valid report is ready to send */
static bool udi_hid_gpd_b_report_valid;
/* ! Report ready to send */
static uint8_t udi_hid_gpd_report[UDI_HID_GPD_REPORT_SIZE];
/* ! Signal if a report transfer is on going */
static bool udi_hid_gpd_b_report_trans_ongoing;
/* ! Buffer used to send report */
COMPILER_WORD_ALIGNED
static uint8_t
		udi_hid_gpd_report_trans[UDI_HID_GPD_REPORT_SIZE];

/* @} */

/* ! HID report descriptor for standard HID gamepad */
UDC_DESC_STORAGE udi_hid_gpd_report_desc_t udi_hid_gpd_report_desc = {
	{
		0x05, 0x01,                /*UsagePage(Generic Desktop)*/
		0x09, 0x04,                /*Usage(Joystick),*/
		0xA1, 0x01,                /*Collection(Application),*/
		0x05, 0x02,                     /*UsagePage(Simulation
		                                 *Controls),*/
		0x09, 0xbb,               /*	Usage (Throttle),*/
		0x15, 0x81,               /*	Logical Minimum(-127)*/
		0x25, 0x7f,               /*	Logical Maximum(127),*/
		0x75, 0x08,               /*	Report Size (8),*/
		0x95, 0x01,               /*	Report Count (1),*/
		0x81, 0x02,               /*	Input (Data, Variable,
		                           *Absolute),*/
		0x05, 0x01,               /*	UsagePage(Generic Desktop)*/
		0x09, 0x01,               /*  Usage(Pointer),*/
		0xA1, 0x00,                     /*Collection(Physical),*/
		0x09, 0x30,               /*	Usage(X),*/
		0x09, 0x31,               /*	Usage (Y),*/
		0x95, 0x02,               /*	Report Count (2),*/
		0x81, 0x02,               /*	Input (Data, Variable,
		                           *Absolute),*/
		0xC0,                   /*End Collection(),*/
		0x09, 0x39,               /*	Usage(Hat Switch),*/
		0x15, 0x00,               /*	Logical Minimum(0),*/
		0x25, 0x03,               /*	Logical Maximum(3),*/
		0x35, 0x00,               /*Physical Minimum(0),*/
		0x46, 0x0E, 0x01,               /*	Physical Maximum(270),*/
		0x65, 0x14,                /*Unit (English Rotation: Angular
		                            *Position), ;Degrees*/
		0x75, 0x04,               /*	Report Size(4),*/
		0x95, 0x01,               /*	Report Count(1),*/
		0x81, 0x02,               /*	Input (Data, Variable, Absolute,
		                           *NULL State),*/
		0x05, 0x09,               /*					Usage
		                           *Page(Buttons),*/
		0x19, 0x01,               /*	Usage Minimum (Button 1),*/
		0x29, 0x04,               /*	Usage Maximum (Button 4),*/
		0x15, 0x00,                                   /*
		                                               *
		                                               *
		                                               * LOGICAL_MINIMUM
		                                               * (0) */
		0x25, 0x01,                     /*					Logical
		                                 * Maximum (1), */
		0x95, 0x04,               /*	Report Count (4), */
		0x75, 0x01,               /* Report size*/
		0x55, 0x00,               /*unit exponent*/
		0x65, 0x00,               /*	Unit (None), */
		0x81, 0x02,               /*	Input (Data, Variable,
		                           * Absolute), */
		0xC0                /*	End Collection() */
	}
};

/**
 * \name Internal routines
 */
/* @{ */

/**
 * \brief Changes gamepad report states (like LEDs)
 *
 * \param rate       New rate value
 *
 */
static bool udi_hid_gpd_setreport(void);

/**
 * \brief Send the report
 *
 * \return \c 1 if send on going, \c 0 if delay.
 */
static bool udi_hid_gpd_send_report(void);

/**
 * \brief Callback called when the report is sent
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer is completed
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer is aborted
 * \param nb_sent    number of data transfered
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
static void udi_hid_gpd_report_sent(udd_ep_status_t status, iram_size_t nb_sent,
		udd_ep_id_t ep);

/**
 * \brief Callback called to update report from USB host
 * udi_hid_gpd_report_set is updated before callback execution
 */
static void udi_hid_gpd_setreport_valid(void);

/* @} */

/* -------------------------------------------- */
/* ------ Interface for UDI HID level */

bool udi_hid_gpd_enable(void)
{
	/* Initialize internal values */
	udi_hid_gpd_rate = 0;
	udi_hid_gpd_protocol = 0;
	udi_hid_gpd_b_report_trans_ongoing = false;
	memset(udi_hid_gpd_report, 0, UDI_HID_GPD_REPORT_SIZE);
	udi_hid_gpd_b_report_valid = false;
	return UDI_HID_GPD_ENABLE_EXT();
}

void udi_hid_gpd_disable(void)
{
	UDI_HID_GPD_DISABLE_EXT();
}

bool udi_hid_gpd_setup(void)
{
	return udi_hid_setup(&udi_hid_gpd_rate,
			&udi_hid_gpd_protocol,
			(uint8_t *)&udi_hid_gpd_report_desc,
			udi_hid_gpd_setreport);
}

uint8_t udi_hid_gpd_getsetting(void)
{
	return 0;
}

static bool udi_hid_gpd_setreport(void)
{
	if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) &&
			(0 == (0xFF & udd_g_ctrlreq.req.wValue)) &&
			(1 == udd_g_ctrlreq.req.wLength)) {
		/* Report OUT type on report ID 0 from USB Host */
		udd_g_ctrlreq.payload = &udi_hid_gpd_report_set;
		udd_g_ctrlreq.callback = udi_hid_gpd_setreport_valid;
		udd_g_ctrlreq.payload_size = 1;
		return true;
	}

	return false;
}

bool udi_hid_gpd_throttle_move(int8_t pos)
{
	int16_t s16_newpos;

	irqflags_t flags = cpu_irq_save();

	/* Add position in HID mouse report */
	s16_newpos = (int8_t)udi_hid_gpd_report[0];
	s16_newpos += pos;
	if ((-127 > s16_newpos) || (127 < s16_newpos)) {
		cpu_irq_restore(flags);
		return false;   /* Overflow of report */
	}

	udi_hid_gpd_report[0] = (uint8_t)s16_newpos;
	/* Valid and send report */
	udi_hid_gpd_b_report_valid = true;
	udi_hid_gpd_send_report();

	cpu_irq_restore(flags);
	return true;
}

bool udi_hid_gpd_moveX(int8_t pos)
{
	int16_t s16_newpos;

	irqflags_t flags = cpu_irq_save();

	/* Add position in HID mouse report */
	s16_newpos = (int8_t)udi_hid_gpd_report[1];
	s16_newpos += pos;
	if ((-127 > s16_newpos) || (127 < s16_newpos)) {
		cpu_irq_restore(flags);
		return false;   /* Overflow of report */
	}

	udi_hid_gpd_report[1] = (uint8_t)s16_newpos;
	/* Valid and send report */
	udi_hid_gpd_b_report_valid = true;
	udi_hid_gpd_send_report();

	cpu_irq_restore(flags);
	return true;
}

bool udi_hid_gpd_moveY(int8_t pos)
{
	int16_t s16_newpos;

	irqflags_t flags = cpu_irq_save();

	/* Add position in HID mouse report */
	s16_newpos = (int8_t)udi_hid_gpd_report[2];
	s16_newpos += pos;
	if ((-127 > s16_newpos) || (127 < s16_newpos)) {
		cpu_irq_restore(flags);
		return false; /* Overflow of report */
	}

	udi_hid_gpd_report[2] = (uint8_t)s16_newpos;
	/* Valid and send report */
	udi_hid_gpd_b_report_valid = true;
	udi_hid_gpd_send_report();

	cpu_irq_restore(flags);
	return true;
}

bool udi_hid_gpd_buttons(bool b_state, uint8_t key_id)
{
	if (b_state) {
		udi_hid_gpd_report[3] |= key_id;
	} else {
		udi_hid_gpd_report[3] &= ~key_id;
	}

	irqflags_t flags = cpu_irq_save();

	/* Valid and send report */
	udi_hid_gpd_b_report_valid = true;
	udi_hid_gpd_send_report();

	cpu_irq_restore(flags);
	return true;
}

/* -------------------------------------------- */
/* ------ Internal routines */

static bool udi_hid_gpd_send_report(void)
{
	if (udi_hid_gpd_b_report_trans_ongoing) {
		return false;
	}

	memcpy(udi_hid_gpd_report_trans, udi_hid_gpd_report,
			UDI_HID_GPD_REPORT_SIZE);
	udi_hid_gpd_b_report_valid = false;
	udi_hid_gpd_b_report_trans_ongoing
		= udd_ep_run(     UDI_HID_GPD_EP_IN,
			false,
			udi_hid_gpd_report_trans,
			UDI_HID_GPD_REPORT_SIZE,
			udi_hid_gpd_report_sent);
	return udi_hid_gpd_b_report_trans_ongoing;
}

static void udi_hid_gpd_report_sent(udd_ep_status_t status, iram_size_t nb_sent,
		udd_ep_id_t ep)
{
	UNUSED(status);
	UNUSED(nb_sent);
	UNUSED(ep);
	udi_hid_gpd_b_report_trans_ongoing = false;
	if (udi_hid_gpd_b_report_valid) {
		udi_hid_gpd_send_report();
	}
}

static void udi_hid_gpd_setreport_valid(void)
{
	UDI_HID_GPD_CHANGE_LED(udi_hid_gpd_report_set);
}

/* @} */
