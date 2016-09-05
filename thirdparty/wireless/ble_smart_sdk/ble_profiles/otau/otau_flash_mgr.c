/**
 * \file
 *
 * \brief OTAU Flash Manager
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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

/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

/****************************************************************************************
*							        Includes	                                        *
*                                                                                       *
****************************************************************************************/
#include <asf.h>
#include <string.h>
#include "ble_utils.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "otau_service.h"
#include "otau_profile.h"
#include "otau_flash_mgr.h"
#include "ofid.h"
#include "platform.h"


/****************************************************************************************
*							        Globals								                *
*																	                    *
****************************************************************************************/
ofid_flash_info_t *flash_inst = NULL;

image_meta_data_status_t meta_loc =
{
	.current_image_data_addr = 0xFFFFFFFF,
	.otau_image_data_addr = 0xFFFFFFFF
};

/** @brief swap_meta_data_locations swaps the meta data location. i.e current image 
 *			meta data location and otau image meta data location
 *
 *	@param[in] None
 *
 *  @return	None
 */
static void swap_meta_data_locations(void);

/** @brief compute_image_meta_loc This function will locate the correct metadata address 
 *			device will reads the meta data and compute the meta data location 
 *			of OTAU and current running image meta data location
 *
 *	@param[in] params unused
 *
 *  @return	AT_BLE_FAILURE in case of failure
 */
static at_ble_status_t compute_image_meta_loc(void *params);

/** @brief check_nupdate_metadata_address during the device restarts and OTAU initialization 
 *			the device needs to know the meta data address, In case for some reason the metadata
 *			is erased, the device needs to be pointed to default memory locations of metadata
 *
 *	@param[in] None
 *
 *  @return	None
 */
static void check_nupdate_metadata_address(void);

const uint32_t crc32_lookup[256] =
{
0x00000000,0x4C11DB7,0x9823B6E,0xD4326D9,
0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,
0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD,
0x4C11DB70,0x48D0C6C7,0x4593E01E,0x4152FDA9,
0x5F15ADAC,0x5BD4B01B,0x569796C2,0x52568B75,
0x6A1936C8,0x6ED82B7F,0x639B0DA6,0x675A1011,
0x791D4014,0x7DDC5DA3,0x709F7B7A,0x745E66CD,
0x9823B6E0,0x9CE2AB57,0x91A18D8E,0x95609039,
0x8B27C03C,0x8FE6DD8B,0x82A5FB52,0x8664E6E5,
0xBE2B5B58,0xBAEA46EF,0xB7A96036,0xB3687D81,
0xAD2F2D84,0xA9EE3033,0xA4AD16EA,0xA06C0B5D,
0xD4326D90,0xD0F37027,0xDDB056FE,0xD9714B49,
0xC7361B4C,0xC3F706FB,0xCEB42022,0xCA753D95,
0xF23A8028,0xF6FB9D9F,0xFBB8BB46,0xFF79A6F1,
0xE13EF6F4,0xE5FFEB43,0xE8BCCD9A,0xEC7DD02D,
0x34867077,0x30476DC0,0x3D044B19,0x39C556AE,
0x278206AB,0x23431B1C,0x2E003DC5,0x2AC12072,
0x128E9DCF,0x164F8078,0x1B0CA6A1,0x1FCDBB16,
0x18AEB13,0x54BF6A4,0x808D07D,0xCC9CDCA,
0x7897AB07,0x7C56B6B0,0x71159069,0x75D48DDE,
0x6B93DDDB,0x6F52C06C,0x6211E6B5,0x66D0FB02,
0x5E9F46BF,0x5A5E5B08,0x571D7DD1,0x53DC6066,
0x4D9B3063,0x495A2DD4,0x44190B0D,0x40D816BA,
0xACA5C697,0xA864DB20,0xA527FDF9,0xA1E6E04E,
0xBFA1B04B,0xBB60ADFC,0xB6238B25,0xB2E29692,
0x8AAD2B2F,0x8E6C3698,0x832F1041,0x87EE0DF6,
0x99A95DF3,0x9D684044,0x902B669D,0x94EA7B2A,
0xE0B41DE7,0xE4750050,0xE9362689,0xEDF73B3E,
0xF3B06B3B,0xF771768C,0xFA325055,0xFEF34DE2,
0xC6BCF05F,0xC27DEDE8,0xCF3ECB31,0xCBFFD686,
0xD5B88683,0xD1799B34,0xDC3ABDED,0xD8FBA05A,
0x690CE0EE,0x6DCDFD59,0x608EDB80,0x644FC637,
0x7A089632,0x7EC98B85,0x738AAD5C,0x774BB0EB,
0x4F040D56,0x4BC510E1,0x46863638,0x42472B8F,
0x5C007B8A,0x58C1663D,0x558240E4,0x51435D53,
0x251D3B9E,0x21DC2629,0x2C9F00F0,0x285E1D47,
0x36194D42,0x32D850F5,0x3F9B762C,0x3B5A6B9B,
0x315D626,0x7D4CB91,0xA97ED48,0xE56F0FF,
0x1011A0FA,0x14D0BD4D,0x19939B94,0x1D528623,
0xF12F560E,0xF5EE4BB9,0xF8AD6D60,0xFC6C70D7,
0xE22B20D2,0xE6EA3D65,0xEBA91BBC,0xEF68060B,
0xD727BBB6,0xD3E6A601,0xDEA580D8,0xDA649D6F,
0xC423CD6A,0xC0E2D0DD,0xCDA1F604,0xC960EBB3,
0xBD3E8D7E,0xB9FF90C9,0xB4BCB610,0xB07DABA7,
0xAE3AFBA2,0xAAFBE615,0xA7B8C0CC,0xA379DD7B,
0x9B3660C6,0x9FF77D71,0x92B45BA8,0x9675461F,
0x8832161A,0x8CF30BAD,0x81B02D74,0x857130C3,
0x5D8A9099,0x594B8D2E,0x5408ABF7,0x50C9B640,
0x4E8EE645,0x4A4FFBF2,0x470CDD2B,0x43CDC09C,
0x7B827D21,0x7F436096,0x7200464F,0x76C15BF8,
0x68860BFD,0x6C47164A,0x61043093,0x65C52D24,
0x119B4BE9,0x155A565E,0x18197087,0x1CD86D30,
0x29F3D35,0x65E2082,0xB1D065B,0xFDC1BEC,
0x3793A651,0x3352BBE6,0x3E119D3F,0x3AD08088,
0x2497D08D,0x2056CD3A,0x2D15EBE3,0x29D4F654,
0xC5A92679,0xC1683BCE,0xCC2B1D17,0xC8EA00A0,
0xD6AD50A5,0xD26C4D12,0xDF2F6BCB,0xDBEE767C,
0xE3A1CBC1,0xE760D676,0xEA23F0AF,0xEEE2ED18,
0xF0A5BD1D,0xF464A0AA,0xF9278673,0xFDE69BC4,
0x89B8FD09,0x8D79E0BE,0x803AC667,0x84FBDBD0,
0x9ABC8BD5,0x9E7D9662,0x933EB0BB,0x97FFAD0C,
0xAFB010B1,0xAB710D06,0xA6322BDF,0xA2F33668,
0xBCB4666D,0xB8757BDA,0xB5365D03,0xB1F740B4
};


