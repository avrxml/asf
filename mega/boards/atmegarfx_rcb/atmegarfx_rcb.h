/**
 * \file
 *
 * \brief ATmegaRFX RCB board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 Xplained Pro board.
 *
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef _ATMEGARFX_RCB_
#define _ATMEGARFX_RCB_
#include "compiler.h"
#include "conf_board.h"
#include "helper.h"


#if AVR8_PART_IS_DEFINED(ATmega128RFA1)
#define MCU_SOC_NAME        "ATMEGA128RFA1"

#ifdef  SENSOR_TERMINAL_BOARD
    #define BOARD_NAME          "RF4CE-EK"
#else    
   #define BOARD_NAME          "ATmega128RFA1 RCB"   
#endif

#elif AVR8_PART_IS_DEFINED(ATmega256RFR2) 
#define MCU_SOC_NAME        "ATMEGA256RFR2"

#ifdef  SENSOR_TERMINAL_BOARD
    #define BOARD_NAME          "ATmega256RFR2-EK"
#else
    #define BOARD_NAME          "ATmega256RFR2 RCB" 
#endif
#endif



#ifdef KEY_RC_BOARD








/* === Externals ============================================================*/


/* === Macros ===============================================================*/

#define NO_OF_LEDS                      (5)

#define KEY_RC_IO_MASK			0XDC
#define KEY_RC_IO_VAL           0XDC
/* Definitions to identify buttons, reference HDMI specification */
#define BUTTON_UNKNOWN            0x00000000
#define BUTTON_F2_RED             0x00000001
#define BUTTON_F3_GREEN           0x00000002
#define BUTTON_F4_YELLOW          0x00000004
#define BUTTON_F1_BLUE            0x00000008
#define BUTTON_SELECT             0x00000010
#define BUTTON_NUMBER_0           0x00000020
#define BUTTON_POWER_OFF_FUNCTION 0x00000040
#define BUTTON_NUMBER_1           0x00000200
#define BUTTON_NUMBER_2           0x00000400
#define BUTTON_NUMBER_3           0x00000800
#define BUTTON_NUMBER_4           0x00001000
#define BUTTON_NUMBER_5           0x00002000
#define BUTTON_NUMBER_6           0x00004000
#define BUTTON_NUMBER_7           0x00008000
#define BUTTON_NUMBER_8           0x00010000
#define BUTTON_NUMBER_9           0x00020000
#define BUTTON_UP                 0x00040000
#define BUTTON_LEFT               0x00080000
#define BUTTON_ENTER              0x00100000
#define BUTTON_RIGHT              0x00200000
#define BUTTON_DOWN               0x00400000
#define BUTTON_LEFT_UP            0x00800000
#define BUTTON_LEFT_DOWN          0x01000000
#define BUTTON_RIGHT_UP           0x02000000
#define BUTTON_RIGHT_DOWN         0x04000000


#define button_id_t             uint32_t
#define NO_OF_BUTTONS           (24)

/* Key RC Button Specific macros */
#define MAX_KEY_SCANS           6
#define NUM_OF_IDENTICAL_KEYS   3
#define INTER_BUTTON_SCAN_DELAY 100
#define WAKEUP_DEBOUNCE_DELAY   5000

/*
 * PINs where buttons are connected
 */
#define BUTTON_IRQ_PIN_1                IOPORT_CREATE_PIN(PORTD, 1)
#define BUTTON_IRQ_PIN_2                IOPORT_CREATE_PIN(PORTD, 2)
#define BUTTON_IRQ_PIN_3                IOPORT_CREATE_PIN(PORTD, 3)
#define BUTTON_PIN_0                    IOPORT_CREATE_PIN(PORTB, 0)
#define BUTTON_PIN_1                    IOPORT_CREATE_PIN(PORTB, 1)
#define BUTTON_PIN_2                    IOPORT_CREATE_PIN(PORTB, 2)
#define BUTTON_PIN_3                    IOPORT_CREATE_PIN(PORTB, 3)
#define BUTTON_PIN_4                    IOPORT_CREATE_PIN(PORTB, 4)
#define BUTTON_PIN_5                    IOPORT_CREATE_PIN(PORTB, 5)
#define BUTTON_PIN_6                    IOPORT_CREATE_PIN(PORTB, 6)
#define BUTTON_PIN_7                    IOPORT_CREATE_PIN(PORTD, 5)
#define BUTTON_PIN_8                    IOPORT_CREATE_PIN(PORTD, 7)
/*
 * PINs where Accelerometer power is connected
 */
#define ACC_PWR                         (PB5)
/*
 * PINs where LEDs are connected
 */
#define LED_1_PIN                       IOPORT_CREATE_PIN(PORTB, 0)
#define LED_2_PIN                       IOPORT_CREATE_PIN(PORTB, 1)
#define LED_3_PIN                       IOPORT_CREATE_PIN(PORTB, 2)
#define LED_4_PIN                       IOPORT_CREATE_PIN(PORTB, 3)
#define LED_5_PIN                       IOPORT_CREATE_PIN(PORTB, 4)

