/**
* \file  config.h
*
* \brief WSNDemo application and stack configuration
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "miwi_config.h"
#include  "miwi_config_mesh.h"
/*****************************************************************************
*****************************************************************************/
#define APP_SENDING_INTERVAL    10000

#define APP_SENDING_INTERVAL_IN_SIMULATION 60000

#define APP_NWKSTATUS_INTERVAL  1000

#define APP_RX_BUF_SIZE         20

#define PAN_COORDINATOR_SHORT_ADDRESS   0x0000

/* To display the short address like -0x1234 */
#define SHORT_ADDRESS_CAPTION_SIZE     7

#if defined(PAN_COORDINATOR)
#if defined(OTAU_ENABLED)
#if defined(OTAU_SERVER)
#define APP_CAPTION     "Server-PAN Coordinator"
#else
#define APP_CAPTION     "Client-PAN Coordinator"
#endif
#else
#define APP_CAPTION     "PAN Coordinator"
#endif
#define APP_NODE_TYPE   0
#define APP_COORDINATOR 1
#define APP_ROUTER      0
#define APP_ENDDEVICE   0
#elif defined(COORDINATOR)
#if defined(OTAU_ENABLED)
#if defined(OTAU_SERVER)
#define APP_CAPTION     "Server-Coordinator"
#else
#define APP_CAPTION     "Client-Coordinator"
#endif
#else
#define APP_CAPTION     "Coordinator"
#endif
#define APP_CAPTION_ED_SIZE  (sizeof(APP_CAPTION_ED) - 1 + SHORT_ADDRESS_CAPTION_SIZE)
#if defined(OTAU_ENABLED)
#if defined(OTAU_SERVER)
#define APP_CAPTION_ED     "Server-End Device"
#else
#define APP_CAPTION_ED     "Client-End Device"
#endif
#else
#define APP_CAPTION_ED     "End Device"
#endif
#define APP_NODE_TYPE   1
#define APP_COORDINATOR 0
#define APP_ROUTER      1
#define APP_ENDDEVICE   0
#else
#if defined(OTAU_ENABLED)
#if defined(OTAU_SERVER)
#define APP_CAPTION     "Server-End Device"
#else
#define APP_CAPTION     "Client-End Device"
#endif
#else
#define APP_CAPTION     "End Device"
#endif
#define APP_NODE_TYPE   2
#define APP_COORDINATOR 0
#define APP_ROUTER      0
#define APP_ENDDEVICE   1
#endif

#endif /* _CONFIG_H_ */
