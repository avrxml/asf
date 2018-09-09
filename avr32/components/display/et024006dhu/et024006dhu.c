/*****************************************************************************
 *
 * \file
 *
 * \brief _ET024006DHU_C_ file for ET024006DHU TFT display driver.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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

/*============================ INCLUDES ======================================*/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "compiler.h"
#include "et024006dhu.h"
#include "conf_et024006dhu.h"

#include "smc.h"
#include "board.h"
#include "gpio.h"
#include "cycle_counter.h"


/*============================ PRIVATE DEFINITIONS ===========================*/


#define ET024006_DMA_USAGE_LIMIT  10

#define ET024006_ID               0x70
#define ET024006_BS0              0x04
#define ET024006_RS               0x02
#define ET024006_RW               0x01

/* Himax HX8347-A01 display controller registers and bitfield definitions */
#define HIMAX_DISPMODECTRL        0x01
#define HIMAX_INVON               2
#define HIMAX_NORON		  1

#define HIMAX_COL_ADDR_START2     0x02
#define HIMAX_COL_ADDR_START1     0x03
#define HIMAX_COL_ADDR_END2       0x04
#define HIMAX_COL_ADDR_END1       0x05
#define HIMAX_ROW_ADDR_START2     0x06
#define HIMAX_ROW_ADDR_START1     0x07
#define HIMAX_ROW_ADDR_END2       0x08
#define HIMAX_ROW_ADDR_END1       0x09
#define HIMAX_PA_START_ROW_HB     0x0A
#define HIMAX_PA_START_ROW_LB     0x0B
#define HIMAX_PA_END_ROW_HB       0x0C
#define HIMAX_PA_END_ROW_LB       0x0D
#define HIMAX_VS_TFA_HB           0x0E
#define HIMAX_VS_TFA_LB           0x0F
#define HIMAX_VS_VSA_HB           0x10
#define HIMAX_VS_VSA_LB           0x11
#define HIMAX_VS_BFA_HB           0x12
#define HIMAX_VS_BFA_LB           0x13
#define HIMAX_VS_VSP_HB           0x14
#define HIMAX_VS_VSP_LB           0x15


#define HIMAX_MEMACCESSCTRL       0x16
#define HIMAX_MY                  7
#define HIMAX_MX                  6
#define HIMAX_MV                  5
#define HIMAX_BGR                 3

#define HIMAX_GATE_SCAN_CTRL      0x18
#define HIMAX_SCROLL_ON           0x2

#define HIMAX_OSCCTRL1            0x19
#define HIMAX_OSC_EN              0 // Default 0

#define HIMAX_OSCCTRL2		  0x1A

#define HIMAX_POWERCTRL1          0x1B
#define HIMAX_GASENB              7 // Default 0
#define HIMAX_PON                 4 // Default 0
#define HIMAX_DK                  3 // Default 1
#define HIMAX_XDK                 2 // Default 0
#define HIMAX_VTFT_TRI            1 // Default 0
#define HIMAX_STB                 0 // Default 1

#define HIMAX_POWERCTRL2          0x1c
#define HIMAX_AP2                 2 // Default 0
#define HIMAX_AP1                 1 // Default 0
#define HIMAX_AP0                 0 // Default 0

#define HIMAX_POWERCTRL3          0x1D
#define HIMAX_POWERCTRL4          0x1E
#define HIMAX_POWERCTRL5          0x1F
#define HIMAX_POWERCTRL6          0x20

#define HIMAX_SRAMWRITE           0x22

#define HIMAX_CYCLECTRL1_x	0x23
#define HIMAX_CYCLECTRL2_x	0x24
#define HIMAX_CYCLECTRL3_x	0x25

#define HIMAX_DISPCTRL1           0x26
#define HIMAX_PT1                 7 // Default 0
#define HIMAX_PT0                 6 // Default 0
#define HIMAX_GON                 5 // Default 1
#define HIMAX_DTE                 4 // Default 0
#define HIMAX_D1                  3 // Default 0
#define HIMAX_D0                  2 // Default 0

#define HIMAX_DISPCTRL2		0x27
#define HIMAX_DISPCTRL3		0x28
#define HIMAX_DISPCTRL4		0x29
#define HIMAX_DISPCTRL5		0x2A
#define HIMAX_DISPCTRL6		0x2C
#define HIMAX_DISPCTRL7		0x2D

#define HIMAX_DISPCTRL9		0x35
#define HIMAX_DISPCTRL10	0x36


#define HIMAX_CYCLECTRL1	0x3A//11
#define HIMAX_CYCLECTRL2	0x3B//12
#define HIMAX_CYCLECTRL3	0x3C//13
#define HIMAX_CYCLECTRL4	0x3D//14
#define HIMAX_CYCLECTRL5	0x3E//15
#define HIMAX_CYCLECTRL6	0x40//16
#define HIMAX_CYCLECTRL14	0x41//17

#define HIMAX_VCOMCTRL1           0x43
#define HIMAX_VCOMG               7 // Default 0

#define HIMAX_VCOMCTRL2           0x44
#define HIMAX_VCOMCTRL3           0x45

#define HIMAX_GAMMACTRL1          0x46
#define HIMAX_GAMMACTRL2          0x47
#define HIMAX_GAMMACTRL3          0x48
#define HIMAX_GAMMACTRL4          0x49
#define HIMAX_GAMMACTRL5          0x4A
#define HIMAX_GAMMACTRL6          0x4B
#define HIMAX_GAMMACTRL7          0x4C
#define HIMAX_GAMMACTRL8          0x4D
#define HIMAX_GAMMACTRL9          0x4E
#define HIMAX_GAMMACTRL10         0x4F
#define HIMAX_GAMMACTRL11         0x50
#define HIMAX_GAMMACTRL12         0x51

#define HIMAX_ID_CODE             0x67
#define HIMAX_ID_CODE_VALUE       0x47

#define HIMAX_INTERNAL28          0x70
#define HIMAX_TEON                3

#define HIMAX_SAP                 0x90
#define HIMAX_OSCCTRL3	          0x93
#define HIMAX_DISPCTRL8		  0x90

