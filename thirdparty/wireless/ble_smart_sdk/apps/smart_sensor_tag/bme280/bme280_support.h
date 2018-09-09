/*
 * bme280_support.h
 *
 * Created: 11/3/2015 3:24:13 PM
 *  Author: gracy.angela
 */ 


#ifndef BME280_SUPPORT_H_
#define BME280_SUPPORT_H_


#include "bme280.h"






/* Initialize bme280 */
BME280_RETURN_FUNCTION_TYPE bme280_sensor_init(void);

/*----------------------------------------------------------------------------*
*  The following functions are used for reading and writing of
*	sensor data using I2C or SPI communication
*----------------------------------------------------------------------------*/

/*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
BME280_RETURN_FUNCTION_TYPE BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
BME280_RETURN_FUNCTION_TYPE BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/*
 * \Brief: SPI/I2C init routine
*/
void I2C_routine(void);


/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BME280_delay_msek(u32 msek);

BME280_RETURN_FUNCTION_TYPE bme280_read_pressure(u32 *bme280_pressure);

BME280_RETURN_FUNCTION_TYPE bme280_read_temperature (s32 *bme280_temperature);

BME280_RETURN_FUNCTION_TYPE bme280_read_humidity(u32 *bme280_humidity);

BME280_RETURN_FUNCTION_TYPE bme280_read_PTH(u32 *bme280_pressure,
s32 *bme280_temperature, u32 *bme280_humidity);


#endif /* BME280_SUPPORT_H_ */