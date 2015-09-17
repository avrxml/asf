/*****************************************************************************
 *
 * \file
 *
 * \brief LCD DIP204 driver
 *
 * This file defines a useful set of functions for the DIP204 interface on
 * AVR32 devices.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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


// Define _ASSERT_ENABLE_ just below to detect SPI errors, such
// as timeout, ...
//#define _ASSERT_ENABLE_
#include <stdarg.h>
#include <stdio.h>
#include "board.h"
#include "conf_dip204.h"
#include "dip204.h"
#include "gpio.h"
#include "compiler.h"
#include "spi.h"
#include "pwm.h"
#include "delay.h"


/*! Write Command start byte */
#define DIP204_WRITE_COMMAND             0xF8

/*! Write Data start byte */
#define DIP204_WRITE_DATA                0xFA

/*! Read Command start byte, for Busy-flag checking */
#define DIP204_READ_COMMAND              0xFC

/*! Read Data from DDRAM */
#define DIP204_READ_DATA                 0xFE

#define DIP204_PERIOD_MAX                  50

#define DIP204_CGRAM_BASE_ADDR           0x40

static void dip204_write_byte(unsigned char byte);
static void dip204_read_byte(unsigned char *byte);
static void dip204_select(void);
static void dip204_unselect(void);
static void dip204_wait_busy(void);


/*! the duty cycle that will be updated to change backlight power */
static unsigned short pwm_duty;

/*! the PWM channel config */
static avr32_pwm_channel_t pwm_channel;

/*! The channel number */
static int channel_id = -1;

#ifdef _ASSERT_ENABLE_
static spi_status_t spi_status;
#endif


/****************************** global functions *****************************/

void dip204_init(backlight_options option, bool backlight_on)
{
  pwm_opt_t pwm_opt;  // pwm option config

  if (option == backlight_PWM)
  {
    channel_id = DIP204_PWM_CHANNEL;
    gpio_enable_module_pin(DIP204_PWM_PIN, DIP204_PWM_FUNCTION);

    // PWM controller configuration
    pwm_opt.diva=0;
    pwm_opt.divb=0;
    pwm_opt.prea=0;
    pwm_opt.preb=0;

    pwm_init(&pwm_opt);
    pwm_duty = (backlight_on) ? DIP204_PERIOD_MAX - 1 : 1;
    pwm_channel.CMR.calg = PWM_MODE_LEFT_ALIGNED;   // channel mode
    pwm_channel.CMR.cpol = PWM_POLARITY_LOW;   // channel polarity
    pwm_channel.CMR.cpd = PWM_UPDATE_PERIOD;   // not used the first time
    pwm_channel.CMR.cpre = AVR32_PWM_CPRE_MCK_DIV_256;   // channel prescaler
    pwm_channel.cdty = pwm_duty;  // channel duty cycle, should be < CPRD
    pwm_channel.cprd = DIP204_PERIOD_MAX;  // channel period
    pwm_channel.cupd = 0;  // channel update is not used here.

    pwm_channel_init(channel_id, &pwm_channel);
    // start PWM
    pwm_start_channels(1 << channel_id);
  }
  else
  {
    if (backlight_on)
    {
      gpio_clr_gpio_pin(DIP204_BACKLIGHT_PIN);
    }
    else
    {
      gpio_set_gpio_pin(DIP204_BACKLIGHT_PIN);
    }
  }
  // delay for power on
  delay_ms(20);
  // select the LCD chip
  dip204_select();
  // Send Command Start Byte
  dip204_write_byte(DIP204_WRITE_COMMAND);
  // Send "extended Function Set" Command  (RE=1)
  dip204_write_byte(0x34);
  // Wait for command execution
  delay_ms(1);
  // Send "Enter 4-Line Mode" Command
  dip204_write_byte(0x09);
  // Wait for command execution
  delay_ms(1);
  // Send "Function Set" Command (RE=0)
  dip204_write_byte(0x30);
  // Wait for command execution
  delay_ms(1);
  // Send "Display On Command: Display On, Cursor On, Blink On"
  dip204_write_byte(0x0F);
  // Wait for command execution
  delay_ms(1);
  // Send "Display Clear" Command
  dip204_write_byte(0x01);
  // Wait for command execution
  delay_ms(5);
  // Send "Entry Mode Set Command: Increment Mode, Entire Shift off"
  dip204_write_byte(0x06);
  // Wait for command execution
  delay_ms(1);
  dip204_wait_busy();
  // unselect chip
  dip204_unselect();
}


