/**
* \file
*
* \brief Example for demonstrating Pervasive Displays Inc. 1.44", 2" or 2.7" EPD
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
*
**/

/**
 * \mainpage Example for demonstrating Pervasive Displays Inc. 1.44", 2" or 2.7" EPD
 *
 * \section Purpose
 *
 * Demo for Pervasive E-Paper display using Mega-RFR2 XPRO.
 *
 * \section Requirements
 *
 * This package can be used with ATMEGA256RFR2 Xplained pro evaluation kit.
 *
 * \section Description
 *
 * This example first configure Pervasive Displays Inc. 1.44", 2" or 2.7" EPD 
 * for access the COG controller, then initialize the EPD, finally it displays 
 * the pre stored images from the image_data.c file on Pervasive Displays EPD.
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board.
 * -# displays the pre stored images from the image_data.c file on Pervasive Displays EPD.
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


/**
* \brief Demonstration for showing the images alternatively on EPD of
* EPD Xplained Pro extension board
*
* \par Content
* -# Include the ASF header files (through asf.h)
* -# Include Pervasive_Displays_small_EPD.h: EPD definitions
* -# Include image_data.h: image data array
*/
								
#include "EPD_demo.h"
				
/**
* \brief The main function will toggle between two images on
* corresponding EPD depends on specified EPD size
*
* \note
* EPD size: EPD_144 = 1.44 inch, EPD_200 = 2 inch, EPD_270 = 2.7 inch
*/
int main(void){
	/** Initialize system clock and Xplained pro board */
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	//sleepmgr_init(); // Optional
	board_init();
	/** Initialize EPD hardware */
	EPD_display_init();
	
for(;;) {
		/** User selects which EPD size to run demonstration by changing the
		* 	USE_EPD_Type in Atmel Studio Project properties under Symbols 
		* 	The Image data arrays for each EPD size are defined at image_data.c 
		*/
		#if(USE_EPD_Type==USE_EPD144)
				EPD_display_from_pointer(EPD_144,(uint8_t *)&image_array_144_2,(uint8_t *)&image_array_144_1);
		#elif(USE_EPD_Type==USE_EPD200)
				EPD_display_from_pointer(EPD_200,(uint8_t *)&image_array_200_2,(uint8_t *)&image_array_200_1);
		#elif(USE_EPD_Type==USE_EPD270)
				EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_2,(uint8_t *)&image_array_270_1);
		#endif


		/* The interval of two images alternatively change is 10 seconds */
		delay_ms(10000);


		#if(USE_EPD_Type==USE_EPD144)
			EPD_display_from_pointer(EPD_144,(uint8_t *)&image_array_144_1,(uint8_t *)&image_array_144_2);
		#elif(USE_EPD_Type==USE_EPD200)
			EPD_display_from_pointer(EPD_200,(uint8_t *)&image_array_200_1,(uint8_t *)&image_array_200_2);
		#elif(USE_EPD_Type==USE_EPD270)
			EPD_display_from_pointer(EPD_270,(uint8_t *)&image_array_270_1,(uint8_t *)&image_array_270_2);
		#endif

		/* The interval of two images alternatively change is 10 seconds */
		delay_ms(10000);
		
	}

}

