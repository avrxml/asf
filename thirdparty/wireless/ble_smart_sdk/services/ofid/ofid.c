/**
 * \file
 *
 * \brief OTAU Flash Interface Driver
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
 * Support</a>
 */

#include <asf.h>
#include "ofid.h"

/**@brief Initialize the OTA flash driver interface layer
 *
 * @param[in] param          Reserve for future use
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS
 */
ofid_op_status_t ofid_init(void *param)
{
	spi_flash_turn_on();
	spi_flash_clock_init();
	spi_flash_init();
	return AT_OFID_OP_SUCESS;		
}


/**@brief Deinitialize the OTA flash driver interface layer
 *
 * @param[in] ofid_flash_info_t     Pointer to flash info
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS
 */
ofid_op_status_t ofid_deinit(ofid_flash_info_t * flash_inst)
{
	int i;
	flash_inst->flash_size = 0;
	for(i=0; i < flash_inst->num_section; i++)
	{
		flash_inst->section_info[i].id = 0;
		flash_inst->section_info[i].size = 0;
		flash_inst->section_info[i].image_cnt = 0;
		flash_inst->section_info[i].start_addr = 0;
		flash_inst->section_info[i].end_addr = 0;
		flash_inst->metadata_addr = 0;
		flash_inst->metadata_size = 0;
	}
	return AT_OFID_OP_SUCESS;
}

/**@brief Used to get the falsh info
 *
 * @param[in] ofid_flash_info_t     Pointer to flash info
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_FLASH_EMPTY_IMAGE
 */
ofid_op_status_t ofid_get_flashinfo(ofid_flash_info_t * flash_inst)
{
	uint32_t size = FLASH_DETECT_SECTION_NUM;
	uint8_t read_buf[32];
	int i = 0;
	int j = 0;
	/*Read the flash total size and number of loadable section*/
	spi_flash_read((unsigned char *)read_buf, \
					(unsigned long)FLASH_HEADER_START_ADDR, \
					size);
	
	/*Check the read buffer*/
	if((read_buf[0] != 0xFF) && (read_buf[1] != 0xFF) && (read_buf[2] != 0xFF) && (read_buf[3] != 0xFF))
	{
		/*Store the total flash size*/
		flash_inst->flash_size = (read_buf[0] | (read_buf[1] << 8) | (read_buf[2] << 16) | (read_buf[3] << 24));
		/*Check the read buffer*/
		if(read_buf[4] != 0xFF)
		{
			/*Store the number of loadable section*/
			flash_inst->num_section = (read_buf[4] | (read_buf[5] << 8) | (read_buf[6] << 16) | (read_buf[7] << 24));
			if(flash_inst->num_section)
			{
				size = (8*flash_inst->num_section);
				/*Read the loadable section flash info*/
				spi_flash_read((unsigned char *)read_buf, \
				(unsigned long)(FLASH_HEADER_START_ADDR+8), \
				size);
				/*Parse the number of loadable section*/
				for(i=0; i < flash_inst->num_section; i++)
				{
					/*Update the section ID*/
					flash_inst->section_info[i].id = (i+1);
					/*Update the section size*/
					flash_inst->section_info[i].size = (read_buf[0 + j] | (read_buf[1 + j] << 8) | (read_buf[2 + j] << 16) | (read_buf[3 + j] << 24));
					/*Update the section size*/
					flash_inst->section_info[i].image_cnt = (read_buf[0 + j + 4] | (read_buf[1 + j + 4] << 8) | (read_buf[2 + j + 4] << 16) | (read_buf[3 + j + 4] << 24));
					if(i==0)
					{
						flash_inst->section_info[i].start_addr = OFD_FW_SECTION_INDEX0_START_ADDR;
						flash_inst->section_info[i].end_addr   = (OFD_FW_SECTION_INDEX0_START_ADDR + (flash_inst->section_info[i].size * flash_inst->section_info[i].image_cnt) - 1);
					}
					else
					{
						flash_inst->section_info[i].start_addr = (flash_inst->section_info[i-1].end_addr + 1);
						flash_inst->section_info[i].end_addr   = ((flash_inst->section_info[i-1].end_addr + 1) + (flash_inst->section_info[i].size*flash_inst->section_info[i].image_cnt) - 1);
					}
					
					j += 8;
				} //end for loop
				flash_inst->metadata_addr = META_DATA_START_ADDRESS; 
				flash_inst->metadata_size = META_DATA_TOTAL_SIZE;
				
				flash_inst->page_size = FLASH_PAGE_SIZE;
			}
		}
		else
		{
			return AT_OFID_FLASH_EMPTY_IMAGE;
		}
	}
	else
	{
		return AT_OFID_FLASH_EMPTY_IMAGE;	
	}
return AT_OFID_OP_SUCESS;
}

