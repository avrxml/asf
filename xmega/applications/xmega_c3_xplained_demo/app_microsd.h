/**
 * \file
 *
 * \brief Task to manage microSD card
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
