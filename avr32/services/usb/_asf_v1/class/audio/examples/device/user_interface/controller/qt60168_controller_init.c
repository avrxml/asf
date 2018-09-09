/**************************************************************************
 *
 * \file
 *
 * \brief QTouch Controller
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "rtc.h"
#include "spi.h"
#include "intc.h"
#include "qt60168.h"
#include "controller.h"
#include "conf_qt60168.h"


static unsigned short New_status = 0;
static unsigned short Old_status = 0;
static int            Read_data=0;
static unsigned short Data;
static int            report_state=0;

bool        special_qt60168_report_all_key(unsigned short *data);
static int  special_qt60168_get_reply(unsigned short *data);
static int  special_qt60168_send_cmd(unsigned char cmd);

#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void rtc_irq(void)
{
  static volatile int   delay_count=0;
  static unsigned short all_key= 0;
  static int            update_delay = 0;

  delay_count++;
  update_delay++;

  if(update_delay>10) {
    if(special_qt60168_report_all_key(&all_key)==true) {
        update_delay=0;
        //gpio_tgl_gpio_pin(LED2_GPIO);
        Old_status = New_status;
        New_status = all_key; // The one that has just been read
        if(Old_status != New_status) {
          update_joystick_status( New_status);
        }
        if(New_status!=0) { // LED2 on if key is currently pressed
          gpio_clr_gpio_pin(LED2_GPIO);
        }
        else {
          gpio_set_gpio_pin(LED2_GPIO);
        }
    }
  }

  if(Read_data==1) {
    Read_data=2;
    delay_count=0;
  }
  if((delay_count>1)&&(Read_data==2)) {
    // We can read the DATA
    // Select QT60168
    spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);
    // Read Reply
    spi_read(QT60168_SPI, &Data);
    // Unselect QT60168
    spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);
    Read_data=0;
  }

  // clear the interrupt flag
  rtc_clear_interrupt(&AVR32_RTC);

}

void rtc_init_qt( void ) {

  // Disable all interrupts. */
  Disable_global_interrupt();

  // Register the RTC interrupt handler to the interrupt controller.
  INTC_register_interrupt(&rtc_irq, AVR32_RTC_IRQ, AVR32_INTC_INT0);

  // Initialize the RTC
  // Frtc = 1024Hz
  rtc_init(&AVR32_RTC, RTC_OSC_32KHZ, 4);

  // Set top value to 0 to generate an interrupt every seconds */
  rtc_set_top_value(&AVR32_RTC, 1);
  // Enable the interrupts
  rtc_enable_interrupt(&AVR32_RTC);

  // Enable the RTC
  rtc_enable(&AVR32_RTC);

  // Enable global interrupts
  Enable_global_interrupt();

}

bool special_qt60168_report_all_key(unsigned short *data)
{
  static int AllKey=0;
  unsigned short dataread;

  switch(report_state) {
    case 0:
      AllKey=0;
      if(special_qt60168_send_cmd(QT60168_CMD_REPORT_ALL_KEYS)==0 ) {
        report_state=1;
      }
      break;
    case 1:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey=dataread;
        report_state=2;
      }
      break;
    case 2:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey|=dataread<<8;
        report_state=3;
      }
      break;
    case 3:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey|=dataread<<16;
        report_state=4;
      }
      break;
    case 4:
      if(special_qt60168_get_reply(&dataread)==0 ) { // Trash CRC of QT60168_CMD_REPORT_ALL_KEYS
        // Update data
        *data = AllKey;
        report_state=0;
      }
      break;
    default:
      report_state=0;
  }
  if (report_state==0) {
     return true;
  }
  else {
     return false;
  }
}

static int special_qt60168_send_cmd(unsigned char cmd)
{
  static int state=0;
  switch(state) {
    case 0:
      // Select QT60168
      spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);
      // Write CMD
      spi_write(QT60168_SPI, cmd);
      // Unselect QT60168
      spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);
      state = 1;
      Read_data=1;
    break;
    case 1:
      if(Read_data==0) {
        state=0;
      }
      break;
    default:
      state=0;
  }

  return state;
}

static int special_qt60168_get_reply(unsigned short *data)
{
    // Send NULL COMMAND
    if(special_qt60168_send_cmd(QT60168_CMD_NULL_COMMAND)==0) {
      *data = Data;
       return 0;
    }
    else {
      return 1;
    };
}
