/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/* 
 * The AT24MAC602 is a serial EEPROM on the REB233 X-Pro board containing, among
 * other things, a 64-bit MAC address. This driver is intended to in the simplest
 * possible way read out this MAC address for use with the Thingsquare firmware.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "contiki.h"

#include "asf.h"
#include "at24mac602.h"
#include "at24mac602-arch.h"
#include "samd21_xplained_pro.h"
#include "trx_access.h"
/*---------------------------------------------------------------------------*/
/* 
 * Define the I2C pins. On the REB233-XPro board, the SDA and SCL pins are
 * shared over all three extension headers.
 */
#define AT24MAC602_SDA_PIN      PIN_PA08
#define AT24MAC602_SCL_PIN      PIN_PA09

#define SCL_0()         port_pin_set_output_level(AT24MAC602_SCL_PIN, false)
#define SCL_1()         port_pin_set_output_level(AT24MAC602_SCL_PIN, true)
#define SDA_0()         port_pin_set_output_level(AT24MAC602_SDA_PIN, false)
#define SDA_1()         port_pin_set_output_level(AT24MAC602_SDA_PIN, true)
#define SDA_IS_1()      port_pin_get_input_level(AT24MAC602_SDA_PIN)

static void set_sda_input(void);
static void set_sda_output(void);
static int  read(uint8_t address, uint8_t len, uint8_t *buf);

#define EEPROM_EUI64_ADDRESS        0x98
#define EEPROM_SERIAL128_ADDRESS    0x80
/*---------------------------------------------------------------------------*/
/**
 * \brief      Init the AT24MAC602 EEPROM arch dependencies, ie MCU pins etc
 * \retval 0   Success
 * \retval -1  Fail
 */
int
at24mac602_arch_init(void)
{
  struct port_config pin_conf;

  port_get_config_defaults(&pin_conf);
  pin_conf.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(AT24MAC602_SCL_PIN, &pin_conf);
  port_pin_set_output_level(AT24MAC602_SCL_PIN, false);

  pin_conf.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(AT24MAC602_SDA_PIN, &pin_conf);
  port_pin_set_output_level(AT24MAC602_SDA_PIN, false);
  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief          Read the EEPROM EUI-64
 * \param eui      Pointer EUI-64 structure
 * \retval  0      Success
 * \retval -1      Fail
 */
int
at24mac602_arch_read_eui64(eui64_t *eui)
{
#define LEN_EUI    8
  uint8_t readbuf[LEN_EUI];
  int i;
  if(read(EEPROM_EUI64_ADDRESS, LEN_EUI, readbuf) == -1) {
    printf("eeprom eui64 read fail.\n");
    return -1;
  }

  for(i = 0; i < LEN_EUI; i++) {
    eui->u8[i] = readbuf[i];
  }
 return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief          Read the EEPROM EUI-64
 * \param eui      Pointer EUI-64 structure
 * \retval  0      Success
 * \retval -1      Fail
 */
int
at24mac602_arch_read_serial128(at24mac602_serialnumber_t *serial)
{
#define LEN_SERIALNUMBER    16
  uint8_t readbuf[LEN_SERIALNUMBER];
  int i;
  if(read(EEPROM_SERIAL128_ADDRESS, LEN_SERIALNUMBER, readbuf) == -1) {
    printf("eeprom serial128 read fail.\n");
    return -1;
  }

  for(i = 0; i < LEN_SERIALNUMBER; i++) {
    serial->u8[i] = readbuf[i];
  }
 return 0;
}
/*---------------------------------------------------------------------------*/
/* 
 * As this a bit-banged approach, we introduce some procedural redundancy in
 * order to reduce I2C speed to less than the 1 MHz the EEPROM is specified for.
 */
#define START_CONDITION()   do {                           \
                              SDA_1();                     \
                              SDA_1();                     \
                              SCL_1();                     \
                              SCL_1();                     \
                              SDA_0();                     \
                              SDA_0();                     \
                              SCL_0();                     \
                              SCL_0();                     \
                            } while(0)


#define STOP_CONDITION()    do {                           \
                              SDA_0();                     \
                              SDA_0();                     \
                              SCL_1();                     \
                              SCL_1();                     \
                              SDA_1();                     \
                              SDA_1();                     \
                              SCL_0();                     \
                              SCL_0();                     \
                            } while(0)

