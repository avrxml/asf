/*
 * \file
 *
 * \brief WINC1500 Exosite Example.
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


#ifndef IO1_BOARD_H_INCLUDE
#define IO1_BOARD_H_INCLUDE

#include <ctype.h>
#include "driver/include/m2m_types.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#define IO1_BOARD_VENDOR_LENGTH_MAX 50
#define IO1_BOARD_ID_LENGTH_MAX 100
#define IO1_BOARD_SERIAL_NUMBER_LENGTH_MAX 100
#define IO1_BOARD_PASSWORD_LENGTH_MAX 100
#define IO1_BOARD_SSID_LENGTH_MAX 100

/** 
*	\brief structure of io1_board information.
*/
typedef struct _io1_board_info
{
	char board_vendor[IO1_BOARD_VENDOR_LENGTH_MAX];
	char board_id[IO1_BOARD_ID_LENGTH_MAX];
	char board_sn[IO1_BOARD_SERIAL_NUMBER_LENGTH_MAX];
	char cik[50];
	uint8_t is_activate;
}io1_board_info;

/*
*	\brief	Initialize each sensor of io1 board.
*
*	\return	true initialized sensor of io1 board.
*/
bool	io1_board_init(void);

/*
*	\brief Returns pointer to the io1-board information.
*	
*	\return	io1_board information ex> name, vendor, s/n.
*/
io1_board_info * io1_board_get_info(void);

/*
*	\brief	Prepares io1_sensor_module for using sensor information.
*			call it, before call io1_board_prepare_to_get_info function.
*/
void	io1_board_prepare_to_get_info(void);

/*
*	\brief	Returns the value of temperature sensor.
*
*	\return	the value of temperature sensor.	
*/
int		io1_board_get_temperature(void);


/*
*	\brief Returns the value of light sensor.
*
*	\return	the value of light sensor.
*/
int		io1_board_get_lightvalue(void);

/*
*	\brief  Sets the Io1 board LED (on,off).
*
*	\param	on	true, LED Light on.
*/
void	io1_board_set_led(bool on);

/*
*	\brief  Write cik value to SD card.
*
*	\param	cik value.
*	\return	true on success, false otherwise.
*/
bool	io1_board_save_cik_value_to_sd(char *pbuf);

/*
*	\brief  reset cik value
*
*/
void	io1_board_reset_cik_value(void);

#ifdef __cplusplus
}
#endif

#endif /* IO1_BOARD_H_ */