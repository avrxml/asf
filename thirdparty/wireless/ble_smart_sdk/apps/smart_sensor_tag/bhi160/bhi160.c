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


/*- Includes ---------------------------------------------------------------*/
#include <asf.h>
#include "sensor_tag.h"
#include "bhi160/bhi160_api/BHy_support.h"
#include "bhi160/bhy_uc_driver_types.h"
#include "bhi160/bhy_uc_driver.h"
#include "bhi160.h"

/**
 * Macros
 */
#define ARRAYSIZE 250	//should be greater or equal to 69 bytes, page size (50) + maximum packet size(18) + 1
#define SAMPLE_DATA_RATE 200
#define ACC_GRAVITY_THRESHOLD_Z 700 // Do not modify these threshold values!
#define ACC_GRAVITY_THRESHOLD_Y (ACC_GRAVITY_THRESHOLD_Z + 100) // Do not modify these threshold values!
#define ACC_GRAVITY_THRESHOLD_X (ACC_GRAVITY_THRESHOLD_Z + 100) // Do not modify these threshold values!

/**
 * Prototypes
 */
void sensors_callback(bhy_data_generic_t * sensor_data, bhy_virtual_sensor_t sensor_id);

/**
 * Variables
 */
//Gyro data to BLE
gyro_positions_t gyro_data;
//Accelerometer data to BLE
accelero_positions_t acc_data;
//Quaternion data to BLE
device_rotation_vector_t quaternion_data;

uint8_t acc_sens_stat = false;
uint8_t gyro_sens_stat = false;
uint8_t rot_sens_stat = false;
uint8_t gra_sens_stat = false;
uint8_t stp_sens_stat = false;

s16 gra_x_data;
s16 gra_y_data;
s16 gra_z_data;
s16 gra_x_data_prev;
s16 gra_y_data_prev;
s16 gra_z_data_prev;
uint8_t step_detected = false;
uint16_t device_drop_detected = false;

/*! \brief process motion sensor data
 *
 */
void process_motion_sensor_data(void)
{
	u8 array[ARRAYSIZE];
	u8 *fifoptr;
	u8 bytes_left_in_fifo=0;
	u16 bytes_remaining;
	u16 bytes_read;
	bhy_data_generic_t	fifo_packet;
	bhy_data_type_t		packet_type;
	BHY_RETURN_FUNCTION_TYPE result;


	if(acc_sens_stat || gyro_sens_stat || rot_sens_stat ||	gra_sens_stat || stp_sens_stat)
	{
		//while (!port_pin_get_input_level(BHI_SENSOR_INT_PIN) /*&& !bytes_remaining*/) ;
		if(bhy_read_fifo(array+bytes_left_in_fifo, ARRAYSIZE-bytes_left_in_fifo, &bytes_read, &bytes_remaining) == BHY_SUCCESS)
		{
			bytes_read += bytes_left_in_fifo;

			fifoptr = array;
			packet_type = BHY_DATA_TYPE_PADDING;

			do {
				/* this function will call callbacks that are registered */
				result = bhy_parse_next_fifo_packet( &fifoptr, &bytes_read, &fifo_packet, &packet_type );
				/* the logic here is that if doing a partial parsing of the fifo, then we should not parse	*/
				/* the last 18 bytes (max length of a packet) so that we don't try to parse an incomplete	*/
				/* packet																					*/
			} while ( (result == BHY_SUCCESS) && (bytes_read > (bytes_remaining ? 18 : 0)) );
			bytes_left_in_fifo = 0;

			if (bytes_remaining) {
				/* shifts the remaining bytes to the beginning of the buffer */
				while (bytes_left_in_fifo < bytes_read)
				array[bytes_left_in_fifo++] = *(fifoptr++);
			}
		}
			
	}
}



/*! \brief bhi sensor callback
 *
 */
