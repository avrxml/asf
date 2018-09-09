/*
 * @file pal.c
 *
 * @brief Performs interface functionalities between the TAL layer and ASF
 * drivers
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "pal.h"
#include "delay.h"
#include <string.h>
#if (SAMD || SAMR21 || SAML21 || SAMR30)
#include "port.h"
#else
#include "ioport.h"
#endif

bool pal_calibrate_rc_osc(void)
{
	return (true);
}

retval_t pal_init(void)
{
#if (PAL_USE_SPI_TRX == 1)
	trx_spi_init();
#endif /* #if (PAL_USE_SPI_TRX = 1) */
#ifdef ENABLE_STACK_NVM
#if (SAMD20) || (SAMD21) || (SAMR21)
	nvm_init(INT_FLASH);
#endif
#endif
	return MAC_SUCCESS;
}

#ifdef ENABLE_STACK_NVM
retval_t pal_ps_get(ps_type_t mem_type, uint16_t offset, uint16_t length,
		void *value)
{
	nvm_read(INT_FLASH,
			(uint32_t)offset + INT_FLASH_END - STACK_FLASH_SIZE + 1,
			value,
			length);
	return MAC_SUCCESS;
}

retval_t pal_ps_set(uint16_t offset, uint16_t length, void *value)
{
	nvm_write(INT_FLASH,
			(uint32_t)offset + INT_FLASH_END - STACK_FLASH_SIZE + 1,
			value, length);
	return MAC_SUCCESS;
}

#endif

retval_t pal_timer_stop(uint8_t timer_id)
{
	uint8_t status;

	status = sw_timer_stop(timer_id);

	if (STATUS_OK == (status_code_genare_t)status) {
		return MAC_SUCCESS;
	} else if (ERR_TIMER_NOT_RUNNING == (status_code_t)status) {
		return PAL_TMR_NOT_RUNNING;
	} else {
		return PAL_TMR_INVALID_ID;
	}
}

void pal_timer_source_select(source_type_t source)
{
	/*    if (TMR_CLK_SRC_DURING_TRX_SLEEP == source) */
	/*    { */
	/*        TIMER_SRC_DURING_TRX_SLEEP(); */
	/*    } */
	/*    else */
	/*    { */
	/*        TIMER_SRC_DURING_TRX_AWAKE(); */
	/*    } */
}

retval_t pal_timer_get_id(uint8_t *timer_id)
{
	status_code_genare_t status;
	status = sw_timer_get_id(timer_id);

	if (STATUS_OK == status) {
		return MAC_SUCCESS;
	}

	return PAL_TMR_INVALID_ID;
}

retval_t pal_timer_start(uint8_t timer_id,
		uint32_t timer_count,
		timeout_type_t timeout_type,
		FUNC_PTR timer_cb,
		void *param_cb)
{
	uint8_t status;
	status = sw_timer_start(timer_id, timer_count,
			(sw_timeout_type_t)timeout_type,
			timer_cb, param_cb);

	if (ERR_TIMER_ALREADY_RUNNING == (status_code_t)status) {
		/*
		 * Timer is already running if the callback function of the
		 * corresponding timer index in the timer array is not NULL.
		 */
		return PAL_TMR_ALREADY_RUNNING;
	}

	if (STATUS_OK == (status_code_genare_t)status) {
		return MAC_SUCCESS;
	}

	return MAC_INVALID_PARAMETER;
}

bool pal_is_timer_running(uint8_t timer_id)
{
	return sw_timer_is_running(timer_id);
}

/**
 * @brief Services timer and sio handler
 *
 * This function calls sio & timer handling functions.
 */
void pal_task(void)
{
	sw_timer_service();
}

void pal_trx_read_timestamp(uint32_t *timestamp)
{
	*timestamp  = sw_timer_get_time();
}

void pal_get_current_time(uint32_t *timer_count)
{
	uint32_t time_val;
	/* This will avoid the hard faults, due to aligned nature of access */
	time_val = sw_timer_get_time();
	MEMCPY_ENDIAN((uint8_t *)timer_count, (uint8_t *)&time_val,
			sizeof(time_val));
}