void dip204_set_backlight(backlight_power power)
{
  if (channel_id != -1)
  {
    if (power == backlight_power_decrease)
    {
      // update channel duty cycle using double buffering to prevent unexpected waveform.
      pwm_duty = Max(pwm_duty - (DIP204_PERIOD_MAX / 10), 1);
      pwm_channel.CMR.cpd = PWM_UPDATE_DUTY;
      // new duty cycle
      pwm_channel.cupd = pwm_duty;
      // set channel configuration.
      pwm_sync_update_channel(channel_id, &pwm_channel);
    }
    else if (power == backlight_power_increase)
    {
      // update channel duty cycle using double buffering to prevent unexpected waveform.
      pwm_duty = Min(pwm_duty + (DIP204_PERIOD_MAX / 10), DIP204_PERIOD_MAX - 1);
      pwm_channel.CMR.cpd = PWM_UPDATE_DUTY;
      // new duty cycle
      pwm_channel.cupd = pwm_duty;
      // set channel configuration.
      pwm_sync_update_channel(channel_id, &pwm_channel);
    }
  }
  else
  {
    if (power == backlight_power_decrease)
    {
      gpio_set_gpio_pin(DIP204_BACKLIGHT_PIN);
    }
    else if (power == backlight_power_increase)
    {
      gpio_clr_gpio_pin(DIP204_BACKLIGHT_PIN);
    }
  }
}


void dip204_show_cursor(void)
{
  /* select the LCD chip */
  dip204_select();
  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send "Display On Command: Display On, Cursor On, Blink On" */
  dip204_write_byte(0x0F);
  dip204_wait_busy();
  /* unselect chip */
  dip204_unselect();
}


void dip204_hide_cursor(void)
{
  /* select the LCD chip */
  dip204_select();
  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send "Display On Command: Display On, Cursor On, Blink On" */
  dip204_write_byte(0x0C);
  dip204_wait_busy();
  /* unselect chip */
  dip204_unselect();
}


void dip204_write_data(unsigned char data)
{
  dip204_select();
  /* Send Write Data Start-Byte */
  dip204_write_byte(DIP204_WRITE_DATA);
  /* send data */
  dip204_write_byte(data);
  dip204_wait_busy();
  dip204_unselect();
}


void dip204_read_data(unsigned char *data)
{
  dip204_select();
  /* Send Read Data Start-Byte */
  dip204_write_byte(DIP204_READ_DATA);
  /* read SPI data */
  dip204_read_byte(data);
  /* wait for LCD */
  dip204_wait_busy();
  dip204_unselect();
}


void dip204_create_char(char ascii_code, const unsigned char data[8])
{
  unsigned char i;
  unsigned char column, line;

  /* select the LCD chip */
  dip204_select();

  /* save cursor position */
  /* Send Read Command Start-Byte */
  dip204_write_byte(DIP204_READ_COMMAND);
  /* Read status */
  dip204_read_byte(&i);
  /* Extract and save line and column cursor information */
  line = ((i&0x60) >> 5) + 1;
  column = (i&0x1F) + 1;

  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send CGRAM Address Set command */
  dip204_write_byte(DIP204_CGRAM_BASE_ADDR + ((ascii_code << 3)&0x38));
  /* wait for LCD */
  dip204_wait_busy();

  /* To proceed the 8 lines */
  for(i=0; i<8; i++)
  {
    /* Send Data Start Byte */
    dip204_write_byte(DIP204_WRITE_DATA);
    /* send data */
    dip204_write_byte(data[i] & 0x1F); //data[i]);
    /* wait for LCD */
    dip204_wait_busy();
  }

  /* unselect chip */
  dip204_unselect();

  /* Reset cursor position */
  dip204_set_cursor_position(column, line);
}


void dip204_set_cursor_position(unsigned char column, unsigned char line)
{
  unsigned char address = 0;

  dip204_select();
  if ((column <= 20) && (line <= 4))
  {
    /* Calculate DDRAM address from line and row values */
    address = ( (line-1) * 32 ) + ( column-1 ) + 128;
  }
  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send Address lower Nibble */
  dip204_write_byte(address);
  dip204_wait_busy();
  dip204_unselect();
}


