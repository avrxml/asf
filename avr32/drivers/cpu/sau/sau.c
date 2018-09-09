/*****************************************************************************
 *
 * \file
 *
 * \brief SAU driver.
 *
 * SAU (Security Access Unit) driver module for AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "sau.h"

/*!
 * Control of the SAU control register CERH/L: enable CERH/L
 */
void sau_enable_channel(unsigned char channel_num)
{
	volatile avr32_sau_t *sau_reg = &AVR32_SAU;
	if (channel_num < AVR32_SAU_CERL_CERL_SIZE) {
		sau_reg->cerl |= (1 << channel_num);
	} else if (channel_num < (AVR32_SAU_CERL_CERL_SIZE 
		+ AVR32_SAU_CERH_CERH_SIZE)) {
		sau_reg->cerh |= (1 << (channel_num 
			- AVR32_SAU_CERL_CERL_SIZE));
	}
}


/*!
 * Control of the SAU control register CERH/L: disable CERH/L
 */
void sau_disable_channel(unsigned char channel_num)
{
	volatile avr32_sau_t *sau_reg = &AVR32_SAU;
	if (channel_num < AVR32_SAU_CERL_CERL_SIZE) {
		sau_reg->cerl &= ~(1 << channel_num);
	} else if (channel_num 
		< (AVR32_SAU_CERL_CERL_SIZE + AVR32_SAU_CERH_CERH_SIZE)) {
		sau_reg->cerh &= ~(1 << (channel_num
			- AVR32_SAU_CERL_CERL_SIZE));
	}
}

/*! \brief Configs specific HSB memories for a set of channels.
 *
 * \param sau_map: The memory and channel map.
 * \param size: The number of channels in an SAU sau_map.
 *
 * \return Bool TRUE if the config succeeded
 *              FALSE if the config failed (the input is not valid).
 */
bool sau_configure_channels(const sau_map_t sau_map, uint32_t size)
{
	uint32_t i;
	volatile avr32_sau_hsb_t *rtr_reg = &AVR32_SAU_HSB;
	
	for (i = 0; i < size; i++) {
		//The SAU is only able to remap address above 0xFFFC0000
		if (sau_map->HSB_mem < 0xFFFC000) {
			sau_exit_setup();
			return false;
		}
		//program the corresponding RTR with the remap target address
		rtr_reg->rtr[sau_map->channel_num] = sau_map->HSB_mem;
		sau_map++;
	}
	//when all channels are configured, return to normal mode
	sau_exit_setup();
	
	//enable all channels
	sau_map--;
	for (i = 0; i < size; i++) {
		sau_enable_channel(sau_map->channel_num);
		sau_map--;
	}
	
	return true;
}

/*!
 * \brief Lock SAU channels
 *
 * \param unlock_HSB_cycles: The number of HSB cycles the channel keeps unlocked
 * \param unlock_key: The key to lock channels
 *
 */
void sau_lock_channels(U8 unlock_HSB_cycles, U8 unlock_key)
{
	uint32_t data;
	data = (0 << AVR32_SAU_CONFIG_OPEN)
		| (unlock_HSB_cycles << AVR32_SAU_CONFIG_UCYC)
			| (unlock_key << AVR32_SAU_CONFIG_UKEY);
	AVR32_SAU.config = data;
}

/*!
 * \brief Unlock an SAU channel
 *
 * \param unlock_channel_number: The number of the channel to unlock
 * \param unlock_key: The key to unlock a channel
 *
 * \return Bool true if the config succeeded
 *              false if the config failed (the input is not valid).
 */
bool sau_unlock_channel(U8 unlock_channel_number, U8 unlock_key)
{
	uint32_t data;
	
	if (unlock_channel_number > (AVR32_SAU_CERL_CERL_SIZE
		+ AVR32_SAU_CERH_CERH_SIZE)) {
		return false;
	}
	
	data = unlock_channel_number | (unlock_key << AVR32_SAU_UR_KEY_OFFSET);
	AVR32_SAU_HSB.ur = data;
	return true;
}

/*!
 * \brief Read the HSB memory value by the SAU channel
 *
 * \param channel_num: The number of the channel to read
 *
 * \return:  the read value.
 */
uint32_t sau_read_channel(unsigned char channel_num)
{
	uint32_t data;
	volatile avr32_sau_hsb_t *rtr_reg = &AVR32_SAU_HSB;
	
	data = rtr_reg->rtr[channel_num];
	// The idle bit: 1 means idle; 0 means busy.
	while (!AVR32_SAU.SR.idle);
	
	return data;
}

/*!
 * \brief Write the HSB memory value by the SAU channel
 *
 * \param channel_num: The number of the channel to write.
 * \param data: The data to write
 */
void sau_write_channel(uint32_t data, unsigned char channel_num)
{
	AVR32_SAU_HSB.rtr[channel_num] = data;
	while (!AVR32_SAU.SR.idle);
}
