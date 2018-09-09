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

#include "BHy_support.h"



extern void mdelay(uint32_t ul_dly_ticks);

void bhy_initialize_support(void)
{
	bhy.bus_write = &bhy_i2c_write;
	bhy.bus_read = &bhy_i2c_read;
	bhy.delay_msec = &bhy_delay_msec;
	bhy.device_addr = BHY_I2C_SLAVE_ADDRESS;

	bhy_reset();

	bhy_init(&bhy);
}



int8_t bhy_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
	enum status_code bhy_write_stat = STATUS_BUSY;

    uint8_t i, buffer[length + REG_ADDR_LENGTH_BYTE];


    buffer[0] = reg_addr;

	for (i=1; i<=length; i++)
	{
		buffer[i] = *reg_data++;
	}

 	bhy_i2c_wr_pkt.address	   = dev_addr;	//i2c slave address
	bhy_i2c_wr_pkt.data	       = buffer; //&reg_addr; // send register address
	bhy_i2c_wr_pkt.data_length = length + REG_ADDR_LENGTH_BYTE; //REG_ADDR_LENGTH_BYTE; // address length always one byte
// 	bhy_i2c_wr_pkt.ten_bit_address = false;
// 	bhy_i2c_wr_pkt.high_speed = false;
	bhy_write_stat = i2c_master_write_packet_wait(&i2c_master_instance, &bhy_i2c_wr_pkt);


	if (bhy_write_stat != STATUS_OK){
		return BHY_ERROR;
	}
	return BHY_SUCCESS;
}


int8_t bhy_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, uint16_t length)
{
	enum status_code bhy_read_stat = STATUS_BUSY, bhy_write_stat = STATUS_BUSY;

	bhy_i2c_wr_pkt.address	   = dev_addr;	//i2c slave address
	bhy_i2c_wr_pkt.data	       = &reg_addr; // send register address
	bhy_i2c_wr_pkt.data_length = REG_ADDR_LENGTH_BYTE; // address length always one byte
// 	bhy_i2c_wr_pkt.ten_bit_address = false;
// 	bhy_i2c_wr_pkt.high_speed = false;
	bhy_write_stat = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &bhy_i2c_wr_pkt);

	if (bhy_write_stat != STATUS_OK){
		return BHY_ERROR;
	}

	bhy_i2c_rd_pkt.address	   = dev_addr;	//i2c slave address
	bhy_i2c_rd_pkt.data	       = rx_data; // send register address
	bhy_i2c_rd_pkt.data_length = length; // address length always one byte
// 	bhy_i2c_rd_pkt.ten_bit_address = false;
// 	bhy_i2c_rd_pkt.high_speed = false;
	bhy_read_stat = i2c_master_read_packet_wait(&i2c_master_instance, &bhy_i2c_rd_pkt);

	if (bhy_read_stat != STATUS_OK){
		return BHY_ERROR;
	}

	return BHY_SUCCESS;
}


void bhy_delay_msec(u32 msec)
{
	//mdelay(msec);
	//delay_ms(msec);
	uint32_t nop_count, nop_count_limit;
	//delay for 1msec
	nop_count_limit = ((26000/3) * msec);
	/*nop_count_limit = (msec);*/
	for (nop_count = 0; nop_count <= nop_count_limit; nop_count ++) {\
	__NOP(); \
	}
	
}


void bhy_reset(void)
{
	bhy_set_reset_request(BHY_RESET_ENABLE);
}
