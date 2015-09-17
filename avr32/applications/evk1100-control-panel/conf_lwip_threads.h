/**************************************************************************
 *
 * \file
 *
 * \brief lwIP core & application threads configuration file.
 *
 * This file contains the possible external configuration of the Ethernet module.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _CONF_LWIP_THREADS_H_
#define _CONF_LWIP_THREADS_H_

/*! define stack size for WEB server task */
#define CTRLPANEL_WEB_SERVER_STACK_SIZE   384

/*! define stack size for TFTP server task */
#define CTRLPANEL_TFTP_SERVER_STACK_SIZE  100

/*! define stack size for SMTP Client task */
#define CTRLPANEL_SMTP_CLIENT_STACK_SIZE  320

/*! define stack size for lwIP task */
#define lwipINTERFACE_STACK_SIZE          512

/*! define stack size for netif task */
#define netifINTERFACE_TASK_STACK_SIZE    256

/*! define WEB server priority */
#define CTRLPANEL_WEB_SERVER_PRIORITY     ( tskIDLE_PRIORITY + 2 )

/*! define TFTP server priority */
#define CTRLPANEL_TFTP_SERVER_PRIORITY    ( tskIDLE_PRIORITY + 3 )

/*! define SMTP client priority */
#define CTRLPANEL_SMTP_CLIENT_PRIORITY    ( tskIDLE_PRIORITY + 5 )

/*! define lwIP task priority */
#define lwipINTERFACE_TASK_PRIORITY       ( configMAX_PRIORITIES - 1 )

/*! define netif task priority */
#define netifINTERFACE_TASK_PRIORITY      ( configMAX_PRIORITIES - 1 )

/*! Number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX                      6


#endif // #ifndef _CONF_LWIP_THREADS_H_