void dip204_clear_display(void)
{
  dip204_select();
  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send Display Clear Command */
  dip204_write_byte(0x01);
  /* Wait for command execution */
  delay_ms(4);
  dip204_wait_busy();
  dip204_unselect();
}


void dip204_write_string(const char *string)
{
  unsigned char i=0;

  dip204_select();
  /* for all chars in string */
  while(string[i]!=0)
  {
    /* Send Write Data Start Byte */
    dip204_write_byte(DIP204_WRITE_DATA);
    /* Send byte */
    dip204_write_byte(string[i]);
    /* go to next char */
    i++;
    dip204_wait_busy();
  }
  dip204_unselect();
}


void dip204_printf_string(const char *format, ...)
{
  va_list arg;
  char string[21];
  unsigned char i=0;

  va_start(arg, format);
  i = vsprintf(string, format, arg);
  while (i < sizeof(string) - 1) string[i++] = '\0';
  va_end(arg);
  dip204_select();
  /* for all chars in string */
  i = 0;
  while(string[i]!='\0')
  {
    /* Send Write Data Start Byte */
    dip204_write_byte(DIP204_WRITE_DATA);
    /* Send byte */
    dip204_write_byte(string[i]);
    /* go to next char */
    i++;
    dip204_wait_busy();
  }
  dip204_unselect();
}


/****************************** local functions ******************************/

/*! \brief function to select the LCD
 *
 */
static void dip204_select(void)
{
  spi_selectChip(DIP204_SPI, DIP204_SPI_NPCS);
}


/*! \brief function to unselect the LCD
 *
 */
static void dip204_unselect(void)
{
#ifdef _ASSERT_ENABLE_
  spi_status =
#endif
  spi_unselectChip(DIP204_SPI, DIP204_SPI_NPCS);
  Assert( SPI_OK==spi_status );
}


/*! \brief hardware abstraction layer to send a byte to LCD
 *         depends if LCD is plugged on SPI or on EBI
 *
 *  \param  byte  Input. byte to write to the LCD (D7 .. D0)
 *
 */
static void dip204_write_byte(unsigned char byte)
{
  unsigned char reverse;

  switch (byte)
  {
    /* MSB first for command */
    case DIP204_READ_COMMAND:
    case DIP204_WRITE_COMMAND:
    case DIP204_READ_DATA:
    case DIP204_WRITE_DATA:
    {
      /* send D7 to D0 */
#ifdef _ASSERT_ENABLE_
      spi_status =
#endif
      spi_write(DIP204_SPI, byte);
      Assert( SPI_OK==spi_status );
      break;
    }
    /* LSB first for all other data */
    default:
    {
      /* reverse byte */
      reverse = bit_reverse8(byte);
      /* send D0 to D3 */
#ifdef _ASSERT_ENABLE_
      spi_status =
#endif
      spi_write(DIP204_SPI, (reverse & 0xF0));
      Assert( SPI_OK==spi_status );
      /* send D4 to D7 */
#ifdef _ASSERT_ENABLE_
      spi_status =
#endif
      spi_write(DIP204_SPI, ((reverse << 4) & 0xF0));
      Assert( SPI_OK==spi_status );
      break;
    }
  }
}


/*! \brief hardware abstraction layer to read a byte from LCD
 *         depends if LCD is plugged on SPI or on EBI
 *
 *  \param  byte  Input. byte read from the LCD (D7 .. D0)
 *
 */
static void dip204_read_byte(unsigned char *byte)
{
  unsigned short reverse = 0x00;

  /* dummy write */
#ifdef _ASSERT_ENABLE_
  spi_status =
#endif
  spi_write(DIP204_SPI, 0x00);
  Assert( SPI_OK==spi_status );
  /* read RSR register */
#ifdef _ASSERT_ENABLE_
  spi_status =
#endif
  spi_read(DIP204_SPI, &reverse);
  Assert( SPI_OK==spi_status );
  /* Revert received byte (issued LSB first by the LCD) */
  *byte = bit_reverse8(reverse);
}


/*! \brief function to wait for LCD becomes not busy
 *
 */
static void dip204_wait_busy(void)
{
  unsigned char status = 0x00;

  /* send read command to LCD */
  dip204_write_byte(DIP204_READ_COMMAND);
  /* read next byte */
  do {
  dip204_read_byte(&status);
  /* keep D7 to know status */
  }while (status & 0x80);
}
