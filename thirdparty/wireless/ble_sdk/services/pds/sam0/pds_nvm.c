/**
* \file
*
* \brief Persistent Storage NVM management.
*
* Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries. 
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

#include <asf.h>
#include <string.h>
#include "pds_nvm.h"
#include "conf_nvm.h"
#include "ble_utils.h"

pds_env_t pds_env_cfg = {
	.flash_id = PDS_INTERNAL_FLASH_ID,
	.pds_nvm_start_address = PDS_START_ADDRESS,	
	.pds_sector_count = PDS_SECTOR_COUNT,
	.pds_sector_size = PDS_SECTOR_SIZE	
};

/**
 * \brief  Initialize the PDS NVM memory controller.
 *
 *  Initialize the NVM memory controller, based on the NVM id the respective driver API's 
 * must be called, If any errors in the initialization will result in PDS Initialization error
 *
 * \param[in] pds_instance Initialized PDS environment structure
 *
 * \return Status of the PDS Initialize procedure
 *
 * \retval PDS_SUCCESS PDS Initialize procedure is completed
 * 
 * \retval  PDS_INIT_ERROR PDS initialization failed, due to inactive sector or NVM Init failures
 * 
 * \retval  for other return values check pds_status_t enum values
 */
pds_status_t pds_nvm_init(pds_env_t *pds_instance)
{
	pds_status_t pds_status = PDS_NVM_INIT_ERROR;
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}
	
	/* Initialize the NVM */
	struct nvm_config nvm_cfg;
	nvm_get_config_defaults(&nvm_cfg);
	nvm_cfg.manual_page_write = false;
	nvm_cfg.wait_states = 2;

	if(nvm_set_config(&nvm_cfg) == STATUS_OK){
		pds_status = PDS_SUCCESS;
	}
	
	/* Storage must start at row start */
	Assert((PDS_START_ADDRESS % (NVMCTRL_PAGE_SIZE * 4)) == 0);
	/* Sector must be of integer number of rows */
	Assert((PDS_SECTOR_SIZE % ((NVMCTRL_PAGE_SIZE * 4)) == 0));
	
	return pds_status;	
}

/**
 * \brief  PDS NVM write operation
 *
 *  Write the PDS data into NVM address location
 *
 * \param[in] pds_instance Initialized PDS environment structure
 *
 * \param[in] mem_address NVM address location
 *
 * \param[in] buffer NVM write buffer
 * 
 * \param[in] len NVM write buffer size
 * 
 * \return Status of the NVM write procedure
 *
 * \retval PDS_SUCCESS NVM write procedure is completed
 * 
 * \retval  for other return values check pds_status_t enum values
 */
pds_status_t pds_nvm_write(pds_env_t *pds_instance, uint32_t mem_address, uint8_t *buffer, uint16_t len)
{
	status_code_genare_t error_code = STATUS_OK;
	uint8_t row_buffer[NVMCTRL_ROW_PAGES * FLASH_PAGE_SIZE];
	volatile uint8_t *dest_add = (uint8_t *)mem_address;
	const uint8_t *src_buf = buffer;
	uint32_t i;
	
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}
	
	/* Calculate the starting row address of the page to update */
	uint32_t row_start_address	= mem_address &	~((FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);
	
	while (len) {
	/* Backup the contents of a row */
	for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
		do {
		error_code = nvm_read_buffer(row_start_address + (i * FLASH_PAGE_SIZE),
		(row_buffer + (i * FLASH_PAGE_SIZE)), FLASH_PAGE_SIZE);
		} while (error_code == STATUS_BUSY);
	
		if (error_code != STATUS_OK) {
		return ((error_code == STATUS_OK) ? PDS_SUCCESS : PDS_NVM_WRITE_ERROR);
		}
	}
	
	/* Update the buffer if necessary */
	for (i = row_start_address;	i < row_start_address +	(FLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES); i++) {
	if (len && ((uint8_t *)i == dest_add)) {
	row_buffer[i - row_start_address] = *src_buf++;
	dest_add++;
	len--;
	}
	}
	
	system_interrupt_enter_critical_section();
	
	/* Erase the row */
	do {
	error_code = nvm_erase_row(row_start_address);
	} while (error_code == STATUS_BUSY);
	
	if (error_code != STATUS_OK) {
	return ((error_code == STATUS_OK) ? PDS_SUCCESS : PDS_NVM_WRITE_ERROR);
	}
	
	
	/* Write the updated row contents to the erased row */
	for (i = 0; i < NVMCTRL_ROW_PAGES; i++) {
	do {
	error_code = nvm_write_buffer(row_start_address + (i * FLASH_PAGE_SIZE), (row_buffer + (i * FLASH_PAGE_SIZE)), FLASH_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);
	
	if (error_code != STATUS_OK) {
	return ((error_code == STATUS_OK) ? PDS_SUCCESS : PDS_NVM_WRITE_ERROR);
	}
	}
	
	system_interrupt_leave_critical_section();
	
	row_start_address += NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;
	}
	
	return ((error_code == STATUS_OK) ? PDS_SUCCESS : PDS_NVM_WRITE_ERROR);	
}

