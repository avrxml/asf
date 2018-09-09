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


#ifndef __BHY_EXTENDED_H__
#define __BHY_EXTENDED_H__
/****************************************************************/
/**\name	DATA TYPES INCLUDES		*/
/************************************************************/
/*!
* @brief The following definition uses for define the data types
*
* @note While porting the API please consider the following
* @note Please check the version of C standard
* @note Are you using Linux platform
*/

/*!
* @brief For the Linux platform support
* Please use the types.h for your data types definitions
*/
#ifdef	__KERNEL__

#include <linux/types.h>
/* singed integer type*/
typedef	int8_t s8;/**< used for signed 8bit */
typedef	int16_t s16;/**< used for signed 16bit */
typedef	int32_t s32;/**< used for signed 32bit */
typedef	int64_t s64;/**< used for signed 64bit */

typedef	u_int8_t u8;/**< used for unsigned 8bit */
typedef	u_int16_t u16;/**< used for unsigned 16bit */
typedef	u_int32_t u32;/**< used for unsigned 32bit */
typedef	u_int64_t u64;/**< used for unsigned 64bit */



#else /* ! __KERNEL__ */
/**********************************************************
* These definition uses for define the C
* standard version data types
***********************************************************/
# if !defined(__STDC_VERSION__)

/************************************************
 * compiler is C11 C standard
************************************************/
#if (__STDC_VERSION__ == 201112L)

/************************************************/
#include <stdint.h>
/************************************************/

/*unsigned integer types*/
typedef	uint8_t u8;/**< used for unsigned 8bit */
typedef	uint16_t u16;/**< used for unsigned 16bit */
typedef	uint32_t u32;/**< used for unsigned 32bit */
typedef	uint64_t u64;/**< used for unsigned 64bit */

/*signed integer types*/
typedef	int8_t s8;/**< used for signed 8bit */
typedef	int16_t s16;/**< used for signed 16bit */
typedef	int32_t s32;/**< used for signed 32bit */
typedef	int64_t s64;/**< used for signed 64bit */
/************************************************
 * compiler is C99 C standard
************************************************/

#elif (__STDC_VERSION__ == 199901L)

/* stdint.h is a C99 supported c library.
which is used to fixed the integer size*/
/************************************************/
#include <stdint.h>
/************************************************/

/*unsigned integer types*/
typedef	uint8_t u8;/**< used for unsigned 8bit */
typedef	uint16_t u16;/**< used for unsigned 16bit */
typedef	uint32_t u32;/**< used for unsigned 32bit */
typedef	uint64_t u64;/**< used for unsigned 64bit */

/*signed integer types*/
typedef int8_t s8;/**< used for signed 8bit */
typedef	int16_t s16;/**< used for signed 16bit */
typedef	int32_t s32;/**< used for signed 32bit */
typedef	int64_t s64;/**< used for signed 64bit */
/************************************************
 * compiler is C89 or other C standard
************************************************/

#else /*  !defined(__STDC_VERSION__) */
/*!
* @brief By default it is defined as 32 bit machine configuration
*	define your data types based on your
*	machine/compiler/controller configuration
*/
#define  MACHINE_32_BIT

/*! @brief
 *	If your machine support 16 bit
 *	define the MACHINE_16_BIT
 */
#ifdef MACHINE_16_BIT
#include <limits.h>
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed long int s32;/**< used for signed 32bit */

#if defined(LONG_MAX) && LONG_MAX == 0x7fffffffffffffffL
typedef long int s64;/**< used for signed 64bit */
typedef unsigned long int u64;/**< used for unsigned 64bit */
#elif defined(LLONG_MAX) && (LLONG_MAX == 0x7fffffffffffffffLL)
typedef long long int s64;/**< used for signed 64bit */
typedef unsigned long long int u64;/**< used for unsigned 64bit */
#else
#warning Either the correct data type for signed 64 bit integer \
could not be found, or 64 bit integers are not supported in your environment.
#warning If 64 bit integers are supported on your platform, \
please set s64 manually.
#endif

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned long int u32;/**< used for unsigned 32bit */

/* If your machine support 32 bit
define the MACHINE_32_BIT*/
#elif defined MACHINE_32_BIT
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */

/* If your machine support 64 bit
define the MACHINE_64_BIT*/
#elif defined MACHINE_64_BIT
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */
typedef	signed long int s64;/**< used for signed 64bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */
typedef	unsigned long int u64;/**< used for unsigned 64bit */

#else
#warning The data types defined above which not supported \
define the data types manually
#endif
#endif

/*** This else will execute for the compilers
 *	which are not supported the C standards
 *	Like C89/C99/C11***/
#else
/*!
* @brief By default it is defined as 32 bit machine configuration
*	define your data types based on your
*	machine/compiler/controller configuration
*/
#define  MACHINE_32_BIT

/* If your machine support 16 bit
define the MACHINE_16_BIT*/
#ifdef MACHINE_16_BIT
#include <limits.h>
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed long int s32;/**< used for signed 32bit */

#if defined(LONG_MAX) && LONG_MAX == 0x7fffffffffffffffL
typedef long int s64;/**< used for signed 64bit */
typedef unsigned long int u64;/**< used for unsigned 64bit */
#elif defined(LLONG_MAX) && (LLONG_MAX == 0x7fffffffffffffffLL)
typedef long long int s64;/**< used for signed 64bit */
typedef unsigned long long int u64;/**< used for unsigned 64bit */
#else
#warning Either the correct data type for signed 64 bit integer \
could not be found, or 64 bit integers are not supported in your environment.
#warning If 64 bit integers are supported on your platform, \
please set s64 manually.
#endif

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned long int u32;/**< used for unsigned 32bit */

/*! @brief If your machine support 32 bit
define the MACHINE_32_BIT*/
#elif defined MACHINE_32_BIT
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */

/* If your machine support 64 bit
define the MACHINE_64_BIT*/
#elif defined MACHINE_64_BIT
/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */
typedef	signed long int s64;/**< used for signed 64bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */
typedef	unsigned long int u64;/**< used for unsigned 64bit */

#else
#warning The data types defined above which not supported \
define the data types manually
#endif
#endif
#endif
/***************************************************************/
/**\name	BUS READ AND WRITE FUNCTION POINTERS        */
/***************************************************************/
/*!
	@brief Define the calling convention of YOUR bus communication routine.
	@note This includes types of parameters. This example shows the
	configuration for an SPI bus link.

    If your communication function looks like this:

    write_my_bus_xy(u8 device_addr, u8 register_addr,
    u8 * data, u8 length);

    The BHY_WR_FUNC_PTR would equal:

    BHY_WR_FUNC_PTR s8 (* bus_write)(u8,
    u8, u8 *, u8)

    Parameters can be mixed as needed refer to the
    @ref BHY_BUS_WRITE_FUNC  macro.


*/
#define BHY_WR_FUNC_PTR s8 (*bus_write)(u8, u8,\
u8 *, u16)
/**< link macro between API function calls and bus write function
	@note The bus write function can change since this is a
	system dependant issue.

    If the bus_write parameter calling order is like: reg_addr,
    reg_data, wr_len it would be as it is here.

    If the parameters are differently ordered or your communication
    function like I2C need to know the device address,
    you can change this macro accordingly.


    BHY_BUS_WRITE_FUNC(device_addr, reg_addr, reg_data, wr_len)\
    bus_write(device_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a
    way that equals your definition in the
    @ref BHY_WR_FUNC_PTR definition.

*/
#define BHY_BUS_WRITE_FUNC(device_addr, reg_addr, reg_data, wr_len)\
bus_write(device_addr, reg_addr, reg_data, wr_len)

/**< Define the calling convention of YOUR bus communication routine.
	@note This includes types of parameters. This example shows the
	configuration for an SPI bus link.

    If your communication function looks like this:

    read_my_bus_xy(u8 device_addr, u8 register_addr,
    u8 * data, u8 length);

    The BHY_RD_FUNC_PTR would equal:

    BHY_RD_FUNC_PTR s8 (* bus_read)(u8,
    u8, u8 *, u8)

    Parameters can be mixed as needed refer to the
    refer BHY_BUS_READ_FUNC  macro.

*/
#define BHY_SPI_READ_MASK 0x80   /* for spi read transactions on SPI the
			MSB has to be set */
#define BHY_RD_FUNC_PTR s8 (*bus_read)(u8,\
			u8, u8 *, u16)

#define BHY_BRD_FUNC_PTR s8 \
(*burst_read)(u8, u8, u8 *, u32)

/**< link macro between API function calls and bus read function
	@note The bus write function can change since this is a
	system dependant issue.

    If the bus_read parameter calling order is like: reg_addr,
    reg_data, wr_len it would be as it is here.

    If the parameters are differently ordered or your communication
    function like I2C need to know the device address,
    you can change this macro accordingly.


    BHY_BUS_READ_FUNC(device_addr, reg_addr, reg_data, wr_len)\
    bus_read(device_addr, reg_addr, reg_data, wr_len)

    This macro lets all API functions call YOUR communication routine in a
    way that equals your definition in the
    refer BHY_WR_FUNC_PTR definition.

    @note: this macro also includes the "MSB='1'
    for reading BHY addresses.

*/
#define BHY_BUS_READ_FUNC(device_addr, reg_addr, reg_data, r_len)\
				bus_read(device_addr, reg_addr, reg_data, r_len)

#define BHY_BURST_READ_FUNC(device_addr, \
register_addr, register_data, rd_len)\
burst_read(device_addr, register_addr, register_data, rd_len)


