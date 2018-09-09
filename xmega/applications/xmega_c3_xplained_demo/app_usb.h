/**
 * \file
 *
 * \brief Task to manage USB Device feature
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