const unsigned char FONT6x8[97][8] = {
  {0x06,0x08,0x08,0x00,0x00,0x00,0x00,0x00}, // columns, rows, num_bytes_per_char
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // space 0x20
  {0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00}, // !
  {0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00}, // "
  {0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00}, // #
  {0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00}, // $
  {0xC0,0xC8,0x10,0x20,0x40,0x98,0x18,0x00}, // %
  {0x40,0xA0,0xA0,0x40,0xA8,0x90,0x68,0x00}, // &
  {0x30,0x30,0x20,0x40,0x00,0x00,0x00,0x00}, // '
  {0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00}, // (
  {0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00}, // )
  {0x00,0x20,0xA8,0x70,0x70,0xA8,0x20,0x00}, // *
  {0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00}, // +
  {0x00,0x00,0x00,0x00,0x30,0x30,0x20,0x40}, // ,
  {0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00}, // -
  {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00}, // .
  {0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00}, // / (forward slash)
  {0x70,0x88,0x88,0xA8,0x88,0x88,0x70,0x00}, // 0 0x30
  {0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00}, // 1
  {0x70,0x88,0x08,0x70,0x80,0x80,0xF8,0x00}, // 2
  {0xF8,0x08,0x10,0x30,0x08,0x88,0x70,0x00}, // 3
  {0x10,0x30,0x50,0x90,0xF8,0x10,0x10,0x00}, // 4
  {0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00}, // 5
  {0x38,0x40,0x80,0xF0,0x88,0x88,0x70,0x00}, // 6
  {0xF8,0x08,0x08,0x10,0x20,0x40,0x80,0x00}, // 7
  {0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00}, // 8
  {0x70,0x88,0x88,0x78,0x08,0x10,0xE0,0x00}, // 9
  {0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x00}, // :
  {0x00,0x00,0x20,0x00,0x20,0x20,0x40,0x00}, // ;
  {0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x00}, // <
  {0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00}, // =
  {0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x00}, // >
  {0x70,0x88,0x08,0x30,0x20,0x00,0x20,0x00}, // ?
  {0x70,0x88,0xA8,0xB8,0xB0,0x80,0x78,0x00}, // @ 0x40
  {0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00}, // A
  {0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00}, // B
  {0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00}, // C
  {0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00}, // D
  {0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00}, // E
  {0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00}, // F
  {0x78,0x88,0x80,0x80,0x98,0x88,0x78,0x00}, // G
  {0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00}, // H
  {0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00}, // I
  {0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00}, // J
  {0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88,0x00}, // K
  {0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00}, // L
  {0x88,0xD8,0xA8,0xA8,0xA8,0x88,0x88,0x00}, // M
  {0x88,0x88,0xC8,0xA8,0x98,0x88,0x88,0x00}, // N
  {0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00}, // O
  {0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00}, // P 0x50
  {0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00}, // Q
  {0xF0,0x88,0x88,0xF0,0xA0,0x90,0x88,0x00}, // R
  {0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00}, // S
  {0xF8,0xA8,0x20,0x20,0x20,0x20,0x20,0x00}, // T
  {0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00}, // U
  {0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00}, // V
  {0x88,0x88,0x88,0xA8,0xA8,0xA8,0x50,0x00}, // W
  {0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00}, // X
  {0x88,0x88,0x50,0x20,0x20,0x20,0x20,0x00}, // Y
  {0xF8,0x08,0x10,0x70,0x40,0x80,0xF8,0x00}, // Z
  {0x78,0x40,0x40,0x40,0x40,0x40,0x78,0x00}, // [
  {0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00}, // \ (back slash)
  {0x78,0x08,0x08,0x08,0x08,0x08,0x78,0x00}, // ]
  {0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00}, // ^
  {0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00}, // _
  {0x60,0x60,0x20,0x10,0x00,0x00,0x00,0x00}, // ` 0x60
  {0x00,0x00,0x60,0x10,0x70,0x90,0x78,0x00}, // a
  {0x80,0x80,0xB0,0xC8,0x88,0xC8,0xB0,0x00}, // b
  {0x00,0x00,0x70,0x88,0x80,0x88,0x70,0x00}, // c
  {0x08,0x08,0x68,0x98,0x88,0x98,0x68,0x00}, // d
  {0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00}, // e
  {0x10,0x28,0x20,0x70,0x20,0x20,0x20,0x00}, // f
  {0x00,0x00,0x70,0x98,0x98,0x68,0x08,0x70}, // g
  {0x80,0x80,0xB0,0xC8,0x88,0x88,0x88,0x00}, // h
  {0x20,0x00,0x60,0x20,0x20,0x20,0x70,0x00}, // i
  {0x10,0x00,0x10,0x10,0x10,0x90,0x60,0x00}, // j
  {0x80,0x80,0x90,0xA0,0xC0,0xA0,0x90,0x00}, // k
  {0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00}, // l
  {0x00,0x00,0xD0,0xA8,0xA8,0xA8,0xA8,0x00}, // m
  {0x00,0x00,0xB0,0xC8,0x88,0x88,0x88,0x00}, // n
  {0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00}, // o
  {0x00,0x00,0xB0,0xC8,0xC8,0xB0,0x80,0x80}, // p 0x70
  {0x00,0x00,0x68,0x98,0x98,0x68,0x08,0x08}, // q
  {0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00}, // r
  {0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00}, // s
  {0x20,0x20,0xF8,0x20,0x20,0x28,0x10,0x00}, // t
  {0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x00}, // u
  {0x00,0x00,0x88,0x88,0x88,0x50,0x20,0x00}, // v
  {0x00,0x00,0x88,0x88,0xA8,0xA8,0x50,0x00}, // w
  {0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00}, // x
  {0x00,0x00,0x88,0x88,0x78,0x08,0x88,0x70}, // y
  {0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00}, // z
  {0x10,0x20,0x20,0x40,0x20,0x20,0x10,0x00}, // {
  {0x20,0x20,0x20,0x00,0x20,0x20,0x20,0x00}, // |
  {0x40,0x20,0x20,0x10,0x20,0x20,0x40,0x00}, // }
  {0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00}, // ~
  {0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00} // DEL
};