#define BHY_MDELAY_DATA_TYPE                 u32
/***************************************************************/
/**\name	I2C ADDRESS DEFINITIONS       */
/***************************************************************/
#define BHY_I2C_ADDR1	 (0x28)
#define BHY_I2C_ADDR2	 (0x29)
/***************************************************************/
/**\name	CONSTANTS     */
/***************************************************************/
#define   BHY_INIT_VALUE				(0)
#define   BHY_GEN_READ_WRITE_LENGTH	    (1)
#define   BHY_BYTES_REMAINING_LENGTH	(2)
#define   BHY_CRC_HOST_LENGTH			(4)
#define   BHY_PARAMETER_ACK_LENGTH		(250)
#define   BHY_READ_BUFFER_LENGTH		(16)
#define   BHY_PARAMETER_ACK_DELAY		(50)
#define   BHY_SIGNATURE_MEM_LEN         (17)
/***************************************************************/
/**\name	BIT SHIFTING DEFINITION      */
/***************************************************************/
#define	BHY_SHIFT_BIT_POSITION_BY_01_BIT		(1)
#define	BHY_SHIFT_BIT_POSITION_BY_02_BITS		(2)
#define	BHY_SHIFT_BIT_POSITION_BY_03_BITS		(3)
#define	BHY_SHIFT_BIT_POSITION_BY_04_BITS		(4)
#define	BHY_SHIFT_BIT_POSITION_BY_05_BITS		(5)
#define	BHY_SHIFT_BIT_POSITION_BY_06_BITS		(6)
#define	BHY_SHIFT_BIT_POSITION_BY_07_BITS		(7)
#define BHY_SHIFT_BIT_POSITION_BY_08_BITS		(8)
#define BHY_SHIFT_BIT_POSITION_BY_16_BITS		(16)
#define BHY_SHIFT_BIT_POSITION_BY_24_BITS		(24)
/****************************************************/
/**\name	ARRAY SIZE DEFINITIONS      */
/***************************************************/
#define BHY_BYTES_REMAINING_SIZE     (2)
#define BHY_BYTES_REMAINING_LSB      (0)
#define BHY_BYTES_REMAINING_MSB      (1)

#define BHY_CRC_HOST_SIZE		(4)
#define BHY_CRC_HOST_LSB        (0)
#define BHY_CRC_HOST_XLSB       (1)
#define BHY_CRC_HOST_XXLSB      (2)
#define BHY_CRC_HOST_MSB        (3)
#define BHY_CRC_HOST_FILE_LSB        (4)
#define BHY_CRC_HOST_FILE_XLSB       (5)
#define BHY_CRC_HOST_FILE_XXLSB      (6)
#define BHY_CRC_HOST_FILE_MSB        (7)

#define BHY_INDEX_LEN          (19)

#define	BHY_WRITE_BUFFER_SIZE		(8)
#define	BHY_WRITE_BUFFER_1_REG	(0)
#define	BHY_WRITE_BUFFER_2_REG	(1)
#define	BHY_WRITE_BUFFER_3_REG	(2)
#define	BHY_WRITE_BUFFER_4_REG	(3)
#define	BHY_WRITE_BUFFER_5_REG	(4)
#define	BHY_WRITE_BUFFER_6_REG	(5)
#define	BHY_WRITE_BUFFER_7_REG	(6)
#define	BHY_WRITE_BUFFER_8_REG	(7)

#define	BHY_READ_BUFFER_SIZE	(16)
#define	BHY_READ_BUFFER_1_REG	(0)
#define	BHY_READ_BUFFER_2_REG	(1)
#define	BHY_READ_BUFFER_3_REG	(2)
#define	BHY_READ_BUFFER_4_REG	(3)
#define	BHY_READ_BUFFER_5_REG	(4)
#define	BHY_READ_BUFFER_6_REG	(5)
#define	BHY_READ_BUFFER_7_REG	(6)
#define	BHY_READ_BUFFER_8_REG	(7)
#define	BHY_READ_BUFFER_9_REG	(8)
#define	BHY_READ_BUFFER_10_REG	(9)
#define	BHY_READ_BUFFER_11_REG	(10)
#define	BHY_READ_BUFFER_12_REG	(11)
#define	BHY_READ_BUFFER_13_REG	(12)
#define	BHY_READ_BUFFER_14_REG	(13)
#define	BHY_READ_BUFFER_15_REG	(14)
#define	BHY_READ_BUFFER_16_REG	(15)

#define BHY_FIFO_SENSOR_DATA_SIZE	(100)
/****************************************************/
/**\name	ERROR CODES      */
/***************************************************/

#define BHY_NULL_PTR			((void *) 0)
#define BHY_NULL				((u8)0)
#define BHY_COMM_RES			((s8)-1)
#define BHY_OUT_OF_RANGE		((s8)-2)
#define	BHY_SUCCESS				((u8)0)
#define	BHY_ERROR				((s8)-3)
/* Constants */
#define BHY_DELAY_SETTLING_TIME			(5)
#define BHY_FIFO_BUFFER_SIZE			(50)
/*This refers BHY return type as s8 */
#define BHY_RETURN_FUNCTION_TYPE        s8
/****************************************************/
/**\name	REGISTER DEFINITIONS       */
/***************************************************/
/****************************************************/
/**\name	I2C REGISTER MAP DEFINITIONS       */
/***************************************************/
/* i2c buffer read for sensor fifo data*/
#define	BHY_I2C_REG_BUFFER_ZERO_ADDR				(0x00)
/* fifo flush, chip control and status registers*/
#define	BHY_I2C_REG_FIFO_FLUSH_ADDR					(0x32)
#define	BHY_I2C_REG_CHIP_CONTROL_ADDR				(0x34)
#define	BHY_I2C_REG_HOST_STATUS_ADDR				(0x35)
#define	BHY_I2C_REG_INT_STATUS_ADDR					(0x36)
#define	BHY_I2C_REG_CHIP_STATUS_ADDR				(0x37)
/* bytes remaining register*/
#define	BHY_I2C_REG_BYTES_REMAINING_LSB_ADDR		(0x38)
#define	BHY_I2C_REG_BYTES_REMAINING_MSB_ADDR		(0x39)
#define	BHY_I2C_REG_PARAMETER_ACKNOWLEDGE_ADDR		(0x3A)
/* saved parameter */
#define	BHY_I2C_REG_PARAMETER_READ_BUFFER_ZERO	(0x3B)

#define	BHY_I2C_REG_PARAMETER_PAGE_SELECT_ADDR	(0x54)
/**< parameter page selection address*/
#define	BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR	(0x55)
/**< host interface control address*/
/*!
 * @brief parameter write buffer*/
#define	BHY_I2C_REG_PARAMETER_WRITE_BUFFER_ZERO		(0x5C)
#define	BHY_I2C_REG_PARAMETER_REQUEST_ADDR			(0x64)
/**< used to select the respective parameter number*/

/* product and revision id */
#define	BHY_BHY_HOST_IRQ_TIMESTAMP_ADDR				(0x6C)
/**< host IRQ time stamp address*/
#define	BHY_ROM_VERSION_ADDR						(0x70)
/**< ROM version address*/
#define	BHY_RAM_VERSION_ADDR						(0x72)
/**< RAM version address*/
#define	BHY_I2C_REG_PRODUCT_ID_ADDR					(0x90)
/**< product id address*/
#define	BHY_I2C_REG_REVISION_ID_ADDR				(0x91)
/**< revision id address*/
#define	BHY_I2C_REG_UPLOAD_0_ADDR					(0x94)
#define	BHY_I2C_REG_UPLOAD_1_ADDR					(0x95)
/**< upload address for RAM patch*/
#define	BHY_I2C_REG_UPLOAD_DATA_ADDR				(0x96)
/**< upload data address for RAM patch*/
#define	BHY_I2C_REG_CRC_HOST_ADDR					(0x97)
/**< CRC register*/
#define	BHY_I2C_REG_RESET_REQUEST_ADDR				(0x9B)
/**< reset register*/
/****************************************************/
/**\name ARRAY DEFINITIONS FOR IRQ TIME STAMP*/
/**************************************************************/
#define	BHY_HOST_IRQ_TIMESTAMP_SIZE			(4)
#define	BHY_HOST_IRQ_TIMESTAMP_LSB_DATA		(0)
#define	BHY_HOST_IRQ_TIMESTAMP_XLSB_DATA	(1)
#define	BHY_HOST_IRQ_TIMESTAMP_XXLSB_DATA	(2)
#define	BHY_HOST_IRQ_TIMESTAMP_MSB_DATA		(3)
/****************************************************/
/**\name ARRAY DEFINITIONS FOR ROM VERSION*/
/**************************************************************/
#define	BHY_ROM_VERSION_SIZE			(2)
#define	BHY_ROM_VERSION_LSB_DATA		(0)
#define	BHY_ROM_VERSION_MSB_DATA		(1)

#define	BHY_RAM_VERSION_SIZE			(2)
#define	BHY_RAM_VERSION_LSB_DATA		(0)
#define	BHY_RAM_VERSION_MSB_DATA		(1)
#define BHY_FIFO_DATA_BUFFER			(200)

