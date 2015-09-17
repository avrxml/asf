/**
* \file
*
* \brief The interface for external application wants to update EPD
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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
*/

#include  "EPD_controller.h"

/**
 * \brief Initialize the EPD hardware setting 
 */
void EPD_display_init(void) {
	EPD_init();
}

/**
 * \brief Show image from the pointer of memory array
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_ptr The pointer of memory that stores previous image
 * \param new_image_ptr The pointer of memory that stores new image
 */
void EPD_display_from_pointer(uint8_t EPD_type_index,uint8_t *previous_image_ptr,
		uint8_t *new_image_ptr) {
	/* Initialize EPD hardware */
	EPD_init();
	
	/* Power on COG Driver */
	EPD_power_on();
	
	/* Initialize COG Driver */
	EPD_initialize_driver(EPD_type_index);
	
	/* Display image data on EPD from image array */
	EPD_display_from_array_prt(EPD_type_index,previous_image_ptr,new_image_ptr);
	
	/* Power off COG Driver */
	EPD_power_off (EPD_type_index);
}

/**
 * \brief Show image from Flash memory
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_address The address of memory that stores previous image
 * \param new_image_address The address of memory that stores new image
 * \param On_EPD_read_flash Developer needs to create an external function to read flash
 */
void EPD_display_from_flash(uint8_t EPD_type_index,long previous_image_address,
long new_image_address,EPD_read_flash_handler On_EPD_read_flash) {
	/* Initialize EPD hardware */
	EPD_init();
	
	/* Power on COG Driver */
	EPD_power_on();
	
	/* Initialize COG Driver */
	EPD_initialize_driver(EPD_type_index);
	
	/* Display image data on EPD from Flash memory */
	EPD_display_from_flash_prt(EPD_type_index,previous_image_address,
	    new_image_address,On_EPD_read_flash);
	
	/* Power off COG Driver */
	EPD_power_off (EPD_type_index);
}
/**
 * \brief Initialize the EPD hardware setting and COG driver
 *
 * \param EPD_type_index The defined EPD size 
 */
void EPD_power_init(uint8_t EPD_type_index) {
	EPD_init();
	EPD_power_on ();
	EPD_initialize_driver (EPD_type_index);
}

/**
 * \brief Show image from Flash memory when SPI is common used with COG and Flash
 *
 * \note 
 * - This function must work with EPD_power_init when SPI is common used with 
 *   COG and Flash, or the charge pump doesn't work correctly.
 * - EPD_power_init -> write data to flash (switch SPI) -> EPD_display_from_flash_Ex
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_address The address of memory that stores previous image
 * \param new_image_address The address of memory that stores new image
 * \param On_EPD_read_flash Developer needs to create an external function to read flash
 */
void EPD_display_from_flash_Ex(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash) {
	
	/* Display image data on EPD from Flash memory */
	EPD_display_from_flash_prt(EPD_type_index,previous_image_address,
	    new_image_address,On_EPD_read_flash);
	
	/* Power off COG Driver */
	EPD_power_off (EPD_type_index);
}


