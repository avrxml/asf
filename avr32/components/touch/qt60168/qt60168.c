/*****************************************************************************
 *
 * \file
 *
 * \brief QT71168 driver for AVR32 UC3.
 *
 * This file is the QT60168 driver.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#ifndef _QT60168_C_
#define _QT60168_C_

#include "board.h"
#include "compiler.h"
#include "gpio.h"
#include "cycle_counter.h"
#include "conf_qt60168.h"
#include "qt60168.h"
#include "spi.h"

/*! \name QT60168 Config
 */
//! @{
#if !defined(QT60168_NTHR)      || \
    !defined(QT60168_NDRIFT)    || \
    !defined(QT60168_NDIL)      || \
    !defined(QT60168_FDIL)      || \
    !defined(QT60168_NDIL)      || \
    !defined(QT60168_NRD)       || \
    !defined(QT60168_BL)        || \
    !defined(QT60168_AKS)       || \
    !defined(QT60168_SSYNC)     || \
    !defined(QT60168_MSYNC)     || \
    !defined(QT60168_BS)        || \
    !defined(QT60168_LSL)       || \
    !defined(QT60168_EEPROM_CRC)
#  warning The QT60168 setups configuration to use in the driver is missing. Default configuration is used.
#define QT60168_NTHR        0x06
#define QT60168_NDRIFT      0x0F
#define QT60168_NDIL        0x01
#define QT60168_FDIL        0x05
#define QT60168_NRD         0x14
#define QT60168_BL          0x01
#define QT60168_AKS         0x01
#define QT60168_SSYNC       0x00
#define QT60168_MSYNC       0x00
#define QT60168_BS          0x01
#define QT60168_LSL         0x0064
#define QT60168_EEPROM_CRC  0x0A
#endif
//! @}

// Check DRDY pin (1 ready, 0 not ready)
//#define qt60168_check_device_ready() (gpio_get_pin_value(QT60168_DRDY_PIN))

U32 g_cpu_hz;


//_____  D E C L A R A T I O N S ___________________________________________


/*! \brief Init the QT60168 blocks
 */
static void qt60168_setup( void );

/*! \brief Wait for command reception.
 *
 */
void qt60168_wait_cmd_received(void);


//_____  P U B L I C   F U N C T I O N S ___________________________________

bool qt60168_is_no_key_pressed( void )
{
  unsigned short dataread, crc;

  qt60168_send_cmd(QT60168_CMD_REPORT_1ST_KEY);
  qt60168_get_reply(&dataread);
  qt60168_get_reply(&crc); // Trash CRC of QT60168_CMD_REPORT_1ST_KEY

  if((dataread&0x1F)==0x1F) return true;
  else return false;
}

bool qt60168_is_key_pressed(U8 key_id)
{
  unsigned short dataread, crc;

  qt60168_send_cmd(QT60168_CMD_STATUS_FOR_KEY_K | (key_id &0x1F));
  qt60168_get_reply(&dataread);
  qt60168_get_reply(&crc); // Trash CRC of QT60168_CMD_STATUS_FOR_KEY_K

  if(dataread & QT60168_KEY_STATUS_DETECTED) return true;
  else return false;
}

int qt60168_report_all_key(void)
{
  unsigned short dataread, crc;

  int allKey=0;
  qt60168_send_cmd(QT60168_CMD_REPORT_ALL_KEYS);
  qt60168_get_reply(&dataread);
  allKey=dataread;
  qt60168_get_reply(&dataread);
  allKey|=dataread<<8;
  qt60168_get_reply(&dataread);
  allKey|=dataread<<16;
  qt60168_get_reply(&crc); // Trash CRC of QT60168_CMD_REPORT_ALL_KEYS

  return allKey;

}

void qt60168_init(U32 cpu_hz)
{
  unsigned short dataread;
  g_cpu_hz = cpu_hz;

  // Check CRC
  qt60168_send_cmd(QT60168_CMD_EEPROM_CRC);
  qt60168_get_reply(&dataread);
  if(dataread != qt60168_setups_block.crc)
  {
    // Load EEPROM
    qt60168_setup();

    // Reset
    do
    {
      // The command must be repeated 2x within 100ms or the
      // command will fail.
      qt60168_send_cmd(QT60168_CMD_FORCE_RESET);
      qt60168_send_cmd(QT60168_CMD_FORCE_RESET);
      qt60168_get_reply(&dataread);
    }
    while(dataread != QT60168_REPLY_FORCE_RESET);
  }

  // Calibration
  // The command must be repeated 2x within 100ms or the
  // command will fail.
  qt60168_send_cmd(QT60168_CMD_CAL_ALL);
  qt60168_send_cmd(QT60168_CMD_CAL_ALL);
  qt60168_get_reply(&dataread);

  // Wait for Calibration
  do
  {
    qt60168_send_cmd(QT60168_CMD_GENERAL_STATUS);
    qt60168_get_reply(&dataread);

  }
  while(dataread & QT60168_STATUS_KEY_IN_CALIBRATION_MASK);
  return;
}

