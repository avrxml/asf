/**
 * \file
 *
 * \brief HTTP component task for the HTTP Netconn Example.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include "task_defs.h"
#include "stdio_serial.h"
#include "ethernet.h"
#include "sysclk.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "httpd.h"
#include "tc.h"

#include "lwip/stats.h"

static void http_task(void *pvParameters);

#if LWIP_STATS
/** Used to compute LwIP bandwidth */
extern uint32_t lwip_tx_count;
extern uint32_t lwip_rx_count;
extern uint32_t lwip_tx_rate;
extern uint32_t lwip_rx_rate;
#endif

/**
 * \brief Create the HTTP task.
 *
 * \param stack_depth_words Task stack size in 32 bits word.
 * \param task_priority Task priority.
 */
void create_http_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority)
{
	/* Create the task as described above. */
	xTaskCreate(http_task, (const signed char *const) "HTTP",
			stack_depth_words, NULL, task_priority,
			NULL);
}

#if LWIP_STATS
// TC3 means TC1 channel 0.
void TC3_Handler(void)
{
	if ((tc_get_status(TC1, 0) & TC_SR_CPCS) == TC_SR_CPCS) {
		lwip_tx_rate = lwip_tx_count;
		lwip_rx_rate = lwip_rx_count;
		lwip_tx_count = 0;
		lwip_rx_count = 0;
	}
}

/**
 * \brief Enable TC1 channel 0 to trigger each second to compute GMAC stats.
 */
static void configure_timer_for_bandwidth_stats(void)
{
	pmc_enable_periph_clk(ID_TC3);		// Warning TC number is the channel not TC number.
										// Hence TC3 means TC1 channel 0.
	tc_init(TC1, 0,						// Init timer counter 1 channel 0.
			TC_CMR_WAVE |				// Waveform Mode is enabled.
			TC_CMR_CPCTRG |				// UP mode with automatic trigger on RC Compare
			TC_CMR_TCCLKS_TIMER_CLOCK5	// Use slow clock to avoid overflow.
	);

	tc_write_rc(TC1, 0, 32768);			// Load the highest possible value into TC.

	/* Configure TC interrupts for TC TC_CHANNEL_CAPTURE only */
	NVIC_SetPriority(TC3_IRQn, 0);		// TC3 means TC1 channel 0.
	NVIC_EnableIRQ(TC3_IRQn);			// TC3 means TC1 channel 0.
	tc_enable_interrupt(TC1, 0, TC_IER_CPCS);
	tc_start(TC1, 0);					// Start Timer counter 0 channel 0.
}
#endif

/**
 * \brief HTTP task core function.
 *
 * \param pvParameters Junk parameter.
 */
static void http_task(void *pvParameters)
{
	struct netconn *conn, *newconn;
	err_t err;
	/* Just to avoid compiler warnings. */
	UNUSED(pvParameters);

	/* Wait for user to read instructions. */
	WAIT_FOR_TOUCH_EVENT;

#if LWIP_STATS
	configure_timer_for_bandwidth_stats();
#endif

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if (conn == NULL)
	{
		printf("http_task: invalid conn\n");

		/* Delete the calling task. */
		vTaskDelete(NULL);
	}

	/* Bind to port 80 (HTTP) with default IP address */
	netconn_bind(conn, NULL, 80);

	/* Put the connection into LISTEN state */
	netconn_listen(conn);

	do {
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) {
		/* Try to instanciate a new HTTP-req task to handle the HTTP request. */
			if (NULL == sys_thread_new("HTTP-req", http_request, newconn,
					mainHTTP_TASK_STACK_SIZE, mainHTTP_TASK_PRIORITY)) {
				/* Failed to instanciate task, free netconn socket. */
				netconn_close(newconn);
				netconn_delete(newconn);
			}
		}
	} while (err == ERR_OK);

	printf("http_task: netconn_accept received error %d, shutting down\n", err);

	/* Free Netconn resource. */
	netconn_close(conn);
	netconn_delete(conn);

	/* Delete the calling task. */
	vTaskDelete(NULL);
}
