/**
 * @file pal_boardtypes.h
 *
 * @brief PAL board types for ATmega128RFA1
 *
 * This header file contains board types based on ATmega128RFA1.
 *
 * $Id: pal_boardtypes.h 24949 2011-01-18 07:21:56Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef PAL_BOARDTYPES_H
#define PAL_BOARDTYPES_H

/* === Includes ============================================================= */

#if defined(VENDOR_BOARDTYPES) && (VENDOR_BOARDTYPES != 0)
#include "vendor_boardtypes.h"
#else   /* Use standard board types as defined below. */

/* === Macros =============================================================== */

/* Atmel internal */
#define EMURFA1                 (0x01)

/* Atmel internal */
#define EVALRFA1                (0x02)

/*
 * Plain Radio Controller board RCB V6.3 with ATmega128RFA1
 */
#define RCB_6_3_PLAIN           (0x03)

/*
 * STK541 USB Adapter board with Radio Controller board RCB V6.3 with ATmega128RFA1
 */
#define RCB_6_3_STK541          (0x04)

/*
 * Radio Controller board RCB V6.3 connected to Key Remote Controller Board
 */
#define RCB_6_3_KEY_RC          (0x05)

/*
 * RCB Breakout Board with Radio Controller board RCB 6.3
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB 6.3
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_6_3_BREAKOUT_BOARD  (0x06)

/*
 * Sensor Terminal board with Radio Controller board RCB 6.3
 * http://www.dresden-elektronik.de/shop/prod75.html
 */
#define RCB_6_3_SENS_TERM_BOARD (0x07)

/*
 * RF4CE Kit RCB 6.3 (Sensor Terminal Board and Plain RCB)
 */
#define RCB_6_3_PLAIN_OR_STB    (0x08)

/*
 * Plain Radio Controller board RCB V6.3 with ATmega128RFA1 (without 32khz oscillator)
 */
#define RCB_6_3_PLAIN_NO_32KHZ_CRYSTAL         (0x09)

/*
 * ATmega128RFA1-EK1 Evaluation Kit
 * http://www.atmel.com/dyn/products/tools_card_mcu.asp?tool_id=4677
 */
#define EK1                     (0x11)

#endif  /* #if defined(VENDOR_BOARDTYPES) && (VENDOR_BOARDTYPES != 0) */

#endif  /* PAL_BOARDTYPES_H */

/* EOF */
