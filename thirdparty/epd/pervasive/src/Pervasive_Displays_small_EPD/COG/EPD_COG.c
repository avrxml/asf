/**
* \file
*
* \brief The link source of different COG and EPD 
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

#include "Pervasive_Displays_small_EPD.h"

/** 
* To maintain the Pervasive Displays COG file structure compatibility with ASF 
* the different COG versions files are copied in this single file.
* To avoid confusions the copied 'c' file contents maintained as such including  
* licence header.
*
* In case in future Pervasive Displays developed new COG versions that c file can 
* be copied here and respective #elif (defined COG_Vxxx_Gx) should be updated. 
* 
* Files copied in this "EPD_COG.C" file
* 1, COG/V110_G1/EPD_COG_process_V110_G1.c
* 2, COG/V110/EPD_COG_partial_update_V110_G1.c
* 3, COG/V230_G2/EPD_COG_process_V230_G2.c
* 4, < future version >

*/

#if (defined COG_V110_G1)

//************************************************************************************//
//#include "COG/V110_G1/EPD_COG_process_V110_G1.c"
//#include "COG/V110/EPD_COG_partial_update_V110_G1.c"
//************************************************************************************//

/**
* \file
*
* \brief The waveform driving processes and updating stages of G1 COG with V110 EPD
*
* Copyright (c) 2012-2013 Pervasive Displays Inc. All rights reserved.
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

#include "EPD_COG_process.h"
/** 
 * \brief The COG parameters of different EPD size 
 */