void qt60168_send_cmd(unsigned char cmd)
{
  // Select QT60168
  spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);

  // Write CMD
  spi_write(QT60168_SPI, cmd);

  // Wait for DRDY
  qt60168_wait_cmd_received();

  // Unselect QT60168
  spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);

  return;
}

void qt60168_get_reply(unsigned short *data)
{
    // Select QT60168
    spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);

    // Send NULL COMMAND
    spi_write(QT60168_SPI, QT60168_CMD_NULL_COMMAND);

    // Wait for DRDY
    qt60168_wait_cmd_received();

    // Read Reply
    spi_read(QT60168_SPI, data);

    // Unselect QT60168
    spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);

    return;
}



//_____  P R I V A T E   F U N C T I O N S _________________________________

static void qt60168_setup( void )
{
  unsigned short dataread;
  int i;

  // The command must be repeated 2x within 100ms or the
  // command will fail.
  qt60168_send_cmd(QT60168_CMD_ENTER_SETUPS_MODE);
  qt60168_send_cmd(QT60168_CMD_ENTER_SETUPS_MODE);

  qt60168_get_reply(&dataread);

  if(dataread != QT60168_REPLY_ENTER_SETUPS_MODE) return;

  // Send NTHR and NDRIFT
  for( i=0 ; i<QT60168_MAX_NUMBER_OF_KEYS ; i++ )
  {
    qt60168_send_cmd(qt60168_setups_block.key_cfg[i].ndrift_nthr);
  }

  for( i=0 ; i<QT60168_MAX_NUMBER_OF_KEYS ; i++)
  {
    qt60168_send_cmd(qt60168_setups_block.key_cfg[i].fdil_ndil);
  }

  // Send NRD
  for( i=0 ; i<QT60168_MAX_NUMBER_OF_KEYS ; i++)
  {
    qt60168_send_cmd(qt60168_setups_block.key_cfg[i].nrd);
  }

  // Send BL, AKS, SSYNC
  for( i=0 ; i<QT60168_MAX_NUMBER_OF_KEYS ; i++)
  {
    qt60168_send_cmd(qt60168_setups_block.key_cfg[i].ssync_aks_bl);
  }

  // Send MSYNC
  qt60168_send_cmd(qt60168_setups_block.msync);

  // Send BS
  qt60168_send_cmd(qt60168_setups_block.bs);

  // Send LSL
  qt60168_send_cmd(qt60168_setups_block.lsl & 0xFF);
  qt60168_send_cmd(qt60168_setups_block.lsl >> 8);

  qt60168_get_reply(&dataread);
  qt60168_get_reply(&dataread);
  return;
}

static int qt60168_check_device_ready(void)
{
#ifndef QT60168_DISABLE_DRDY
  int value;
#if BOARD==EVK1104
  // QT60168_DRDY_PIN Pin is on the same pin has AVR32_EBI_NWE1_0_PIN pin
  // It is require to temporary disable AVR32_EBI_NWE1_0_PIN function 0
  // to read correct status of QT60168_DRDY_PIN (This is only for EVK1104
  static const gpio_map_t SDRAMC_EBI_GPIO_MAP =
  {
   {AVR32_EBI_NWE1_0_PIN,AVR32_EBI_NWE1_0_FUNCTION}
  };
  gpio_enable_gpio_pin(QT60168_DRDY_PIN);
  // Wait for 1 us
  cpu_delay_cy( cpu_us_2_cy(1, g_cpu_hz) );
  // Get Value
#endif
  value=gpio_get_pin_value(QT60168_DRDY_PIN);
  // Give AVR32_EBI_NWE1_0_FUNCTION back
#if BOARD==EVK1104
    // restore AVR32_EBI_NWE1_0_PIN function
    gpio_enable_module(SDRAMC_EBI_GPIO_MAP, sizeof(SDRAMC_EBI_GPIO_MAP) / sizeof(SDRAMC_EBI_GPIO_MAP[0]));
#endif
  return value;
#else
  // Wait for 3000 us
  cpu_delay_cy( cpu_us_2_cy(26000, g_cpu_hz) );
  return 1;
#endif
}

void qt60168_wait_cmd_received(void)
{


  // Wait for 40 us before looking at DRDY
  cpu_delay_cy( cpu_us_2_cy(40, g_cpu_hz) );
  while(!qt60168_check_device_ready());

  return;
}

#endif // _QT60168_C_
