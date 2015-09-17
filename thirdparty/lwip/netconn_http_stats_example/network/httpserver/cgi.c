/**
 * \file
 *
 * \brief CGI request function handlers.
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

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/stats.h>
#include <lwip/opt.h>
#include "httpd.h"
#include "cgi.h"
#include "sysclk.h"
#include "conf_eth.h"

#define CGI_MATCH_NONE   0
#define CGI_MATCH_WORD   1  /* Select item in table only if string match */
#define CGI_MATCH_EXT    2  /* Select item in table if the extention match */
#define CGI_MATCH_NAME   3  /* Select item in table if the string is content */

#define CHIPID_NVTYP   (((CHIPID->CHIPID_CIDR) & \
	CHIPID_CIDR_NVPTYP_Msk) >>  CHIPID_CIDR_NVPTYP_Pos)
#define CHIPID_NVPSIZ  (((CHIPID->CHIPID_CIDR) & \
	CHIPID_CIDR_NVPSIZ_Msk) >>  CHIPID_CIDR_NVPSIZ_Pos)
#define CHIPID_SRAMSIZ (((CHIPID->CHIPID_CIDR) & \
	CHIPID_CIDR_SRAMSIZ_Msk) >>  CHIPID_CIDR_SRAMSIZ_Pos)
#define CHIPID_ARCH    (((CHIPID->CHIPID_CIDR) & \
	CHIPID_CIDR_ARCH_Msk) >>  CHIPID_CIDR_ARCH_Pos)
#define CHIPID_EPRCOC  (((CHIPID->CHIPID_CIDR) & \
	CHIPID_CIDR_EPROC_Msk) >>  CHIPID_CIDR_EPROC_Pos)

/* Macro to unpack the ip address from LwIP format into 4 integers. */
#define IP_ADDR_TO_INT_TUPLE(addr) \
	(int)((addr) >>  0 & 0xff), \
	(int)((addr) >>  8 & 0xff), \
	(int)((addr) >> 16 & 0xff), \
	(int)((addr) >> 24 & 0xff)

static const char _unknown[] = "unknown";

static const char *const chip_id_eproc_names[] = {
	_unknown,                               /* 0 */
	"arm946es",                             /* 1 */
	"arm7tdmi",                             /* 2 */
	"cortex-m3",                            /* 3 */
	"arm920t",                              /* 4 */
	"arm926ejs",                            /* 5 */
	"cortex-a5",                            /* 6 */
	"cortex-m4"                             /* 7 */
};

static const char *const chip_id_nvpsize[] = {
	"none",                                 /*  0 */
	"8K bytes",                             /*  1 */
	"16K bytes",                            /*  2 */
	"32K bytes",                            /*  3 */
	_unknown,                               /*  4 */
	"64K bytes",                            /*  5 */
	_unknown,                               /*  6 */
	"128K bytes",                           /*  7 */
	_unknown,                               /*  8 */
	"256K bytes",                           /*  9 */
	"512K bytes",                           /* 10 */
	_unknown,                               /* 11 */
	"1024K bytes",                          /* 12 */
	_unknown,                               /* 13 */
	"2048K bytes",                          /* 14 */
	_unknown,                               /* 15 */
};

static const char *const chip_id_sramsize[] = {
	"48K bytes",                            /*  0 */
	"1K bytes",                             /*  1 */
	"2K bytes",                             /*  2 */
	"6K bytes",                             /*  3 */
	"112K bytes",                           /*  4 */
	"4K bytes",                             /*  5 */
	"80K bytes",                            /*  6 */
	"160K bytes",                           /*  7 */
	"8K bytes",                             /*  8 */
	"16K bytes",                            /*  9 */
	"32K bytes",                            /* 10 */
	"64K bytes",                            /* 11 */
	"128K bytes",                           /* 12 */
	"256K bytes",                           /* 13 */
	"96K bytes",                            /* 14 */
	"512K bytes",                           /* 15 */
};

