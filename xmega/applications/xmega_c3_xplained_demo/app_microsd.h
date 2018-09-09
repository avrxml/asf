/**
 * \file
 *
 * \brief Task to manage microSD card
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
#ifndef APP_MICROSD_H_INCLUDED
#define APP_MICROSD_H_INCLUDED

/**
 * \ingroup demo_xmegac3_xplained_group
 * \defgroup microsd_task_group microSD task
 *
 * The microSD dectection, installation, mount and file controls are performed
 * in the microSD task. This task is enabled when the USB MSC is disabled,
 * because an embedded file system (here FatFS) and a USB device MSC cannot run
 * in same time on a same memory.
 * The implementation is available in app_microsd.c/.h files.
 *
 * @{
 */

/**
 * \brief Display on or off the rectangle around the microSD icon
 *
 * \param on_going  display on if true
 */
void app_microsd_display_access(bool on_going);

/**
 * \brief Authorizes the microSD task to run
 * Always called by USB task.
 */
void app_microsd_start(void);

/**
 * \brief Stop the microSD task
 * Always called by USB task.
 */
void app_microsd_stop(void);

/**
 * \brief microSD task to schedule
 * This function checks the card presence.
 * It mounts file system and create a log file, if card present.
 */
void app_microsd_task(void);

/**
 * \brief Returns the state of microSD
 * Use it before call \ref app_microsd_write_file()
 *
 * \return Card is ready for write operation, if true
 */
bool app_microsd_is_ready(void);

/**
 * \brief Writes a string in a log file
 *
 * \param str  string to write in log file
 */
bool app_microsd_write_file(const TCHAR *str);

/* ! @} */

#endif /* APP_MICROSD_H_INCLUDED */
