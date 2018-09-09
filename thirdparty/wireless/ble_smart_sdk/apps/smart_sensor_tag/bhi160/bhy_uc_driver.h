/*
****************************************************************************
* Copyright (C) 2015 - 2018 Bosch Sensortec GmbH
*
* Usage: APIs and Drivers for BHI160
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/


#ifndef BHY_UC_DRIVER_H_
#define BHY_UC_DRIVER_H_

#include "BHy_support.h"
#include "bhy_uc_driver_types.h"

/****************************************************************************/
/*							Driver Functions								*/
/****************************************************************************/


/* initializes the driver, the API and loads the ram patch into the sensor	*/
BHY_RETURN_FUNCTION_TYPE bhy_driver_init
	(const u8 *bhy_fw_data, const u32 bhy_fw_len);

/* this functions enables the selected virtual sensor						*/
BHY_RETURN_FUNCTION_TYPE bhy_enable_virtual_sensor
	(bhy_virtual_sensor_t sensor_id, u8 wakeup_status, u16 sample_rate,
	 u16 max_report_latency_ms, u8 flush_sensor, u16 change_sensitivity,
	 u16 dynamic_range);

/* this functions disables the selected virtual sensor						*/
BHY_RETURN_FUNCTION_TYPE bhy_disable_virtual_sensor
	(bhy_virtual_sensor_t sensor_id, u8 wakeup_status);

/* retrieves the fifo data. it needs a buffer of at least 51 bytes to work	*/
/* it outputs the data into the variable buffer. the number of bytes read	*/
/* into bytes_read  and the bytes remaining in the fifo into bytes_left.	*/
/* Setting BST_APPLICATION_BOARD to 1 will limit the size to 51 bytes all	*/
/* the time. Arguments :													*/
/* buffer			Pointer to the buffer to use for the fifo readout.		*/
/* buffer_size		Size of the buffer to work with. Needs to be 51 bytes+	*/
/* bytes_read		Pointer to output the number of bytes actually read from*/
/*					the fifo.												*/
/* bytes_left		Pointer to output the number of bytes still in the fifo.*/
/*					This function automatically keeps track of the current	*/
/*					fifo readout progress.
*/
BHY_RETURN_FUNCTION_TYPE bhy_read_fifo
	(u8 * buffer, const u16 buffer_size, u16 * bytes_read, u16 * bytes_left);

/* This function parses the next fifo packet and return it into a generic 	*/
/* data structure while telling you what the data type is so you can 		*/
/* retrieve it. Here are the parameters:									*/
/* fifo_buffer			pointer to the fifo byte that is the start of a 	*/
/*						packet. This pointer will be automatically 			*/
/*						incremented so you can call this function in a loop	*/
/* fifo_buffer_length	pointer to the amount of data left in the 			*/
/*						fifo_buffer. This data will be automatically		*/
/*						decremented so you can call this function in a loop	*/
/* fifo_data_output		buffer in which to place the data					*/
/* fifo_data_type		data type output									*/
BHY_RETURN_FUNCTION_TYPE bhy_parse_next_fifo_packet
	(u8 **fifo_buffer, u16 *fifo_buffer_length,
	bhy_data_generic_t * fifo_data_output, bhy_data_type_t * fifo_data_type);

/* This function will detect the timestamp packet accordingly and update	*/
/* either the MSW or the LSW of the system timestamp. Arguments :			*/
/* timestamp_packet		The timestamp packet processed by the parse_next_	*/
/*						fifo_packet, properly typecasted					*/
/* system_timestamp		Pointer to a 32bit variable holding the system		*/
/*						timestamp in 1/32000th seconds.	it will wrap around	*/
/*						every 36 hours.										*/
BHY_RETURN_FUNCTION_TYPE bhy_update_system_timestamp(bhy_data_scalar_u16_t *timestamp_packet,
u32 * system_timestamp);

/* This function writes arbitrary data to an arbitrary parameter page. To be*/
/* used carefully since it can override system configurations. Refer to the */
/* datasheet for free to use parameter pages. Here are the arguments:		*/
/* page					Page number. Valid range 0 to 15.					*/
/* parameter			Parameter number. Valid range 0 to 127.				*/
/* data					Pointer to the data source to write to.				*/
/* length				Number of bytes to write. Valid range 1 to 8.		*/
BHY_RETURN_FUNCTION_TYPE bhy_write_parameter_page(u8 page, u8 parameter, u8 * data, u8 length);

/* This function reads arbitrary data to an arbitrary parameter page. To be*/
/* used carefully since it can override system configurations. Refer to the */
/* datasheet for free to use parameter pages. Here are the arguments:		*/
/* page					Page number. Valid range 0 to 15.					*/
/* parameter			Parameter number. Valid range 0 to 127.				*/
/* data					Pointer to the data source to write to.				*/
/* length				Number of bytes to read. Valid range 1 to 16.		*/
BHY_RETURN_FUNCTION_TYPE bhy_read_parameter_page(u8 page, u8 parameter, u8 * data, u8 length);

#if BHY_DEBUG

	/* This function outputs the debug data to function pointer. You need to 	*/
	/* provide a function that takes as argument a zero-terminated string and	*/
	/* prints it 																*/
	void bhy_print_debug_packet
	(bhy_data_debug_t *packet, void (*debug_print_ptr)(const u8 *));

#endif


#if BHY_CALLBACK_MODE

	/* These functions will install the callback and return an error code if	*/
	/* there is already a callback installed									*/
	BHY_RETURN_FUNCTION_TYPE bhy_install_sensor_callback ( bhy_virtual_sensor_t sensor_id, u8 wakeup_status, void (*sensor_callback)(bhy_data_generic_t *, bhy_virtual_sensor_t) );
	BHY_RETURN_FUNCTION_TYPE bhy_install_timestamp_callback ( u8 wakeup_status, void (*timestamp_callback)(bhy_data_scalar_u16_t *) );
	BHY_RETURN_FUNCTION_TYPE bhy_install_meta_event_callback ( bhy_meta_event_type_t meta_event_id, void (*meta_event_callback)(bhy_data_meta_event_t *, bhy_meta_event_type_t) );



	/* These functions will uninstall the callback and return an error code if	*/
	/* there was no callback installed											*/
	BHY_RETURN_FUNCTION_TYPE bhy_uninstall_sensor_callback ( bhy_virtual_sensor_t sensor_id, u8 wakeup_status );
	BHY_RETURN_FUNCTION_TYPE bhy_uninstall_timestamp_callback ( u8 wakeup_status );
	BHY_RETURN_FUNCTION_TYPE bhy_uninstall_meta_event_callback ( bhy_meta_event_type_t meta_event_id );

#endif










#endif /* BHY_UC_DRIVER_H_ */