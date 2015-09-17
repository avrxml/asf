/*****************************************************************************
 *
 * \file
 *
 * \brief Boot Selector Application.
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
 * <A href="http://www.atmel.com/products/avr/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