/*
 * PINs where LCDs are connected
 */
#define LCD_CS_ON_BOARD      IOPORT_CREATE_PIN(PORTE, 4)
#define LCD_RST_ON_BOARD     IOPORT_CREATE_PIN(PORTE, 4)

/*
 * PORT where button is connected
 */
#define BUTTON_IRQ_PORT                 (PORTD)
#define BUTTON_IRQ_PORT_DIR             (DDRD)
#define BUTTON_IRQ_PORT_IN              (PIND)
#define BUTTON_PORT1                    (PORTB)
#define BUTTON_PORT1_DIR                (DDRB)
#define BUTTON_PORT1_IN                 (PINB)
#define BUTTON_PORT2                    (PORTD)
#define BUTTON_PORT2_DIR                (DDRD)
#define BUTTON_PORT2_IN                 (PIND)

/*
 * ISR vectors for buttons
 */
#define BUTTON_1_ISR_vect               (INT1_vect)
#define BUTTON_2_ISR_vect               (INT2_vect)
#define BUTTON_3_ISR_vect               (INT3_vect)

/*
 * ISR mask for buttons
 */
#define BUTTON_1_ISR_MASK               (1 << INT1)
#define BUTTON_2_ISR_MASK               (1 << INT2)
#define BUTTON_3_ISR_MASK               (1 << INT3)

/*
 * ISR flag for buttons
 */
#define BUTTON_FLAG_REG                 (EIFR)
#define BUTTON_1_ISR_FLAG               (1 << INTF1)
#define BUTTON_2_ISR_FLAG               (1 << INTF2)
#define BUTTON_3_ISR_FLAG               (1 << INTF3)

/*
 * Button input mask
 */
#define BUTTON_IRQ_1_IN_MASK            (1 << PIND1)
#define BUTTON_IRQ_2_IN_MASK            (1 << PIND2)
#define BUTTON_IRQ_3_IN_MASK            (1 << PIND3)


/* Disable all button interrupts */
#define DISABLE_ALL_BUTTON_IRQS()       EIMSK &= ~(BUTTON_1_ISR_MASK | BUTTON_2_ISR_MASK | BUTTON_3_ISR_MASK)
/* Enable all button interrupts */
#define ENABLE_ALL_BUTTON_IRQS()        EIMSK |= (BUTTON_1_ISR_MASK | BUTTON_2_ISR_MASK | BUTTON_3_ISR_MASK)
/* Clear all button IRQ flags */
#define CLEAR_ALL_BUTTON_IRQ_FLAGS()    BUTTON_FLAG_REG = BUTTON_1_ISR_FLAG | BUTTON_2_ISR_FLAG | BUTTON_3_ISR_FLAG

#define LED0                 0
#define LED1                 1
#define LED2                 2
#define LED3                 3
#define LED4                 4


#define LED_COUNT             5

/*
 * Macros controlling the latch
 */
#define LATCH_PORT                      PORTE
#define LATCH_PORT_DIR                  DDRE
#define LATCH_PIN                       PE5
#define LATCH_HIGH()                    LATCH_PORT |= 1 << LATCH_PIN
#define LATCH_LOW()                     LATCH_PORT &= ~(1 << LATCH_PIN)
#define LATCH_PULSE()                   LATCH_HIGH(); LATCH_LOW()
#define LATCH_DATA                      PORTB
#define LATCH_DATA_DIR                  DDRB
#define LATCH_INIT()                    do { \
        LATCH_PORT &= ~(1 << LATCH_PIN); \
        LATCH_PORT_DIR |= 1 << LATCH_PIN; \
    } while (0)   

#else /* KEY_RC_BOARD */
      
#define BB_SIO_MASK  0XD0
#define BB_SIO_VAL 	 0XC0 
/*
 * PINs where buttons are connected
 */
#define BUTTON_PIN_0                    (PB0)

#define LED0                 LED0_GPIO
#define LED1                 LED1_GPIO
#define LED2                 LED2_GPIO
#define LED0_GPIO			  LED_0  
#define LED1_GPIO			  LED_1 
#define LED2_GPIO			  LED_2 
#define LED_COUNT             3

#define LED0_RCB			  IOPORT_CREATE_PIN(PORTE, 2)	  
#define LED1_RCB		      IOPORT_CREATE_PIN(PORTE, 3)	  
#define LED2_RCB			  IOPORT_CREATE_PIN(PORTE, 4)	  

#define LED_ADDR_DEC_DDR                (DDRD)

/* LED address decoding port output register */
#define LED_ADDR_DEC_PORT               (PORTD)

#define GPIO_PUSH_BUTTON_0			  IOPORT_CREATE_PIN(PORTE, 5)


