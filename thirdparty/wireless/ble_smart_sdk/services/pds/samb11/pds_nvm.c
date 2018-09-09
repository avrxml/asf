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
	pds_status_t pds_status = PDS_SUCCESS;
	spi_flash_turn_on();
	spi_flash_clock_init();
	spi_flash_init();

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
	pds_status_t status = PDS_SUCCESS; 
	
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}
	
	if(spi_flash_write((unsigned char *)buffer, \
				(unsigned long)mem_address, \
				len) == -1){
					status = PDS_NVM_WRITE_ERROR;
	}
	

	return status;	
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

	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}

	spi_flash_read((unsigned char *)buffer, \
			(unsigned long)mem_address, \
			len);

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
	
	if (pds_instance->flash_id != PDS_INTERNAL_FLASH_ID){
		/*  Unsupported NVM identifier */
		return PDS_INVALID_FLASH_ID;
	}

	if(spi_flash_erase((unsigned long)mem_address, len)){
		status = PDS_NVM_DELETE_ERROR;
	}
	
	return status;
}

