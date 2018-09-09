/**
 * \file
 *
 * \brief Non volatile memories management for XMEGA devices
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
#include "common_nvm.h"
#include "conf_nvm.h"
#include "flash.h"

status_code_t nvm_init(mem_type_t mem)
{
	switch (mem) {
	case INT_FLASH:
		/* No initialization required for internal memory */
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_read_char(mem_type_t mem, uint32_t address, uint8_t *data)
{
	switch (mem) {
	case INT_FLASH:
		flash_read(address, 1, data);
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_write_char(mem_type_t mem, uint32_t address, uint8_t data)
{
	switch (mem) {
	case INT_FLASH:
		flash_write(address, 1, &data);
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_read(mem_type_t mem, uint32_t address, void *buffer,
		uint32_t len)
{
	switch (mem) {
	case INT_FLASH:
		flash_read(address, len, buffer);
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_write(mem_type_t mem, uint32_t address, void *buffer,
		uint32_t len)
{
	switch (mem) {
	case INT_FLASH:
		flash_write(address, len, buffer);
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_page_erase(mem_type_t mem, uint32_t page_number)
{
	switch (mem) {
	case INT_FLASH:

		flash_erase_page(page_number);

		break;

	default:
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_get_size(mem_type_t mem, uint32_t *size)
{
	switch (mem) {
	case INT_FLASH:
		*size = (uint32_t)FLASH_SIZE;
		break;

	default:
		/* Other memories not supported */
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_get_page_size(mem_type_t mem, uint32_t *size)
{
	switch (mem) {
	case INT_FLASH:
		*size = (uint32_t)FLASH_PAGE_SIZE;
		break;

	default:
		/* Other memories not supported */
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

status_code_t nvm_get_pagenumber(mem_type_t mem, uint32_t address,
		uint32_t *num)
{
	switch (mem) {
	case INT_FLASH:
		*num = (uint32_t)(address / FLASH_PAGE_SIZE);
		break;

	default:
		/* Other memories not supported */
		return ERR_INVALID_ARG;
	}

	return STATUS_OK;
}