/**************************************************************/
/**\name	STRUCTURE DEFINITIONS                         */
/**************************************************************/
/*!
*	@brief BHY structure
*	This structure holds all relevant information about BHY
*/
struct bhy_t {
u8 product_id;/**< product id of BHY */
u8 device_addr;/**< device address of BHY */
BHY_WR_FUNC_PTR;/**< bus write function pointer
used to map the user bus write functions*/
BHY_RD_FUNC_PTR;/**< bus read function pointer
used to map the user bus read functions*/
BHY_BRD_FUNC_PTR;/**< burst read function pointer
used to map the user burst read functions*/
void (*delay_msec)(BHY_MDELAY_DATA_TYPE);/**< delay function pointer */
};
/*!
*	@brief BHY parameter read buffer
*	structure holds all relevant information about
*	the respective parameter bytes data
*/
struct parameter_read_buffer_t {
u8 parameter_1;/**<parameter bytes 1*/
u8 parameter_2;/**<parameter bytes 1*/
u8 parameter_3;/**<parameter bytes 1*/
u8 parameter_4;/**<parameter bytes 1*/
u8 parameter_5;/**<parameter bytes 1*/
u8 parameter_6;/**<parameter bytes 1*/
u8 parameter_7;/**<parameter bytes 1*/
u8 parameter_8;/**<parameter bytes 1*/
u8 parameter_9;/**<parameter bytes 1*/
u8 parameter_10;/**<parameter bytes 1*/
u8 parameter_11;/**<parameter bytes 1*/
u8 parameter_12;/**<parameter bytes 1*/
u8 parameter_13;/**<parameter bytes 1*/
u8 parameter_14;/**<parameter bytes 1*/
u8 parameter_15;/**<parameter bytes 1*/
u8 parameter_16;/**<parameter bytes 1*/
};
/*!
*	@brief BHY load parameter bytes structure
*	This structure holds all relevant information about BHY
*	load parameter bytes
*/
struct parameter_write_buffer_t {
u8 write_parameter_byte1;/**<parameter byte 1*/
u8 write_parameter_byte2;/**<parameter byte 2*/
u8 write_parameter_byte3;/**<parameter byte 3*/
u8 write_parameter_byte4;/**<parameter byte 4*/
u8 write_parameter_byte5;/**<parameter byte 5*/
u8 write_parameter_byte6;/**<parameter byte 6*/
u8 write_parameter_byte7;/**<parameter byte 7*/
u8 write_parameter_byte8;/**<parameter byte 8*/
};
/*!
*	@brief meta event interrupt structure
*	This structure holds meta event
*	interrupt details
*/
struct meta_event_intr_t {
u8 meta_event1_intr;/**< Meta event1 interrupt */
u8 meta_event2_intr;/**< Meta event2 interrupt */
u8 meta_event3_intr;/**< Meta event3 interrupt */
u8 meta_event4_intr;/**< Meta event4 interrupt */
u8 meta_event5_intr;/**< Meta event5 interrupt */
u8 meta_event6_intr;/**< Meta event6 interrupt */
u8 meta_event7_intr;/**< Meta event7 interrupt */
u8 meta_event8_intr;/**< Meta event8 interrupt */
u8 meta_event9_intr;/**< Meta event9 interrupt */
u8 meta_event10_intr;/**< Meta event10 interrupt */
u8 meta_event11_intr;/**< Meta event11 interrupt */
u8 meta_event12_intr;/**< Meta event12 interrupt */
u8 meta_event13_intr;/**< Meta event13 interrupt */
u8 meta_event14_intr;/**< Meta event14 interrupt */
u8 meta_event15_intr;/**< Meta event15 interrupt */
u8 meta_event16_intr;/**< Meta event16 interrupt */
u8 meta_event17_intr;/**< Meta event17 interrupt */
u8 meta_event18_intr;/**< Meta event18 interrupt */
u8 meta_event19_intr;/**< Meta event19 interrupt */
u8 meta_event20_intr;/**< Meta event20 interrupt */
u8 meta_event21_intr;/**< Meta event21 interrupt */
u8 meta_event22_intr;/**< Meta event22 interrupt */
u8 meta_event23_intr;/**< Meta event23 interrupt */
u8 meta_event24_intr;/**< Meta event24 interrupt */
u8 meta_event25_intr;/**< Meta event25 interrupt */
u8 meta_event26_intr;/**< Meta event26 interrupt */
u8 meta_event27_intr;/**< Meta event27 interrupt */
u8 meta_event28_intr;/**< Meta event28 interrupt */
u8 meta_event29_intr;/**< Meta event29 interrupt */
u8 meta_event30_intr;/**< Meta event30 interrupt */
u8 meta_event31_intr;/**< Meta event31 interrupt */
u8 meta_event32_intr;/**< Meta event32 interrupt */
};
/*!
*	@brief meta event Event structure
*	This structure holds meta event
*	Event details
*/
struct meta_event_event_t {
u8 meta_event1_event;/**< Meta event1 event */
u8 meta_event2_event;/**< Meta event2 event */
u8 meta_event3_event;/**< Meta event3 event */
u8 meta_event4_event;/**< Meta event4 event */
u8 meta_event5_event;/**< Meta event5 event */
u8 meta_event6_event;/**< Meta event6 event */
u8 meta_event7_event;/**< Meta event7 event */
u8 meta_event8_event;/**< Meta event8 event */
u8 meta_event9_event;/**< Meta event9 event */
u8 meta_event10_event;/**< Meta event10 event */
u8 meta_event11_event;/**< Meta event11 event */
u8 meta_event12_event;/**< Meta event12 event */
u8 meta_event13_event;/**< Meta event13 event */
u8 meta_event14_event;/**< Meta event14 event */
u8 meta_event15_event;/**< Meta event15 event */
u8 meta_event16_event;/**< Meta event16 event */
u8 meta_event17_event;/**< Meta event17 event */
u8 meta_event18_event;/**< Meta event18 event */
u8 meta_event19_event;/**< Meta event19 event */
u8 meta_event20_event;/**< Meta event20 event */
u8 meta_event21_event;/**< Meta event21 event */
u8 meta_event22_event;/**< Meta event22 event */
u8 meta_event23_event;/**< Meta event23 event */
u8 meta_event24_event;/**< Meta event24 event */
u8 meta_event25_event;/**< Meta event25 event */
u8 meta_event26_event;/**< Meta event26 event */
u8 meta_event27_event;/**< Meta event27 event */
u8 meta_event28_event;/**< Meta event28 event */
u8 meta_event29_event;/**< Meta event29 event */
u8 meta_event30_event;/**< Meta event30 event */
u8 meta_event31_event;/**< Meta event31 event */
u8 meta_event32_event;/**< Meta event32 event */
};
/*!
*	@brief sensor status bank structure
*	This structure holds sensor status bank information
*/
struct sensor_status_bank_t {
u8 data_available;/**<contains the status bank data available information*/
u8 i2c_nack;/**<contains the status bank i2c NACK information*/
u8 device_id_error;/**<contains the status bank device id error information*/
u8 transient_error;/**<contains the status bank transient error information*/
u8 data_lost;/**<contains the status bank data loss information*/
u8 sensor_power_mode;
/**<contains the status bank sensor power mode information*/
};
/*!
*	@brief physical sensor status
*	This structure holds accel physical sensor status
*/
struct accel_physical_status_t {
u16 accel_sample_rate;/**<contains the accel sampling rate information*/
u16 accel_dynamic_range;/**<contains the accel dynamic range information*/
u8 accel_flag;/**<contains the accel flag information*/
};
/*!
*	@brief physical sensor status
*	This structure holds gyro physical sensor status
*/
struct gyro_physical_status_t {
u16 gyro_sample_rate;/**<contains the gyro sampling rate information*/
u16 gyro_dynamic_range;/**<contains the gyro dynamic range information*/
u8 gyro_flag;/**<contains the gyro flag information*/
};
/*!
*	@brief physical sensor status
*	This structure holds mag physical sensor status
*/
struct mag_physical_status_t {
u16 mag_sample_rate;/**<contains the mag sampling rate information*/
u16 mag_dynamic_range;/**<contains the mag dynamic range information*/
u8 mag_flag;/**<contains the mag flag information*/
};
/*!
*	@brief Sensor information
*	This structure holds non wakeup sensor information
*/
struct sensor_information_non_wakeup_t {
u8 non_wakeup_sensor_type;/**<contains the sensor type*/
u8 non_wakeup_driver_id;/**<contains the driver id*/
u8 non_wakeup_driver_version;/**<contains the driver version*/
u8 non_wakeup_power;/**<contains the power example 0.1mA*/
u16 non_wakeup_max_range;
/**<contains the maxim range of sensor data in SI units*/
u16 non_wakeup_resolution;
/**<contains the no of bit resolution of underlying sensor*/
u16 non_wakeup_max_rate;/**<contains the maximum rate in Hz*/
u16 non_wakeup_fifo_reserved;/**< contains the fifo size*/
u16 non_wakeup_fifo_max;/**< contains the entire fifo size*/
u8 non_wakeup_event_size;/**< contains the no of bytes sensor data packet*/
u8 non_wakeup_min_rate;/**<contains the minimum rate in Hz*/
};
/*!
*	@brief Sensor information
*	This structure holds wakeup sensor information
*/
struct sensor_information_wakeup_t {
u8 wakeup_sensor_type;/**<contains the sensor type*/
u8 wakeup_driver_id;/**<contains the driver id*/
u8 wakeup_driver_version;/**<contains the driver version*/
u8 wakeup_power;/**<contains the power example 0.1mA*/
u16 wakeup_max_range;
/**<contains the maxim range of sensor data in SI units*/
u16 wakeup_resolution;
/**<contains the no of bit resolution of underlying sensor*/
u16 wakeup_max_rate;/**<contains the maximum rate in Hz*/
u16 wakeup_fifo_reserved;/**< contains the fifo size*/
u16 wakeup_fifo_max;/**< contains the entire fifo size*/
u8 wakeup_event_size;/**< contains the no of bytes sensor data packet*/
u8 wakeup_min_rate;/**<contains the minimum rate in Hz*/
};
/*!
*	@brief Sensor configuration
*	This structure holds non wakeup sensor configuration
*/
struct sensor_configuration_non_wakeup_t {
u16 non_wakeup_sample_rate;/**<contains the sample rate information*/
u16 non_wakeup_max_report_latency;/**<contains the maximum report latency*/
u16 non_wakeup_change_sensitivity;/**<contains the sensitivity*/
u16 non_wakeup_dynamic_range;/**<contains the dynamic range*/
};
/*!
*	@brief Sensor configuration
*	This structure holds wakeup sensor configuration
*/
struct sensor_configuration_wakeup_t {
u16 wakeup_sample_rate;/**<contains the sample rate information*/
u16 wakeup_max_report_latency;/**<contains the maximum report latency*/
u16 wakeup_change_sensitivity;/**<contains the sensitivity*/
u16 wakeup_dynamic_range;/**<contains the dynamic range*/
};
/*!
*	@brief Sensor configuration
*	This structure holds soft pass through informations
*/
struct soft_pass_through_read_t {
u8 i2c_slave_address;/**< contain the value of I2C slave address*/
u8 start_register;/**< contain the value of  start register*/
u8 read_length;/**< contain the value of  read length*/
u8 completion_status;/**< contain the value of  completion status*/
u8 reg_value_byte1;/**< contain the value of returned register value1*/
u8 reg_value_byte2;/**< contain the value of returned register value2*/
u8 reg_value_byte3;/**< contain the value of returned register value3*/
u8 reg_value_byte4;/**< contain the value of returned register value4*/
};
/*!
*	@brief Sensor configuration
*	This structure holds write soft pass through informations
*/
struct soft_pass_through_write_t {
u8 i2c_slave_address;/**< contain the value of I2C slave address*/
u8 start_register;/**< contain the value of  start register*/
u8 write_length;/**< contain the value of  read length*/
u8 completion_status;/**< contain the value of  completion status*/
u8 reg_value_byte1;/**< contain the value of returned register value1*/
u8 reg_value_byte2;/**< contain the value of returned register value2*/
u8 reg_value_byte3;/**< contain the value of returned register value3*/
u8 reg_value_byte4;/**< contain the value of returned register value4*/
};


