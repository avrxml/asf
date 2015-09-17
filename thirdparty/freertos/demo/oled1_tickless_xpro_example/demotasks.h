/**
 * \file
 *
 * \brief FreeRTOS demo tasks header.
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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

#ifndef DEMOTASKS_H
#define DEMOTASKS_H

/**
 * \defgroup freertos_sam0_demo_tasks_group FreeRTOS demo tasks
 *
 * The demo tasks demonstrate basic use of FreeRTOS, with inter-task
 * communication using queues and mutexes.
 *
 * For details on how the demo works, see \ref appdoc_intro.
 *
 * For detailed information on the tasks, see:
 * - \ref main_task()
 * - \ref graph_task()
 * - \ref terminal_task()
 * - \ref about_task()
 * - \ref uart_task()
 *
 * The demo tasks depend on the following drivers:
 * - \ref oled1_xpro_io_group
 * - \ref edbg_cdc_rx_group
 * - \ref asfdoc_common2_gfx_mono
 *
 * @{
 */

extern bool volatile tickless_enable;
void demotasks_init(void);

/** @} */

#endif // DEMO_TASKS_H
