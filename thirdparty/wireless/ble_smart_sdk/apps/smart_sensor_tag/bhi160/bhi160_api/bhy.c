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

/*! file <BHY >
    brief <Sensor driver for BHY> */
#include "bhy.h"
/* static structure for bhy */
static struct bhy_t *p_bhy;
/* contain the saved parameters data*/
static struct parameter_read_buffer_t read_buffer;
/* contain the load parameters data*/
static struct parameter_write_buffer_t write_buffer;
/* Assign the sensor status bank*/
struct sensor_status_bank_t sensor_status_bank;

/*!
 *	@brief
 *	This function is used for initialize
 *	bus read and bus write functions
 *	and device address
 *	product id is read in the register 0x90 bit from 0 to 7
 *
 *	@param bhy : structure pointer
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *	@note
 *	While changing the parameter of the bhy_t
 *	consider the following point:
 *	Changing the reference value of the parameter
 *	will changes the local copy or local reference
 *	make sure your changes will not
 *	affect the reference value of the parameter
 *	(Better case don't change the reference value of the parameter)
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_init(struct bhy_t *bhy)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* assign bhy ptr */
	p_bhy = bhy;
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_PRODUCT_ID_ADDR,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	/* read product Id */
	p_bhy->product_id = v_data_u8;
	return com_rslt;
}
/*!
 * @brief
 *	This API write the data to
 *	the given register
 *
 *
 *	@param v_addr_u8 -> Address of the register
 *	@param v_data_u8 -> The data from the register
 *	@param v_len_u8 -> no of bytes to read
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BHY_RETURN_FUNCTION_TYPE bhy_write_reg(u8 v_addr_u8,
u8 *v_data_u8, u16 v_len_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* check the p_bhy structure as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* write data from register*/
			com_rslt =
			p_bhy->BHY_BUS_WRITE_FUNC(p_bhy->device_addr,
			v_addr_u8, v_data_u8, v_len_u16);
		}
	return com_rslt;
}
/*!
 * @brief
 *	This API reads the data from
 *	the given register
 *
 *
 *	@param v_addr_u8 -> Address of the register
 *	@param v_data_u8 -> The data from the register
 *	@param v_len_u8 -> no of bytes to read
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 */
BHY_RETURN_FUNCTION_TYPE bhy_read_reg(u8 v_addr_u8,
u8 *v_data_u8, u16 v_len_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* check the p_bhy structure as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* Read data from register*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			v_addr_u8, v_data_u8, v_len_u16);
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the FIFO flush from the register 0x32
 *	bit 0 to 7
 *
 *
 *	@param v_fifo_flush_u8 : The value of fifo flush
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_fifo_flush(u8 *v_fifo_flush_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;

	if (BHY_NULL_PTR == p_bhy) {
		com_rslt = BHY_NULL;
		} else {
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_FIFO_FLUSH__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_fifo_flush_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_FIFO_FLUSH);
	}
	return com_rslt;
}
/*!
 *	@brief API used to set the FIFO flush from the register 0x32
 *	bit 0 to 7
 *
 *
 *	@param v_fifo_flush_u8 : The value of fifo flush
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_fifo_flush(u8 v_fifo_flush_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;

	if (BHY_NULL_PTR == p_bhy) {
		com_rslt = BHY_NULL;
		} else {
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_FIFO_FLUSH__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		if (BHY_SUCCESS == com_rslt) {
			v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
			BHY_I2C_REG_FIFO_FLUSH,
			v_fifo_flush_u8);
			com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
			(p_bhy->device_addr,
			BHY_I2C_REG_FIFO_FLUSH__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		}
	}
	return com_rslt;
}
/*!
 *	@brief API used to get the chip control from the register 0x34
 *	bit 0 to 7
 *  @note Chip control used to provide the control fundamental
 *  behaviour of the chip
 *
 *	@param v_chipcontrol_u8 : The value of chip control
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_chip_control(u8 *v_chipcontrol_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy structure as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the chip control status*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_CHIP_CONTROL__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_chipcontrol_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_CONTROL);
		}
	return com_rslt;
}
/*!
 *	@brief API used to set the chip control from the register 0x34
 *	bit 0 to 7
 *  @note Chip control used to provide the control fundamental
 *  behaviour of the chip
 *
 *	@param v_chipcontrol_u8 : The value of chip control
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_chip_control(u8 v_chipcontrol_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy structure as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_CHIP_CONTROL__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		if (BHY_SUCCESS == com_rslt) {
			v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_CONTROL,
			v_chipcontrol_u8);
			com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
			(p_bhy->device_addr,
			BHY_I2C_REG_CHIP_CONTROL__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		}
	}
	return com_rslt;
}
/*!
 *	@brief API used to get the host status from the register 0x35
 *	bit 0 to 7
 *
 *
 *	@param v_algo_standby_u8 : The value of algorithm standby duration
 *	@param v_algo_id_u8 : The value of algorithm id
 *	@param v_host_interface_id_u8 : The value of host interface id
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_host_status(
u8 *v_algo_standby_u8, u8 *v_algo_id_u8,
u8 *v_host_interface_id_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
		/* read the host status*/
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_HOST_STATUS__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		/* read the host algo status*/
		*v_algo_standby_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_STATUS_ALGO_STANDBY);
		/* read the host algo id */
		*v_algo_id_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_HOST_STATUS_ALGO_ID);
		/* host interface id */
		*v_host_interface_id_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_HOST_INTERFACE_ID);
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the interrupt status from the register 0x36
 *	bit 0 to 7
 *
 *
 *	@param v_host_int_u8 :
 *	The value of host interrupt status
 *	@param v_wakeup_water_mark_u8 :
 *	The value of wakeup watermark status
 *	@param v_wakeup_latency_u8 :
 *	The value of wakeup latency status
 *	@param v_wakeup_immediate_u8 :
 *	The value of wakeup immediate status
 *	@param v_non_wakeup_water_mark_u8 :
 *	The value of non wakeup watermark status
 *	@param v_non_wakeup_latency_u8 :
 *	The value of non wakeup latency status
 *	@param v_non_wakeup_immediate_u8 :
 *	The value of non wakeup immediate
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_interrupt_status(
u8 *v_host_int_u8,
u8 *v_wakeup_water_mark_u8, u8 *v_wakeup_latency_u8,
u8 *v_wakeup_immediate_u8, u8 *v_non_wakeup_water_mark_u8,
u8 *v_non_wakeup_latency_u8, u8 *v_non_wakeup_immediate_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
		/* read the interrupt status*/
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_INT_STATUS__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		/* read the host interrupt status*/
		*v_host_int_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_INT_STATUS_HOST_INTR);
		/* read the wakeup watermark interrupt status*/
		*v_wakeup_water_mark_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_WM);
		/* read the wakeup latency interrupt status*/
		*v_wakeup_latency_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_LATENCY);
		/* read the wakeup immediate interrupt status*/
		*v_wakeup_immediate_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_IMMEDIATE);
		/* read the non wakeup watermark interrupt status*/
		*v_non_wakeup_water_mark_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_BHY_INT_STATUS_NON_WAKEUP_WM);
		/* read the non wakeup latency interrupt status*/
		*v_non_wakeup_latency_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_INT_STATUS_NON_WAKEUP_LATENCY);
		/* read the non wakeup immediate status*/
		*v_non_wakeup_immediate_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_INT_STATUS_NON_WAKEUP_IMMEDIATE);
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the chip status from the register 0x37
 *	bit 0 to 7
 *
 *
 *
 *	@param v_eeprom_detected_u8 : The value of eeprom
 *	detected status
 *	@param v_ee_upload_done_u8 : The value of ee_upload
 *	done status
 *	@param v_ee_upload_error_u8 : The value of ee_upload
 *	done error
 *	@param v_firmware_idle_u8 : The value of firmware error status
 *	@param v_no_eeprom_u8 : The value of no eeprom status
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_chip_status(
u8 *v_eeprom_detected_u8,
u8 *v_ee_upload_done_u8, u8 *v_ee_upload_error_u8,
u8 *v_firmware_idle_u8, u8 *v_no_eeprom_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the chip status*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_CHIP_STATUS__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			/* read eeprom detected status*/
			*v_eeprom_detected_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_STATUS_EEPROM_DETECTED);
			/* read eeprom upload done status*/
			*v_ee_upload_done_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_DONE);
			/* read eeprom upload error status*/
			*v_ee_upload_error_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_ERROR);
			/* read firmware idle status*/
			*v_firmware_idle_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_STATUS_FIRMWARE_IDLE);
			/* read no eeprom detected status*/
			*v_no_eeprom_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_CHIP_STATUS_NO_EEPROM);
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the bytes remaining from the register 0x38
 *	and 0x39 bit 0 to 7
 *
 *
 *	@param v_bytes_remaining_u16 : The value of bytes remaining
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *	@note This indicates how many bytes are available in the FIFO buffer
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_read_bytes_remaining(u16 *v_bytes_remaining_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* Array contains the bytes reaming of FIFO lSB and MSB data
		v_data_u8[LSB_ZERO] - LSB
		v_data_u8[MSB_ONE] - MSB*/
	u8 v_data_u8[BHY_BYTES_REMAINING_SIZE] = {BHY_INIT_VALUE,
	BHY_INIT_VALUE};
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt = BHY_NULL;
		} else {
			/* read bytes remaining data */
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_BYTES_REMAINING_LSB_ADDR,
			v_data_u8, BHY_BYTES_REMAINING_LENGTH);
			/* get the bytes remaining data*/
			*v_bytes_remaining_u16 = (u16)
			((v_data_u8[BHY_BYTES_REMAINING_MSB]
			<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			| (v_data_u8[BHY_BYTES_REMAINING_LSB]));
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the parameter
 *	acknowledgement from the register 0x3A
 *	bit 0 to 7
 *
 *
 *	@param v_parameter_acknowledge_u8:
 *	The value of parameter acknowledgement
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_parameter_acknowledge(
u8 *v_parameter_acknowledge_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the parameter acknowledgement*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_PARAMETER_ACKNOWLEDGE_ADDR,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_parameter_acknowledge_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the parameter
 *	page select from the register 0x54
 *	bit 0 to 7
 *
 *
 *	@param v_page_select_u8 : The value of parameter page selection
 *       page information      |   value
 *     ------------------------|----------
 *     BHY_PAGE_SELECT_PARAMETER_PAGE  | 0
 *     BHY_PAGE_SELECT_PARAMETER_SIZE  | 1
 *
 *	@param v_parameter_page_u8 : The value of page selection
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_parameter_page_select(
u8 v_page_select_u8, u8 *v_parameter_page_u8)
{
/* variable used for return the status of communication result*/
BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
u8 v_data_u8 = BHY_INIT_VALUE;
/* check the p_bhy pointer as NULL*/
if (BHY_NULL_PTR == p_bhy) {
	com_rslt =  BHY_NULL;
	} else {
		/* read the parameter page information*/
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_PARAMETER_PAGE_SELECT__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		switch (v_page_select_u8) {
		case BHY_PAGE_SELECT_PARAMETER_PAGE:
			*v_parameter_page_u8 = BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_PAGE);
		break;
		case BHY_PAGE_SELECT_PARAMETER_SIZE:
			*v_parameter_page_u8
			= BHY_GET_BITSLICE(v_data_u8,
			BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_SIZE);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	}
return com_rslt;
}
/*!
 *	@brief API used to set the parameter
 *	page select from the register 0x54
 *	bit 0 to 7
 *
 *
 *	@param v_page_select_u8 : The value of parameter page selection
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_parameter_page_select(
u8 v_page_select_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			v_data_u8 = v_page_select_u8;
			/* read the parameter page information*/
			com_rslt = p_bhy->BHY_BUS_WRITE_FUNC
			(p_bhy->device_addr,
			BHY_I2C_REG_PARAMETER_PAGE_SELECT__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			}
			return com_rslt;
		}

/*!
 *	@brief API used to get the host interface control
 *	from the register 0x55
 *	bit 0 to 7
 *
 *
 *
 *	@param v_algo_standby_req_u8 : The value of algorithm standby
 *	request
 *	@param v_abort_transfer_u8 : The value of abort transfer
 *	@param v_update_transfer_cnt_u8 : The value of update
 *	transfer count
 *	@param v_wakeup_fifo_intr_disable_u8 :
 *	The value of wakeup fifo host
 *	interrupt disable
 *	@param v_ned_coordinates_u8 : The value of NED coordinates
 *	@param v_ap_suspend_u8 : The value of AP suspended
 *	@param v_sensor_selftest_u8 : The value of sensor self test
 *	@param v_non_wakeup_fifo_intr_disable_u8:
 *	The value of non wakeup fifo host
 *	interrupt disable
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_host_interface_control(
u8 *v_algo_standby_req_u8,
u8 *v_abort_transfer_u8, u8 *v_update_transfer_cnt_u8,
u8 *v_wakeup_fifo_intr_disable_u8, u8 *v_ned_coordinates_u8,
u8 *v_ap_suspend_u8, u8 *v_sensor_selftest_u8,
u8 *v_non_wakeup_fifo_intr_disable_u8)
{
/* variable used for return the status of communication result*/
BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
u8 v_data_u8 = BHY_INIT_VALUE;
/* check the p_bhy pointer as NULL*/
if (BHY_NULL_PTR == p_bhy) {
	com_rslt =  BHY_NULL;
	} else {
		/* read the host interrupt status*/
		com_rslt =
		p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		/* read algorithm standby request status*/
		*v_algo_standby_req_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST);
		/* read host interrupt control status*/
		*v_abort_transfer_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER);
		/* read update transfer control status*/
		*v_update_transfer_cnt_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT);
		/* read wakeup fifo host interrupt disable status*/
		*v_wakeup_fifo_intr_disable_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE);
		/* read ned coordinates status*/
		*v_ned_coordinates_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE);
		/* read AP suspended status*/
		*v_ap_suspend_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND);
		/* read self test status*/
		*v_sensor_selftest_u8 = BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST);
		/* read non wakeup fifo host interrupt disable status*/
		*v_non_wakeup_fifo_intr_disable_u8 =
		BHY_GET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE);
	}