const struct COG_parameters_t COG_parameters[COUNT_OF_EPD_TYPE] = {
	{
		// FOR 1.44"
		{0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00},
		0x03,
		(128/8),
		96,
		((((128+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.0"
		{0x00,0x00,0x00,0x00,0x01,0xFF,0xE0,0x00},
		0x03,
		(200/8),
		96,
		((((200+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.7"
		{0x00,0x00,0x00,0x7F,0xFF,0xFE,0x00,0x00},
		0x00,
		(264/8),
		176,
		((((264+176)*2)/8)+1),
		0,
		630
	}
};

/* Temperature factor combines with stage time for each driving stage */
const uint16_t temperature_table[3][8] = {
	{(480*17),(480*12),(480*8),(480*4),(480*3),(480*2),(480*1),(480*0.7)},
	{(480*17),(480*12),(480*8),(480*4),(480*3),(480*2),(480*1),(480*0.7)},
	{(630*17),(630*12),(630*8),(630*4),(630*3),(630*2),(630*1),(630*0.7)},
};

const uint8_t   SCAN_TABLE[4] = {0xC0,0x30,0x0C,0x03};
static uint16_t stage_time;
static COG_line_data_packet_type COG_Line;
static EPD_read_flash_handler _On_EPD_read_flash=NULL;
static uint16_t current_frame_time;
static uint8_t  *data_line_even;
static uint8_t  *data_line_odd;
static uint8_t  *data_line_scan;

/**
* \brief According to EPD size and temperature to get stage_time
* \note Refer to COG document Section 5.3 for more details
*
* \param EPD_type_index The defined EPD size
*/
static void set_temperature_factor(uint8_t EPD_type_index) {
	int8_t temperature;
	temperature = get_temperature();
	if (temperature <= -10) {
		stage_time = temperature_table[EPD_type_index][0];
		} else if (-5 >= temperature && temperature > -10) {
		stage_time = temperature_table[EPD_type_index][1];
		} else if (5 >= temperature && temperature > -5) {
		stage_time = temperature_table[EPD_type_index][2];
		} else if (10 >= temperature && temperature > 5) {
		stage_time = temperature_table[EPD_type_index][3];
		} else if (15 >= temperature && temperature > 10) {
		stage_time = temperature_table[EPD_type_index][4];
		} else if (20 >= temperature && temperature > 15) {
		stage_time = temperature_table[EPD_type_index][5];
		} else if (40 >= temperature && temperature > 20) {
		stage_time = temperature_table[EPD_type_index][6];
	} else stage_time = temperature_table[EPD_type_index][7];
}

/**
* \brief Initialize the EPD hardware setting
*/
void EPD_init(void) {
	EPD_display_hardware_init();
	EPD_cs_low();
	EPD_pwm_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
}


/**
* \brief Select the EPD size to get line data array for driving COG
*
* \param EPD_type_index The defined EPD size
*/
void COG_driver_EPDtype_select(uint8_t EPD_type_index) {
	switch(EPD_type_index) {
		case EPD_144:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_144.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_144.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_144.scan[0];
		break;
		case EPD_200:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_200.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_200.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_200.scan[0];
		break;
		case EPD_270:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_270.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_270.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_270.scan[0];
		break;
	}
}


/**
* \brief Power on COG Driver
* \note For detailed flow and description, please refer to the COG document Section 3.
*/
void EPD_power_on (void) {
	/* Initial state */
	EPD_discharge_low();
	EPD_rst_low();
	EPD_cs_low();
	epd_spi_init();
	epd_spi_attach();
	
	PWM_run(5); //The PWM signal starts toggling
	EPD_Vcc_turn_on(); //Vcc and Vdd >= 2.7V
	PWM_run(10);
	EPD_cs_high(); // /CS=1
	EPD_border_high(); //BORDER=1
	EPD_rst_high(); // /RESET=1
	PWM_run(5);
	EPD_rst_low(); // /RESET=0
	PWM_run(5);
	EPD_rst_high(); // /RESET=1
	PWM_run(5);
}


/**
* \brief Initialize COG Driver
* \note For detailed flow and description, please refer to the COG document Section 4.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_initialize_driver (uint8_t EPD_type_index) {
	uint8_t SendBuffer[2];
	uint16_t k;
	
	// Empty the Line buffer
	for (k = 0; k <= LINE_BUFFER_DATA_SIZE; k ++) {
		COG_Line.uint8[k] = 0x00;
	}
	// Determine the EPD size for driving COG
	COG_driver_EPDtype_select(EPD_type_index);

	// Sense temperature to determine Temperature Factor
	set_temperature_factor(EPD_type_index);
	k = 0;
	while (EPD_IsBusy()) {
		if((k++) >= 0x0FFF) return ERROR_BUSY;
	}
	// Channel select
	epd_spi_send (0x01, (uint8_t *)&COG_parameters[EPD_type_index].channel_select, 8);
	
	// DC/DC frequency setting
	epd_spi_send_byte (0x06, 0xFF);
	
	// High power mode OSC setting
	epd_spi_send_byte (0x07, 0x9D);
	
	// Disable ADC
	epd_spi_send_byte (0x08, 0x00);
	
	// Set Vcom level
	SendBuffer[0] = 0xD0;
	SendBuffer[1] = 0x00;
	epd_spi_send (0x09, SendBuffer, 2);
	
	// Gate and source voltage level
	epd_spi_send_byte (0x04,COG_parameters[EPD_type_index].voltage_level);
	PWM_run(5);
	
	// Driver latch on (cancel register noise)
	epd_spi_send_byte(0x03, 0x01);
	
	// Driver latch off
	epd_spi_send_byte(0x03, 0x00);
	
	// Start charge pump positive V VGH & VDH on
	epd_spi_send_byte (0x05, 0x01);
	PWM_run(30);
	
	// Start charge pump neg voltage VGL & VDL on
	epd_spi_send_byte (0x05, 0x03);
	delay_ms (30);
	
	// Set charge pump Vcom_Driver to ON
	epd_spi_send_byte(0x05, 0x0F);
	delay_ms(30);
	
	// Output enable to disable
	epd_spi_send_byte(0x02, 0x24);
	
	return RES_OK;
}

/**
 * \brief The driving stages for getting Odd/Even data and writing the data
 * from memory array to COG
 *
 * \note
 * - There are 4 stages to complete an image update on EPD.
 * - Each of the 4 stages time should be the same uses the same number of frames.
 * - One dot/pixel is comprised of 2 bits which are White(10), Black(11) or Nothing(01).
 *   The image data bytes must be divided into Odd and Even bytes.
 * - The COG driver uses a buffer to write one line of data (FIFO) - interlaced
 *   Even byte {D(200,y),D(198,y), D(196,y), D(194,y)}, ... ,{D(8,y),D(6,y),D(4,y), D(2,y)}
 *   Scan byte {S(1), S(2)...}, Odd{D(1,y),D(3,y)...}
 *   Odd byte  {D(1,y),D(3,y), D(5,y), D(7,y)}, ... ,{D(193,y),D(195,y),D(197,y), D(199,y)}
 * - One data bit can be
 * - For more details on the driving stages, please refer to the COG document Section 5.
 *
 * \param EPD_type_index The defined EPD size
 * \param image_ptr The pointer of memory that stores image that will send to COG
 * \param stage_no The assigned stage number that will proceed
 */
static void stage_handle_array(uint8_t EPD_type_index,uint8_t *image_prt,uint8_t stage_no) {
	/* x for horizontal_size loop, y for vertical_size loop, which are EPD pixel size */
	uint16_t x,y,k;
	static volatile uint8_t	temp_byte; // Temporary storage for image data check
	uint8_t *backup_image_prt; // Backup image address pointer
	backup_image_prt = image_prt;
	current_frame_time = COG_parameters[EPD_type_index].frame_time_offset;
	/* Start a system SysTick timer to ensure the same duration of each stage  */
	start_EPD_timer();
	
	/* Do while total time of frames exceed stage time
	* Per frame */
	do {
		image_prt=backup_image_prt;
		
		/* Per data line (vertical size) */
		for (y = 0; y < COG_parameters[EPD_type_index].vertical_size; y++) {
			/* Set charge pump voltage level reduce voltage shift */
			epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);
			
			k = COG_parameters[EPD_type_index].horizontal_size-1;
			for (x = 0; x < COG_parameters[EPD_type_index].horizontal_size; x++) {
				temp_byte =*image_prt++;
				switch(stage_no) {
					case Stage1: // Compensate, Inverse previous image
					/* Example at stage 1 to get Even and Odd data
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+ */
					data_line_odd[x]     = ((temp_byte & 0x80) ? BLACK3  : WHITE3); // BLACK3 = 0xC0 = 1100 0000
					data_line_odd[x]    |= ((temp_byte & 0x20) ? BLACK2  : WHITE2); // BLACK2 = 0x30 = 0011 0000
					data_line_odd[x]    |= ((temp_byte & 0x08) ? BLACK1  : WHITE1); // WHITE1 = 0x08 = 0000 1000
					data_line_odd[x]    |= ((temp_byte & 0x02) ? BLACK0  : WHITE0); // WHITE0 = 0x02 = 0000 0010
					/* data_line_odd[x] = 1100 0000 | 0011 0000 | 0000 1000 | 0000 0010 = 1111 1010
					* See Even data row at the table below*/
					
					data_line_even[k]    = ((temp_byte & 0x01) ? BLACK3  : WHITE3); // WHITE3 = 0x80 = 1000 0000
					data_line_even[k]   |= ((temp_byte & 0x04) ? BLACK2  : WHITE2); // BLACK2 = 0x30 = 0011 0000
					data_line_even[k]   |= ((temp_byte & 0x10) ? BLACK1  : WHITE1); // BLACK1 = 0x0C = 0000 1100
					data_line_even[k--] |= ((temp_byte & 0x40) ? BLACK0  : WHITE0); // WHITE0 = 0x02 = 0000 0010
					/* data_line_even[k] = 1000 0000 | 0011 0000 | 0000 1100 | 0000 0010 = 1011 1110
					* See Odd data row at the table below
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+
					* | Color   |  W |  B |  W |  W |  B |  W |  B |  B | W=White, B=Black, N=Nothing
					* +---------+----+----+----+----+----+----+----+----+
					* | Stage 1 |  B |  W |  B |  B |  W |  B |  W |  W | Inverse
					* +---------+----+----+----+----+----+----+----+----+
					* | Input   | 11 | 10 | 11 | 11 | 10 | 11 | 10 | 10 | W=10, B=11, N=01
					* +---------+----+----+----+----+----+----+----+----+
					* |Odd data | 11 |    | 11 |    | 10 |    | 10 |    | = 1111 1010
					* +---------+----+----+----+----+----+----+----+----+
					* |Even data|    | 10 |    | 11 |    | 11 |    | 10 | = 1011 1110
					* +---------+----+----+----+----+----+----+----+----+ */
						break;
					case Stage2: // White
					data_line_odd[x]     = ((temp_byte & 0x80) ?  WHITE3 : NOTHING3);
					data_line_odd[x]    |= ((temp_byte & 0x20) ?  WHITE2 : NOTHING2);
					data_line_odd[x]    |= ((temp_byte & 0x08) ?  WHITE1 : NOTHING1);
					data_line_odd[x]    |= ((temp_byte & 0x02) ?  WHITE0 : NOTHING0);

					data_line_even[k]    = ((temp_byte & 0x01) ?  WHITE3 : NOTHING3);
					data_line_even[k]   |= ((temp_byte & 0x04) ?  WHITE2 : NOTHING2);
					data_line_even[k]   |= ((temp_byte & 0x10) ?  WHITE1 : NOTHING1);
					data_line_even[k--] |= ((temp_byte & 0x40) ?  WHITE0 : NOTHING0);
						break;
					case Stage3: // Inverse new image
					data_line_odd[x]     = ((temp_byte & 0x80) ? BLACK3  : NOTHING3);
					data_line_odd[x]    |= ((temp_byte & 0x20) ? BLACK2  : NOTHING2);
					data_line_odd[x]    |= ((temp_byte & 0x08) ? BLACK1  : NOTHING1);
					data_line_odd[x]    |= ((temp_byte & 0x02) ? BLACK0  : NOTHING0);

					data_line_even[k]    = ((temp_byte & 0x01) ? BLACK3  : NOTHING3);
					data_line_even[k]   |= ((temp_byte & 0x04) ? BLACK2  : NOTHING2);
					data_line_even[k]   |= ((temp_byte & 0x10) ? BLACK1  : NOTHING1);
					data_line_even[k--] |= ((temp_byte & 0x40) ? BLACK0  : NOTHING0);
						break;
					case Stage4: // New image
					data_line_odd[x]     = ((temp_byte & 0x80) ? WHITE3  : BLACK3 );
					data_line_odd[x]    |= ((temp_byte & 0x20) ? WHITE2  : BLACK2 );
					data_line_odd[x]    |= ((temp_byte & 0x08) ? WHITE1  : BLACK1 );
					data_line_odd[x]    |= ((temp_byte & 0x02) ? WHITE0  : BLACK0 );

					data_line_even[k]    = ((temp_byte & 0x01) ? WHITE3  : BLACK3 );
					data_line_even[k]   |= ((temp_byte & 0x04) ? WHITE2  : BLACK2 );
					data_line_even[k]   |= ((temp_byte & 0x10) ? WHITE1  : BLACK1 );
					data_line_even[k--] |= ((temp_byte & 0x40) ? WHITE0  : BLACK0 );
						break;
				}
			}
			
			/* Scan byte shift per data line */
			data_line_scan[(y>>2)] = SCAN_TABLE[(y%4)];
			
			/* For 1.44 inch EPD, the border uses the internal signal control byte. */
			if(EPD_type_index == EPD_144)
				COG_Line.line_data_by_size.line_data_for_144.border_byte=0x00;
			
			/* Sending data */
			epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, 
				COG_parameters[EPD_type_index].data_line_size);
			
			/* Turn on Output Enable */
			epd_spi_send_byte (0x02, 0x2F);
			
			data_line_scan[(y>>2)]=0;
		}
		/* Count the frame time with offset */
		current_frame_time=(uint16_t)get_current_time_tick()+
			COG_parameters[EPD_type_index].frame_time_offset;
	} while (stage_time>current_frame_time);
	
	/* Do while the SysTick timer fulfills the stage time */
	while(stage_time>get_current_time_tick());
	
	/* Stop system timer */
	stop_EPD_timer();
}

/**
 * \brief The driving stages for getting Odd/Even data and writing the data
 * from Flash memory to COG
 *
 * \note
 * - Refer to stage_handle_array comment note.
 * - For more details on the driving stages, please refer to the COG document Section 5.
 *
 * \param EPD_type_index The defined EPD size
 * \param image_data_address The address of flash memory that stores image
 * \param stage_no The assigned stage number that will proceed
 */
static void stage_handle_flash(uint8_t EPD_type_index,long image_data_address,uint8_t stage_no) {
	/* x for horizontal_size loop, y for vertical_size loop, which are EPD pixel size */
	uint16_t x,y,k;
	static volatile uint8_t	temp_byte; // Temporary storage for image data check
	long original_image_address; // Backup original image address
	uint8_t byte_array[LINE_BUFFER_DATA_SIZE];
	original_image_address=image_data_address;
	current_frame_time=COG_parameters[EPD_type_index].frame_time_offset;
	
	/* Start a system SysTick timer to ensure the same duration of each stage  */
	start_EPD_timer();
	
	/* Do while total time of frames exceed stage time
	* Per frame */
	do {
		image_data_address=original_image_address;
		
		/* Per data line (vertical size) */
		for (y = 0; y < COG_parameters[EPD_type_index].vertical_size; y++){
			/* Set charge pump voltage level reduce voltage shift */
			epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);
			
			k = COG_parameters[EPD_type_index].horizontal_size-1;
			if(_On_EPD_read_flash!=NULL) {
				_On_EPD_read_flash(image_data_address,(uint8_t *)&byte_array,
				COG_parameters[EPD_type_index].horizontal_size);
			}
			
			for (x = 0; x < COG_parameters[EPD_type_index].horizontal_size; x++){
				temp_byte =byte_array[x];
				switch(stage_no) {
					case Stage1: // Compensate, Inverse previous image
					/* See the stage #1 example in stage_handle_array */
					data_line_odd[x]	 =  ((temp_byte & 0x80) ? BLACK3  : WHITE3);
					data_line_odd[x]	|=  ((temp_byte & 0x20) ? BLACK2  : WHITE2);
					data_line_odd[x]	|=  ((temp_byte & 0x08) ? BLACK1  : WHITE1);
					data_line_odd[x]	|=  ((temp_byte & 0x02) ? BLACK0  : WHITE0);

					data_line_even[k]    = ((temp_byte & 0x01) ? BLACK3  : WHITE3);
					data_line_even[k]   |= ((temp_byte & 0x04) ? BLACK2  : WHITE2);
					data_line_even[k]   |= ((temp_byte & 0x10) ? BLACK1  : WHITE1);
					data_line_even[k--] |= ((temp_byte & 0x40) ? BLACK0  : WHITE0);
						break;
					
					case Stage2: // White
					data_line_odd[x] 	 =  ((temp_byte & 0x80) ?  WHITE3 : NOTHING3);
					data_line_odd[x]	|=  ((temp_byte & 0x20) ?  WHITE2 : NOTHING2);
					data_line_odd[x]	|=  ((temp_byte & 0x08) ?  WHITE1 : NOTHING1);
					data_line_odd[x]	|=  ((temp_byte & 0x02) ?  WHITE0 : NOTHING0);

					data_line_even[k]    =  ((temp_byte & 0x01) ?  WHITE3 : NOTHING3);
					data_line_even[k]   |=  ((temp_byte & 0x04) ?  WHITE2 : NOTHING2);
					data_line_even[k]   |=  ((temp_byte & 0x10) ?  WHITE1 : NOTHING1);
					data_line_even[k--] |=  ((temp_byte & 0x40) ?  WHITE0 : NOTHING0);
						break;
					
					case Stage3: // Inverse new image
					data_line_odd[x]     = ((temp_byte & 0x80) ? BLACK3  : NOTHING3);
					data_line_odd[x]    |= ((temp_byte & 0x20) ? BLACK2  : NOTHING2);
					data_line_odd[x]    |= ((temp_byte & 0x08) ? BLACK1  : NOTHING1);
					data_line_odd[x]    |= ((temp_byte & 0x02) ? BLACK0  : NOTHING0);

					data_line_even[k]    = ((temp_byte & 0x01) ? BLACK3  : NOTHING3);
					data_line_even[k]   |= ((temp_byte & 0x04) ? BLACK2  : NOTHING2);
					data_line_even[k]   |= ((temp_byte & 0x10) ? BLACK1  : NOTHING1);
					data_line_even[k--] |= ((temp_byte & 0x40) ? BLACK0  : NOTHING0);
						break;
					
					case Stage4: // New image
					data_line_odd[x]     = ((temp_byte & 0x80) ? WHITE3  : BLACK3 );
					data_line_odd[x]    |= ((temp_byte & 0x20) ? WHITE2  : BLACK2 );
					data_line_odd[x]    |= ((temp_byte & 0x08) ? WHITE1  : BLACK1 );
					data_line_odd[x]    |= ((temp_byte & 0x02) ? WHITE0  : BLACK0 );

					data_line_even[k]    = ((temp_byte & 0x01) ? WHITE3  : BLACK3 );
					data_line_even[k]   |= ((temp_byte & 0x04) ? WHITE2  : BLACK2 );
					data_line_even[k]   |= ((temp_byte & 0x10) ? WHITE1  : BLACK1 );
					data_line_even[k--] |= ((temp_byte & 0x40) ? WHITE0  : BLACK0 );
						break;

                }
            }
			image_data_address+=LINE_SIZE;
			/* Scan byte shift per data line */
			data_line_scan[(y>>2)]= SCAN_TABLE[(y%4)];
			
			/* For 1.44 inch EPD, the border uses the internal signal control byte. */
			if(EPD_type_index==EPD_144)
				COG_Line.line_data_by_size.line_data_for_144.border_byte=0x00;
			
			/* Sending data */
			epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, 
				COG_parameters[EPD_type_index].data_line_size);

			/* Turn on Output Enable */
			epd_spi_send_byte (0x02, 0x2F);

			data_line_scan[(y>>2)]=0;
		}
		/* Count the frame time with offset */
		current_frame_time=(uint16_t)get_current_time_tick()+
			COG_parameters[EPD_type_index].frame_time_offset ;
	} while (stage_time>current_frame_time);
	
	/* Do while the SysTick timer fulfills the stage time */
	while(stage_time>get_current_time_tick());
	
	/* Stop system timer */
	stop_EPD_timer();
}

/**
* \brief Write image data from memory array to the EPD
* \note For more detail on driving stages, please refer to COG document Section 5.
*
* \param EPD_type_index The defined EPD size
* \param previous_image_ptr The pointer of memory that stores previous image
* \param new_image_ptr The pointer of memory that stores new image
*/
void EPD_display_from_array_prt (uint8_t EPD_type_index, uint8_t *previous_image_ptr,
uint8_t *new_image_ptr) {
	stage_handle_array(EPD_type_index,previous_image_ptr,Stage1);
	stage_handle_array(EPD_type_index,previous_image_ptr,Stage2);
	stage_handle_array(EPD_type_index,new_image_ptr,Stage3);
	stage_handle_array(EPD_type_index,new_image_ptr,Stage4);
}

/**
* \brief Write image data from Flash memory to the EPD
*
* \param EPD_type_index The defined EPD size
* \param previous_image_flash_address The previous image address of flash memory
* \param new_image_flash_address The new image address of flash memory
* \param On_EPD_read_flash Developer needs to create an external function to read flash
*/
void EPD_display_from_flash_prt (uint8_t EPD_type_index, long previous_image_flash_address,
     long new_image_flash_address,EPD_read_flash_handler On_EPD_read_flash) {
	_On_EPD_read_flash=On_EPD_read_flash;
	stage_handle_flash(EPD_type_index,previous_image_flash_address,Stage1);
	stage_handle_flash(EPD_type_index,previous_image_flash_address,Stage2);
	stage_handle_flash(EPD_type_index,new_image_flash_address     ,Stage3);
	stage_handle_flash(EPD_type_index,new_image_flash_address     ,Stage4);
}

/**
* \brief Write Nothing Frame to COG
* \note A frame whose all D(x,y) are N(01). 0101=0x55=NOTHING
*
* \param EPD_type_index The defined EPD size
*/
static inline void nothing_frame (uint8_t EPD_type_index) {
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]=NOTHING;
		data_line_odd[i]=NOTHING;
	}
	for (i = 0; i < COG_parameters[EPD_type_index].vertical_size; i++) {
		/* Set charge pump voltage level reduce voltage shift */
		epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);
		
		/* Scan byte shift per data line */
		data_line_scan[(i>>2)]=SCAN_TABLE[(i%4)];
		
		/* Sending data */
		epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
		
		/* Turn on Output Enable */
		epd_spi_send_byte (0x02, 0x2F);
		
		data_line_scan[(i>>2)]=0;
	}
}

/**
* \brief Write Dummy Line to COG
* \note A line whose all Scan Bytes are 0x00
*
* \param EPD_type_index The defined EPD size
*/
static inline void dummy_line(uint8_t EPD_type_index) {
	uint8_t	i;
	for (i = 0; i < (COG_parameters[EPD_type_index].vertical_size/8); i++) {
		switch(EPD_type_index) {
			case EPD_144:
				COG_Line.line_data_by_size.line_data_for_144.scan[i]=0x00;
				break;
			case EPD_200:
				COG_Line.line_data_by_size.line_data_for_200.scan[i]=0x00;
				break;
			case EPD_270:
				COG_Line.line_data_by_size.line_data_for_270.scan[i]=0x00;
				break;
		}
	}
	/* Set charge pump voltage level reduce voltage shift */
	epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);
	
	/* Sending data */
	epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);

	/* Turn on Output Enable */
	epd_spi_send_byte (0x02, 0x2F);
}
static void border_line(uint8_t EPD_type_index)
{
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]=NOTHING;
		data_line_odd[i]=NOTHING;
	}
	for (i = 0; i < (COG_parameters[EPD_type_index].vertical_size/4); i++) {
		data_line_scan[i]=0x00;
	}
	COG_Line.line_data_by_size.line_data_for_144.border_byte=0xaa;
	epd_spi_send_byte (0x04, 0x03);

	// SPI (0x0a, line data....)
	epd_spi_send (0x0a, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);

	// SPI (0x02, 0x2F)
	epd_spi_send_byte (0x02, 0x2F);
}
/**
* \brief Power Off COG Driver
* \note For detailed flow and description, please refer to the COG document Section 6.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_power_off (uint8_t EPD_type_index) {
	
	nothing_frame (EPD_type_index);
	
	if(EPD_type_index==EPD_144) {
		EPD_border_high();
		border_line(EPD_type_index);
		delay_ms (200);
	}
	else {
		dummy_line(EPD_type_index);
		delay_ms (25);
		EPD_border_low();
		delay_ms (200);
		EPD_border_high();
	}
	// Latch reset turn on
	epd_spi_send_byte (0x03, 0x01);
	
	// Output enable off
	epd_spi_send_byte (0x02, 0x05);
	
	// Power off charge pump Vcom
	epd_spi_send_byte (0x05, 0x0E);
	
	// Power off charge negative voltage
	epd_spi_send_byte (0x05, 0x02);
	
	// Discharge
	epd_spi_send_byte (0x04, 0x0C);	
	delay_ms (120);
	
	// Turn off all charge pumps
	epd_spi_send_byte (0x05, 0x00);
	
	// Turn off osc
	epd_spi_send_byte (0x07, 0x0D);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0x50);
	delay_ms (40);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0xA0);	
	delay_ms (40);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0x00);
	
	// Set power and signals = 0
	EPD_rst_low ();
	epd_spi_detach ();
	EPD_cs_low ();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	
	// External discharge = 1
	EPD_discharge_high ();
	delay_ms (150);
	
	// External discharge = 0
	EPD_discharge_low ();
	
	return RES_OK;
}

//************************************************************************************//
//#include "COG/V110/EPD_COG_partial_update_V110.c"
//************************************************************************************//

