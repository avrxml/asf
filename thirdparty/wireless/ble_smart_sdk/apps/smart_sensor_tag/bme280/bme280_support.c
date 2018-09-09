/*
****************************************************************************
* Copyright (C) 2014 - 2015 Bosch Sensortec GmbH
*
* bme280_support.c
* Date: 2015/03/27
* Revision: 1.0.5 $
*
* Usage: Sensor Driver support file for BME280 sensor
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
/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/
#include <asf.h>
#include "bme280_support.h"
#include "bhi160/bhi160_api/BHy_support.h"
#include "bhi160/bhy_uc_driver_types.h"
#include "bhi160/bhy_uc_driver.h"
#include "bhi160.h"
#include "status_codes.h"




#define MASK_DATA1	0xFF
#define MASK_DATA2	0x80
#define MASK_DATA3	0x7F

/************** I2C/SPI buffer length ******/
#define	I2C_BUFFER_LEN 8
#define SENSOR_ADDR_LENGTH_BYTE 1


//struct i2c_master_module i2c_master_instance;


/*----------------------------------------------------------------------------*
 *  struct bme280_t parameters can be accessed by using bme280
 *	bme280_t having the following parameters
 *	Bus write function pointer: BME280_WR_FUNC_PTR
 *	Bus read function pointer: BME280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *	Chip id of the sensor: chip_id
 *---------------------------------------------------------------------------*/
struct bme280_t bme280_parameters;



	


/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bme280
*-------------------------------------------------------------------------*/
void I2C_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bme280 the following structure parameter can be accessed
 *	Bus write function pointer: BME280_WR_FUNC_PTR
 *	Bus read function pointer: BME280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
	bme280_parameters.bus_write  = BME280_I2C_bus_write;
	bme280_parameters.bus_read   = BME280_I2C_bus_read;
	bme280_parameters.dev_addr   = BME280_I2C_ADDRESS1;
	bme280_parameters.delay_msec = BME280_delay_msek;
}



/*-------------------------------------------------------------------*
*	This is a sample code for read and write the data by using I2C/SPI
*	Use either I2C or SPI based on your need
*	The device address defined in the bme280.h file
*-----------------------------------------------------------------------*/
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
BME280_RETURN_FUNCTION_TYPE BME280_I2C_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	uint8_t buffer[3];
	buffer[0] = reg_addr;
	buffer[1] = *reg_data;
	uint8_t data[8];

	data[0] = dev_addr;
	data[1] = buffer[0];
	data[2] = 2; //cnt
	data[3] = 0; //status
	data[4] = buffer[1];
	data[5] = 0;

                                                                 	bhy_write_parameter_page(BHY_PAGE_15, 2, data, 6);
	return SUCCESS;
}


 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of data byte of to be read
 */
BME280_RETURN_FUNCTION_TYPE BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	uint8_t data[8];
	uint8_t data_rd[8];

	for (uint8_t i=0; i<cnt; i++)
	{
		data[0] = dev_addr;
		data[1] = reg_addr + i;
		data[2] = 1; //write length
		data[3] = 0; //status
		data[4] = reg_addr;//Not required
		data[5] = 0;//Not required
	
		bhy_write_parameter_page(BHY_PAGE_15, 1, data, 3);
		bhy_read_parameter_page(BHY_PAGE_15, 2, data_rd, 4+1);
		reg_data[i] = data_rd[4];	
	}
	return SUCCESS;		
}




/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BME280_delay_msek(u32 msek)
{
	/*Here you can write your own delay routine*/
	uint32_t nop_count, nop_count_limit;
	//delay for 1msec
	nop_count_limit = ((26000/3) * msek);
	/*nop_count_limit = (msec);*/
	for (nop_count = 0; nop_count <= nop_count_limit; nop_count ++) {\
		__NOP(); \
	}

}