/**************************************************************/
/**\name	COMMON STRUCTURE FOR DATA PROCESSING    */
/**************************************************************/
/*!
*	@brief used for x,y,z and accuracy status
*	data processing
*/
struct xyz_accuracy_t {
s16 x;/**<contains the
x axis data */
s16 y;/**<contains the
y axis data */
s16 z;/**<contains the
z axis data */
u8 accuracy;/**<contains the
accuracy  data */
};
/*!
*	@brief used for x,y,z,w and radians status
*	data processing
*/
struct xyzw_radians_t {
s16 x;/**<contains the
x axis data */
s16 y;/**<contains the
y axis data */
s16 z;/**<contains the
z axis data */
s16 w;/**<contains the
w axis data */
s16 radians;/**<contains the
radians data */
};
/*!
*	@brief used for only one data validation of unsigned integer
*   like light,humidity,proximity,step counter
*	activity and time stamp.
*/
struct fifo_size_two_t {
u16 data_process;/**< value of unsigned processed data*/
};
/*!
*	@brief used for processing the temperature data
*/
struct process_temperature_t {
s16 temperature;/**< value of temperature data*/
};
/*!
*	@brief used for processing the meta events
*/
struct process_meta_event_t {
u8 event_number;/**< value of meta event number*/
u8 sensor_type;/**< value of sensor type*/
u8 event_spec;/**< value of event specific value*/
};
/*!
*	@brief used for processing the uncalib
*	mag and gyro data
*/
struct uncalib_bias_t {
s16 uncalib_x;/**<contains the signed 16bit uncalib x data*/
s16 uncalib_y;/**<contains the signed 16bit uncalib y data*/
s16 uncalib_z;/**<contains the signed 16bit uncalib z data*/
s16 bias_x;/**<contains the signed 16bit  bias x data*/
s16 bias_y;/**<contains the signed 16bit  bias y data*/
s16 bias_z;/**<contains the signed 16bit  bias z data*/
u8 accuracy;/**<contains the signed
16bit estimation accuracy data*/
};
/*!
*	@brief used for processing the activity data
*/
struct activity_t {
u8 bit_on;/**< value of activity bits on*/
u8 bit_off;/**< value of activity bits off*/
};
/*!
*	@brief barometer data
*/
struct barometer_process_t {
u32 barometer;/**<contains the
24 bit unsigned barometer data*/
};
/*!
*	@brief processing the one byte data
* like heart rate
*/
struct heart_rate_data_process_t {
u8 data;/**<contains the
one byte data data*/
};
/*!
*	@brief used to process BSX_A, BSX_B and BSX_C data
*/
struct bsx_process_data_t {
s32 x;/**<contains the signed 32bit  x data*/
s32 y;/**<contains the signed 32bit  y data*/
s32 z;/**<contains the signed 32bit  z data*/
u32 timestamp;/**<contains the signed 32 bit time stamp data*/
u8 vector;/**< contains the accuracy data*/
};
/**************************************************************/
/**\name	FIFO INDEX DEFINITION    */
/**************************************************************/
#define	BHY_SENSOR_ID_INDEX		(1)

/* rotational vector data */
#define	BHY_RVECTOR_X_LSB_DATA	      (0)
#define	BHY_RVECTOR_X_MSB_DATA	      (1)
#define	BHY_RVECTOR_Y_LSB_DATA	      (2)
#define	BHY_RVECTOR_Y_MSB_DATA	      (3)
#define	BHY_RVECTOR_Z_LSB_DATA	      (4)
#define	BHY_RVECTOR_Z_MSB_DATA	      (5)
#define	BHY_RVECTOR_W_LSB_DATA	      (6)
#define	BHY_RVECTOR_W_MSB_DATA	      (7)
#define	BHY_RVECTOR_ACCURACY_LSB_DATA (8)
#define	BHY_RVECTOR_ACCURACY_MSB_DATA (9)
#define	BHY_RVECTOR_LENGTH            (10)

/* barometer data */
#define	BHY_BAROMETER_LSB_DATA	      (0)
#define	BHY_BAROMETER_XLSB_DATA	      (1)
#define	BHY_BAROMETER_MSB_DATA	      (2)
#define	BHY_BAROMETER_LENGTH	      (3)
#define BHY_TWO_BYTE_LENGTH           (2)
#define BHY_STEP_AMBIENT_TEMPERATURE_MSB_DATA    (1)
#define BHY_STEP_AMBIENT_TEMPERATURE_LSB_DATA    (0)


/* activity*/
#define	BHY_ACTIVITY_BIT_ON		(1)
#define	BHY_ACTIVITY_BIT_OFF	(0)
#define	BHY_ACTIVITY_LENGTH		(2)


/* BSX a */
#define BHY_BSXA_X_LSB_DATA				(0)
#define BHY_BSXA_X_XLSB_DATA			(1)
#define BHY_BSXA_X_XXLSB_DATA			(2)
#define BHY_BSXA_X_MSB_DATA				(3)
#define BHY_BSXA_Y_LSB_DATA				(4)
#define BHY_BSXA_Y_XLSB_DATA			(5)
#define BHY_BSXA_Y_XXLSB_DATA			(6)
#define BHY_BSXA_Y_MSB_DATA				(7)
#define BHY_BSXA_Z_LSB_DATA				(8)
#define BHY_BSXA_Z_XLSB_DATA			(9)
#define BHY_BSXA_Z_XXLSB_DATA			(10)
#define BHY_BSXA_Z_MSB_DATA				(11)
#define BHY_BSXA_TIMESTAMP_LSB_DATA		(12)
#define BHY_BSXA_TIMESTAMP_XLSB_DATA	(13)
#define BHY_BSXA_TIMESTAMP_XXLSB_DATA	(14)
#define	BHY_BSXA_TIMESTAMP_MSB_DATA		(15)
#define BHY_BSXA_VECTOR_DATA			(16)
#define BHY_BSXA_LENGTH					(17)

/* meta event*/
#define	BHY_BHY_META_EVENT_NUMBER			(0)
#define	BHY_BHY_META_EVENT_SENSOR_TYPE		(1)
#define	BHY_BHY_META_EVENT_SPEC_VALUE		(2)
#define	BHY_BHY_META_EVENT_LENGTH			(3)
/* RAM PATCH definitions*/
/* accel data */
#define	BHY_X_LSB_DATA	 (0)
#define	BHY_X_MSB_DATA	 (1)
#define	BHY_Y_LSB_DATA	 (2)
#define	BHY_Y_MSB_DATA	 (3)
#define	BHY_Z_LSB_DATA	 (4)
#define	BHY_Z_MSB_DATA	 (5)
#define	BHY_VECTOR_DATA	 (6)
#define	BHY_XYZ_VECTOR_LENGTH        (7)
#define INCREMENT_INDEX              (1)
#define DECREMENT_INDEX              (1)

#define	BHY_TWO_LENGTH					(2)
#define	BHY_LSB_DATA		(0)
#define	BHY_MSB_DATA		(1)
#define BHY_FALSE           (0)
#define BHY_TRUE            (1)

/* rotational vector data */
#define	BHY_RVECTOR_X_LSB_DATA	       (0)
#define	BHY_RVECTOR_X_MSB_DATA	       (1)
#define	BHY_RVECTOR_Y_LSB_DATA	       (2)
#define	BHY_RVECTOR_Y_MSB_DATA	       (3)
#define	BHY_RVECTOR_Z_LSB_DATA	       (4)
#define	BHY_RVECTOR_Z_MSB_DATA	       (5)
#define	BHY_RVECTOR_W_LSB_DATA	       (6)
#define	BHY_RVECTOR_W_MSB_DATA	       (7)
#define	BHY_RVECTOR_ACCURACY_LSB_DATA  (8)
#define	BHY_RVECTOR_ACCURACY_MSB_DATA  (9)
#define	BHY_RVECTOR_LENGTH            (10)