/**
* \file
*
* \brief The partial update waveform of driving processes and updating stages 
         of G1 COG with V110 EPD
*
* Copyright (c) 2012-2013 Pervasive Displays Inc. All rights reserved.
*
*  Authors: Pervasive Displays Inc.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "EPD_COG_process.h"

extern void read_flash(long Address,uint8_t *target_address, uint8_t byte_length);
extern void write_flash(long Address,uint8_t *source_address, uint8_t byte_length);
extern void erase_image(long address,uint8_t ptype);
void EPD_display_partialupdate (uint8_t EPD_type_index, long previous_image_address,
                                long new_image_address,long mark_image_address,
                                EPD_read_flash_handler On_EPD_read_flash);
#define _flash_line_size	64 //bytes of a line size in flash

//const uint8_t   SCAN_TABLE[4] = {0xC0,0x30,0x0C,0x03};
//static uint16_t stage_time;
//static uint16_t stage_time;
//static COG_line_data_packet_type COG_Line;
//static EPD_read_flash_handler _On_EPD_read_flash=NULL;
//static uint16_t current_frame_time;
//static uint8_t  *data_line_even;
//static uint8_t  *data_line_odd;
//static uint8_t  *data_line_scan;
//


/**
* \brief Save the image combined with inputted ASCII string for next 
*
* \param EPD_type_index The defined EPD size
* \param previous_image_address The previous image address
* \param new_image_address The new (canvas) image address
* \param mark_image_address The mark image address
*/
static void save_partial_image (uint8_t EPD_type_index,long previous_address,
                                long new_address,long mark_address ) {
	uint16_t y,x;
	long mark_image_original=mark_address;
	uint8_t *previous_lin,*new_line,*mark_line;
	epd_spi_attach();
	previous_lin =  (uint8_t*) malloc(COG_parameters[EPD_type_index].horizontal_size);
	new_line =	(uint8_t*) malloc(COG_parameters[EPD_type_index].horizontal_size);
	mark_line = (uint8_t*) malloc(COG_parameters[EPD_type_index].horizontal_size);

	for(y=0; y<=COG_parameters[EPD_type_index].vertical_size; y++) {
		read_flash(previous_address,previous_lin,COG_parameters[EPD_type_index].horizontal_size);
		read_flash(new_address,new_line,COG_parameters[EPD_type_index].horizontal_size);
		read_flash(mark_address,mark_line,COG_parameters[EPD_type_index].horizontal_size);
		for(x=0; x<COG_parameters[EPD_type_index].horizontal_size; x++) {
			/** Only move the non-marked area of previous image. Keep ASCII text */
			if(mark_line[x]==0xFF) {
				new_line[x]=previous_lin[x];
			}
		}
		write_flash(new_address,new_line,COG_parameters[EPD_type_index].horizontal_size);
		previous_address+=_flash_line_size;
		new_address+=_flash_line_size;
		mark_address+=_flash_line_size;
	}

	free(previous_lin);
	free(new_line);
	free(mark_line);

	//Clear Mark Image
	erase_image(mark_image_original,EPD_type_index);
	epd_spi_detach();
}

