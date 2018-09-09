/**
* \file
*
* \brief The interface for external application wants to update EPD
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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


