/**
* \file
*
* \brief This file for Persistent Data Storage management.
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
#include "ble_utils.h"
#include "conf_nvm.h"
#include "pds_nvm.h"
#include "pds.h"


/* ! \brief Static variable to store pds active sector information */
static uint8_t active_sector = PDS_INVALID_SECTOR;

/* ! \brief Static variable to store pds status information */
static bool pds_init_status = false;

/**
 * \brief  Initialize the PDS sectors and  NVM memory controller.
 *
 *  If the device is new and not initialized, then need to initialize the sector by
 * erasing the complete sector and writing the sector header. Also the sector header
 * status is set to valid status, Initial sector number will be set to 1. If sector's are initialized
 * then all sector's of the PDS must be scanned and the active sector is decided based on the 
 * higher value of the sector header number.
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
pds_status_t pds_init(pds_env_t *pds_instance)
{
	pds_status_t status;
	sector_header_t sec_header;
	uint8_t sector;	
	uint32_t last_sector_number = 0;
	
	Assert((pds_instance != NULL));
	
	if(pds_nvm_init(pds_instance) != PDS_SUCCESS){
		DBG_LOG_DEV("PDS NVM Init Failed");
		return PDS_INIT_ERROR;
	}

	/* Look for last active sector (Highest sector header number) */
	for (sector = 0; sector < pds_instance->pds_sector_count; sector++) {
				
		if((status = pds_nvm_read(pds_instance, \
					((sector * pds_instance->pds_sector_size) + pds_instance->pds_nvm_start_address), \
					(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
			return status;
		}
		
		if ((sec_header.status == 0) && (memcmp(sec_header.signature, PDS_SECTOR_SIGNATURE, 6) == 0)) {
			if (sec_header.number > last_sector_number) {
				last_sector_number = sec_header.number;
				active_sector      = sector;
			}
		}
	}
	
	/* No active/valid sector found. Initialize all the sector's */
	if (active_sector == PDS_INVALID_SECTOR) {
		/* Erase all the PDS sector and Initialize one sector (APÍ is common for both new device or reset the device to factory new) */
		if((status = pds_delete_all(pds_instance)) != PDS_SUCCESS){
			return status;
		}
	}
	
	if(active_sector < pds_instance->pds_sector_count){
		DBG_LOG_DEV("PDS Initialized\r\n");
		pds_init_status = true;
		return PDS_SUCCESS;
	}
	
	return PDS_INIT_ERROR;
}

/**
 * \brief  Read the PDS item from active sector
 *
 * Read the item id value from the active sector. Which will scan the
 * entire sector to find the valid block header of the item. If the item id in 
 * the block header matches, further it checks for non deletion and CRC matches.
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \param[in] item_id  16-bit item Identifier 
 *
 * \param[out]  buf buffer to store the item values that's matched
 *
 * \param[in] buf_len maximum length of the buffer to store the valid value
 *
 * \param[out] read_size Actual valid size of the item read into buffer, This helps the application to 
 *			retrieve and identify the variable size of similar item category. Where application can store variable 
 *                   size of data items into PDS and reduces wastage of PDS memory. The read size set 0 in case valid item not found.
 *			otherwise a valid item matched then the item size will be set to read_size referenced variable pointer.
 *
 * \return Status of the PDS Read procedure
 *
 * \retval PDS_SUCCESS PDS Item id found and item read success
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_read(pds_env_t *pds_instance, uint16_t item_id, uint8_t *buf, uint16_t buf_len, uint16_t *read_size)
{
	pds_status_t status = PDS_ITEM_NOT_FOUND;
	uint8_t lbuf[sizeof(block_header_t)];
	block_header_t *block_header_info;
	uint32_t address, address_max;	
	
	/* Validate the Input data */
	Assert((pds_init_status) && (buf != NULL) && (buf_len > 0) && \
			(item_id != 0) && (pds_instance != NULL) && (read_size != NULL));
		
	/* Initialize the read size value (0) to default */	
	*read_size = 0;
	
	address_max = (pds_instance->pds_nvm_start_address +  \
					(active_sector * pds_instance->pds_sector_size) + pds_instance->pds_sector_size);
	
	/* Calculate the read start address i.e start of the block address of the item 
	    by scanning the items in the sector also Skip the active sector header */
	address = pds_instance->pds_nvm_start_address + sizeof(sector_header_t) + \
				(active_sector * pds_instance->pds_sector_size);
	
	/* Read address must start and end within the active sector and must not read more than a active sector. */
	while ((address + sizeof(block_header_t)) < address_max){
		
		/* Read block header and find the item id */
		if((status = pds_nvm_read(pds_instance, address, lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return status;
		}
		address += sizeof(block_header_t);
		
		block_header_info = (block_header_t *)lbuf;
		
		/* Check Item matches with given item id */
		if (block_header_info->id == item_id){
						
			/* Check the Item is marked  for deletion and it's active */
			if ((PDS_BLOCK_DELETE_MASK(block_header_info->status)) && \
			(!PDS_BLOCK_ACTIVE_MASK(block_header_info->status))){
				/* Found Valid Item and check for the address ranges */
				if((address + block_header_info->item_size) >= address_max){
					return PDS_ITEM_INVALID;
				}
				
				if(block_header_info->item_size > buf_len){
					return PDS_ITEM_BIGGER_SIZE;
				}
				
				if((status = pds_nvm_read(pds_instance, address, buf, \
								block_header_info->item_size)) != PDS_SUCCESS){
					return status;
				}
				
				if(block_header_info->checksum == pds_calc_crc(buf, buf_len)){
					/* Copy the actual valid item size */
					*read_size = block_header_info->item_size;
					return PDS_SUCCESS;
				}			
			}
			status = PDS_ITEM_INVALID;
		}
		
		if(block_header_info->item_size == 0xFFFF){
			/* Reached empty space of active sector */
			break;					
		}
		
		/* Read Next Block Header */
		address += block_header_info->item_size;			
	}
		
	/* return the pds read status */
	return status;
}

/**
 * \brief  Write PDS item into active sector
 *
 * Scan the empty space in active sector. If the empty space is found the
 * item of the block header will be written with calculated CRC. 
 * once the item is written the block is activated.
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \param[in] item_id  16-bit item Identifier 
 *
 * \param[in]  buf buffer to write the item values in to NVM
 *
 * \param[in] len item buffer length
 *
 * \return Status of the PDS Write procedure
 *
 * \retval PDS_SUCCESS PDS Item id and it's values are written successfully
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_write(pds_env_t *pds_instance, uint16_t item_id, uint8_t *buf, uint16_t len)
{
	pds_status_t status;
	uint8_t lbuf[sizeof(block_header_t)] = {0, };
	block_header_t *block_header_info = (block_header_t *)lbuf;
	block_header_t block_check;	
	uint32_t address, address_max;
	
	/* Validate the given Input data */
	Assert((pds_init_status != false) && (buf != NULL) && (len > 0) \
			&& (item_id != 0) && (pds_instance != NULL));
	
	address_max = (pds_instance->pds_nvm_start_address +  \
					(active_sector * pds_instance->pds_sector_size) + pds_instance->pds_sector_size);
	
	memset((uint8_t *)&block_check, 0xFF, sizeof(block_header_t));
	
	/* Calculate the read start address i.e start of the block address of the item 
	    by scanning the items in the sector  and skip the active sector header */ 
	address = (active_sector * pds_instance->pds_sector_size) + \
				pds_instance->pds_nvm_start_address + sizeof(sector_header_t);
	
	/* Item write address must start and end within the active sector and must not write more than a active sector. */
	while ((address + (sizeof(block_header_t)) + len) < address_max){

		/* Read block header and find the empty sector space to write new item  */
		if((status = pds_nvm_read(pds_instance, address, \
						lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return status;
		}
		
		if(!memcmp((uint8_t *)&block_check, lbuf, sizeof(block_header_t))){
			/* Empty space to store the new item */
			block_check.id = item_id; 
			block_check.item_size = len;
			block_check.status = 0xFF;
			block_check.checksum = pds_calc_crc(buf, len);
						
			/* write the item header with checksum */
			if((status = pds_nvm_write(pds_instance, address, \
							(uint8_t *)&block_check, sizeof(block_header_t))) != PDS_SUCCESS){
				return status;
			}
			
			/* Write PDS Item into empty space */
			if((status = pds_nvm_write(pds_instance, (address + sizeof(block_header_t)),\
							 buf, len)) != PDS_SUCCESS){
				return status;
			}
			
			/* Write the item header with calculated checksum and Active status flag */
			block_check.status &= ~(1 << PDS_BLOCK_ACTIVE_FLAG_Pos);
			if((status = pds_nvm_write(pds_instance, address, \
							(uint8_t *)&block_check, sizeof(block_header_t))) != PDS_SUCCESS){
				return status;
			}
			return PDS_SUCCESS;			
		}else {
			
			if(block_header_info->item_size == 0xFFFF){
				/* No item exists here after and inconsistency in the active sector */
				return PDS_SECTOR_ITEM_ERROR;
			}
			
			/* Check the item id exists */
			if(block_header_info->id == item_id) {
				/* check for item is valid, not marked for deletion and status is valid */
				if ((!(block_header_info->status & (1 << PDS_BLOCK_ACTIVE_FLAG_Pos))) && \
				(block_header_info->status & (1 << PDS_BLOCK_DELETE_FLAG_Pos)))	{
					return PDS_ITEM_EXISTS;
				}
			}	
						
			/* Read Next Block Header */
			address += block_header_info->item_size + sizeof(block_header_t);				
		}
	}
				
	/* return the pds write status */
	return PDS_NVM_NO_MEMORY;
}

/**
 * \brief  Delete PDS item from active sector
 *
 * Scan the Item id from PDS active sector and if the item is valid, update
 * the status deletion flag.
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \param[in] item_id  16-bit item Identifier 
 *
 * \return Status of the PDS Delete procedure
 *
 * \retval PDS_SUCCESS PDS Item id  found and deleted successfully
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_delete(pds_env_t *pds_instance, uint16_t item_id)
{
	pds_status_t status;
	uint8_t lbuf[sizeof(block_header_t)];
	block_header_t *block_header_info;
	uint32_t address, address_max;
	
	/* Validate the given Input data */
	Assert((pds_init_status != false) && (item_id != 0) && (pds_instance != NULL));
	
	address_max = (pds_instance->pds_nvm_start_address +  \
					(active_sector * pds_instance->pds_sector_size) + pds_instance->pds_sector_size);	
	
	/* Calculate the scanning start address i.e start of the block address of the 
	    item by scanning the items in the sector and Skip the active sector header */
	address = pds_instance->pds_nvm_start_address + sizeof(sector_header_t) + \
				(active_sector * pds_instance->pds_sector_size);
	
	/* Scan address must start and end within the active sector and must not read more than a active sector. */
	while ((address + sizeof(block_header_t)) < address_max){
		
		/* Read block header and find the item id */		
		if((status = pds_nvm_read(pds_instance, address, lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return status;
		}
		
		block_header_info = (block_header_t *)lbuf;
		
		if ((block_header_info->id == item_id) && (block_header_info->status & (1 << PDS_BLOCK_DELETE_FLAG_Pos))){
			/* Item matched, mark item for deletion */
			block_header_info->status &= ~(1 << PDS_BLOCK_DELETE_FLAG_Pos);
			if((status = pds_nvm_write(pds_instance, address, lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
				return status;
			}
			return PDS_SUCCESS;
		}else{
			if(block_header_info->item_size == 0xFFFF){
				/* No item exists here after */
				break;
			}
			
			/* Read Next Block Header */
			address += block_header_info->item_size + sizeof(block_header_t);
		}
	}
	
	/* return the pds read status */
	return PDS_ITEM_NOT_FOUND;	
}

/**
 * \brief  List PDS similar Major item ID's from active sector
 *
 * Scan the major (MSB) Item id's from PDS active sector & ignore the minor id's 
 * (Sub ID or LSB of Item ID) and if the item is valid and matched copy the entire 
 * item id's into buffer and update the matched item counts
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \param[in] item_id  16-bit item Identifier 
 *
 * \param[in]  item_id_buf buffer to store the item id that's matched
 *
 * \param[in] len maximum length of the buffer to store the complete item id's
 *
 * \return count return the number of matched major item id's. 
 * If no item id matches then it returns the count 0 or any error.
 *
 */
uint16_t pds_list_item(pds_env_t *pds_instance, uint16_t item_id, uint16_t *item_id_buf, uint16_t len)
{
	pds_status_t status;
	uint16_t count = 0;
	uint32_t address, address_max;
	
	uint8_t lbuf[sizeof(block_header_t)];
	block_header_t *block_header_info;
	
	/* Validate the given Input data */
	Assert((pds_init_status != false) && (item_id_buf != NULL) && (len > 0) && \
			(item_id != 0) && (pds_instance != NULL));
	
	address_max = (pds_instance->pds_nvm_start_address +  \
							(active_sector * pds_instance->pds_sector_size) + pds_instance->pds_sector_size);
	
	/* Calculate the read start address i.e start of the block address of the item 
	    which used to read and scan items and Skip the active sector header */
	address = pds_instance->pds_nvm_start_address + sizeof(sector_header_t) + \
				(active_sector * pds_instance->pds_sector_size);
	
	/* Read access must start and end within the storage and must not read more than a active sector. */
	while ((address + sizeof(block_header_t)) < address_max){		
	
		/* Read block header to find the item id */
		if((status = pds_nvm_read(pds_instance, address, lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return count;
		}

		block_header_info = (block_header_t *)lbuf;
		
		if ((block_header_info->id & 0xFF00) == (item_id & 0xFF00)){
			/* Item matched */
			/* Check the Item is marked  for deletion and it's active */
			if ((PDS_BLOCK_DELETE_MASK(block_header_info->status)) && \
			(!PDS_BLOCK_ACTIVE_MASK(block_header_info->status))){				
				item_id_buf[count++] = block_header_info->id;
				if (count == len){
					return count;
				}
			}			
		}
		
		if(block_header_info->item_size == 0xFFFF){
			/* No item exists here after */
			break;
		}
				
		/* Read Next Block Header */
		address += block_header_info->item_size + sizeof(block_header_t);
	}
	
	/* return the pds read status */
	return count;
}

/**
 * \brief  Check item id exists in the active sector
 *
 * Scan the Item id from PDS active sector and if the item is 
 * valid return the status as item found
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \param[in] item_id  16-bit item Identifier 
 *
 * \return Status of the item existence in the active sector
 *
 * \retval PDS_SUCCESS PDS Item id  found in the PDS
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_item_exist(pds_env_t *pds_instance, uint16_t item_id)
{
	pds_status_t status;
	uint8_t lbuf[sizeof(block_header_t)];
	block_header_t *block_header_info;
	uint32_t address, address_max;
	
	/* Validate the Input data */
	Assert((pds_init_status != false) && (item_id != 0) && (pds_instance != NULL));
	
	address_max = (pds_instance->pds_nvm_start_address +  \
					(active_sector * pds_instance->pds_sector_size) + pds_instance->pds_sector_size);
	
	/* Calculate the read start address i.e start of the block address of the item by 
	    scanning the items in the sector and Skip the active sector header */
	address = pds_instance->pds_nvm_start_address + sizeof(sector_header_t) + \
				(active_sector * pds_instance->pds_sector_size);
	
	/* Read access address must start and end within the storage and must not read more than a active sector. */
	while ((address + sizeof(block_header_t)) < address_max){
		
		/* Read block header and find the item id */	
		if((status = pds_nvm_read(pds_instance, address, lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return status;
		}
		
		block_header_info = (block_header_t *)lbuf;
		
		if (block_header_info->id == item_id){
			/* Item matched */
			/* Check the Item is marked  for deletion and it's active */
			if ((PDS_BLOCK_DELETE_MASK(block_header_info->status)) && \
				(!PDS_BLOCK_ACTIVE_MASK(block_header_info->status))){
				address += sizeof(block_header_t);
				return PDS_SUCCESS;
			}
		}
		
		if(block_header_info->item_size == 0xFFFF){
			/* No item exists here after */
			break;
		}
		
		/* Read Next Block Header */
		address += block_header_info->item_size + sizeof(block_header_t);
	}
	
	/* return the pds read status */
	return PDS_ITEM_NOT_FOUND;
}

/**
 * \brief  Delete all the PDS sector and initialize the sector's again
 *
 * Delete the PDS sectors and erase it's contents. Also initialize the PDS
 * sector as the new device. The sector header number set to lowest valid value.
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \retval PDS_SUCCESS PDS sector's and contents are erased and initialized
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_delete_all(pds_env_t *pds_instance)
{
	pds_status_t status;
	uint8_t sector;
	uint32_t address;
	sector_header_t sec_header;
	
	Assert((pds_init_status != false));
	
	for (sector = 0; sector < pds_instance->pds_sector_count; sector++) {
		address = (pds_instance->pds_nvm_start_address + \
					(sector * pds_instance->pds_sector_size));
					
		if((status = pds_nvm_erase(pds_instance, address, \
						pds_instance->pds_sector_size)) != PDS_SUCCESS){
			return status;
		}
	}
	
	memcpy(sec_header.signature, PDS_SECTOR_SIGNATURE, 6);
	sec_header.number = 0x01; /* Initialize the sector header from beginning */
	sec_header.status = 0x00; /* Set the status as active for new sector */
	
	if((status = pds_nvm_write(pds_instance, pds_instance->pds_nvm_start_address, \
							(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
		return status;
	}
	active_sector = 0x00;
		
	return PDS_SUCCESS;
}

/**
 * \brief  PDS Sector compact operation involves moving of valid item from
 * one sector to another sector (new sector). This requires erase of new sector and writing 
 * new sector header and copying all the valid items into new sector one by one. 
 * Once the old sector items are copied into new sector, then new sector will be activated
 * at the end of the compact operation.
 *
 * \param[in] pds_instance Initialized PDS environment structure, 
 *
 * \retval PDS_SUCCESS  PDS Compact operation successfully completed.
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_compact_sector(pds_env_t *pds_instance)
{
	pds_status_t status = PDS_COMPACT_FAILED;
	uint32_t address, address_max, new_address;
	uint8_t lbuf[sizeof(block_header_t)];
	block_header_t *block_header_info;
	sector_header_t sec_header;
	
	/* Validate the Input data */
	Assert((pds_init_status != false) && (pds_instance != NULL));
	
	/* Calculate the read start address i.e start of the block address of the item by scanning the items in the sector */
	address = (pds_instance->pds_nvm_start_address + \
				((active_sector % pds_instance->pds_sector_count) * pds_instance->pds_sector_size));
	
	address_max = address + pds_instance->pds_sector_size;
	
	new_address = (pds_instance->pds_nvm_start_address + \
					(((active_sector +1) % pds_instance->pds_sector_count) * pds_instance->pds_sector_size));
	
	DBG_LOG("Compacting sector Starts: Active-Now:%d, old address: 0x%X, new_address:0x%X", \
			(unsigned int)active_sector, (unsigned int)address, (unsigned int)new_address);
	
		
	/* Erase the shadow PDS sector(s) */
	if((status = pds_nvm_erase(pds_instance, new_address, \
					pds_instance->pds_sector_size)) != PDS_SUCCESS){
		return status;
	}
	
	
	/* Read the active sector header */
	if((status = pds_nvm_read(pds_instance, address, \
					(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
		return status;
	}
	
	if (!sec_header.status){
		sec_header.number++;
		sec_header.status = PDS_INVALID_SECTOR;
		if((status = pds_nvm_write(pds_instance, new_address, \
						(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
			return status;
		}
	}else{
		/* Previous sector is inactive so the data in the invalid sector is not considered */
		
		/* Initialize the sector header from beginning */
		sec_header.number = 0x01; 
		/* Set the status as active for new sector */
		sec_header.status = 0x00; 
		if((status = pds_nvm_write(pds_instance, new_address, \
						(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
			return status;
		}
		active_sector = ((active_sector + 1) % pds_instance->pds_sector_count);
		return PDS_NEW_SECTOR_INITIALIZED;
	}
	
	address += sizeof(sector_header_t);
	new_address += sizeof(sector_header_t);	
	
	/* Read access address must start and end within the storage and must not read more than a active sector. */	
	while ((address + sizeof(block_header_t)) < address_max){
		
		/* Read block header and find the item id */	
		if((status = pds_nvm_read(pds_instance, address, \
						lbuf, sizeof(block_header_t))) != PDS_SUCCESS){
			return status;
		}
		address += sizeof(block_header_t);
		
		block_header_info = (block_header_t *)lbuf;
		
		/* Item matched */
		/* Check the Item is marked  for deletion and it's active */
		if ((PDS_BLOCK_DELETE_MASK(block_header_info->status)) && \
		(!PDS_BLOCK_ACTIVE_MASK(block_header_info->status))){			
			if((address + block_header_info->item_size) < address_max){
				uint8_t buf[block_header_info->item_size];
					if((status = pds_nvm_read(pds_instance, address, buf, \
									block_header_info->item_size)) != PDS_SUCCESS){
						return status;
					}
					if(block_header_info->checksum == pds_calc_crc(buf, block_header_info->item_size)){					
						/* write into new pds sector */						
						/* copy the block header */
						if((status = pds_nvm_write(pds_instance, new_address, \
										(uint8_t *)block_header_info, sizeof(block_header_t))) != PDS_SUCCESS){
							return status;
						}
						new_address += sizeof(block_header_t);
						/* copy the item data */			
						if((status = pds_nvm_write(pds_instance, new_address, \
										buf, block_header_info->item_size)) != PDS_SUCCESS){
							return status;
						}
						DBG_LOG("Moving Item-ID:0x%X, From:0x%X to 0x%X", \
								block_header_info->id, (unsigned int)address, (unsigned int)new_address);
						new_address += block_header_info->item_size;
						status = PDS_SUCCESS;				
					}
			}
		}
		
		if(block_header_info->item_size == 0xFFFF){
			/* No item exists here after */
			break;			
		}
		
		/* Read Next Block Header */
		address += block_header_info->item_size;
	}
	
	/* Make new PDS sector active */
	active_sector = ((active_sector + 1) % pds_instance->pds_sector_count);
	new_address = (pds_instance->pds_nvm_start_address + (active_sector * pds_instance->pds_sector_size));
	sec_header.status = 0x00;
	
	if((status = pds_nvm_write(pds_instance, new_address, \
					(uint8_t *)&sec_header, sizeof(sector_header_t))) != PDS_SUCCESS){
		return status;
	}
	
	DBG_LOG("Compacting sector Ends: Active-Now:%d", active_sector);
	
	/* return the pds compact status */
	return status;
}

/**
 * \brief Calculate crc of the given buffer
 *
 * \param[in] buf The pointer to uint8_t size buffer to calculate crc
 * \param[in] len The length of the buffer
 *
 * \return crc
 */
uint16_t pds_calc_crc(uint8_t *buf, uint16_t len)
{
	uint16_t crc = 0xFFFF;
	uint16_t idx;
	uint8_t value;
	for (idx = 0; idx < len; idx++) {
		value = (uint8_t)((crc >> 8) ^ buf[idx]);
		value = value ^ (value >> 4);
		crc   = (crc << 8) ^ ((uint16_t)value << 12) ^ ((uint16_t)value << 5) ^ (uint16_t)value;
	}
	return crc;
}

/* end of PDS (pds.c) file */