/**
 * \page - Quick Start Guide
 *
 * This is the quick start guide for the EPD Xplained Pro extension board with
 * Pervasive Displays Inc.'s small size EPDs on how to setup the kit to Atmel Xplained Pro. 
 * The code example in main.c shows two images alternatively change and instructs 
 * how to use the display function.
 *
 * \note
 * - Released Date: 10 Mar, 2014.  Version: 1.11
 * - PDi = Pervasive Displays Inc.(PDi) http://www.pervasivedisplays.com
 * - EPD = Electronic Paper Display (Electrophoretic Display)
 * - COG = Chip on Glass, the driver IC on EPD module
 * - COG G1 or G2: G is for generation. PDi offers V110 with G1 COG and V230 with G2 COG
 *   EPD to the market.(~2014)
 * - For driving PDi's small size EPDs, please read the "COG Driver Interface
 *   Timing" document(hereinafter COG Document) first. It explains the interface
 *   to the COG driver of EPD for a MCU based solution.
 * - COG G1 Document number: 4P008-00
 *   Download URL: http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=138408
 * - COG G2 Document number: 4P015-00
 *   Download URL: http://www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=198794
 * - This project code supports EPD size: 1.44 inch, 2 inch and 2.7 inch
 * - Supports Atmel Xplained PRO: ATMEGA256RFR2 Xplained PRO
 *
 * \section File_Explanation
 * - <b>image_data:</b>\n
 *   It defines the image arrays of each EPD size. User can use the array without
 *   application input.
 * - <b>Config</b> folder:\n
 *     -# <b>conf_EPD:</b> The EPD configurations.\n
 *        - COG_Vxxx_Gx: define which COG driving waveform you will use
 *        - COG_SPI_baudrate: SPI speed
 * - <b>Pervasive_Displays_small_EPD</b> folder:\n
 *   All of the COG driving waveforms are located in this folder. Logically developer
 *   doesn't need to change the codes in this folder in order to keep correct driving
 *   the EPDs.\n\n
 *   <b><em>Software architecture:</em></b>\n
 *   [Application] <-- [COG Interface (<em>EPD_controller</em>)] <-- [COG Process
 *   (<em>EPD_COG_process</em>)] <-- [Hardware Driver (<em>EPD_hardware_driver
 *   & EPD_hardware_gpio</em>)]\n\n
 *    -# <b>EPD_hardware_driver:</b>\n
 *       Most of the COG hardware initialization and configuration. User can implement
 *       the driver layer of EPD if some variables need to be adjusted. The provided
 *       settings and functions are Timer, SPI, PWM, temperature and EPD hardware initialization.
 *    -# <b>EPD_hardware_gpio:</b>\n
 *       GPIO pins configuration.
 *    -# <b>EPD_COG_process.h:</b>\n
 *       The common definition of COG driving process.
 *    -# <b>EPD_controller:</b>\n
 *       The interface for external function to work with EPD.
 *    -# <b>EPD_COG:</b>\n
 *       The link source of different COG and EPD switching to be used.
 *    -# <b>COG</b> folder:\n
 *       Each COG driving file presents the different waveform driving processes of COG
 *       and updating stages.
 *       The parameters of driving different EPD is defined at COG_parameters_t structure
 *       which is easy for developer adjusting initial parameters, resolution, frame time
 *       of MCU and the size of data line.
 *       - <b>EPD_COG_process_Vxxx_Gx:</b>\n
 *         The waveform driving processes and updating stages of COG v? with VXXX EPD.
 *       - <b>EPD_COG_partial_update_Vxxx:</b>\n
 *         The partial update waveform of driving processes and updating stages
 *         of COG v? with VXXX EPD.
 *
 *
 * \section Use_Case
 * -# <b>EPD_display_from_pointer</b>: Load two image data arrays from image_data.c
 *   according to predefined EPD size.
 * -# <b>EPD_display_from_flash</b>:
 *   Load stored image data from flash memory according to predefined EPD size. User
 *   must convert 1 bit bitmap image file to hex data in advance and store in flash
 *   memory.
 *
 * \section Steps
 * -# Ensure the EPD is connected correctly on the EPD Xplained Pro extension board
 * -# Connect the EPD Xplained Pro to the ATMEGA256RFR2 Xplained Pro header marked EXT3
 * -# Connect the ATMEGA256RFR2 Xplained Pro to computer's USB port via USB cable
 * -# Ensure what the EPD size you are connecting. Open Atmel Studio Project properties 
 *    under Symbols and find "USE_EPD_Type=USE_EPD200". Change the USE_EPDXXX 
 *    to the correct size.
 * -# Close the J2 jumper if the connected EPD is 1.44" V110, 1.44" V230 or 2" V230.
 *    For the other types, please keep the J2 is opened.
 * -# Start debugging to program the driving code to MCU. The EPD will show two images
 *   alternatively change every 10 seconds (default).
 *
 *
 * \section EPD Xplained Pro extension header pins
 * ================================================================
 * |Pin| Function       | Description                             |
 * |---|----------------|-----------------------------------------|
 * | 1 | ID             | Communication line to ID chip           |
 * | 2 | GND            | Ground                                  |
 * | 3 | Temperature    | On board temperature sensor output (ADC)|
 * | 4 | BORDER_CONTROL | Border control pin (GPIO)               |
 * | 5 | DISCHARGE      | EPD discharge when EPD power off (GPIO) |
 * | 6 | /RESET         | Reset signal. Low enable (GPIO)         |
 * | 7 | PWM            | Square wave when EPD power on (PWM)     |
 * | 8 | PANEL_ON       | COG driver power control pin (GPIO)     |
 * | 9 | BUSY           | COG busy pin (GPIO)                     |
 * |10 | FLASH_CS       | On board flash chip select (GPIO)       |
 * |11 |                |                                         |
 * |12 |                |                                         |
 * |13 |                |                                         |
 * |14 |                |                                         |
 * |15 | /EPD_CS        | Chip Select. Low enable (GPIO)          |
 * |16 | SPI_MOSI       | Serial input from host MCU to EPD       |
 * |17 | SPI_MISO       | Serial output from EPD to host MCU      |
 * |18 | SPI_CLK        | Clock for SPI                           |
 * |19 | GND            | Ground                                  |
 * |20 | VCC            | Target supply voltage                   |
 *
 * \section PDi EPD displays
 * ========================================
 * | Size | PDi Model              | FPL  |
 * |------|------------------------|------|
 * | 1.44 | EK014AS014, EK014AS015 | V110 |
 * | 2.0  | EG020AS012, EG020AS013 | V110 |
 * | 2.7  | EM027AS012, EM027AS013 | V110 |
 * | 1.44 | TBD (Q2'14)            | V230 |
 * | 2.0  | EG020BS011, EG020BS012 | V230 |
 * | 2.7  | EM027BS013, EM027BS014 | V230 |
 */