/****************************************************************************************
*							        Implementations						                *
*                                       												*
****************************************************************************************/

/** @brief swap_meta_data_locations swaps the meta data location. i.e current image 
 *			meta data location and otau image meta data location
 *
 *	@param[in] None
 *
 *  @return	None
 */
static void swap_meta_data_locations(void)
{
	uint32_t addr;
	addr = meta_loc.current_image_data_addr;
	meta_loc.current_image_data_addr = meta_loc.otau_image_data_addr;
	meta_loc.otau_image_data_addr = addr;
}

/** @brief ofm_read_meta_data reads meta data and validate the CRC of the meta
 *			data.
 *
 *	@param[in] params of type image_meta_data_t *
 *	@param[in] flash_id of type flash_id_t
 *
 *  @return	If the CRC validation of meta data is correct then AT_BLE_SUCCESS
 *			will be returned. If CRC is invalid then AT_BLE_FAILURE will be returned
 */
at_ble_status_t ofm_read_meta_data(void *params, flash_id_t flash_id)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	ofid_data_info_t ofid_info;	
	image_meta_data_t *image_meta_info = NULL;
	image_meta_info = (image_meta_data_t *)params;
	uint8_t *meta_buf = (uint8_t *)image_meta_info;
	
	ofid_info.data_buf = meta_buf;
	if (flash_id == OTAU_IMAGE_META_DATA_ID)
	{
		ofid_info.flash_addr = meta_loc.otau_image_data_addr;	
	}
	else if (flash_id == IMAGE_META_DATA_ID)
	{
		ofid_info.flash_addr = meta_loc.current_image_data_addr;
	}
	
	ofid_info.flash_info = flash_inst;
	ofid_info.section_id = 0xFF;
	ofid_info.size = sizeof(image_meta_data_t);
	
	if (ofid_read_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
	{
		/* Validate the CRC for meta data */
		image_crc_t crc = 0;
		
		/* Check whether valid meta data present */
		if (image_meta_info->len == (sizeof(image_meta_data_t) - sizeof(image_meta_info->len)))
		{
			crc = crc32_compute(meta_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);
			
			/* validate CRC */
			if (crc == image_meta_info->header_crc)
			{
				/* CRC is matched and correct */
				status = AT_BLE_SUCCESS;
			}
			else
			{
				/* CRC is incorrect */
				DBG_OTAU("Meta data CRC is incorrect at Address:0x%X", ofid_info.flash_addr);
			}
		}
		else
		{
			DBG_OTAU("Invalid meta data length at Address:0x%X", ofid_info.flash_addr);
			
			uint8_t page_buf[flash_inst->page_size];
			uint32_t backup_addr = FLASH_SECTOR_SIZE;
			
			/* Restore the meta data from backup location */			
			if (meta_loc.otau_image_data_addr < meta_loc.current_image_data_addr)
			{
				/* Copy two meta data pages from backup location */
				backup_addr += meta_loc.otau_image_data_addr;
			}
			else
			{
				backup_addr += meta_loc.current_image_data_addr;
			}
			
			if(ofm_read_page(0x01, backup_addr, page_buf, flash_inst->page_size) == AT_BLE_SUCCESS)
			{
				if(ofm_write_page(0x01, (backup_addr-FLASH_SECTOR_SIZE), page_buf, flash_inst->page_size) != AT_BLE_SUCCESS)
				{
					DBG_OTAU("Meta data restore failed");
				}
			}
			
			if(ofm_read_page(0x01, (backup_addr + flash_inst->page_size), page_buf, flash_inst->page_size) == AT_BLE_SUCCESS)
			{
				if(ofm_write_page(0x01, (backup_addr-FLASH_SECTOR_SIZE + flash_inst->page_size), page_buf, flash_inst->page_size) != AT_BLE_SUCCESS)
				{
					DBG_OTAU("Meta data restore failed");
				}
			}
			
			if (ofid_read_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
			{
				/* Validate the CRC for meta data */
				crc = 0;
				
				/* Check whether valid meta data present */
				if (image_meta_info->len == (sizeof(image_meta_data_t) - sizeof(image_meta_info->len)))
				{
					crc = crc32_compute(meta_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);
					
					/* validate CRC */
					if (crc == image_meta_info->header_crc)
					{
						/* CRC is matched and correct */
						status = AT_BLE_SUCCESS;
					}
					else
					{
						/* CRC is incorrect */
						DBG_OTAU("Meta data CRC is incorrect at Address:0x%X", ofid_info.flash_addr);
					}
				}
				else
				{
					DBG_OTAU("Backup Failure: Invalid meta data Length");
				}
			}
			else
			{
				DBG_OTAU("Backup Failure: Not able to read meta data");
			}
		}
		
	}
	else
	{
		DBG_OTAU("Meta data read failed at Address:0x%X", ofid_info.flash_addr);
	}
	
	return status;
}

/** @brief ofm_otau_meta_data_update updates the meta data with given meta data.
 *			Before writing into the flash the CRC will be calculated and appended
 *
 *	@param[in] params of type image_meta_data_t
 *
 *  @return	returns AT_BLE_SUCCESS when meta data is successfully written into the
 *					flash memory or returns  AT_BLE_FAILURE where failure to update the meta
 *					data
 */
at_ble_status_t ofm_otau_meta_data_update(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_meta_data_t *image_meta_info = NULL;
	image_meta_info = (image_meta_data_t *)params;
	uint8_t *meta_buf = (uint8_t *)image_meta_info;
	image_crc_t crc;
	image_meta_data_t current_meta_data;	
	ofid_data_info_t ofid_info;
	
	image_meta_info->len = sizeof(image_meta_data_t) - sizeof(image_meta_info->len);
	image_meta_info->flashid = OTAU_IMAGE_META_DATA_ID;
	
	crc = crc32_compute(meta_buf, (image_meta_info->len-2), false, 0);
	
	DBG_OTAU("meta data len:%d", image_meta_info->len);
	
	image_meta_info->header_crc = crc;
	
	ofid_info.flash_info = flash_inst;
	ofid_info.section_id = 0xFF;
	ofid_info.size = sizeof(image_meta_data_t);
	ofid_info.data_buf = (uint8_t *)&current_meta_data;
	ofid_info.flash_addr = meta_loc.current_image_data_addr;
	if (ofid_read_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
	{	
		/* First Update it in backup sector */
		/* Erase sector */
		ofid_info.flash_addr = meta_loc.current_image_data_addr + FLASH_SECTOR_SIZE;
		DBG_OTAU("Backup Meta Update Address: 0x%6X", ofid_info.flash_addr);
		if(ofid_erase_metadata(&ofid_info) == AT_OFID_OP_SUCESS)
		{
			/* Write the Current image meta data */
			if (ofid_write_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
			{
				/* Write OTAU image meta data */
				ofid_info.data_buf = meta_buf;
				ofid_info.flash_addr = meta_loc.otau_image_data_addr + FLASH_SECTOR_SIZE;
				DBG_OTAU("Backup Meta Update Address: 0x%6X", ofid_info.flash_addr);
				if(ofid_write_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
				{
					status = AT_BLE_SUCCESS;
				}
			}
		}
				
		/* Erase sector */
		ofid_info.data_buf = (uint8_t *)&current_meta_data;
		ofid_info.flash_addr = meta_loc.current_image_data_addr;
		DBG_OTAU("Real Meta Update Address: 0x%6X", ofid_info.flash_addr);
		if(ofid_erase_metadata(&ofid_info) == AT_OFID_OP_SUCESS)
		{
			if (ofid_write_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
			{
				ofid_info.data_buf = meta_buf;
				ofid_info.flash_addr = meta_loc.otau_image_data_addr;
				DBG_OTAU("Real Meta Update Address: 0x%6X", ofid_info.flash_addr);
				if(ofid_write_metadeta(&ofid_info) == AT_OFID_OP_SUCESS)
				{
					status = AT_BLE_SUCCESS;
				}
			}
		}
	}
	return status;
}

/** @brief check_nupdate_metadata_address during the device restarts and OTAU initialization 
 *			the device needs to know the meta data address, In case for some reason the metadata
 *			is erased, the device needs to be pointed to default memory locations of metadata
 *
 *	@param[in] None
 *
 *  @return	None
 */
static void check_nupdate_metadata_address(void)
{
	if (meta_loc.current_image_data_addr == 0xFFFFFFFF)
	{
		if (meta_loc.otau_image_data_addr == flash_inst->metadata_addr)
		{
			meta_loc.current_image_data_addr = (flash_inst->metadata_addr + flash_inst->page_size);
		}
		else
		{
			meta_loc.current_image_data_addr = flash_inst->metadata_addr;
		}		
	}
	if (meta_loc.otau_image_data_addr == 0xFFFFFFFF)
	{		
		if (meta_loc.current_image_data_addr == flash_inst->metadata_addr)
		{
			meta_loc.otau_image_data_addr = (flash_inst->metadata_addr + flash_inst->page_size);
		}
		else
		{
			meta_loc.otau_image_data_addr = flash_inst->metadata_addr;
		}
	}
}

static void ofm_restore_metadata(void)
{
	/* Erase the actual image section */
	if(ofm_erase_memory(flash_inst->metadata_addr, FLASH_SECTOR_SIZE) == AT_BLE_SUCCESS)
	{
		/* Read the page by page from backup location and store it */
		uint32_t b_idx;
		uint8_t page_buf[flash_inst->page_size];
		
		for (b_idx = 0; b_idx < FLASH_SECTOR_SIZE; b_idx = b_idx + flash_inst->page_size)
		{
			if(ofm_read_page(0x01, (flash_inst->metadata_addr + FLASH_SECTOR_SIZE + b_idx), page_buf, flash_inst->page_size) == AT_BLE_SUCCESS)
			{
				if(ofm_write_page(0x01, (flash_inst->metadata_addr + b_idx), page_buf, flash_inst->page_size) == AT_BLE_SUCCESS)
				{
					DBG_OTAU("Restoring the metadata From:%d To:%d", 
							(flash_inst->metadata_addr + FLASH_SECTOR_SIZE + b_idx),
							(flash_inst->metadata_addr + b_idx));								
				}
			}
		}					
	}
}

/** @brief compute_image_meta_loc This function will locate the correct metadata address 
 *			device will reads the meta data and compute the meta data location 
 *			of OTAU and current running image meta data location
 *
 *	@param[in] params unused
 *
 *  @return	AT_BLE_FAILURE in case of failure
 */
static at_ble_status_t compute_image_meta_loc(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	uint8_t read_buf[sizeof(image_meta_data_t)];
	int32_t idx;
	image_meta_data_t *meta_data_info = (image_meta_data_t *)read_buf;
	firmware_version_t fw_ver[TOTAL_META_DATA_SECTIONS];
	ofid_data_info_t meta_data;	
	uint8_t completed_download_meta_data[TOTAL_META_DATA_SECTIONS] = {0, 0};
	uint8_t meta_data_total_section[TOTAL_META_DATA_SECTIONS] = {0, 0};
	
	memset((uint8_t *)&fw_ver[0], 0xFF, sizeof(firmware_version_t));
	memset((uint8_t *)&fw_ver[1], 0xFF, sizeof(firmware_version_t));
	memset((uint8_t *)&meta_loc, 0xFF, sizeof(image_meta_data_status_t));
	
	for (idx = 0; idx < TOTAL_META_DATA_SECTIONS; idx++)
	{
		/* Check whether valid meta data present */
		/* validate CRC */
		meta_data.data_buf = read_buf;
		meta_data.flash_addr = (flash_inst->metadata_addr + (flash_inst->page_size * idx));
		meta_data.section_id = 0xFF;
		meta_data.size = sizeof(image_meta_data_t);
		meta_data.flash_info = flash_inst;
		
		if(ofid_read_metadeta(&meta_data) == AT_OFID_OP_SUCESS)
		{
			image_crc_t crc = 0;
			
		 /* Check whether valid meta data present */
		 if ( meta_data_info->len == (sizeof(image_meta_data_t) - sizeof(meta_data_info->len)) )
		 {
			 crc = crc32_compute(read_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);		 
			 			 
			/* validate CRC */
			if (crc == meta_data_info->header_crc)
			{
				memcpy((uint8_t *)&fw_ver[idx], (uint8_t *)&meta_data_info->dev_info.fw_version, sizeof(firmware_version_t));
				meta_data_total_section[idx] = meta_data_info->dev_info.total_sections;
				uint32_t idxs;				
				for (idxs = 0; idxs < meta_data_info->dev_info.total_sections; idxs++)
				{
					uint32_t section_image_size = 0;
					uint32_t downloaded_section_image_size = 0;			
					
					/* Patch section present in the image */
					if(meta_data_info->patch_section.section_id == (idxs+1))
					{
						memcpy((uint8_t *)&section_image_size, meta_data_info->patch_section.size, 3);
						memcpy((uint8_t *)&downloaded_section_image_size, meta_data_info->patch_downloaded_info.size, 3);
						if ((section_image_size) && (section_image_size == downloaded_section_image_size))
						{
							DBG_OTAU("Patch T:%d, S:%d", idx, section_image_size);
							completed_download_meta_data[idx]++;
						}
						else
						{
							DBG_OTAU("Patch F:%d S:%d, D:%d", idx, section_image_size, downloaded_section_image_size);
							break;
						}
					}
					
					/* Application header section present in the image */
					if(meta_data_info->app_header_section.section_id == (idxs+1))
					{
						memcpy((uint8_t *)&section_image_size, meta_data_info->app_header_section.size, 3);
						memcpy((uint8_t *)&downloaded_section_image_size, meta_data_info->app_hdr_downloaded_info.size, 3);
						if ((section_image_size) && (section_image_size == downloaded_section_image_size))
						{
							DBG_OTAU("Patch Header T:%d, S:%d", idx, section_image_size);
							completed_download_meta_data[idx]++;
						}
						else
						{
							DBG_OTAU("Patch Header F:%d S:%d, D:%d", idx, section_image_size, downloaded_section_image_size);
							break;
						}
					}
					
					/* Application section present in the image */
					if(meta_data_info->app_section.section_id == (idxs+1))
					{
						memcpy((uint8_t *)&section_image_size, meta_data_info->app_section.size, 3);
						memcpy((uint8_t *)&downloaded_section_image_size, meta_data_info->app_downloaded_info.size, 3);
						/* Check the for the size zero and compare the downloaded size */
						if ((section_image_size) && (section_image_size == downloaded_section_image_size))
						{
							DBG_OTAU("App T:%d, S:%d", idx, section_image_size);
							completed_download_meta_data[idx]++;
						}
						else
						{
							DBG_OTAU("App F:%d S:%d, D:%d", idx, section_image_size, downloaded_section_image_size);
							break;
						}
					}					
				}			

				if (idx == 0)
				{
					DBG_OTAU("Meta Firmware version-[idx-%d]: %d.%d,%d", idx,
					meta_data_info->dev_info.fw_version.major_number,
					meta_data_info->dev_info.fw_version.minor_number,
					meta_data_info->dev_info.fw_version.build_number);
					DBG_OTAU("Section Id 0x%8X, len->%d",  meta_data_info->section_image_id, meta_data_info->len);
					
					meta_loc.current_image_data_addr = meta_data.flash_addr;
				}
				else if(idx == 1)
				{
					DBG_OTAU("Meta Firmware version-[idx-%d]: %d.%d,%d", idx,
					meta_data_info->dev_info.fw_version.major_number,
					meta_data_info->dev_info.fw_version.minor_number,
					meta_data_info->dev_info.fw_version.build_number);
					DBG_OTAU("Section Id 0x%8X, len->%d",  meta_data_info->section_image_id, meta_data_info->len);					
					meta_loc.otau_image_data_addr = meta_data.flash_addr;					
				}				
			 }
			 else
			 {
				 DBG_OTAU("meta data header crc calculation failed: %ld, meta_data_info->header_crc:0x%4X, calc crc:0x%4X", idx, meta_data_info->header_crc, crc);

				/* It must be  present in the backup location of metadata restore the metadata to main metadata location */
				
				ofm_restore_metadata();
				idx--;
				
			 }
		   }
		   else
		   {
			   DBG_OTAU("meta data not found: %ld", idx);
			   ofm_restore_metadata();
			   idx--;
		   }
		}			
	}
	
	/* Get the current running image */
	if ((completed_download_meta_data[0] == meta_data_total_section[0]) && (completed_download_meta_data[1]  == meta_data_total_section[1]))
	{
		if(fw_ver[0].major_number < fw_ver[1].major_number)
		{
			/* Running image at lower part meta data */
			/* Swap first and second part */
			DBG_OTAU("Swap @Device Fw major");
			swap_meta_data_locations();
		}
		else if(fw_ver[0].minor_number < fw_ver[1].minor_number)
		{
			DBG_OTAU("Swap @Device Fw minor");
			swap_meta_data_locations();	
		}
		else if (fw_ver[0].build_number < fw_ver[1].build_number)
		{
			DBG_OTAU("Swap @Device Fw build");
			swap_meta_data_locations();	
		}
	}
	else if (completed_download_meta_data[1] == meta_data_total_section[1])
	{
		DBG_OTAU("Swap @completed meta data idx1");
		swap_meta_data_locations();
	}
	
	check_nupdate_metadata_address();
	
	/* Update the section id for OTAU image in case of factory image */
	meta_data.data_buf = read_buf;
	meta_data.flash_addr = meta_loc.current_image_data_addr;
	meta_data.section_id = 0xFF;
	meta_data.size = sizeof(image_meta_data_t);
	meta_data.flash_info = flash_inst;
	
	if(ofid_read_metadeta(&meta_data) == AT_OFID_OP_SUCESS)
	{
		image_crc_t crc = 0;
		
		/* Check whether valid meta data present */
		if ( meta_data_info->len == (sizeof(image_meta_data_t) - sizeof(meta_data_info->len)) )
		{
			crc = crc32_compute(read_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);
			
			/* validate CRC */
			if (crc == meta_data_info->header_crc)
			{
				
				OTAU_LOG("Device Firmware version: %d.%d.%d", 
					meta_data_info->dev_info.fw_version.major_number,
					meta_data_info->dev_info.fw_version.minor_number,
					meta_data_info->dev_info.fw_version.build_number);
				
				if(meta_data_info->section_image_id == 0x00000000)
				{
					DBG_OTAU("Current Image resides at Upper part of section");
					/* Update the OTAU section image id */
					meta_data.data_buf = read_buf;
					meta_data.flash_addr = meta_loc.otau_image_data_addr;
					meta_data.section_id = 0xFF;
					meta_data.size = sizeof(image_meta_data_t);
					meta_data.flash_info = flash_inst;
					if(ofid_read_metadeta(&meta_data) == AT_OFID_OP_SUCESS)
					{
						/* Check whether valid meta data present */
						if ( meta_data_info->len == (sizeof(image_meta_data_t) - sizeof(meta_data_info->len)) )
						{
							crc = crc32_compute(read_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);
							
							/* validate CRC */
							if (crc == meta_data_info->header_crc)
							{
								DBG_OTAU("OTAU image download at Lower part of section");
								meta_data_info->section_image_id = 0xFFFFF111;
								/* Update the meta data */
								ofm_otau_meta_data_update(read_buf);
							}
						}
					}
				}
				else if(meta_data_info->section_image_id == 0xFFFFF111)
				{
					DBG_OTAU("Current Image resides at Lower part of section");
					/* Update the OTAU section image id */
					meta_data.data_buf = read_buf;
					meta_data.flash_addr = meta_loc.otau_image_data_addr;
					meta_data.section_id = 0xFF;
					meta_data.size = sizeof(image_meta_data_t);
					meta_data.flash_info = flash_inst;
					if(ofid_read_metadeta(&meta_data) == AT_OFID_OP_SUCESS)
					{
						/* Check whether valid meta data present */
						if ( meta_data_info->len == (sizeof(image_meta_data_t) - sizeof(meta_data_info->len)) )
						{
							crc = crc32_compute(read_buf, (sizeof(image_meta_data_t) - sizeof(image_crc_t)), false, 0);
							
							/* validate CRC */
							if (crc == meta_data_info->header_crc)
							{
								DBG_OTAU("OTAU image download at Upper part of section");
								
								meta_data_info->section_image_id = 0x00000000;
								/* Update the meta data */
								ofm_otau_meta_data_update(read_buf);
							}
						}
					}
				}
				else
				{
					DBG_OTAU("Unable to identify the section identifier");
				}
			}
			
		}
	}
		
	DBG_OTAU("Current data address:0x%8X", meta_loc.current_image_data_addr);
	DBG_OTAU("OTAU data address:0x%8X", meta_loc.otau_image_data_addr);
	
	return status;
}

/** @brief ofm_write_page Function will call the flash API's to write the 
 *			page in case of the input parameters are correct
 *
 *	@param[in] section_id section id of the flash memory needs to be updated
 *	@param[in] addr starting address of the page
 *	@param[in] buf pointer to the buffer memory that needs to be copied into flash memory
 *	@param[in] len length of the page data needs to be updated
 *
 *  @return	AT_BLE_SUCCESS in case of the page is written successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_write_page(section_id_t section_id, uint32_t addr, uint8_t *buf, uint32_t len)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	ofid_data_info_t page_data;
	
	page_data.data_buf = buf;
	page_data.flash_addr = addr;
	page_data.section_id = section_id;
	page_data.size = len;
	page_data.flash_info = flash_inst;
	
	if(ofid_write_data(&page_data) == AT_OFID_OP_SUCESS)
	{
		status = AT_BLE_SUCCESS;
	}
	
	return status;
}

#ifdef OTAU_FLASH_TEST
#include "led.h"
#include "timer_hw.h"
at_ble_status_t ofm_flash_erase_nwrite_test(bool page_write, bool sector_write)
{
	at_ble_status_t status = AT_BLE_FAILURE;	
	ofid_data_info_t page_data;
	uint8_t buf[FLASH_SECTOR_SIZE];
	uint32_t idx;
	uint32_t addr;	
	
	for (idx = 0; idx < FLASH_SECTOR_SIZE; idx++)
	{
		buf[idx] = (idx % 0xFF);
	}
	
	/* Erase the Fixed memory Location */
	addr = flash_inst->section_info[0].start_addr;
	addr += flash_inst->section_info[0].size/2;
	debug_pin_set_low();
	debug_pin1_set_low();
	
	/* Make debug Line high */
	debug_pin_set_high();
	debug_pin1_set_high();

	if (ofm_erase_section_image(0x01) != AT_BLE_SUCCESS)
	{
		DBG_OTAU("Flash Sector Erase Test Failed");
		while(1);
	}
	debug_pin_set_low();
	
	/* Make debug Line high */
	debug_pin_set_high();
	
	page_data.flash_addr = addr;
	page_data.section_id = 0x01;
	page_data.data_buf = buf;
	page_data.flash_info = flash_inst;
	
	/* Erase the page */
	if (page_write)
	{
		page_data.size = FLASH_PAGE_SIZE;
	}
	
	if (sector_write)
	{
		page_data.size = FLASH_SECTOR_SIZE;
	}	
	debug_pin_set_low();
	
	/* Make debug Line high */
	debug_pin_set_high();
	
	if(ofid_write_data(&page_data) == AT_OFID_OP_SUCESS)
	{		
		status = AT_BLE_SUCCESS;
	}
	debug_pin_set_low();
	debug_pin1_set_low();
	
	/* Make debug Line high */
	debug_pin_set_high();
	debug_pin1_set_high();
	
	return status;
}

void ofm_flash_test(void)
{

	debug_pin_init();
	
	/* Temporary fix */
	flash_inst->metadata_size = (1024 *4);
	flash_inst->section_info[0].start_addr = 0x1000;
	flash_inst->section_info[0].end_addr = 0x9FFF;
	flash_inst->section_info[0].size = (36 *1024);
	
	flash_inst->section_info[1].start_addr = 0xA000;
	flash_inst->section_info[1].end_addr = 0xBFFF;
	flash_inst->section_info[1].size = (uint32_t)(8 *1024);
	
	flash_inst->section_info[2].start_addr = 0xC000;
	flash_inst->section_info[2].end_addr = 0x3BFFF;
	flash_inst->section_info[2].size = (192 *1024);
	
	DBG_OTAU("Flash Erase Test");
	ofm_flash_erase_nwrite_test(true, false);
	ofm_flash_erase_nwrite_test(false, true);
	DBG_OTAU("Flash Erase Complete");
}

#endif /* OTAU_FLASH_TEST */

/** @brief ofm_erase_section_image OTAU image section will be erased based on the section identifier
 *			sections will be erased at the beginning of the OTAU process.
 *
 *	@param[in] section_id section id of the flash memory needs to be erased
 *
 *  @return	AT_BLE_SUCCESS in case of the section erased successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_erase_section_image(section_id_t section_id)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_meta_data_t meta_data;
	ofid_data_info_t sector_info;
	
	sector_info.data_buf = NULL;
	sector_info.section_id = section_id;
	sector_info.flash_info = flash_inst;
	
	if(ofm_read_meta_data(&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		switch (section_id)
		{
			case 0x1:
			{
				sector_info.size = flash_inst->section_info[0].size/2;				
				if ((meta_data.section_image_id & 0xF) == 0x1)
				{
					sector_info.flash_addr = flash_inst->section_info[0].start_addr;
					sector_info.flash_addr += flash_inst->section_info[0].size/2;
				}
				else if ((meta_data.section_image_id & 0xF) == 0x0)
				{
					sector_info.flash_addr = flash_inst->section_info[0].start_addr;
				}
			}
			break;
			
			case 0x2:
			{
				sector_info.size = flash_inst->section_info[1].size/2;
				
				if ((meta_data.section_image_id & 0xF0) == 0x10)
				{
					sector_info.flash_addr = flash_inst->section_info[1].start_addr;
					sector_info.flash_addr += flash_inst->section_info[1].size/2;
				}
				else if ((meta_data.section_image_id & 0xF0) == 0x00)
				{
					sector_info.flash_addr = flash_inst->section_info[1].start_addr;
				}
			}
			break;
			
			case 0x03:
			{
				sector_info.size = flash_inst->section_info[2].size/2;				
				if ((meta_data.section_image_id & 0xF00) == 0x100)
				{
					sector_info.flash_addr = flash_inst->section_info[2].start_addr;
					sector_info.flash_addr += flash_inst->section_info[2].size/2;
				}
				else if ((meta_data.section_image_id & 0xF00) == 0x000)
				{
					sector_info.flash_addr = flash_inst->section_info[2].start_addr;
				}
			}
			break;
			
			default:
			{
				DBG_OTAU("Unknown section %d", section_id);
				return status;
			}			
			break;
		}
	}
	DBG_OTAU("Erasing section: Address->0x%6X, Size->%d, id->%d", sector_info.flash_addr, sector_info.size, sector_info.section_id);
	if(ofid_erase_sector(&sector_info) == AT_OFID_OP_SUCESS)
	{
		status = AT_BLE_SUCCESS;
	}
	return status;
}

at_ble_status_t ofm_erase_memory(uint32_t addr, uint32_t size)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	
	if (ofid_erase_memory(addr, size) != AT_OFID_OP_SUCESS)
	{
		status = AT_BLE_FAILURE;
	}
	
	return status;
}

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
at_ble_status_t ofm_read_page(section_id_t section_id, uint32_t addr, uint8_t *buf, uint32_t len)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	ofid_data_info_t page_data;
	
	page_data.data_buf = buf;
	page_data.flash_addr = addr;
	page_data.section_id = section_id;
	page_data.size = len;
	page_data.flash_info = flash_inst;
	
	if(ofid_read_data(&page_data) == AT_OFID_OP_SUCESS)
	{
		status = AT_BLE_SUCCESS;
	}
	
	return status;
}

/** @brief ofm_init OTAU flash driver initialization
 *			and gets the information about the flash memory
 *
 *	@param[in] params of type ofid_flash_info_t
 *
 *  @return	AT_BLE_SUCCESS flash info read successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_init(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
#ifdef OTAU_FLASH_TEST
	ofm_flash_test();
#endif
	
	
	if(ofid_init(NULL) == AT_OFID_OP_SUCESS)
	{
		if (params != NULL)
		{
			flash_inst = (ofid_flash_info_t *)params;
			/* Read the device flash info */
			if(ofid_get_flashinfo(flash_inst) == AT_OFID_OP_SUCESS)
			{				
				uint32_t idx;
				flash_inst->section_info[0].size *= flash_inst->section_info[0].image_cnt;
				flash_inst->section_info[1].size *= flash_inst->section_info[1].image_cnt;
				flash_inst->section_info[2].size *= flash_inst->section_info[2].image_cnt;
		
				DBG_OTAU("Flash_Info: \r\nTotal Flash Size->0x%6X", flash_inst->flash_size);
				DBG_OTAU("metadata_addr->0x%6X", flash_inst->metadata_addr);
				DBG_OTAU("metadata_size->0x%6X", flash_inst->metadata_size);
				DBG_OTAU("num_section->0x%6X", flash_inst->num_section);
				DBG_OTAU("page_size->0x%6X", flash_inst->page_size);
				for (idx = 0; idx < flash_inst->num_section; idx++)
				{
					DBG_OTAU("section start_addr->0x%6X", flash_inst->section_info[idx].start_addr);
					DBG_OTAU("section end_addr->0x%6X", flash_inst->section_info[idx].end_addr);
					DBG_OTAU("section size->0x%6X", flash_inst->section_info[idx].size);
					DBG_OTAU("section id->0x%6X", flash_inst->section_info[idx].id);
					DBG_OTAU("section image_cnt->0x%6X", flash_inst->section_info[idx].image_cnt);
				}				
				
				compute_image_meta_loc(NULL);
				status = AT_BLE_SUCCESS;
			}
		}		
	}
	return status;
}

/** @brief crc32_compute calculate a checksum for buffer, buffer length
 *			if resume is true then crc32 will be calculated from previously calculated crc32 value
 *	@param[in] buf input buffer for CRC calculation
 *	@param[in] len length of the buffer
 *	@param[in] resume compute the crc from previous results
 *	@param[in] crc previously computed crc value
 *
 *  @return	calculated crc32 for given data and len
 */
image_crc_t crc32_compute(uint8_t *buf, uint32_t len, bool resume, image_crc_t crc)
{
	/* Local Variables */
	uint32_t idx, l_idx;
	
	if (!resume)
	{
		crc = 0xFFFFFFFF;
	}

	for (l_idx = 0; l_idx < len; l_idx++)
	{
		uint8_t tvar = *buf++;
		idx = ((int)(crc >> 24) ^ tvar) & 0xFF;
		crc = (crc << 8) ^ crc32_lookup[idx];
	}
	return crc;
}

/** @brief ofm_get_device_info OTAU get device information
 *			and gets the information about the firmware version and device
 *
 *	@param[in] device_info device information will be read from metadata
 *
 *  @return	AT_BLE_SUCCESS device info read successfully or AT_BLE_FAILURE
 */
at_ble_status_t ofm_get_device_info(device_info_t *device_info)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_meta_data_t meta_data_info;
	
	ofid_data_info_t meta_data = {
		.data_buf = (uint8_t *)&meta_data_info,
		.flash_addr = meta_loc.current_image_data_addr,
		.section_id = 0xFF,
		.size = sizeof(image_meta_data_t),
		.flash_info = flash_inst
	};
	
	
	if(ofid_read_metadeta(&meta_data) == AT_OFID_OP_SUCESS)
	{
		memcpy((uint8_t *)device_info, (uint8_t *)&meta_data_info.dev_info, sizeof(device_info_t));
		status = AT_BLE_SUCCESS;
	}
	else
	{
		DBG_OTAU("OFM:Read meta data failed");
	}	
	
	return status;
}

/** @brief ofm_restart_device Restarts the device
 *
 *	@param[in] None
 *
 *  @return	None
 */
void ofm_restart_device(void)
{
	platform_chip_reset();
}