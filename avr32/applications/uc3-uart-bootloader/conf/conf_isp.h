/**************************************************************************
 *
 * \file
 *
 * \brief ISP configuration file.
 *
 * This file contains the possible external configuration of the ISP.
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


#ifndef _CONF_ISP_H_
#define _CONF_ISP_H_

#include <avr32/io.h>
#include "compiler.h"
#include "trampoline.h"
#include "flash_api.h"

//_____ D E F I N I T I O N S ______________________________________________

// 1.0.2
#define ISP_VERSION                   0x02
#define ISP_ID0                       0x00
#define ISP_ID1                       0x00

#define ISP_CFG1                      (*(volatile U32 *)ISP_CFG1_ADDRESS)
#define ISP_CFG1_ADDRESS              (FLASH_API_USER_PAGE_ADDRESS + ISP_CFG1_OFFSET)
#if UC3L0 || UC3D
#define ISP_CFG1_OFFSET               0x000000FC
#elif UC3A || UC3B || UC3C || UC3L0128 || UC3L0256
#define ISP_CFG1_OFFSET               0x000001FC
#endif
#define ISP_CFG1_SIZE                 4

#define ISP_CFG1_BOOT_KEY1            16
#define ISP_CFG1_BOOT_KEY1_MASK       0xFFFF0000
#define ISP_CFG1_BOOT_KEY1_OFFSET     16
#define ISP_CFG1_BOOT_KEY1_SIZE       16
#define ISP_CFG1_BOOT_KEY1_VALUE      0xE11E

#define ISP_CFG1_FORCE                9
#define ISP_CFG1_FORCE_MASK           0x00000200
#define ISP_CFG1_FORCE_OFFSET         9
#define ISP_CFG1_FORCE_SIZE           1

#define ISP_CFG1_IO_COND_EN           8
#define ISP_CFG1_IO_COND_EN_MASK      0x00000100
#define ISP_CFG1_IO_COND_EN_OFFSET    8
#define ISP_CFG1_IO_COND_EN_SIZE      1

#define ISP_CFG1_CRC8                 0
#define ISP_CFG1_CRC8_MASK            0x000000FF
#define ISP_CFG1_CRC8_OFFSET          0
#define ISP_CFG1_CRC8_SIZE            8
#define ISP_CFG1_CRC8_POLYNOMIAL      0x107

#define ISP_CFG2                      (*(volatile U32 *)ISP_CFG2_ADDRESS)
#define ISP_CFG2_ADDRESS              (FLASH_API_USER_PAGE_ADDRESS + ISP_CFG2_OFFSET)
#if UC3L0 || UC3D
#define ISP_CFG2_OFFSET               0x000000F8
#elif UC3A || UC3B || UC3C || UC3L0128 || UC3L0256
#define ISP_CFG2_OFFSET               0x000001F8
#endif
#define ISP_CFG2_SIZE                 4

#define ISP_CFG2_BOOT_KEY             17
#define ISP_CFG2_BOOT_KEY_MASK        0xFFFE0000
#define ISP_CFG2_BOOT_KEY_OFFSET      17
#define ISP_CFG2_BOOT_KEY_SIZE        15
#define ISP_CFG2_BOOT_KEY_VALUE       0x494F

#define ISP_CFG2_IO_COND_LEVEL        16
#define ISP_CFG2_IO_COND_LEVEL_MASK   0x00010000
#define ISP_CFG2_IO_COND_LEVEL_OFFSET 16
#define ISP_CFG2_IO_COND_LEVEL_SIZE   1

#define ISP_CFG2_IO_COND_PIN          8
#define ISP_CFG2_IO_COND_PIN_MASK     0x0000FF00
#define ISP_CFG2_IO_COND_PIN_OFFSET   8
#define ISP_CFG2_IO_COND_PIN_SIZE     8

#define ISP_CFG2_CRC8                 0
#define ISP_CFG2_CRC8_MASK            0x000000FF
#define ISP_CFG2_CRC8_OFFSET          0
#define ISP_CFG2_CRC8_SIZE            8
#define ISP_CFG2_CRC8_POLYNOMIAL      0x107

#define ISP_KEY                       (*(volatile U32 *)ISP_KEY_ADDRESS)
#define ISP_KEY_ADDRESS               (AVR32_SRAM_ADDRESS + ISP_KEY_OFFSET)
#define ISP_KEY_OFFSET                0x00000000
#define ISP_KEY_SIZE                  4
#define ISP_KEY_VALUE                 ('I' << 24 | 'S' << 16 | 'P' << 8 | 'K')

#ifndef ISP_OSC
  #define ISP_OSC                     0
#endif

#define DFU_BUFFER_LENGTH_WRITE       2048 //
// Must be the same as in the rfl field of the RS232_I03.xml batchisp protocol
// description file.
#define DFU_BUFFER_LENGTH_READ        255

#define DFU_BUFFER_LENGTH             ((DFU_BUFFER_LENGTH_READ < DFU_BUFFER_LENGTH_WRITE) ? DFU_BUFFER_LENGTH_WRITE : DFU_BUFFER_LENGTH_READ)

#define PROGRAM_START_ADDRESS         (AVR32_FLASH_ADDRESS + PROGRAM_START_OFFSET)
#define PROGRAM_START_OFFSET          0x00002000


#endif  // _CONF_ISP_H_