static const struct archnames { unsigned value; const char *name; }
chip_id_archnames[] = {
	{ 0x19, "AT91SAM9xx Series" },
	{ 0x29, "AT91SAM9XExx Series" },
	{ 0x34, "AT91x34 Series" },
	{ 0x37, "CAP7 Series" },
	{ 0x39, "CAP9 Series" },
	{ 0x3B, "CAP11 Series" },
	{ 0x3C, "SAM4E Series" },
	{ 0x40, "AT91x40 Series" },
	{ 0x42, "AT91x42 Series" },
	{ 0x55, "AT91x55 Series" },
	{ 0x60, "AT91SAM7Axx Series" },
	{ 0x61, "AT91SAM7AQxx Series" },
	{ 0x63, "AT91x63 Series" },
	{ 0x70, "AT91SAM7Sxx Series" },
	{ 0x71, "AT91SAM7XCxx Series" },
	{ 0x72, "AT91SAM7SExx Series" },
	{ 0x73, "AT91SAM7Lxx Series" },
	{ 0x75, "AT91SAM7Xxx Series" },
	{ 0x76, "AT91SAM7SLxx Series" },
	{ 0x80, "ATSAM3UxC Series (100-pin version)" },
	{ 0x81, "ATSAM3UxE Series (144-pin version)" },
	{ 0x83, "ATSAM3AxC Series (100-pin version)" },
	{ 0x84, "ATSAM3XxC Series (100-pin version)" },
	{ 0x85, "ATSAM3XxE Series (144-pin version)" },
	{ 0x86, "ATSAM3X8H Series (217-pin version)" },
	{ 0x88, "ATSAM3SxA Series (48-pin version)" },
	{ 0x89, "ATSAM3SxB Series (64-pin version)" },
	{ 0x8A, "SAM4S Series" },
	{ 0x92, "AT91x92 Series" },
	{ 0x95, "ATSAM3NxC Series (100-pin version)" },
	{ 0xF0, "AT75Cxx Series" },
	{ 0, NULL },
};

/* CGI handlers are not thread safe: tx_buf must be protected. */
static sys_sem_t cgi_sem;

/* TX buffer size to answer HTTP request. */
#define TX_REQ_BUFLEN 			1024
static uint8_t tx_buf[TX_REQ_BUFLEN];

/* FreeRTOS buffer size to list tasks and CPU usage. */
#define FREERTOS_STATS_BUFLEN 	512
static int8_t freertos_stats[FREERTOS_STATS_BUFLEN];

/* Function declarations */
static int cgi_error(struct netconn *client, const char *name, char *recv_buf, size_t recv_len);
static int cgi_status(struct netconn *client, const char *name, char *recv_buf, size_t recv_len);
static int cgi_chipInfo(struct netconn *client, const char *name, char *recv_buf, size_t recv_len);

/*
 * CGI table where we associate one callback to one page.
 */
HttpCGI cgi_table[] = {
	{ CGI_MATCH_NAME, "error", cgi_error },
	{ CGI_MATCH_NAME, "status", cgi_status },
	{ CGI_MATCH_NAME, "get_chipinfo", cgi_chipInfo },
	{ CGI_MATCH_NONE, NULL, NULL }
};

/**
 * \brief Used to handle unsupported CGI requests.
 *
 * \param name Not used.
 * \param recv_buf Receive buffer.
 * \param recv_len Receive buffer length.
 *
 * \return -1.
 */
static int cgi_error(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)client;
	(void)name;
	(void)recv_buf;
	(void)recv_len;

	return -1;
}

/**
 * \brief Send a JSON string representing the board status.
 *
 * \param name Not used.
 * \param recv_buf Receive buffer.
 * \param recv_len Receive buffer length.
 *
 * \return 0.
 */