#define OUTPUT_BYTE(b)      do {                               \
                              for(n = 7; n >= 0; n--) {        \
                                /* send bit, MSB */            \
                                SCL_0();                       \
                                if(b & (1 << n)) {             \
                                  SDA_1();                     \
                                } else {                       \
                                  SDA_0();                     \
                                }                              \
                                SCL_1();                       \
                              }                                \
                              SCL_0();                         \
                              SCL_0();                         \
                              SCL_1();                         \
                              SCL_1();                         \
                              SCL_0();                         \
                              SCL_0();                         \
                            } while(0)
/*---------------------------------------------------------------------------*/
/* Set SDA pin as input */
static void
set_sda_input(void)
{
  struct port_config pin_conf;
  pin_conf.direction = PORT_PIN_DIR_INPUT;
  pin_conf.input_pull = PORT_PIN_PULL_NONE;
  port_pin_set_config(AT24MAC602_SDA_PIN, &pin_conf);
}
/*---------------------------------------------------------------------------*/
/* Set SDA pin as output */
static void
set_sda_output(void)
{
  struct port_config pin_conf;
  pin_conf.direction = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(AT24MAC602_SDA_PIN, &pin_conf);
  port_pin_set_output_level(AT24MAC602_SDA_PIN, false);
}
/*---------------------------------------------------------------------------*/
/* Reset the I2C endpoint on the EEPROM (see datasheet) */
void
reset_i2c(void)
{
  int i;
  set_sda_output();
  START_CONDITION();
  for(i = 0; i < 9; i++) {
    SCL_0();
    SCL_1();
  }
  SCL_0();
  START_CONDITION();
  STOP_CONDITION();
}
/*---------------------------------------------------------------------------*/
/* Read a number of bytes from a specified address in the extended mem area. */
static int
read(uint8_t address, uint8_t len, uint8_t *buf)
{
  #define COMMAND_WRITE     0xB0    /* Write I2C command */
  #define COMMAND_READ      0xB1    /* Read I2C command */
  int i, n;
 ENTER_TRX_CRITICAL_REGION();
  /* 
   * In order to ensure we get the proper data read out, we should perform a
   * dummy write to the device so we start off at the right address. Then, we
   * restart the operation as a read and read out the EUI-64. But first, reset
   * the I2C on the EEPROM so we know it's in a good state.
   */
  reset_i2c();

  /* slave address byte, write operation */
  START_CONDITION();
  OUTPUT_BYTE(COMMAND_WRITE);

  /* dummy write to start address, sets the address pointer right */
  OUTPUT_BYTE(address);
  STOP_CONDITION();

  /* start condition, restart transaction as a read operation */
  START_CONDITION();
  OUTPUT_BYTE(COMMAND_READ);
  
  /* read out the data */
  set_sda_input();
  for(i = 0; i < len; i++) {
    volatile uint8_t temp = 0;
    for(n = 7; n >= 0; n--) {
      /* get byte bit by bit, MSB first */
      SCL_0();
      if(SDA_IS_1()) {
        temp |= 1 << n;
      }
      SCL_1();
    }

    /* store the read byte */
    buf[i] = temp;

    /* mcu ACK to the EEPROM */
    SCL_0();
    set_sda_output();
    SDA_0();
    SCL_1();
    SCL_0();
    set_sda_input();
  }

  /* finish up */
  STOP_CONDITION();
  LEAVE_TRX_CRITICAL_REGION();
  return 0;
 }
/*---------------------------------------------------------------------------*/