void sensors_callback(bhy_data_generic_t * sensor_data, bhy_virtual_sensor_t sensor_id)
{
	s16 tempx,tempy, tempz;
	switch (sensor_id)
	{
		case VS_ID_GAME_ROTATION_VECTOR:
			quaternion_data.x = sensor_data->data_quaternion.x;
			quaternion_data.y = sensor_data->data_quaternion.y;
			quaternion_data.z = sensor_data->data_quaternion.z;
			quaternion_data.w = sensor_data->data_quaternion.w;
		break;

		case VS_ID_ACCELEROMETER:
			acc_data.x_pos = sensor_data->data_vector.x;
			acc_data.y_pos = sensor_data->data_vector.y;
			acc_data.z_pos = sensor_data->data_vector.z;
		break;

		case VS_ID_GYROSCOPE:
			gyro_data.x_pos = sensor_data->data_vector.x/16;
			gyro_data.y_pos = sensor_data->data_vector.y/16;
			gyro_data.z_pos = sensor_data->data_vector.z/16;
		break;

		case VS_ID_STEP_DETECTOR_WAKEUP:
			step_detected = true;
		break;

		case VS_ID_GRAVITY:
			gra_x_data = sensor_data->data_vector.x;
			gra_y_data = sensor_data->data_vector.y;
			gra_z_data = sensor_data->data_vector.z;

			if((gra_x_data) > (gra_x_data_prev))
			tempx = (gra_x_data) - (gra_x_data_prev);
			else
			tempx = (gra_x_data_prev) - (gra_x_data);

			if((gra_y_data) > (gra_y_data_prev))
			tempy = (gra_y_data) - (gra_y_data_prev);
			else
			tempy = (gra_y_data_prev) - (gra_y_data);

			if((gra_z_data) > (gra_z_data_prev))
			tempz = (gra_z_data) - (gra_z_data_prev);
			else
			tempz = (gra_z_data_prev) - (gra_z_data);

			if ((tempx >ACC_GRAVITY_THRESHOLD_X) ||  \
			    (tempy >ACC_GRAVITY_THRESHOLD_Y) ||  \
				(tempz >ACC_GRAVITY_THRESHOLD_Z))
			{
				device_drop_detected++;
			}

			gra_x_data_prev = gra_x_data;
			gra_y_data_prev = gra_y_data;
			gra_z_data_prev = gra_z_data;
		break;
		default:
		break;
	}
}

/*! \brief enable accelerometer
 *
 */
void enable_accelerometer(void)
{
	if(bhy_enable_virtual_sensor(VS_TYPE_ACCELEROMETER, VS_NON_WAKEUP, SAMPLE_DATA_RATE, 0, VS_FLUSH_NONE, 0, 0) != BHY_SUCCESS)
	{
		while(1);
	}
	bhy_install_sensor_callback(VS_TYPE_ACCELEROMETER, VS_NON_WAKEUP, sensors_callback);
	acc_sens_stat = true;
}

/*! \brief disable accelerometer
 *
 */
void disable_accelerometer(void)
{
	bhy_disable_virtual_sensor(VS_TYPE_ACCELEROMETER, VS_NON_WAKEUP);
	bhy_uninstall_sensor_callback (VS_TYPE_ACCELEROMETER, VS_NON_WAKEUP);
	acc_sens_stat = false;
}

/*! \brief enable gyroscope
 *
 */
void enable_gyroscope(void)
{
	if(bhy_enable_virtual_sensor(VS_TYPE_GYROSCOPE, VS_NON_WAKEUP, SAMPLE_DATA_RATE, 0, VS_FLUSH_NONE, 0, 0) != BHY_SUCCESS)
	{
		while(1);
	}
	bhy_install_sensor_callback(VS_TYPE_GYROSCOPE, VS_NON_WAKEUP, sensors_callback);
	gyro_sens_stat = true;
}

/*! \brief disable gyroscope
 *
 */
void disable_gyroscope(void)
{
	bhy_disable_virtual_sensor(VS_TYPE_GYROSCOPE, VS_NON_WAKEUP);
	bhy_uninstall_sensor_callback (VS_TYPE_GYROSCOPE, VS_NON_WAKEUP);
	gyro_sens_stat = false;
}

/*! \brief enable rotation vector
 *
 */
