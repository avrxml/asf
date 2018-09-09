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


#ifndef __BHI_H__
#define __BHI_H__

#include "bhi160/bhi160_api/BHy_support.h"
#include "sensor_tag.h"

/* macros */
//Gyro data to BLE
extern gyro_positions_t gyro_data;
//Accelerometer data to BLE
extern accelero_positions_t acc_data;
//Quaternion data to BLE
extern device_rotation_vector_t quaternion_data;

/* external variables */
extern  const u32 _bhi_fw_len;
//extern const u8 _bhi_fw[21248];
extern uint8_t step_detected;
extern uint16_t device_drop_detected;

// extern uint16_t temperature_data;
// extern uint16_t humidity_data;
// extern uint32_t pressure_data;
// extern uint16_t als_data;

/* prototypes */
void process_motion_sensor_data(void);
void enable_accelerometer(void);
void disable_accelerometer(void);
void enable_gyroscope(void);
void disable_gyroscope(void);
void enable_rotation_vector(void);
void disable_rotation_vector(void);
void enable_step_detector(void);
void disable_step_detector(void);
void enable_drop_detector(void);
void disable_drop_detector(void);
void enable_BME280_temperature(void);
void enable_BME280_humidity(void);
void enable_BME280_pressure(void);
void disable_BME280_temperature(void);
void disable_BME280_humidity(void);
void disable_BME280_pressure(void);
void enable_environmental_sensors(void);
void disable_environmental_sensors(void);



// 
// VEML6080 specific defines
// 

//When ADDR pin is tied to VDDIo
#define VEML6080_I2C_ADDRESS1  0x48
//When ADDR pin is tied to GND
#define VEML6080_I2C_ADDRESS2  0x10


#define VEML6080_CMD_LENGTH_BYTE   1
#define VEML6080_DATA_LENGTH_BYTE  2

#define VEML6080_INIT_VALUE  0x00


//Command codes of VEML6080
#define VEML6080_CONFIG_CMD_CODE	0x00
#define VEML6080_UVDATA_CMD_CODE	0x04
#define VEML6080_ALSDATA_CMD_CODE	0x08


enum status_code veml6080_init(void);
//enum status_code veml6080_read_uvdata (uint16_t *uv_data);
enum status_code veml6080_read_alsdata(uint32_t *sensor_data);
#endif /* __BHI_H__ */