static int cgi_status(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;
	uint32_t length = 0;
	uint32_t nb = 11;
	uint32_t i, count, new_entry;

#if LWIP_STATS
	extern uint32_t lwip_tx_rate;
	extern uint32_t lwip_rx_rate;
#else
	volatile uint32_t lwip_tx_rate = 0;
	volatile uint32_t lwip_rx_rate = 0;
#endif

	/* Protect tx_buf buffer from concurrent access. */
	sys_arch_sem_wait(&cgi_sem, 0);

	status.tot_req++;
	status.up_time = xTaskGetTickCount() / 1000;

	/* Update board status. */
	sprintf(status.last_connected_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->remote_ip.addr));
	sprintf(status.local_ip, "%d.%d.%d.%d", IP_ADDR_TO_INT_TUPLE(client->pcb.ip->local_ip.addr));
	length += sprintf((char *)tx_buf, "{\"board_ip\":\"%s\",\"remote_ip\":\"%s\",\"download\":%u,\"upload\":%u",
								status.local_ip, status.last_connected_ip,
								lwip_rx_rate, lwip_tx_rate);

	/* Turn FreeRTOS stats into JSON. */
	vTaskGetRunTimeStats(freertos_stats);
	length += sprintf((char *)tx_buf + length, ",\"rtos\":{\"10");
	// i = 2 to skip first 13 10 sequence.
	for (i = 2, count = 0, new_entry = 0; i < FREERTOS_STATS_BUFLEN && freertos_stats[i]; ++i) {
		if (freertos_stats[i] == 13) {
			tx_buf[length++] = '\"';
			new_entry = 1;
			continue;
		}
		if (freertos_stats[i] == 10)
			continue;
		if (freertos_stats[i] == 9) {
			count += 1;
			if (count == 4) {
				tx_buf[length++] = '\"';
				tx_buf[length++] = ':';
				tx_buf[length++] = '\"';
				count = 0;
				continue;
			}
		}
		if (count != 0)
			continue;

		if (new_entry == 1) {
			new_entry = 0;
			tx_buf[length++] = ',';
			tx_buf[length++] = '\"';
			/* Append ID to task name since JSON id must be unique. */
			tx_buf[length++] = '0' + nb / 10;
			tx_buf[length++] = '0' + nb % 10;
			nb++;
		}
		tx_buf[length++] = freertos_stats[i];
	}
	tx_buf[length++] = '}';

	char * memp_names[] = {
#define LWIP_MEMPOOL(name,num,size,desc) desc,
#include "lwip/memp_std.h"
	};
	length += sprintf((char *)tx_buf + length, ",\"lwip\":{");
#if MEM_STATS || MEMP_STATS
	length += sprintf((char *)tx_buf + length, "\"HEAP\":{\"Cur\":%d,\"Size\":%d,\"Max\":%d,\"Err\":%u}", lwip_stats.mem.used, lwip_stats.mem.avail, lwip_stats.mem.max, lwip_stats.mem.err);
	if (MEMP_MAX > 0)
		tx_buf[length++] = ',';
#endif
	for (uint32_t z= 0; z < MEMP_MAX; ++z) {
		length += sprintf((char *)tx_buf + length, "\"%s\":{\"Cur\":%d,\"Size\":%d,\"Max\":%d,\"Err\":%u}", memp_names[z], lwip_stats.memp[z].used, lwip_stats.memp[z].avail, lwip_stats.memp[z].max, lwip_stats.memp[z].err);
		if (z + 1 < MEMP_MAX)
			tx_buf[length++] = ',';
	}
	tx_buf[length++] = '}';

	/* Remaining board status. */
	length += sprintf((char *)tx_buf + length, ",\"up_time\":%u,\"tot_req\":%u}", status.up_time, status.tot_req);

	/* Send answer. */
	http_sendOk(client, HTTP_CONTENT_JSON);
	/* Use NETCONN_COPY to avoid corrupting the buffer after releasing the semaphore. */
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);

	/* Release semaphore to allow further use of tx_buf. */
	sys_sem_signal(&cgi_sem);

	return 0;
}

