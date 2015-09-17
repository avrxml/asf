/**
* \file
*
* \brief The definition of COG driving data and process
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

#ifndef DISPLAY_COG_PROCESS__H_INCLUDED
#define DISPLAY_COG_PROCESS__H_INCLUDED

#include "Pervasive_Displays_small_EPD.h"
/**
 * \brief The definition for driving stage to compare with for getting Odd and Even data  */
#define BLACK0   (uint8_t)(0x03) /**< getting bit1 or bit0 as black color(11) */
#define BLACK1   (uint8_t)(0x0C) /**< getting bit3 or bit2 as black color(11) */
#define BLACK2   (uint8_t)(0x30) /**< getting bit5 or bit4 as black color(11) */
#define BLACK3   (uint8_t)(0xC0) /**< getting bit7 or bit6 as black color(11) */
#define WHITE0   (uint8_t)(0x02) /**< getting bit1 or bit0 as white color(10) */
#define WHITE1   (uint8_t)(0x08) /**< getting bit3 or bit2 as white color(10) */
#define WHITE2   (uint8_t)(0x20) /**< getting bit5 or bit4 as white color(10) */
#define WHITE3   (uint8_t)(0x80) /**< getting bit7 or bit6 as white color(10) */
#define NOTHING0 (uint8_t)(0x01) /**< getting bit1 or bit0 as nothing input(01) */
#define NOTHING1 (uint8_t)(0x04) /**< getting bit3 or bit2 as nothing input(01) */
#define NOTHING2 (uint8_t)(0x10) /**< getting bit5 or bit4 as nothing input(01) */
#define NOTHING3 (uint8_t)(0x40) /**< getting bit7 or bit6 as nothing input(01) */
#define NOTHING  (uint8_t)(0x00) /**< sending Nothing frame, 01=Nothing, 0101=0x5 */


#define ALL_BLACK	 	 (uint8_t)(0xFF)
#define ALL_WHITE		 (uint8_t)(0xAA)
#define BORDER_BYTE_B    (uint8_t)(0xFF)
#define BORDER_BYTE_W    (uint8_t)(0xAA)
#define ERROR_BUSY       (uint8_t)(0xF0)
#define ERROR_COG_ID     (uint8_t)(0xF1)
#define ERROR_BREAKAGE   (uint8_t)(0xF2)
#define ERROR_DC         (uint8_t)(0xF3)
#define ERROR_CHARGEPUMP (uint8_t)(0xF4)
#define RES_OK           (uint8_t)(0x00)

/**
 * \brief The COG Driver uses a buffer to update the EPD line by line.
   \note Use the 2.7" maximum data(66)+scan(44)+dummy(1) bytes as line buffer size=111.*/
#define LINE_BUFFER_DATA_SIZE 111

/**
 * \brief Support 1.44", 2" and 2.7" three type EPD currently */
#define COUNT_OF_EPD_TYPE 3

/**
 * \brief Four driving stages */
enum Stage {
	Stage1, /**< Inverse previous image */
	Stage2, /**< White */
	Stage3, /**< Inverse new image */
	Stage4  /**< New image */
};

#if (defined COG_V110_G1)
/** 
 * \brief Line data structure of 1.44 inch EPD
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t border_byte;  /**< Internal border_control, for 1.44" EPD only */
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
} ;

/** 
 * \brief Line data structure of 2 inch EPD
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
	uint8_t dummy_data;	/**< dummy byte 0x00 */
} ;

/** 
 * \brief Line data structure of 2.7 inch EPD
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
	uint8_t dummy_data;	/**< dummy byte 0x00 */
} ;

#elif (defined COG_V110_G2) 

/** 
 * \brief Line data structure of 1.44 inch EPD with G2 COG
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
	uint8_t border_byte; /**< Internal border_control*/
} ;

/** 
 * \brief Line data structure of 2 inch EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
} ;
    
/** 
 * \brief Line data structure of 2.7 inch EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
} ;

#elif (defined COG_V230_G2)
/** 
 * \brief Line data structure of 1.44 inch V230 EPD with G2 COG
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
	uint8_t border_byte; /**< Internal border_control*/
} ;

/** 
 * \brief Line data structure of 2 inch V230 EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
} ;
    
/** 
 * \brief Line data structure of 2.7 inch V230 EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
} ;

struct EPD_WaveformTable_Struct
{
	uint8_t stage1_frame1;
	uint8_t stage1_block1;
	uint8_t stage1_step1;
	
	uint16_t stage2_t1;
	uint16_t stage2_t2;
	uint8_t stage2_cycle;
	
	uint8_t stage3_frame3;
	uint8_t stage3_block3;
	uint8_t stage3_step3;
};
struct EPD_V230_G2_Struct
{
	 int16_t frame_y0;
	 int16_t frame_y1;
	 int16_t block_y0;
	 int16_t block_y1;
	 int16_t block_size;
	 int16_t step_size;
	 int16_t frame_cycle;
	 int16_t step_y0;
	 int16_t step_y1;
	 int16_t number_of_steps;
};
#else
#error "ERROR: The EPD's COG type is not defined."
#endif

/** 
 * \brief Packet structure of a line data */
typedef union {
	union {
		struct COG_144_line_data_t line_data_for_144; /**< line data structure of 1.44" EPD */
		struct COG_200_line_data_t line_data_for_200; /**< line data structure of 2" EPD */
		struct COG_270_line_data_t line_data_for_270; /**< line data structure of 2.7" EPD */
	} line_data_by_size; /**< the line data of specific EPD size */
	uint8_t uint8[LINE_BUFFER_DATA_SIZE]; /**< the maximum line buffer data size as length */
} COG_line_data_packet_type;

/** 
 * \brief Define the COG driver's parameters */
struct COG_parameters_t {
	uint8_t   channel_select[8]; /**< the SPI register data of Channel Select */
	uint8_t   voltage_level;     /**< the SPI register data of Voltage Level */
	uint16_t  horizontal_size;   /**< the bytes of width of EPD */
	uint16_t  vertical_size;     /**< the bytes of height of EPD */
	uint8_t   data_line_size;    /**< Data + Scan + Dummy bytes */
	uint16_t  frame_time_offset; /**< the rest of frame time in a stage */
	uint16_t  stage_time;        /**< defined stage time */
} ;

extern const struct COG_parameters_t  COG_parameters[COUNT_OF_EPD_TYPE];
extern const uint8_t   SCAN_TABLE[4];
void EPD_init(void);
void EPD_power_on (void);
uint8_t EPD_initialize_driver (uint8_t EPD_type_index);
void EPD_display_from_array_prt (uint8_t EPD_type_index, uint8_t *previous_image_ptr,
uint8_t *new_image_ptr);
void EPD_display_from_flash_prt (uint8_t EPD_type_index, long previous_image_flash_address,
	long new_image_flash_address,EPD_read_flash_handler On_EPD_read_flash);
uint8_t EPD_power_off (uint8_t EPD_type_index);
void COG_driver_EPDtype_select(uint8_t EPD_type_index);

void stage_init (uint8_t EPD_type_index,struct EPD_V230_G2_Struct *S_epd_v230, 
	uint8_t block_size,uint8_t step_size, uint8_t frame_cycle);
	
void nothing_line(uint8_t EPD_type_index);

void read_line_data_handle(uint8_t EPD_type_index,uint8_t *image_prt,uint8_t stage_no); 

void EPD_timer_handler(void);	

#endif 	//DISPLAY_COG_PROCESS__H_INCLUDED