/**
* \brief The driving stages for getting Odd/Even data and writing the data
* from Flash memory to COG using partial update
*
* \note
* - Refer to stage_handle_array comment note in EPD_COG_process_V110.c
* - Partial update uses one buffer driving method which is discarded the previous
*   image compensate and run 2 stages only.
*   -# The 1st stage is black/white alternately
*   -# The 2nd stage is to show new image
* - just the marked area will be changed.
*
* \param EPD_type_index The defined EPD size
* \param previous_image_address The previous image address
* \param new_image_address The new (canvas) image address
* \param mark_image_address The mark image address
* \param stage_no The assigned stage number that will proceed
*/
static void stage_handle_partial_update(uint8_t EPD_type_index,long previous_image_address,
                                        long new_image_address,long mark_image_address,
                                        uint8_t stage_no) {
	/** x for horizontal_size loop, y for vertical_size loop, which are EPD pixel size */
	uint16_t x,y,k;
	static volatile uint8_t	temp_byte; // Temporary storage for image data check
	long address_offset;
	uint8_t *new_line,*mark_line;
	uint8_t frame_count; //count for sending black or white
	/* Get line data array of EPD size */
	COG_driver_EPDtype_select(EPD_type_index);

	new_line =  (uint8_t*) malloc(COG_parameters[EPD_type_index].horizontal_size);
	mark_line = (uint8_t*) malloc(COG_parameters[EPD_type_index].horizontal_size);

	current_frame_time=COG_parameters[EPD_type_index].frame_time_offset;

	/* Start a system SysTick timer to ensure the same duration of each stage  */
	start_EPD_timer();

	/* Do while total time of frames exceed stage time
	* Per frame */
	do {
		address_offset=0;
		frame_count=0;
		/* Per data line (vertical size) */
		for (y = 0; y < COG_parameters[EPD_type_index].vertical_size; y++) {
			/* Set charge pump voltage level reduce voltage shift */
			epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);

			k = COG_parameters[EPD_type_index].horizontal_size-1;
			if(_On_EPD_read_flash!=NULL) {
				_On_EPD_read_flash((new_image_address+address_offset),new_line,
				                   COG_parameters[EPD_type_index].horizontal_size);
				_On_EPD_read_flash((mark_image_address+address_offset),mark_line,
				                   COG_parameters[EPD_type_index].horizontal_size);
			}
			/** Per dot/pixel */
			for (x = 0; x < COG_parameters[EPD_type_index].horizontal_size; x++) {
				switch(stage_no) {
				case Stage1: /*!< black/white alternately */
					/** change the pixel(x) of marked line only if not 0xFF.
					    If 0xFF, send Nothing(01) means not change.
						The other pixels, send white(10)/black(11) alternately by frame_count. */
					if(mark_line[x]==0xFF) {
						data_line_odd[x]    = NOTHING; //Nothing(01), 01010101
						data_line_even[k--] = NOTHING;
					} else {
						if((frame_count%2)) {
							data_line_odd[x]    = 0xAA; //white(10), 10101010
							data_line_even[k--] = 0xAA;

						} else {
							data_line_odd[x]    = 0xFF; //black(11), 11111111
							data_line_even[k--] = 0xFF;
						}
					}
					break;

				case Stage2: /*!< new image */
					/** change the pixel(x) of marked line only if not 0xFF.
					    If 0xFF, send Nothing(01) means not change.
						The other pixels, send new image data. */
					if(mark_line[x]==0xFF) {
						data_line_odd[x]    = NOTHING;
						data_line_even[k--] = NOTHING;
					} else {
						temp_byte=new_line[x];
						/** Example to get Even and Odd data
						* +---------+----+----+----+----+----+----+----+----+
						* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
						* |temp_byte+----+----+----+----+----+----+----+----+
						* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
						* +---------+----+----+----+----+----+----+----+----+ */
						data_line_odd[x]    = ((temp_byte & 0x80) ? WHITE3  : BLACK3 )
						                      | ((temp_byte & 0x20) ? WHITE2  : BLACK2 )
						                      | ((temp_byte & 0x08) ? WHITE1  : BLACK1 )
						                      | ((temp_byte & 0x02) ? WHITE0  : BLACK0 );
						/** WHITE3 = 0x80 = 1000 0000, WHITE2 = 0x20 = 0010 0000
						 *  BLACK1 = 0x0C = 0000 1100, BLACK0 = 0x03 = 0000 0011
						 *  data_line_odd[] = 1000 0000 | 0010 0000 | 0000 1100 | 0000 0011 = 1010 1111 */
						data_line_even[k--] = ((temp_byte & 0x01) ? WHITE3  : BLACK3 )
						                      | ((temp_byte & 0x04) ? WHITE2  : BLACK2 )
						                      | ((temp_byte & 0x10) ? WHITE1  : BLACK1 )
						                      | ((temp_byte & 0x40) ? WHITE0  : BLACK0 );
						/** BLACK3 = 0xC0 = 1100 0000, WHITE2 = 0x20 = 0010 0000
						 *  WHITE1 = 0x08 = 0000 1000, BLACK0 = 0x03 = 0000 0011
						 *  data_line_even[] = 1100 0000 | 0010 0000 | 0000 1000 | 0000 0011 = 1110 1011
						 * +---------+----+----+----+----+----+----+----+----+
						 * |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
						 * |temp_byte+----+----+----+----+----+----+----+----+
						 * |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
						 * +---------+----+----+----+----+----+----+----+----+
						 * | Color   |  W |  B |  W |  W |  B |  W |  B |  B | W=White, B=Black, N=Nothing
						 * +---------+----+----+----+----+----+----+----+----+
						 * | Stage 2 |  W |  B |  W |  W |  B |  W |  B |  B |
						 * +---------+----+----+----+----+----+----+----+----+
						 * | Input   | 10 | 11 | 10 | 10 | 11 | 10 | 11 | 11 | W=10, B=11, N=01
						 * +---------+----+----+----+----+----+----+----+----+
						 * |Odd data | 10 |    | 10 |    | 11 |    | 11 |    | = 1010 1111
						 * +---------+----+----+----+----+----+----+----+----+
						 * |Even data|    | 11 |    | 10 |    | 10 |    | 11 | = 1110 1011
						 * +---------+----+----+----+----+----+----+----+----+ */
					}
					break;
				}
			}
			address_offset+=LINE_SIZE;
			/* Scan byte shift per data line */
			data_line_scan[(y>>2)]= SCAN_TABLE[(y%4)];

			/* For 1.44 inch EPD, the border uses the internal signal control byte. */
			if(EPD_type_index==EPD_144)
				COG_Line.line_data_by_size.line_data_for_144.border_byte=0x00;

			/* Sending data */
			epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8,
			          COG_parameters[EPD_type_index].data_line_size);

			/* Turn on Output Enable */
			epd_spi_send_byte (0x02, 0x2F);

			data_line_scan[(y>>2)]=0;
		}
		/* Count the frame time with offset */
		current_frame_time=(uint16_t)get_current_time_tick()+
		                   COG_parameters[EPD_type_index].frame_time_offset ;
		frame_count++;
	} while (stage_time>current_frame_time);

	/* Do while the SysTick timer fulfills the stage time */
	while(stage_time>get_current_time_tick());
	free(new_line);
	free(mark_line);
	/* Stop system timer */
	stop_EPD_timer();
}