const unsigned char FONT8x8[97][8] = {
  {0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00}, // columns, rows, num_bytes_per_char
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // space 0x20
  {0x30,0x78,0x78,0x30,0x30,0x00,0x30,0x00}, // !
  {0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00}, // "
  {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // #
  {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, // $
  {0x00,0x63,0x66,0x0C,0x18,0x33,0x63,0x00}, // %
  {0x1C,0x36,0x1C,0x3B,0x6E,0x66,0x3B,0x00}, // &
  {0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00}, // '
  {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // (
  {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // )
  {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // *
  {0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00}, // +
  {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // ,
  {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // -
  {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // .
  {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, // / (forward slash)
  {0x3E,0x63,0x63,0x6B,0x63,0x63,0x3E,0x00}, // 0 0x30
  {0x18,0x38,0x58,0x18,0x18,0x18,0x7E,0x00}, // 1
  {0x3C,0x66,0x06,0x1C,0x30,0x66,0x7E,0x00}, // 2
  {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 3
  {0x0E,0x1E,0x36,0x66,0x7F,0x06,0x0F,0x00}, // 4
  {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 5
  {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 6
  {0x7E,0x66,0x06,0x0C,0x18,0x18,0x18,0x00}, // 7
  {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 8
  {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}, // 9
  {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // :
  {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // ;
  {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00}, // <
  {0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00}, // =
  {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, // >
  {0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00}, // ?
  {0x3E,0x63,0x6F,0x69,0x6F,0x60,0x3E,0x00}, // @ 0x40
  {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // A
  {0x7E,0x33,0x33,0x3E,0x33,0x33,0x7E,0x00}, // B
  {0x1E,0x33,0x60,0x60,0x60,0x33,0x1E,0x00}, // C
  {0x7C,0x36,0x33,0x33,0x33,0x36,0x7C,0x00}, // D
  {0x7F,0x31,0x34,0x3C,0x34,0x31,0x7F,0x00}, // E
  {0x7F,0x31,0x34,0x3C,0x34,0x30,0x78,0x00}, // F
  {0x1E,0x33,0x60,0x60,0x67,0x33,0x1F,0x00}, // G
  {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // H
  {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // I
  {0x0F,0x06,0x06,0x06,0x66,0x66,0x3C,0x00}, // J
  {0x73,0x33,0x36,0x3C,0x36,0x33,0x73,0x00}, // K
  {0x78,0x30,0x30,0x30,0x31,0x33,0x7F,0x00}, // L
  {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, // M
  {0x63,0x73,0x7B,0x6F,0x67,0x63,0x63,0x00}, // N
  {0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00}, // O
  {0x7E,0x33,0x33,0x3E,0x30,0x30,0x78,0x00}, // P 0x50
  {0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00}, // Q
  {0x7E,0x33,0x33,0x3E,0x36,0x33,0x73,0x00}, // R
  {0x3C,0x66,0x30,0x18,0x0C,0x66,0x3C,0x00}, // S
  {0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00}, // T
  {0x66,0x66,0x66,0x66,0x66,0x66,0x7E,0x00}, // U
  {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // V
  {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // W
  {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, // X
  {0x66,0x66,0x66,0x3C,0x18,0x18,0x3C,0x00}, // Y
  {0x7F,0x63,0x46,0x0C,0x19,0x33,0x7F,0x00}, // Z
  {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // [
  {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, // \ (back slash)
  {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // ]
  {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, // ^
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // _
  {0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // ` 0x60
  {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3B,0x00}, // a
  {0x70,0x30,0x3E,0x33,0x33,0x33,0x6E,0x00}, // b
  {0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00}, // c
  {0x0E,0x06,0x3E,0x66,0x66,0x66,0x3B,0x00}, // d
  {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00}, // e
  {0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00}, // f
  {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x7C}, // g
  {0x70,0x30,0x36,0x3B,0x33,0x33,0x73,0x00}, // h
  {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // i
  {0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3C}, // j
  {0x70,0x30,0x33,0x36,0x3C,0x36,0x73,0x00}, // k
  {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // l
  {0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00}, // m
  {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00}, // n
  {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00}, // o
  {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, // p 0x70
  {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, // q
  {0x00,0x00,0x6E,0x3B,0x33,0x30,0x78,0x00}, // r
  {0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00}, // s
  {0x08,0x18,0x3E,0x18,0x18,0x1A,0x0C,0x00}, // t
  {0x00,0x00,0x66,0x66,0x66,0x66,0x3B,0x00}, // u
  {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00}, // v
  {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, // w
  {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, // x
  {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x7C}, // y
  {0x00,0x00,0x7E,0x4C,0x18,0x32,0x7E,0x00}, // z
  {0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00}, // {
  {0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x0C,0x00}, // |
  {0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00}, // }
  {0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00}, // ~
  {0x1C,0x36,0x36,0x1C,0x00,0x00,0x00,0x00}// DEL
};
const unsigned char FONT8x16[97][16] = {
  {0x08,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // columns, rows, num_bytes_per_char
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // space 0x20
  {0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00}, // !
  {0x00,0x63,0x63,0x63,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // "
  {0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36,0x36,0x7F,0x36,0x36,0x00,0x00,0x00,0x00}, // #
  {0x0C,0x0C,0x3E,0x63,0x61,0x60,0x3E,0x03,0x03,0x43,0x63,0x3E,0x0C,0x0C,0x00,0x00}, // $
  {0x00,0x00,0x00,0x00,0x00,0x61,0x63,0x06,0x0C,0x18,0x33,0x63,0x00,0x00,0x00,0x00}, // %
  {0x00,0x00,0x00,0x1C,0x36,0x36,0x1C,0x3B,0x6E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00}, // &
  {0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '
  {0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00,0x00,0x00}, // (
  {0x00,0x00,0x18,0x0C,0x0C,0x06,0x06,0x06,0x06,0x0C,0x0C,0x18,0x00,0x00,0x00,0x00}, // )
  {0x00,0x00,0x00,0x00,0x42,0x66,0x3C,0xFF,0x3C,0x66,0x42,0x00,0x00,0x00,0x00,0x00}, // *
  {0x00,0x00,0x00,0x00,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00}, // +
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00}, // ,
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // -
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00}, // .
  {0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00}, // / (forward slash)
  {0x00,0x00,0x3E,0x63,0x63,0x63,0x6B,0x6B,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // 0 0x30
  {0x00,0x00,0x0C,0x1C,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3F,0x00,0x00,0x00,0x00}, // 1
  {0x00,0x00,0x3E,0x63,0x03,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00}, // 2
  {0x00,0x00,0x3E,0x63,0x03,0x03,0x1E,0x03,0x03,0x03,0x63,0x3E,0x00,0x00,0x00,0x00}, // 3
  {0x00,0x00,0x06,0x0E,0x1E,0x36,0x66,0x66,0x7F,0x06,0x06,0x0F,0x00,0x00,0x00,0x00}, // 4
  {0x00,0x00,0x7F,0x60,0x60,0x60,0x7E,0x03,0x03,0x63,0x73,0x3E,0x00,0x00,0x00,0x00}, // 5
  {0x00,0x00,0x1C,0x30,0x60,0x60,0x7E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // 6
  {0x00,0x00,0x7F,0x63,0x03,0x06,0x06,0x0C,0x0C,0x18,0x18,0x18,0x00,0x00,0x00,0x00}, // 7
  {0x00,0x00,0x3E,0x63,0x63,0x63,0x3E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // 8
  {0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x3F,0x03,0x03,0x06,0x3C,0x00,0x00,0x00,0x00}, // 9
  {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00}, // :
  {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00}, // ;
  {0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00}, // <
  {0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00}, // =
  {0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00}, // >
  {0x00,0x00,0x3E,0x63,0x63,0x06,0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00}, // ?
  {0x00,0x00,0x3E,0x63,0x63,0x6F,0x6B,0x6B,0x6E,0x60,0x60,0x3E,0x00,0x00,0x00,0x00}, // @ 0x40
  {0x00,0x00,0x08,0x1C,0x36,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x00,0x00,0x00,0x00}, // A
  {0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x33,0x33,0x33,0x33,0x7E,0x00,0x00,0x00,0x00}, // B
  {0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1E,0x00,0x00,0x00,0x00}, // C
  {0x00,0x00,0x7C,0x36,0x33,0x33,0x33,0x33,0x33,0x33,0x36,0x7C,0x00,0x00,0x00,0x00}, // D
  {0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00}, // E
  {0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00}, // F
  {0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x6F,0x63,0x63,0x37,0x1D,0x00,0x00,0x00,0x00}, // G
  {0x00,0x00,0x63,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00}, // H
  {0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00}, // I
  {0x00,0x00,0x0F,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00}, // J
  {0x00,0x00,0x73,0x33,0x36,0x36,0x3C,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00}, // K
  {0x00,0x00,0x78,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00}, // L
  {0x00,0x00,0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00}, // M
  {0x00,0x00,0x63,0x63,0x73,0x7B,0x7F,0x6F,0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00}, // N
  {0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x00,0x00,0x00,0x00}, // O
  {0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00}, // P 0x50
  {0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x63,0x6B,0x6F,0x3E,0x06,0x07,0x00,0x00}, // Q
  {0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00}, // R
  {0x00,0x00,0x3E,0x63,0x63,0x30,0x1C,0x06,0x03,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // S
  {0x00,0x00,0xFF,0xDB,0x99,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00}, // T
  {0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // U
  {0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x08,0x00,0x00,0x00,0x00}, // V
  {0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x36,0x00,0x00,0x00,0x00}, // W
  {0x00,0x00,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x3C,0x66,0xC3,0xC3,0x00,0x00,0x00,0x00}, // X
  {0x00,0x00,0xC3,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00}, // Y
  {0x00,0x00,0x7F,0x63,0x43,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00}, // Z
  {0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00}, // [
  {0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00,0x00}, // \ (back slash)
  {0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00}, // ]
  {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ^
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00}, // _
  {0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ` 0x60
  {0x00,0x00,0x00,0x00,0x00,0x3C,0x46,0x06,0x3E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00}, // a
  {0x00,0x00,0x70,0x30,0x30,0x3C,0x36,0x33,0x33,0x33,0x33,0x6E,0x00,0x00,0x00,0x00}, // b
  {0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x60,0x60,0x60,0x63,0x3E,0x00,0x00,0x00,0x00}, // c
  {0x00,0x00,0x0E,0x06,0x06,0x1E,0x36,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00}, // d
  {0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x7E,0x60,0x63,0x3E,0x00,0x00,0x00,0x00}, // e
  {0x00,0x00,0x1C,0x36,0x32,0x30,0x7C,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00}, // f
  {0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00,0x00}, // g
  {0x00,0x00,0x70,0x30,0x30,0x36,0x3B,0x33,0x33,0x33,0x33,0x73,0x00,0x00,0x00,0x00}, // h
  {0x00,0x00,0x0C,0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00}, // i
  {0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00}, // j
  {0x00,0x00,0x70,0x30,0x30,0x33,0x33,0x36,0x3C,0x36,0x33,0x73,0x00,0x00,0x00,0x00}, // k
  {0x00,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00}, // l
  {0x00,0x00,0x00,0x00,0x00,0x6E,0x7F,0x6B,0x6B,0x6B,0x6B,0x6B,0x00,0x00,0x00,0x00}, // m
  {0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x00,0x00}, // n
  {0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00}, // o
  {0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x3E,0x30,0x30,0x78,0x00,0x00}, // p 0x70
  {0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00,0x00}, // q
  {0x00,0x00,0x00,0x00,0x00,0x6E,0x3B,0x33,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00}, // r
  {0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x38,0x0E,0x03,0x63,0x3E,0x00,0x00,0x00,0x00}, // s
  {0x00,0x00,0x08,0x18,0x18,0x7E,0x18,0x18,0x18,0x18,0x1B,0x0E,0x00,0x00,0x00,0x00}, // t
  {0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00}, // u
  {0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x36,0x36,0x1C,0x1C,0x08,0x00,0x00,0x00,0x00}, // v
  {0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x00,0x00,0x00,0x00}, // w
  {0x00,0x00,0x00,0x00,0x00,0x63,0x36,0x1C,0x1C,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, // x
  {0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00,0x00}, // y
  {0x00,0x00,0x00,0x00,0x00,0x7F,0x66,0x0C,0x18,0x30,0x63,0x7F,0x00,0x00,0x00,0x00}, // z
  {0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00}, // {
  {0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00}, // |
  {0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00}, // }
  {0x00,0x00,0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ~
  {0x00,0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
}; // DEL

/*! \brief Local storage of TFT driver data.
 */
static struct {
  unsigned long cpu_hz;
  unsigned long hsb_hz;
} tft_data =
{
  .cpu_hz = 0,
  .hsb_hz = 0
};

/*============================ PRIVATE PROTOTYPES ============================*/

static __inline__ void et024006_SelectRegister( uint8_t address );
static __inline__ void et024006_WriteRegister( uint8_t address, uint8_t value );
static __inline__ uint8_t et024006_ReadRegister( uint8_t address );
static void et024006_SetRegister( uint8_t address, uint8_t bitmask );

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
static void et024006_InitSPI( void );
#endif

static void et024006_SetupInterface( void );
static void et024006_ResetDisplay( void );

static void et024006_AdjustGamma( void );
static void et024006_AdjustSleepingGamma( void );
static void et024006_GeneralSettings( void );
static void et024006_PowerSettings(void);
static void et024006_InterfaceSettings(void);
static void et024006_PowerUp(void);
static void et024006_PowerOn( void );

/*============================ MACROS ========================================*/

#define et024006_SelectSPI()    (PORTC.OUTCLR = PIN4_bm)
#define et024006_DeselectSPI()  (PORTC.OUTSET = PIN4_bm)

#define et024006_SendSPI(value) \
    SPIC.DATA = (value); \
    do {} while ((SPIC.STATUS & SPI_IF_bm) == 0x00);

#define et024006_SendDummySPI() \
  et024006_SendSPI( 0xff );

#define et024006_ReadSPI() \
  (SPIC.DATA)



/*============================ IMPLEMENTATION (PUBLIC FUNCTIONS) =============*/

/*! \brief Initialize the TFT display.
 *  @param cpu_hz CPU speed in Hz. This is needed for power up timings.
 *  @param hsb_hz HSB bus speed in Hz. This parameter is needed to set up the SMC.
 *  If SPI mode is used then this parameter is ignored.
 */
void et024006_Init( unsigned long cpu_hz, unsigned long hsb_hz )
{
  tft_data.cpu_hz = cpu_hz;
  tft_data.hsb_hz = hsb_hz;

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_InitSPI();
#endif
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  smc_init(tft_data.hsb_hz);
#endif
  et024006_SetupInterface();
  et024006_ResetDisplay();
  et024006_AdjustGamma();
  et024006_GeneralSettings();
  et024006_InterfaceSettings();
  et024006_PowerSettings();
  et024006_PowerUp();
  et024006_PowerOn();
}
/*! \brief Configures a scroll area on the display.
 *
 *  @param tfa Top fixed area in rows. This is an area that is unaffected
 *  from the scrolling and that starts at the top of the display area.
 *  @param vsa Scroll area in rows. This is the actual scroll area.
 *  @param bfa Bottom fixed area in rows. This is a section at the bottom
 *  of the display area that will not be used for scrolling.
 *
 */
void et024006_ScrollAreaSetup(uint16_t tfa, uint16_t vsa, uint16_t bfa)
{
  et024006_WriteRegister(HIMAX_VS_TFA_HB, tfa >> 8);
  et024006_WriteRegister(HIMAX_VS_TFA_LB, tfa);

  et024006_WriteRegister(HIMAX_VS_VSA_HB, vsa >> 8);
  et024006_WriteRegister(HIMAX_VS_VSA_LB, vsa);

  et024006_WriteRegister(HIMAX_VS_BFA_HB, bfa >> 8);
  et024006_WriteRegister(HIMAX_VS_BFA_LB, bfa);
}

/*! \brief Scroll the scroll area down/up some lines.
 *  @param lines Number of lines the scroll are should be scrolled.
 *  A positive lines value means lines down. A negative value means lines
 *  up.
 */
void et024006_Scroll(int16_t lines)
{
  uint16_t current_vsp;
  uint16_t current_vsa;
  Assert(lines > ET024006_HEIGHT);
  Assert(lines < -ET024006_HEIGHT);

  current_vsp = ((uint16_t) et024006_ReadRegister(HIMAX_VS_VSP_HB)) << 8;
  current_vsp |= et024006_ReadRegister(HIMAX_VS_VSP_LB);

  current_vsa = ((uint16_t) et024006_ReadRegister(HIMAX_VS_VSA_HB)) << 8;
  current_vsa |= et024006_ReadRegister(HIMAX_VS_VSA_LB);

  lines += current_vsp;
  if(lines < 0)
      lines = ET024006_HEIGHT + lines;
  else if ( lines > ET024006_HEIGHT)
      lines -= ET024006_HEIGHT;

  et024006_WriteRegister(HIMAX_VS_VSP_HB,((uint16_t) lines) >> 8);
  et024006_WriteRegister(HIMAX_VS_VSP_LB,(uint16_t) lines);
}

/*! \brief Enables the scrolling feature.
 */
void et024006_ScrollEnable()
{
  et024006_WriteRegister(HIMAX_GATE_SCAN_CTRL, HIMAX_SCROLL_ON);
}

/*! \brief Disables the scrolling.
 */
void et024006_ScrollDisable()
{
  et024006_WriteRegister(HIMAX_GATE_SCAN_CTRL, 0x0);
}

/*! \brief Converts a RGB 8:8:8 color to RGB 5:6:5.
 *  @param r Red color value
 *  @param g Green color value
 *  @param b Blue color value
 */
et024006_color_t et024006_Color( uint8_t r, uint8_t g, uint8_t b )
{
  uint16_t red = r >> 3;
  uint16_t green = g >> 2;
  uint16_t blue = b >> 3;

  red <<= (5 + 6);
  green <<= 5;

  et024006_color_t color = red | green | blue;
 // color = (color >> 8) | (color << 8); // Convert endianness.
  return color;
}


void et024006_DrawPixel( uint16_t x, uint16_t y, et024006_color_t color )
{
  // Sanity check on parameters.
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );

  // Set up draw area and write the two bytes of pixel data.
  et024006_SetLimits( x, y, x, y );
  et024006_SelectRegister( HIMAX_SRAMWRITE );

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );
  et024006_SendSPI( color & 0xff );
  et024006_SendSPI( color >> 8 );
  et024006_DeselectSPI();
#else
  *ET024006_PARAM_ADDR = color;
#endif
}


et024006_color_t et024006_GetPixel( uint16_t x, uint16_t y )
{
  et024006_color_t color;

  // Sanity check on parameters.
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );

  // Set up draw area and read the three bytes of pixel data.
  et024006_SetLimits( x, y, x, y );
  et024006_SelectRegister( HIMAX_SRAMWRITE );

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  color = *ET024006_PARAM_ADDR;
#endif

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS | ET024006_RW );
  et024006_SendDummySPI(); // First return byte is useless.

  et024006_SendDummySPI();
  uint8_t red = et024006_ReadSPI();
  et024006_SendDummySPI();
  uint8_t green = et024006_ReadSPI();
  et024006_SendDummySPI();
  uint8_t blue = et024006_ReadSPI();

  et024006_DeselectSPI();

  // Convert to 16-bit color format.
  color = et024006_Color( red, green, blue );
#endif
  return color;
}


void et024006_DrawHorizLine( uint16_t x, uint16_t y, uint16_t length, et024006_color_t color )
{
  et024006_DrawFilledRect( x, y, length, 1, color );
}


void et024006_DrawVertLine( uint16_t x, uint16_t y, uint16_t length, et024006_color_t color )
{
  et024006_DrawFilledRect( x, y, 1, length, color );
}


void et024006_DrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, et024006_color_t color )
{
  et024006_SetLimits( 0, 0, ET024006_WIDTH - 1, ET024006_HEIGHT - 1 );

  // Sanity check on parameters.
  Assert( x1 < ET024006_WIDTH );
  Assert( y1 < ET024006_HEIGHT );
  Assert( x2 < ET024006_WIDTH );
  Assert( y2 < ET024006_HEIGHT );

  // Compute deltas, ie. "width" and "height" of line, then
  // compute x and y direction, and make deltas positive for later use.
  int16_t xinc = 1; // Start off assuming direction is positive, ie. right.
  int16_t dx = x2 - x1;
  if (dx < 0){
    xinc = -1;
    dx = -dx;
  }
  int16_t yinc = 1; // Start off assuming direction is positive, ie. down.
  int16_t dy = y2 - y1;
  if (dy < 0){
    yinc = -1;
    dy = -dy;
  }

  // Set up current point.
  uint16_t x = x1;
  uint16_t y = y1;
  uint16_t i;
  // A "flat" line (dx>dy) is handled differently from a "steep" line (dx<dy).
  if (dx > dy) {
    // Walk along X, draw pixel, and step Y when required.
    int16_t e = dx >> 1;
    for ( i = 0; i <= dx; ++i) {
      et024006_DrawQuickPixel(x,y,color);
      // Sub-pixel "error" overflowed, so we step Y and reset the "error".
      if (e <= 0){
        e += dx;
        y += yinc;
      }
      // Walk one step along X.
      e -= dy;
      x += xinc;
    }
  } else {
    // Walk along Y, draw pixel, and step X when required.
    int16_t e = dy >> 1;
    for (i = 0; i <= dy; ++i) {
      et024006_DrawQuickPixel(x,y,color);
      // Sub-pixel "error" overflowed, so we step X and reset the "error".
      if (e <= 0){
        e += dy;
        x += xinc;
      }
      // Walk one step along Y.
      e -= dx;
      y += yinc;
    }
  }
}


void et024006_DrawFilledRect( uint16_t x, uint16_t y, uint16_t width, uint16_t height, et024006_color_t color )
{
  // Sanity check on parameters.
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );
  Assert( width > 0 );
  Assert( height > 0 );

  // Compute bottom right point.
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;

  // More sanity check.
  Assert( x2 < ET024006_WIDTH );
  Assert( y2 < ET024006_HEIGHT );

  // Set up draw area and copy pixel color until area is full.
  et024006_SetLimits( x, y, x2, y2 );
  uint32_t count = (uint32_t) width * height;
  et024006_DuplicatePixel( color, count );
}

#define TFT_QUADRANT0 ((1 << 1) | (1 << 0))
#define TFT_QUADRANT1 ((1 << 3) | (1 << 2))
#define TFT_QUADRANT2 ((1 << 5) | (1 << 4))
#define TFT_QUADRANT3 ((1 << 7) | (1 << 6))

/*!
 * This function draws a filled circle, or parts of it.
 * The radius is the distance from the center to the arc itself, which
 * means that the total width or height of a circle will be (radius*2+1).
 * The quadrantMask parameter is a bitmask that decides which quadrants of the
 * circle to draw. Use the QUADRANT, HALF and WHOLE constants from the
 * header file and OR them together if required.
 * Radius equal to zero gives a single pixel.
 *
 * \param  x             X of center.
 * \param  y             Y of center.
 * \param  radius        Circle radius.
 * \param  color         Circle color.
 * \param  quadrantMask  Bitmask deciding which quadrants to draw.
 */
void et024006_DrawFilledCircle(
	uint16_t x,
	uint16_t y,
	uint16_t radius,
	uint16_t color,
	uint8_t quadrantMask )
{
	// Draw only a pixel if radius is zero.
	if (radius == 0) {
		et024006_DrawPixel( x, y, color );
		return;
	}

	// Set up start iterators.
	uint16_t offsetX = 0;
	uint16_t offsetY = radius;
	int16_t error = 3 - 2 * radius;

	// Iterate offsetX from 0 to radius.
	while (offsetX <= offsetY) {
		// Draw vertical lines tracking each quadrant.
		if (quadrantMask & TFT_QUADRANT0) {
			et024006_DrawVertLine( x + offsetY, y - offsetX,
				offsetX + 1, color );
			et024006_DrawVertLine( x + offsetX, y - offsetY,
				offsetY + 1, color );
		}
		if (quadrantMask & TFT_QUADRANT1) {
			et024006_DrawVertLine( x - offsetY, y - offsetX,
				offsetX + 1, color );
			et024006_DrawVertLine( x - offsetX, y - offsetY,
				offsetY + 1, color );
		}
		if (quadrantMask & TFT_QUADRANT2) {
			et024006_DrawVertLine( x - offsetY, y, offsetX + 1,
				color );
			et024006_DrawVertLine( x - offsetX, y, offsetY + 1,
				color );
		}
		if (quadrantMask & TFT_QUADRANT3) {
			et024006_DrawVertLine( x + offsetY, y, offsetX + 1,
				color );
			et024006_DrawVertLine( x + offsetX, y, offsetY + 1,
				color );
		}

		// Update error value and step offsetY when required.
		if (error < 0) {
			error += ((offsetX << 2) + 6);
		} else {
			error += (((offsetX - offsetY) << 2) + 10);
			--offsetY;
		}

		// Next X.
		++offsetX;
	}
}

void et024006_enter_idle(void)
{
  et024006_AdjustSleepingGamma();
}

void et024006_leave_idle(void)
{
  et024006_AdjustGamma();
}

static void et024006_CopyBitsToScreen(
  void *bitmap,
  uint32_t count,
  et024006_color_t foreground_color,
  et024006_color_t background_color )
{
  uint8_t *u8_bitmap = (uint8_t *) bitmap;
  uint8_t bit_mask;
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  while (count >= 8)
  {
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x01))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x02))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x04))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x08))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x10))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x20))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x40))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, 0x80))?foreground_color:background_color;
    u8_bitmap++;
    count -= 8;
  }
  bit_mask = 1;
  while (count)
  {
    *ET024006_PARAM_ADDR = (Tst_bits(*u8_bitmap, bit_mask))?foreground_color:background_color;
    bit_mask <<= 1;
    count--;
  }
#endif
}

void et024006_PutBitmap(
  void *bitmap,
  uint16_t map_width,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height,
  et024006_color_t foreground_color,
  et024006_color_t background_color )
{
  uint8_t *u8_bitmap = (uint8_t *) bitmap;

  // Sanity check on parameters.
  Assert( bitmap != NULL );
  Assert( width <= map_width );
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );
  Assert( width > 0 );
  Assert( height > 0 );

  // Compute bottom right point.
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;

  // More sanity check.
  Assert( x2 < ET024006_WIDTH );
  Assert( y2 < ET024006_HEIGHT );

  // Set up draw area.
  et024006_SetLimits(x, y, x2, y2);

  // we need access to the display SRAM now
  et024006_SelectRegister( HIMAX_SRAMWRITE );

  // In case of no horizontal pixmap clipping, easier handling is possible.
  if (ET024006_BITMAP_WIDTH(map_width) == map_width)
  {
    // Compute pixel count and copy pixels to screen.
    uint32_t count = (uint32_t) width * height;
    et024006_CopyBitsToScreen(u8_bitmap, count, foreground_color, background_color);
  }
  else
  {
    // Copy line by line to screen.
    uint16_t lines_left = height;
    while (lines_left > 0)
    {
      et024006_CopyBitsToScreen(u8_bitmap, width, foreground_color, background_color);
      u8_bitmap += ET024006_BITMAP_WIDTH(map_width);
      --lines_left;
    }
  }
}

void et024006_PutPixmap(
  et024006_color_t const * pixmap,
  uint16_t map_width,
  uint16_t map_x,
  uint16_t map_y,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height )
{
  // Sanity check on parameters.
  Assert( pixmap != NULL );
  Assert( width <= map_width );
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );
  Assert( width > 0 );
  Assert( height > 0 );

  // Compute bottom right point.
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;

  // More sanity check.
  Assert( x2 < ET024006_WIDTH );
  Assert( y2 < ET024006_HEIGHT );

  // Set up draw area.
  et024006_SetLimits( x, y, x2, y2 );

  // Offset into pixmap.
  pixmap += map_x;
  if (map_y > 0) {
    pixmap += (uint32_t) map_y * map_width;
  }
  // we need access to the display SRAM now
  et024006_SelectRegister( HIMAX_SRAMWRITE );

  // In case of no horizontal pixmap clipping, easier handling is possible.
  if ((map_width == width) && (map_x == 0)) {
    // Compute pixel count and copy pixels to screen.
    uint32_t count = (uint32_t) width * height;
    et024006_CopyPixelsToScreen( pixmap, count );
  } else {
    // Copy line by line to screen.
    uint16_t lines_left = height;
    while (lines_left > 0) {
      et024006_CopyPixelsToScreen( pixmap, width );
      pixmap += map_width;
      --lines_left;
    }
  }
}


void et024006_GetPixmap(
  et024006_color_t * pixmap,
  uint16_t map_width,
  uint16_t map_x,
  uint16_t map_y,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height )
{
  // Sanity check on parameters.
  Assert( pixmap != NULL );
  Assert( width <= map_width );
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );
  Assert( width > 0 );
  Assert( height > 0 );

  // Compute bottom right point.
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;

  // More sanity check.
  Assert( x2 < ET024006_WIDTH );
  Assert( y2 < ET024006_HEIGHT );

  // Set up read area.
  et024006_SetLimits( x, y, x2, y2 );

  // Offset into pixmap.
  pixmap += map_x;
  if (map_y > 0) {
    pixmap += (uint32_t) map_y * map_width;
  }

  // In case of no horizontal pixmap clipping, easier handling is possible.
  if ((map_width == width) && (map_x == 0)) {
    // Compute pixel count and copy pixels from screen.
    uint32_t count = (uint32_t) width * height;
    et024006_CopyPixelsFromScreen( pixmap, count );
  } else {
    // Copy line by line from screen.
    uint16_t lines_left = height;
    while (lines_left > 0) {
      et024006_CopyPixelsFromScreen( pixmap, width );
      pixmap += map_width;
      --lines_left;
    }
  }
}
/*! \brief Writes a string to the display.
 * \param lcd_string Pointer to string data
 * \param font_style Pointer to data lookup table (I.E. Font table)
 * \param x X start direction
 * \param y Y start direction
 * \param fcolor Font color RGB565 format
 * \param bcolor Background color RGB565 format use -1 to indicate transparency
 *   Example:       sprintf(thestring,"Hello World");
 *                  et024006_PrintString(thestring,FONT6x8,52,25,White,Black);
 *                  print "Hello World" in white text on a black background
 *
 * Notes: the first line in the font table must contain 3 variables in this order
 *                  Loc[0][0] = font x width
 *                  Loc[0][1] = font y height
 *                  Loc[0][2] = total data bytes per individual font
 *
 *                  No bounds checking
 */
void et024006_PrintString(char *lcd_string, const unsigned char *font_style,
                          uint16_t x,
                          uint16_t y,
                          uint16_t fcolor,
                          int bcolor)
{
  uint16_t  row, col;
  unsigned char mask = 0, xfont, yfont, font_size;
  const unsigned char *data;
  uint16_t saved_x = x;

  // if string is empty there is nothing to do
  if( *lcd_string == '\0')
    return;

  data = font_style;  // point to the start of the font table
  xfont = *data;  // get font x width
  data++;
  yfont = *data;  // get font y length
  data++;
  font_size = *data;  // get data bytes per font

  // If transparent mode
  if(bcolor == -1)
  {
    // set window to display size
    et024006_SetLimits( 0, 0, ET024006_WIDTH - 1, ET024006_HEIGHT - 1 );

    do
    {
      if(*lcd_string =='\n') {
        x = saved_x;
        y += yfont;
        lcd_string++;  // next character in string
        continue;
      } else if(*lcd_string =='\t') {
        x += xfont;
        lcd_string++;  // next character in string
        continue;
      }
      // Checks if the character can be printed
      if (*lcd_string >= 32 && *lcd_string < (32 + 96))
      {
        // point to character data in font table
        data =  (font_style + font_size) +  // header offset
          (font_size * (int)(*lcd_string - 32)); // character select
      }
      // Print default character
      else
        data =  (font_style + font_size) + font_size * 95;

      for (row = y; row < (y + yfont); row++)
      {
        mask = 0x80;
        for (col = x; col < (x + xfont); col++)
        {
          if (*data & mask) // if pixel data then put dot
          {
            et024006_DrawQuickPixel( col, row, fcolor );
          }
          mask >>= 1;
        }
        // Next row data
        data++;
      }
      // move to next character start pixel
      x += xfont;
      lcd_string++;  // next character in string

    }while(*lcd_string !='\0');  // keep spitting chars out until end of string
  }
  else
  {
    do
    {
      if(*lcd_string =='\n') {
        x = saved_x;
        y += yfont;
        lcd_string++;  // next character in string
        continue;
      } else if(*lcd_string =='\t') {
        x += xfont;
        lcd_string++;  // next character in string
        continue;
      }
      // point to character data in font table
      data =  (font_style + font_size) +  // header offset
        (font_size * (int)(*lcd_string - 32)); // character select

      // set a window for the character
      et024006_SetLimits( x, y, x + xfont - 1, y + yfont - 1 );
      et024006_SelectRegister( HIMAX_SRAMWRITE );

      for (row=0; row < yfont;row++)
      {
        mask = 0x80;
        for (col = 0; col < xfont; col++)
        {
          if (*data & mask) // if pixel data then put dot
          {
            *ET024006_PARAM_ADDR = fcolor;
          }
          else  // else use background color
          {
            *ET024006_PARAM_ADDR = bcolor;
          }
          mask >>= 1;
        }

        // Next row data
        data++;
      }
      // move to next character start pixel
      x += xfont;
      lcd_string++;  // next character in string

    }while(*lcd_string !='\0');  // keep spitting chars out until end of string
  }
}
void et024006_PrintConsole(char *lcd_string, uint16_t fcolor, int bcolor)
{
    static U8 current_line = 1;
    static U8 clear_lines = 0;

    if(clear_lines || current_line == ((ET024006_HEIGHT/10)-1))
    {
      et024006_DrawFilledRect(10, (current_line) * 10, ET024006_WIDTH, 10, WHITE);
      clear_lines = 1;
    }

    et024006_PrintString(lcd_string, (const unsigned char*) &FONT6x8,
                         10, 10 * current_line, fcolor, bcolor);

    current_line = current_line % 23;
    current_line++;
}



/*============================ IMPLEMENTATION (PRIVATE FUNCTIONS) ============*/

/* --- Register manipulation functions --- */
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
__always_inline static void et024006_WriteRegister( uint8_t address, uint8_t value )
{
  *ET024006_CMD_ADDR = (uint16_t) address;
  *ET024006_PARAM_ADDR = (uint16_t) value;
}

__always_inline static uint8_t et024006_ReadRegister( uint8_t address )
{
  *ET024006_CMD_ADDR = (uint16_t) address;
  return *ET024006_PARAM_ADDR;
}

__always_inline static void et024006_SelectRegister( uint8_t address )
{
  *ET024006_CMD_ADDR = (uint16_t) address;
}


#elif(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)


__always_inline static uint8_t et024006_ReadRegister( uint8_t address )
{
  et024006_SelectRegister( address );
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS | ET024006_RW );
  et024006_SendDummySPI();
  uint8_t value = et024006_ReadSPI();
  et024006_DeselectSPI();
  return value;
}


static void et024006_SelectRegister( uint8_t value )
{
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 );
  et024006_SendSPI( value );
  et024006_DeselectSPI();
}


static void et024006_WriteRegister( uint8_t address, uint8_t value )
{
  et024006_SelectRegister( address );
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );
  et024006_SendSPI( value );
  et024006_DeselectSPI();
}

#else
#error No TFT interface mode is selected
#endif

/* --- Shortcuts to common register operations --- */

static void et024006_SetRegister( uint8_t address, uint8_t bitmask )
{
  uint8_t value = et024006_ReadRegister( address );
  value |= bitmask;
  et024006_WriteRegister( address, value );
}

// Not used
/*
static void et024006_ClearRegister( uint8_t address, uint8_t bitmask )
{
  uint8_t value = et024006_ReadRegister( address );
  value &= ~bitmask;
  et024006_WriteRegister( address, value );
}
*/

/*! \brief Sets the display limits according to the corner coordinates.
 *  Writing to the display will result in writing to the area specified through
 *  this function.
 */
void et024006_SetLimits( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 )
{
  et024006_WriteRegister( HIMAX_COL_ADDR_START2, (x1 >> 8) );
  et024006_WriteRegister( HIMAX_COL_ADDR_START1, (x1 & 0xff) );
  et024006_WriteRegister( HIMAX_COL_ADDR_END2, (x2 >> 8) );
  et024006_WriteRegister( HIMAX_COL_ADDR_END1, (x2 & 0xff) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_START2, (y1 >> 8) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_START1, (y1 & 0xff) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_END2, (y2 >> 8) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_END1, (y2 & 0xff) );
}

/*! \brief Set the starting point of the next read/write from/to RAM.
 *  This sets only the start point of the RAM window.
 */
void et024006_SetQuickLimits( uint16_t x, uint16_t y )
{
  et024006_WriteRegister( HIMAX_COL_ADDR_START2, (x >> 8) );
  et024006_WriteRegister( HIMAX_COL_ADDR_START1, (x & 0xff) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_START2, (y >> 8) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_START1, (y & 0xff) );
}

/*! \brief Set the ending point of the next read/write from/to RAM.
 *  This sets only the end point of the RAM window.
 */
void et024006_SetQuickLimits2( uint16_t x, uint16_t y )
{
  et024006_WriteRegister( HIMAX_COL_ADDR_END2, (x >> 8) );
  et024006_WriteRegister( HIMAX_COL_ADDR_END1, (x & 0xff) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_END2, (y >> 8) );
  et024006_WriteRegister( HIMAX_ROW_ADDR_END1, (y & 0xff) );
}


void et024006_DrawQuickPixel( uint16_t x, uint16_t y, et024006_color_t color )
{
  // Sanity check on parameters.
  Assert( x < ET024006_WIDTH );
  Assert( y < ET024006_HEIGHT );

  // Set up draw area and write the two bytes of pixel data.
  et024006_SetQuickLimits( x, y );
  et024006_SelectRegister( HIMAX_SRAMWRITE );
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );
  et024006_SendSPI( color & 0xff );
  et024006_SendSPI( color >> 8 );
  et024006_DeselectSPI();
#endif
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  *ET024006_PARAM_ADDR = color;
#endif

}



/* --- Pixel block operations --- */

void et024006_DuplicatePixel( et024006_color_t color, uint32_t count )
{
  Assert( (count >> 24) == 0 );
  Assert( count > 0 );

  et024006_SelectRegister( HIMAX_SRAMWRITE );
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  //uint8_t countLowByte = count;
  while (count >= 8) {
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    *ET024006_PARAM_ADDR = color;
    count-=8;
  }
  while (count > 0) {
    *ET024006_PARAM_ADDR = color;
    --count;
  }
#endif
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );

  if (count < ET024006_DMA_USAGE_LIMIT) {
    Assert( ET024006_DMA_USAGE_LIMIT < 256 );

    uint8_t countLowByte = count & 0xff;
    while (countLowByte > 0) {
      et024006_SendSPI( color & 0xff );
      et024006_SendSPI( color >> 8 );
      --countLowByte;
    }
  } else {
    DMA.CTRL = DMA_ENABLE_bm;

    DMA.CH0.DESTADDR0 = (((uint32_t) &(SPIC.DATA)) >>  0) & 0xff;
    DMA.CH0.DESTADDR1 = (((uint32_t) &(SPIC.DATA)) >>  8) & 0xff;
    DMA.CH0.DESTADDR2 = (((uint32_t) &(SPIC.DATA)) >> 16) & 0xff;

    DMA.CH0.TRFCNT = 2; // Two bytes in a pixel.

    DMA.CH0.ADDRCTRL = (uint8_t)
      DMA_CH_SRCRELOAD_BLOCK_gc |
      DMA_CH_SRCDIR_INC_gc |
      DMA_CH_DESTRELOAD_NONE_gc |
      DMA_CH_DESTDIR_FIXED_gc;
    DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_SPIC_gc;

    ++count; // DMA issue workaround!!!

    while (count >= 255) {
      DMA.CH0.SRCADDR0 = (((uint32_t) &color) >>  0) & 0xff;
      DMA.CH0.SRCADDR1 = (((uint32_t) &color) >>  8) & 0xff;
      DMA.CH0.SRCADDR2 = (((uint32_t) &color) >> 16) & 0xff;

      DMA.CH0.REPCNT = 255;
      DMA.CH0.CTRLA = (uint8_t)
        DMA_CH_ENABLE_bm |
        DMA_CH_REPEAT_bm |
        DMA_CH_TRFREQ_bm |
        DMA_CH_SINGLE_bm |
        DMA_CH_BURSTLEN_1BYTE_gc;
      do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);

      count -= 255;
    }

    if ((count & 0xff) > 0) {
      DMA.CH0.SRCADDR0 = (((uint32_t) &color) >>  0) & 0xff;
      DMA.CH0.SRCADDR1 = (((uint32_t) &color) >>  8) & 0xff;
      DMA.CH0.SRCADDR2 = (((uint32_t) &color) >> 16) & 0xff;

      DMA.CH0.REPCNT = count & 0xff;
      DMA.CH0.CTRLA = (uint8_t)
        DMA_CH_ENABLE_bm |
        DMA_CH_REPEAT_bm |
        DMA_CH_TRFREQ_bm |
        DMA_CH_SINGLE_bm |
        DMA_CH_BURSTLEN_1BYTE_gc;
      do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
    }
  }

  et024006_DeselectSPI();
#endif
}

void et024006_DrawBitmap( const uint16_t data[], U16 columnOffset, U16 rowOffset )
{
  int col, row;
  U16 columns = data[1];
  U16 rows  = data[0];
  et024006_SetLimits( 0 + rowOffset,  0 + columnOffset,  rows - 1 + rowOffset, columns - 1 + columnOffset );
  data += 2;
  et024006_SelectRegister( HIMAX_SRAMWRITE );
  U16 *pData = (U16 *)(&data[2]);
  for (col=columns;col>0;col--)
  {
    pData = (U16 *)(&data[2+col]);
    for (row=0;row<rows;row++)
    {
      *ET024006_PARAM_ADDR = *pData;
      pData = pData+columns;
    }
  }
}


void et024006_CopyPixelsToScreen( et024006_color_t const * pixels, uint32_t count )
{
  Assert( pixels != NULL );
  Assert( count > 0 );

  et024006_SelectRegister( HIMAX_SRAMWRITE );

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  while (count >= 8)
  {
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    *ET024006_PARAM_ADDR = *pixels++;
    count-=8;
  }
  while( count )
  {
    *ET024006_PARAM_ADDR = *pixels;
    pixels++;
    count--;
  }
#endif

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );

/*  if (count < ET024006_DMA_USAGE_LIMIT) {
    Assert( ET024006_DMA_USAGE_LIMIT < 256 );

    uint8_t countLowByte = count & 0xff;
    while (countLowByte > 0) {
      et024006_SendSPI( color & 0xff );
      et024006_SendSPI( color >> 8 );
      --countLowByte;
    }
  } else*/
  {
    DMA.CTRL = DMA_ENABLE_bm;

    DMA.CH0.DESTADDR0 = (((uint32_t) &(SPIC.DATA)) >>  0) & 0xff;
    DMA.CH0.DESTADDR1 = (((uint32_t) &(SPIC.DATA)) >>  8) & 0xff;
    DMA.CH0.DESTADDR2 = (((uint32_t) &(SPIC.DATA)) >> 16) & 0xff;

    DMA.CH0.SRCADDR0 = (((uint32_t) pixels) >>  0) & 0xff;
    DMA.CH0.SRCADDR1 = (((uint32_t) pixels) >>  8) & 0xff;
    DMA.CH0.SRCADDR2 = (((uint32_t) pixels) >> 16) & 0xff;

    DMA.CH0.ADDRCTRL = (uint8_t)
      DMA_CH_SRCRELOAD_NONE_gc |
      DMA_CH_SRCDIR_INC_gc |
      DMA_CH_DESTRELOAD_NONE_gc |
      DMA_CH_DESTDIR_FIXED_gc;
    DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_SPIC_gc;


    uint32_t byteCount = count * sizeof(et024006_color_t);
    ++byteCount; // DMA issue workaround!!!
    Assert( (byteCount >> 24) == 0x00 ); // Byte count should be 24 bit or less.
    uint8_t blockCount = byteCount >> 16;
    uint16_t remainderCount = byteCount & 0xffff;

    if (blockCount > 0) {
      DMA.CH0.TRFCNT = 0; // Equals 65536.
      DMA.CH0.REPCNT = blockCount;
      DMA.CH0.CTRLA = (uint8_t)
        DMA_CH_ENABLE_bm |
        DMA_CH_REPEAT_bm |
        DMA_CH_TRFREQ_bm |
        DMA_CH_SINGLE_bm |
        DMA_CH_BURSTLEN_1BYTE_gc;
      do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
    }

    if (remainderCount > 0) {
      DMA.CH0.TRFCNT = remainderCount;
      DMA.CH0.CTRLA = (uint8_t)
        DMA_CH_ENABLE_bm |
        DMA_CH_TRFREQ_bm |
        DMA_CH_SINGLE_bm |
        DMA_CH_BURSTLEN_1BYTE_gc;
      do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
    }
  }

  et024006_DeselectSPI();


#if 0
  Assert( pixels != NULL );
  Assert( (count >> 24) == 0 )
  Assert( count > 0 );

  count *= sizeof( et024006_color_t );
  uint8_t const * buffer = (uint8_t const *) pixels;

  uint8_t countHigh = (count >> 16);
  uint16_t countLow = count & 0xffff;

  et024006_SelectRegister( HIMAX_SRAMWRITE );
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS );

  if (countLow > 0) {
    do {
      et024006_SendSPI( *buffer );
      ++buffer;
      --countLow;
    } while (countLow > 0);
  }

  if (countHigh > 0) {
    do {
      do {
        et024006_SendSPI( *buffer );
        ++buffer;
        --countLow;
      } while (countLow > 0);
      --countHigh;
    } while (countHigh > 0);
  }

  et024006_DeselectSPI();
#endif

#endif
}


void et024006_CopyPixelsFromScreen( et024006_color_t * pixels, uint32_t count )
{
  Assert( pixels != NULL );

  et024006_SelectRegister( HIMAX_SRAMWRITE );

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  while( count)
  {
    *pixels = *ET024006_PARAM_ADDR;
    pixels++;
    count--;
  }
#endif

#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
  et024006_SelectSPI();
  et024006_SendSPI( ET024006_ID | ET024006_BS0 | ET024006_RS | ET024006_RW );
  et024006_SendDummySPI();

  while (count > 0) {
    et024006_SendDummySPI();
    uint8_t red = et024006_ReadSPI();
    et024006_SendDummySPI();
    uint8_t green = et024006_ReadSPI();
    et024006_SendDummySPI();
    uint8_t blue = et024006_ReadSPI();

    *pixels = et024006_Color( red, green, blue );

    ++pixels;
    --count;
  }

  et024006_DeselectSPI();
#endif
}


/* --- Interface and setup functions --- */
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_SPI)
static void et024006_InitSPI( void )
{
  //Init SS pin to output
  PORTC.DIRSET = PIN4_bm;
  // Init MOSI pin to output
  PORTC.DIRSET = PIN5_bm;
  //Init MISO pin to input
  PORTC.DIRCLR = PIN6_bm;
  // Init SCK pin to output
  PORTC.DIRSET = PIN7_bm;

  // Set SS output to high. (No slave addressed).
  PORTC.OUTSET = PIN4_bm;

  SPIC.INTCTRL = SPI_INTLVL_OFF_gc;

  SPIC.CTRL = ( SPI_ENABLE_bm |          // Turn SPI ON
                SPI_MODE_3_gc |          // SPI mode 3
                SPI_PRESCALER_DIV4_gc |  // Not quite fastest prescale mode
                SPI_MASTER_bm);          // in master mode
}
#endif

static void et024006_SetupInterface( void )
{

  // et024006_TE (tearing sync) signal from display is input
  // without any pull resistors
  gpio_enable_gpio_pin(ET024006DHU_TE_PIN);

  // Backlight pin (PWM) for display is output
  gpio_enable_module_pin(ET024006DHU_BL_PIN, ET024006DHU_BL_FUNCTION);
  // Turns backlight ON
  /*TODO Add backlight driver */

  // Reset pin for display is output
  gpio_set_gpio_pin(ET024006DHU_RESET_PIN);
}

/*! \brief Does a hard reset of the display.
 */
static void et024006_ResetDisplay( void )
{
  // clear reset line
  gpio_clr_gpio_pin(ET024006DHU_RESET_PIN);
  // 50us delay
  cpu_delay_us( 50, tft_data.cpu_hz );

  gpio_set_gpio_pin(ET024006DHU_RESET_PIN);
  // 5ms delay
  cpu_delay_ms( 5, tft_data.cpu_hz );
}

/*! \brief Tests if a valid and functional display is connected.
 *   This function currently works only for the 16-bit color space configuration.
 *   For other configurations the read values of the display ram are different
 * due to the internal handling.
 *
 * Test for 16-bit 65k colors:
 *
 * Test value 16-bit 0xAAAA -> R=0b10101, G=0b010101, B=0b01010
 * Internal values are 18-bit -> R=0b101011, G=0b010101, B=0b010100
 * Upon read 8-bit values are returned R=0xAC, G=0x54, B=0x50
 *
 * Test value 16-bit 0x5555 -> R=0b01010, G=0b101010, B=0b10101
 * Internal values are 18-bit -> R=0b010100, G=0b101010, B=0b101011
 * Upon read 8-bit values are returned R=0x50, G=0xA8, B=0xAC
 *
 * \returns 0 if test was successful
 * \returns -1 if one of the test pattern that was written to the internal
 * RAM of the display is not read back correctly. This test only applies if the
 * display is connected to the EBI.
 * \returns -2 if the display id was not read as expected.
 */
int et024006_TestDisplay(void)
{
  U16 tmp;
  int ret_value = 0;

#if 0
  U16 display_id;
  // test if this is a valid display
  display_id = et024006_ReadRegister(HIMAX_ID_CODE); // 0x47
  if((display_id & 0x00FF) != 0x0047)
  {
    ret_value = -2;
  }
#endif

#if( ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)

  // If the display is connected to the EBI we test if all
  // data lines are connected by writing a test pattern to
  // the display.
  //else
  {
    // write test pattern at first position in display RAM
    et024006_SetQuickLimits( 0, 0 );
    et024006_SelectRegister( HIMAX_SRAMWRITE );
    *ET024006_PARAM_ADDR = 0xAAAA;

    // set to read access at the first RAM position
    et024006_SetQuickLimits( 0, 0 );
    et024006_SelectRegister( HIMAX_SRAMWRITE );

    // dummy read as described in the datasheet
    tmp = *ET024006_PARAM_ADDR;

    // read 8-bit R value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x00AC)
    {
      ret_value = -1;
    }
    // read 8-bit G value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x0054)
    {
      ret_value = -1;
    }
    // read 8-bit B value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x0050)
    {
      ret_value = -1;
    }

    // write next test pattern to display RAM
    et024006_SetQuickLimits( 0, 0 );
    et024006_SelectRegister( HIMAX_SRAMWRITE );
    *ET024006_PARAM_ADDR = 0x5555;

    // set access to first RAM location
    et024006_SetQuickLimits( 0, 0 );
    et024006_SelectRegister( HIMAX_SRAMWRITE );

    // we need to do a dummy read as described in the datasheet
    tmp = *ET024006_PARAM_ADDR;
    // read 8-bit R value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x0050)
    {
      ret_value = -1;
    }
    // read 8-bit G value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x00A8)
    {
      ret_value = -1;
    }
    // read 8-bit B value and compare with expected result
    tmp = *ET024006_PARAM_ADDR;
    if(tmp != 0x00AC)
    {
      ret_value = -1;
    }

  }
#endif
  return ret_value;
}


/*! \brief Display gamma setup.
 *
 * Parameters are provided by EDT for the display.
 */
static void et024006_AdjustGamma( void )
{
  et024006_WriteRegister( HIMAX_GAMMACTRL1, 0x94 );
  et024006_WriteRegister( HIMAX_GAMMACTRL2, 0x41 );
  et024006_WriteRegister( HIMAX_GAMMACTRL3, 0x00 );
  et024006_WriteRegister( HIMAX_GAMMACTRL4, 0x33 );
  et024006_WriteRegister( HIMAX_GAMMACTRL5, 0x25 );
  et024006_WriteRegister( HIMAX_GAMMACTRL6, 0x45 );
  et024006_WriteRegister( HIMAX_GAMMACTRL7, 0x44 );
  et024006_WriteRegister( HIMAX_GAMMACTRL8, 0x77 );
  et024006_WriteRegister( HIMAX_GAMMACTRL9, 0x12 );
  et024006_WriteRegister( HIMAX_GAMMACTRL10, 0xCC );
  et024006_WriteRegister( HIMAX_GAMMACTRL11, 0x46 );
  et024006_WriteRegister( HIMAX_GAMMACTRL12, 0x82 );
}

static void et024006_AdjustSleepingGamma( void )
{
  et024006_WriteRegister( HIMAX_GAMMACTRL1, 0xf0 );
  et024006_WriteRegister( HIMAX_GAMMACTRL2, 0x0f );
  et024006_WriteRegister( HIMAX_GAMMACTRL6, 0xff );
  et024006_WriteRegister( HIMAX_GAMMACTRL7, 0xff );
  et024006_WriteRegister( HIMAX_GAMMACTRL10, 0xff );
}


/*! \brief General display settings.
 *
 * This are settings provided by EDT and are optimized for the display.
 */
static void et024006_GeneralSettings(void)
{
  et024006_WriteRegister( HIMAX_DISPMODECTRL, (1 << HIMAX_INVON) |
  	(1 << HIMAX_NORON));

  et024006_WriteRegister( HIMAX_CYCLECTRL1, 0x01 );
  et024006_WriteRegister( HIMAX_CYCLECTRL2, 0x01 );
  et024006_WriteRegister( HIMAX_CYCLECTRL3, 0xF0 );
  et024006_WriteRegister( HIMAX_CYCLECTRL4, 0x00 );
  et024006_WriteRegister( HIMAX_CYCLECTRL5, 0x38 );
  et024006_WriteRegister( HIMAX_CYCLECTRL6, 0x0F );
  et024006_WriteRegister( HIMAX_CYCLECTRL14, 0xF0 );


  et024006_WriteRegister( HIMAX_DISPCTRL2, 0x02 );
  et024006_WriteRegister( HIMAX_DISPCTRL3, 0x02 );
  et024006_WriteRegister( HIMAX_DISPCTRL4, 0x02 );
  et024006_WriteRegister( HIMAX_DISPCTRL5, 0x02 );
  et024006_WriteRegister( HIMAX_DISPCTRL6, 0x02 );
  et024006_WriteRegister( HIMAX_DISPCTRL7, 0x02 );

  et024006_WriteRegister( HIMAX_OSCCTRL1, 0x49 );
  et024006_WriteRegister( HIMAX_OSCCTRL3, 0x08 );
}

/*! \brief Power setup.
 *
 * Sets basic DC/DC and VCOM parameters.
 */
static void et024006_PowerSettings(void)
{
  et024006_WriteRegister( HIMAX_CYCLECTRL1_x, 0x95 );
  et024006_WriteRegister( HIMAX_CYCLECTRL2_x, 0x95 );
  et024006_WriteRegister( HIMAX_CYCLECTRL3_x, 0xFF );
  et024006_WriteRegister( HIMAX_DISPCTRL8, 0x7F );
  et024006_WriteRegister( HIMAX_DISPCTRL9, 0x38 );
  et024006_WriteRegister( HIMAX_DISPCTRL10, 0x78 );

  et024006_WriteRegister( HIMAX_POWERCTRL3, 0x07 );
  et024006_WriteRegister( HIMAX_POWERCTRL4, 0x00 );
  et024006_WriteRegister( HIMAX_POWERCTRL5, 0x04 );
  et024006_WriteRegister( HIMAX_POWERCTRL6, 0x40 );
  et024006_WriteRegister( HIMAX_VCOMCTRL2, 0x38 );
  et024006_WriteRegister( HIMAX_VCOMCTRL3, 0x12 );
}

/*! \brief Sets display interface.
 *
 * Configures screen orientation and color mode.
 */
static void et024006_InterfaceSettings(void)
{
  // Scanning order from frame memory to display
  // RGB -> BGR swap
  et024006_WriteRegister( HIMAX_MEMACCESSCTRL,
    (1 << HIMAX_MX)
    | (1 << HIMAX_MV)
    | (1 << HIMAX_BGR) );

}

/*! \brief Power up display.
 *
 * Turns on DC/DC converters.
 */
static void et024006_PowerUp(void)
{
  cpu_delay_ms( 10, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_POWERCTRL2, 0x04 ); // AP = 100

  cpu_delay_ms( 20, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_VCOMCTRL1, (1 << HIMAX_VCOMG) );

  cpu_delay_ms( 5, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_POWERCTRL1, (1 << HIMAX_DK) );

  cpu_delay_ms( 40, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_POWERCTRL1, (1 << HIMAX_PON) );

  // In case the et024006_DisplayOn function is called right after
  cpu_delay_ms( 40, tft_data.cpu_hz );
}


/*! \brief Turns the display on.
 */
static void et024006_PowerOn( void )
{
  et024006_WriteRegister( HIMAX_DISPCTRL1, (1 << HIMAX_D0) );

  cpu_delay_ms( 40, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_DISPCTRL1, (1 << HIMAX_GON) |
  	(1 << HIMAX_D0) );
  et024006_WriteRegister( HIMAX_DISPCTRL1, (1 << HIMAX_GON) |
  	(1 << HIMAX_D1) | (1 << HIMAX_D0));

  cpu_delay_ms( 40, tft_data.cpu_hz );
  et024006_WriteRegister( HIMAX_DISPCTRL1, (1 << HIMAX_GON)| (1 << HIMAX_DTE) |
  	(1 << HIMAX_D1) | (1 << HIMAX_D0));

  et024006_SetRegister( HIMAX_INTERNAL28, (1 << HIMAX_TEON) );
}
/* EOF */