#define NUM_CHECK 10
#if (defined __ICCAVR__)
#define _BV(x) (1 << (x))
#endif

/*
 * Various helper macros for accessing the memory mapped External RAM on the
 * Sensor Terminal Board
 */
#define XRAM_DATA_SETINP()   do{ XRAM_DATA_DDR = 0x00; XRAM_DATA_PORT = 0x00; }while(0)
#define XRAM_DATA_SETOUTP()  do{ XRAM_DATA_DDR = 0xFF; }while(0)
#define XRAM_CTRL_RD_LO()    do{ XRAM_CTRL_PORT &= ~XRAM_RD; }while(0)
#define XRAM_CTRL_RD_HI()    do{ XRAM_CTRL_PORT |= XRAM_RD; }while(0)
#define XRAM_CTRL_WR_LO()    do{ XRAM_CTRL_PORT &= ~XRAM_WR; }while(0)
#define XRAM_CTRL_WR_HI()    do{ XRAM_CTRL_PORT |= XRAM_WR; }while(0)

/**
 * \name Macros for XRAM access
 * \{
 */

/* XRAM specific port macros */

/* XRAM control port output register */
#define XRAM_CTRL_PORT                   (PORTE)

/* XRAM control port direction register */
#define XRAM_CTRL_DDR                    (DDRE)

/* Port pin which is used to signal a read access from the XRAM */
#define XRAM_RD                          (0x20)  /* PE5 */

/* Port pin which is used to signal a write access to the XRAM */
#define XRAM_WR                          (0x10)  /* PE4 */

/* XRAM data port output register */
#define XRAM_DATA_PORT                   (PORTB)

/* XRAM data port direction register */
#define XRAM_DATA_DDR                    (DDRB)

/* XRAM data port input register */
#define XRAM_DATA_PIN                    (PINB)

/* Port where the latch's ALE signal is connected to */
#define XRAM_ALE_PORT                    (PORTG)

/* Data direction register used to activate the ALE signal */
#define XRAM_ALE_DDR                     (DDRG)

/* Pin where the latch's ALE signal is connected to */
#define XRAM_ALE_PIN                     (0x04)  /* PG2 */


/* Button address decoding port output register */
#define BUTTON_ADDR_DEC_PORT            (PORTD)
#define BUTTON_PORT                     (PORTB)
#define BUTTON_PORT_DIR                 (DDRB)

/* Button address decoding port direction register */
#define BUTTON_ADDR_DEC_DDR             (DDRD)
/* LED address decoding port direction register */

#define BUTTON_INPUT_PINS               (PINB)

#ifdef SENSOR_TERMINAL_BOARD
/**
 * \name FTDI based USB macros
 * \{
 */

/* USB specific port macros */

/* USB control port output register */
#define USB_CTRL_PORT                   (PORTE)

/* USB control port direction register */
#define USB_CTRL_DDR                    (DDRE)

/* USB control port input register */
#define USB_CTRL_PIN                    (PINE)

/* Port pin which gives indication of reception of byte */
#define USB_RXF                         (0x80)  /* PE7*/

/* Port pin which gives indication of transmission of byte */
#define USB_TXE                         (0x40)  /* PE6*/

/* Port pin which is used to signal a read access from the FT245 */
#define USB_RD                          (0x20)  /* PE5 */

/* Port pin which is used to signal a write access to the FT245 */
#define USB_WR                          (0x10)  /* PE4 */

/* USB data port output register */
#define USB_DATA_PORT                   (PORTB)

/* USB data port direction register */
#define USB_DATA_DDR                    (DDRB)

/* USB data port input register */
#define USB_DATA_PIN                    (PINB)

/* Memory address mapped to USB FIFO */
#define USB_FIFO_AD                     (0x2200)

/* Port where the latch's ALE signal is connected to */
#define USB_ALE_PORT                    (PORTG)

/* Data direction register used to activate the ALE signal */
#define USB_ALE_DDR                     (DDRG)

/* Pin where the latch's ALE signal is connected to */
#define USB_ALE_PIN                     (0x04)  /* PG2 */

/* USB address decoding port output register */
#define USB_ADDR_DEC_PORT               (PORTD)

/* USB address decoding port direction register */
#define USB_ADDR_DEC_DDR                (DDRD)

//! \}
/*
 * USB0 non-generic (board specific) initialization part.
 * If this is required, the following macro is filled with the proper
 * implementation.
 */
/* Enable USB address decoding. */
#define USB_INIT_NON_GENERIC()      do { \
        USB_ADDR_DEC_PORT &= ~_BV(6);        \
        USB_ADDR_DEC_DDR |= _BV(6);          \
        USB_ADDR_DEC_PORT &= ~_BV(7);        \
        USB_ADDR_DEC_DDR |= _BV(7);          \
    } while (0)           

#endif     
#endif

#endif  /* _ATMEGARFX_RCB_ */