/**
 * \brief Write image data from Flash memory to the EPD using partial update
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_address The previous image address
 * \param new_image_address The new (canvas) image address
 * \param mark_image_address The mark image address
 * \param On_EPD_read_flash Developer needs to create an external function to read flash
*/
void EPD_display_partialupdate (uint8_t EPD_type_index, long previous_image_address,
								long new_image_address,long mark_image_address,
								EPD_read_flash_handler On_EPD_read_flash) {
									
	_On_EPD_read_flash=On_EPD_read_flash;
	/** partial update uses two stages: black/white and new image */
	stage_handle_partial_update(EPD_type_index,previous_image_address,new_image_address,
	                            mark_image_address,Stage1);
	stage_handle_partial_update(EPD_type_index,previous_image_address,new_image_address,
	                            mark_image_address,Stage2);

	/** Power off COG Driver */
	EPD_power_off (EPD_type_index);
	/** Save image combines with ASCII text  */
	if (previous_image_address != new_image_address) {
		save_partial_image (EPD_type_index,previous_image_address,new_image_address,mark_image_address);
	}
}

#elif (defined COG_V230_G2)

//************************************************************************************//
//#include "COG/V230_G2/EPD_COG_process_V230_G2.c"
//************************************************************************************//

/**
* \file
*
* \brief The waveform driving processes and updating stages of G2 COG with V230 EPD
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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
**/

#include "EPD_COG_process.h"
#ifdef COG_V230_G2

