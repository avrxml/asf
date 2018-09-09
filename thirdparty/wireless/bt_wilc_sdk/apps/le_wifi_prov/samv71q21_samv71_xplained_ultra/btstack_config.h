/**
 *
 * \file
 *
 * \brief Header file for BT stack configuration for SAMV71 + ATWILC3000.
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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
#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

#define CONF_MODE_WIFI_AND_BLE	
#define HAVE_UART_DMA_SET_FLOWCONTROL
#define HAVE_EMBEDDED_TIME_MS
#define HAVE_BTSTACK_STDIN
#define HAVE_FREERTOS_TASK_NOTIFICATIONS

#define ENABLE_BLE
#define ENABLE_CLASSIC
#define ENABLE_LOG_INFO
#define ENABLE_LOG_ERROR
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_CENTRAL

#define HCI_ACL_PAYLOAD_SIZE 1021

/*memory config*/
#define MAX_NR_AVDTP_CONNECTIONS 1
#define MAX_NR_AVDTP_STREAM_ENDPOINTS 1
#define MAX_NR_AVRCP_CONNECTIONS 1
#define MAX_NR_BNEP_CHANNELS 1
#define MAX_NR_BNEP_SERVICES 1
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES  2
#define MAX_NR_DB_MEM_DEVICE_LINK_KEYS  2
#define MAX_NR_DB_MEM_SERVICES 1
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_GATT_SUBCLIENTS 1
#define MAX_NR_HCI_CONNECTIONS 1
#define MAX_NR_HFP_CONNECTIONS 1
#define MAX_NR_L2CAP_CHANNELS  4
#define MAX_NR_L2CAP_SERVICES  3
#define MAX_NR_LE_DEVICE_DB_ENTRIES 1
#define MAX_NR_LE_DEVICE_DB_ENTRIES 1
#define MAX_NR_RFCOMM_CHANNELS 1
#define MAX_NR_RFCOMM_MULTIPLEXERS 1
#define MAX_NR_RFCOMM_SERVICES 2
#define MAX_NR_SERVICE_RECORD_ITEMS 2
#define MAX_NR_SM_LOOKUP_ENTRIES 1
#define MAX_NR_WHITELIST_ENTRIES 1
#endif