/**@brief Used to verify the section ID
 *
 * @param[in] ofid_data_info_t     Pointer to section id
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_SECTION_ID
 */
ofid_op_status_t ofid_validate_id(ofid_data_info_t *data)
{
	int i;
	for(i=0; i < data->flash_info->num_section; i++)
	{
		if(data->flash_info->section_info[i].id == data->section_id)
			return AT_OFID_OP_SUCESS;
	}
	return AT_OFID_INVALID_SECTION_ID;
}

/**@brief Used to verify the flash address
 *
 * @param[in] ofid_data_info_t     Pointer to section to be validate
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_ADDR
 */
ofid_op_status_t ofid_validate_addr(ofid_data_info_t *data)
{
	if((data->flash_addr >= data->flash_info->section_info[data->section_id-1].start_addr) && ((data->flash_addr + data->size) <= data->flash_info->section_info[data->section_id-1].end_addr))
		return 	AT_OFID_OP_SUCESS;
	return 	AT_OFID_INVALID_ADDR;
}

/**@brief Read SPI flash memory
 *
 * @param[in] ofid_data_info_t     Pointer to section data to be read
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_LENGTH
 */
ofid_op_status_t ofid_read_data(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;
	
	if(data->size > FLASH_PAGE_SIZE)
	{
		return AT_OFID_INVALID_LENGTH;
	}
	
    if((status = ofid_validate_id(data)) == AT_OFID_OP_SUCESS)
	{
		spi_flash_read((unsigned char *)data->data_buf, \
		(unsigned long)data->flash_addr, \
		data->size);

	}
	return status;
}

/**@brief Erase SPI flash sector memory
 *
 * @param[in] ofid_data_info_t     Pointer to section to be erase
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS
 */
ofid_op_status_t ofid_erase_sector(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;
	
	if((status = ofid_validate_id(data)) == AT_OFID_OP_SUCESS)
	{
		spi_flash_erase((unsigned long)data->flash_addr, data->size);
	}
	return status;
}

/**@brief Erases the memory, here mostly sector size
 *
 * @param[in] address     Flash address
 * @param[in] size        Flash size
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_ADDR
 */
ofid_op_status_t ofid_erase_memory(uint32_t address, uint32_t size)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;

	if(spi_flash_erase((unsigned long)address, size))
	{
		status = AT_OFID_INVALID_ADDR;
	}
	return status;
}

/**@brief Write SPI flash memory
 *
 * @param[in] ofid_data_info_t     Pointer to section to be write
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_LENGTH
 */
ofid_op_status_t ofid_write_data(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;
	
	if(data->size > FLASH_PAGE_SIZE)
	{
		return AT_OFID_INVALID_LENGTH;
	}
	
	if((status = ofid_validate_id(data)) == AT_OFID_OP_SUCESS)
	{
		spi_flash_write((unsigned char *)data->data_buf, \
				(unsigned long)data->flash_addr, \
				data->size);	
	}
	return status;
}

/**@brief Read meta data information
 *
 * @param[in] ofid_data_info_t     Pointer to meta data section to be read
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_LENGTH
 */
ofid_op_status_t ofid_read_metadeta(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;
	
	if((!data->size) && (data->size > META_DATA_TOTAL_SIZE))
	{
		return AT_OFID_INVALID_LENGTH;
	}
	
	spi_flash_read((unsigned char *)data->data_buf, \
			(unsigned long)data->flash_addr, \
			data->size);	
	return status;
}

/**@brief Erase meta data information
 *
 * @param[in] ofid_data_info_t     Pointer to meta data section to be erase
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_LENGTH
 */
ofid_op_status_t ofid_erase_metadata(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;

	if((!data->size) && (data->size > META_DATA_TOTAL_SIZE))
	{
		return AT_OFID_INVALID_LENGTH;
	}
	
	spi_flash_erase((unsigned long)data->flash_addr, META_DATA_TOTAL_SIZE);
	
	return status;
}

/**@brief Write meta data information
 *
 * @param[in] ofid_data_info_t     Pointer to meta data section to be write
 *
 * @return at_ble_status_t AT_OFID_OP_SUCESS or AT_OFID_INVALID_LENGTH
 */
ofid_op_status_t ofid_write_metadeta(ofid_data_info_t *data)
{
	ofid_op_status_t status = AT_OFID_OP_SUCESS;
	
	if((!data->size) && (data->size > META_DATA_TOTAL_SIZE))
	{
		return AT_OFID_INVALID_LENGTH;
	}	
	spi_flash_write((unsigned char *)data->data_buf, \
	(unsigned long)data->flash_addr, \
	data->size);
		
	return status;
}