/**
 * \file
 *
 * \brief OTAU Flash Manager declarations
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 *Support</a>
 */



// =======================
#ifndef __OTAU_FLASH_MGR_H__
#define __OTAU_FLASH_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
*							        Macro                                               *
****************************************************************************************/
#define TOTAL_META_DATA_SECTIONS	(2)

typedef struct image_meta_data_status
{
	uint32_t current_image_data_addr;
	uint32_t otau_image_data_addr;
}image_meta_data_status_t;

/****************************************************************************************
*							        Prototypes                                          *
****************************************************************************************/

/** @brief crc32_compute calculate a checksum for buffer, buffer length
 *
 *	@param[in] buf input buffer for CRC calculation
 *	@param[in] len length of the buffer
 *	@param[in] resume compute the crc from previous results
 *	@param[in] crc previously computed crc value
 *
 *  @return	calculated crc32 for given data and len
 */
image_crc_t crc32_compute(uint8_t *buf, uint32_t len, bool resume, image_crc_t crc);

/** @brief ofm_init OTAU flash driver initialization
 *			and gets the information about the flash memory
 *
 *	@param[in] params of type ofid_flash_info_t
 *
 *  @return	AT_BLE_SUCCESS flash info read successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_init(void *params);

/** @brief ofm_get_device_info OTAU get device information
 *			and gets the information about the firmware version and device
 *
 *	@param[in] device_info device information will be read from metadata
 *
 *  @return	AT_BLE_SUCCESS device info read successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_get_device_info(device_info_t *device_info);

/** @brief ofm_otau_meta_data_update updates the meta data with given meta data.
 *			Before writing into the flash the CRC will be calculated and appended
 *
 *	@param[in] params of type image_meta_data_t
 *
 *  @return	returns AT_BLE_SUCCESS when meta data is successfully written into the
 *					flash memory or returns  AT_BLE_FAILURE where failure to update the meta
 *					data
 */
at_ble_status_t ofm_otau_meta_data_update(void *params);

/** @brief ofm_read_meta_data reads meta data and validate the CRC of the meta
 *			data.
 *
 *	@param[in] params of type image_meta_data_t *
 *	@param[in] flash_id of type flash_id_t
 *
 *  @return	If the CRC validation of meta data is correct then AT_BLE_SUCCESS
 *			will be returned. If CRC is invalid then AT_BLE_FAILURE will be returned
 */
at_ble_status_t ofm_read_meta_data(void *params, flash_id_t flash_id);

/** @brief ofm_write_page Function will call the flash api's to write the 
 *			page in case of the input parameters are correct 
 *
 *	@param[in] section_id section id of the flash memory needs to be updated
 *	@param[in] addr starting address of the page
 *	@param[in] buf pointer to the buffer memory that needs to be copied into flash memory
 *	@param[in] len length of the page data needs to be updated
 *
 *  @return	AT_BLE_SUCCESS in case of the page is written successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_write_page(section_id_t section_id, uint32_t addr, uint8_t *buf, uint32_t len);

/** @brief ofm_read_page Function will call the flash api's to read the 
 *			page in case of the input parameters are correct 
 *
 *	@param[in] section_id section id of the flash memory needs to be read
 *	@param[in] addr starting address of the page
 *	@param[in] buf pointer to the buffer memory that needs to be copied from flash memory
 *	@param[in] len length of the page data needs to be read
 *
 *  @return	AT_BLE_SUCCESS in case of the page is read successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_read_page(section_id_t section_id, uint32_t addr, uint8_t *buf, uint32_t len);

/** @brief ofm_erase_section_image OTAU image section will be erased based on the section identifier
 *			sections will be erased at the beginning of the OTAU process.
 *
 *	@param[in] section_id section id of the flash memory needs to be erased
 *
 *  @return	AT_BLE_SUCCESS in case of the section erased successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_erase_section_image(section_id_t section_id);

/** @brief ofm_restart_device Restarts the device
 *
 *	@param[in] None
 *
 *  @return	None
 */
void ofm_restart_device(void);

at_ble_status_t ofm_erase_memory(uint32_t addr, uint32_t size);

#ifdef OTAU_FLASH_TEST
at_ble_status_t ofm_flash_erase_nwrite_test(bool page_write, bool sector_write);
void ofm_flash_test(void);
#endif /* OTAU_FLASH_TEST */

#ifdef __cplusplus
}
#endif

#endif /*__OTAU_FLASH_MGR_H__ */

