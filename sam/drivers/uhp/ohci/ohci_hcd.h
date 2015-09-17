/**
 * \file
 *
 * \brief OHCI Host Controller Driver header file with APIs for UHD interface.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "ohci.h"

/* Bus status. */
#define BUS_CONNECT     0
#define BUS_DISCONNECT  1
#define BUS_RESET       2

/* PID type. */
enum pid {
	TD_PID_SETUP = 0,
	TD_PID_OUT   = 1,
	TD_PID_IN    = 2,
};

/* OHCI interrupt source. */
enum ohci_interrupt_source {
	OHCI_INTERRUPT_SO = 0,
	OHCI_INTERRUPT_WDH,
	OHCI_INTERRUPT_SF,
	OHCI_INTERRUPT_RD,
	OHCI_INTERRUPT_UE,
	OHCI_INTERRUPT_FNO,
	OHCI_INTERRUPT_RHSC,
	OHCI_INTERRUPT_OC,
	OHCI_NUM_OF_INTERRUPT_SOURCE,
};

typedef void (*ohci_callback_t)(void *pointer);

void ohci_init(void);
void ohci_deinit(void);
bool ohci_get_device_speed (void);
uint16_t ohci_get_frame_number (void);
void ohci_bus_reset(void);
void ohci_bus_suspend(void);
bool ohci_is_suspend(void);
void ohci_bus_resume(void);
bool ohci_add_ed_control(ed_info_t *ed_info);
bool ohci_add_ed_bulk(ed_info_t *ed_info);
bool ohci_add_ed_period(ed_info_t *ed_info);
void ohci_remove_ed(uint8_t ep_number);
void ohci_clear_ed_transfer_status(struct ohci_td_general *td_general_header);
bool ohci_add_td_control(enum pid pid, uint8_t *buf, uint16_t buf_size);
bool ohci_add_td_non_control(uint8_t ep_number, uint8_t *buf,
		uint32_t buf_size, struct ohci_td_general **td_general_header);
void ohci_remove_td(uint8_t ep_number);
void ohci_enable_interrupt(enum ohci_interrupt_source int_source);
void ohci_disable_interrupt(enum ohci_interrupt_source int_source);
void ohci_register_callback(enum ohci_interrupt_source int_source, void *call_back);
void ohci_unregister_callback(enum ohci_interrupt_source int_source);

