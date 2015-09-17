/**
 * @file pal_types.h
 *
 * @brief Definition of supported PAL types
 *
 * This header file contains the supported PAL types.
 *
 * $Id: pal_types.h 26463 2011-04-29 07:05:35Z yogesh.bellan $
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
#ifndef PAL_TYPES_H
#define PAL_TYPES_H

/* PAL_GENERIC_TYPE */
#define AVR_                             (0x01)
#define MEGA_RF_                         (0x02)
#define XMEGA_                           (0x03)
#define AVR32_                           (0x04)
#define ARM7_                            (0x05)
#define SAM3_                            (0x06)

#if (PAL_GENERIC_TYPE == AVR_)
    /* PAL_TYPE for AVR 8-bit MCUs */
    #define AT90USB1287                 (0x01)
    #define ATMEGA1281                  (0x02)
    #define ATMEGA1284P                 (0x03)
    #define ATMEGA2561                  (0x04)
    #define ATMEGA644P                  (0x05)

#elif (PAL_GENERIC_TYPE == XMEGA_)
    /* PAL_TYPE for XMEGA MCUs */
    #define ATXMEGA128A1                (0x01)
    #define ATXMEGA128A3                (0x02)
    #define ATXMEGA256A3                (0x03)
    #define ATXMEGA256D3                (0x04)

#elif (PAL_GENERIC_TYPE == AVR32_)
    /* PAL_TYPE for AVR 32-bit MCUs */
      #define AT32UC3A3256               (0x01)
      #define AT32UC3L064                (0x02)
      #define AT32UC3A0512               (0x03)
      #define AT32UC3B1128               (0x04)
      #define AT32UC3B164                (0x05)
#elif (PAL_GENERIC_TYPE == ARM7_)
    /* PAL_TYPE for ARM7 MCUs */
    #define AT91SAM7S256                (0x01)
    /* AT91SAM7X256 and AT91SAM7CX256:
     *
     * AT91SAM7CX256 is the same as AT91SAM7X256 except hardware AES engine.
     * If this boardtype is used, the file are actually taken from the
     * directory PAL/ARM7/AT91SAM7X256.
     * Only in case security shall be used, separate security files are selected
     * based on this PAL type.
     */
    #define AT91SAM7X256                (0x02)
    #define AT91SAM7XC256               (0x03)

#elif (PAL_GENERIC_TYPE == SAM3_)

    #define AT91SAM3S4B                 (0x01)
    #define AT91SAM3S4C                 (0x02)


#elif (PAL_GENERIC_TYPE == MEGA_RF_)
    /* PAL_TYPE for MEGA-RF single chips (MCU plus transceiver) */
    #define ATMEGA64RFA1                (0x01)
    #define ATMEGA128RFA1               (0x02)
    #define ATMEGA256RFA1               (0x03)
    #define ATMEGA64RFR1                (0x04)
    #define ATMEGA128RFR1               (0x05)
    #define ATMEGA256RFR1               (0x06)
    #if (PAL_TYPE == ATMEGA128RFA1)
        #define __ATMEGA128RFA1__       (ATMEGA128RFA1)
    #endif
#else
    #error "Undefined PAL_GENERIC_TYPE"
#endif

/* Depending on the generic device type include platform-dependend types (IAR, GCC) */
#if ((PAL_GENERIC_TYPE == AVR_) || (PAL_GENERIC_TYPE == XMEGA_) || (PAL_GENERIC_TYPE == MEGA_RF_))
#include "avrtypes.h"
#elif (PAL_GENERIC_TYPE == ARM7_)
#include "armtypes.h"
#elif (PAL_GENERIC_TYPE == SAM3_)
#include "core_cm3.h"
#include "armtypes.h"
#elif (PAL_GENERIC_TYPE == AVR32_)
#include "avr32types.h"
#else
#error "Unknown PAL_GENERIC_TYPE"
#endif
#endif  /* PAL_TYPES_H */
/* EOF */