/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
BME280_RETURN_FUNCTION_TYPE bme280_sensor_init(void)
{

	/* result of communication results*/
	s32 com_rslt = ERROR;
		
	//Initialize I2C functions	
	I2C_routine();

/*--------------------------------------------------------------------------*
 *  This function used to assign the value/reference of
 *	the following parameters
 *	I2C address
 *	Bus Write
 *	Bus read
 *	Chip id
*-------------------------------------------------------------------------*/
	com_rslt = bme280_init(&bme280_parameters);

	/*	For initialization it is required to set the mode of
	 *	the sensor as "NORMAL"
	 *	data acquisition/read/write is possible in this mode
	 *	by using the below API able to set the power mode as NORMAL*/
	/* Set the power mode as NORMAL*/
	com_rslt = bme280_set_power_mode(BME280_FORCED_MODE);
	/*	For reading the pressure, humidity and temperature data it is required to
	 *	set the OSS setting of humidity, pressure and temperature
	 * The "BME280_CTRLHUM_REG_OSRSH" register sets the humidity
	 * data acquisition options of the device.
	 * changes to this registers only become effective after a write operation to
	 * "BME280_CTRLMEAS_REG" register.
	 * In the code automated reading and writing of "BME280_CTRLHUM_REG_OSRSH"
	 * register first set the "BME280_CTRLHUM_REG_OSRSH" and then read and write
	 * the "BME280_CTRLMEAS_REG" register in the function*/
	com_rslt = bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	/* set the pressure oversampling*/
	com_rslt = bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
	/* set the temperature oversampling*/
	com_rslt = bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);

	/* This API used to Write the standby time of the sensor input
	 *	value have to be given
	 *	Normal mode comprises an automated perpetual cycling between an (active)
	 *	Measurement period and an (inactive) standby period.
	 *	The standby time is determined by the contents of the register t_sb.
	 *	Standby time can be set using BME280_STANDBYTIME_125_MS.
	 *	Usage Hint : bme280_set_standbydur(BME280_STANDBYTIME_125_MS)*/

// 	com_rslt = bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
// 
// 	/* This API used to read back the written value of standby time*/
// 	com_rslt = bme280_get_standby_durn(&v_stand_by_time_u8);
	
	return com_rslt;

}


BME280_RETURN_FUNCTION_TYPE bme280_read_pressure(u32 *bme280_pressure)
{
	s32 uncomp_pressure = BME280_INIT_VALUE;
	BME280_RETURN_FUNCTION_TYPE read_status = ERROR;
	
	read_status = bme280_read_uncomp_pressure(&uncomp_pressure);
	if (read_status != SUCCESS){
		return read_status;
	}
	
	*bme280_pressure = bme280_compensate_pressure_int32(uncomp_pressure);
	
	return read_status;
}


BME280_RETURN_FUNCTION_TYPE bme280_read_temperature (s32 *bme280_temperature)
{
	s32 uncomp_temperature = BME280_INIT_VALUE;
	BME280_RETURN_FUNCTION_TYPE read_status = ERROR;
	
	read_status = bme280_read_uncomp_temperature(&uncomp_temperature);
	if (read_status != SUCCESS){
		return read_status;
	}
	
	*bme280_temperature = bme280_compensate_temperature_int32(uncomp_temperature);
	
	return read_status;
	
}


BME280_RETURN_FUNCTION_TYPE bme280_read_humidity(u32 *bme280_humidity)
{
	s32 uncomp_humidity = BME280_INIT_VALUE;
	BME280_RETURN_FUNCTION_TYPE read_status = ERROR;
	
	read_status = bme280_read_uncomp_humidity(&uncomp_humidity);
	if (read_status != SUCCESS){
		return read_status;
	}
	
	*bme280_humidity = bme280_compensate_humidity_int32(uncomp_humidity);
	
	return read_status;
}

BME280_RETURN_FUNCTION_TYPE bme280_read_PTH(u32 *bme280_pressure,
								s32 *bme280_temperature, u32 *bme280_humidity)
{
	BME280_RETURN_FUNCTION_TYPE read_status = ERROR;
	
	read_status = bme280_read_pressure_temperature_humidity(bme280_pressure, bme280_temperature,
								bme280_humidity);
								
	return read_status;
}
								



