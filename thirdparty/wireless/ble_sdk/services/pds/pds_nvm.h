/**
* \file
*
* \brief This file for Persistent Data Storage - Non-Volatile Memory management.
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

#ifndef __PDS_NVM_H__
#define __PDS_NVM_H__

/**< \brief list of PDS Item-ID's */
typedef enum pds_item_id{
	BLE_BONDING_INFO = 0xB0	
}pds_item_id_t;

/**< \brief list of possible PDS status */
typedef enum pds_status{
	/*!< All PDS successful operation status */
	PDS_SUCCESS = 0x00,
	/*!< PDS Initialization failed */
	PDS_INIT_ERROR = 0x01, 
	/*!< PDS NVM Initialization failures */
	PDS_NVM_INIT_ERROR,
	/*!< PDS NVM Memory read error */
	PDS_NVM_READ_ERROR,
	/*!< PDS NVM Memory Write error */
	PDS_NVM_WRITE_ERROR,
	/*!< PDS Invalid Flash ID */
	PDS_INVALID_FLASH_ID,
	/*!< PDS NVM memory delete operation error */
	PDS_NVM_DELETE_ERROR,
	/*!< PDS NVM memory out of it's range */
	PDS_NVM_NO_MEMORY,
	/*!< PDS Item ID Not found */
	PDS_ITEM_NOT_FOUND,
	/*!< PDS Item ID not exists in the active sector */
	PDS_ITEM_EXISTS,
	/*!< PDS Item already deleted */
	PDS_ITEM_DELETED,
	/*!< PDS Item is inactive, might due to power failures etc. */
	PDS_ITEM_INACTIVE,
	/*!< PDS item existing CRC and calculated CRC not matched, data inconsistent */
	PDS_ITEM_CRC_ERROR,
	/*!< PDS item not valid */
	PDS_ITEM_INVALID,
	/*!< PDS item is bigger than it's buffer size */
	PDS_ITEM_BIGGER_SIZE,
	/*!< PDS new sector initialized and no active sector found */
	PDS_NEW_SECTOR_INITIALIZED,
	/*!< PDS active item has error */
	PDS_SECTOR_ITEM_ERROR,
	/*!< PDS Invalid operation */
	PDS_INVALID_OPERATION,
	/*!< PDS compact operation failed */
	PDS_COMPACT_FAILED
}pds_status_t;

/**< \brief PDS Invalid sector identifier */
#define PDS_INVALID_SECTOR			(0xFF)

/**< \brief PDS Internal flash ID */
#define PDS_INTERNAL_FLASH_ID		(0x01)
#define PDS_EXTERNAL_FLASH_ID		(0x02)
#define PDS_EXTERNAL_EEPROM_ID		(0x03)
#define PDS_INTERNAL_EEPROM_ID		(0x04)

/**< \brief PDS Identifier */
#define PDS_ID_SUBID(pds_id)			   (0x00FF & pds_id)
#define PDS_ID_MAINID(pds_id)			   (0x00FF & (pds_id >> 8))
#define PDS_ID(main_id, sub_id)			   ((main_id << 8) | sub_id)

/**< \brief PDS_BLOCK_ACTIVE_FLAG_Pos Flag */
#define PDS_BLOCK_ACTIVE_FLAG_Pos          7 
#define PDS_BLOCK_ACTIVE_MASK(value)       ((0x1ul << PDS_BLOCK_ACTIVE_FLAG_Pos) & value)

/**< \brief PDS_BLOCK_DELETE_FLAG_Pos Flag */
#define PDS_BLOCK_DELETE_FLAG_Pos          5 
#define PDS_BLOCK_DELETE_MASK(value)       ((0x1ul << PDS_BLOCK_DELETE_FLAG_Pos) & value)

 /**< FLASH base address */
#define NVM_MEMORY							((volatile uint16_t *)FLASH_ADDR)

COMPILER_PACK_SET(1)  /**< \brief Start of compiler pack */
/**
 * \internal Item structure for faster search of item
 */
typedef struct pds_item_type {
	/*!< Id of an item. */
	uint16_t item_id;
	/*!< Location of the this item. */
	uint32_t item_address;
}pds_item_type_t;
/**
* \Structure containing environment parameters of the NVM module.
*/
typedef struct pds_env {
	/*!<Id of the flash Internal/External */
	uint32_t flash_id;
	/*Starting location of NVM */
	uint32_t pds_nvm_start_address;
	/*!<Number of pds sector */
	uint32_t pds_sector_count;	
	/*!<pds sector size */
	uint32_t pds_sector_size;
}pds_env_t;

/**
 * \brief Block header, located before each block of item data.
 */
typedef struct block_header {
	/*!< Status flag. Active sector has to set 8th bit to zero. 
	     setting 7th bit to zero will enable write lock for this item*/
	uint8_t status;
	/*!< Id of stored item. MSB represent item type (Pairing,Device info etc)\
	LSB represent item sequence number, wrap around to lowest available number */
	uint16_t id;
	/*!< The size of the item. the block header size excluded */
	uint16_t item_size;
	/*!< Checksum of the header excluding "status" field. */
	uint16_t checksum;
}block_header_t;


/**
 * \brief Sector header, located at sector start.
 */
typedef struct sector_header {
	/*!< status flag. Active sector has it set to zero.*/ 
	uint8_t status;
	/*!< Number of the sector. Sector with largest number is used. */
	uint32_t number;
	/*!< Sector signature, to prevent occasional sector recognition as active. */
	uint8_t signature[6];
}sector_header_t;

COMPILER_PACK_RESET() /*!< End of compiler pack */

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
pds_status_t pds_nvm_init(pds_env_t *pds_instance);

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
pds_status_t pds_nvm_write(pds_env_t *pds_instance, uint32_t mem_address, uint8_t *buffer,
								uint16_t len);
								
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
pds_status_t pds_nvm_read(pds_env_t *pds_instance, uint32_t mem_address, uint8_t *buffer,
								uint16_t len);
								
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
pds_status_t pds_nvm_erase(pds_env_t *pds_instance, uint32_t mem_address, uint16_t len);

#endif /* __PDS_NVM_H__ */

