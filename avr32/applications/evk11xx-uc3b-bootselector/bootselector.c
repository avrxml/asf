/*****************************************************************************
 *
 * \file
 *
 * \brief Boot Selector Application.
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

/*! \mainpage
 * \section intro Introduction
 *  This program can be use to run two different application programs (program 1 and program 2) on the UC3B1256 present on
 *  most EVK board.

 *  The selection of the program is chosen according to the USB mode button state upon power-up.
 *
 *  - If the USB mode button is pressed, then program 2 will be executed
 *  - Else program 1 will be executed
 *
 *  The boot selector program is located from 0x80002000 to 0x80002FFF. You must take care of
 *    keeping the USB DFU bootloader on the AT32UC3B, otherwise this application will not work.
 *
 * \section compinfo Compilation Info
 *    The Program 1 must start at address 0x80003000 and must not exceed 122KBytes.
 *    Update linker script to force linker to build at this address :

 *       - FLASH (rxai!w) :  ORIGIN = 0x80003000, LENGTH = 0x0001E800

 *    The Program 2 must start at address 0x80021800 and must not exceed 122KBytes.
 *     update linker script to force linker to build at this address :

 *       - FLASH (rxai!w) :  ORIGIN = 0x80021800, LENGTH = 0x0001E800
 *
 *   There are predefined linker scripts files present in the /linker_script
 *   folder of the present application:
 *
 *   - linker_scripts\gcc\link_uc3b1256_prog1.lds, for PROGRAM1_START_ADDRESS under gcc.
 *   - linker_scripts\gcc\link_uc3b1256_prog2.lds, for PROGRAM2_START_ADDRESS under gcc.
 *   - linker_scripts\iar\lnkuc3b1256_prog1.xcl, for PROGRAM1_START_ADDRESS under iar.
 *   - linker_scripts\iar\lnkuc3b1256_prog2.xcl, for PROGRAM2_START_ADDRESS under iar.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/8-bit/avr-mcus/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <avr32/io.h>
#include "board.h"
#include "gpio.h"

// USB MODE BUTTON
#define GPIO_USB_MODE_BUTTON     AVR32_PIN_PA03

// Start Address of the 1st Program
#define PROGRAM1_START_OFFSET      0x00003000
#define PROGRAM1_START_ADDRESS     (AVR32_FLASH_ADDRESS + PROGRAM1_START_OFFSET)

// Start Address of the 2nd Program
#define PROGRAM2_START_OFFSET      0x00021800
#define PROGRAM2_START_ADDRESS     (AVR32_FLASH_ADDRESS + PROGRAM2_START_OFFSET)

/*! \brief This function will execute a jump to the given address
 *
 *  \param address the address to jump.
 */
void jumpAddress(unsigned int address)
{
  // Address is store in r12
  asm ("icall r12");
}

/*! \brief This is the main function.
 *
 */
int main(void)
{
  U8 button_status=0;

  // Read Button Status
  button_status=gpio_get_pin_value(GPIO_USB_MODE_BUTTON);

  // If Button is pressed, launch prog2.
  if(button_status==0) {
    jumpAddress(PROGRAM2_START_ADDRESS);
  }
  // else launch prog1.
  else {
    jumpAddress(PROGRAM1_START_ADDRESS);
  }
  while(1); // Never reached
}