/* uncalibration mag data*/
#define	BHY_UNCALIB_X_LSB_DATA		(0)
#define	BHY_UNCALIB_X_MSB_DATA		(1)
#define	BHY_UNCALIB_Y_LSB_DATA		(2)
#define	BHY_UNCALIB_Y_MSB_DATA		(3)
#define	BHY_UNCALIB_Z_LSB_DATA		(4)
#define	BHY_UNCALIB_Z_MSB_DATA		(5)
#define	BHY_BIAS_X_LSB_DATA			(6)
#define	BHY_BIAS_X_MSB_DATA			(7)
#define	BHY_BIAS_Y_LSB_DATA			(8)
#define	BHY_BIAS_Y_MSB_DATA			(9)
#define	BHY_BIAS_Z_LSB_DATA			(10)
#define	BHY_BIAS_Z_MSB_DATA			(11)
#define	BHY_UNCALIB_ACCURACY_DATA	(12)
#define	BHY_UNCALIB_LENGTH			(13)
/**************************************************************/
/**\name	CONSTANT HEX DEFINITION    */
/**************************************************************/
#define BHY_PARAMETER_ACK_CHECK         (0x80)
#define	BHY_MASK_LSB_DATA           (0x00FF)
#define	BHY_MASK_MSB_DATA           (0xFF00)
#define	BHY_SIC_MASK_MSB_DATA		(0x000000FF)
#define	BHY_SIC_MASK_LSB_DATA		(0x0000FF00)
#define	BHY_SIC_MASK_LSB1_DATA		(0x00FF0000)
#define	BHY_SIC_MASK_LSB2_DATA		(0xFF000000)
#define	BHY_MASK_META_EVENT         (0xFF)

/**************************************************************/
/**\name	PAGE SELECTION DEFINITION   */
/**************************************************************/
#define	BHY_PAGE_1	(0x01)
/**< page 1 system page*/
#define	BHY_PAGE_3	(0x03)
/**< page 3 sensor page*/
#define	BHY_PAGE_15	(0x0F)
/**< page 15 sensor page*/
/**************************************************************/
/**\name	READ PARAMETER REQUEST      */
/**************************************************************/
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_30		(0x1E)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_0		(0x00)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_1		(0x01)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_2		(0x02)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_3		(0x03)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_4		(0x04)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_5		(0x05)
#define	BHY_PARAMETER_REQUEST_READ_PARAMETER_6		(0x06)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_7		(0x07)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_8		(0x08)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_9		(0x09)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_10		(0x0A)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_11		(0x0B)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_12		(0x0C)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_13		(0x0D)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_14		(0x0E)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_15		(0x0F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_16		(0x10)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_17		(0x11)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_18		(0x12)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_19		(0x13)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_20		(0x14)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_21		(0x15)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_22		(0x16)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_23		(0x17)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_24		(0x18)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_25		(0x19)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_31		(0x1F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_33		(0x21)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_34		(0x22)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_35		(0x23)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_36		(0x24)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_37		(0x25)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_38		(0x26)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_39		(0x27)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_40		(0x28)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_41		(0x29)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_42		(0x2A)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_43		(0x2B)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_44		(0x2C)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_45		(0x2D)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_46		(0x2E)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_47		(0x2F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_48		(0x30)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_49		(0x31)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_50		(0x32)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_51		(0x33)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_52		(0x34)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_53		(0x35)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_54		(0x36)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_55		(0x37)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_56		(0x38)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_57		(0x39)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_63		(0x3F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_64		(0x40)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_65		(0x41)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_66		(0x42)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_67		(0x43)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_68		(0x44)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_73		(0x49)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_74		(0x4A)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_75		(0x4B)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_76		(0x4C)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_77		(0x4D)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_78		(0x4E)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_79		(0x4F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_80		(0x50)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_81		(0x51)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_82		(0x52)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_83		(0x53)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_84		(0x54)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_86		(0x56)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_87		(0x57)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_88		(0x58)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_89		(0x59)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_95		(0x5F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_97		(0x61)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_98		(0x62)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_99		(0x63)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_100	(0x64)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_101	(0x65)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_102	(0x66)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_103	(0x67)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_104	(0x68)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_105	(0x69)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_106	(0x6A)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_107	(0x6B)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_108	(0x6C)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_109	(0x6D)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_110	(0x6E)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_111	(0x6F)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_112	(0x70)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_113	(0x71)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_114	(0x72)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_115	(0x73)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_116	(0x74)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_117	(0x75)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_118	(0x76)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_119	(0x77)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_120	(0x78)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_121	(0x79)
#define BHY_PARAMETER_REQUEST_READ_PARAMETER_127	(0x7F)
/**************************************************************/
/**\name	WRITE PARAMETER REQUEST   */
/**************************************************************/
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_0		(0x80)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_1		(0x81)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_2		(0x82)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_3		(0x83)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_4		(0x84)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_5		(0x85)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_6		(0x86)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_7		(0x87)
#define BHY_PARAMETER_REQUEST_WRITE_PARAMETER_8		(0x88)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_9	    (0x89)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_10    (0x8A)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_11    (0x8B)
/* non wakeup sensor configuration*/
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_64	(0xC0)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_65	(0xC1)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_66	(0xC2)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_67	(0xC3)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_68	(0xC4)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_69	(0xC5)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_70	(0xC6)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_71	(0xC7)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_72	(0xC8)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_73	(0xC9)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_74	(0xCA)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_75	(0xCB)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_76	(0xCC)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_77	(0xCD)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_78	(0xCE)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_79	(0xCF)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_80	(0xD0)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_81	(0xD1)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_82	(0xD2)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_83	(0xD3)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_84	(0xD4)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_85	(0xD5)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_86	(0xD6)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_87	(0xD7)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_88	(0xD8)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_89	(0xD9)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_90	(0xDA)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_95	(0xDF)
/* wakeup sensor configuration*/
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_97	(0xE1)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_98	(0xE2)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_99	(0xE3)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_100	(0xE4)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_101	(0xE5)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_102	(0xE6)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_103	(0xE7)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_104	(0xE8)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_105	(0xE9)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_106	(0xEA)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_107	(0xEB)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_108	(0xEC)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_109	(0xED)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_110	(0xEE)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_111	(0xEF)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_112	(0xF0)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_113	(0xF1)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_114	(0xF2)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_115	(0xF3)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_116	(0xF4)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_117	(0xF5)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_118	(0xF6)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_119	(0xF7)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_120	(0xF8)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_121	(0xF9)
#define	BHY_PARAMETER_REQUEST_WRITE_PARAMETER_127	(0xFF)
/**************************************************************/
/**\name	BIT MASK DEFINITION FOR META EVENTS   */
/**************************************************************/
#define	BHY_META_EVENT1_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT1_EVENT_ENABLE	(0x02)

#define	BHY_META_EVENT2_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT2_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT3_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT3_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT4_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT4_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT5_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT5_EVENT_ENABLE	(0x02)

#define	BHY_META_EVENT6_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT6_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT7_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT7_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT8_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT8_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT9_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT9_EVENT_ENABLE	(0x02)

#define	BHY_META_EVENT10_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT10_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT11_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT11_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT12_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT12_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT13_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT13_EVENT_ENABLE	 (0x02)

#define	BHY_META_EVENT14_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT14_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT15_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT15_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT16_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT16_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT17_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT17_EVENT_ENABLE	 (0x02)

#define	BHY_META_EVENT18_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT18_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT19_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT19_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT20_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT20_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT21_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT21_EVENT_ENABLE	 (0x02)

#define	BHY_META_EVENT22_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT22_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT23_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT23_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT24_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT24_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT25_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT25_EVENT_ENABLE	 (0x02)

#define	BHY_META_EVENT26_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT26_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT27_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT27_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT28_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT28_EVENT_ENABLE    (0x80)

#define	BHY_META_EVENT29_INTR_ENABLE     (0x01)
#define	BHY_META_EVENT29_EVENT_ENABLE	 (0x02)

#define	BHY_META_EVENT30_INTR_ENABLE     (0x04)
#define	BHY_META_EVENT30_EVENT_ENABLE    (0x08)

#define	BHY_META_EVENT31_INTR_ENABLE     (0x10)
#define	BHY_META_EVENT31_EVENT_ENABLE    (0x20)

#define	BHY_META_EVENT32_INTR_ENABLE     (0x40)
#define	BHY_META_EVENT32_EVENT_ENABLE    (0x80)