return com_rslt;
}
/*!
 *	@brief API used to set the host interface control
 *	from the register 0x55
 *	bit 0 to 7
 *
 *
 *	@param v_host_interface_select_u8 : The value of host interface selection
 *         host interface                 |   value
 *     -----------------------------------|----------
 *     BHY_HOST_ALGO_STANDBY_REQUEST      | 0
 *     BHY_HOST_ABORT_TRANSFER            | 1
 *     BHY_HOST_UPDATE_TRANSFER_COUNT     | 2
 *     BHY_HOST_WAKEUP_FIFO_DISABLE       | 3
 *     BHY_HOST_NED_COORDINATE            | 4
 *     BHY_HOST_AP_SUSPEND                | 5
 *     BHY_HOST_SELFTEST                  | 6
 *     BHY_HOST_NON_WAKEUP_FIFO_DISABLE   | 7
 *
 *	@param v_host_interface_status_u8 : The value of host interface
 *       Data        |  status
 *  -----------------| -----------
 *       0           |  DISABLE
 *       1           |  ENABLE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_host_interface_control(
u8 v_host_interface_select_u8, u8 v_host_interface_status_u8)
{
/* variable used for return the status of communication result*/
BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
u8 v_data_u8 = BHY_INIT_VALUE;
/* check the p_bhy pointer as NULL*/
if (BHY_NULL_PTR == p_bhy) {
	com_rslt =  BHY_NULL;
} else {
switch (v_host_interface_select_u8) {
case BHY_HOST_ALGO_STANDBY_REQUEST:
	/* write algorithm standby request status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_ABORT_TRANSFER:
	/* write host interrupt control status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_UPDATE_TRANSFER_COUNT:
	/* write update transfer control status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_WAKEUP_FIFO_DISABLE:
	/* write wakeup fifo host interrupt disable status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_NED_COORDINATE:
	/* write ned coordinates status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_AP_SUSPEND:
	/* write AP suspended status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_SELFTEST:
	/* write self test status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
case BHY_HOST_NON_WAKEUP_FIFO_DISABLE:
/* write non wakeup fifo host interrupt disable status*/
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__REG,
	&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	if (BHY_SUCCESS == com_rslt) {
		v_data_u8 = BHY_SET_BITSLICE(v_data_u8,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE,
		v_host_interface_status_u8);
		com_rslt += p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__REG,
		&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
	}
break;
default:
	com_rslt = BHY_OUT_OF_RANGE;
break;
}
}
return com_rslt;
}
/*!
 *	@brief API used to get the parameter
 *	request from the register 0x64
 *	bit 0 to 7
 *
 *
 *	@param v_parameter_request_u8 : The value of parameter request
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_parameter_request(
u8 *v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_PARAMETER_REQUEST_ADDR,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_parameter_request_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the parameter
 *	request from the register 0x64
 *	bit 0 to 7
 *
 *
 *	@param v_parameter_request_u8 : The value of parameter request
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_parameter_request(
u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* write load parameter request*/
			 v_data_u8 = v_parameter_request_u8;
			com_rslt = p_bhy->BHY_BUS_WRITE_FUNC
			(p_bhy->device_addr,
			BHY_I2C_REG_LOAD_PARAMETER_REQUEST__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the host IRQ time stamp
 *	request from the register 0x6C to 0x6F
 *	bit 0 to 7
 *
 *
 *	@param v_host_irq_timestamp_u32 : The value of host irq time stamp
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_host_irq_timestamp(
u32 *v_host_irq_timestamp_u32)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8[BHY_HOST_IRQ_TIMESTAMP_SIZE] = {BHY_INIT_VALUE,
	BHY_INIT_VALUE, BHY_INIT_VALUE, BHY_INIT_VALUE};
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_BHY_HOST_IRQ_TIMESTAMP_ADDR,
			v_data_u8, BHY_HOST_IRQ_TIMESTAMP_SIZE);
			*v_host_irq_timestamp_u32 =
			(u32)((v_data_u8[BHY_HOST_IRQ_TIMESTAMP_MSB_DATA]
			<< BHY_SHIFT_BIT_POSITION_BY_24_BITS)
			|(v_data_u8[BHY_HOST_IRQ_TIMESTAMP_XXLSB_DATA]
			<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
			|(v_data_u8[BHY_HOST_IRQ_TIMESTAMP_XLSB_DATA]
			<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			| (v_data_u8[BHY_HOST_IRQ_TIMESTAMP_LSB_DATA]));
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the ROM version
 *	request from the register 0x70 to 0x71
 *	bit 0 to 7
 *
 *
 *	@param v_rom_version_u16 : The value ROM version
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_rom_version(u16 *v_rom_version_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8[BHY_ROM_VERSION_SIZE] = {BHY_INIT_VALUE,
	BHY_INIT_VALUE};
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_ROM_VERSION_ADDR,
			v_data_u8, BHY_ROM_VERSION_SIZE);
			*v_rom_version_u16 =
			(u16)((v_data_u8[BHY_ROM_VERSION_MSB_DATA]
			<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			| (v_data_u8[BHY_ROM_VERSION_LSB_DATA]));
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the RAM version
 *	request from the register 0x72 to 0x73
 *	bit 0 to 7
 *
 *
 *	@param v_ram_version_u16 : The value RAM version
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_ram_version(
u16 *v_ram_version_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8[BHY_RAM_VERSION_SIZE] = {BHY_INIT_VALUE,
	BHY_INIT_VALUE};
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_RAM_VERSION_ADDR,
			v_data_u8, BHY_RAM_VERSION_SIZE);
			*v_ram_version_u16 =
			(u16)((v_data_u8[BHY_RAM_VERSION_MSB_DATA]
			<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			| (v_data_u8[BHY_RAM_VERSION_LSB_DATA]));
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the product id
 *	request from the register 0x90
 *	bit 0 to 7
 *
 *
 *	@param v_product_id_u8 : The value of product id
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_product_id(u8 *v_product_id_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_PRODUCT_ID_ADDR,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_product_id_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the revision id
 *	request from the register 0x91
 *	bit 0 to 7
 *
 *
 *	@param v_revision_id_u8 : The value of revision id
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_revision_id(u8 *v_revision_id_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* read the load parameter request rate*/
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_REVISION_ID_ADDR,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
			*v_revision_id_u8 = v_data_u8;
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the CRC host
 *	request from the register 0x91
 *	bit 0 to 7
 *
 *
 *	@param v_crc_host_u32 : The value of CRC host
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_crc_host(u32 *v_crc_host_u32)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* Array contains the sensor time it is 32 bit data
	a_data_u8[0] - crc HOST
	a_data_u8[1] - crc HOST
	a_data_u8[2] - crc HOST
	a_data_u8[3] - crc HOST
	*/
	u8 a_data_u8[BHY_CRC_HOST_SIZE] = {BHY_INIT_VALUE,
	BHY_INIT_VALUE, BHY_INIT_VALUE, BHY_INIT_VALUE};
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			com_rslt =
			p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
			BHY_I2C_REG_CRC_HOST_ADDR,
			a_data_u8, BHY_CRC_HOST_LENGTH);
			*v_crc_host_u32 = (u32)
			(((u32)a_data_u8[BHY_CRC_HOST_MSB]
			<< BHY_SHIFT_BIT_POSITION_BY_24_BITS) |
			((u32)a_data_u8[BHY_CRC_HOST_XXLSB]
			<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
			|(a_data_u8[BHY_CRC_HOST_XLSB]
			<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			| (a_data_u8[BHY_CRC_HOST_LSB]));
		}
	return com_rslt;
}
/*!
 *	@brief API used to set the reset
 *	request from the register 0x9B
 *	bit 0 to 7
 *
 *
 *	@param v_reset_request_u8 : The value of parameter request
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_reset_request(u8 v_reset_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			v_data_u8 = v_reset_request_u8;
			/* write load parameter request*/
			com_rslt = p_bhy->BHY_BUS_WRITE_FUNC
			(p_bhy->device_addr,
			BHY_I2C_REG_RESET_REQUEST__REG,
			&v_data_u8, BHY_GEN_READ_WRITE_LENGTH);
		}
	return com_rslt;
}
/*!
 *	@brief API used to flash the ram patch
 *
 *
 *	@param memory : The value of data from the
 *  ram patch.
 *	@param v_file_length_u32 : Length of the patch data
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_initialize(
u8 *memory, u32 v_file_length_u32)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_upload_addr = BHY_UPLOAD_DATA;
	u8 v_chip_control_u8 = BHY_CHIP_CTRL_ENABLE_1;
	u32 v_crc_from_memory_u32 = BHY_INIT_VALUE;
	u32 v_length_to_process_u32 = BHY_INIT_VALUE;
	u32 v_crc_host_u32 = BHY_INIT_VALUE;
	u32 write_data = BHY_INIT_VALUE;
	u8 data_from_mem[BHY_SIGNATURE_MEM_LEN];
	u8 data_byte[BHY_RAM_WRITE_LENGTH];
	u32 read_index_u8 = BHY_INIT_VALUE;
	s32 reverse_index_s32 = BHY_INIT_VALUE;
	u32 bytes_to_write_u8 = BHY_RAM_WRITE_LENGTH;
	u32 read_bytes_index = BHY_INIT_VALUE;
	u32 write_index_u32 = BHY_INIT_VALUE;
	u32 write_ram_index = BHY_INIT_VALUE;
	u32 index_mem = BHY_INIT_VALUE;
	u32 write_length = BHY_INIT_VALUE;
	u32 data_to_process = BHY_INIT_VALUE;
	u8 init_array_data = BHY_INIT_VALUE;
	/* initialize the array*/
	for (; init_array_data
	< BHY_SIGNATURE_MEM_LEN; init_array_data++)
		data_from_mem[init_array_data] = BHY_INIT_VALUE;
	for (init_array_data = BHY_INIT_VALUE;
	init_array_data < BHY_RAM_WRITE_LENGTH; init_array_data++)
		data_byte[init_array_data] = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
		/* Assign the memory data into the local array*/
		for (read_index_u8 = BHY_INIT_VALUE;
		read_index_u8 <= BHY_SIGNATURE_LENGTH; read_index_u8++) {
			data_from_mem[read_index_u8] = *(memory+read_index_u8);
		}
		/* Verify the signature of the data*/
		if ((data_from_mem[BHY_SIGNATURE_1] == BHY_IMAGE_SIGNATURE1)
		&& (data_from_mem[BHY_SIGNATURE_2] == BHY_IMAGE_SIGNATURE2)) {
			com_rslt = BHY_SUCCESS;
		} else {
			com_rslt = BHY_ERROR;
		}
		/* read the CRC data from memory */
		v_crc_from_memory_u32 = (u32)
		(((u32)data_from_mem[BHY_CRC_HOST_FILE_MSB]
		<< BHY_SHIFT_BIT_POSITION_BY_24_BITS) |
		((u32)data_from_mem[BHY_CRC_HOST_FILE_XXLSB]
		<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
		|(data_from_mem[BHY_CRC_HOST_FILE_XLSB]
		<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
		| (data_from_mem[BHY_CRC_HOST_FILE_LSB]));
		/* Remove the first 16 bytes*/
		data_to_process = v_file_length_u32 - BHY_SIGNATURE_LENGTH;
		/* data to write for length calculation*/
		v_length_to_process_u32 =
		(data_to_process / BHY_RAM_WRITE_LENGTH);
		read_index_u8 = BHY_SIGNATURE_LENGTH;
		read_bytes_index = BHY_INIT_READ_BYTES;
		for (write_index_u32 = BHY_INIT_VALUE;
		write_index_u32 < v_length_to_process_u32;) {
			/* reverse the data*/
			if (read_index_u8 <= read_bytes_index
			&& read_bytes_index <= data_to_process) {
				for (reverse_index_s32
				= bytes_to_write_u8 - BHY_INIT_BYTE_MINUS_ONE;
				reverse_index_s32 >= BHY_INIT_VALUE;) {
					data_byte[reverse_index_s32]
					= *(memory + read_index_u8);
					read_index_u8++;
					reverse_index_s32--;
				}
			}
			/* write the data into the register*/
			read_bytes_index = read_bytes_index
			+ BHY_RAM_WRITE_LENGTH;
			reverse_index_s32 = BHY_INIT_VALUE;

			for (write_ram_index = BHY_INIT_VALUE;
			write_ram_index <= BHY_CHECK_BYTE;) {
				*(memory + index_mem)
				= data_byte[write_ram_index];
				index_mem++;
				write_ram_index++;
			}
				write_index_u32++;
		}
		/* set the reset as 0x01*/
		com_rslt = bhy_set_reset_request(BHY_RESET_ENABLE);
		com_rslt += bhy_write_reg(BHY_I2C_REG_CHIP_CONTROL_ADDR,
		&v_chip_control_u8, BHY_GEN_READ_WRITE_LENGTH);
		/* set the upload data*/
		com_rslt += bhy_write_reg(BHY_I2C_REG_UPLOAD_0_ADDR,
		&v_upload_addr, BHY_GEN_READ_WRITE_LENGTH);
		com_rslt += bhy_write_reg(BHY_I2C_REG_UPLOAD_1_ADDR,
		&v_upload_addr, BHY_GEN_READ_WRITE_LENGTH);
		/* write the chip control register as 0x02*/
		write_length =
		data_to_process / BHY_RAM_WRITE_LENGTH_API;
		read_index_u8 = BHY_INIT_VALUE;
		/* write the memory of data */
		if (com_rslt == BHY_SUCCESS) {
			for (read_index_u8 = BHY_INIT_VALUE;
				read_index_u8 <
				write_length; read_index_u8++) {
				com_rslt +=
				bhy_write_reg(BHY_I2C_REG_UPLOAD_DATA_ADDR,
				(u8 *)(memory + write_data),
				BHY_RAM_WRITE_LENGTH_API);
				write_data = write_data
				+ BHY_RAM_WRITE_LENGTH_API;
			}
		}
		/* Check the CRC success*/
		com_rslt = bhy_get_crc_host(&v_crc_host_u32);
		if (v_crc_from_memory_u32 == v_crc_host_u32)
			com_rslt = BHY_SUCCESS;
		 else
			com_rslt = BHY_ERROR;
		/* disable upload mode*/
		v_chip_control_u8 = BHY_CHIP_CTRL_ENABLE_2;
		/* write the chip control register as 0x02*/
		com_rslt += bhy_write_reg(BHY_I2C_REG_CHIP_CONTROL_ADDR,
		&v_chip_control_u8, BHY_GEN_READ_WRITE_LENGTH);

		}
		return com_rslt;
}
BHY_RETURN_FUNCTION_TYPE bhy_initialize_from_rom(
const u8 *memory, const u32 v_file_length_u32)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_upload_addr = BHY_UPLOAD_DATA;
	u8 v_chip_control_u8 = BHY_CHIP_CTRL_ENABLE_1;
	u32 v_crc_from_memory_u32 = BHY_INIT_VALUE;
	u32 v_crc_host_u32 = BHY_INIT_VALUE;
	u32 write_data = BHY_INIT_VALUE;
	u8 data_from_mem[BHY_SIGNATURE_MEM_LEN];
	u8 data_byte[BHY_RAM_WRITE_LENGTH_API];
	u32 read_index_u8 = BHY_INIT_VALUE;
	u32 reverse_index_u32 = BHY_INIT_VALUE;
	u32 reverse_block_index_u32 = BHY_INIT_VALUE;
	u32 write_length = BHY_INIT_VALUE;
	u32 data_to_process = BHY_INIT_VALUE;
	u32 packet_length = BHY_INIT_VALUE;;
	u8 i = BHY_INIT_VALUE;
	/* initialize the array*/
	for (; i < BHY_SIGNATURE_MEM_LEN; i++)
	data_from_mem[i] = BHY_INIT_VALUE;
	for (i = BHY_INIT_VALUE; i < BHY_RAM_WRITE_LENGTH; i++)
	data_byte[i] = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
		/* Assign the memory data into the local array*/
		for (read_index_u8 = BHY_INIT_VALUE;
		read_index_u8 <= BHY_SIGNATURE_LENGTH; read_index_u8++) {
			data_from_mem[read_index_u8] = *(memory+read_index_u8);
		}
		/* Verify the signature of the data*/
		if ((data_from_mem[BHY_SIGNATURE_1] == BHY_IMAGE_SIGNATURE1)
		&& (data_from_mem[BHY_SIGNATURE_2] == BHY_IMAGE_SIGNATURE2)) {
			com_rslt = BHY_SUCCESS;
			} else {
			com_rslt = BHY_ERROR;
		}
		/* read the CRC data from memory */
		v_crc_from_memory_u32 = (u32)
		(((u32)data_from_mem[BHY_CRC_HOST_FILE_MSB]
		<< BHY_SHIFT_BIT_POSITION_BY_24_BITS) |
		((u32)data_from_mem[BHY_CRC_HOST_FILE_XXLSB]
		<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
		|(data_from_mem[BHY_CRC_HOST_FILE_XLSB]
		<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
		| (data_from_mem[BHY_CRC_HOST_FILE_LSB]));
		/* Remove the first 16 bytes*/
		data_to_process = v_file_length_u32 - BHY_SIGNATURE_LENGTH;
								
		/* set the reset as 0x01*/
		com_rslt = bhy_set_reset_request(BHY_RESET_ENABLE);
		com_rslt += bhy_write_reg(BHY_I2C_REG_CHIP_CONTROL_ADDR,
		&v_chip_control_u8, BHY_GEN_READ_WRITE_LENGTH);
		/* set the upload data*/
		com_rslt += bhy_write_reg(BHY_I2C_REG_UPLOAD_0_ADDR,
		&v_upload_addr, BHY_GEN_READ_WRITE_LENGTH);
		com_rslt += bhy_write_reg(BHY_I2C_REG_UPLOAD_1_ADDR,
		&v_upload_addr, BHY_GEN_READ_WRITE_LENGTH);
		/* write the chip control register as 0x02*/
		write_length =
		data_to_process / BHY_RAM_WRITE_LENGTH_API;
		read_index_u8 = BHY_INIT_VALUE;
		
		
		/* write the memory of data */
		/*skips first 16 bytes*/
		write_data += 16;
		if (com_rslt == BHY_SUCCESS) {
			for (read_index_u8 = BHY_INIT_VALUE;
			read_index_u8 <=
			write_length; read_index_u8++) {
				
				packet_length = read_index_u8 == write_length ? 
				(data_to_process % BHY_RAM_WRITE_LENGTH_API) / BHY_RAM_WRITE_LENGTH :
				BHY_RAM_WRITE_LENGTH_API / BHY_RAM_WRITE_LENGTH;
				
				/*reverse the data*/
				for (reverse_block_index_u32 = 1 ; reverse_block_index_u32 <= 
					packet_length ; 
					reverse_block_index_u32++) {
					for (reverse_index_u32 = 0; reverse_index_u32 < 
						BHY_RAM_WRITE_LENGTH; reverse_index_u32++) {
						data_byte[reverse_index_u32 + ((reverse_block_index_u32-1)*BHY_RAM_WRITE_LENGTH)] = 
						*(memory + write_data + BHY_RAM_WRITE_LENGTH * reverse_block_index_u32 
						- (reverse_index_u32+1));
						}
					}
				
				com_rslt +=
				bhy_write_reg(BHY_I2C_REG_UPLOAD_DATA_ADDR,
				data_byte,
				packet_length * BHY_RAM_WRITE_LENGTH);
				write_data = write_data
				+ (packet_length * BHY_RAM_WRITE_LENGTH);
			}
		}
		
		/* Check the CRC success*/
		com_rslt = bhy_get_crc_host(&v_crc_host_u32);
		if (v_crc_from_memory_u32 == v_crc_host_u32)
		com_rslt = BHY_SUCCESS;
		else
		com_rslt = BHY_ERROR;
		/* disable upload mode*/
		v_chip_control_u8 = BHY_CHIP_CTRL_ENABLE_2;
		/* write the chip control register as 0x02*/
		com_rslt += bhy_write_reg(BHY_I2C_REG_CHIP_CONTROL_ADDR,
		&v_chip_control_u8, BHY_GEN_READ_WRITE_LENGTH);

	}
	return com_rslt;

}
/*!
* @brief API used assign the data of meta event byte 0 data
*
* @param v_meta_event_data_u8: The value of meta event byte0
*
*/
static void bhy_assign_meta_event_byte0(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_5;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 1 data
*
* @param v_meta_event_data_u8: The value of meta event byte1
*
*/
static void bhy_assign_meta_event_byte1(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_5;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 2 data
*
* @param v_meta_event_data_u8: The value of meta event byte2
*
*/
static void bhy_assign_meta_event_byte2(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_5;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 3 data
*
* @param v_meta_event_data_u8: The value of meta event byte3
*
*/
static void bhy_assign_meta_event_byte3(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_5;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 4 data
*
* @param v_meta_event_data_u8: The value of meta event byte4
*
*/
static void bhy_assign_meta_event_byte4(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 5 data
*
* @param v_meta_event_data_u8: The value of meta event byte5
*
*/
static void bhy_assign_meta_event_byte5(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte6 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 6 data
*
* @param v_meta_event_data_u8: The value of meta event byte6
*
*/
static void bhy_assign_meta_event_byte6(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	v_meta_event_data_u8;
	write_buffer.write_parameter_byte8 =
	read_buffer.parameter_8;
}
/*!
* @brief API used assign the data of meta event byte 7 data
*
* @param v_meta_event_data_u8: The value of meta event byte7
*
*/
static void bhy_assign_meta_event_byte7(u8 v_meta_event_data_u8)
{
	write_buffer.write_parameter_byte1 =
	read_buffer.parameter_1;
	write_buffer.write_parameter_byte2 =
	read_buffer.parameter_2;
	write_buffer.write_parameter_byte3 =
	read_buffer.parameter_3;
	write_buffer.write_parameter_byte4 =
	read_buffer.parameter_4;
	write_buffer.write_parameter_byte5 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte6 =
	read_buffer.parameter_6;
	write_buffer.write_parameter_byte7 =
	read_buffer.parameter_7;
	write_buffer.write_parameter_byte8 =
	v_meta_event_data_u8;
}
/*!
 *	@brief API used to set the meta event for
 *	non wakeup FIFO control from
 *	system page-1 parameter 1
 *
 *
 *	@param v_meta_event_u8 : The value of meta event selection
 *       meta event          |   value
 * --------------------------|---------------
 *   BHY_META_EVENT_1            |  1
 *   BHY_META_EVENT_2            |  2
 *   BHY_META_EVENT_3            |  3
 *   BHY_META_EVENT_4            |  4
 *   BHY_META_EVENT_5            |  5
 *   BHY_META_EVENT_6            |  6
 *   BHY_META_EVENT_7            |  7
 *   BHY_META_EVENT_8            |  8
 *   BHY_META_EVENT_9            |  9
 *   BHY_META_EVENT_10           |  10
 *   BHY_META_EVENT_11           |  11
 *   BHY_META_EVENT_12           |  12
 *   BHY_META_EVENT_13           |  13
 *   BHY_META_EVENT_14           |  14
 *   BHY_META_EVENT_15           |  15
 *   BHY_META_EVENT_16           |  16
 *   BHY_META_EVENT_17           |  17
 *   BHY_META_EVENT_18           |  18
 *   BHY_META_EVENT_19           |  19
 *   BHY_META_EVENT_20           |  20
 *   BHY_META_EVENT_21           |  21
 *   BHY_META_EVENT_22           |  22
 *   BHY_META_EVENT_23           |  23
 *   BHY_META_EVENT_24           |  24
 *   BHY_META_EVENT_25           |  25
 *   BHY_META_EVENT_26           |  26
 *   BHY_META_EVENT_27           |  27
 *   BHY_META_EVENT_28           |  28
 *   BHY_META_EVENT_29           |  29
 *   BHY_META_EVENT_30           |  30
 *   BHY_META_EVENT_31           |  31
 *   BHY_META_EVENT_32           |  32

 *
 *	@param v_event_type_u8 : The value of
 *	interrupt or event selection
 *          value           |    Event
 * -------------------------|----------------
 *   BHY_META_EVENT_INTR_ENABLE |   0
 *   BHY_META_EVENT_ENABLE      |   1
 *
 *	@param v_input_data_u8 : The value of meta event
 *	@note Value for write each event in a bit
 *    value       |  Event or Interrupt
 * ---------------|----------------------
 *      0         |     DISABLE EVENT
 *      1         |     ENABLE EVENT
 *
 *	@param v_meta_parameter_request_u8: The value of parameter
 *	for wakeup or non wakeup fifo
 *
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_meta_event(
u8 v_meta_event_u8,
u8 v_input_data_u8, u8 v_event_type_u8,
u8 v_meta_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_meta_bytes_u8 = BHY_INIT_VALUE;
	/* read the stored meta events using saved parameter bytes*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, v_meta_parameter_request_u8);
	if (BHY_SUCCESS == com_rslt) {
		/* switch for selecting the different meta events*/
		switch (v_meta_event_u8) {
		/* case for meta event one*/
		case BHY_META_EVENT_1:
			/* switch for selecting interrupt or event*/
			switch (v_event_type_u8) {
			/* case for event enable*/
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 1, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			/* case for interrupt enable*/
			case BHY_META_INTR_ENABLE:
				/* set the meta event 1, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_1
				| v_input_data_u8);
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
			/* case for meta event 2*/
		case BHY_META_EVENT_2:
			/* case for interrupt or event enable*/
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 2, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 2, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		/* case for meta event 3*/
		case BHY_META_EVENT_3:
			switch (v_event_type_u8) {
			/* case for event or interrupt enable*/
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 3, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 3, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		/* case for meta event four*/
		case BHY_META_EVENT_4:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 4, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 4, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_1
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte0(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_5:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 5, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 5, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_2
				| v_input_data_u8);
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_6:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 6, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 6, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_7:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 7, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 7, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_8:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 8, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 8, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_2
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte1(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_9:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 9, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 9, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_3
				| v_input_data_u8);
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_10:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 10, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 10, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_11:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 11, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 11, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_12:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 12, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 12, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_3
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte2(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_13:
			/* set the meta event 13, event enable*/
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 13, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_4
				| v_input_data_u8);
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_14:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 14, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 14, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_15:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 15, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 15, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_16:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 16, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 16, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_4
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte3(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_17:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 17, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 17, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_5
				| v_input_data_u8);
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_18:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 18, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 18, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_19:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 19, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 19, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_20:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 20, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 20, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_5
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte4(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_21:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 21, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_6
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 21, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_6
				| v_input_data_u8);
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_22:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 22, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_6
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 22, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_6
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_23:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 23, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_6
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 23, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_6
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_24:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 24, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 24, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte5(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_25:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 25, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 25, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_7
				| v_input_data_u8);
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_26:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 26, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 26, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_27:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 27, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 27, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_28:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 28, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 28, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_7
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte6(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_29:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 29, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_01_BIT)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 29, interrupt enable*/
				v_meta_bytes_u8 = (
				read_buffer.parameter_8
				| v_input_data_u8);
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_30:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 30, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_03_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 30, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_02_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_31:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 31, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_05_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 31, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_04_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;
		case BHY_META_EVENT_32:
			switch (v_event_type_u8) {
			case BHY_META_EVENT_ENABLE:
				/* set the meta event 32, event enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_07_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			case BHY_META_INTR_ENABLE:
				/* set the meta event 32, interrupt enable*/
				v_meta_bytes_u8 = ((read_buffer.parameter_8
				& ~BHY_MASK_META_EVENT) |
				((v_input_data_u8
				<< BHY_SHIFT_BIT_POSITION_BY_06_BITS)
				& BHY_MASK_META_EVENT));
				bhy_assign_meta_event_byte7(v_meta_bytes_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		break;

		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	}
	/* write the meta event values to the meta event bytes*/
	com_rslt += bhy_write_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_WRITE_PARAMETER_1);
	return com_rslt;
}
/*!
 *	@brief API used to get the meta event control from
 *	system page-1 parameter 1
 *
 *	@param v_meta_event_u8 : The value of meta event selection
 *       meta event          |   value
 * --------------------------|---------------
 *   BHY_META_EVENT_1            |  1
 *   BHY_META_EVENT_2            |  2
 *   BHY_META_EVENT_3            |  3
 *   BHY_META_EVENT_4            |  4
 *   BHY_META_EVENT_5            |  5
 *   BHY_META_EVENT_6            |  6
 *   BHY_META_EVENT_7            |  7
 *   BHY_META_EVENT_8            |  8
 *   BHY_META_EVENT_9            |  9
 *   BHY_META_EVENT_10           |  10
 *   BHY_META_EVENT_11           |  11
 *   BHY_META_EVENT_12           |  12
 *   BHY_META_EVENT_13           |  13
 *   BHY_META_EVENT_14           |  14
 *   BHY_META_EVENT_15           |  15
 *   BHY_META_EVENT_16           |  16
 *   BHY_META_EVENT_17           |  17
 *   BHY_META_EVENT_18           |  18
 *   BHY_META_EVENT_19           |  19
 *   BHY_META_EVENT_20           |  20
 *   BHY_META_EVENT_21           |  21
 *   BHY_META_EVENT_22           |  22
 *   BHY_META_EVENT_23           |  23
 *   BHY_META_EVENT_24           |  24
 *   BHY_META_EVENT_25           |  25
 *   BHY_META_EVENT_26           |  26
 *   BHY_META_EVENT_27           |  27
 *   BHY_META_EVENT_28           |  28
 *   BHY_META_EVENT_29           |  29
 *   BHY_META_EVENT_30           |  30
 *   BHY_META_EVENT_31           |  31
 *   BHY_META_EVENT_32           |  32
 *
 *	@param meta_intr :
 *	The value of meta event interrupt selection
 *
 *	@param meta_event :
 *	The value of meta event event selection
 *
 *	@param v_event_type_u8 : The value of
 *	interrupt or event selection
 *          value           |    Event
 * -------------------------|----------------
 *   BHY_META_EVENT_INTR_ENABLE |   0
 *   BHY_META_EVENT_ENABLE      |   1
 *
 *	@param v_meta_parameter_request_u8: The value of parameter request
 *	for wakeup or non wakeup fifo
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_meta_event(
u8 v_meta_event_u8, u8 v_event_type_u8,
struct meta_event_intr_t *meta_intr,
struct meta_event_event_t *meta_event,
u8 v_meta_parameter_request_u8)
{
/* variable used for return the status of communication result*/
BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
u8 v_meta_event_byte_0_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_1_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_2_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_3_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_4_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_5_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_6_u8 = BHY_INIT_VALUE;
u8 v_meta_event_byte_7_u8 = BHY_INIT_VALUE;
/* check the p_bhy pointer as NULL*/
if (BHY_NULL_PTR == p_bhy) {
	com_rslt =  BHY_NULL;
} else {
	/* input as page 1 and parameter 1 for system page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, v_meta_parameter_request_u8);
	v_meta_event_byte_0_u8 = read_buffer.parameter_1;
	v_meta_event_byte_1_u8 = read_buffer.parameter_2;
	v_meta_event_byte_2_u8 = read_buffer.parameter_3;
	v_meta_event_byte_3_u8 = read_buffer.parameter_4;
	v_meta_event_byte_4_u8 = read_buffer.parameter_5;
	v_meta_event_byte_5_u8 = read_buffer.parameter_6;
	v_meta_event_byte_6_u8 = read_buffer.parameter_7;
	v_meta_event_byte_7_u8 = read_buffer.parameter_8;
	switch (v_meta_event_u8) {
	case BHY_META_EVENT_1:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event1_intr = (u8)
			(v_meta_event_byte_0_u8 &
			BHY_META_EVENT1_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event1_event = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT1_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_2:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event2_intr = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT2_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event2_event = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT2_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_3:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event3_intr = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT3_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event3_event = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT3_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_4:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event4_intr = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT4_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event4_event = (u8)
			((v_meta_event_byte_0_u8 &
			BHY_META_EVENT4_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_5:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event5_intr = (u8)
			(v_meta_event_byte_1_u8 &
			BHY_META_EVENT5_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event5_event = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT5_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_6:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event6_intr = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT6_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event6_event = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT6_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_7:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event7_intr = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT7_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event7_event = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT7_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_8:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event8_intr = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT8_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event8_event = (u8)
			((v_meta_event_byte_1_u8 &
			BHY_META_EVENT8_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_9:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event9_intr = (u8)
			(v_meta_event_byte_2_u8 &
			BHY_META_EVENT9_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event9_event = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT9_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_10:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event10_intr = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT10_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event10_event = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT10_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_11:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event11_intr = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT11_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event11_event = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT11_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_12:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event12_intr = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT12_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event12_event = (u8)
			((v_meta_event_byte_2_u8 &
			BHY_META_EVENT12_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_13:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event13_intr = (u8)
			(v_meta_event_byte_3_u8 &
			BHY_META_EVENT13_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event13_event = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT13_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_14:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event14_intr = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT14_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event14_event = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT14_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_15:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event15_intr = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT15_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event15_event = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT15_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_16:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event16_intr = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT16_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event16_event = (u8)
			((v_meta_event_byte_3_u8 &
			BHY_META_EVENT16_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_17:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event17_intr = (u8)
			(v_meta_event_byte_4_u8 &
			BHY_META_EVENT17_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event17_event = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT17_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_18:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event18_intr = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT18_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event18_event = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT18_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_19:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event19_intr = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT19_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event19_event = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT19_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_20:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event20_intr = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT20_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event20_event = (u8)
			((v_meta_event_byte_4_u8 &
			BHY_META_EVENT20_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_21:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event21_intr = (u8)
			(v_meta_event_byte_5_u8 &
			BHY_META_EVENT21_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event21_event = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT21_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_22:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event22_intr = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT22_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event22_event = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT22_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_23:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event23_intr = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT23_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event23_event = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT23_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_24:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event24_intr = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT24_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event24_event = (u8)
			((v_meta_event_byte_5_u8 &
			BHY_META_EVENT24_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_25:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event25_intr = (u8)
			(v_meta_event_byte_6_u8 &
			BHY_META_EVENT25_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event25_event = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT25_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_26:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event26_intr = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT26_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event26_event = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT26_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_27:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event27_intr = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT27_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event27_event = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT27_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_28:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event28_intr = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT28_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event28_event = (u8)
			((v_meta_event_byte_6_u8 &
			BHY_META_EVENT28_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_29:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event29_intr = (u8)
			(v_meta_event_byte_7_u8 &
			BHY_META_EVENT29_INTR_ENABLE);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event29_event = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT29_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_30:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event30_intr = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT30_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event30_event = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT30_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_31:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event31_intr = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT31_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event31_event = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT31_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	case BHY_META_EVENT_32:
		switch (v_event_type_u8) {
		case BHY_META_INTR_ENABLE:
			meta_intr->meta_event32_intr = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT32_INTR_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_06_BITS);
		break;
		case BHY_META_EVENT_ENABLE:
			meta_event->meta_event32_event = (u8)
			((v_meta_event_byte_7_u8 &
			BHY_META_EVENT32_EVENT_ENABLE)
			>> BHY_SHIFT_BIT_POSITION_BY_07_BITS);
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	break;
	default:
		com_rslt = BHY_OUT_OF_RANGE;
	break;
	}
}
return com_rslt;
}
/*!
 *	@brief API used to get the fifo water mark from
 *	system page-1 parameter 2
 *
 *
 *	@param v_parameter_u8 :
 *	The value of fifo water mark wakeup or non-wakeup selection
 *       water mark          |   value
 * --------------------------|---------------
 *   FIFO_WM_WAKEUP          |  0
 *   FIFO_WM_NON_WAKEUP      |  1
 *
 *	@param v_fifo_water_mark_u16: The value of fifo water mark
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_fifo_water_mark(
u8 v_parameter_u8, u16 *v_fifo_water_mark_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_fifo_wm_wakeup_lsb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_wm_wakeup_msb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_wm_non_wakeup_lsb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_wm_non_wakeup_msb_u8 = BHY_INIT_VALUE;
	/* input as page 1 and parameter 2 for system page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_READ_PARAMETER_2);
	if (BHY_SUCCESS == com_rslt) {
			switch (v_parameter_u8) {
			case BHY_FIFO_WATER_MARK_WAKEUP:
				/* fifo wakeup water mark*/
				v_fifo_wm_wakeup_lsb_u8 =
				read_buffer.parameter_1;
				v_fifo_wm_wakeup_msb_u8 =
				read_buffer.parameter_2;
				*v_fifo_water_mark_u16 =
				(u16)((v_fifo_wm_wakeup_msb_u8
				<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
				|(v_fifo_wm_wakeup_lsb_u8));
			break;
			case BHY_FIFO_WATER_MARK_NON_WAKEUP:
				/* fifo non wakeup water mark*/
				v_fifo_wm_non_wakeup_lsb_u8 =
				read_buffer.parameter_5;
				v_fifo_wm_non_wakeup_msb_u8 =
				read_buffer.parameter_6;
				*v_fifo_water_mark_u16 =
				(u16)((v_fifo_wm_non_wakeup_msb_u8
				<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
				|(v_fifo_wm_non_wakeup_lsb_u8));
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		}
	return com_rslt;
}
/*!
 *	@brief API used to set the fifo water mark from
 *	system page-1 parameter 2
 *
 *
 *	@param v_parameter_u8 :
 *	The value of fifo water mark wakeup or non-wakeup selection
 *       water mark          |   value
 * --------------------------|---------------
 *   FIFO_WM_WAKEUP          |  0
 *   FIFO_WM_NON_WAKEUP      |  1
 *
 *	@param v_fifo_water_mark_u16: The value of fifo water mark
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_fifo_water_mark(u8 v_parameter_u8,
u16 v_fifo_water_mark_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_fifo_wm_lsb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_wm_msb_u8 = BHY_INIT_VALUE;
	/* read the fifo water mark using saved parameter bytes*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_READ_PARAMETER_2);
	if (BHY_SUCCESS == com_rslt) {
		v_fifo_wm_lsb_u8 = (u8)(v_fifo_water_mark_u16
		& BHY_MASK_LSB_DATA);
		v_fifo_wm_msb_u8 = (u8)((v_fifo_water_mark_u16 & 
		BHY_MASK_MSB_DATA)>> 
		BHY_SHIFT_BIT_POSITION_BY_08_BITS);
		switch (v_parameter_u8) {
		/* fifo wakeup water mark*/
		case BHY_FIFO_WATER_MARK_WAKEUP:
			write_buffer.write_parameter_byte1 = v_fifo_wm_lsb_u8;
			write_buffer.write_parameter_byte2 = v_fifo_wm_msb_u8;
			write_buffer.write_parameter_byte4
			= read_buffer.parameter_4;
			write_buffer.write_parameter_byte5
			= read_buffer.parameter_5;
		break;
		/* fifo non wakeup water mark*/
		case BHY_FIFO_WATER_MARK_NON_WAKEUP:
			write_buffer.write_parameter_byte1
			= read_buffer.parameter_1;
			write_buffer.write_parameter_byte2
			= read_buffer.parameter_2;
			write_buffer.write_parameter_byte5 = v_fifo_wm_lsb_u8;
			write_buffer.write_parameter_byte6 = v_fifo_wm_msb_u8;
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	}
	com_rslt += bhy_write_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_WRITE_PARAMETER_2);
	return com_rslt;
}
/*!
 *	@brief API used to get the fifo size from
 *	system page-1 parameter 2
 *	@note wakeup - bytes from 2 and 3
 *	@note non wakeup - bytes from 6 and 7
 *
 *
 *	@param v_fifo_size_select_u8 :
 *	The value of fifo size mark wakeup or non-wakeup selection
 *       water mark          |   value
 * --------------------------|---------------
 *   BHY_FIFO_SIZE_WAKEUP        |  0
 *   BHY_FIFO_SIZE_NON_WAKEUP    |  1
 *
 *	@param v_fifo_size_u16 : The value of fifo size
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_fifo_size(
u8 v_fifo_size_select_u8, u16 *v_fifo_size_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_fifo_size_wakeup_lsb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_size_wakeup_msb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_size_non_wakeup_lsb_u8 = BHY_INIT_VALUE;
	u8 v_fifo_size_non_wakeup_msb_u8 = BHY_INIT_VALUE;
	/* check the p_bhy pointer as NULL*/
	if (BHY_NULL_PTR == p_bhy) {
		com_rslt =  BHY_NULL;
		} else {
			/* input as page 1 and parameter 2 for system page*/
			com_rslt = bhy_read_parameter_bytes(
			BHY_PAGE_1, BHY_PARAMETER_REQUEST_READ_PARAMETER_2);
			if (BHY_SUCCESS == com_rslt) {
				switch (v_fifo_size_select_u8) {
				case BHY_FIFO_SIZE_WAKEUP:
					/* fifo size in wakeup*/
					v_fifo_size_wakeup_lsb_u8 =
					read_buffer.parameter_3;
					v_fifo_size_wakeup_msb_u8 =
					read_buffer.parameter_4;
					*v_fifo_size_u16 =
					(u16)((v_fifo_size_wakeup_msb_u8
					<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
					|(v_fifo_size_wakeup_lsb_u8));
				break;
				case BHY_FIFO_SIZE_NON_WAKEUP:
					/* fifo size in non wakeup*/
					v_fifo_size_non_wakeup_lsb_u8 =
					read_buffer.parameter_7;
					v_fifo_size_non_wakeup_msb_u8 =
					read_buffer.parameter_8;
					*v_fifo_size_u16 =
					(u16)((v_fifo_size_non_wakeup_msb_u8
					<< BHY_SHIFT_BIT_POSITION_BY_08_BITS)
					|(v_fifo_size_non_wakeup_lsb_u8));
				break;
				default:
					com_rslt = BHY_OUT_OF_RANGE;
				break;
				}
			}
		}
	return com_rslt;
}
/*!
 *	@brief API used to assign the sensor status bank data
 *
 *
 *	@param v_sensor_status_u8 :
 *	The value for saved parameter data
 *
 *
 *	@return Nothing
 *
 *
*/
static void bhy_assign_sensor_status_bank(u8 v_sensor_status_u8)
{
	/* Assign the sensor status bank*/
	/* data available*/
	sensor_status_bank.data_available = (u8)
	(v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_DATA_AVAILABLE);
	/* i2c nack*/
	sensor_status_bank.i2c_nack = (u8)
	((v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_I2C_NACK)
	>> BHY_SHIFT_BIT_POSITION_BY_01_BIT);
	/* device id error*/
	sensor_status_bank.device_id_error = (u8)
	((v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_DEVICE_ID_ERROR)
	>> BHY_SHIFT_BIT_POSITION_BY_02_BITS);
	/* transient error*/
	sensor_status_bank.transient_error = (u8)
	((v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_TRANSIENT_ERROR)
	>> BHY_SHIFT_BIT_POSITION_BY_03_BITS);
	/* data lost*/
	sensor_status_bank.data_lost = (u8)
	((v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_DATA_LOST)
	>> BHY_SHIFT_BIT_POSITION_BY_04_BITS);
	/* sensor power mode*/
	sensor_status_bank.sensor_power_mode = (u8)
	((v_sensor_status_u8
	& BHY_SENSOR_BANK_STATUS_POWER_MODE)
	>> BHY_SHIFT_BIT_POSITION_BY_05_BITS);

}
/*!
 *	@brief API used to get the sensor status bank from
 *	system page-1 parameter 3 to 6
 *	@note Sensor status bank 0 : parameter 3 contains 1 to 16 sensor type
 *	@note Sensor status bank 1 : parameter 4 contains 17 to 32 sensor type
 *	@note Sensor status bank 2 : parameter 5 contains 33 to 48 sensor type
 *	@note Sensor status bank 3 : parameter 6 contains 49 to 64 sensor type
 *
 *
 *	@param v_sensor_type_u8 :
 *	The value of sensor status bank sensor type selection
 *       v_sensor_type_u8             |   value
 * -----------------------------------|---------------
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_1   |  0
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_2   |  1
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_3   |  3
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_4   |  4
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_5   |  5
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_6   |  6
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_7   |  7
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_8   |  8
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_9   |  9
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_10  |  10
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_11  |  11
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_12  |  12
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_13  |  13
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_14  |  14
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_15  |  15
 *   BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_16  |  16
 *
 *
 *	@param v_sensor_status_parameter_u8 :
 *	The value of sensor status bank parameter selection
 *         parameter               |    value
 *  -------------------------------|-----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_3 |  0x03
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_4 |  0x04
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_5 |  0x05
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_6 |  0x06
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_sensor_status_bank(
u8 v_sensor_status_parameter_u8, u8 v_sensor_type_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_data_u8 = BHY_INIT_VALUE;
		/* input as page 1 and parameter
		3(4 or 5 or 6)for system page*/
		com_rslt = bhy_read_parameter_bytes(
		BHY_PAGE_1, v_sensor_status_parameter_u8);
		if (BHY_SUCCESS == com_rslt) {
			switch (v_sensor_type_u8) {
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_1:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_1;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_2:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_2;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_3:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_3;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_4:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_4;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_5:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_5;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_6:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_6;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_7:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_7;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_8:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_8;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_9:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_9;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_10:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_10;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_11:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_11;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_12:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_12;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_13:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_13;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_14:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_14;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_15:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_15;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			case BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_16:
				/* sensor status of data available*/
				v_data_u8 = read_buffer.parameter_16;
				bhy_assign_sensor_status_bank(v_data_u8);
			break;
			default:
				com_rslt = BHY_OUT_OF_RANGE;
			break;
			}
		}
	return com_rslt;
}
/*!
 *	@brief API used to get the host IRQ time
 *	stamp and current time stamp from
 *	system page-1 parameter 30
 *	Host IRQ time stamp bytes 0 to 3
 *	Current time stamp bytes 4 to 7
 *
 *
 *	@param v_time_stamp_selection_u8 :
 *	The value of Host IRQ or current time stamp selection
 *       time stamp          |   value
 * --------------------------|---------------
 *   BHY_HOST_IRQ_TIMESTAMP     |  0
 *   BHY_CURRENT_TIME_STAMP      |  1
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *	@param v_time_stamp_u32 :
 *	The value of Host IRQ or current time stamp
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_time_stamp(
u8 v_time_stamp_selection_u8, u32 *v_time_stamp_u32)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u16 v_lsb_time_u16 = BHY_INIT_VALUE;
	u16 v_msb_time_u16 = BHY_INIT_VALUE;
	/* input as page 1 and parameter 30 for system page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_READ_PARAMETER_30);
	if (BHY_SUCCESS == com_rslt) {
		switch (v_time_stamp_selection_u8) {
		case BHY_HOST_IRQ_TIMESTAMP:
			/* host IRQ time stamp*/
			/* IRQ time stamp lsb data*/
			v_lsb_time_u16 = (u16)((read_buffer.parameter_2
			 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			 | (read_buffer.parameter_1));
			 /* IRQ time stamp msb data*/
			v_msb_time_u16 = (u16)((read_buffer.parameter_4
			 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			 | (read_buffer.parameter_3));
			 /* return IRQ time stamp*/
			*v_time_stamp_u32 = (u32)(((u32)v_msb_time_u16
			<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
			| (v_lsb_time_u16));
		break;
		case BHY_CURRENT_TIME_STAMP:
			/* current time stamp*/
			 /* current time stamp lsb data*/
			v_lsb_time_u16 = (u16)((read_buffer.parameter_6
			 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			 | (read_buffer.parameter_5));
			 /* current time stamp msb data*/
			 v_msb_time_u16 = (u16)((read_buffer.parameter_8
			 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
			 | (read_buffer.parameter_7));
			 /* return current time stamp*/
			 *v_time_stamp_u32 = (u32)(((u32)v_msb_time_u16
			<< BHY_SHIFT_BIT_POSITION_BY_16_BITS)
			| (v_lsb_time_u16));
		break;
		default:
			com_rslt = BHY_OUT_OF_RANGE;
		break;
		}
	}
	return com_rslt;
}
/*!
 *	@brief API used to get the physical sensor status
 *	system page-1 parameter 31
 *	@note Accel sample rate byte 0 and 1
 *	@note Accel dynamic range byte 2 and 3
 *	@note Accel flags byte 4
 *	@note Gyro sample rate byte 5 and 6
 *	@note Gyro dynamic range byte 7 and 8
 *	@note Gyro flags byte 9
 *	@note Mag sample rate byte 10 and 11
 *	@note Mag dynamic range byte 12 and 13
 *	@note Mag flags byte 14
 *
 *	@param accel_status : contains the accel physical status
 *	@param gyro_status : contains the gyro physical status
 *	@param mag_status : contains the mag physical status
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_physical_sensor_status(
struct accel_physical_status_t *accel_status,
struct gyro_physical_status_t *gyro_status,
struct mag_physical_status_t *mag_status)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 1 and parameter 31 for system page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_1, BHY_PARAMETER_REQUEST_READ_PARAMETER_31);
	/* accel physical status*/
	/* accl sample rate*/
	accel_status->accel_sample_rate = (u16)((read_buffer.parameter_2
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_1));
	/* accel dynamic range*/
	accel_status->accel_dynamic_range = (u16)((read_buffer.parameter_4
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_3));
	/* accel flag*/
	accel_status->accel_flag = read_buffer.parameter_5;
	/* gyro physical status*/
	/* gyro sample rate*/
	gyro_status->gyro_sample_rate = (u16)((read_buffer.parameter_7
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_6));
	/* accel dynamic range*/
	gyro_status->gyro_dynamic_range = (u16)((read_buffer.parameter_9
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_8));
	/* accel flag*/
	gyro_status->gyro_flag = (u8)(read_buffer.parameter_10);
	/* mag physical status*/
	/* mag sample rate*/
	mag_status->mag_sample_rate = (u16)((read_buffer.parameter_12
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_11));
	/* mag dynamic range*/
	mag_status->mag_dynamic_range = (u16)((read_buffer.parameter_14
	<< BHY_SHIFT_BIT_POSITION_BY_08_BITS) | (read_buffer.parameter_13));
	/* accel flag*/
	mag_status->mag_flag = (u8)(read_buffer.parameter_15);

	return com_rslt;
}
/*!
 *	@brief API used to get the non wakeup sensor information,
 *	Sensor page-3 parameter 1 to 31
 *
 *	@param v_parameter_request_u8 :
 *	Value desired parameter to read non wakeup sensor information
 *   param_request                   | value | Virtual sensor
 * ----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_1   | 0x01  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_2   | 0x02  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_3   | 0x03  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_4   | 0x04  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_5   | 0x05  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_6   | 0x06  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_7   | 0x87  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_8   | 0x88  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_9   | 0x89  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_10  | 0x8A  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_11  | 0x8B  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_12  | 0x8C  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_13  | 0x8D  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_14  | 0x8E  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_15  | 0x8F  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_16  | 0x90  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_17  | 0x91  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_18  | 0x92  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_19  | 0x93  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_20  | 0x94  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_21  | 0x95  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_22  | 0x96  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_23  | 0x97  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_24  | 0x98  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_25  | 0x99  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_31  | 0x9F  | Activity
 *
 *	@param sensor_information :
 *	holds the value of non wakeup sensor information
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_non_wakeup_sensor_information(
u8 v_parameter_request_u8,
struct sensor_information_non_wakeup_t *sensor_information)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 3 and parameter request for sensor page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);
	/* sensor type information */
	sensor_information->non_wakeup_sensor_type
	= (u8)(read_buffer.parameter_1);
	/* driver id information */
	sensor_information->non_wakeup_driver_id =
	(u8)(read_buffer.parameter_2);
	/* driver version information */
	sensor_information->non_wakeup_driver_version =
	(u8)(read_buffer.parameter_3);
	/* power information */
	sensor_information->non_wakeup_power =
	(u8)(read_buffer.parameter_4);
	/* maximum range information */
	sensor_information->non_wakeup_max_range = (u16)
	((read_buffer.parameter_6 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_5));
	/* resolution information */
	sensor_information->non_wakeup_resolution = (u16)
	((read_buffer.parameter_8 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_7));
	/* maximum rate information */
	sensor_information->non_wakeup_max_rate = (u16)
	((read_buffer.parameter_10 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_9));
	/* fifo reserved information */
	sensor_information->non_wakeup_fifo_reserved = (u16)
	((read_buffer.parameter_12 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_11));
	/* fifo max information */
	sensor_information->non_wakeup_fifo_max = (u16)
	((read_buffer.parameter_14 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_13));
	/* event size information */
	sensor_information->non_wakeup_event_size = read_buffer.parameter_15;
	/* minimum rate information */
	sensor_information->non_wakeup_min_rate = read_buffer.parameter_16;

	return com_rslt;
}
/*!
 *	@brief API used to get the wakeup sensor information,
 *	Sensor page-3 parameter 32 to 63
 *
 *	@param v_parameter_request_u8 :
 *	Value desired parameter to read non wakeup sensor information
 *   param_request                   | value | Virtual sensor
 * ----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_33  | 0xA1  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_34  | 0xA2  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_35  | 0xA3  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_36  | 0xA4  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_37  | 0xA5  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_38  | 0xA6  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_39  | 0xA7  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_40  | 0xA8  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_41  | 0xA9  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_42  | 0xAA  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_43  | 0xAB  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_44  | 0xAC  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_45  | 0xAD  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_46  | 0xAE  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_47  | 0xAF  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_48  | 0xB0  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_49  | 0xB1  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_50  | 0xB2  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_51  | 0xB3  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_52  | 0xB4  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_53  | 0xB5  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_54  | 0xB6  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_55  | 0xB7  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_56  | 0xB8  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_57  | 0xB9  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_63  | 0xBF  | Activity
 *
 *	@param sensor_information :
 *	holds the value of non wakeup sensor information
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_wakeup_sensor_information(
u8 v_parameter_request_u8,
struct sensor_information_wakeup_t *sensor_information)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 3 and parameter request for sensor page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);
	/* sensor type information */
	sensor_information->wakeup_sensor_type =
	(u8)(read_buffer.parameter_1);
	/* driver id information */
	sensor_information->wakeup_driver_id =
	(u8)(read_buffer.parameter_2);
	/* driver version information */
	sensor_information->wakeup_driver_version =
	(u8)(read_buffer.parameter_3);
	/* power information */
	sensor_information->wakeup_power =
	(u8)(read_buffer.parameter_4);
	/* maximum range information */
	sensor_information->wakeup_max_range = (u16)
	((read_buffer.parameter_6 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_5));
	/* resolution information */
	sensor_information->wakeup_resolution = (u16)
	((read_buffer.parameter_8 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_7));
	/* maximum rate information */
	sensor_information->wakeup_max_rate = (u16)
	((read_buffer.parameter_10 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_9));
	/* fifo reserved information */
	sensor_information->wakeup_fifo_reserved = (u16)
	((read_buffer.parameter_12 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_11));
	/* fifo max information */
	sensor_information->wakeup_fifo_max = (u16)
	((read_buffer.parameter_14 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_13));
	/* event size information */
	sensor_information->wakeup_event_size = read_buffer.parameter_15;
	/* minimum rate information */
	sensor_information->wakeup_min_rate = read_buffer.parameter_16;

	return com_rslt;
}
/*!
 *	@brief API used to set the sensor non wakeup configuration
 *	Sensor page-3 parameter 65 to 89
 *
 *	@param sensor_configuration : contains the non wakeup sensor configuration
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *   param_request                   | value | Virtual sensor
 * ----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_65  | 0xC1  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_66  | 0xC2  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_67  | 0xC3  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_68  | 0xC4  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_69  | 0xC5  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_70  | 0xC6  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_71  | 0xC7  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_72  | 0xC8  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_73  | 0xC9  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_74  | 0xCA  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_75  | 0xCB  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_76  | 0xCC  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_77  | 0xCD  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_78  | 0xCE  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_79  | 0xCF  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_80  | 0xD0  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_81  | 0xD1  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_82  | 0xD2  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_83  | 0xD3  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_84  | 0xD4  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_85  | 0xD5  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_86  | 0xD6  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_87  | 0xD7  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_88  | 0xD8  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_89  | 0xD9  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_95  | 0xDF  | Activity
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_non_wakeup_sensor_configuration(
struct sensor_configuration_non_wakeup_t *sensor_configuration,
u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* write sample rate*/
	write_buffer.write_parameter_byte1 = (u8)(
	sensor_configuration->non_wakeup_sample_rate & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte2 = (u8)(
	(sensor_configuration->non_wakeup_sample_rate
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write maximum report latency*/
	write_buffer.write_parameter_byte3 = (u8)(
	sensor_configuration->non_wakeup_max_report_latency
	& BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte4 = (u8)(
	(sensor_configuration->non_wakeup_max_report_latency
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write sensitivity*/
	write_buffer.write_parameter_byte5 = (u8)(
	sensor_configuration->non_wakeup_change_sensitivity
	 & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte6 = (u8)(
	(sensor_configuration->non_wakeup_change_sensitivity
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write dynamic range*/
	write_buffer.write_parameter_byte7 = (u8)(
	sensor_configuration->non_wakeup_dynamic_range & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte8 = (u8)(
	(sensor_configuration->non_wakeup_dynamic_range
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* load the parameter of non wakeup sensor configuration*/
	com_rslt = bhy_write_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);

	return com_rslt;
}
/*!
 *	@brief API used to get the sensor non wakeup configuration
 *	Sensor page-3 parameter 65 to 89
 *
 *	@param v_sample_rate_u16 :
 *	contains the non wakeup sample rate data
 *	@param v_max_report_latency_u16:
 *	contains the non wakeup max report latency
 *	@param v_change_sensitivity_u16:
 *	contains the non wakeup sensitivity
 *	@param v_dynamic_range_u16:
 *	contains the non wakeup dynamic range
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *   param_request                   | value | Virtual sensor
 * ----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_65  | 0xC1  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_66  | 0xC2  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_67  | 0xC3  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_68  | 0xC4  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_69  | 0xC5  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_70  | 0xC6  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_71  | 0xC7  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_72  | 0xC8  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_73  | 0xC9  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_74  | 0xCA  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_75  | 0xCB  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_76  | 0xCC  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_77  | 0xCD  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_78  | 0xCE  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_79  | 0xCF  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_80  | 0xD0  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_81  | 0xD1  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_82  | 0xD2  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_83  | 0xD3  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_84  | 0xD4  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_85  | 0xD5  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_86  | 0xD6  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_87  | 0xD7  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_88  | 0xD8  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_89  | 0xD9  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_95  | 0xDF  | Activity
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_non_wakeup_sensor_configuration(
u8 v_parameter_request_u8,
u16 *v_sample_rate_u16, u16 *v_max_report_latency_u16,
u16 *v_change_sensitivity_u16, u16 *v_dynamic_range_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 3 and parameter request for sensor page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);
	/* sample rate information */
	*v_sample_rate_u16 = (u16)
	((read_buffer.parameter_2 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_1));
	/* max report latency information */
	*v_max_report_latency_u16 = (u16)
	((read_buffer.parameter_4 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_3));
	/* sensitivity information */
	*v_change_sensitivity_u16 = (u16)
	((read_buffer.parameter_6 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_5));
	/* dynamic range information */
	*v_dynamic_range_u16 = (u16)
	((read_buffer.parameter_8 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_7));

	return com_rslt;
}
/*!
 *	@brief API used to set the sensor wakeup configuration
 *	Sensor page-3 parameter 65 to 89
 *
 *	@param sensor_configuration : contains the wakeup sensor configuration
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *   param_request                    | value | Virtual sensor
 * -----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_97   | 0xE1  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_98   | 0xE2  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_99   | 0xE3  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_100  | 0xE4  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_101  | 0xE5  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_102  | 0xE6  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_103  | 0xE7  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_104  | 0xE8  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_105  | 0xE9  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_106  | 0xEA  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_107  | 0xEB  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_108  | 0xEC  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_109  | 0xED  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_110  | 0xEE  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_111  | 0xEF  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_112  | 0xF0  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_113  | 0xF1  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_114  | 0xF2  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_115  | 0xF3  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_116  | 0xF4  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_117  | 0xF5  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_118  | 0xF6  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_119  | 0xF7  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_120  | 0xF8  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_121  | 0xF9  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_127  | 0xFF  | Activity
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_wakeup_sensor_configuration(
struct sensor_configuration_wakeup_t *sensor_configuration,
u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* write sample rate*/
	write_buffer.write_parameter_byte1 = (u8)(
	sensor_configuration->wakeup_sample_rate & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte2 = (u8)(
	(sensor_configuration->wakeup_sample_rate
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write maximum report latency*/
	write_buffer.write_parameter_byte3 = (u8)(
	sensor_configuration->wakeup_max_report_latency & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte4 = (u8)(
	(sensor_configuration->wakeup_max_report_latency
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write change sensitivity*/
	write_buffer.write_parameter_byte5 = (u8)(
	sensor_configuration->wakeup_change_sensitivity & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte6 = (u8)(
	(sensor_configuration->wakeup_change_sensitivity
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* write dynamic range*/
	write_buffer.write_parameter_byte7 = (u8)(
	sensor_configuration->wakeup_dynamic_range & BHY_MASK_LSB_DATA);
	write_buffer.write_parameter_byte8 = (u8)(
	(sensor_configuration->wakeup_dynamic_range
	& BHY_MASK_MSB_DATA) >> BHY_SHIFT_BIT_POSITION_BY_08_BITS);
	/* load the parameter of non wakeup sensor configuration*/
	com_rslt = bhy_write_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);

	return com_rslt;
}
/*!
 *	@brief API used to get the sensor wakeup configuration
 *	Sensor page-3 parameter 65 to 89
 *
 *	@param v_sample_rate_u16 : contains the  wakeup sample rate data
 *	@param v_max_report_latency_u16: contains the  wakeup max report latency
 *	@param v_change_sensitivity_u16: contains the  wakeup sensitivity
 *	@param v_dynamic_range_u16: contains the  wakeup dynamic range
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *   param_request                    | value | Virtual sensor
 * -----------------------------------|-------|----------------
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_97   | 0xE1  | Accelerometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_98   | 0xE2  | Magnetometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_99   | 0xE3  | Orientation
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_100  | 0xE4  | Gyroscope
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_101  | 0xE5  | Light
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_102  | 0xE6  | Barometer
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_103  | 0xE7  | Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_104  | 0xE8  | Proximity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_105  | 0xE9  | Gravity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_106  | 0xEA  | Liner accel
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_107  | 0xEB  | Rotation vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_108  | 0xEC  | Humidity
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_109  | 0xED  | Ambient Temperature
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_110  | 0xEE  | Uncalibrated Mag
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_111  | 0xEF  | Game rotation Vector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_112  | 0xF0  | Uncalibrated Gyro
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_113  | 0xF1  | Signification Motion
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_114  | 0xF2  | Step detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_115  | 0xF3  | Step Counter
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_116  | 0xF4  | Geomagnetic
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_117  | 0xF5  | Heart Rate
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_118  | 0xF6  | Tilt Detector
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_119  | 0xF7  | Wakeup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_120  | 0xF8  | Glance Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_121  | 0xF9  | Pickup Gesture
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_127  | 0xFF  | Activity
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_wakeup_sensor_configuration(
u8 v_parameter_request_u8,
u16 *v_sample_rate_u16, u16 *v_max_report_latency_u16,
u16 *v_change_sensitivity_u16, u16 *v_dynamic_range_u16)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 3 and parameter request for sensor page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_3, v_parameter_request_u8);
	/* sample rate information */
	*v_sample_rate_u16 = (u16)
	((read_buffer.parameter_2 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_1));
	/* max report latency information */
	*v_max_report_latency_u16 = (u16)
	((read_buffer.parameter_4 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_3));
	/* sensitivity information */
	*v_change_sensitivity_u16 = (u16)
	((read_buffer.parameter_6 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_5));
	/* dynamic range information */
	*v_dynamic_range_u16 = (u16)
	((read_buffer.parameter_8 << BHY_SHIFT_BIT_POSITION_BY_08_BITS)
	| (read_buffer.parameter_7));

	return com_rslt;
}
/*!
 *	@brief API used to get the soft pass through
 *	Sensor page-15 parameter 0 to 7
 *
 *	@param soft_pass_through :
 *	contains the value of soft pass through
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *              parameter               |        value
 *  ------------------------------------|--------------------
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_1   |       0x01
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_2   |       0x02
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_3   |       0x03
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_soft_pass_through(
struct soft_pass_through_read_t *soft_pass_through,
u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* input as page 3 and parameter request for sensor page*/
	com_rslt = bhy_read_parameter_bytes(
	BHY_PAGE_15, v_parameter_request_u8);
	/* i2c slave address information */
	soft_pass_through->i2c_slave_address =
	(u8)(read_buffer.parameter_1);
		/* start register information */
	soft_pass_through->start_register =
	(u8)(read_buffer.parameter_2);
		/* read length information */
	soft_pass_through->read_length =
	(u8)(read_buffer.parameter_3);
		/* completion status information */
	soft_pass_through->completion_status =
	(u8)(read_buffer.parameter_4);
		/* return register value 1 information */
	soft_pass_through->reg_value_byte1 =
	(u8)(read_buffer.parameter_5);
		/* return register value 2 information */
	soft_pass_through->reg_value_byte2 =
	(u8)(read_buffer.parameter_6);
		/* return register value 3 information */
	soft_pass_through->reg_value_byte3 =
	(u8)(read_buffer.parameter_7);
		/* return register value 4 information */
	soft_pass_through->reg_value_byte4 =
	(u8)(read_buffer.parameter_8);

	return com_rslt;
}
/*!
 *	@brief API used to set the soft pass through
 *	Sensor page-15 parameter 0 to 7
 *
 *	@param soft_pass_through :
 *	contains the value of soft pass through
 *
 *	@param v_parameter_request_u8: value of selected parameter request
 *              parameter               |        value
 *  ------------------------------------|--------------------
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_1   |       0x01
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_2   |       0x02
 *    BHY_PARAMETER_REQUEST_WRITE_PARAMETER_3   |       0x03
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_soft_pass_through(
struct soft_pass_through_write_t *soft_pass_through,
u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	/* i2c slave address information */
	write_buffer.write_parameter_byte1 =
	soft_pass_through->i2c_slave_address;
	/* start register information */
	write_buffer.write_parameter_byte2 =
	soft_pass_through->start_register;
	/* write length information */
	write_buffer.write_parameter_byte3 =
	soft_pass_through->write_length;
	/* completion status information */
	write_buffer.write_parameter_byte4 =
	soft_pass_through->completion_status;
	/* return register value 1 information */
	write_buffer.write_parameter_byte5 =
	soft_pass_through->reg_value_byte1;
	/* return register value 2 information */
	write_buffer.write_parameter_byte6 =
	soft_pass_through->reg_value_byte2;
	/* return register value 3 information */
	write_buffer.write_parameter_byte7 =
	soft_pass_through->reg_value_byte3;
	/* return register value 4 information */
	write_buffer.write_parameter_byte8 =
	soft_pass_through->reg_value_byte4;
	/* load the parameter of soft
	pass through sensor configuration*/
	com_rslt = bhy_write_parameter_bytes(
	BHY_PAGE_15, v_parameter_request_u8);
	return com_rslt;
}

/*!
 *	@brief API used to get the data from the parameter
 *	I2C page register from 0x3B to 0x4A
 *
 *
 *	@param v_page_select_u8 : This input value for set the desired page
 *	@param v_parameter_request_u8 :
 *	This input value for set the desired parameter
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_read_parameter_bytes(
u8 v_page_select_u8, u8 v_parameter_request_u8)
{
	/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_parameter_ack_u8 = BHY_INIT_VALUE;
	u8 init_array_data = BHY_INIT_VALUE;
	u8 a_read_data_u8[BHY_READ_BUFFER_SIZE];
	u8 v_parameter_ack_check_u8 = BHY_INIT_VALUE;

	for (; init_array_data < BHY_READ_BUFFER_SIZE; init_array_data++)
		a_read_data_u8[init_array_data] = BHY_INIT_VALUE;
	/* select the page*/
	com_rslt = bhy_set_parameter_page_select(v_page_select_u8);
	/* select the parameter*/
	com_rslt += bhy_set_parameter_request(v_parameter_request_u8);
	/* read the values*/
	for (v_parameter_ack_check_u8 = BHY_INIT_VALUE;
	v_parameter_ack_check_u8 < BHY_PARAMETER_ACK_LENGTH;
	v_parameter_ack_check_u8++) {
		/* read acknowledgement*/
		com_rslt = bhy_get_parameter_acknowledge(&v_parameter_ack_u8);
		if (v_parameter_ack_u8 == v_parameter_request_u8) {
				break;
		} else if (v_parameter_ack_u8 == BHY_PARAMETER_ACK_CHECK) {
			p_bhy->delay_msec(BHY_PARAMETER_ACK_DELAY);
			com_rslt = BHY_ERROR;
		} else {
				/* device not ready yet */
				p_bhy->delay_msec(1);
		}
	}
	com_rslt =
	p_bhy->BHY_BUS_READ_FUNC(p_bhy->device_addr,
	BHY_I2C_REG_PARAMETER_READ_BUFFER_ZERO,
	a_read_data_u8, BHY_READ_BUFFER_LENGTH);
	read_buffer.parameter_1 =
	a_read_data_u8[BHY_READ_BUFFER_1_REG];
	read_buffer.parameter_2 =
	a_read_data_u8[BHY_READ_BUFFER_2_REG];
	read_buffer.parameter_3 =
	a_read_data_u8[BHY_READ_BUFFER_3_REG];
	read_buffer.parameter_4 =
	a_read_data_u8[BHY_READ_BUFFER_4_REG];
	read_buffer.parameter_5 =
	a_read_data_u8[BHY_READ_BUFFER_5_REG];
	read_buffer.parameter_6 =
	a_read_data_u8[BHY_READ_BUFFER_6_REG];
	read_buffer.parameter_7 =
	a_read_data_u8[BHY_READ_BUFFER_7_REG];
	read_buffer.parameter_8 =
	a_read_data_u8[BHY_READ_BUFFER_8_REG];
	read_buffer.parameter_9 =
	a_read_data_u8[BHY_READ_BUFFER_9_REG];
	read_buffer.parameter_10 =
	a_read_data_u8[BHY_READ_BUFFER_10_REG];
	read_buffer.parameter_11 =
	a_read_data_u8[BHY_READ_BUFFER_11_REG];
	read_buffer.parameter_12 =
	a_read_data_u8[BHY_READ_BUFFER_12_REG];
	read_buffer.parameter_13 =
	a_read_data_u8[BHY_READ_BUFFER_13_REG];
	read_buffer.parameter_14 =
	a_read_data_u8[BHY_READ_BUFFER_14_REG];
	read_buffer.parameter_15 =
	a_read_data_u8[BHY_READ_BUFFER_15_REG];
	read_buffer.parameter_16 =
	a_read_data_u8[BHY_READ_BUFFER_16_REG];

	return com_rslt;
}
/*!
 *	@brief API used to set the data from the parameter
 *	I2C page register from 0x5C to 0x63
 *
 *
 *	@param v_page_select_u8 : This input value for set the desired page
 *	@param v_parameter_request_u8 :
 *	This input value for set the desired parameter
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_write_parameter_bytes(
u8 v_page_select_u8, u8 v_parameter_request_u8)
{
/* variable used for return the status of communication result*/
	BHY_RETURN_FUNCTION_TYPE com_rslt = BHY_COMM_RES;
	u8 v_parameter_ack_u8 = BHY_INIT_VALUE;
	u8 v_parameter_ack_check_u8 = BHY_INIT_VALUE;
	u8 v_write_parameter_byte_u8[BHY_WRITE_BUFFER_SIZE];
	u8 init_array_data = BHY_INIT_VALUE;

	for (; init_array_data < BHY_WRITE_BUFFER_SIZE; init_array_data++)
		v_write_parameter_byte_u8[init_array_data] = BHY_INIT_VALUE;
	/* check the p_bhy structure as NULL*/
	if (p_bhy == BHY_NULL) {
		return BHY_NULL;
		} else {
		/* Assign the load parameters*/
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_1_REG]
		= write_buffer.write_parameter_byte1;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_2_REG]
		= write_buffer.write_parameter_byte2;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_3_REG]
		= write_buffer.write_parameter_byte3;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_4_REG]
		= write_buffer.write_parameter_byte4;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_5_REG]
		= write_buffer.write_parameter_byte5;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_6_REG]
		= write_buffer.write_parameter_byte6;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_7_REG]
		= write_buffer.write_parameter_byte7;
		v_write_parameter_byte_u8[BHY_WRITE_BUFFER_8_REG]
		= write_buffer.write_parameter_byte8;
		/* write values to the load address*/
		com_rslt = p_bhy->BHY_BUS_WRITE_FUNC
		(p_bhy->device_addr,
		BHY_I2C_REG_PARAMETER_WRITE_BUFFER_ZERO,
		&v_write_parameter_byte_u8[BHY_WRITE_BUFFER_1_REG],
		BHY_WRITE_BUFFER_SIZE);
		/* select the page*/
		com_rslt += bhy_set_parameter_page_select(v_page_select_u8);
		/* select the parameter*/
		com_rslt += bhy_set_parameter_request(v_parameter_request_u8);
		for (v_parameter_ack_check_u8 = BHY_INIT_VALUE;
		v_parameter_ack_check_u8 < BHY_PARAMETER_ACK_LENGTH;
		v_parameter_ack_check_u8++) {
			/* read the acknowledgement*/
			com_rslt += bhy_get_parameter_acknowledge(
			&v_parameter_ack_u8);
			if (v_parameter_ack_u8 == v_parameter_request_u8) {
				com_rslt += BHY_SUCCESS;
				break;
			} else if (v_parameter_ack_u8
			== BHY_PARAMETER_ACK_CHECK) {
				p_bhy->delay_msec(BHY_PARAMETER_ACK_DELAY);
				com_rslt += BHY_ERROR;
			} else {
				/* device not ready yet */
				p_bhy->delay_msec(1);
			}
		}

	}
	return com_rslt;
}
