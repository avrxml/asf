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


#ifndef BHY_SUPPORT_H_
#define BHY_SUPPORT_H_

/************************************************************************/
/**\name Includes                                                       */
/************************************************************************/
#include <asf.h>
#include "bhy.h"
#include "status_codes.h"




/************************************************************************/
/**\name Macros                                                         */
/************************************************************************/

/*! determines the I2C slave address of BHy
* The default I2C address of the BHy device is 0101000b (0x28). */
/* 0x28 CONFLICTS ON ATMEL DEV KITS WITH THE ONBOARD EDBG!!!!   */
#define BHY_I2C_SLAVE_ADDRESS		BHY_I2C_ADDR1
/*! the external pin on the main board connected to the BNO055 RESET pin */
//#define BNO055_RESET_PIN				EXT1_PIN_15
/*! determines the active state of BNO055 reset */
//#define BNO055_RESET_ACTIVE				false

/*! the delay required to wait for BHY chip to reset */
#define BHY_RESET_DELAY_MS			UINT32_C(50)

/* User defines */

#define REG_ADDR_LENGTH_BYTE  1
/************************************************************************/
/**\name Global Variables                                               */
/************************************************************************/

/*! instantiates a BHY software instance structure which retains
* chip ID, internal sensors IDs, I2C address and pointers
* to required functions (bus read/write and delay functions) */
struct bhy_t bhy;

/*! instantiates an I2C packet software instance structure which retains
* I2C slave address, data buffer and data length and is used to read/write
* data on the I2C bus */

/* Init software module instance. */
//! [dev_inst]
struct i2c_master_module i2c_master_instance;

struct i2c_master_packet bhy_i2c_wr_pkt;
struct i2c_master_packet bhy_i2c_rd_pkt;
//twi_packet_t ;

/************************************************************************/
/**\name Function Prototype Declarations                                */
/************************************************************************/

/*!
* @brief		Initializes BHY smart sensor and its required connections
*
* @param[in]	NULL
*
* @param[out]	NULL
*
* @return		NULL
*
*/
void bhy_initialize_support(void);

/*!
* @brief		Sends data to BHY via I2C
*
* @param[in]	dev_addr	Device I2C slave address
*
* @param[in]	reg_addr	Address of destination register
*
* @param[in]	reg_data	Pointer to data buffer to be sent
*
* @param[in]	length		Length of the data to be sent
*
* @retval		0			BHY_SUCCESS
* @retval		-1			BHY_ERROR
*
*/
int8_t bhy_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);

/*!
* @brief		Receives data from BHY on I2C
*
* @param[in]	dev_addr	Device I2C slave address
*
* @param[in]	reg_addr	Address of destination register
*
* @param[out]	reg_data	Pointer to data buffer to be received
*
* @param[in]	length		Length of the data to be received
*
* @retval		0			BHY_SUCCESS
* @retval		-1			BHY_ERROR
*
*/
int8_t bhy_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, uint16_t length);

/*!
* @brief		Initiates a delay of the length of the argument in milliseconds
*
* @param[in]	msec	Delay length in terms of milliseconds
*
* @param[out]	NULL
*
* @return		NULL
*
*/
void bhy_delay_msec(u32 msec);

/*!
* @brief		Resets the BHY chip
*
* @param[in]	NULL
*
* @param[out]	NULL
*
* @return		NULL
*
*/
void bhy_reset(void);




#endif /* BHY_SUPPORT_H_ */