/**************************************************************/
/**\name	BIT MASK DEFINITION FOR SENSOR BANKS   */
/**************************************************************/
#define	BHY_SENSOR_BANK_STATUS_DATA_AVAILABLE		(0x01)
#define	BHY_SENSOR_BANK_STATUS_I2C_NACK			(0x02)
#define	BHY_SENSOR_BANK_STATUS_DEVICE_ID_ERROR	(0x04)
#define	BHY_SENSOR_BANK_STATUS_TRANSIENT_ERROR	(0x08)
#define	BHY_SENSOR_BANK_STATUS_DATA_LOST			(0x10)
#define	BHY_SENSOR_BANK_STATUS_POWER_MODE			(0xE0)
#define	BHY_POWER_MODE_SENSOR_NOT_PRESENT		(0x00)
#define	BHY_POWER_MODE_POWER_DOWN				(0x01)
#define	BHY_POWER_MODE_POWER_SUSPEND			(0x02)
#define	BHY_POWER_MODE_POWER_SELFTEST			(0x03)
#define	BHY_POWER_MODE_POWER_INTR_MOTION		(0x04)
#define	BHY_POWER_MODE_POWER_ONE_SHOT			(0x05)
#define	BHY_POWER_MODE_POWER_LOW_POWER			(0x06)
#define	BHY_POWER_MODE_POWER_ACTIVE				(0x07)
#define BHY_SIGNATURE_1					(0)
#define BHY_SIGNATURE_2					(1)
#define BHY_SIGNATURE_LENGTH			(16)
#define BHY_RAM_WRITE_LENGTH		(4)
#define BHY_RAM_WRITE_LENGTH_API	(32)
#define	BHY_CHIP_CTRL_ENABLE_1		(0x02)
#define	BHY_CHIP_CTRL_ENABLE_2		(0x01)
#define BHY_UPLOAD_DATA				(0x00)
#define BHY_RESET_ENABLE			(0x01)
#define BHY_INIT_READ_BYTES			(19)
#define BHY_INIT_BYTE_MINUS_ONE     (1)
#define BHY_CHECK_BYTE				(3)
#define	BHY_IMAGE_SIGNATURE1		(0x2A)
#define	BHY_IMAGE_SIGNATURE2		(0x65)
/**************************************************************/
/**\name	META EVENT DEFINITION  */
/**************************************************************/
#define	BHY_META_EVENT_1		(1)
#define	BHY_META_EVENT_2		(2)
#define	BHY_META_EVENT_3		(3)
#define	BHY_META_EVENT_4		(4)
#define	BHY_META_EVENT_5		(5)
#define	BHY_META_EVENT_6		(6)
#define	BHY_META_EVENT_7		(7)
#define	BHY_META_EVENT_8		(8)
#define	BHY_META_EVENT_9		(9)
#define	BHY_META_EVENT_10		(10)
#define	BHY_META_EVENT_11		(11)
#define	BHY_META_EVENT_12		(12)
#define	BHY_META_EVENT_13		(13)
#define	BHY_META_EVENT_14		(14)
#define	BHY_META_EVENT_15		(15)
#define	BHY_META_EVENT_16		(16)
#define	BHY_META_EVENT_17		(17)
#define	BHY_META_EVENT_18		(18)
#define	BHY_META_EVENT_19		(19)
#define	BHY_META_EVENT_20		(20)
#define	BHY_META_EVENT_21		(21)
#define	BHY_META_EVENT_22		(22)
#define	BHY_META_EVENT_23		(23)
#define	BHY_META_EVENT_24		(24)
#define	BHY_META_EVENT_25		(25)
#define	BHY_META_EVENT_26		(26)
#define	BHY_META_EVENT_27		(27)
#define	BHY_META_EVENT_28		(28)
#define	BHY_META_EVENT_29		(29)
#define	BHY_META_EVENT_30		(30)
#define	BHY_META_EVENT_31		(31)
#define	BHY_META_EVENT_32		(32)
/**************************************************************/
/**\name	SENSOR ID DEFINITION  FOR NON WAKEUP*/
/**************************************************************/
#define BHY_NON_WAKEUP_ROTATION_VECTOR				(11)
#define BHY_NON_WAKEUP_GAME_ROTATION_VECTOR			(15)
#define BHY_NON_WAKEUP_GEOMAG_ROTATION_VECTOR		(20)
#define BHY_NON_WAKEUP_ACCELR						(1)
#define BHY_NON_WAKEUP_MAG                          (2)
#define BHY_NON_WAKEUP_ORIENTATION					(3)
#define BHY_NON_WAKEUP_GYRO                         (4)
#define BHY_NON_WAKEUP_GRAVITY						(9)
#define BHY_NON_WAKEUP_LINEAR_ACCEL					(10)
#define BHY_NON_WAKEUP_LIGHT						(5)
#define BHY_NON_WAKEUP_PROXIMITY					(8)
#define BHY_NON_WAKEUP_HUMIDITY						(12)
#define BHY_NON_WAKEUP_STEP_COUNTER					(19)
#define BHY_NON_WAKEUP_TEMPERATURE					(7)
#define BHY_NON_WAKEUP_AMBIENT_TEMPERATURE			(13)
#define BHY_NON_WAKEUP_BAROMETER					(6)
#define BHY_NON_WAKEUP_SIGNIFICANT_MOTION			(17)
#define BHY_NON_WAKEUP_STEP_DETECTOR				(18)
#define BHY_NON_WAKEUP_TILT_DETECTOR				(22)
#define BHY_NON_WAKEUP_WAKE_GESTURE					(23)
#define BHY_NON_WAKEUP_GLANCE_GESTURE				(24)
#define	BHY_NON_WAKEUP_PICKUP_GESTURE				(25)
#define	BHY_NON_WAKEUP_UNCALIB_MAG					(14)
#define	BHY_NON_WAKEUP_UNCALIB_GYRO					(16)
#define	BHY_NON_WAKEUP_HEART_RATE					(21)
#define	BHY_NON_WAKEUP_ACTIVITY						(31)
#define	BHY_NON_WAKEUP_BSX_C						(249)
#define	BHY_NON_WAKEUP_BSX_B						(250)
#define	BHY_NON_WAKEUP_BSX_A						(251)
#define	BHY_NON_WAKEUP_TIMESTAMP_LSW				(252)
#define	BHY_NON_WAKEUP_TIMESTAMP_MSW				(253)
#define	BHY_NON_WAKEUP_META_EVENTS					(254)

/**************************************************************/
/**\name	SENSOR ID DEFINITION FOR WAKEUP*/
/**************************************************************/
#define BHY_WAKEUP_ROTATION_VECTOR				(43)
#define BHY_WAKEUP_GAME_ROTATION_VECTOR			(47)
#define WAKEUP_GEOMAG_ROTATION_VECTOR			(52)
#define BHY_WAKEUP_ACCEL						(33)
#define BHY_WAKEUP_MAG							(34)
#define BHY_WAKEUP_ORIENTATION					(35)
#define BHY_WAKEUP_GYRO							(36)
#define BHY_WAKEUP_GRAVITY						(41)
#define BHY_WAKEUP_LINEAR_ACCEL					(42)
#define BHY_WAKEUP_LIGHT						(37)
#define BHY_WAKEUP_PROXIMITY					(40)
#define BHY_WAKEUP_HUMIDITY						(44)
#define BHY_WAKEUP_STEP_COUNTER					(51)
#define BHY_WAKEUP_TEMPERATURE					(39)
#define BHY_WAKEUP_AMBIENT_TEMPERATURE			(45)
#define BHY_WAKEUP_BAROMETER					(38)
#define BHY_WAKEUP_SIGNIFICANT_MOTION			(49)
#define BHY_WAKEUP_STEP_DETECTOR				(50)
#define BHY_WAKEUP_TILT_DETECTOR				(54)
#define BHY_WAKEUP_WAKE_GESTURE					(55)
#define BHY_WAKEUP_GLANCE_GESTURE				(56)
#define	BHY_WAKEUP_PICKUP_GESTURE				(57)
#define	BHY_WAKEUP_UNCALIB_MAG					(46)
#define	BHY_WAKEUP_UNCALIB_GYRO					(48)
#define	BHY_WAKEUP_HEART_RATE					(53)
#define	BHY_WAKEUP_ACTIVITY						(63)
#define	BHY_WAKEUP_TIMESTAMP_LSW				(246)
#define	BHY_WAKEUP_TIMESTAMP_MSW				(247)
#define	BHY_WAKEUP_META_EVENTS					(248)
/**************************************************************/
/**\name	USER DATA REGISTERS DEFINITION START    */
/**************************************************************/

