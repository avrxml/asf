/*****************************************************************************
 *
 * \file
 *
 * \brief Driver for the Cirrus Logic CS2200 clock synthesizer chip
 *
 *                       used.
 *
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CS2200_H_
#define _CS2200_H_

#include <avr32/io.h>
#include "compiler.h"

// Configuration file.
#include "conf_cs2200.h"


//! \name Communication Interfaces
//! @{
#define CS2200_INTERFACE_SPI  1
#define CS2200_INTERFACE_TWI  2
//! @}

//! Build the 32-bits ratio constant to be loaded in the CS2200 to make the desired frequency.
#define _32_BITS_RATIO(frequency, fosc)   ((int)(((float)frequency/(fosc))*(1<<20)))

//! Compute the PPM of a number
#define PPM(value, ppm)        ( ((int)( (long long)value*(ppm)/1000000) ) )

//! Add x PPM to a value
#define PPM_ADD(value, ppm)    ( ((int)(value + (long long)value*(ppm)/1000000)) )

//! Remove x PPM to a value
#define PPM_SUB(value, ppm)    ( ((int)(value - (long long)value*(ppm)/1000000)) )

// To check the value of the CS2200_NB_TRIES define.
#if CS2200_NB_TRIES < 1
#  error The number of times to initialize the CS2200 must be greater than 0. Update the CS2200_NB_TRIES define value.
#endif

#if CS2200_INTERFACE == CS2200_INTERFACE_SPI
#  error The SPI interface is not supported.
#elif CS2200_INTERFACE == CS2200_INTERFACE_TWI
#  define CS2200_TWI_ADDR      (0x9C >> 1)
// To check the value of the CS2200_TWI_MASTER_SPEED define.
#  if (CS2200_TWI_MASTER_SPEED > 100000)
#    error The TWI clock speed is out of the specificated range. It must be <= than 100KHz
#  endif
#endif

extern void cs2200_switch_on(void);
extern void cs2200_switch_off(void);

/*! \brief Function used to initialize the chip and communication interface.
 */
bool cs2200_setup(uint32_t out_freq, uint32_t fosc);

/*! \brief Function used to program the CLK_OUT frequency.
 */
void cs2200_freq_clk_out(uint32_t ratio);

/*! \brief Function used to adjust the CLK_OUT frequency (LSW only).
 */
void cs2200_freq_clk_adjust(uint16_t lsw_ratio);

/*! \brief Function used to program the CLK_OUT frequency and wait that the PLL is locked.
 */
void cs2200_set_new_freq_clk_out(uint32_t ratio);

/*! \brief Reads data from the CS2200 chip.
 */
void cs2200_read(uint8_t address, void *buffer, uint8_t len);

/*! \brief Writes data into the CS2200 chip.
 */
void cs2200_write(uint8_t address, const void *buffer, uint8_t len);

/*! \brief Writes data into the CS2200 chip using the non blocking release of
 *  the TWI driver.
 */
int cs2200_write_ex(uint8_t address, const void *buffer, uint8_t len);

/*! \brief Enter into the test mode.
 */
void cs2200_enter_test_mode(void);

/*! \brief Leave the test mode.
 */
void cs2200_leave_test_mode(void);

/*! \brief Enter power down mode and stop twi comms
 */
void cs2200_enter_power_down_mode(void);

/*! \brief Leave power down mode and twi comms can be restarted
 */
void cs2200_leave_power_down_mode(void);

/*! \name Macro Constructors
 *
 * These macros create inline functions to access the cs2200 registers.
 */
//! @{
#define CS2200_READ(name)                                                      \
static inline void CS2200_READ_##name(void *data, size_t len)                  \
{                                                                              \
  cs2200_read(CS2200_REG_##name##_ADDR, data, len);                            \
}

#define CS2200_READ1(name)                                                     \
static inline uint8_t CS2200_READ_##name(void)                                 \
{                                                                              \
  uint8_t data;                                                                \
  cs2200_read(CS2200_REG_##name##_ADDR, &data, sizeof(data));                  \
  return data;                                                                 \
}

#define CS2200_READ2(name)                                                     \
static inline uint16_t CS2200_READ_##name(void)                                \
{                                                                              \
  uint16_t data;                                                               \
  cs2200_read(CS2200_REG_##name##_ADDR, &data, sizeof(data));                  \
  return data;                                                                 \
}

#define CS2200_READ4(name)                                                     \
static inline uint32_t CS2200_READ_##name(void)                                \
{                                                                              \
  uint32_t data;                                                               \
  cs2200_read(CS2200_REG_##name##_ADDR, &data, sizeof(data));                  \
  return data;                                                                 \
}

#define CS2200_WRITE(name)                                                     \
static inline void CS2200_WRITE_##name(const void *data, size_t len)           \
{                                                                              \
  cs2200_write(CS2200_REG_##name##_ADDR, data, len);                           \
}

#define CS2200_WRITE1(name)                                                    \
static inline void CS2200_WRITE_##name(uint8_t data)                           \
{                                                                              \
  cs2200_write(CS2200_REG_##name##_ADDR, &data, sizeof(data));                 \
}

#define CS2200_WRITE2(name)                                                    \
static inline void CS2200_WRITE_##name(uint16_t data)                          \
{                                                                              \
  cs2200_write(CS2200_REG_##name##_ADDR, &data, sizeof(data));                 \
}

#define CS2200_WRITE4(name)                                                    \
static inline void CS2200_WRITE_##name(uint32_t data)                          \
{                                                                              \
  cs2200_write(CS2200_REG_##name##_ADDR, &data, sizeof(data));                 \
}
//! @}

/*! \name Register Addresses
 */
//! @{
#define CS2200_REG_TEST_MODE_1_ADDR       0x00
#define CS2200_REG_DEVICE_ID_ADDR         0x01
#define CS2200_REG_DEVICE_CTRL_ADDR       0x02
#define CS2200_REG_DEVICE_CFG_1_ADDR      0x03
#define CS2200_REG_GLOBAL_CFG_ADDR        0x05
#define CS2200_REG_32_BITS_RATIO_ADDR     0x06 // to 0x09
#define CS2200_REG_LSW_RATIO_ADDR         0x08 // to 0x09
#define CS2200_REG_LSB_RATIO_ADDR         0x09
#define CS2200_REG_FUNCT_CFG_1_ADDR       0x16
#define CS2200_REG_FUNCT_CFG_2_ADDR       0x17
#define CS2200_REG_TEST_MODE_2_ADDR       0x77
//! @}

// Read-only.
CS2200_READ1(DEVICE_ID);

// Read-write.
CS2200_READ1(DEVICE_CTRL);
CS2200_WRITE1(DEVICE_CTRL);
CS2200_READ1(DEVICE_CFG_1);
CS2200_WRITE1(DEVICE_CFG_1);
CS2200_READ1(GLOBAL_CFG);
CS2200_WRITE1(GLOBAL_CFG);
CS2200_READ4(32_BITS_RATIO);
CS2200_WRITE4(32_BITS_RATIO);
CS2200_WRITE1(LSB_RATIO);
CS2200_READ1(FUNCT_CFG_1);
CS2200_WRITE1(FUNCT_CFG_1);
CS2200_READ1(FUNCT_CFG_2);
CS2200_WRITE1(FUNCT_CFG_2);
CS2200_WRITE1(TEST_MODE_1);
CS2200_WRITE1(TEST_MODE_2);

#endif  // _CS2200_H_
