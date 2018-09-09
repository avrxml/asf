/**
 * \file
 *
 * \brief mode change configuration.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef MODES_H_INCLUDED
#define MODES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/** AP mode Settings */
#define MAIN_WLAN_SSID               "WILC_DEMO_AP" /* < SSID */
#define MAIN_WLAN_AUTH               M2M_WIFI_SEC_OPEN /* < Security manner */
#define MAIN_WLAN_AP_CHANNEL         M2M_WIFI_CH_6 /* < AP Channel number */

/** P2P mode Settings */
#define MAIN_WLAN_DEVICE_NAME        "WINC_P2P" /* < P2P Device Name */
#define MAIN_WLAN_P2P_CHANNEL        (M2M_WIFI_CH_11) /* < P2P Channel number */

#define HOLD_TIME_IN_MODE               (60000)
#define DELAY_FOR_MODE_CHANGE           (500)

void mode_change(void* argument);
#ifdef __cplusplus
}
#endif

#endif /* MODES_H_INCLUDED */
