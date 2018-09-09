/**
 * \file
 *
 * \brief Display organization for this application.
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
#ifndef CONF_APP_DISPLAY_H_INCLUDED
#define CONF_APP_DISPLAY_H_INCLUDED

#define DISPLAY_INTRO_ATMEL_DELAY           3000lu /* Unit ms */
#define DISPLAY_INTRO_MSG_EXIT              "SW1 to skip intro"
#define DISPLAY_INTRO_MSG_DELAY             7000lu /* Unit ms */
#define DISPLAY_INTRO_MSG_A                 "The demo gets sensors"
#define DISPLAY_INTRO_MSG_B                 "samples and outputs"
#define DISPLAY_INTRO_MSG_C                 "them on DISP, USB, SD"
#define DISPLAY_INTRO_HELP_DELAY            7000lu /* Unit ms */
#define DISPLAY_INTRO_HELP_A                "SW0 turns ON/OFF USB"
#define DISPLAY_INTRO_HELP_B                "SW1 ejects MicroSD"
#define DISPLAY_INTRO_HELP_C                "QTBs change sampling"
#define DISPLAY_INTRO_INF_DELAY             7000lu /* Unit ms */
#define DISPLAY_INTRO_INF_A                 "USB driver inf file"
#define DISPLAY_INTRO_INF_B                 "is written on uSD"
#define DISPLAY_INTRO_INF_C                 "at startup"

#define DISPLAY_CPU_LOAD_TEXT_POS_X         0
#define DISPLAY_CPU_LOAD_TEXT_POS_Y         0
#define DISPLAY_CPU_LOAD_TEXT               "CPU load"
#define DISPLAY_CPU_LOAD_PROBAR_POS_X       DISPLAY_CPU_LOAD_TEXT_POS_X
#define DISPLAY_CPU_LOAD_PROBAR_POS_Y       (DISPLAY_CPU_LOAD_TEXT_POS_Y + 9)
#define DISPLAY_CPU_LOAD_PROBAR_SIZE_Y      (4)
#define DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X  (6 * 8)

#define DISPLAY_SAMPLING_TEXT_POS_X         ((128 / 2) + 3)
#define DISPLAY_SAMPLING_TEXT_POS_Y         0
#define DISPLAY_SAMPLING_TEXT               "Sampling\r\n%2u,%01us"
#define DISPLAY_SAMPLING_PROCIRCLE_POS_X    (DISPLAY_SAMPLING_TEXT_POS_X + 6 * 5 + 13)
#define DISPLAY_SAMPLING_PROCIRCLE_POS_Y    (DISPLAY_SAMPLING_TEXT_POS_Y + 9 + 5)
#define DISPLAY_SAMPLING_PROCIRCLE_RADIUS   (4)
#define DISPLAY_SAMPLING_PROCIRCLE_MAX_SIZE_X  (6 * 8)

#define DISPLAY_LIGHT_TEXT_POS_X            DISPLAY_SAMPLING_TEXT_POS_X
#define DISPLAY_LIGHT_TEXT_POS_Y            19
#define DISPLAY_LIGHT_TEXT                  "Light"
#define DISPLAY_LIGHT_PROBAR_POS_X          DISPLAY_LIGHT_TEXT_POS_X
#define DISPLAY_LIGHT_PROBAR_POS_Y          (DISPLAY_LIGHT_TEXT_POS_Y + 9)
#define DISPLAY_LIGHT_PROBAR_SIZE_Y         (4)
#define DISPLAY_LIGHT_PROBAR_MAX_SIZE_X     (6 * 6)

#define DISPLAY_TEMP_TEXT_POS_X             (DISPLAY_TEMP_TEXT_VALUE_POS_X + (6 * 2) + 2)
#define DISPLAY_TEMP_TEXT_POS_Y             23
#define DISPLAY_TEMP_TEXT                   "C"
#define DISPLAY_TEMP_TEXT_VALUE_POS_X       (107)
#define DISPLAY_TEMP_TEXT_VALUE_POS_Y       DISPLAY_TEMP_TEXT_POS_Y

#define DISPLAY_USB_STA_POS_X               0
#define DISPLAY_USB_STA_POS_Y               16
#define DISPLAY_USB_STA_SIZE_X              24
#define DISPLAY_USB_STA_SIZE_Y              16
#define DISPLAY_USB_STACDC_POS_X            DISPLAY_USB_STA_POS_X
#define DISPLAY_USB_STACDC_POS_Y            (DISPLAY_USB_STA_POS_Y + 2)
#define DISPLAY_USB_STACDC_SIZE_X           DISPLAY_USB_STA_SIZE_X
#define DISPLAY_USB_STACDC_SIZE_Y           13

#define DISPLAY_MICROSD_STA_POS_X           32
#define DISPLAY_MICROSD_STA_POS_Y           18
#define DISPLAY_MICROSD_STA_SIZE_X          ((6 * 3) + 4)
#define DISPLAY_MICROSD_STA_SIZE_Y          (8 + 4)
#define DISPLAY_MICROSD_STA_TEXT_POS_X      (DISPLAY_MICROSD_STA_POS_X + 2)
#define DISPLAY_MICROSD_STA_TEXT_POS_Y      (DISPLAY_MICROSD_STA_POS_Y + 2)
#define DISPLAY_MICROSD_STA_TEXT            "uSD"

#endif /* CONF_APP_DISPLAY_H_INCLUDED */
