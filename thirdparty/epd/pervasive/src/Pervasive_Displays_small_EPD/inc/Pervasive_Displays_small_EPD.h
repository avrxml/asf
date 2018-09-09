/**
* \file
*
* \brief The definition of Pervasive Displays Inc.'s EPDs
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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
*/

#ifndef 	EPAPER_H_INCLUDED
#define 	EPAPER_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <asf.h>

/** 
 * \brief Developer needs to create an external function if wants to read flash */
typedef void (*EPD_read_flash_handler)(long flash_address,uint8_t *target_buffer, 
		uint8_t byte_length);
		
#if !defined(FALSE)
#define FALSE 0 /**< define FALSE=0 */
#endif

#if !defined(TRUE)
#define TRUE (1) /**< define TRUE=1 */
#endif

#if !defined(NULL)
#define NULL (void *)0  /**< define NULL */
#endif
#define _NOP() asm("nop") /**< define NOP for delay*/

#define LINE_SIZE	64  /**< maximum data line size */

/**
 * \brief Support 1.44", 2" and 2.7" three type EPD */
enum EPD_SIZE {
	EPD_144,
	EPD_200,
	EPD_270
};

#include "conf_EPD.h"
#include "EPD_hardware_gpio.h"
#include "EPD_hardware_driver.h"
#include "EPD_COG_process.h"
#include "EPD_controller.h"

#endif	//EPAPER_H_INCLUDED