/**
 * \brief  PDS NVM  read operation
 *
 *  Read the PDS data from NVM address location for the given size
 *
 * \param[in] pds_instance Initialized PDS environment structure
 *
 * \param[in] mem_address NVM address location
 *
 * \param[in] buffer NVM read buffer
 *
 * \param[in] len NVM read buffer size
 * 
 * \return Status of the NVM read operation
 *
 * \retval PDS_SUCCESS NVM read procedure is completed
 * 
 * \retval  for other return values check pds_status_t enum values
 */
pds_status_t pds_nvm_read(pds_env_t *pds_instance, uint32_t mem_address, uint8_t *buffer, uint16_t len)
{
	uint32_t page_address;
	uint16_t idx;
	uint16_t data;
	uint8_t mem_hole;
	/* Get a pointer to the module hardware instance */
	Nvmctrl *const nvm_module = NVMCTRL;
	
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}
	
	Assert((mem_address <= NV_MEMORY_END) && ((mem_address + len) <= (NV_MEMORY_END + 1)));
	/* Check if the module is busy */
	if (!nvm_is_ready()) {
	return PDS_NVM_READ_ERROR;
	}
	
	/* Clear error flags */
	nvm_module->STATUS.reg = NVMCTRL_STATUS_MASK;
	page_address = (mem_address/2);
	mem_hole = mem_address%2;
	
	if (mem_hole)
	{
		data = NVM_MEMORY[page_address++];
		buffer[0] = (data >> 8);
	}
	
	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy to ensure alignment */
	for (idx = mem_hole; idx < len; idx += 2){
		/* Fetch next 16-bit chunk from the NVM memory space */
		data = NVM_MEMORY[page_address++];
		
		/* Copy first byte of the 16-bit chunk to the destination buffer */
		buffer[idx] = (data & 0xFF);
		/* Check the read request end is odd byte */
		if (idx < (len - 1)){
			buffer[idx + 1] = (data >> 8);
		}
	}
	return PDS_SUCCESS;	
}

/**
 * \brief  PDS NVM  erase operation
 *
 *  Erase the PDS data from NVM address location for the given size
 *
 * \param[in] pds_instance Initialized PDS environment structure
 *
 * \param[in] mem_address NVM address location
 *
 * \param[in] len NVM erase size
 * 
 * \return Status of the NVM read operation
 *
 * \retval PDS_SUCCESS NVM erase procedure is completed
 * 
 * \retval  for other return values check pds_status_t enum values
 */
pds_status_t pds_nvm_erase(pds_env_t *pds_instance, uint32_t mem_address, uint16_t len)
{
	pds_status_t status = PDS_SUCCESS;
	uint32_t row_start_addr;
	uint32_t page_nums;
	
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}

	row_start_addr = mem_address & ~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1);
	page_nums = len/NVMCTRL_PAGE_SIZE;
	
	system_interrupt_enter_critical_section();
	while (page_nums >= NVMCTRL_ROW_PAGES) {
		
		nvm_erase_row(row_start_addr);
		row_start_addr += NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;
		page_nums -= NVMCTRL_ROW_PAGES;
	}	
	system_interrupt_leave_critical_section();
	
	return status;
}

