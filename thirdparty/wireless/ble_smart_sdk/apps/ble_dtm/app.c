/**
 * \file
 *
 * \brief DTM Application
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

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the iBeacon Application
 */
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "at_ble_api.h"
#include "platform.h"

int main(void)
{
	uint16_t plf_event_type;
	uint8_t plf_event_data[16];
	uint16_t plf_event_data_len;
	uint32_t sleep_count = 0;
	uint32_t clk_enables;
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_driver_init();
	clk_enables = *((volatile unsigned int *)0x4000b00c);
	clk_enables |= 0x1E000;
	*((volatile unsigned int *)0x4000b00c) = clk_enables;
	acquire_sleep_lock();
	while(1)
	{
		while(platform_event_get(&plf_event_type,plf_event_data,&plf_event_data_len))// for WFI(wait for Interrupt)
		{
			//This is  added just not to optimize it out.
			sleep_count++;
			sleep_count--;
		}
	}
	
	return status;
}