/**************************************************************/
/**\name	FIFO FLUSH LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_FIFO_FLUSH__POS             (0)
#define BHY_I2C_REG_FIFO_FLUSH__MSK            (0xFF)
#define BHY_I2C_REG_FIFO_FLUSH__LEN             (8)
#define BHY_I2C_REG_FIFO_FLUSH__REG             (BHY_I2C_REG_FIFO_FLUSH_ADDR)
/**************************************************************/
/**\name	CHIP CONTROL LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_CHIP_CONTROL__POS             (0)
#define BHY_I2C_REG_CHIP_CONTROL__MSK            (0xFF)
#define BHY_I2C_REG_CHIP_CONTROL__LEN             (8)
#define BHY_I2C_REG_CHIP_CONTROL__REG            \
(BHY_I2C_REG_CHIP_CONTROL_ADDR)
/**************************************************************/
/**\name	HOST STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_HOST_STATUS__POS             (0)
#define BHY_I2C_REG_HOST_STATUS__MSK            (0xFF)
#define BHY_I2C_REG_HOST_STATUS__LEN             (8)
#define BHY_I2C_REG_HOST_STATUS__REG             (BHY_I2C_REG_HOST_STATUS_ADDR)

#define BHY_I2C_REG_HOST_STATUS_ALGO_STANDBY__POS  (1)
#define BHY_I2C_REG_HOST_STATUS_ALGO_STANDBY__MSK  (0x02)
#define BHY_I2C_REG_HOST_STATUS_ALGO_STANDBY__LEN  (1)
#define BHY_I2C_REG_HOST_STATUS_ALGO_STANDBY__REG  \
(BHY_I2C_REG_HOST_STATUS_ADDR)

#define BHY_I2C_REG_BHY_HOST_INTERFACE_ID__POS     (2)
#define BHY_I2C_REG_BHY_HOST_INTERFACE_ID__MSK     (0x1C)
#define BHY_I2C_REG_BHY_HOST_INTERFACE_ID__LEN     (3)
#define BHY_I2C_REG_BHY_HOST_INTERFACE_ID__REG     \
(BHY_I2C_REG_HOST_STATUS_ADDR)

#define BHY_I2C_REG_BHY_HOST_STATUS_ALGO_ID__POS     (5)
#define BHY_I2C_REG_BHY_HOST_STATUS_ALGO_ID__MSK     (0xE0)
#define BHY_I2C_REG_BHY_HOST_STATUS_ALGO_ID__LEN     (3)
#define BHY_I2C_REG_BHY_HOST_STATUS_ALGO_ID__REG     \
(BHY_I2C_REG_HOST_STATUS_ADDR)
/**************************************************************/
/**\name	INTERRUPT STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_INT_STATUS__POS             (0)
#define BHY_I2C_REG_INT_STATUS__MSK            (0xFF)
#define BHY_I2C_REG_INT_STATUS__LEN             (8)
#define BHY_I2C_REG_INT_STATUS__REG             (BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_BHY_INT_STATUS_HOST_INTR__POS  (0)
#define BHY_I2C_REG_BHY_INT_STATUS_HOST_INTR__MSK  (0x01)
#define BHY_I2C_REG_BHY_INT_STATUS_HOST_INTR__LEN  (1)
#define BHY_I2C_REG_BHY_INT_STATUS_HOST_INTR__REG  (BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_WM__POS    (1)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_WM__MSK    (0x02)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_WM__LEN    (1)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_WM__REG    \
(BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_LATENCY__POS   (2)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_LATENCY__MSK   (0x04)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_LATENCY__LEN   (1)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_LATENCY__REG   \
(BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_IMMEDIATE__POS   (3)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_IMMEDIATE__MSK   (0x08)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_IMMEDIATE__LEN   (1)
#define BHY_I2C_REG_BHY_INT_STATUS_WAKEUP_IMMEDIATE__REG   \
(BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_BHY_INT_STATUS_NON_WAKEUP_WM__POS   (4)
#define BHY_I2C_REG_BHY_INT_STATUS_NON_WAKEUP_WM__MSK   (0x10)
#define BHY_I2C_REG_BHY_INT_STATUS_NON_WAKEUP_WM__LEN   (1)
#define BHY_I2C_REG_BHY_INT_STATUS_NON_WAKEUP_WM__REG   \
(BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_LATENCY__POS   (5)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_LATENCY__MSK   (0x20)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_LATENCY__LEN   (1)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_LATENCY__REG   \
(BHY_I2C_REG_INT_STATUS_ADDR)

#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_IMMEDIATE__POS   (6)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_IMMEDIATE__MSK   (0x40)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_IMMEDIATE__LEN   (1)
#define BHY_I2C_REG_INT_STATUS_NON_WAKEUP_IMMEDIATE__REG   \
(BHY_I2C_REG_INT_STATUS_ADDR)
/**************************************************************/
/**\name CHIP STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_CHIP_STATUS__POS  (0)
#define BHY_I2C_REG_CHIP_STATUS__MSK  (0xFF)
#define BHY_I2C_REG_CHIP_STATUS__LEN  (8)
#define BHY_I2C_REG_CHIP_STATUS__REG  (BHY_I2C_REG_CHIP_STATUS_ADDR)

#define BHY_I2C_REG_CHIP_STATUS_EEPROM_DETECTED__POS  (0)
#define BHY_I2C_REG_CHIP_STATUS_EEPROM_DETECTED__MSK  (0x01)
#define BHY_I2C_REG_CHIP_STATUS_EEPROM_DETECTED__LEN  (1)
#define BHY_I2C_REG_CHIP_STATUS_EEPROM_DETECTED__REG  \
(BHY_I2C_REG_CHIP_STATUS_ADDR)

#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_DONE__POS  (1)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_DONE__MSK  (0x02)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_DONE__LEN  (1)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_DONE__REG  \
(BHY_I2C_REG_CHIP_STATUS_ADDR)

#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_ERROR__POS  (2)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_ERROR__MSK  (0x04)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_ERROR__LEN  (1)
#define BHY_I2C_REG_CHIP_STATUS_EE_UPLOAD_ERROR__REG  \
(BHY_I2C_REG_CHIP_STATUS_ADDR)

#define BHY_I2C_REG_CHIP_STATUS_FIRMWARE_IDLE__POS  (3)
#define BHY_I2C_REG_CHIP_STATUS_FIRMWARE_IDLE__MSK  (0x08)
#define BHY_I2C_REG_CHIP_STATUS_FIRMWARE_IDLE__LEN  (1)
#define BHY_I2C_REG_CHIP_STATUS_FIRMWARE_IDLE__REG  \
(BHY_I2C_REG_CHIP_STATUS_ADDR)

#define BHY_I2C_REG_CHIP_STATUS_NO_EEPROM__POS  (4)
#define BHY_I2C_REG_CHIP_STATUS_NO_EEPROM__MSK  (0x10)
#define BHY_I2C_REG_CHIP_STATUS_NO_EEPROM__LEN  (1)
#define BHY_I2C_REG_CHIP_STATUS_NO_EEPROM__REG  (BHY_I2C_REG_CHIP_STATUS_ADDR)

/**************************************************************/
/**\name PAGE SELECT LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT__POS    (0)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT__MSK    (0xFF)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT__LEN    (8)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT__REG     \
(BHY_I2C_REG_PARAMETER_PAGE_SELECT_ADDR)

#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_PAGE__POS    (0)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_PAGE__MSK    (0x0F)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_PAGE__LEN    (4)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_PAGE__REG    \
(BHY_I2C_REG_PARAMETER_PAGE_SELECT_ADDR)

#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_SIZE__POS    (4)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_SIZE__MSK    (0xF0)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_SIZE__LEN    (4)
#define BHY_I2C_REG_PARAMETER_PAGE_SELECT_PARAMETER_SIZE__REG    \
(BHY_I2C_REG_PARAMETER_PAGE_SELECT_ADDR)
/**************************************************************/
/**\name HOST INTERFACE SELECTION LENGTH, POSITION AND MASK    */
/**************************************************************/
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL__POS    (0)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL__MSK    (0xFF)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL__LEN    (8)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL__REG     \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__POS    (0)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__MSK    (0x01)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ALGO_STABDY_REQUEST__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__POS    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__MSK    (0x02)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_ABORT_TRANSFER__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__POS    (2)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__MSK    (0x04)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_TRANSFER_COUNT__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__POS    (3)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__MSK    (0x08)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_WAKEUP_FIFO_DISABLE__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__POS    (4)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__MSK    (0x10)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NED_COORDINATE__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__POS    (5)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__MSK    (0x20)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_AP_SUSPEND__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__POS    (6)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__MSK    (0x40)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__LEN    (1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_SELFTEST__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)

#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__POS    \
(7)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__MSK    \
(0x80)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__LEN    \
(1)
#define BHY_I2C_REG_HOST_INTERFACE_CONTROL_NON_WAKEUP_FIFO_DISABLE__REG    \
(BHY_I2C_REG_HOST_INTERFACE_CONTROL_ADDR)
/**************************************************************/
/**\name LOAD PARAMETER REQUEST SELECTION LENGTH, POSITION AND MASK */
/**************************************************************/
#define BHY_I2C_REG_LOAD_PARAMETER_REQUEST__POS    (0)
#define BHY_I2C_REG_LOAD_PARAMETER_REQUEST__MSK    (0xFF)
#define BHY_I2C_REG_LOAD_PARAMETER_REQUEST__LEN    (8)
#define BHY_I2C_REG_LOAD_PARAMETER_REQUEST__REG     \
(BHY_I2C_REG_PARAMETER_REQUEST_ADDR)

/**************************************************************/
/**\name RESET REQUEST SELECTION LENGTH, POSITION AND MASK */
/**************************************************************/
#define BHY_I2C_REG_RESET_REQUEST__POS    (0)
#define BHY_I2C_REG_RESET_REQUEST__MSK    (0xFF)
#define BHY_I2C_REG_RESET_REQUEST__LEN    (8)
#define BHY_I2C_REG_RESET_REQUEST__REG     \
(BHY_I2C_REG_RESET_REQUEST_ADDR)

