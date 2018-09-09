/**
 * \file
 *
 * \brief Board specific configuration file.
 *
 * To use this board, define BOARD=AVR477_TARGET.
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
 */
/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * 
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef PAL_CONFIG_H
#define PAL_CONFIG_H

/* === Includes =============================================================*/

#include "pal_boardtypes.h"

#if (BOARD_TYPE == RCB_6_3_SENS_TERM_BOARD)

/*
 * This header file is required since a function with
 * return type retval_t is declared
 */
#include "return_val.h"

/* === Types ================================================================*/

/* Enumerations used to identify LEDs */
typedef enum led_id_tag
{
    LED_0,
    LED_1
} led_id_t;

#define NO_OF_LEDS                      (2)


/* Enumerations used to identify buttons */
typedef enum button_id_tag
{
    BUTTON_0
} button_id_t;

#define NO_OF_BUTTONS                   (1)

/* === Externals ============================================================*/


/* === Macros ===============================================================*/

/**
 * The default CPU clock
 *
 * The board must be configured for 16 MHz clock, which can be fused
 * for either the 16 MHz internal RC oscillator, or for the 16 MHz
 * crystal oscillator that is required for the transceiver operation.
 * If F_CPU is set to 4 or 8 MHz, the prescaler reduces the CPU clock.
 * One of the following lines need to be enabled.
 */
#define F_CPU                           (16000000UL)
//#define F_CPU                           (8000000UL)
//#define F_CPU                           (4000000UL)

/*
 * Wait for 65 ns.
 * time t7: SLP_TR time (see data sheet or SWPM).
 * In case the system clock is slower than 32 MHz we do not need
 * to have this value filled.
 */
#define PAL_WAIT_65_NS()                // empty


/*
 * PORT where LEDs are connected
 */
#define LED_PORT                        (PORTB)
#define LED_PORT_DIR                    (DDRB)

/* LED address decoding port output register */
#define LED_ADDR_DEC_PORT               (PORTD)

/* LED address decoding port direction register */
#define LED_ADDR_DEC_DDR                (DDRD)

/*
 * Bit numbers address where LEDs are mapped to.
 */
#define LED_BIT_0                       (0)
#define LED_BIT_1                       (1)

/*
 * LED bit mask
 */
#define LED_BIT_MASK                    ((1 << LED_BIT_0) | (1 << LED_BIT_1))

/*
 * PORT where buttons are connected.
 */
#define BUTTON_PORT                     (PORTB)
#define BUTTON_PORT_DIR                 (DDRB)
#define BUTTON_INPUT_PINS               (PINB)

/*
 * PINs where buttons are connected
 */
#define BUTTON_PIN_0                    (PB0)

/* Button address decoding port output register */
#define BUTTON_ADDR_DEC_PORT            (PORTD)

/* Button address decoding port direction register */
#define BUTTON_ADDR_DEC_DDR             (DDRD)


/*
 * Port/Pins where relays are connected.
 */
#define RELAY_PORT                      (PORTE)
#define RELAY_PORT_DIR                  (DDRE)
#define RELAY_PIN_1                     (PE2)
#define RELAY_PIN_2                     (PE3)
#define RELAY_PIN_1_INIT()              do \
                                        {  \
                                            RELAY_PORT |= 1 << RELAY_PIN_1;  /* low active */\
                                            RELAY_PORT_DIR |= 1 << RELAY_PIN_1; \
                                        } while (0)
#define RELAY_PIN_2_INIT()              do \
                                        {  \
                                            RELAY_PORT |= 1 << RELAY_PIN_2;  /* low active */\
                                            RELAY_PORT_DIR |= 1 << RELAY_PIN_2; \
                                        } while (0)
#define RELAY_1_ON()                    RELAY_PORT &= ~(1 << RELAY_PIN_1)  /* low active */
#define RELAY_1_OFF()                   RELAY_PORT |= 1 << RELAY_PIN_1
#define RELAY_2_ON()                    RELAY_PORT &= ~(1 << RELAY_PIN_2)  /* low active */
#define RELAY_2_OFF()                   RELAY_PORT |= 1 << RELAY_PIN_2
#define RELAY_1_TOGGLE()                do                                              \
                                        {                                               \
                                            if (RELAY_PORT & (1 << RELAY_PIN_1))        \
                                            {                                           \
                                                RELAY_PORT &= ~(1 << RELAY_PIN_1);      \
                                            }                                           \
                                            else                                        \
                                            {                                           \
                                                RELAY_PORT |= 1 << RELAY_PIN_1;         \
                                            }                                           \
                                        } while (0)
#define RELAY_2_TOGGLE()                do                                              \
                                        {                                               \
                                            if (RELAY_PORT & (1 << RELAY_PIN_2))        \
                                            {                                           \
                                                RELAY_PORT &= ~(1 << RELAY_PIN_2);      \
                                            }                                           \
                                            else                                        \
                                            {                                           \
                                                RELAY_PORT |= 1 << RELAY_PIN_2;         \
                                            }                                           \
                                        } while (0)

#ifdef USB0

/*
 * FTDI based USB macros
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

#endif  /* ifdef USB0 */


/*
 * UART0 and UART1 non-generic (board specific) initialization part.
 * If this is required, the following macro is filled with the proper
 * implementation.
 * Currently this macro is empty for this board.
 */
#define UART_0_INIT_NON_GENERIC()
#define UART_1_INIT_NON_GENERIC()


/*
 * This board has an external eeprom that stores the IEEE address
 * and other information.
 */
#ifndef EXTERN_EEPROM_AVAILABLE
#define EXTERN_EEPROM_AVAILABLE            (1)
#endif

/*
 * IEEE address of board in EEPROM
 */
#define EE_IEEE_ADDR                    (0)
#define EE_XTAL_TRIM_ADDR               (21)

#if (EXTERN_EEPROM_AVAILABLE == 1)

/* Port where the external EEPROM's chip select is connected to */
#define EXT_EE_CS_PORT          PORTG

/* Data direction register for the external EEPROM's chip select */
#define EXT_EE_CS_DDR           DDRG

/* Pin where the external EEPROM's chip select is connected to */
#define EXT_EE_CS_PIN           (0x20) /* PG5 */

#endif


/*
 * Alert initialization
 */
#define ALERT_INIT()                    (pal_led_init())


/*
 * Alert indication
 */
#define ALERT_INDICATE()                do {    \
        pal_led(LED_0, LED_TOGGLE);             \
        pal_led(LED_1, LED_TOGGLE);             \
} while (0)


/* === Prototypes ===========================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


/*
 * MCU pins used for debugging
 */
#define TST_PIN_0_HIGH()
#define TST_PIN_0_LOW()
#define TST_PIN_1_HIGH()
#define TST_PIN_1_LOW()
#define TST_PIN_2_HIGH()
#define TST_PIN_2_LOW()
#define TST_PIN_3_HIGH()
#define TST_PIN_3_LOW()
#define TST_PIN_4_HIGH()
#define TST_PIN_4_LOW()
#define TST_PIN_5_HIGH()
#define TST_PIN_5_LOW()
#define TST_PIN_6_HIGH()
#define TST_PIN_6_LOW()
#define TST_PIN_7_HIGH()
#define TST_PIN_7_LOW()
#define TST_PIN_8_HIGH()
#define TST_PIN_8_LOW()

#endif /* RCB_6_3_SENS_TERM_BOARD */

#endif  /* PAL_CONFIG_H */
/* EOF */