#define ADDRESS_NULL		0xffffffff
//EPD Panel parameters
const struct COG_parameters_t COG_parameters[COUNT_OF_EPD_TYPE]  = {
	{
		// FOR 1.44"
		{0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00},
		0x03,
		(128/8),
		96,
		((((128+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.0"
		{0x00,0x00,0x00,0x00,0x01,0xFF,0xE0,0x00},
		0x03,
		(200/8),
		96,
		((((200+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.7"
		{0x00,0x00,0x00,0x7F,0xFF,0xFE,0x00,0x00},
		0x00,
		(264/8),
		176,
		((((264+176)*2)/8)+1),
		0,
		630
	}
};

//EPD Waveform parameters
 const struct EPD_WaveformTable_Struct E_Waveform[COUNT_OF_EPD_TYPE][2]  = {
		{// FOR 1.44"
			{//50 ¡Ù T ¡Ö 40
				4,				//stage1_frame1
				16,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				16,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ¡Ù T ¡Ö 10
				4,				//stage1_frame1
				16,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				16,				//stage3_block3
				2				//stage3_step3
			}
			
		},
		{// For 2.0"
			{//50 ¡Ù T ¡Ö 40
				4,				//stage1_frame1
				48,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				48,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ¡Ù T ¡Ö 10
				2,				//stage1_frame1
				48,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				2,				//stage3_frame3
				48,				//stage3_block3
				2				//stage3_step3
			}
		},
		{// For 2.7"
			{//50 ¡Ù T ¡Ö 40
				4,				//stage1_frame1
				22,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				22,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ¡Ù T ¡Ö 10
				2,				//stage1_frame1
				22,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				2,				//stage3_frame3
				22,				//stage3_block3
				2				//stage3_step3
			}
		},
	 
 };

const uint8_t   SCAN_TABLE[4] = {0xC0,0x30,0x0C,0x03};
	
static struct EPD_WaveformTable_Struct *action__Waveform_param;
static COG_line_data_packet_type COG_Line;
static EPD_read_flash_handler _On_EPD_read_flash=NULL;
static uint16_t current_frame_time;
static uint8_t  *data_line_even;
static uint8_t  *data_line_odd;
static uint8_t  *data_line_scan;
static uint8_t  *data_line_border_byte;

/**
* \brief According to EPD size and temperature to get stage_time
* \note Refer to COG document Section 5.3 for more details
*
* \param EPD_type_index The defined EPD size
*/
static void set_temperature_factor(uint8_t EPD_type_index) {
	int8_t temperature;
	temperature = get_temperature();	
        if (50 >= temperature  && temperature > 40){
			action__Waveform_param=&E_Waveform[EPD_type_index][0];
		}else if (40 >= temperature  && temperature > 10){
			action__Waveform_param=&E_Waveform[EPD_type_index][1];
		}else action__Waveform_param=&E_Waveform[EPD_type_index][1]; //Default
}

/**
* \brief Initialize the EPD hardware setting
*/
void EPD_init(void) {
	EPD_display_hardware_init();
	EPD_cs_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
}

/**
* \brief Select the EPD size to get line data array for driving COG
*
* \param EPD_type_index The defined EPD size
*/
void COG_driver_EPDtype_select(uint8_t EPD_type_index) {
	switch(EPD_type_index) {
		case EPD_144:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_144.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_144.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_144.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_144.border_byte;
		break;
		case EPD_200:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_200.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_200.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_200.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_200.border_byte;
		break;
		case EPD_270:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_270.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_270.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_270.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_270.border_byte;
		break;
	}
}

/**
* \brief Power on COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 3.
*/
void EPD_power_on (void) {	
	//epd_spi_init_2M();
	/* Initial state */
	EPD_Vcc_turn_on(); //Vcc and Vdd >= 2.7V	
	EPD_cs_high();
	EPD_border_high();
	EPD_rst_high();
	delay_ms(5);	
	EPD_rst_low();
	delay_ms(5);
	EPD_rst_high();
	delay_ms(5);
}


/**
* \brief Initialize COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 4.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_initialize_driver (uint8_t EPD_type_index) {
	
	uint16_t i;
	// Empty the Line buffer
	for (i = 0; i <= LINE_BUFFER_DATA_SIZE; i ++) {
		COG_Line.uint8[i] = 0x00;
	}
	// Determine the EPD size for driving COG
	COG_driver_EPDtype_select(EPD_type_index);

	// Sense temperature to determine Temperature Factor
	set_temperature_factor(EPD_type_index);
	i = 0;
	
	while (EPD_IsBusy()) {
		if((i++) >= 0x0FFF) return ERROR_BUSY;
	}
	
	//Check COG ID
	if((SPI_R(0x72,0x00) & 0x0f) !=0x02) return ERROR_COG_ID;

	//Disable OE
	epd_spi_send_byte(0x02,0x40);	

	//Check Breakage
	if((SPI_R(0x0F,0x00) & 0x80) != 0x80) return ERROR_BREAKAGE;
	
	//Power Saving Mode
 	epd_spi_send_byte(0x0B, 0x02);

	//Channel Select
	epd_spi_send (0x01, (uint8_t *)&COG_parameters[EPD_type_index].channel_select, 8);

	//High Power Mode Osc Setting
	epd_spi_send_byte(0x07,0xD1);

	//Power Setting
	epd_spi_send_byte(0x08,0x02);

	//Set Vcom level
	epd_spi_send_byte(0x09,0xC2);

	//Power Setting
	epd_spi_send_byte(0x04,0x03);

	//Driver latch on
	epd_spi_send_byte(0x03,0x01);

	//Driver latch off
	epd_spi_send_byte(0x03,0x00);

	delay_ms(5);

	//Chargepump Start
	i=0;
	do {
		//Start chargepump positive V
		//VGH & VDH on
		epd_spi_send_byte(0x05,0x01);

		delay_ms(240);

		//Start chargepump neg voltage
		//VGL & VDL on
		epd_spi_send_byte(0x05,0x03);

		delay_ms(40);

		//Set chargepump
		//Vcom_Driver to ON
		//Vcom_Driver on
		epd_spi_send_byte(0x05,0x0F);

		delay_ms(40);

		//Check DC/DC
		if((SPI_R(0x0F,0x00) & 0x40) != 0x00) break;	
		
	}while((i++) != 4);
	
	if(i>=4) 
	{
		//Output enable to disable
		epd_spi_send_byte(0x02,0x40);
		return ERROR_CHARGEPUMP;
	}
	else  return RES_OK;
}

/**
* \brief Initialize the parameters of Block type stage 
*
* \param EPD_type_index The defined EPD size
* \param EPD_V230_G2_Struct The Block type waveform structure
* \param block_size The width of Block size
* \param step_size The width of Step size
* \param frame_cycle The width of Step size
*/
void stage_init(uint8_t EPD_type_index,struct EPD_V230_G2_Struct *S_epd_v230,
				uint8_t block_size,uint8_t step_size,
				uint8_t frame_cycle)
{
	S_epd_v230->frame_y0 = 0;
	S_epd_v230->frame_y1 = 176;
	S_epd_v230->block_y0 = 0;
	S_epd_v230->block_y1 = 0;
	S_epd_v230->step_y0 = 0;
	S_epd_v230->step_y1 = 0;
	S_epd_v230->block_size = action__Waveform_param->stage1_block1;
	S_epd_v230->step_size =action__Waveform_param->stage1_step1;
	S_epd_v230->frame_cycle = action__Waveform_param->stage1_frame1;
	S_epd_v230->number_of_steps = (COG_parameters[EPD_type_index].vertical_size / S_epd_v230->step_size) + (action__Waveform_param->stage1_block1 / action__Waveform_param->stage1_step1) -1;
	
}

/**
* \brief For Frame type waveform to update all black/white pattern
*
* \param EPD_type_index The defined EPD size
* \param bwdata Black or White color to whole screen
* \param work_time The working time
*/
static inline void same_data_frame (uint8_t EPD_type_index, uint8_t bwdata, uint32_t work_time) {
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]=bwdata;
		data_line_odd[i]=bwdata;
	}
	start_EPD_timer();
	do 
	{	
		for (i = 0; i < COG_parameters[EPD_type_index].vertical_size; i++) {
			
			/* Scan byte shift per data line */
			data_line_scan[(i>>2)]=SCAN_TABLE[(i%4)];
			
			/* Sending data */
			epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
		 
			/* Turn on Output Enable */
			epd_spi_send_byte (0x02, 0x07);
		
			data_line_scan[(i>>2)]=0;
			
		}
	} while (get_current_time_tick()<work_time);
		/* Stop system timer */
		stop_EPD_timer();
}

/**
* \brief Write nothing Line to COG
* \note A line whose all Scan Bytes are 0x00
*
* \param EPD_type_index The defined EPD size
*/
void nothing_line(uint8_t EPD_type_index) {
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]	=	NOTHING;
		data_line_odd[i]	=	NOTHING;
	}
}


/**
* \brief Get line data of Stage 1 and 3
*
* \note
* - One dot/pixel is comprised of 2 bits which are White(10), Black(11) or Nothing(01).
*   The image data bytes must be divided into Odd and Even bytes.
* - The COG driver uses a buffer to write one line of data (FIFO) - interlaced
*   It's different order from COG_G1
*   Odd byte {D(199,y),D(197,y), D(195,y), D(193,y)}, ... ,{D(7,y),D(5,y),D(3,y), D(1,y)}
*   Scan byte {S(96), S(95)...}
*   Odd byte  {D(2,y),D(4,y), D(6,y), D(8,y)}, ... ,{D(194,y),D(196,y),D(198,y), D(200,y)}
* - For more details on the driving stages, please refer to the COG G2 document Section 5.
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of memory that stores image that will send to COG
* \param stage_no The assigned stage number that will proceed
*/
void read_line_data_handle(uint8_t EPD_type_index,uint8_t *image_prt,uint8_t stage_no)
{
	int16_t x,y,k;
	uint8_t	temp_byte; // Temporary storage for image data check
	k=COG_parameters[EPD_type_index].horizontal_size-1;	
	for (x =0 ; x < COG_parameters[EPD_type_index].horizontal_size ; x++) {
				temp_byte = *image_prt++;				
				switch(stage_no) {
					case Stage1: // Inverse image
					/* Example at stage 1 to get Even and Odd data. It's different order from G1.
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+ */
					data_line_odd[x]       = ((temp_byte & 0x40) ? BLACK3  : WHITE3); // WHITE3 = 0x80 = 1000 0000
					data_line_odd[x]      |= ((temp_byte & 0x10) ? BLACK2  : WHITE2); // BLACK2 = 0x30 = 0011 0000
					data_line_odd[x]      |= ((temp_byte & 0x04) ? BLACK1  : WHITE1); // BLACK1 = 0x0C = 0000 1100
					data_line_odd[x]	  |= ((temp_byte & 0x01) ? BLACK0  : WHITE0); // WHITE0 = 0x02 = 0000 0010
					/* data_line_odd[x] = 1000 0000 | 0011 0000 | 0000 1100 | 0000 0010 = 1011 1110 ==> 1011 1110
					* See Even data row at the table below*/
					
					data_line_even[k]    = ((temp_byte & 0x80) ? BLACK0  : WHITE0); // BLACK0 = 0x03 = 0000 0011
					data_line_even[k]   |= ((temp_byte & 0x20) ? BLACK1  : WHITE1); // BLACK1 = 0x0C = 0000 1100
					data_line_even[k]   |= ((temp_byte & 0x08) ? BLACK2  : WHITE2); // WHITE2 = 0x20 = 0010 0000
					data_line_even[k--] |= ((temp_byte & 0x02) ? BLACK3  : WHITE3); // WHITE3 = 0x80 = 1000 0000
					/* data_line_even[k] = 0000 0011 | 0000 1100 | 0010 0000 | 1000 0000 = 1010 1111 ==> 1111 1010
					* See Odd data row at the table below
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+
					* | Color   |  W |  B |  W |  W |  B |  W |  B |  B | W=White, B=Black, N=Nothing
					* +---------+----+----+----+----+----+----+----+----+
					* | Stage 1 |  B |  W |  B |  B |  W |  B |  W |  W | Inverse
					* +---------+----+----+----+----+----+----+----+----+
					* | Input   | 11 | 10 | 11 | 11 | 10 | 11 | 10 | 10 | W=10, B=11, N=01
					* +---------+----+----+----+----+----+----+----+----+
					* |Even data| 11 |    | 11 |    | 10 |    | 10 |    | = 1111 1010
					* +---------+----+----+----+----+----+----+----+----+
					* |Odd data |    | 10 |    | 11 |    | 11 |    | 10 | = 1011 1110
					* +---------+----+----+----+----+----+----+----+----+ */
					break;				
					case Stage3: // New image
						data_line_odd[x]		 = ((temp_byte & 0x40) ? WHITE3  : BLACK3 );
						data_line_odd[x]		|= ((temp_byte & 0x10) ? WHITE2  : BLACK2 );
						data_line_odd[x]		|= ((temp_byte & 0x04) ? WHITE1  : BLACK1 );
						data_line_odd[x]		|= ((temp_byte & 0x01) ? WHITE0  : BLACK0 );

						data_line_even[k]		 = ((temp_byte & 0x80) ? WHITE0  : BLACK0 );
						data_line_even[k]		|= ((temp_byte & 0x20) ? WHITE1  : BLACK1 );
						data_line_even[k]		|= ((temp_byte & 0x08) ? WHITE2  : BLACK2 );
						data_line_even[k--]		|= ((temp_byte & 0x02) ? WHITE3  : BLACK3 );
					break;
				}
		}	
}


/**
* \brief The base function to handle the driving stages for Frame and Block type
*
* \note
* - There are 3 stages to complete an image update on COG_V230_G2 type EPD.
* - For more details on the driving stages, please refer to the COG G2 document Section 5.4
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of image array that stores image that will send to COG
* \param image_data_address The address of memory that stores image
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
void stage_handle_Base(uint8_t EPD_type_index,uint8_t *image_prt,long image_data_address,
						uint8_t stage_no,uint8_t lineoffset)
{	
	struct EPD_V230_G2_Struct S_epd_v230;
	uint16_t cycle,m,i; //m=number of steps
	uint8_t isLastframe = 0;	//If it is the last frame to send Nothing at the fist scan line
	uint8_t isLastBlock=0;		//If the beginning line of block is in active range of EPD
	//uint8_t isScanOn=0;
	int16_t scanline_no=0;
	uint8_t *action_block_prt;
	long action_block_address;
	uint32_t line_time=8;
	uint8_t byte_array[LINE_BUFFER_DATA_SIZE];
	
	/** Stage 2: BLACK/WHITE image, Frame type */
	if(stage_no==Stage2)
	{
		for(i=0;i<action__Waveform_param->stage2_cycle;i++)
		{
			same_data_frame (EPD_type_index,ALL_BLACK,action__Waveform_param->stage2_t1);
			same_data_frame (EPD_type_index,ALL_WHITE,action__Waveform_param->stage2_t2);
		}
		return;
	}
	/** Stage 1 & 3, Block type */
	// The frame/block/step of Stage1 and Stage3 are default the same.
	stage_init(EPD_type_index,
				&S_epd_v230,
				action__Waveform_param->stage1_block1,
				action__Waveform_param->stage1_step1,
				action__Waveform_param->stage1_frame1);
	 /* Repeat number of frames */
   	 for (cycle = 0; cycle < (S_epd_v230.frame_cycle ); cycle++)
   	 {
	     if (cycle == (S_epd_v230.frame_cycle - 1)) isLastframe = 1;
		 
		 isLastBlock = 0;
		 
		 /* Move number of steps */
	   	 for (m = 0; m < S_epd_v230.number_of_steps; m++)	 
	   	 {
		   	 S_epd_v230.step_y0 = (S_epd_v230.step_size * m) + S_epd_v230.frame_y0;
		   	 S_epd_v230.step_y1 = (S_epd_v230.step_size * (m + 1)) + S_epd_v230.frame_y0;
		   	 S_epd_v230.block_y0 = S_epd_v230.step_y1 - S_epd_v230.block_size;
		   	 S_epd_v230.block_y1 = S_epd_v230.step_y1;
			
			/* reset block_y0=frame_y0 if block is not in active range of EPD */
		   	 if (S_epd_v230.block_y0 < S_epd_v230.frame_y0) S_epd_v230.block_y0 = S_epd_v230.frame_y0;
			
			/* if the beginning line of block is in active range of EPD */
			 if (S_epd_v230.block_y1 == S_epd_v230.block_size) isLastBlock = 1;
			 	
			 if(image_prt!=NULL)
			 {
				 action_block_prt=(image_prt+(int)(S_epd_v230.block_y0*lineoffset));	
			 }
			 else if(_On_EPD_read_flash!=NULL)	//Read line data in range of block, read first
			 {
				action_block_address=image_data_address+(int)(S_epd_v230.block_y0*lineoffset);
				_On_EPD_read_flash(action_block_address,(uint8_t *)&byte_array,
									COG_parameters[EPD_type_index].horizontal_size);
				action_block_prt=&byte_array;
			 }
			
			/* Update line data */
		   	 for (i = S_epd_v230.block_y0; i < S_epd_v230.block_y1; i++)
		   	 {
			     if (i >= COG_parameters[EPD_type_index].vertical_size) break;
				 if (  isLastBlock &&(i < (S_epd_v230.step_size + S_epd_v230.block_y0)))//isLastframe &&
				  {
					  nothing_line(EPD_type_index);					
				  }
				  else	 
				  {			  					 
					  read_line_data_handle(EPD_type_index,action_block_prt,stage_no);					
				  }
			   		
					if(_On_EPD_read_flash!=NULL)	//Read line data in range of block
					{
						action_block_address +lineoffset;
						_On_EPD_read_flash(action_block_address,(uint8_t *)&byte_array,
						COG_parameters[EPD_type_index].horizontal_size);
						action_block_prt=&byte_array;
					}
					else 	action_block_prt+=lineoffset;
					
				    scanline_no= (COG_parameters[EPD_type_index].vertical_size-1)-i;
					
				   	/* Scan byte shift per data line */
				   	data_line_scan[(scanline_no>>2)] = SCAN_TABLE[(scanline_no%4)];
				   
				   	/*  the border uses the internal signal control byte. */
				   	*data_line_border_byte=0x00;
					   
					/* Sending data */
					epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8,
				    COG_parameters[EPD_type_index].data_line_size);
					
					 
					/* Turn on Output Enable */
					epd_spi_send_byte (0x02, 0x07);
					   
					data_line_scan[(scanline_no>>2)]=0;
		   	 }
	   	 }
   	 }		
}

/**
* \brief The driving stages from image array (image_data.h) to COG
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of image array that stores image that will send to COG
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
void stage_handle(uint8_t EPD_type_index,uint8_t *image_prt,uint8_t stage_no,uint8_t lineoffset)
{
	stage_handle_Base(EPD_type_index,image_prt,ADDRESS_NULL,stage_no,lineoffset);	
}

/**
* \brief The driving stages from memory to COG
*
* \note
* - This function is additional added here for developer if the image data
*   is stored in Flash memory.
*
* \param EPD_type_index The defined EPD size
* \param image_data_address The address of flash memory that stores image
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
static void stage_handle_ex(uint8_t EPD_type_index,long image_data_address,uint8_t stage_no,uint8_t lineoffset) {
	stage_handle_Base(EPD_type_index,NULL,image_data_address,stage_no,lineoffset);	
}

/**
* \brief Write image data from memory array (image_data.h) to the EPD
*
* \param EPD_type_index The defined EPD size
* \param previous_image_ptr The pointer of memory that stores previous image
* \param new_image_ptr The pointer of memory that stores new image
*/
void EPD_display_from_array_prt (uint8_t EPD_type_index, uint8_t *previous_image_ptr,
		uint8_t *new_image_ptr) {	
	_On_EPD_read_flash=NULL;		
	stage_handle(EPD_type_index,new_image_ptr,Stage1,COG_parameters[EPD_type_index].horizontal_size);	
	stage_handle(EPD_type_index,new_image_ptr,Stage2,COG_parameters[EPD_type_index].horizontal_size);	
	stage_handle(EPD_type_index,new_image_ptr,Stage3,COG_parameters[EPD_type_index].horizontal_size);	
}

/**
* \brief Write image data from Flash memory to the EPD
* \note This function is additional added here for developer if the image data
* is stored in Flash.
*
* \param EPD_type_index The defined EPD size
* \param previous_image_flash_address The start address of memory that stores previous image
* \param new_image_flash_address The start address of memory that stores new image
* \param On_EPD_read_flash Developer needs to create an external function to read flash
*/
void EPD_display_from_flash_prt (uint8_t EPD_type_index, long previous_image_flash_address,
    long new_image_flash_address,EPD_read_flash_handler On_EPD_read_flash) {
		
	uint8_t line_len=LINE_SIZE;
	if(line_len==0) line_len=COG_parameters[EPD_type_index].horizontal_size;
		
	_On_EPD_read_flash=On_EPD_read_flash;	
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage1,line_len);
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage2,line_len);
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage3,line_len);	
}