/**************************************************/
/**\name	BIT SLICE GET AND SET FUNCTIONS  */
/*************************************************/
#define BHY_GET_BITSLICE(regvar, bitname)\
		((regvar & bitname##__MSK) >> bitname##__POS)


#define BHY_SET_BITSLICE(regvar, bitname, val)\
		((regvar & ~bitname##__MSK) | \
		((val<<bitname##__POS)&bitname##__MSK))

/**************************************************************/
/**\name USER SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_HOST_STATUS_ALGO_STANDBY		(0)
#define BHY_HOST_STATUS_ALGO_ID			(1)
#define BHY_FIFO_SIZE_WAKEUP			(0)
#define	BHY_FIFO_SIZE_NON_WAKEUP		(1)
#define	BHY_HOST_IRQ_TIMESTAMP			(0)
#define	BHY_CURRENT_TIME_STAMP			(1)
/**************************************************************/
/**\name INTERRUPT STATUS SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_INT_STATUS_HOST_INTR			(0)
#define BHY_INT_STATUS_WAKEUP_WM			(1)
#define BHY_INT_STATUS_WAKEUP_LATENCY		(2)
#define BHY_INT_STATUS_WAKEUP_IMMEDIATE		(3)
#define BHY_INT_STATUS_NON_WAKEUP_WM		(4)
#define BHY_INT_STATUS_NON_WAKEUP_LATENCY	(5)
#define BHY_INT_STATUS_NON_WAKEUP			(6)
/**************************************************************/
/**\name CHIP STATUS SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_CHIP_STATUS_EEPROM_DETECTED		(0)
#define BHY_CHIP_STATUS_EE_UPLOAD_DONE		(1)
#define BHY_CHIP_STATUS_EE_UPLOAD_ERROR		(2)
#define BHY_CHIP_STATUS_FIRMWARE_IDLE		(3)
#define BHY_CHIP_STATUS_NO_EEPROM			(4)
/**************************************************************/
/**\name PARAMETER PAGE SELECTION SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_PAGE_SELECT_PARAMETER_PAGE		(0)
#define BHY_PAGE_SELECT_PARAMETER_SIZE		(1)
/**************************************************************/
/**\name HOST INTERFACE CONTROL SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_HOST_ALGO_STANDBY_REQUEST		(0)
#define BHY_HOST_ABORT_TRANSFER				(1)
#define BHY_HOST_UPDATE_TRANSFER_COUNT		(2)
#define BHY_HOST_WAKEUP_FIFO_DISABLE		(3)
#define BHY_HOST_NED_COORDINATE				(4)
#define BHY_HOST_AP_SUSPEND					(5)
#define BHY_HOST_SELFTEST					(6)
#define BHY_HOST_NON_WAKEUP_FIFO_DISABLE	(7)
/**************************************************************/
/**\name META EVENT SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_META_EVENT_BYTE_0		(0)
#define BHY_META_EVENT_BYTE_1		(1)
#define BHY_META_EVENT_BYTE_2		(2)
#define BHY_META_EVENT_BYTE_3		(3)
#define BHY_META_EVENT_BYTE_4		(4)
#define BHY_META_EVENT_BYTE_5		(5)
#define BHY_META_EVENT_BYTE_6		(6)
#define BHY_META_EVENT_BYTE_7		(7)
/**************************************************************/
/**\name META EVENT AND INTERRUPT SELECTION DEFINITIONS  */
/**************************************************************/
#define BHY_META_INTR_ENABLE	(0)
#define	BHY_META_EVENT_ENABLE	(1)
/**************************************************************/
/**\name META EVENT BYTE-0 SELECTION */
/**************************************************************/
#define	BHY_FLUSH_COMPLETE		(0)
#define	BHY_SAMPLE_RATE_CHANGE	(1)
#define	BHY_POWER_MODE			(2)
#define	BHY_META_ERROR			(3)
/**************************************************************/
/**\name META EVENT BYTE-2 SELECTION */
/**************************************************************/
#define	BHY_SENSOR_ERROR		(0)
#define	BHY_FIFO_OVERFLOW		(1)
/**************************************************************/
/**\name META EVENT BYTE-3 SELECTION */
/**************************************************************/
#define	BHY_DYNAMIC_RANGE		(0)
#define	BHY_FIFO_WM				(1)
#define	BHY_SELF_TEST_RESULT	(2)
#define	BHY_INITIALIZED			(3)
/**************************************************************/
/**************************************************************/
/**\name FIFO WATER MARK SELECTION DEFINITIONS  */
/**************************************************************/
#define	BHY_FIFO_WATER_MARK_WAKEUP			(0)
#define	BHY_FIFO_WATER_MARK_NON_WAKEUP		(1)
/**************************************************************/
/**\name SENSOR STATUS BANK DEFINITIONS  */
/**************************************************************/
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_1   (0)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_2   (1)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_3   (3)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_4   (4)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_5   (5)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_6   (6)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_7   (7)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_8   (8)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_9   (9)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_10  (10)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_11  (11)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_12  (12)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_13  (13)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_14  (14)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_15  (15)
#define	BHY_SENSOR_STATUS_BANK_SENSOR_TYPE_16  (16)

/**************************************************************/
/**\name SENSOR STATUS BANK POWER MODE DEFINITION  */
/**************************************************************/

#define	BHY_SENSOR_NOT_PRESENT		(0x00)
#define	BHY_POWER_DOWN				(0x01)
#define	BHY_SUSPEND					(0x02)
#define	BHY_SELF_TEST				(0x03)
#define	BHY_INTR_MOTION				(0x04)
#define	BHY_ONE_SHOT				(0x05)
#define	BHY_LOW_POWER_ACTIVE		(0x06)
#define	BHY_ACTIVE					(0x07)

/**************************************************************/
/**\name FUNCTION DECLARATIONS  */
/**************************************************************/
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
BHY_RETURN_FUNCTION_TYPE bhy_init(struct bhy_t *bhy);
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
u8 *v_data_u8, u16 v_len_u16);
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
u8 *v_data_u8, u16 v_len_u16);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_fifo_flush(u8 *v_fifo_flush_u8);
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
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_set_fifo_flush(u8 v_fifo_flush_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_chip_control(u8 *v_chipcontrol_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_set_chip_control(u8 v_chipcontrol_u8);
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
u8 *v_host_interface_id_u8);
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
u8 *v_non_wakeup_latency_u8, u8 *v_non_wakeup_immediate_u8);
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
u8 *v_firmware_idle_u8, u8 *v_no_eeprom_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_read_bytes_remaining(
u16 *v_bytes_remaining_u16);
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
 *	@note
 *
 *
 *
 *
*/
BHY_RETURN_FUNCTION_TYPE bhy_get_parameter_acknowledge(
u8 *v_parameter_acknowledge_u8);
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
u8 v_page_select_u8, u8 *v_parameter_page_u8);
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
u8 v_page_select_u8);
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
u8 *v_non_wakeup_fifo_intr_disable_u8);
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
u8 v_host_interface_select_u8, u8 v_host_interface_status_u8);
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
u8 *v_parameter_request_u8);
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
u8 v_parameter_request_u8);
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
u32 *v_host_irq_timestamp_u32);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_rom_version(
u16 *v_rom_version_u16);
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
u16 *v_ram_version_u16);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_product_id(u8 *v_product_id_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_revision_id(u8 *v_revision_id_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_get_crc_host(u32 *v_crc_host_u32);
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
BHY_RETURN_FUNCTION_TYPE bhy_set_reset_request(u8 v_reset_request_u8);
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
BHY_RETURN_FUNCTION_TYPE bhy_set_meta_event(
u8 v_meta_event_u8,
u8 v_input_data_u8, u8 v_event_type_u8,
u8 v_meta_parameter_request_u8);
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
struct meta_event_event_t *meta_event, u8 v_meta_parameter_request_u8);
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
u8 v_parameter_u8, u16 *v_fifo_water_mark_u16);
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
u16 v_fifo_water_mark_u16);
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
u8 v_fifo_size_select_u8, u16 *v_fifo_size_u16);

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
u8 v_sensor_status_parameter_u8, u8 v_sensor_type_u8);
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
u8 v_time_stamp_selection_u8, u32 *v_time_stamp_u32);
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
struct mag_physical_status_t *mag_status);
/*!
 *	@brief API used to get the non wakeup sensor information,
 *	Sensor page-3 parameter 1 to 31
 *
 *	@param v_parameter_request_u8 :
 *	Value desired parameter to read non wakeup sensor information
 *        param_request                 |     value
 *   -----------------------------------|------------
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_1   |      0x01
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_2   |      0x02
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_3   |      0x03
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_4   |      0x04
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_5   |      0x05
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_6   |      0x06
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_7   |      0x87
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_8   |      0x88
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_9   |      0x89
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_10  |      0x8A
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_11  |      0x8B
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_12  |      0x8C
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_13  |      0x8D
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_14  |      0x8E
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_15  |      0x8F
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_16  |      0x90
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_17  |      0x91
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_18  |      0x92
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_19  |      0x93
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_20  |      0x94
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_21  |      0x95
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_22  |      0x96
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_23  |      0x97
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_24  |      0x98
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_25  |      0x99
 *     BHY_PARAMETER_REQUEST_READ_PARAMETER_31  |      0x9F
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
struct sensor_information_non_wakeup_t *sensor_information);
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
 *  BHY_PARAMETER_REQUEST_READ_PARAMETER_52  | 0xB4  | Geomagnetic rotation
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
struct sensor_information_wakeup_t *sensor_information);
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
u8 v_parameter_request_u8);
/*!
 *	@brief API used to get the sensor non wakeup configuration
 *	Sensor page-3 parameter 65 to 89
 *
 *	@param v_sample_rate_u16 : contains the non wakeup sample rate data
 *	@param v_max_report_latency_u16:
 *	contains the non wakeup max report latency
 *	@param v_change_sensitivity_u16: contains the non wakeup sensitivity
 *	@param v_dynamic_range_u16: contains the non wakeup dynamic range
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
u8 v_parameter_request_u8, u16 *v_sample_rate_u16,
u16 *v_max_report_latency_u16,
u16 *v_change_sensitivity_u16, u16 *v_dynamic_range_u16);
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
u8 v_parameter_request_u8);
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
u8 v_parameter_request_u8, u16 *v_sample_rate_u16,
u16 *v_max_report_latency_u16,
u16 *v_change_sensitivity_u16, u16 *v_dynamic_range_u16);
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
u8 v_parameter_request_u8);
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
u8 v_parameter_request_u8);
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
u8 v_page_select_u8, u8 v_parameter_request_u8);
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
u8 v_page_select_u8, u8 v_parameter_request_u8);
/*!
 *	@brief API used to flash the ram patch. There is two versions,
 *         one that accepts constant input data
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
u8 *memory, u32 v_file_length_u32);
BHY_RETURN_FUNCTION_TYPE bhy_initialize_from_rom(
const u8 *memory, const u32 v_file_length_u32);


#endif