/*************************EXAMPLE FUNCTION CALLS = for reference ****************************************/
// /* This function is an example for reading sensor data
//  *	\param: None
//  *	\return: communication result
//  */
// s32 bme280_data_readout_template(void);
// /*----------------------------------------------------------------------------*
//  *  struct bme280_t parameters can be accessed by using bme280
//  *	bme280_t having the following parameters
//  *	Bus write function pointer: BME280_WR_FUNC_PTR
//  *	Bus read function pointer: BME280_RD_FUNC_PTR
//  *	Delay function pointer: delay_msec
//  *	I2C address: dev_addr
//  *	Chip id of the sensor: chip_id
//  *---------------------------------------------------------------------------*/
// struct bme280_t bme280;
// /* This function is an example for reading sensor data
//  *	\param: None
//  *	\return: communication result
//  */
// s32 bme280_data_readout_template(void)
// {
// 	/* The variable used to assign the standby time*/
// 	u8 v_stand_by_time_u8 = BME280_INIT_VALUE;
// 	/* The variable used to read uncompensated temperature*/
// 	s32 v_data_uncomp_tem_s32 = BME280_INIT_VALUE;
// 	/* The variable used to read uncompensated pressure*/
// 	s32 v_data_uncomp_pres_s32 = BME280_INIT_VALUE;
// 	/* The variable used to read uncompensated pressure*/
// 	s32 v_data_uncomp_hum_s32 = BME280_INIT_VALUE;
// 	/* The variable used to read real temperature*/
// 	s32 v_actual_temp_s32 = BME280_INIT_VALUE;
// 	/* The variable used to read real pressure*/
// 	u32 v_actual_press_u32 = BME280_INIT_VALUE;
// 	/* The variable used to read real humidity*/
// 	u32 v_actual_humity_u32 = BME280_INIT_VALUE;
// 	/* result of communication results*/
// 	s32 com_rslt = ERROR;
// 
// 
// 
//  /*********************** START INITIALIZATION ************************/
//   /*	Based on the user need configure I2C or SPI interface.
//   *	It is example code to explain how to use the bme280 API*/
//   #ifdef BME280_API
// 	I2C_routine();
// 	/*SPI_routine(); */
// 	#endif
// /*--------------------------------------------------------------------------*
//  *  This function used to assign the value/reference of
//  *	the following parameters
//  *	I2C address
//  *	Bus Write
//  *	Bus read
//  *	Chip id
// *-------------------------------------------------------------------------*/
// 	com_rslt = bme280_init(&bme280);
// 
// 	/*	For initialization it is required to set the mode of
// 	 *	the sensor as "NORMAL"
// 	 *	data acquisition/read/write is possible in this mode
// 	 *	by using the below API able to set the power mode as NORMAL*/
// 	/* Set the power mode as NORMAL*/
// 	com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
// 	/*	For reading the pressure, humidity and temperature data it is required to
// 	 *	set the OSS setting of humidity, pressure and temperature
// 	 * The "BME280_CTRLHUM_REG_OSRSH" register sets the humidity
// 	 * data acquisition options of the device.
// 	 * changes to this registers only become effective after a write operation to
// 	 * "BME280_CTRLMEAS_REG" register.
// 	 * In the code automated reading and writing of "BME280_CTRLHUM_REG_OSRSH"
// 	 * register first set the "BME280_CTRLHUM_REG_OSRSH" and then read and write
// 	 * the "BME280_CTRLMEAS_REG" register in the function*/
// 	com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
// 
// 	/* set the pressure oversampling*/
// 	com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_2X);
// 	/* set the temperature oversampling*/
// 	com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_4X);
// /*--------------------------------------------------------------------------*/
// /*------------------------------------------------------------------------*
// ************************* START GET and SET FUNCTIONS DATA ****************
// *---------------------------------------------------------------------------*/
// 	/* This API used to Write the standby time of the sensor input
// 	 *	value have to be given
// 	 *	Normal mode comprises an automated perpetual cycling between an (active)
// 	 *	Measurement period and an (inactive) standby period.
// 	 *	The standby time is determined by the contents of the register t_sb.
// 	 *	Standby time can be set using BME280_STANDBYTIME_125_MS.
// 	 *	Usage Hint : bme280_set_standbydur(BME280_STANDBYTIME_125_MS)*/
// 
// 	com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
// 
// 	/* This API used to read back the written value of standby time*/
// 	com_rslt += bme280_get_standby_durn(&v_stand_by_time_u8);
// /*-----------------------------------------------------------------*
// ************************* END GET and SET FUNCTIONS ****************
// *------------------------------------------------------------------*/
// 
// /************************* END INITIALIZATION *************************/
// 
// /*------------------------------------------------------------------*
// ************ START READ UNCOMPENSATED PRESSURE, TEMPERATURE
// AND HUMIDITY DATA ********
// *---------------------------------------------------------------------*/
// 	/* API is used to read the uncompensated temperature*/
// 	com_rslt += bme280_read_uncomp_temperature(&v_data_uncomp_tem_s32);
// 
// 	/* API is used to read the uncompensated pressure*/
// 	com_rslt += bme280_read_uncomp_pressure(&v_data_uncomp_pres_s32);
// 
// 	/* API is used to read the uncompensated humidity*/
// 	com_rslt += bme280_read_uncomp_humidity(&v_data_uncomp_hum_s32);
// 
// 	/* API is used to read the uncompensated temperature,pressure
// 	and humidity data */
// 	com_rslt += bme280_read_uncomp_pressure_temperature_humidity(
// 	&v_data_uncomp_tem_s32, &v_data_uncomp_pres_s32, &v_data_uncomp_hum_s32);
// /*--------------------------------------------------------------------*
// ************ END READ UNCOMPENSATED PRESSURE AND TEMPERATURE********
// *-------------------------------------------------------------------------*/
// 
// /*------------------------------------------------------------------*
// ************ START READ TRUE PRESSURE, TEMPERATURE
// AND HUMIDITY DATA ********
// *---------------------------------------------------------------------*/
// 	/* API is used to read the true temperature*/
// 	/* Input value as uncompensated temperature and output format*/
// 	com_rslt += bme280_compensate_temperature_int32(v_data_uncomp_tem_s32);
// 
// 	/* API is used to read the true pressure*/
// 	/* Input value as uncompensated pressure */
// 	com_rslt += bme280_compensate_pressure_int32(v_data_uncomp_pres_s32);
// 
// 	/* API is used to read the true humidity*/
// 	/* Input value as uncompensated humidity and output format*/
// 	com_rslt += bme280_compensate_H_int32(v_data_uncomp_hum_s32);
// 
// 	/* API is used to read the true temperature, humidity and pressure*/
// 	com_rslt += bme280_read_pressure_temperature_humidity(
// 	&v_actual_press_u32, &v_actual_temp_s32, &v_actual_humity_u32);
// /*--------------------------------------------------------------------*
// ************ END READ TRUE PRESSURE, TEMPERATURE AND HUMIDITY ********
// *-------------------------------------------------------------------------*/
// 
// /*-----------------------------------------------------------------------*
// ************************* START DE-INITIALIZATION ***********************
// *-------------------------------------------------------------------------*/
// 	/*	For de-initialization it is required to set the mode of
// 	 *	the sensor as "SLEEP"
// 	 *	the device reaches the lowest power consumption only
// 	 *	In SLEEP mode no measurements are performed
// 	 *	All registers are accessible
// 	 *	by using the below API able to set the power mode as SLEEP*/
// 	 /* Set the power mode as SLEEP*/
// 	com_rslt += bme280_set_power_mode(BME280_SLEEP_MODE);
// /*---------------------------------------------------------------------*
// ************************* END DE-INITIALIZATION **********************
// *---------------------------------------------------------------------*/
// return com_rslt;
// }