/**
 * \brief Return the chip ID name.
 *
 * \param idx Index in the chip_id_eproc_names table.
 *
 * \return The chip id name.
 */
static const char *chipid_eproc_name(int idx)
{
	if (idx <= 7) {
		return chip_id_eproc_names[idx];
	}

	return _unknown;
}

/**
 * \brief Return the non volatile programmable memory size.
 *
 * \param idx Index in the chip_id_nvpsize table.
 *
 * \return The chip non volatile programmable memory size.
 */
static const char *chipid_nvpsize(int idx)
{
	if (idx <= 15) {
		return chip_id_nvpsize[idx];
	}

	return _unknown;
}

/**
 * \brief Return the chip SRAM size.
 *
 * \param idx Index in the chip_id_sramsize table.
 *
 * \return The chip SRAM size.
 */
static const char *chipid_sramsize(int idx)
{
	if (idx <= 15) {
		return chip_id_sramsize[idx];
	}

	return _unknown;
}

/**
 * \brief Return the chip architecture name.
 *
 * \param idx Index in the chip_id_archnames table.
 *
 * \return The chip architecture name.
 */
static const char *chipid_archnames(unsigned value)
{
	for (int i = 0; chip_id_archnames[i].name; i++) {
		if (chip_id_archnames[i].value == value) {
			return chip_id_archnames[i].name;
		}
	}

	return _unknown;
}

/**
 * \brief Send the chip ID information.
 *
 * \param name Not used.
 * \param recv_buf Receive buffer.
 * \param recv_len Receive buffer length.
 *
 * \return 0.
 */
static int cgi_chipInfo(struct netconn *client, const char *name, char *recv_buf, size_t recv_len)
{
	(void)recv_buf;
	(void)recv_len;
	(void)name;

	/* Protect tx_buf buffer from concurrent access. */
	sys_arch_sem_wait(&cgi_sem, 0);

	sprintf((char *)tx_buf,
			"{\"core_name\":\"%s\",\"arch_name\":\"%s\",\"sram_size\":\"%s\",\"flash_size\":\"%s\"}",
			chipid_eproc_name(CHIPID_EPRCOC),
			chipid_archnames(CHIPID_ARCH),
			chipid_sramsize(CHIPID_SRAMSIZ),
			chipid_nvpsize(CHIPID_NVPSIZ));

	/* Send answer. */
	http_sendOk(client, HTTP_CONTENT_JSON);
	/* Use NETCONN_COPY to avoid corrupting the buffer after releasing the semaphore. */
	netconn_write(client, tx_buf, strlen((char *)tx_buf), NETCONN_COPY);

	/* Release semaphore to allow further use of tx_buf. */
	sys_sem_signal(&cgi_sem);

	return 0;
}

/**
 * \brief CGI request search engine.
 *
 * \param name CGI request name.
 * \param table CGI handler table.
 * \return A valid function handler for the specified CGI request, NULL otherwise.
 */
http_handler_t cgi_search(const char *name, HttpCGI *table)
{
	/* Allocate the CGI semaphore. */
	/* This is necessary to avoid race conditions on tx_buf.*/
	if (!sys_sem_valid(&cgi_sem)) {
		sys_sem_new(&cgi_sem, 1);
	}

	if (!table) {
		return NULL;
	}

	int i = 0;
	const char *ext = get_ext(name);

	while (table[i].name) {
		if (ext && table[i].type == CGI_MATCH_EXT) {
			if (!strcmp(table[i].name, ext)) {
				break;
			}
		} else if (table[i].type == CGI_MATCH_NAME) {
			if (strstr(name, table[i].name) != NULL) {
				break;
			}
		} else { /* (table[i].type == CGI_MATCH_WORD) */
			if (!strcmp(table[i].name, name)) {
				break;
			}
		}

		i++;
	}

	return table[i].handler;
}
