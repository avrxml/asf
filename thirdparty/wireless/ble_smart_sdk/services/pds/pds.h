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


#ifndef __PDS_H__
#define __PDS_H__

#include "pds_nvm.h"

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
pds_status_t pds_init(pds_env_t *pds_instance);

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
 * \return Status of the PDS Read procedure
 *
 * \retval PDS_SUCCESS PDS Item id found and item read success
 * 
 * \retval  for other return values, check pds_status_t enum values
 */
pds_status_t pds_read(pds_env_t *pds_instance, uint16_t item_id, uint8_t *buf, uint16_t buf_len, uint16_t *read_size);

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
pds_status_t pds_write(pds_env_t *pds_instance, uint16_t item_id, uint8_t *buf, uint16_t len);

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
pds_status_t pds_delete(pds_env_t *pds_instance, uint16_t item_id);

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
uint16_t pds_list_item(pds_env_t *pds_instance, uint16_t item_id, uint16_t *item_id_buf, uint16_t len);

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
pds_status_t pds_item_exist(pds_env_t *pds_instance, uint16_t item_id);

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
pds_status_t pds_delete_all(pds_env_t *pds_instance);

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
pds_status_t pds_compact_sector(pds_env_t *pds_instance);

/**
 * \brief Calculate crc of the given buffer
 *
 * \param[in] buf The pointer to uint8_t size buffer to calculate crc
 * \param[in] len The length of the buffer
 *
 * \return crc
 */
uint16_t pds_calc_crc(uint8_t *buf, uint16_t len);

#endif /* __PDS_H__ */

