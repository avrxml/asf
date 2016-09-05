/**
 * \file
 *
 * \brief IoT Cloud Demo.
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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** Access point configuration for provisioning. */
#define DEMO_WLAN_AP_NAME                               "WINC1500_00:00"
#define DEMO_WLAN_AP_CHANNEL                            M2M_WIFI_CH_11
#define DEMO_WLAN_AP_WEP_INDEX                          (0)
#define DEMO_WLAN_AP_WEP_SIZE                           WEP_40_KEY_STRING_SIZE
#define DEMO_WLAN_AP_WEP_KEY                            "1234567890"
#define DEMO_WLAN_AP_SECURITY                           M2M_WIFI_SEC_OPEN
#define DEMO_WLAN_AP_MODE                               SSID_MODE_VISIBLE
#define DEMO_WLAN_AP_DOMAIN_NAME                        "atmelconfig.com"
#define DEMO_WLAN_AP_IP_ADDRESS                         {192, 168, 1, 1}

/** PubNub Settings. */
#define DEMO_PUBNUB_PUBLISH_KEY                         "demo"
#define DEMO_PUBNUB_SUBSCRIBE_KEY                       "demo"
#define DEMO_PUBNUB_CHANNEL						        "WINC1500_00:00"
#define DEMO_PUBNUB_PUBLISH_INTERVAL                    (3000)
#define DEMO_PUBNUB_SUBSCRIBE_INTERVAL                  (1000)

/** Android App related Settings. */
#define DEMO_ANDROID_REPORT_INTERVAL                    (1000)
#define DEMO_ANDROID_SENSOR_NAME                        "Temp1"
#define DEMO_ANDROID_SERVER_PORT                        (6666)

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