void enable_rotation_vector(void)
{
	if(bhy_enable_virtual_sensor(VS_TYPE_GAME_ROTATION_VECTOR, VS_NON_WAKEUP, SAMPLE_DATA_RATE, 0, VS_FLUSH_ALL, 0, 0) != BHY_SUCCESS)
	{
		while(1);
	}
	bhy_install_sensor_callback(VS_TYPE_GAME_ROTATION_VECTOR, VS_NON_WAKEUP, sensors_callback);
	rot_sens_stat = true;
}

/*! \brief disable rotation vector
 *
 */
void disable_rotation_vector(void)
{
	bhy_disable_virtual_sensor(VS_TYPE_GAME_ROTATION_VECTOR, VS_NON_WAKEUP);
	bhy_uninstall_sensor_callback (VS_TYPE_GAME_ROTATION_VECTOR, VS_NON_WAKEUP);
	rot_sens_stat = false;
}

/*! \brief enable step detector
 *
 */
void enable_step_detector(void)
{
	if(bhy_enable_virtual_sensor(VS_TYPE_STEP_DETECTOR, VS_WAKEUP, SAMPLE_DATA_RATE, 0, VS_FLUSH_NONE, 0, 0) != BHY_SUCCESS)
	{
		while(1);
	}
	bhy_install_sensor_callback(VS_TYPE_STEP_DETECTOR, VS_WAKEUP, sensors_callback);
	stp_sens_stat = true;
}

/*! \brief disable step detector
 *
 */
void disable_step_detector(void)
{
	bhy_disable_virtual_sensor(VS_TYPE_STEP_DETECTOR, VS_WAKEUP);
	bhy_uninstall_sensor_callback (VS_TYPE_STEP_DETECTOR, VS_WAKEUP);
	stp_sens_stat = false;
}

/*! \brief enable drop detector
 *
 */
void enable_drop_detector(void)
{
	if(bhy_enable_virtual_sensor(VS_TYPE_GRAVITY, VS_NON_WAKEUP, SAMPLE_DATA_RATE, 0, VS_FLUSH_NONE, 0, 0) != BHY_SUCCESS)
	{
		while(1);
	}
	bhy_install_sensor_callback(VS_TYPE_GRAVITY, VS_NON_WAKEUP, sensors_callback);
	gra_sens_stat = true;
}

/*! \brief  disable drop detector
 *
 */
void disable_drop_detector(void)
{
	bhy_disable_virtual_sensor(VS_TYPE_GRAVITY, VS_NON_WAKEUP);
	bhy_uninstall_sensor_callback (VS_TYPE_GRAVITY, VS_NON_WAKEUP);
	gra_sens_stat = false;
}

void enable_environmental_sensors(void)
{
	//Nothing now
}

void disable_environmental_sensors(void)
{
	//Nothing now
}
enum status_code veml6080_init(void)
{

	uint8_t data[8];

	data[0] = 0x10;
	data[1] = VEML6080_CONFIG_CMD_CODE;
	data[2] = 2; //write length
	data[3] = 0; //status
	data[4] = VEML6080_INIT_VALUE;
	data[5] = VEML6080_INIT_VALUE;

	bhy_write_parameter_page(BHY_PAGE_15, 2, data, 6);
	return STATUS_OK;
}

/*---------------------------------------------*/
/*Function to read the ALS data from veml6080*/
/*---------------------------------------------*/
enum status_code veml6080_read_alsdata(uint32_t *sensor_data)
{

	uint8_t data[6];
	uint8_t data_rd[6];
		
	data[0] = 0x10;
	data[1] = VEML6080_UVDATA_CMD_CODE;
	data[2] = 2; //write length
	data[3] = 0; //status
	data[4] = VEML6080_UVDATA_CMD_CODE;
	data[5] = 0;

	bhy_write_parameter_page(BHY_PAGE_15, 1, data, 3);
	bhy_read_parameter_page(BHY_PAGE_15, 2, data_rd, 4+2);
	*sensor_data = ((data_rd[5] << 8) | data_rd[4]);
	return STATUS_OK;
}
