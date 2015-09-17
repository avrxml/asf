/**************************************************************************
 *
 * \file
 *
 * \brief QTouch Controller
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "qt60168_controller_init.h"

#include "controller.h"


#include "cycle_counter.h"

#include "spi.h"

#include "conf_qt60168.h"


#define JOYSTICK_STATUS_IDLE     0x00000000
#define JOYSTICK_STATUS_UP       0x00000001
#define JOYSTICK_STATUS_DOWN     0x00000002
#define JOYSTICK_STATUS_RIGHT    0x00000004
#define JOYSTICK_STATUS_LEFT     0x00000008
#define JOYSTICK_STATUS_PRESSED  0x00000010
static int qwheel_status = JOYSTICK_STATUS_IDLE;

void evaluate_detect_status(bool status, int status_pressed)
{
    //pressed
    if (status == 1) {
      qwheel_status |= status_pressed;
    }
    // release
    else {
      qwheel_status &= ~status_pressed;
    }
}


void update_joystick_status(unsigned short new_status)
{
  bool status;

  status = (new_status & (1<<QT60168_TOUCH_SENSOR_WHEEL_UP))?true:false;
  evaluate_detect_status(status, JOYSTICK_STATUS_UP);


  status =(new_status & (1<<QT60168_TOUCH_SENSOR_WHEEL_DOWN))?true:false;
  evaluate_detect_status(status, JOYSTICK_STATUS_DOWN);


  status = (new_status & (1<<QT60168_TOUCH_SENSOR_WHEEL_RIGHT))?true:false;
  evaluate_detect_status(status, JOYSTICK_STATUS_RIGHT);


  status = (new_status & (1<<QT60168_TOUCH_SENSOR_WHEEL_LEFT))?true:false;
  evaluate_detect_status(status, JOYSTICK_STATUS_LEFT);


  status = (new_status & (1<<QT60168_TOUCH_SENSOR_BUTTON_0))?true:false;
  evaluate_detect_status(status, JOYSTICK_STATUS_PRESSED);

}

void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz)
{
   static const gpio_map_t QT60168_SPI_GPIO_MAP = { { QT60168_SPI_SCK_PIN,
      QT60168_SPI_SCK_FUNCTION }, // SPI Clock.
      { QT60168_SPI_MISO_PIN, QT60168_SPI_MISO_FUNCTION }, // MISO.
      { QT60168_SPI_MOSI_PIN, QT60168_SPI_MOSI_FUNCTION }, // MOSI.
      { QT60168_SPI_NPCS0_PIN, QT60168_SPI_NPCS0_FUNCTION } // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions = {
      .reg = QT60168_SPI_NCPS,
      .baudrate = QT60168_SPI_MASTER_SPEED, // Defined in conf_qt60168.h.
      .bits = QT60168_SPI_BITS, // Defined in conf_qt60168.h.
      .spck_delay = 0,
      .trans_delay = 0,
      .stay_act = 0,
      .spi_mode = 3,
      .modfdis = 1 };

  // Assign I/Os to SPI.
  gpio_enable_module(QT60168_SPI_GPIO_MAP, sizeof(QT60168_SPI_GPIO_MAP)
      / sizeof(QT60168_SPI_GPIO_MAP[0]));

   // Initialize as master.
  spi_initMaster(QT60168_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(QT60168_SPI, 0, 0, 0);

  // Enable SPI.
  spi_enable(QT60168_SPI);

  // Initialize QT60168 with SPI clock Osc0.
  spi_setupChipReg(QT60168_SPI, &spiOptions, fpba_hz);

  // Initialize QT60168 component.
  qt60168_init(fpba_hz);

  rtc_init_qt();
}


bool is_joystick_up(void)
{
  return (qwheel_status&JOYSTICK_STATUS_UP)?true:false;
}

bool is_joystick_down(void)
{    return (qwheel_status&JOYSTICK_STATUS_DOWN)?true:false;

}

bool is_joystick_right(void)
{
    return (qwheel_status&JOYSTICK_STATUS_RIGHT)?true:false;
}

bool is_joystick_left(void)
{
    return (qwheel_status&JOYSTICK_STATUS_LEFT)?true:false;
}

bool is_joystick_pressed(void)
{
    return (qwheel_status&JOYSTICK_STATUS_PRESSED)?true:false;
}
