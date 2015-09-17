/**
 * \file
 *
 * \brief Example configuration for the Metering Application Emulator
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

#ifndef CONF_APP_EMU_H
#define CONF_APP_EMU_H

/* Enable debug printouts */
#define APPEMU_DEBUG

/* APPEMU: ON/OFF JUMPER CONFIG */
#define PIN_APPEMU_GPIO      (PIO_PC3_IDX)
#define PIN_APPEMU_FLAGS     (PIO_INPUT)
#define PIN_APPEMU           {PIO_PC3, PIOC, ID_PIOC, PIO_INPUT, PIO_DEBOUNCE |	\
			     			  PIO_PULLUP}
#define PIN_APPEMU_MASK      PIO_PC3
#define PIN_APPEMU_PIO       PIOC
#define PIN_APPEMU_ID        ID_PIOC
#define PIN_APPEMU_TYPE      PIO_INPUT
#define PIN_APPEMU_ATTR      PIO_DEBOUNCE | PIO_PULLUP

#endif /* CONF_APP_EMU_H */