/**
* \brief Write Dummy Line to COG
* \note A line whose all Scan Bytes are 0x00
*
* \param EPD_type_index The defined EPD size
*/
static inline void dummy_line(uint8_t EPD_type_index) {
	uint8_t	i;
	for (i = 0; i < (COG_parameters[EPD_type_index].vertical_size/8); i++) {
		switch(EPD_type_index) {
			case EPD_144:
			COG_Line.line_data_by_size.line_data_for_144.scan[i]=0x00;
			break;
			case EPD_200:
			COG_Line.line_data_by_size.line_data_for_200.scan[i]=0x00;
			break;
			case EPD_270:
			COG_Line.line_data_by_size.line_data_for_270.scan[i]=0x00;
			break;
		}
	}
	/* Set charge pump voltage level reduce voltage shift */
	epd_spi_send_byte (0x04, COG_parameters[EPD_type_index].voltage_level);
	
	/* Sending data */
	epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);

	/* Turn on Output Enable */
	epd_spi_send_byte (0x02, 0x07);
}


/**
* \brief Write Border(Input) Dummy Line
* \note Set Border byte 0xFF to write Black and set 0xAA to write White
*
* \param EPD_type_index The defined EPD size
*/
static void border_dummy_line(uint8_t EPD_type_index)
{
	uint16_t	i;
	for (i = 0; i < COG_parameters[EPD_type_index].data_line_size; i++)
	{
		COG_Line.uint8[i] = 0x00;
	}
	
	*data_line_border_byte=BORDER_BYTE_B;
	//Write a Border(B) Dummy Line
	epd_spi_send (0x0a, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
	//Turn on OE
	epd_spi_send_byte (0x02, 0x07);
	
	sys_delay_ms(40);
	
	*data_line_border_byte=BORDER_BYTE_W;
	//Write a Borde(B) Dummy Line
	epd_spi_send (0x0a, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
	//Turn on OE
	epd_spi_send_byte (0x02, 0x07);

	sys_delay_ms(200);
	
	
}


/**
* \brief Power Off COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 6.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_power_off (uint8_t EPD_type_index) {

	if(EPD_type_index==EPD_144 || EPD_type_index==EPD_200) 	{
		border_dummy_line(EPD_type_index);
		dummy_line(EPD_type_index);
	}

	delay_ms (25);
	if(EPD_type_index==EPD_270)	{
		EPD_border_low();
		delay_ms (200);
		EPD_border_high();
	}
	//Check DC/DC
	if((SPI_R(0x0F,0x00) & 0x40) == 0x00) return ERROR_DC;
	// Latch reset turn on
	epd_spi_send_byte (0x03, 0x01);
	
	// Output enable off
	epd_spi_send_byte (0x02, 0x05);
	
	// Power off charge pump Vcom
	epd_spi_send_byte (0x05, 0x0E);
	
	// Power off charge negative voltage
	epd_spi_send_byte (0x05, 0x02);
	
	// Discharge
	epd_spi_send_byte (0x04, 0x0C);
	delay_ms (120);
	
	// Turn off all charge pumps
	epd_spi_send_byte (0x05, 0x00);
	
	// Turn off osc
	epd_spi_send_byte (0x07, 0x0D);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0x50);
	delay_ms (40);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0xA0);
	delay_ms (40);
	
	// Discharge internal
	epd_spi_send_byte (0x04, 0x00);
	
	// Set power and signals = 0
	EPD_rst_low ();
	epd_spi_detach ();
	EPD_cs_low ();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	
	// External discharge = 1
	EPD_discharge_high ();
	delay_ms (150);
	
	// External discharge = 0
	EPD_discharge_low ();
	
	return RES_OK;
}

/**
* \brief Power Off COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 6.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_power_off2 (uint8_t EPD_type_index) {
	uint8_t y;		

	if(EPD_type_index==EPD_144 || EPD_type_index==EPD_200) 	{
		border_dummy_line(EPD_type_index);
		dummy_line(EPD_type_index);
	}

	delay_ms (25);
	if(EPD_type_index==EPD_270)	{
		EPD_border_low();
		delay_ms (200);
		EPD_border_high();
	}

	//Check DC/DC
	if((SPI_R(0x0F,0x00) & 0x40) == 0x00) return ERROR_DC;
	
	//Turn on Latch Reset
	epd_spi_send_byte (0x03, 0x01);
	//Turn off OE
	epd_spi_send_byte (0x02, 0x05);
	//Power off charge pump Vcom
	epd_spi_send_byte (0x05, 0x03);
	//Power off charge pump neg voltage
	epd_spi_send_byte (0x05, 0x01);
	delay_ms(250);
	//Turn off all charge pump 
	epd_spi_send_byte (0x05, 0x00);
	delay_ms(300);
	//Turn off OSC
	epd_spi_send_byte (0x07, 0x05);
	
	epd_spi_detach ();
	EPD_cs_low();
	EPD_rst_low();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	delay_ms (10);
		
	for(y=0;y<10;y++)
	{
		EPD_discharge_high ();		
		delay_ms (10);		
		EPD_discharge_low ();	
		delay_ms (10);	
	}
	return RES_OK;
}

#endif

#else
#error "ERROR: The EPD's COG type is not defined."
#endif
