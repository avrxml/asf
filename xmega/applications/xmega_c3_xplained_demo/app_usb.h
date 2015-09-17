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

#ifndef _APP_USB_H_
#define _APP_USB_H_

/**
 * \ingroup demo_xmegac3_xplained_group
 * \defgroup usb_task_group USB task
 *
 * The USB device stack and the CDC/MSC interfaces are interrupt driven and
 * do not require a task except to perform a memory transfer outside interrupt
 * routine. Also, the task checks the switch SW0 to enable and disable USB
 * feature. The implementation is available in app_usb.c/.h files.
 *
 * @{
 */

/**
 * \brief USB task to schedule
 * This function checks the SW0 switch to enable/disable USB feature.
 * When USB is enabled, this task executes the Mass Storage memory transfer.
 * Also, the USB task manages the display of USB icon and rectangle.
 */
void app_usb_task(void);

/**
 * \brief Manages a counter to toggle USB rectangle
 * Called when a start of frame is received on USB line each 1ms.
 */
void app_usb_sof_action(void);

/**
 * \brief Set or clear a flag about Virtual Port state
 *
 * \param b_enable  Virtual Port has been opened if true
 */
void app_usb_cdc_set_dtr(bool b_enable);

/**
 * \brief Gives the Virtual Port state
 *
 * \return Virtual Port is open if true
 */
bool app_usb_cdc_is_open(void);

/* ! @} */

#endif /* _APP_USB_H_ */
