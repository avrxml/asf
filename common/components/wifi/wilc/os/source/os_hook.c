/**
 *
 * \file
 *
 * \brief
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#include <os/include/os_hook.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <lwip/tcpip.h>

extern void wilc_netif_tx_from_queue(hif_msg_t *msg);
extern uint8_t m2m_wifi_handle_events(void *p);

/* HIF thread. */
xTaskHandle hif_thread;

/* HIF thread queue. */
xQueueHandle hif_queue = NULL;

/* HIF thread binary semaphore. */
static xSemaphoreHandle hif_notify_sem;

/* HIF initialized. */
static uint8_t has_init = 0;

/**
 * \brief HIF thread entry function.
 */
static void os_hif_task(void *pv)
{
	while(1) {
		hif_msg_t msg;
		
		if (xQueueReceive(hif_queue, (void *)&msg, portMAX_DELAY) != pdFALSE) {
			/* WiFi init. */
			if (msg.id == MSG_START) {
				if (msg.handler) 
					msg.handler(msg.priv);				
			}
			
			/* Incoming data packet. */
			else if (msg.id == MSG_RX) {
				m2m_wifi_handle_events(NULL);

			} 
			/* Outgoing data packet. */
			else if ((msg.id == MSG_TX_STA) || (msg.id == MSG_TX_AP)) {
				wilc_netif_tx_from_queue(&msg);
			}
			/* WiFi command. */
			else if (msg.id == MSG_CMD) {

				if (msg.handler) 
					msg.handler(msg.priv);
			} 
			/* Error. */
			else {
				osprintf("Warning: Wrong id  msg id %d \r\n", msg.id);
			}
		}
	}
}

/**
 * \brief Put interrupt request in netif queue for later processing.
 */
void os_hook_isr(void)
{
	signed portBASE_TYPE woken = pdFALSE;
	hif_msg_t msg;

	msg.id = MSG_RX;
	msg.pbuf = NULL;
	msg.payload_size = 0;
	msg.payload = NULL;
	if (xQueueSendFromISR(hif_queue, &msg, &woken) != pdTRUE) {
		osprintf("os_hook_isr: queue full error!\n");
	}
	else {
		portEND_SWITCHING_ISR(woken);
	}
}

/**
 * \brief Initialize netif thread.
 */
void os_hook_init(void)
{
	if (!has_init) {
		vSemaphoreCreateBinary(hif_notify_sem);
		xSemaphoreTake(hif_notify_sem, portMAX_DELAY);
		hif_queue = xQueueCreate(32, sizeof(hif_msg_t));
		
		xTaskCreate(os_hif_task, (const void *)"WiFiHIF", 1024, NULL, (configMAX_PRIORITIES - 1), &hif_thread);			
		has_init = 1;
	}
}

void os_hook_set_handle(void *task)
{
	hif_thread = task;
}

/**
 * \brief Run handler function in the netif thread context and return immediately.
 */
uint8_t os_hook_dispatch_no_wait(wifi_task_cb handler, void *p)
{
	hif_msg_t msg;
	if (hif_thread == xTaskGetCurrentTaskHandle()) {
		handler(p);
	} 
	else {
		msg.id = MSG_CMD;
		msg.handler = handler;
		msg.priv = p;
		return xQueueSend(hif_queue, (void *)&msg, portMAX_DELAY);
	}
	return 0;
}

/**
 * \brief Run handler function in the netif thread context and wait for function return.
 */
void os_hook_dispatch_wait(wifi_task_cb handler, struct params_dispatch *p, void* pv)
{
	hif_msg_t msg;
	if (hif_thread == xTaskGetCurrentTaskHandle()) {
		p->signal_semaphore = 0;
		handler(pv);
	}
	else {
		p->signal_semaphore = 1;
		msg.id = MSG_CMD;
		msg.handler = handler;
		msg.priv = pv;
		xQueueSend(hif_queue, (void *)&msg, portMAX_DELAY);
		
		xSemaphoreTake(hif_notify_sem, portMAX_DELAY);
	}
}

/**
 * \brief Start WiFi in the netif thread context and wait for function return.
 */
void os_hook_send_start(wifi_task_cb handler, struct params_dispatch *p, void* pv)
{
	hif_msg_t msg;
 
	p->signal_semaphore = 1;
	msg.id = MSG_START;
	msg.handler = handler;
	msg.priv = pv;
	xQueueSend(hif_queue, (void *)&msg, portMAX_DELAY);
	xSemaphoreTake(hif_notify_sem, portMAX_DELAY);
}

/**
 * \brief Release lock to notify calling thread that netif job is done.
 */
void os_hook_notify(void)
{
	xSemaphoreGive(hif_notify_sem);
}
