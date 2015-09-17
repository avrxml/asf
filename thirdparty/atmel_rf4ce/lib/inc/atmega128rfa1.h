/**
 * @file atmega128rfa1.h
 *
 * @brief File contains register and command defines specific for AT86RF231.
 *
 * $Id: atmega128rfa1.h 27079 2011-06-08 05:44:08Z pawan.jadia $
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
#ifndef ATMEGA128RFA1_H
#define ATMEGA128RFA1_H

/* === INCLUDES ============================================================ */
/* Utilization of regular TAL */
#include "tal_types.h"
#include "avrtypes.h"

#if (TAL_TYPE == ATMEGARF_TAL_1)

/* === EXTERNALS =========================================================== */


/* === TYPES =============================================================== */


/* === MACROS ============================================================== */

#define TRX_SUPPORTED_CHANNELS          (0x07FFF800)

/**
 * Tolerance of the phyTransmitPower PIB attribute.  This is encoded
 * into the two MSBits of the attribute, and is effectively read-only.
 */
#define TX_PWR_TOLERANCE                (0x80)

/*
 * Typical timing values
 */
#define RST_PULSE_WIDTH_US                  (1)
#define P_ON_TO_CLKM_AVAILABLE_TYP_US       (380)
#define SLEEP_TO_TRX_OFF_TYP_US             (240) /* TRX Parameter: tTR2 typical value */
#define SLEEP_TO_TRX_OFF_MAX_US             (1000) /* TRX Parameter: tTR2 max. value */
#define TRX_OFF_TO_SLEEP_TIME_CLKM_CYCLES   (35)  /* TRX Parameter: tTR3 */
#define TRX_OFF_TO_PLL_ON_TIME_US           (110) /* TRX Parameter: tTR4 */
#define RESET_TO_TRX_OFF_TYP_US             (37) /* TRX Parameter: tTR13 typical value */
#define RESET_TO_TRX_OFF_MAX_US             (1000) /* TRX Parameter: tTR2 max. value */
#define TRX_IRQ_DELAY_US                    (9)
#define IRQ_PROCESSING_DLY_US               (32)

/*
 * CSMA timimg parameters
 */
#define CCA_DETECTION_TIME_SYM          (8)
#define CCA_PROCESS_TIME_SYM            (1)
#define CCA_DURATION_SYM                (CCA_DETECTION_TIME_SYM + CCA_PROCESS_TIME_SYM)
#define CSMA_PROCESSING_TIME_US         (100)
#define CCA_PREPARATION_DURATION_US     (50)
#define CCA_PRE_START_DURATION_US       (20)
#define PRE_TX_DURATION_US              (32)    // 16 us processing delay +
                                                // 16 us software handling duration

#define HAVE_REGISTER_MAP (1)
/** Offset for register PINA
 */
#define RG_PINA                          (0x20)

/** Offset for register DDRA
 */
#define RG_DDRA                          (0x21)

/** Offset for register PORTA
 */
#define RG_PORTA                         (0x22)

/** Offset for register PINB
 */
#define RG_PINB                          (0x23)

/** Offset for register DDRB
 */
#define RG_DDRB                          (0x24)

/** Offset for register PORTB
 */
#define RG_PORTB                         (0x25)

/** Offset for register PINC
 */
#define RG_PINC                          (0x26)

/** Offset for register DDRC
 */
#define RG_DDRC                          (0x27)

/** Offset for register PORTC
 */
#define RG_PORTC                         (0x28)

/** Offset for register PIND
 */
#define RG_PIND                          (0x29)

/** Offset for register DDRD
 */
#define RG_DDRD                          (0x2a)

/** Offset for register PORTD
 */
#define RG_PORTD                         (0x2b)

/** Offset for register PINE
 */
#define RG_PINE                          (0x2c)

/** Offset for register DDRE
 */
#define RG_DDRE                          (0x2d)

/** Offset for register PORTE
 */
#define RG_PORTE                         (0x2e)

/** Offset for register PINF
 */
#define RG_PINF                          (0x2f)

/** Offset for register DDRF
 */
#define RG_DDRF                          (0x30)

/** Offset for register PORTF
 */
#define RG_PORTF                         (0x31)

/** Offset for register PING
 */
#define RG_PING                          (0x32)

/** Offset for register DDRG
 */
#define RG_DDRG                          (0x33)

/** Offset for register PORTG
 */
#define RG_PORTG                         (0x34)

/** Offset for register TIFR0
 */
#define RG_TIFR0                         (0x35)
  /** Access parameters for sub-register TOV0 in register @ref RG_TIFR0
   */
#define SR_TOV0                      0x35, 0x01, 0
  /** Access parameters for sub-register OCF0A in register @ref RG_TIFR0
   */
#define SR_OCF0A                     0x35, 0x02, 1
  /** Access parameters for sub-register OCF0B in register @ref RG_TIFR0
   */
#define SR_OCF0B                     0x35, 0x04, 2

/** Offset for register TIFR1
 */
#define RG_TIFR1                         (0x36)
  /** Access parameters for sub-register TOV1 in register @ref RG_TIFR1
   */
#define SR_TOV1                      0x36, 0x01, 0
  /** Access parameters for sub-register OCF1A in register @ref RG_TIFR1
   */
#define SR_OCF1A                     0x36, 0x02, 1
  /** Access parameters for sub-register OCF1B in register @ref RG_TIFR1
   */
#define SR_OCF1B                     0x36, 0x04, 2
  /** Access parameters for sub-register OCF1C in register @ref RG_TIFR1
   */
#define SR_OCF1C                     0x36, 0x08, 3
  /** Access parameters for sub-register ICF1 in register @ref RG_TIFR1
   */
#define SR_ICF1                      0x36, 0x20, 5

/** Offset for register TIFR2
 */
#define RG_TIFR2                         (0x37)
  /** Access parameters for sub-register TOV2 in register @ref RG_TIFR2
   */
#define SR_TOV2                      0x37, 0x01, 0
  /** Access parameters for sub-register OCF2A in register @ref RG_TIFR2
   */
#define SR_OCF2A                     0x37, 0x02, 1
  /** Access parameters for sub-register OCF2B in register @ref RG_TIFR2
   */
#define SR_OCF2B                     0x37, 0x04, 2

/** Offset for register TIFR3
 */
#define RG_TIFR3                         (0x38)
  /** Access parameters for sub-register TOV3 in register @ref RG_TIFR3
   */
#define SR_TOV3                      0x38, 0x01, 0
  /** Access parameters for sub-register OCF3A in register @ref RG_TIFR3
   */
#define SR_OCF3A                     0x38, 0x02, 1
  /** Access parameters for sub-register OCF3B in register @ref RG_TIFR3
   */
#define SR_OCF3B                     0x38, 0x04, 2
  /** Access parameters for sub-register OCF3C in register @ref RG_TIFR3
   */
#define SR_OCF3C                     0x38, 0x08, 3
  /** Access parameters for sub-register ICF3 in register @ref RG_TIFR3
   */
#define SR_ICF3                      0x38, 0x20, 5

/** Offset for register TIFR4
 */
#define RG_TIFR4                         (0x39)
  /** Access parameters for sub-register TOV4 in register @ref RG_TIFR4
   */
#define SR_TOV4                      0x39, 0x01, 0
  /** Access parameters for sub-register OCF4A in register @ref RG_TIFR4
   */
#define SR_OCF4A                     0x39, 0x02, 1
  /** Access parameters for sub-register OCF4B in register @ref RG_TIFR4
   */
#define SR_OCF4B                     0x39, 0x04, 2
  /** Access parameters for sub-register OCF4C in register @ref RG_TIFR4
   */
#define SR_OCF4C                     0x39, 0x08, 3
  /** Access parameters for sub-register ICF4 in register @ref RG_TIFR4
   */
#define SR_ICF4                      0x39, 0x20, 5

/** Offset for register TIFR5
 */
#define RG_TIFR5                         (0x3a)
  /** Access parameters for sub-register TOV5 in register @ref RG_TIFR5
   */
#define SR_TOV5                      0x3a, 0x01, 0
  /** Access parameters for sub-register OCF5A in register @ref RG_TIFR5
   */
#define SR_OCF5A                     0x3a, 0x02, 1
  /** Access parameters for sub-register OCF5B in register @ref RG_TIFR5
   */
#define SR_OCF5B                     0x3a, 0x04, 2
  /** Access parameters for sub-register OCF5C in register @ref RG_TIFR5
   */
#define SR_OCF5C                     0x3a, 0x08, 3
  /** Access parameters for sub-register ICF5 in register @ref RG_TIFR5
   */
#define SR_ICF5                      0x3a, 0x20, 5

/** Offset for register PCIFR
 */
#define RG_PCIFR                         (0x3b)
  /** Access parameters for sub-register PCIF in register @ref RG_PCIFR
   */
#define SR_PCIF                      0x3b, 0x07, 0

/** Offset for register EIFR
 */
#define RG_EIFR                          (0x3c)
  /** Access parameters for sub-register INTF in register @ref RG_EIFR
   */
#define SR_INTF                      0x3c, 0xff, 0

/** Offset for register EIMSK
 */
#define RG_EIMSK                         (0x3d)
  /** Access parameters for sub-register INT in register @ref RG_EIMSK
   */
#define SR_INT                       0x3d, 0xff, 0

/** Offset for register GPIOR0
 */
#define RG_GPIOR0                        (0x3e)
  /** Access parameters for sub-register GPIOR0 in register @ref RG_GPIOR0
   */
#define SR_GPIOR0                    0x3e, 0xff, 0

/** Offset for register EECR
 */
#define RG_EECR                          (0x3f)
  /** Access parameters for sub-register EERE in register @ref RG_EECR
   */
#define SR_EERE                      0x3f, 0x01, 0
  /** Access parameters for sub-register EEPE in register @ref RG_EECR
   */
#define SR_EEPE                      0x3f, 0x02, 1
  /** Access parameters for sub-register EEMPE in register @ref RG_EECR
   */
#define SR_EEMPE                     0x3f, 0x04, 2
  /** Access parameters for sub-register EERIE in register @ref RG_EECR
   */
#define SR_EERIE                     0x3f, 0x08, 3
  /** Access parameters for sub-register EEPM in register @ref RG_EECR
   */
#define SR_EEPM                      0x3f, 0x30, 4

/** Offset for register EEDR
 */
#define RG_EEDR                          (0x40)

/** Offset for register EEAR
 */
#define RG_EEAR                          (0x41)

/** Offset for register GTCCR
 */
#define RG_GTCCR                         (0x43)
  /** Access parameters for sub-register PSRSYNC in register @ref RG_GTCCR
   */
#define SR_PSRSYNC                   0x43, 0x01, 0
  /** Access parameters for sub-register PSRASY in register @ref RG_GTCCR
   */
#define SR_PSRASY                    0x43, 0x02, 1
  /** Access parameters for sub-register TSM in register @ref RG_GTCCR
   */
#define SR_TSM                       0x43, 0x80, 7

/** Offset for register TCCR0A
 */
#define RG_TCCR0A                        (0x44)
  /** Access parameters for sub-register WGM0 in register @ref RG_TCCR0A
   */
#define SR_WGM0                      0x44, 0x03, 0
  /** Access parameters for sub-register COM0B in register @ref RG_TCCR0A
   */
#define SR_COM0B                     0x44, 0x30, 4
  /** Access parameters for sub-register COM0A in register @ref RG_TCCR0A
   */
#define SR_COM0A                     0x44, 0xc0, 6

/** Offset for register TCCR0B
 */
#define RG_TCCR0B                        (0x45)
  /** Access parameters for sub-register CS0 in register @ref RG_TCCR0B
   */
#define SR_CS0                       0x45, 0x07, 0
  /** Access parameters for sub-register WGM02 in register @ref RG_TCCR0B
   */
#define SR_WGM02                     0x45, 0x08, 3
  /** Access parameters for sub-register FOC0B in register @ref RG_TCCR0B
   */
#define SR_FOC0B                     0x45, 0x40, 6
  /** Access parameters for sub-register FOC0A in register @ref RG_TCCR0B
   */
#define SR_FOC0A                     0x45, 0x80, 7

/** Offset for register TCNT0
 */
#define RG_TCNT0                         (0x46)

/** Offset for register OCR0A
 */
#define RG_OCR0A                         (0x47)

/** Offset for register OCR0B
 */
#define RG_OCR0B                         (0x48)

/** Offset for register GPIOR1
 */
#define RG_GPIOR1                        (0x4a)
  /** Access parameters for sub-register GPIOR1 in register @ref RG_GPIOR1
   */
#define SR_GPIOR1                    0x4a, 0xff, 0

/** Offset for register GPIOR2
 */
#define RG_GPIOR2                        (0x4b)
  /** Access parameters for sub-register GPIOR2 in register @ref RG_GPIOR2
   */
#define SR_GPIOR2                    0x4b, 0xff, 0

/** Offset for register SPCR
 */
#define RG_SPCR                          (0x4c)
  /** Access parameters for sub-register SPR in register @ref RG_SPCR
   */
#define SR_SPR                       0x4c, 0x03, 0
  /** Access parameters for sub-register CPHA in register @ref RG_SPCR
   */
#define SR_CPHA                      0x4c, 0x04, 2
  /** Access parameters for sub-register CPOL in register @ref RG_SPCR
   */
#define SR_CPOL                      0x4c, 0x08, 3
  /** Access parameters for sub-register MSTR in register @ref RG_SPCR
   */
#define SR_MSTR                      0x4c, 0x10, 4
  /** Access parameters for sub-register DORD in register @ref RG_SPCR
   */
#define SR_DORD                      0x4c, 0x20, 5
  /** Access parameters for sub-register SPE in register @ref RG_SPCR
   */
#define SR_SPE                       0x4c, 0x40, 6
  /** Access parameters for sub-register SPIE in register @ref RG_SPCR
   */
#define SR_SPIE                      0x4c, 0x80, 7

/** Offset for register SPSR
 */
#define RG_SPSR                          (0x4d)
  /** Access parameters for sub-register SPI2X in register @ref RG_SPSR
   */
#define SR_SPI2X                     0x4d, 0x01, 0
  /** Access parameters for sub-register WCOL in register @ref RG_SPSR
   */
#define SR_WCOL                      0x4d, 0x40, 6
  /** Access parameters for sub-register SPIF in register @ref RG_SPSR
   */
#define SR_SPIF                      0x4d, 0x80, 7

/** Offset for register SPDR
 */
#define RG_SPDR                          (0x4e)

/** Offset for register ACSR
 */
#define RG_ACSR                          (0x50)
  /** Access parameters for sub-register ACIS in register @ref RG_ACSR
   */
#define SR_ACIS                      0x50, 0x03, 0
  /** Access parameters for sub-register ACIC in register @ref RG_ACSR
   */
#define SR_ACIC                      0x50, 0x04, 2
  /** Access parameters for sub-register ACIE in register @ref RG_ACSR
   */
#define SR_ACIE                      0x50, 0x08, 3
  /** Access parameters for sub-register ACI in register @ref RG_ACSR
   */
#define SR_ACI                       0x50, 0x10, 4
  /** Access parameters for sub-register ACO in register @ref RG_ACSR
   */
#define SR_ACO                       0x50, 0x20, 5
  /** Access parameters for sub-register ACBG in register @ref RG_ACSR
   */
#define SR_ACBG                      0x50, 0x40, 6
  /** Access parameters for sub-register ACD in register @ref RG_ACSR
   */
#define SR_ACD                       0x50, 0x80, 7

/** Offset for register OCDR
 */
#define RG_OCDR                          (0x51)

/** Offset for register SMCR
 */
#define RG_SMCR                          (0x53)
  /** Access parameters for sub-register SE in register @ref RG_SMCR
   */
#define SR_SE                        0x53, 0x01, 0
  /** Access parameters for sub-register SM in register @ref RG_SMCR
   */
#define SR_SM                        0x53, 0x0e, 1

/** Offset for register MCUSR
 */
#define RG_MCUSR                         (0x54)
  /** Access parameters for sub-register PORF in register @ref RG_MCUSR
   */
#define SR_PORF                      0x54, 0x01, 0
  /** Access parameters for sub-register EXTRF in register @ref RG_MCUSR
   */
#define SR_EXTRF                     0x54, 0x02, 1
  /** Access parameters for sub-register BORF in register @ref RG_MCUSR
   */
#define SR_BORF                      0x54, 0x04, 2
  /** Access parameters for sub-register WDRF in register @ref RG_MCUSR
   */
#define SR_WDRF                      0x54, 0x08, 3
  /** Access parameters for sub-register JTRF in register @ref RG_MCUSR
   */
#define SR_JTRF                      0x54, 0x10, 4

/** Offset for register MCUCR
 */
#define RG_MCUCR                         (0x55)
  /** Access parameters for sub-register IVCE in register @ref RG_MCUCR
   */
#define SR_IVCE                      0x55, 0x01, 0
  /** Access parameters for sub-register IVSEL in register @ref RG_MCUCR
   */
#define SR_IVSEL                     0x55, 0x02, 1
  /** Access parameters for sub-register PUD in register @ref RG_MCUCR
   */
#define SR_PUD                       0x55, 0x10, 4
  /** Access parameters for sub-register JTD in register @ref RG_MCUCR
   */
#define SR_JTD                       0x55, 0x80, 7

/** Offset for register SPMCSR
 */
#define RG_SPMCSR                        (0x57)
  /** Access parameters for sub-register SPMEN in register @ref RG_SPMCSR
   */
#define SR_SPMEN                     0x57, 0x01, 0
  /** Access parameters for sub-register PGERS in register @ref RG_SPMCSR
   */
#define SR_PGERS                     0x57, 0x02, 1
  /** Access parameters for sub-register PGWRT in register @ref RG_SPMCSR
   */
#define SR_PGWRT                     0x57, 0x04, 2
  /** Access parameters for sub-register BLBSET in register @ref RG_SPMCSR
   */
#define SR_BLBSET                    0x57, 0x08, 3
  /** Access parameters for sub-register RWWSRE in register @ref RG_SPMCSR
   */
#define SR_RWWSRE                    0x57, 0x10, 4
  /** Access parameters for sub-register SIGRD in register @ref RG_SPMCSR
   */
#define SR_SIGRD                     0x57, 0x20, 5
  /** Access parameters for sub-register RWWSB in register @ref RG_SPMCSR
   */
#define SR_RWWSB                     0x57, 0x40, 6
  /** Access parameters for sub-register SPMIE in register @ref RG_SPMCSR
   */
#define SR_SPMIE                     0x57, 0x80, 7

/** Offset for register RAMPZ
 */
#define RG_RAMPZ                         (0x5b)

/** Offset for register SP
 */
#define RG_SP                            (0x5d)

/** Offset for register SREG
 */
#define RG_SREG                          (0x5f)
  /** Access parameters for sub-register C in register @ref RG_SREG
   */
#define SR_C                         0x5f, 0x01, 0
  /** Access parameters for sub-register Z in register @ref RG_SREG
   */
#define SR_Z                         0x5f, 0x02, 1
  /** Access parameters for sub-register N in register @ref RG_SREG
   */
#define SR_N                         0x5f, 0x04, 2
  /** Access parameters for sub-register V in register @ref RG_SREG
   */
#define SR_V                         0x5f, 0x08, 3
  /** Access parameters for sub-register S in register @ref RG_SREG
   */
#define SR_S                         0x5f, 0x10, 4
  /** Access parameters for sub-register H in register @ref RG_SREG
   */
#define SR_H                         0x5f, 0x20, 5
  /** Access parameters for sub-register T in register @ref RG_SREG
   */
#define SR_T                         0x5f, 0x40, 6
  /** Access parameters for sub-register I in register @ref RG_SREG
   */
#define SR_I                         0x5f, 0x80, 7

/** Offset for register WDTCSR
 */
#define RG_WDTCSR                        (0x60)
  /** Access parameters for sub-register WDP in register @ref RG_WDTCSR
   */
#define SR_WDP                       0x60, 0x07, 0
  /** Access parameters for sub-register WDE in register @ref RG_WDTCSR
   */
#define SR_WDE                       0x60, 0x08, 3
  /** Access parameters for sub-register WDCE in register @ref RG_WDTCSR
   */
#define SR_WDCE                      0x60, 0x10, 4
  /** Access parameters for sub-register WDIE in register @ref RG_WDTCSR
   */
#define SR_WDIE                      0x60, 0x40, 6
  /** Access parameters for sub-register WDIF in register @ref RG_WDTCSR
   */
#define SR_WDIF                      0x60, 0x80, 7

/** Offset for register CLKPR
 */
#define RG_CLKPR                         (0x61)
  /** Access parameters for sub-register CLKPS in register @ref RG_CLKPR
   */
#define SR_CLKPS                     0x61, 0x0f, 0
  /** Access parameters for sub-register CLKPCE in register @ref RG_CLKPR
   */
#define SR_CLKPCE                    0x61, 0x80, 7

/** Offset for register PRR2
 */
#define RG_PRR2                          (0x63)
  /** Access parameters for sub-register PRRAM in register @ref RG_PRR2
   */
#define SR_PRRAM                     0x63, 0x0f, 0

/** Offset for register PRR0
 */
#define RG_PRR0                          (0x64)
  /** Access parameters for sub-register PRADC in register @ref RG_PRR0
   */
#define SR_PRADC                     0x64, 0x01, 0
  /** Access parameters for sub-register PRUSART0 in register @ref RG_PRR0
   */
#define SR_PRUSART0                  0x64, 0x02, 1
  /** Access parameters for sub-register PRSPI in register @ref RG_PRR0
   */
#define SR_PRSPI                     0x64, 0x04, 2
  /** Access parameters for sub-register PRTIM1 in register @ref RG_PRR0
   */
#define SR_PRTIM1                    0x64, 0x08, 3
  /** Access parameters for sub-register PRPGA in register @ref RG_PRR0
   */
#define SR_PRPGA                     0x64, 0x10, 4
  /** Access parameters for sub-register PRTIM0 in register @ref RG_PRR0
   */
#define SR_PRTIM0                    0x64, 0x20, 5
  /** Access parameters for sub-register PRTIM2 in register @ref RG_PRR0
   */
#define SR_PRTIM2                    0x64, 0x40, 6
  /** Access parameters for sub-register PRTWI in register @ref RG_PRR0
   */
#define SR_PRTWI                     0x64, 0x80, 7

/** Offset for register PRR1
 */
#define RG_PRR1                          (0x65)
  /** Access parameters for sub-register PRUSART in register @ref RG_PRR1
   */
#define SR_PRUSART                   0x65, 0x07, 0
  /** Access parameters for sub-register PRTIM3 in register @ref RG_PRR1
   */
#define SR_PRTIM3                    0x65, 0x08, 3
  /** Access parameters for sub-register PRTIM4 in register @ref RG_PRR1
   */
#define SR_PRTIM4                    0x65, 0x10, 4
  /** Access parameters for sub-register PRTIM5 in register @ref RG_PRR1
   */
#define SR_PRTIM5                    0x65, 0x20, 5
  /** Access parameters for sub-register PRTRX24 in register @ref RG_PRR1
   */
#define SR_PRTRX24                   0x65, 0x40, 6

/** Offset for register OSCCAL
 */
#define RG_OSCCAL                        (0x66)

/** Offset for register PCICR
 */
#define RG_PCICR                         (0x68)
  /** Access parameters for sub-register PCIE in register @ref RG_PCICR
   */
#define SR_PCIE                      0x68, 0x07, 0

/** Offset for register EICRA
 */
#define RG_EICRA                         (0x69)
  /** Access parameters for sub-register ISC0 in register @ref RG_EICRA
   */
#define SR_ISC0                      0x69, 0x03, 0
  /** Access parameters for sub-register ISC1 in register @ref RG_EICRA
   */
#define SR_ISC1                      0x69, 0x0c, 2
  /** Access parameters for sub-register ISC2 in register @ref RG_EICRA
   */
#define SR_ISC2                      0x69, 0x30, 4
  /** Access parameters for sub-register ISC3 in register @ref RG_EICRA
   */
#define SR_ISC3                      0x69, 0xc0, 6

/** Offset for register EICRB
 */
#define RG_EICRB                         (0x6a)
  /** Access parameters for sub-register ISC4 in register @ref RG_EICRB
   */
#define SR_ISC4                      0x6a, 0x03, 0
  /** Access parameters for sub-register ISC5 in register @ref RG_EICRB
   */
#define SR_ISC5                      0x6a, 0x0c, 2
  /** Access parameters for sub-register ISC6 in register @ref RG_EICRB
   */
#define SR_ISC6                      0x6a, 0x30, 4
  /** Access parameters for sub-register ISC7 in register @ref RG_EICRB
   */
#define SR_ISC7                      0x6a, 0xc0, 6

/** Offset for register PCMSK0
 */
#define RG_PCMSK0                        (0x6b)

/** Offset for register PCMSK1
 */
#define RG_PCMSK1                        (0x6c)

/** Offset for register PCMSK2
 */
#define RG_PCMSK2                        (0x6d)

/** Offset for register TIMSK0
 */
#define RG_TIMSK0                        (0x6e)
  /** Access parameters for sub-register TOIE0 in register @ref RG_TIMSK0
   */
#define SR_TOIE0                     0x6e, 0x01, 0
  /** Access parameters for sub-register OCIE0A in register @ref RG_TIMSK0
   */
#define SR_OCIE0A                    0x6e, 0x02, 1
  /** Access parameters for sub-register OCIE0B in register @ref RG_TIMSK0
   */
#define SR_OCIE0B                    0x6e, 0x04, 2

/** Offset for register TIMSK1
 */
#define RG_TIMSK1                        (0x6f)
  /** Access parameters for sub-register TOIE1 in register @ref RG_TIMSK1
   */
#define SR_TOIE1                     0x6f, 0x01, 0
  /** Access parameters for sub-register OCIE1A in register @ref RG_TIMSK1
   */
#define SR_OCIE1A                    0x6f, 0x02, 1
  /** Access parameters for sub-register OCIE1B in register @ref RG_TIMSK1
   */
#define SR_OCIE1B                    0x6f, 0x04, 2
  /** Access parameters for sub-register OCIE1C in register @ref RG_TIMSK1
   */
#define SR_OCIE1C                    0x6f, 0x08, 3
  /** Access parameters for sub-register ICIE1 in register @ref RG_TIMSK1
   */
#define SR_ICIE1                     0x6f, 0x20, 5

/** Offset for register TIMSK2
 */
#define RG_TIMSK2                        (0x70)
  /** Access parameters for sub-register TOIE2 in register @ref RG_TIMSK2
   */
#define SR_TOIE2                     0x70, 0x01, 0
  /** Access parameters for sub-register OCIE2A in register @ref RG_TIMSK2
   */
#define SR_OCIE2A                    0x70, 0x02, 1
  /** Access parameters for sub-register OCIE2B in register @ref RG_TIMSK2
   */
#define SR_OCIE2B                    0x70, 0x04, 2

/** Offset for register TIMSK3
 */
#define RG_TIMSK3                        (0x71)
  /** Access parameters for sub-register TOIE3 in register @ref RG_TIMSK3
   */
#define SR_TOIE3                     0x71, 0x01, 0
  /** Access parameters for sub-register OCIE3A in register @ref RG_TIMSK3
   */
#define SR_OCIE3A                    0x71, 0x02, 1
  /** Access parameters for sub-register OCIE3B in register @ref RG_TIMSK3
   */
#define SR_OCIE3B                    0x71, 0x04, 2
  /** Access parameters for sub-register OCIE3C in register @ref RG_TIMSK3
   */
#define SR_OCIE3C                    0x71, 0x08, 3
  /** Access parameters for sub-register ICIE3 in register @ref RG_TIMSK3
   */
#define SR_ICIE3                     0x71, 0x20, 5

/** Offset for register TIMSK4
 */
#define RG_TIMSK4                        (0x72)
  /** Access parameters for sub-register TOIE4 in register @ref RG_TIMSK4
   */
#define SR_TOIE4                     0x72, 0x01, 0
  /** Access parameters for sub-register OCIE4A in register @ref RG_TIMSK4
   */
#define SR_OCIE4A                    0x72, 0x02, 1
  /** Access parameters for sub-register OCIE4B in register @ref RG_TIMSK4
   */
#define SR_OCIE4B                    0x72, 0x04, 2
  /** Access parameters for sub-register OCIE4C in register @ref RG_TIMSK4
   */
#define SR_OCIE4C                    0x72, 0x08, 3
  /** Access parameters for sub-register ICIE4 in register @ref RG_TIMSK4
   */
#define SR_ICIE4                     0x72, 0x20, 5

/** Offset for register TIMSK5
 */
#define RG_TIMSK5                        (0x73)
  /** Access parameters for sub-register TOIE5 in register @ref RG_TIMSK5
   */
#define SR_TOIE5                     0x73, 0x01, 0
  /** Access parameters for sub-register OCIE5A in register @ref RG_TIMSK5
   */
#define SR_OCIE5A                    0x73, 0x02, 1
  /** Access parameters for sub-register OCIE5B in register @ref RG_TIMSK5
   */
#define SR_OCIE5B                    0x73, 0x04, 2
  /** Access parameters for sub-register OCIE5C in register @ref RG_TIMSK5
   */
#define SR_OCIE5C                    0x73, 0x08, 3
  /** Access parameters for sub-register ICIE5 in register @ref RG_TIMSK5
   */
#define SR_ICIE5                     0x73, 0x20, 5

/** Offset for register NHVCR
 */
#define RG_NHVCR                         (0x74)
  /** Access parameters for sub-register SWPW in register @ref RG_NHVCR
   */
#define SR_SWPW                      0x74, 0x07, 0
  /** Access parameters for sub-register SEPW in register @ref RG_NHVCR
   */
#define SR_SEPW                      0x74, 0x38, 3
  /** Access parameters for sub-register ENVMR in register @ref RG_NHVCR
   */
#define SR_ENVMR                     0x74, 0x40, 6
  /** Access parameters for sub-register ENVMP in register @ref RG_NHVCR
   */
#define SR_ENVMP                     0x74, 0x80, 7

/** Offset for register NEMCR
 */
#define RG_NEMCR                         (0x75)

/** Offset for register ADCSRC
 */
#define RG_ADCSRC                        (0x77)
  /** Access parameters for sub-register ADSUT in register @ref RG_ADCSRC
   */
#define SR_ADSUT                     0x77, 0x3f, 0
  /** Access parameters for sub-register ADTHT in register @ref RG_ADCSRC
   */
#define SR_ADTHT                     0x77, 0xc0, 6

/** Offset for register ADC
 */
#define RG_ADC                           (0x78)

/** Offset for register ADCSRA
 */
#define RG_ADCSRA                        (0x7a)
  /** Access parameters for sub-register ADPS in register @ref RG_ADCSRA
   */
#define SR_ADPS                      0x7a, 0x07, 0
  /** Access parameters for sub-register ADIE in register @ref RG_ADCSRA
   */
#define SR_ADIE                      0x7a, 0x08, 3
  /** Access parameters for sub-register ADIF in register @ref RG_ADCSRA
   */
#define SR_ADIF                      0x7a, 0x10, 4
  /** Access parameters for sub-register ADATE in register @ref RG_ADCSRA
   */
#define SR_ADATE                     0x7a, 0x20, 5
  /** Access parameters for sub-register ADSC in register @ref RG_ADCSRA
   */
#define SR_ADSC                      0x7a, 0x40, 6
  /** Access parameters for sub-register ADEN in register @ref RG_ADCSRA
   */
#define SR_ADEN                      0x7a, 0x80, 7

/** Offset for register ADCSRB
 */
#define RG_ADCSRB                        (0x7b)
  /** Access parameters for sub-register ADTS in register @ref RG_ADCSRB
   */
#define SR_ADTS                      0x7b, 0x07, 0
  /** Access parameters for sub-register MUX5 in register @ref RG_ADCSRB
   */
#define SR_MUX5                      0x7b, 0x08, 3
  /** Access parameters for sub-register ACCH in register @ref RG_ADCSRB
   */
#define SR_ACCH                      0x7b, 0x10, 4
  /** Access parameters for sub-register REFOK in register @ref RG_ADCSRB
   */
#define SR_REFOK                     0x7b, 0x20, 5
  /** Access parameters for sub-register ACME in register @ref RG_ADCSRB
   */
#define SR_ACME                      0x7b, 0x40, 6
  /** Access parameters for sub-register AVDDOK in register @ref RG_ADCSRB
   */
#define SR_AVDDOK                    0x7b, 0x80, 7

/** Offset for register ADMUX
 */
#define RG_ADMUX                         (0x7c)
  /** Access parameters for sub-register MUX in register @ref RG_ADMUX
   */
#define SR_MUX                       0x7c, 0x1f, 0
  /** Access parameters for sub-register ADLAR in register @ref RG_ADMUX
   */
#define SR_ADLAR                     0x7c, 0x20, 5
  /** Access parameters for sub-register REFS in register @ref RG_ADMUX
   */
#define SR_REFS                      0x7c, 0xc0, 6

/** Offset for register DIDR2
 */
#define RG_DIDR2                         (0x7d)
  /** Access parameters for sub-register ADC8D in register @ref RG_DIDR2
   */
#define SR_ADC8D                     0x7d, 0x01, 0
  /** Access parameters for sub-register ADC9D in register @ref RG_DIDR2
   */
#define SR_ADC9D                     0x7d, 0x02, 1
  /** Access parameters for sub-register ADC10D in register @ref RG_DIDR2
   */
#define SR_ADC10D                    0x7d, 0x04, 2
  /** Access parameters for sub-register ADC11D in register @ref RG_DIDR2
   */
#define SR_ADC11D                    0x7d, 0x08, 3
  /** Access parameters for sub-register ADC12D in register @ref RG_DIDR2
   */
#define SR_ADC12D                    0x7d, 0x10, 4
  /** Access parameters for sub-register ADC13D in register @ref RG_DIDR2
   */
#define SR_ADC13D                    0x7d, 0x20, 5
  /** Access parameters for sub-register ADC14D in register @ref RG_DIDR2
   */
#define SR_ADC14D                    0x7d, 0x40, 6
  /** Access parameters for sub-register ADC15D in register @ref RG_DIDR2
   */
#define SR_ADC15D                    0x7d, 0x80, 7

/** Offset for register DIDR0
 */
#define RG_DIDR0                         (0x7e)
  /** Access parameters for sub-register ADC0D in register @ref RG_DIDR0
   */
#define SR_ADC0D                     0x7e, 0x01, 0
  /** Access parameters for sub-register ADC1D in register @ref RG_DIDR0
   */
#define SR_ADC1D                     0x7e, 0x02, 1
  /** Access parameters for sub-register ADC2D in register @ref RG_DIDR0
   */
#define SR_ADC2D                     0x7e, 0x04, 2
  /** Access parameters for sub-register ADC3D in register @ref RG_DIDR0
   */
#define SR_ADC3D                     0x7e, 0x08, 3
  /** Access parameters for sub-register ADC4D in register @ref RG_DIDR0
   */
#define SR_ADC4D                     0x7e, 0x10, 4
  /** Access parameters for sub-register ADC5D in register @ref RG_DIDR0
   */
#define SR_ADC5D                     0x7e, 0x20, 5
  /** Access parameters for sub-register ADC6D in register @ref RG_DIDR0
   */
#define SR_ADC6D                     0x7e, 0x40, 6
  /** Access parameters for sub-register ADC7D in register @ref RG_DIDR0
   */
#define SR_ADC7D                     0x7e, 0x80, 7

/** Offset for register DIDR1
 */
#define RG_DIDR1                         (0x7f)
  /** Access parameters for sub-register AIN0D in register @ref RG_DIDR1
   */
#define SR_AIN0D                     0x7f, 0x01, 0
  /** Access parameters for sub-register AIN1D in register @ref RG_DIDR1
   */
#define SR_AIN1D                     0x7f, 0x02, 1

/** Offset for register TCCR1A
 */
#define RG_TCCR1A                        (0x80)
  /** Access parameters for sub-register TCCR1A_WGM1 in register @ref RG_TCCR1A
   */
#define SR_TCCR1A_WGM1               0x80, 0x03, 0
  /** Access parameters for sub-register COM1C in register @ref RG_TCCR1A
   */
#define SR_COM1C                     0x80, 0x0c, 2
  /** Access parameters for sub-register COM1B in register @ref RG_TCCR1A
   */
#define SR_COM1B                     0x80, 0x30, 4
  /** Access parameters for sub-register COM1A in register @ref RG_TCCR1A
   */
#define SR_COM1A                     0x80, 0xc0, 6

/** Offset for register TCCR1B
 */
#define RG_TCCR1B                        (0x81)
  /** Access parameters for sub-register CS1 in register @ref RG_TCCR1B
   */
#define SR_CS1                       0x81, 0x07, 0
  /** Access parameters for sub-register TCCR1B_WGM1 in register @ref RG_TCCR1B
   */
#define SR_TCCR1B_WGM1               0x81, 0x18, 3
  /** Access parameters for sub-register ICES1 in register @ref RG_TCCR1B
   */
#define SR_ICES1                     0x81, 0x40, 6
  /** Access parameters for sub-register ICNC1 in register @ref RG_TCCR1B
   */
#define SR_ICNC1                     0x81, 0x80, 7

/** Offset for register TCCR1C
 */
#define RG_TCCR1C                        (0x82)
  /** Access parameters for sub-register FOC1C in register @ref RG_TCCR1C
   */
#define SR_FOC1C                     0x82, 0x20, 5
  /** Access parameters for sub-register FOC1B in register @ref RG_TCCR1C
   */
#define SR_FOC1B                     0x82, 0x40, 6
  /** Access parameters for sub-register FOC1A in register @ref RG_TCCR1C
   */
#define SR_FOC1A                     0x82, 0x80, 7

/** Offset for register TCNT1
 */
#define RG_TCNT1                         (0x84)

/** Offset for register ICR1
 */
#define RG_ICR1                          (0x86)

/** Offset for register OCR1A
 */
#define RG_OCR1A                         (0x88)

/** Offset for register OCR1B
 */
#define RG_OCR1B                         (0x8a)

/** Offset for register OCR1C
 */
#define RG_OCR1C                         (0x8c)

/** Offset for register TCCR3A
 */
#define RG_TCCR3A                        (0x90)
  /** Access parameters for sub-register TCCR3A_WGM3 in register @ref RG_TCCR3A
   */
#define SR_TCCR3A_WGM3               0x90, 0x03, 0
  /** Access parameters for sub-register COM3C in register @ref RG_TCCR3A
   */
#define SR_COM3C                     0x90, 0x0c, 2
  /** Access parameters for sub-register COM3B in register @ref RG_TCCR3A
   */
#define SR_COM3B                     0x90, 0x30, 4
  /** Access parameters for sub-register COM3A in register @ref RG_TCCR3A
   */
#define SR_COM3A                     0x90, 0xc0, 6

/** Offset for register TCCR3B
 */
#define RG_TCCR3B                        (0x91)
  /** Access parameters for sub-register CS3 in register @ref RG_TCCR3B
   */
#define SR_CS3                       0x91, 0x07, 0
  /** Access parameters for sub-register TCCR3B_WGM3 in register @ref RG_TCCR3B
   */
#define SR_TCCR3B_WGM3               0x91, 0x18, 3
  /** Access parameters for sub-register ICES3 in register @ref RG_TCCR3B
   */
#define SR_ICES3                     0x91, 0x40, 6
  /** Access parameters for sub-register ICNC3 in register @ref RG_TCCR3B
   */
#define SR_ICNC3                     0x91, 0x80, 7

/** Offset for register TCCR3C
 */
#define RG_TCCR3C                        (0x92)
  /** Access parameters for sub-register FOC3C in register @ref RG_TCCR3C
   */
#define SR_FOC3C                     0x92, 0x20, 5
  /** Access parameters for sub-register FOC3B in register @ref RG_TCCR3C
   */
#define SR_FOC3B                     0x92, 0x40, 6
  /** Access parameters for sub-register FOC3A in register @ref RG_TCCR3C
   */
#define SR_FOC3A                     0x92, 0x80, 7

/** Offset for register TCNT3
 */
#define RG_TCNT3                         (0x94)

/** Offset for register ICR3
 */
#define RG_ICR3                          (0x96)

/** Offset for register OCR3A
 */
#define RG_OCR3A                         (0x98)

/** Offset for register OCR3B
 */
#define RG_OCR3B                         (0x9a)

/** Offset for register OCR3C
 */
#define RG_OCR3C                         (0x9c)

/** Offset for register TCCR4A
 */
#define RG_TCCR4A                        (0xa0)
  /** Access parameters for sub-register TCCR4A_WGM4 in register @ref RG_TCCR4A
   */
#define SR_TCCR4A_WGM4               0xa0, 0x03, 0
  /** Access parameters for sub-register COM4C in register @ref RG_TCCR4A
   */
#define SR_COM4C                     0xa0, 0x0c, 2
  /** Access parameters for sub-register COM4B in register @ref RG_TCCR4A
   */
#define SR_COM4B                     0xa0, 0x30, 4
  /** Access parameters for sub-register COM4A in register @ref RG_TCCR4A
   */
#define SR_COM4A                     0xa0, 0xc0, 6

/** Offset for register TCCR4B
 */
#define RG_TCCR4B                        (0xa1)
  /** Access parameters for sub-register CS4 in register @ref RG_TCCR4B
   */
#define SR_CS4                       0xa1, 0x07, 0
  /** Access parameters for sub-register TCCR4B_WGM4 in register @ref RG_TCCR4B
   */
#define SR_TCCR4B_WGM4               0xa1, 0x18, 3
  /** Access parameters for sub-register ICES4 in register @ref RG_TCCR4B
   */
#define SR_ICES4                     0xa1, 0x40, 6
  /** Access parameters for sub-register ICNC4 in register @ref RG_TCCR4B
   */
#define SR_ICNC4                     0xa1, 0x80, 7

/** Offset for register TCCR4C
 */
#define RG_TCCR4C                        (0xa2)
  /** Access parameters for sub-register FOC4C in register @ref RG_TCCR4C
   */
#define SR_FOC4C                     0xa2, 0x20, 5
  /** Access parameters for sub-register FOC4B in register @ref RG_TCCR4C
   */
#define SR_FOC4B                     0xa2, 0x40, 6
  /** Access parameters for sub-register FOC4A in register @ref RG_TCCR4C
   */
#define SR_FOC4A                     0xa2, 0x80, 7

/** Offset for register TCNT4
 */
#define RG_TCNT4                         (0xa4)

/** Offset for register ICR4
 */
#define RG_ICR4                          (0xa6)

/** Offset for register OCR4A
 */
#define RG_OCR4A                         (0xa8)

/** Offset for register OCR4B
 */
#define RG_OCR4B                         (0xaa)

/** Offset for register OCR4C
 */
#define RG_OCR4C                         (0xac)

/** Offset for register TCCR2A
 */
#define RG_TCCR2A                        (0xb0)
  /** Access parameters for sub-register WGM2 in register @ref RG_TCCR2A
   */
#define SR_WGM2                      0xb0, 0x03, 0
  /** Access parameters for sub-register COM2B in register @ref RG_TCCR2A
   */
#define SR_COM2B                     0xb0, 0x30, 4
  /** Access parameters for sub-register COM2A in register @ref RG_TCCR2A
   */
#define SR_COM2A                     0xb0, 0xc0, 6

/** Offset for register TCCR2B
 */
#define RG_TCCR2B                        (0xb1)
  /** Access parameters for sub-register CS2 in register @ref RG_TCCR2B
   */
#define SR_CS2                       0xb1, 0x07, 0
  /** Access parameters for sub-register WGM22 in register @ref RG_TCCR2B
   */
#define SR_WGM22                     0xb1, 0x08, 3
  /** Access parameters for sub-register FOC2B in register @ref RG_TCCR2B
   */
#define SR_FOC2B                     0xb1, 0x40, 6
  /** Access parameters for sub-register FOC2A in register @ref RG_TCCR2B
   */
#define SR_FOC2A                     0xb1, 0x80, 7

/** Offset for register TCNT2
 */
#define RG_TCNT2                         (0xb2)

/** Offset for register OCR2A
 */
#define RG_OCR2A                         (0xb3)

/** Offset for register OCR2B
 */
#define RG_OCR2B                         (0xb4)

/** Offset for register ASSR
 */
#define RG_ASSR                          (0xb6)
  /** Access parameters for sub-register TCR2BUB in register @ref RG_ASSR
   */
#define SR_TCR2BUB                   0xb6, 0x01, 0
  /** Access parameters for sub-register TCR2AUB in register @ref RG_ASSR
   */
#define SR_TCR2AUB                   0xb6, 0x02, 1
  /** Access parameters for sub-register OCR2BUB in register @ref RG_ASSR
   */
#define SR_OCR2BUB                   0xb6, 0x04, 2
  /** Access parameters for sub-register OCR2AUB in register @ref RG_ASSR
   */
#define SR_OCR2AUB                   0xb6, 0x08, 3
  /** Access parameters for sub-register TCN2UB in register @ref RG_ASSR
   */
#define SR_TCN2UB                    0xb6, 0x10, 4
  /** Access parameters for sub-register AS2 in register @ref RG_ASSR
   */
#define SR_AS2                       0xb6, 0x20, 5
  /** Access parameters for sub-register EXCLK in register @ref RG_ASSR
   */
#define SR_EXCLK                     0xb6, 0x40, 6
  /** Access parameters for sub-register EXCLKAMR in register @ref RG_ASSR
   */
#define SR_EXCLKAMR                  0xb6, 0x80, 7

/** Offset for register TWBR
 */
#define RG_TWBR                          (0xb8)

/** Offset for register TWSR
 */
#define RG_TWSR                          (0xb9)
  /** Access parameters for sub-register TWPS in register @ref RG_TWSR
   */
#define SR_TWPS                      0xb9, 0x03, 0
  /** Access parameters for sub-register TWS in register @ref RG_TWSR
   */
#define SR_TWS                       0xb9, 0xf8, 3

/** Offset for register TWAR
 */
#define RG_TWAR                          (0xba)
  /** Access parameters for sub-register TWGCE in register @ref RG_TWAR
   */
#define SR_TWGCE                     0xba, 0x01, 0
  /** Access parameters for sub-register TWA in register @ref RG_TWAR
   */
#define SR_TWA                       0xba, 0xfe, 1

/** Offset for register TWDR
 */
#define RG_TWDR                          (0xbb)

/** Offset for register TWCR
 */
#define RG_TWCR                          (0xbc)
  /** Access parameters for sub-register TWIE in register @ref RG_TWCR
   */
#define SR_TWIE                      0xbc, 0x01, 0
  /** Access parameters for sub-register TWEN in register @ref RG_TWCR
   */
#define SR_TWEN                      0xbc, 0x04, 2
  /** Access parameters for sub-register TWWC in register @ref RG_TWCR
   */
#define SR_TWWC                      0xbc, 0x08, 3
  /** Access parameters for sub-register TWSTO in register @ref RG_TWCR
   */
#define SR_TWSTO                     0xbc, 0x10, 4
  /** Access parameters for sub-register TWSTA in register @ref RG_TWCR
   */
#define SR_TWSTA                     0xbc, 0x20, 5
  /** Access parameters for sub-register TWEA in register @ref RG_TWCR
   */
#define SR_TWEA                      0xbc, 0x40, 6
  /** Access parameters for sub-register TWINT in register @ref RG_TWCR
   */
#define SR_TWINT                     0xbc, 0x80, 7

/** Offset for register TWAMR
 */
#define RG_TWAMR                         (0xbd)
  /** Access parameters for sub-register TWAM in register @ref RG_TWAMR
   */
#define SR_TWAM                      0xbd, 0xfe, 1

/** Offset for register UCSR0A
 */
#define RG_UCSR0A                        (0xc0)
  /** Access parameters for sub-register MPCM0 in register @ref RG_UCSR0A
   */
#define SR_MPCM0                     0xc0, 0x01, 0
  /** Access parameters for sub-register U2X0 in register @ref RG_UCSR0A
   */
#define SR_U2X0                      0xc0, 0x02, 1
  /** Access parameters for sub-register UPE0 in register @ref RG_UCSR0A
   */
#define SR_UPE0                      0xc0, 0x04, 2
  /** Access parameters for sub-register DOR0 in register @ref RG_UCSR0A
   */
#define SR_DOR0                      0xc0, 0x08, 3
  /** Access parameters for sub-register FE0 in register @ref RG_UCSR0A
   */
#define SR_FE0                       0xc0, 0x10, 4
  /** Access parameters for sub-register UDRE0 in register @ref RG_UCSR0A
   */
#define SR_UDRE0                     0xc0, 0x20, 5
  /** Access parameters for sub-register TXC0 in register @ref RG_UCSR0A
   */
#define SR_TXC0                      0xc0, 0x40, 6
  /** Access parameters for sub-register RXC0 in register @ref RG_UCSR0A
   */
#define SR_RXC0                      0xc0, 0x80, 7

/** Offset for register UCSR0B
 */
#define RG_UCSR0B                        (0xc1)
  /** Access parameters for sub-register TXB80 in register @ref RG_UCSR0B
   */
#define SR_TXB80                     0xc1, 0x01, 0
  /** Access parameters for sub-register RXB80 in register @ref RG_UCSR0B
   */
#define SR_RXB80                     0xc1, 0x02, 1
  /** Access parameters for sub-register UCSZ02 in register @ref RG_UCSR0B
   */
#define SR_UCSZ02                    0xc1, 0x04, 2
  /** Access parameters for sub-register TXEN0 in register @ref RG_UCSR0B
   */
#define SR_TXEN0                     0xc1, 0x08, 3
  /** Access parameters for sub-register RXEN0 in register @ref RG_UCSR0B
   */
#define SR_RXEN0                     0xc1, 0x10, 4
  /** Access parameters for sub-register UDRIE0 in register @ref RG_UCSR0B
   */
#define SR_UDRIE0                    0xc1, 0x20, 5
  /** Access parameters for sub-register TXCIE0 in register @ref RG_UCSR0B
   */
#define SR_TXCIE0                    0xc1, 0x40, 6
  /** Access parameters for sub-register RXCIE0 in register @ref RG_UCSR0B
   */
#define SR_RXCIE0                    0xc1, 0x80, 7

/** Offset for register UCSR0C
 */
#define RG_UCSR0C                        (0xc2)
  /** Access parameters for sub-register UCPOL0 in register @ref RG_UCSR0C
   */
#define SR_UCPOL0                    0xc2, 0x01, 0
  /** Access parameters for sub-register UCSZ0 in register @ref RG_UCSR0C
   */
#define SR_UCSZ0                     0xc2, 0x06, 1
  /** Access parameters for sub-register USBS0 in register @ref RG_UCSR0C
   */
#define SR_USBS0                     0xc2, 0x08, 3
  /** Access parameters for sub-register UPM0 in register @ref RG_UCSR0C
   */
#define SR_UPM0                      0xc2, 0x30, 4
  /** Access parameters for sub-register UMSEL0 in register @ref RG_UCSR0C
   */
#define SR_UMSEL0                    0xc2, 0xc0, 6

/** Offset for register UBRR0
 */
#define RG_UBRR0                         (0xc4)

/** Offset for register UDR0
 */
#define RG_UDR0                          (0xc6)

/** Offset for register UCSR1A
 */
#define RG_UCSR1A                        (0xc8)
  /** Access parameters for sub-register MPCM1 in register @ref RG_UCSR1A
   */
#define SR_MPCM1                     0xc8, 0x01, 0
  /** Access parameters for sub-register U2X1 in register @ref RG_UCSR1A
   */
#define SR_U2X1                      0xc8, 0x02, 1
  /** Access parameters for sub-register UPE1 in register @ref RG_UCSR1A
   */
#define SR_UPE1                      0xc8, 0x04, 2
  /** Access parameters for sub-register DOR1 in register @ref RG_UCSR1A
   */
#define SR_DOR1                      0xc8, 0x08, 3
  /** Access parameters for sub-register FE1 in register @ref RG_UCSR1A
   */
#define SR_FE1                       0xc8, 0x10, 4
  /** Access parameters for sub-register UDRE1 in register @ref RG_UCSR1A
   */
#define SR_UDRE1                     0xc8, 0x20, 5
  /** Access parameters for sub-register TXC1 in register @ref RG_UCSR1A
   */
#define SR_TXC1                      0xc8, 0x40, 6
  /** Access parameters for sub-register RXC1 in register @ref RG_UCSR1A
   */
#define SR_RXC1                      0xc8, 0x80, 7

/** Offset for register UCSR1B
 */
#define RG_UCSR1B                        (0xc9)
  /** Access parameters for sub-register TXB81 in register @ref RG_UCSR1B
   */
#define SR_TXB81                     0xc9, 0x01, 0
  /** Access parameters for sub-register RXB81 in register @ref RG_UCSR1B
   */
#define SR_RXB81                     0xc9, 0x02, 1
  /** Access parameters for sub-register UCSZ12 in register @ref RG_UCSR1B
   */
#define SR_UCSZ12                    0xc9, 0x04, 2
  /** Access parameters for sub-register TXEN1 in register @ref RG_UCSR1B
   */
#define SR_TXEN1                     0xc9, 0x08, 3
  /** Access parameters for sub-register RXEN1 in register @ref RG_UCSR1B
   */
#define SR_RXEN1                     0xc9, 0x10, 4
  /** Access parameters for sub-register UDRIE1 in register @ref RG_UCSR1B
   */
#define SR_UDRIE1                    0xc9, 0x20, 5
  /** Access parameters for sub-register TXCIE1 in register @ref RG_UCSR1B
   */
#define SR_TXCIE1                    0xc9, 0x40, 6
  /** Access parameters for sub-register RXCIE1 in register @ref RG_UCSR1B
   */
#define SR_RXCIE1                    0xc9, 0x80, 7

/** Offset for register UCSR1C
 */
#define RG_UCSR1C                        (0xca)
  /** Access parameters for sub-register UCPOL1 in register @ref RG_UCSR1C
   */
#define SR_UCPOL1                    0xca, 0x01, 0
  /** Access parameters for sub-register UCSZ1 in register @ref RG_UCSR1C
   */
#define SR_UCSZ1                     0xca, 0x06, 1
  /** Access parameters for sub-register USBS1 in register @ref RG_UCSR1C
   */
#define SR_USBS1                     0xca, 0x08, 3
  /** Access parameters for sub-register UPM1 in register @ref RG_UCSR1C
   */
#define SR_UPM1                      0xca, 0x30, 4
  /** Access parameters for sub-register UMSEL1 in register @ref RG_UCSR1C
   */
#define SR_UMSEL1                    0xca, 0xc0, 6

/** Offset for register UBRR1
 */
#define RG_UBRR1                         (0xcc)

/** Offset for register UDR1
 */
#define RG_UDR1                          (0xce)

/** Offset for register ATBR18
 */
#define RG_ATBR18                        (0xd0)

/** Offset for register ATBR33
 */
#define RG_ATBR33                        (0xd1)

/** Offset for register SCCR0
 */
#define RG_SCCR0                         (0xdc)
  /** Access parameters for sub-register SCCMP in register @ref RG_SCCR0
   */
#define SR_SCCMP                     0xdc, 0x07, 0
  /** Access parameters for sub-register SCTSE in register @ref RG_SCCR0
   */
#define SR_SCTSE                     0xdc, 0x08, 3
  /** Access parameters for sub-register SCCKSEL in register @ref RG_SCCR0
   */
#define SR_SCCKSEL                   0xdc, 0x10, 4
  /** Access parameters for sub-register SCEN in register @ref RG_SCCR0
   */
#define SR_SCEN                      0xdc, 0x20, 5
  /** Access parameters for sub-register SCMBTS in register @ref RG_SCCR0
   */
#define SR_SCMBTS                    0xdc, 0x40, 6
  /** Access parameters for sub-register SCRES in register @ref RG_SCCR0
   */
#define SR_SCRES                     0xdc, 0x80, 7

/** Offset for register SCCR1
 */
#define RG_SCCR1                         (0xdd)
  /** Access parameters for sub-register SCENBO in register @ref RG_SCCR1
   */
#define SR_SCENBO                    0xdd, 0x01, 0

/** Offset for register SCSR
 */
#define RG_SCSR                          (0xde)
  /** Access parameters for sub-register SCBSY in register @ref RG_SCSR
   */
#define SR_SCBSY                     0xde, 0x01, 0

/** Offset for register SCIRQM
 */
#define RG_SCIRQM                        (0xdf)
  /** Access parameters for sub-register IRQMCP in register @ref RG_SCIRQM
   */
#define SR_IRQMCP                    0xdf, 0x07, 0
  /** Access parameters for sub-register IRQMOF in register @ref RG_SCIRQM
   */
#define SR_IRQMOF                    0xdf, 0x08, 3
  /** Access parameters for sub-register IRQMBO in register @ref RG_SCIRQM
   */
#define SR_IRQMBO                    0xdf, 0x10, 4

/** Offset for register SCIRQS
 */
#define RG_SCIRQS                        (0xe0)
  /** Access parameters for sub-register IRQSCP in register @ref RG_SCIRQS
   */
#define SR_IRQSCP                    0xe0, 0x07, 0
  /** Access parameters for sub-register IRQSOF in register @ref RG_SCIRQS
   */
#define SR_IRQSOF                    0xe0, 0x08, 3
  /** Access parameters for sub-register IRQSBO in register @ref RG_SCIRQS
   */
#define SR_IRQSBO                    0xe0, 0x10, 4

/** Offset for register SCCNTLL
 */
#define RG_SCCNTLL                       (0xe1)
  /** Access parameters for sub-register SCCNTLL in register @ref RG_SCCNTLL
   */
#define SR_SCCNTLL                   0xe1, 0xff, 0

/** Offset for register SCCNTLH
 */
#define RG_SCCNTLH                       (0xe2)
  /** Access parameters for sub-register SCCNTLH in register @ref RG_SCCNTLH
   */
#define SR_SCCNTLH                   0xe2, 0xff, 0

/** Offset for register SCCNTHL
 */
#define RG_SCCNTHL                       (0xe3)
  /** Access parameters for sub-register SCCNTHL in register @ref RG_SCCNTHL
   */
#define SR_SCCNTHL                   0xe3, 0xff, 0

/** Offset for register SCCNTHH
 */
#define RG_SCCNTHH                       (0xe4)
  /** Access parameters for sub-register SCCNTHH in register @ref RG_SCCNTHH
   */
#define SR_SCCNTHH                   0xe4, 0xff, 0

/** Offset for register SCBTSRLL
 */
#define RG_SCBTSRLL                      (0xe5)
  /** Access parameters for sub-register SCBTSRLL in register @ref RG_SCBTSRLL
   */
#define SR_SCBTSRLL                  0xe5, 0xff, 0

/** Offset for register SCBTSRLH
 */
#define RG_SCBTSRLH                      (0xe6)
  /** Access parameters for sub-register SCBTSRLH in register @ref RG_SCBTSRLH
   */
#define SR_SCBTSRLH                  0xe6, 0xff, 0

/** Offset for register SCBTSRHL
 */
#define RG_SCBTSRHL                      (0xe7)
  /** Access parameters for sub-register SCBTSRHL in register @ref RG_SCBTSRHL
   */
#define SR_SCBTSRHL                  0xe7, 0xff, 0

/** Offset for register SCBTSRHH
 */
#define RG_SCBTSRHH                      (0xe8)
  /** Access parameters for sub-register SCBTSRHH in register @ref RG_SCBTSRHH
   */
#define SR_SCBTSRHH                  0xe8, 0xff, 0

/** Offset for register SCTSRLL
 */
#define RG_SCTSRLL                       (0xe9)
  /** Access parameters for sub-register SCTSRLL in register @ref RG_SCTSRLL
   */
#define SR_SCTSRLL                   0xe9, 0xff, 0

/** Offset for register SCTSRLH
 */
#define RG_SCTSRLH                       (0xea)
  /** Access parameters for sub-register SCTSRLH in register @ref RG_SCTSRLH
   */
#define SR_SCTSRLH                   0xea, 0xff, 0

/** Offset for register SCTSRHL
 */
#define RG_SCTSRHL                       (0xeb)
  /** Access parameters for sub-register SCTSRHL in register @ref RG_SCTSRHL
   */
#define SR_SCTSRHL                   0xeb, 0xff, 0

/** Offset for register SCTSRHH
 */
#define RG_SCTSRHH                       (0xec)
  /** Access parameters for sub-register SCTSRHH in register @ref RG_SCTSRHH
   */
#define SR_SCTSRHH                   0xec, 0xff, 0

/** Offset for register SCOCR3LL
 */
#define RG_SCOCR3LL                      (0xed)
  /** Access parameters for sub-register SCOCR3LL in register @ref RG_SCOCR3LL
   */
#define SR_SCOCR3LL                  0xed, 0xff, 0

/** Offset for register SCOCR3LH
 */
#define RG_SCOCR3LH                      (0xee)
  /** Access parameters for sub-register SCOCR3LH in register @ref RG_SCOCR3LH
   */
#define SR_SCOCR3LH                  0xee, 0xff, 0

/** Offset for register SCOCR3HL
 */
#define RG_SCOCR3HL                      (0xef)
  /** Access parameters for sub-register SCOCR3HL in register @ref RG_SCOCR3HL
   */
#define SR_SCOCR3HL                  0xef, 0xff, 0

/** Offset for register SCOCR3HH
 */
#define RG_SCOCR3HH                      (0xf0)
  /** Access parameters for sub-register SCOCR3HH in register @ref RG_SCOCR3HH
   */
#define SR_SCOCR3HH                  0xf0, 0xff, 0

/** Offset for register SCOCR2LL
 */
#define RG_SCOCR2LL                      (0xf1)
  /** Access parameters for sub-register SCOCR2LL in register @ref RG_SCOCR2LL
   */
#define SR_SCOCR2LL                  0xf1, 0xff, 0

/** Offset for register SCOCR2LH
 */
#define RG_SCOCR2LH                      (0xf2)
  /** Access parameters for sub-register SCOCR2LH in register @ref RG_SCOCR2LH
   */
#define SR_SCOCR2LH                  0xf2, 0xff, 0

/** Offset for register SCOCR2HL
 */
#define RG_SCOCR2HL                      (0xf3)
  /** Access parameters for sub-register SCOCR2HL in register @ref RG_SCOCR2HL
   */
#define SR_SCOCR2HL                  0xf3, 0xff, 0

/** Offset for register SCOCR2HH
 */
#define RG_SCOCR2HH                      (0xf4)
  /** Access parameters for sub-register SCOCR2HH in register @ref RG_SCOCR2HH
   */
#define SR_SCOCR2HH                  0xf4, 0xff, 0

/** Offset for register SCOCR1LL
 */
#define RG_SCOCR1LL                      (0xf5)
  /** Access parameters for sub-register SCOCR1LL in register @ref RG_SCOCR1LL
   */
#define SR_SCOCR1LL                  0xf5, 0xff, 0

/** Offset for register SCOCR1LH
 */
#define RG_SCOCR1LH                      (0xf6)
  /** Access parameters for sub-register SCOCR1LH in register @ref RG_SCOCR1LH
   */
#define SR_SCOCR1LH                  0xf6, 0xff, 0

/** Offset for register SCOCR1HL
 */
#define RG_SCOCR1HL                      (0xf7)
  /** Access parameters for sub-register SCOCR1HL in register @ref RG_SCOCR1HL
   */
#define SR_SCOCR1HL                  0xf7, 0xff, 0

/** Offset for register SCOCR1HH
 */
#define RG_SCOCR1HH                      (0xf8)
  /** Access parameters for sub-register SCOCR1HH in register @ref RG_SCOCR1HH
   */
#define SR_SCOCR1HH                  0xf8, 0xff, 0

/** Offset for register TCCR5A
 */
#define RG_TCCR5A                        (0x120)
  /** Access parameters for sub-register TCCR5A_WGM5 in register @ref RG_TCCR5A
   */
#define SR_TCCR5A_WGM5               0x120, 0x03, 0
  /** Access parameters for sub-register COM5C in register @ref RG_TCCR5A
   */
#define SR_COM5C                     0x120, 0x0c, 2
  /** Access parameters for sub-register COM5B in register @ref RG_TCCR5A
   */
#define SR_COM5B                     0x120, 0x30, 4
  /** Access parameters for sub-register COM5A in register @ref RG_TCCR5A
   */
#define SR_COM5A                     0x120, 0xc0, 6

/** Offset for register TCCR5B
 */
#define RG_TCCR5B                        (0x121)
  /** Access parameters for sub-register CS5 in register @ref RG_TCCR5B
   */
#define SR_CS5                       0x121, 0x07, 0
  /** Access parameters for sub-register TCCR5B_WGM5 in register @ref RG_TCCR5B
   */
#define SR_TCCR5B_WGM5               0x121, 0x18, 3
  /** Access parameters for sub-register ICES5 in register @ref RG_TCCR5B
   */
#define SR_ICES5                     0x121, 0x40, 6
  /** Access parameters for sub-register ICNC5 in register @ref RG_TCCR5B
   */
#define SR_ICNC5                     0x121, 0x80, 7

/** Offset for register TCCR5C
 */
#define RG_TCCR5C                        (0x122)
  /** Access parameters for sub-register FOC5C in register @ref RG_TCCR5C
   */
#define SR_FOC5C                     0x122, 0x20, 5
  /** Access parameters for sub-register FOC5B in register @ref RG_TCCR5C
   */
#define SR_FOC5B                     0x122, 0x40, 6
  /** Access parameters for sub-register FOC5A in register @ref RG_TCCR5C
   */
#define SR_FOC5A                     0x122, 0x80, 7

/** Offset for register TCNT5
 */
#define RG_TCNT5                         (0x124)

/** Offset for register ICR5
 */
#define RG_ICR5                          (0x126)

/** Offset for register OCR5A
 */
#define RG_OCR5A                         (0x128)

/** Offset for register OCR5B
 */
#define RG_OCR5B                         (0x12a)

/** Offset for register OCR5C
 */
#define RG_OCR5C                         (0x12c)

/** Offset for register LLCR
 */
#define RG_LLCR                          (0x12f)
  /** Access parameters for sub-register LLENCAL in register @ref RG_LLCR
   */
#define SR_LLENCAL                   0x12f, 0x01, 0
  /** Access parameters for sub-register LLSHORT in register @ref RG_LLCR
   */
#define SR_LLSHORT                   0x12f, 0x02, 1
  /** Access parameters for sub-register LLTCO in register @ref RG_LLCR
   */
#define SR_LLTCO                     0x12f, 0x04, 2
  /** Access parameters for sub-register LLCAL in register @ref RG_LLCR
   */
#define SR_LLCAL                     0x12f, 0x08, 3
  /** Access parameters for sub-register LLCOMP in register @ref RG_LLCR
   */
#define SR_LLCOMP                    0x12f, 0x10, 4
  /** Access parameters for sub-register LLDONE in register @ref RG_LLCR
   */
#define SR_LLDONE                    0x12f, 0x20, 5

/** Offset for register LLDRL
 */
#define RG_LLDRL                         (0x130)
  /** Access parameters for sub-register LLDRL in register @ref RG_LLDRL
   */
#define SR_LLDRL                     0x130, 0x0f, 0

/** Offset for register LLDRH
 */
#define RG_LLDRH                         (0x131)
  /** Access parameters for sub-register LLDRH in register @ref RG_LLDRH
   */
#define SR_LLDRH                     0x131, 0x1f, 0

/** Offset for register DRTRAM3
 */
#define RG_DRTRAM3                       (0x132)
  /** Access parameters for sub-register DRTRAM3_DRTMN in register @ref RG_DRTRAM3
   */
#define SR_DRTRAM3_DRTMN             0x132, 0x03, 0
  /** Access parameters for sub-register DRTRAM3_DRTMP in register @ref RG_DRTRAM3
   */
#define SR_DRTRAM3_DRTMP             0x132, 0x0c, 2
  /** Access parameters for sub-register DRTRAM3_ENDRT in register @ref RG_DRTRAM3
   */
#define SR_DRTRAM3_ENDRT             0x132, 0x10, 4
  /** Access parameters for sub-register DRTRAM3_DRTSWOK in register @ref RG_DRTRAM3
   */
#define SR_DRTRAM3_DRTSWOK           0x132, 0x20, 5

/** Offset for register DRTRAM2
 */
#define RG_DRTRAM2                       (0x133)
  /** Access parameters for sub-register DRTRAM2_DRTMN in register @ref RG_DRTRAM2
   */
#define SR_DRTRAM2_DRTMN             0x133, 0x03, 0
  /** Access parameters for sub-register DRTRAM2_DRTMP in register @ref RG_DRTRAM2
   */
#define SR_DRTRAM2_DRTMP             0x133, 0x0c, 2
  /** Access parameters for sub-register DRTRAM2_ENDRT in register @ref RG_DRTRAM2
   */
#define SR_DRTRAM2_ENDRT             0x133, 0x10, 4
  /** Access parameters for sub-register DRTRAM2_DRTSWOK in register @ref RG_DRTRAM2
   */
#define SR_DRTRAM2_DRTSWOK           0x133, 0x20, 5
  /** Access parameters for sub-register DISPC in register @ref RG_DRTRAM2
   */
#define SR_DISPC                     0x133, 0x80, 7

/** Offset for register DRTRAM1
 */
#define RG_DRTRAM1                       (0x134)
  /** Access parameters for sub-register DRTRAM1_DRTMN in register @ref RG_DRTRAM1
   */
#define SR_DRTRAM1_DRTMN             0x134, 0x03, 0
  /** Access parameters for sub-register DRTRAM1_DRTMP in register @ref RG_DRTRAM1
   */
#define SR_DRTRAM1_DRTMP             0x134, 0x0c, 2
  /** Access parameters for sub-register DRTRAM1_ENDRT in register @ref RG_DRTRAM1
   */
#define SR_DRTRAM1_ENDRT             0x134, 0x10, 4
  /** Access parameters for sub-register DRTRAM1_DRTSWOK in register @ref RG_DRTRAM1
   */
#define SR_DRTRAM1_DRTSWOK           0x134, 0x20, 5

/** Offset for register DRTRAM0
 */
#define RG_DRTRAM0                       (0x135)
  /** Access parameters for sub-register DRTRAM0_DRTMN in register @ref RG_DRTRAM0
   */
#define SR_DRTRAM0_DRTMN             0x135, 0x03, 0
  /** Access parameters for sub-register DRTRAM0_DRTMP in register @ref RG_DRTRAM0
   */
#define SR_DRTRAM0_DRTMP             0x135, 0x0c, 2
  /** Access parameters for sub-register DRTRAM0_ENDRT in register @ref RG_DRTRAM0
   */
#define SR_DRTRAM0_ENDRT             0x135, 0x10, 4
  /** Access parameters for sub-register DRTRAM0_DRTSWOK in register @ref RG_DRTRAM0
   */
#define SR_DRTRAM0_DRTSWOK           0x135, 0x20, 5

/** Offset for register DPDS0
 */
#define RG_DPDS0                         (0x136)
  /** Access parameters for sub-register PBDRV in register @ref RG_DPDS0
   */
#define SR_PBDRV                     0x136, 0x03, 0
  /** Access parameters for sub-register PDDRV in register @ref RG_DPDS0
   */
#define SR_PDDRV                     0x136, 0x0c, 2
  /** Access parameters for sub-register PEDRV in register @ref RG_DPDS0
   */
#define SR_PEDRV                     0x136, 0x30, 4
  /** Access parameters for sub-register PFDRV in register @ref RG_DPDS0
   */
#define SR_PFDRV                     0x136, 0xc0, 6

/** Offset for register DPDS1
 */
#define RG_DPDS1                         (0x137)
  /** Access parameters for sub-register PGDRV in register @ref RG_DPDS1
   */
#define SR_PGDRV                     0x137, 0x03, 0

/** Offset for register TRXPR
 */
#define RG_TRXPR                         (0x139)
  /** Access parameters for sub-register TRXRST in register @ref RG_TRXPR
   */
#define SR_TRXRST                    0x139, 0x01, 0
  /** Access parameters for sub-register SLPTR in register @ref RG_TRXPR
   */
#define SR_SLPTR                     0x139, 0x02, 1
#define SR_TRXTST                    0x139, 0x04, 2
  /** Access parameters for sub-register ATBE in register @ref RG_TRXPR
   */
#define SR_ATBE                      0x139, 0x08, 3

/** Offset for register AES_TEST
 */
#define RG_AES_TEST                      (0x13b)
  /** Access parameters for sub-register AES_OK in register @ref RG_AES_TEST
   */
#define SR_AES_OK                    0x13b, 0x01, 0
  /** Access parameters for sub-register AES_DONE in register @ref RG_AES_TEST
   */
#define SR_AES_DONE                  0x13b, 0x02, 1
  /** Access parameters for sub-register AES_TR in register @ref RG_AES_TEST
   */
#define SR_AES_TR                    0x13b, 0x80, 7

/** Offset for register AES_CTRL
 */
#define RG_AES_CTRL                       (0x13c)
  /** Access parameters for sub-register AES_IM in register @ref RG_AES_CTRL
   */
#define SR_AES_IM                    0x13c, 0x04, 2
  /** Access parameters for sub-register AES_DIR in register @ref RG_AES_CTRL
   */
#define SR_AES_DIR                   0x13c, 0x08, 3
  /** Access parameters for sub-register AES_MODE in register @ref RG_AES_CTRL
   */
#define SR_AES_MODE                  0x13c, 0x20, 5
  /** Access parameters for sub-register AES_REQUEST in register @ref RG_AES_CTRL
   */
#define SR_AES_REQUEST               0x13c, 0x80, 7

/** Offset for register AES_STATUS
 */
#define RG_AES_STATUS                        (0x13d)
  /** Access parameters for sub-register AES_RY in register @ref RG_AES_STATUS
   */
#define SR_AES_RY                    0x13d, 0x01, 0
  /** Access parameters for sub-register AES_ER in register @ref RG_AES_STATUS
   */
#define SR_AES_ER                    0x13d, 0x80, 7

/** Offset for register AES_STATE
 */
#define RG_AES_STATE                     (0x13e)
  /** Access parameters for sub-register AES_STATE in register @ref RG_AES_STATE
   */
#define SR_AES_STATE                 0x13e, 0xff, 0

/** Offset for register AES_KEY
 */
#define RG_AES_KEY                       (0x13f)
  /** Access parameters for sub-register AES_KEY in register @ref RG_AES_KEY
   */
#define SR_AES_KEY                   0x13f, 0xff, 0

/** Offset for register TRX_STATUS
 */
#define RG_TRX_STATUS                    (0x141)
  /** Access parameters for sub-register TRX_STATUS in register @ref RG_TRX_STATUS
   */
#define SR_TRX_STATUS                0x141, 0x1f, 0
   /** Constant P_ON for sub-register @ref SR_TRX_STATUS
    */

#undef P_ON
#undef BUSY_RX
#undef BUSY_TX
#undef RX_ON
#undef TRX_OFF
#undef PLL_ON
#undef TRX_SLEEP
#undef BUSY_RX_AACK
#undef BUSY_TX_ARET
#undef RX_AACK_ON
#undef TX_ARET_ON
#undef RX_ON_NOCLK
#undef RX_AACK_ON_NOCLK
#undef BUSY_RX_AACK_NOCLK
#undef STATE_TRANSITION_IN_PROGRESS

/** Transceiver states */
typedef enum tal_trx_status_tag
{
    /** Constant P_ON for sub-register SR_TRX_STATUS */
    P_ON                            = (0),

    /** Constant BUSY_RX for sub-register SR_TRX_STATUS */
    BUSY_RX                         = (1),

    /** Constant BUSY_TX for sub-register SR_TRX_STATUS */
    BUSY_TX                         = (2),

    /** Constant RX_ON for sub-register SR_TRX_STATUS */
    RX_ON                           = (6),

    /** Constant TRX_OFF for sub-register SR_TRX_STATUS */
    TRX_OFF                         = (8),

    /** Constant PLL_ON for sub-register SR_TRX_STATUS */
    PLL_ON                          = (9),

    /** Constant SLEEP for sub-register SR_TRX_STATUS */
    TRX_SLEEP                       = (15),

    /** Constant BUSY_RX_AACK for sub-register SR_TRX_STATUS */
    BUSY_RX_AACK                    = (17),

    /** Constant BUSY_TX_ARET for sub-register SR_TRX_STATUS */
    BUSY_TX_ARET                    = (18),

    /** Constant RX_AACK_ON for sub-register SR_TRX_STATUS */
    RX_AACK_ON                      = (22),

    /** Constant TX_ARET_ON for sub-register SR_TRX_STATUS */
    TX_ARET_ON                      = (25),

    /** Constant RX_ON_NOCLK for sub-register SR_TRX_STATUS */
    RX_ON_NOCLK                     = (28),

    /** Constant RX_AACK_ON_NOCLK for sub-register SR_TRX_STATUS */
    RX_AACK_ON_NOCLK                = (29),

    /** Constant BUSY_RX_AACK_NOCLK for sub-register SR_TRX_STATUS */
    BUSY_RX_AACK_NOCLK              = (30),

    /** Constant STATE_TRANSITION_IN_PROGRESS for sub-register SR_TRX_STATUS */
    STATE_TRANSITION_IN_PROGRESS    = (31)
} SHORTENUM tal_trx_status_t;

#define SR_TST_STATUS                0x141, 0x20, 5
#define TST_STATUS_TEST_MODE_IS_DISABLED    (0)
#define TST_STATUS_TEST_MODE_IS_ACTIVE      (1)
  /** Access parameters for sub-register CCA_STATUS in register @ref RG_TRX_STATUS
   */
#define SR_CCA_STATUS                0x141, 0x40, 6
   /** Constant CCA_STATUS_CHANNEL_IS_BUSY for sub-register @ref SR_CCA_STATUS
    */
#define CCA_STATUS_CHANNEL_IS_BUSY (0)
   /** Constant CCA_STATUS_CHANNEL_IS_IDLE for sub-register @ref SR_CCA_STATUS
    */
#define CCA_STATUS_CHANNEL_IS_IDLE (1)
  /** Access parameters for sub-register CCA_DONE in register @ref RG_TRX_STATUS
   */
#define SR_CCA_DONE                  0x141, 0x80, 7
   /** Constant CCA_CALCULATION_NOT_FINISHED for sub-register @ref SR_CCA_DONE
    */
#define CCA_CALCULATION_NOT_FINISHED (0)
   /** Constant CCA_CALCULATION_DONE for sub-register @ref SR_CCA_DONE
    */
#define CCA_CALCULATION_DONE     (1)

/** Offset for register TRX_STATE
 */
#define RG_TRX_STATE                     (0x142)
  /** Access parameters for sub-register TRX_CMD in register @ref RG_TRX_STATE
   */
#define SR_TRX_CMD                   0x142, 0x1f, 0

#undef CMD_NOP
#undef CMD_TX_START
#undef CMD_FORCE_TRX_OFF
#undef CMD_FORCE_PLL_ON
#undef CMD_RX_ON
#undef CMD_TRX_OFF
#undef CMD_PLL_ON
#undef CMD_SLEEP
#undef CMD_RX_AACK_ON
#undef CMD_TX_ARET_ON

/** Transceiver commands */
typedef enum trx_cmd_tag
{
    /** Constant CMD_NOP for sub-register SR_TRX_CMD */
    CMD_NOP                         = (0),

    /** Constant CMD_TX_START for sub-register SR_TRX_CMD */
    CMD_TX_START                    = (2),

    /** Constant CMD_FORCE_TRX_OFF for sub-register SR_TRX_CMD */
    CMD_FORCE_TRX_OFF               = (3),

    /** Constant CMD_FORCE_PLL_ON for sub-register SR_TRX_CMD */
    CMD_FORCE_PLL_ON                = (4),

    /** Constant CMD_RX_ON for sub-register SR_TRX_CMD */
    CMD_RX_ON                       = (6),

    /** Constant CMD_TRX_OFF for sub-register SR_TRX_CMD */
    CMD_TRX_OFF                     = (8),

    /** Constant CMD_PLL_ON for sub-register SR_TRX_CMD */
    CMD_PLL_ON                      = (9),

    /** Software implemented command */
    CMD_SLEEP                       = (15),

    /** Constant CMD_RX_AACK_ON for sub-register SR_TRX_CMD */
    CMD_RX_AACK_ON                  = (22),

    /** Constant CMD_TX_ARET_ON for sub-register SR_TRX_CMD */
    CMD_TX_ARET_ON                  = (25)
} SHORTENUM trx_cmd_t;

#define SR_TRAC_STATUS               0x142, 0xe0, 5

#undef TRAC_SUCCESS
#undef TRAC_SUCCESS_DATA_PENDING
#undef TRAC_WAIT_FOR_ACK
#undef TRAC_CHANNEL_ACCESS_FAILURE
#undef TRAC_NO_ACK
#undef TRAC_INVALID

/** Transceiver's trac status */
typedef enum trx_trac_status_tag
{
    TRAC_SUCCESS = 0,
    TRAC_SUCCESS_DATA_PENDING = 1,
    TRAC_WAIT_FOR_ACK = 2,
    TRAC_CHANNEL_ACCESS_FAILURE = 3,
    TRAC_NO_ACK = 5,
    TRAC_INVALID = 7
} SHORTENUM trx_trac_status_t;

/** Offset for register TRX_CTRL_0
 */
#define RG_TRX_CTRL_0                    (0x143)
  /** Access parameters for sub-register CLKM_CTRL in register @ref RG_TRX_CTRL_0
   */
#define SR_CLKM_CTRL                 0x143, 0x07, 0
   /** Constant CLKM_NO_CLOCK for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_NO_CLOCK            (0)
   /** Constant CLKM_1MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_1MHz                (1)
   /** Constant CLKM_2MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_2MHz                (2)
   /** Constant CLKM_4MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_4MHz                (3)
   /** Constant CLKM_8MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_8MHz                (4)
   /** Constant CLKM_16MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_16MHz               (5)
   /** Constant CLKM_1_4MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_1_4MHz              (6)
   /** Constant CLKM_1_16MHz for sub-register @ref SR_CLKM_CTRL
    */
#define CLKM_1_16MHz             (7)
  /** Access parameters for sub-register CLKM_SHA_SEL in register @ref RG_TRX_CTRL_0
   */
#define SR_CLKM_SHA_SEL              0x143, 0x08, 3
   /** Constant CLKM_SHA_DISABLE for sub-register @ref SR_CLKM_SHA_SEL
    */
#define CLKM_SHA_DISABLE         (0)
   /** Constant CLKM_SHA_ENABLE for sub-register @ref SR_CLKM_SHA_SEL
    */
#define CLKM_SHA_ENABLE          (1)
  /** Access parameters for sub-register PAD_IO_CLKM in register @ref RG_TRX_CTRL_0
   */
#define SR_PAD_IO_CLKM               0x143, 0x30, 4
   /** Constant PAD_CLKM_2mA for sub-register @ref SR_PAD_IO_CLKM
    */
#define PAD_CLKM_2mA             (0)
   /** Constant PAD_CLKM_4mA for sub-register @ref SR_PAD_IO_CLKM
    */
#define PAD_CLKM_4mA             (1)
   /** Constant PAD_CLKM_6mA for sub-register @ref SR_PAD_IO_CLKM
    */
#define PAD_CLKM_6mA             (2)
   /** Constant PAD_CLKM_8mA for sub-register @ref SR_PAD_IO_CLKM
    */
#define PAD_CLKM_8mA             (3)
  /** Access parameters for sub-register PAD_IO in register @ref RG_TRX_CTRL_0
   */
#define SR_PAD_IO                    0x143, 0xc0, 6
   /** Constant PAD_IO_2mA for sub-register @ref SR_PAD_IO
    */
#define PAD_IO_2mA               (0)
   /** Constant PAD_IO_4mA for sub-register @ref SR_PAD_IO
    */
#define PAD_IO_4mA               (1)
   /** Constant PAD_IO_6mA for sub-register @ref SR_PAD_IO
    */
#define PAD_IO_6mA               (2)
   /** Constant PAD_IO_8mA for sub-register @ref SR_PAD_IO
    */
#define PAD_IO_8mA               (3)

/** Offset for register TRX_CTRL_1
 */
#define RG_TRX_CTRL_1                    (0x144)
  /** Access parameters for sub-register IRQ_POLARITY in register @ref RG_TRX_CTRL_1
   */
#define SR_IRQ_POLARITY              0x144, 0x01, 0
   /** Constant IRQ_POL_HIGH_ACTIVE_IRQ for sub-register @ref SR_IRQ_POLARITY
    */
#define IRQ_POL_HIGH_ACTIVE_IRQ  (0)
   /** Constant IRQ_POL_LOW_ACTIVE_IRQ for sub-register @ref SR_IRQ_POLARITY
    */
#define IRQ_POL_LOW_ACTIVE_IRQ   (1)
  /** Access parameters for sub-register IRQ_MASK_MODE in register @ref RG_TRX_CTRL_1
   */
#define SR_IRQ_MASK_MODE             0x144, 0x02, 1
   /** Constant IRQ_MASK_MODE_OFF for sub-register @ref SR_IRQ_MASK_MODE
    */
#define IRQ_MASK_MODE_OFF        (0)
   /** Constant IRQ_MASK_MODE_ON for sub-register @ref SR_IRQ_MASK_MODE
    */
#define IRQ_MASK_MODE_ON         (1)
  /** Access parameters for sub-register SPI_CMD_MODE in register @ref RG_TRX_CTRL_1
   */
#define SR_SPI_CMD_MODE              0x144, 0x0c, 2
   /** Constant SPI_CMD_MODE_DEFAULT for sub-register @ref SR_SPI_CMD_MODE
    */
#define SPI_CMD_MODE_DEFAULT     (0)
   /** Constant SPI_CMD_MODE_MONITOR_TRX_STATUS for sub-register @ref SR_SPI_CMD_MODE
    */
#define SPI_CMD_MODE_MONITOR_TRX_STATUS (1)
   /** Constant SPI_CMD_MODE_MONITOR_PHY_RSSI for sub-register @ref SR_SPI_CMD_MODE
    */
#define SPI_CMD_MODE_MONITOR_PHY_RSSI (2)
   /** Constant SPI_CMD_MODE_MONITOR_IRQ_STATUS for sub-register @ref SR_SPI_CMD_MODE
    */
#define SPI_CMD_MODE_MONITOR_IRQ_STATUS (3)
  /** Access parameters for sub-register RX_BL_CTRL in register @ref RG_TRX_CTRL_1
   */
#define SR_RX_BL_CTRL                0x144, 0x10, 4
  /** Access parameters for sub-register TX_AUTO_CRC_ON in register @ref RG_TRX_CTRL_1
   */
#define SR_TX_AUTO_CRC_ON            0x144, 0x20, 5
  /** Access parameters for sub-register IRQ_2_EXT_EN in register @ref RG_TRX_CTRL_1
   */
/** Constant TX_AUTO_CRC_ENABLE for sub-register TX_AUTO_CRC_ON in register RG_TRX_CTRL_1 */
#define TX_AUTO_CRC_DISABLE   (0)

/** Constant TX_AUTO_CRC_ENABLE for sub-register TX_AUTO_CRC_ON in register RG_TRX_CTRL_1 */
#define TX_AUTO_CRC_ENABLE   (1)

#define SR_IRQ_2_EXT_EN              0x144, 0x40, 6
  /** Access parameters for sub-register PA_EXT_EN in register @ref RG_TRX_CTRL_1
   */
#define SR_PA_EXT_EN                 0x144, 0x80, 7

/** Offset for register PHY_TX_PWR
 */
#define RG_PHY_TX_PWR                    (0x145)
  /** Access parameters for sub-register TX_PWR in register @ref RG_PHY_TX_PWR
   */
#define SR_TX_PWR                    0x145, 0x0f, 0
   /** Constant _3_2dBm for sub-register @ref SR_TX_PWR
    */
#define _3_2dBm                  (0)
   /** Constant _2_8dBm for sub-register @ref SR_TX_PWR
    */
#define _2_8dBm                  (1)
   /** Constant _2_3dBm for sub-register @ref SR_TX_PWR
    */
#define _2_3dBm                  (2)
   /** Constant _1_8dBm for sub-register @ref SR_TX_PWR
    */
#define _1_8dBm                  (3)
   /** Constant _1_3dBm for sub-register @ref SR_TX_PWR
    */
#define _1_3dBm                  (4)
   /** Constant _0_7dBm for sub-register @ref SR_TX_PWR
    */
#define _0_7dBm                  (5)
   /** Constant _0_0dBm for sub-register @ref SR_TX_PWR
    */
#define _0_0dBm                  (6)
   /** Constant __1dBm for sub-register @ref SR_TX_PWR
    */
#define __1dBm                   (7)
   /** Constant __2dBm for sub-register @ref SR_TX_PWR
    */
#define __2dBm                   (8)
   /** Constant __3dBm for sub-register @ref SR_TX_PWR
    */
#define __3dBm                   (9)
   /** Constant __4dBm for sub-register @ref SR_TX_PWR
    */
#define __4dBm                   (10)
   /** Constant __5dBm for sub-register @ref SR_TX_PWR
    */
#define __5dBm                   (11)
   /** Constant __7dBm for sub-register @ref SR_TX_PWR
    */
#define __7dBm                   (12)
   /** Constant __9dBm for sub-register @ref SR_TX_PWR
    */
#define __9dBm                   (13)
   /** Constant __12dBm for sub-register @ref SR_TX_PWR
    */
#define __12dBm                  (14)
   /** Constant __17dBm for sub-register @ref SR_TX_PWR
    */
#define __17dBm                  (15)
  /** Access parameters for sub-register PA_LT in register @ref RG_PHY_TX_PWR
   */
#define SR_PA_LT                     0x145, 0x30, 4
   /** Constant PA_LT_2us for sub-register @ref SR_PA_LT
    */
#define PA_LT_2us                (0)
   /** Constant PA_LT_4us for sub-register @ref SR_PA_LT
    */
#define PA_LT_4us                (1)
   /** Constant PA_LT_6us for sub-register @ref SR_PA_LT
    */
#define PA_LT_6us                (2)
   /** Constant PA_LT_8us for sub-register @ref SR_PA_LT
    */
#define PA_LT_8us                (3)
  /** Access parameters for sub-register PA_BUF_LT in register @ref RG_PHY_TX_PWR
   */
#define SR_PA_BUF_LT                 0x145, 0xc0, 6
   /** Constant PA_BUF_LT_0us for sub-register @ref SR_PA_BUF_LT
    */
#define PA_BUF_LT_0us            (0)
   /** Constant PA_BUF_LT_2us for sub-register @ref SR_PA_BUF_LT
    */
#define PA_BUF_LT_2us            (1)
   /** Constant PA_BUF_LT_4us for sub-register @ref SR_PA_BUF_LT
    */
#define PA_BUF_LT_4us            (2)
   /** Constant PA_BUF_LT_6us for sub-register @ref SR_PA_BUF_LT
    */
#define PA_BUF_LT_6us            (3)

/** Offset for register PHY_RSSI
 */
#define RG_PHY_RSSI                      (0x146)
  /** Access parameters for sub-register RSSI in register @ref RG_PHY_RSSI
   */
#define SR_RSSI                      0x146, 0x1f, 0
  /** Access parameters for sub-register RND_VALUE in register @ref RG_PHY_RSSI
   */
#define SR_RND_VALUE                 0x146, 0x60, 5
  /** Access parameters for sub-register RX_CRC_VALID in register @ref RG_PHY_RSSI
   */
#define SR_RX_CRC_VALID              0x146, 0x80, 7
   /** Constant CRC16_NOT_VALID for sub-register @ref SR_RX_CRC_VALID
    */
#define CRC16_NOT_VALID          (0)
   /** Constant CRC16_VALID for sub-register @ref SR_RX_CRC_VALID
    */
#define CRC16_VALID              (1)

/** Offset for register PHY_ED_LEVEL
 */
#define RG_PHY_ED_LEVEL                  (0x147)

/** Offset for register PHY_CC_CCA
 */
#define RG_PHY_CC_CCA                    (0x148)
  /** Access parameters for sub-register CHANNEL in register @ref RG_PHY_CC_CCA
   */
#define SR_CHANNEL                   0x148, 0x1f, 0
   /** Constant _2405MHz for sub-register @ref SR_CHANNEL
    */
#define _2405MHz                 (11)
   /** Constant _2410MHz for sub-register @ref SR_CHANNEL
    */
#define _2410MHz                 (12)
   /** Constant _2415MHz for sub-register @ref SR_CHANNEL
    */
#define _2415MHz                 (13)
   /** Constant _2420MHz for sub-register @ref SR_CHANNEL
    */
#define _2420MHz                 (14)
   /** Constant _2425MHz for sub-register @ref SR_CHANNEL
    */
#define _2425MHz                 (15)
   /** Constant _2430MHz for sub-register @ref SR_CHANNEL
    */
#define _2430MHz                 (16)
   /** Constant _2435MHz for sub-register @ref SR_CHANNEL
    */
#define _2435MHz                 (17)
   /** Constant _2440MHz for sub-register @ref SR_CHANNEL
    */
#define _2440MHz                 (18)
   /** Constant _2445MHz for sub-register @ref SR_CHANNEL
    */
#define _2445MHz                 (19)
   /** Constant _2450MHz for sub-register @ref SR_CHANNEL
    */
#define _2450MHz                 (20)
   /** Constant _2455MHz for sub-register @ref SR_CHANNEL
    */
#define _2455MHz                 (21)
   /** Constant _2460MHz for sub-register @ref SR_CHANNEL
    */
#define _2460MHz                 (22)
   /** Constant _2465MHz for sub-register @ref SR_CHANNEL
    */
#define _2465MHz                 (23)
   /** Constant _2470MHz for sub-register @ref SR_CHANNEL
    */
#define _2470MHz                 (24)
   /** Constant _2475MHz for sub-register @ref SR_CHANNEL
    */
#define _2475MHz                 (25)
   /** Constant _2480MHz for sub-register @ref SR_CHANNEL
    */
#define _2480MHz                 (26)
  /** Access parameters for sub-register CCA_MODE in register @ref RG_PHY_CC_CCA
   */
#define SR_CCA_MODE                  0x148, 0x60, 5
   /** Constant CCA_Mode_0 for sub-register @ref SR_CCA_MODE
    */
#define CCA_Mode_0               (0)
   /** Constant CCA_Mode_1 for sub-register @ref SR_CCA_MODE
    */
#define CCA_Mode_1               (1)
   /** Constant CCA_Mode_2 for sub-register @ref SR_CCA_MODE
    */
#define CCA_Mode_2               (2)
   /** Constant CCA_Mode_3 for sub-register @ref SR_CCA_MODE
    */
#define CCA_Mode_3               (3)
  /** Access parameters for sub-register CCA_REQUEST in register @ref RG_PHY_CC_CCA
   */
#define SR_CCA_REQUEST               0x148, 0x80, 7

/** Constant CCA_START for sub-register CCA_REQUEST in register RG_PHY_CC_CCA */
#define CCA_START               (1)


/** Offset for register CCA_THRES
 */
#define RG_CCA_THRES                     (0x149)
  /** Access parameters for sub-register CCA_ED_THRES in register @ref RG_CCA_THRES
   */
#define SR_CCA_ED_THRES              0x149, 0x0f, 0
   /** Constant RSSI_BASE_VAL_00dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_00dB       (0)
   /** Constant RSSI_BASE_VAL_02dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_02dB       (1)
   /** Constant RSSI_BASE_VAL_04dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_04dB       (2)
   /** Constant RSSI_BASE_VAL_06dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_06dB       (3)
   /** Constant RSSI_BASE_VAL_08dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_08dB       (4)
   /** Constant RSSI_BASE_VAL_10dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_10dB       (5)
   /** Constant RSSI_BASE_VAL_12dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_12dB       (6)
   /** Constant RSSI_BASE_VAL_14dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_14dB       (7)
   /** Constant RSSI_BASE_VAL_16dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_16dB       (8)
   /** Constant RSSI_BASE_VAL_18dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_18dB       (9)
   /** Constant RSSI_BASE_VAL_20dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_20dB       (10)
   /** Constant RSSI_BASE_VAL_22dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_22dB       (11)
   /** Constant RSSI_BASE_VAL_24dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_24dB       (12)
   /** Constant RSSI_BASE_VAL_26dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_26dB       (13)
   /** Constant RSSI_BASE_VAL_28dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_28dB       (14)
   /** Constant RSSI_BASE_VAL_30dB for sub-register @ref SR_CCA_ED_THRES
    */
#define RSSI_BASE_VAL_30dB       (15)
  /** Access parameters for sub-register CCA_CS_THRES in register @ref RG_CCA_THRES
   */
#define SR_CCA_CS_THRES              0x149, 0xf0, 4

/** Offset for register RX_CTRL
 */
#define RG_RX_CTRL                       (0x14a)
  /** Access parameters for sub-register PDT_THRES in register @ref RG_RX_CTRL
   */
#define SR_PDT_THRES                 0x14a, 0x0f, 0
  /** Access parameters for sub-register SOFT_MODE in register @ref RG_RX_CTRL
   */
/** Constant THRES_ANT_DIV_ENABLE for sub-register SR_PDT_THRES in register RG_RX_CTRL */
#define THRES_ANT_DIV_ENABLE        (0x3)

/** Constant THRES_ANT_DIV_DISABLED for sub-register SR_PDT_THRES in register RG_RX_CTRL */
#define THRES_ANT_DIV_DISABLED       (0x7)

#define SR_SOFT_MODE                 0x14a, 0x10, 4
   /** Constant SOFT_MODE_DISABLE for sub-register @ref SR_SOFT_MODE
    */
#define SOFT_MODE_DISABLE        (0)
   /** Constant SOFT_MODE_ENABLE for sub-register @ref SR_SOFT_MODE
    */
#define SOFT_MODE_ENABLE         (1)
  /** Access parameters for sub-register ACR_MODE in register @ref RG_RX_CTRL
   */
#define SR_ACR_MODE                  0x14a, 0x20, 5
   /** Constant ACR_DISABLE for sub-register @ref SR_ACR_MODE
    */
#define ACR_DISABLE              (0)
   /** Constant ACR_ENABLE for sub-register @ref SR_ACR_MODE
    */
#define ACR_ENABLE               (1)
  /** Access parameters for sub-register SDM_MODE in register @ref RG_RX_CTRL
   */
#define SR_SDM_MODE                  0x14a, 0xc0, 6
   /** Constant X_SELECT_SDM1_DCU_ON for sub-register @ref SR_SDM_MODE
    */
#define X_SELECT_SDM1_DCU_ON     (0)
   /** Constant X_SELECT_SDM2_DCU_OFF for sub-register @ref SR_SDM_MODE
    */
#define X_SELECT_SDM2_DCU_OFF    (1)
   /** Constant X_SELECT_SDM2_DCU_ON for sub-register @ref SR_SDM_MODE
    */
#define X_SELECT_SDM2_DCU_ON     (2)
   /** Constant X_SELECT_SDM3_DCU_OFF for sub-register @ref SR_SDM_MODE
    */
#define X_SELECT_SDM3_DCU_OFF    (3)

/** Offset for register SFD_VALUE
 */
#define RG_SFD_VALUE                     (0x14b)
  /** Access parameters for sub-register SFD_VALUE in register @ref RG_SFD_VALUE
   */
#define SR_SFD_VALUE                 0x14b, 0xff, 0

/** Offset for register TRX_CTRL_2
 */
#define RG_TRX_CTRL_2                    (0x14c)
  /** Access parameters for sub-register OQPSK_DATA_RATE in register @ref RG_TRX_CTRL_2
   */
#define SR_OQPSK_DATA_RATE           0x14c, 0x03, 0
   /** Constant ALTRATE_250KBPS for sub-register @ref SR_OQPSK_DATA_RATE
    */
#define ALTRATE_250KBPS         (0)
   /** Constant ALTRATE_500KBPS for sub-register @ref SR_OQPSK_DATA_RATE
    */
#define ALTRATE_500KBPS         (1)
   /** Constant ALTRATE_1MBPS for sub-register @ref SR_OQPSK_DATA_RATE
    */
#define ALTRATE_1MBPS           (2)
   /** Constant ALTRATE_2MBPS for sub-register @ref SR_OQPSK_DATA_RATE
    */
#define ALTRATE_2MBPS           (3)
  /** Access parameters for sub-register RX_SAFE_MODE in register @ref RG_TRX_CTRL_2
   */
#define SR_RX_SAFE_MODE              0x14c, 0x80, 7

/** Constant RX_SAFE_MODE_DISABLE for sub-register RX_SAFE_MODE in register RG_TRX_CTRL_2 */
#define RX_SAFE_MODE_DISABLE     (0)

/** Constant RX_SAFE_MODE_ENABLE for sub-register RX_SAFE_MODE in register RG_TRX_CTRL_2 */
#define RX_SAFE_MODE_ENABLE     (1)

/** Offset for register ANT_DIV
 */
#define RG_ANT_DIV                       (0x14d)
  /** Access parameters for sub-register ANT_CTRL in register @ref RG_ANT_DIV
   */
#define SR_ANT_CTRL                  0x14d, 0x03, 0
   /** Constant NOT_ALLOWED for sub-register @ref SR_ANT_CTRL
    */
#define NOT_ALLOWED              (0)
   /** Constant RFN_ANT_EXT_SW_EN_0_ for sub-register @ref SR_ANT_CTRL
    */
#define RFN_ANT_EXT_SW_EN_0_     (1)
   /** Constant DIG1_2_ANT_EXT_SW_EN_1_ for sub-register @ref SR_ANT_CTRL
    */
#define DIG1_2_ANT_EXT_SW_EN_1_  (1)
   /** Constant RFP_ANT_EXT_SW_EN_0_ for sub-register @ref SR_ANT_CTRL
    */
#define RFP_ANT_EXT_SW_EN_0_     (2)
   /** Constant ANT_DIV_ANT_CTRL_DIG1_2_ANT_EXT_SW_EN_1_0 for sub-register @ref SR_ANT_CTRL
    */
#define ANT_DIV_ANT_CTRL_DIG1_2_ANT_EXT_SW_EN_1_0 (2)
   /** Constant RFN_RFP_ANT_EXT_SW_EN_0_ for sub-register @ref SR_ANT_CTRL
    */
#define RFN_RFP_ANT_EXT_SW_EN_0_ (3)
   /** Constant NOT_ALLOWED_ANT_EXT_SW_EN_1_ for sub-register @ref SR_ANT_CTRL
    */
#define NOT_ALLOWED_ANT_EXT_SW_EN_1_ (3)
  /** Access parameters for sub-register ANT_EXT_SW_EN in register @ref RG_ANT_DIV
   */
/** Constant ENABLE_ANTENNA_0 for sub-register SR_ANT_CTRL in register RG_ANT_DIV */
#define ENABLE_ANTENNA_0        (2)

/** Constant ENABLE_ANTENNA_1 for sub-register SR_ANT_CTRL in register RG_ANT_DIV */
#define ENABLE_ANTENNA_1        (1)

/** Constant ENABLE_ANTENNA_X for sub-register SR_ANT_CTRL in register RG_ANT_DIV */
#define ENABLE_ANTENNA_X        (3)

#define SR_ANT_EXT_SW_EN             0x14d, 0x04, 2
   /** Constant ANT_EXT_SW_INTERNAL_SWITCH for sub-register @ref SR_ANT_EXT_SW_EN
    */
#define ANT_EXT_SW_INTERNAL_SWITCH (0)
   /** Constant ANT_EXT_SW_EXTERNAL_SWITCH for sub-register @ref SR_ANT_EXT_SW_EN
    */
#define ANT_EXT_SW_EXTERNAL_SWITCH (1)
  /** Access parameters for sub-register ANT_DIV_EN in register @ref RG_ANT_DIV
   */
/** Constant ANT_EXT_SW_SWITCH_DISABLE for sub-register SR_ANT_EXT_SW_EN */
#define ANT_EXT_SW_SWITCH_DISABLE   (0)

/** Constant ANT_EXT_SW_SWITCH_ENABLE for sub-register SR_ANT_EXT_SW_EN */
#define ANT_EXT_SW_SWITCH_ENABLE   (1)

#define SR_ANT_DIV_EN                0x14d, 0x08, 3
   /** Constant ANT_DIV_DISABLE for sub-register @ref SR_ANT_DIV_EN
    */
#define ANT_DIV_DISABLE          (0)
   /** Constant ANT_DIV_ENABLE for sub-register @ref SR_ANT_DIV_EN
    */
#define ANT_DIV_ENABLE           (1)
  /** Access parameters for sub-register ANT_SEL in register @ref RG_ANT_DIV
   */
#define SR_ANT_SEL                   0x14d, 0x80, 7
   /** Constant ANT_SEL_ANTENNA_0 for sub-register @ref SR_ANT_SEL
    */
#define ANT_SEL_ANTENNA_0        (0)
   /** Constant ANT_SEL_ANTENNA_1 for sub-register @ref SR_ANT_SEL
    */
#define ANT_SEL_ANTENNA_1        (1)

/** Offset for register IRQ_MASK
 */
#define RG_IRQ_MASK                      (0x14e)
  /** Access parameters for sub-register PLL_LOCK_EN in register @ref RG_IRQ_MASK
   */
#define SR_PLL_LOCK_EN               0x14e, 0x01, 0
  /** Access parameters for sub-register PLL_UNLOCK_EN in register @ref RG_IRQ_MASK
   */
#define SR_PLL_UNLOCK_EN             0x14e, 0x02, 1
  /** Access parameters for sub-register RX_START_EN in register @ref RG_IRQ_MASK
   */
#define SR_RX_START_EN               0x14e, 0x04, 2
  /** Access parameters for sub-register RX_END_EN in register @ref RG_IRQ_MASK
   */
#define SR_RX_END_EN                 0x14e, 0x08, 3
  /** Access parameters for sub-register CCA_ED_READY_EN in register @ref RG_IRQ_MASK
   */
#define SR_CCA_ED_READY_EN           0x14e, 0x10, 4
  /** Access parameters for sub-register AMI_EN in register @ref RG_IRQ_MASK
   */
#define SR_AMI_EN                    0x14e, 0x20, 5
  /** Access parameters for sub-register TX_END_EN in register @ref RG_IRQ_MASK
   */
#define SR_TX_END_EN                 0x14e, 0x40, 6
  /** Access parameters for sub-register AWAKE_EN in register @ref RG_IRQ_MASK
   */
#define SR_AWAKE_EN                  0x14e, 0x80, 7

/** Offset for register IRQ_STATUS
 */
#define RG_IRQ_STATUS                    (0x14f)
  /** Access parameters for sub-register PLL_LOCK in register @ref RG_IRQ_STATUS
   */
#define SR_PLL_LOCK                  0x14f, 0x01, 0
  /** Access parameters for sub-register PLL_UNLOCK in register @ref RG_IRQ_STATUS
   */
#define SR_PLL_UNLOCK                0x14f, 0x02, 1
  /** Access parameters for sub-register RX_START in register @ref RG_IRQ_STATUS
   */
#define SR_RX_START                  0x14f, 0x04, 2
  /** Access parameters for sub-register RX_END in register @ref RG_IRQ_STATUS
   */
#define SR_RX_END                    0x14f, 0x08, 3
  /** Access parameters for sub-register CCA_ED_READY in register @ref RG_IRQ_STATUS
   */
#define SR_CCA_ED_READY              0x14f, 0x10, 4
  /** Access parameters for sub-register AMI in register @ref RG_IRQ_STATUS
   */
#define SR_AMI                       0x14f, 0x20, 5
  /** Access parameters for sub-register TX_END in register @ref RG_IRQ_STATUS
   */
#define SR_TX_END                    0x14f, 0x40, 6
  /** Access parameters for sub-register AWAKE in register @ref RG_IRQ_STATUS
   */
#define SR_AWAKE                     0x14f, 0x80, 7

/** Offset for register VREG_CTRL
 */
#define RG_VREG_CTRL                     (0x150)
  /** Access parameters for sub-register DVREG_TRIM in register @ref RG_VREG_CTRL
   */
#define SR_DVREG_TRIM                0x150, 0x03, 0
  /** Access parameters for sub-register DVDD_OK in register @ref RG_VREG_CTRL
   */
#define SR_DVDD_OK                   0x150, 0x04, 2
  /** Access parameters for sub-register DVREG_EXT in register @ref RG_VREG_CTRL
   */
#define SR_DVREG_EXT                 0x150, 0x08, 3
  /** Access parameters for sub-register AVREG_TRIM in register @ref RG_VREG_CTRL
   */
#define SR_AVREG_TRIM                0x150, 0x30, 4
  /** Access parameters for sub-register AVDD_OK in register @ref RG_VREG_CTRL
   */
#define SR_AVDD_OK                   0x150, 0x40, 6
  /** Access parameters for sub-register AVREG_EXT in register @ref RG_VREG_CTRL
   */
#define SR_AVREG_EXT                 0x150, 0x80, 7

/** Offset for register BATMON
 */
#define RG_BATMON                        (0x151)
  /** Access parameters for sub-register BATMON_VTH in register @ref RG_BATMON
   */
#define SR_BATMON_VTH                0x151, 0x0f, 0
   /** Constant _2_550V_BATMON_HR_1_1_70V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_550V_BATMON_HR_1_1_70V_BATMON_HR_0_ (0)
   /** Constant _2_625V_BATMON_HR_1_1_75V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_625V_BATMON_HR_1_1_75V_BATMON_HR_0_ (1)
   /** Constant _2_700V_BATMON_HR_1_1_80V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_700V_BATMON_HR_1_1_80V_BATMON_HR_0_ (2)
   /** Constant _2_775V_BATMON_HR_1_1_85V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_775V_BATMON_HR_1_1_85V_BATMON_HR_0_ (3)
   /** Constant _2_850V_BATMON_HR_1_1_90V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_850V_BATMON_HR_1_1_90V_BATMON_HR_0_ (4)
   /** Constant _2_925V_BATMON_HR_1_1_95V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _2_925V_BATMON_HR_1_1_95V_BATMON_HR_0_ (5)
   /** Constant _3_000V_BATMON_HR_1_2_00V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_000V_BATMON_HR_1_2_00V_BATMON_HR_0_ (6)
   /** Constant _3_075V_BATMON_HR_1_2_05V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_075V_BATMON_HR_1_2_05V_BATMON_HR_0_ (7)
   /** Constant _3_150V_BATMON_HR_1_2_10V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_150V_BATMON_HR_1_2_10V_BATMON_HR_0_ (8)
   /** Constant _3_225V_BATMON_HR_1_2_15V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_225V_BATMON_HR_1_2_15V_BATMON_HR_0_ (9)
   /** Constant _3_300V_BATMON_HR_1_2_20V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_300V_BATMON_HR_1_2_20V_BATMON_HR_0_ (10)
   /** Constant _3_375V_BATMON_HR_1_2_25V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_375V_BATMON_HR_1_2_25V_BATMON_HR_0_ (11)
   /** Constant _3_450V_BATMON_HR_1_2_30V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_450V_BATMON_HR_1_2_30V_BATMON_HR_0_ (12)
   /** Constant _3_525V_BATMON_HR_1_2_35V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_525V_BATMON_HR_1_2_35V_BATMON_HR_0_ (13)
   /** Constant _3_600V_BATMON_HR_1_2_40V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_600V_BATMON_HR_1_2_40V_BATMON_HR_0_ (14)
   /** Constant _3_675V_BATMON_HR_1_2_45V_BATMON_HR_0_ for sub-register @ref SR_BATMON_VTH
    */
#define _3_675V_BATMON_HR_1_2_45V_BATMON_HR_0_ (15)
  /** Access parameters for sub-register BATMON_HR in register @ref RG_BATMON
   */
#define SR_BATMON_HR                 0x151, 0x10, 4
   /** Constant HIGH_RANGE_DISABLED for sub-register @ref SR_BATMON_HR
    */
#define HIGH_RANGE_DISABLED      (0)
   /** Constant HIGH_RANGE_ENABLED for sub-register @ref SR_BATMON_HR
    */
#define HIGH_RANGE_ENABLED       (1)
  /** Access parameters for sub-register BATMON_OK in register @ref RG_BATMON
   */
#define SR_BATMON_OK                 0x151, 0x20, 5
   /** Constant BATMON_NOT_VALID for sub-register @ref SR_BATMON_OK
    */
#define BATMON_NOT_VALID         (0)
   /** Constant BATMON_VALID for sub-register @ref SR_BATMON_OK
    */
#define BATMON_VALID             (1)
  /** Access parameters for sub-register BAT_LOW_EN in register @ref RG_BATMON
   */
#define SR_BAT_LOW_EN                0x151, 0x40, 6
   /** Constant BATTERY_IRQ_DISABLED for sub-register @ref SR_BAT_LOW_EN
    */
#define BATTERY_IRQ_DISABLED     (0)
   /** Constant BATTERY_IRQ_ENABLED for sub-register @ref SR_BAT_LOW_EN
    */
#define BATTERY_IRQ_ENABLED      (1)
  /** Access parameters for sub-register BAT_LOW in register @ref RG_BATMON
   */
#define SR_BAT_LOW                   0x151, 0x80, 7
   /** Constant BATTERY_NOT_LOW for sub-register @ref SR_BAT_LOW
    */
#define BATTERY_NOT_LOW          (0)
   /** Constant BATTERY_LOW for sub-register @ref SR_BAT_LOW
    */
#define BATTERY_LOW              (1)

/** Offset for register XOSC_CTRL
 */
#define RG_XOSC_CTRL                     (0x152)
  /** Access parameters for sub-register XTAL_TRIM in register @ref RG_XOSC_CTRL
   */
#define SR_XTAL_TRIM                 0x152, 0x0f, 0
   /** Constant _0_0pF for sub-register @ref SR_XTAL_TRIM
    */
#define _0_0pF                   (0)
   /** Constant _0_3pF for sub-register @ref SR_XTAL_TRIM
    */
#define _0_3pF                   (1)
   /** Constant _0_6pF for sub-register @ref SR_XTAL_TRIM
    */
#define _0_6pF                   (2)
   /** Constant _0_9pF for sub-register @ref SR_XTAL_TRIM
    */
#define _0_9pF                   (3)
   /** Constant _1_2pF for sub-register @ref SR_XTAL_TRIM
    */
#define _1_2pF                   (4)
   /** Constant _1_5pF for sub-register @ref SR_XTAL_TRIM
    */
#define _1_5pF                   (5)
   /** Constant _1_8pF for sub-register @ref SR_XTAL_TRIM
    */
#define _1_8pF                   (6)
   /** Constant _2_1pF for sub-register @ref SR_XTAL_TRIM
    */
#define _2_1pF                   (7)
   /** Constant _2_4pF for sub-register @ref SR_XTAL_TRIM
    */
#define _2_4pF                   (8)
   /** Constant _2_7pF for sub-register @ref SR_XTAL_TRIM
    */
#define _2_7pF                   (9)
   /** Constant _3_0pF for sub-register @ref SR_XTAL_TRIM
    */
#define _3_0pF                   (10)
   /** Constant _3_3pF for sub-register @ref SR_XTAL_TRIM
    */
#define _3_3pF                   (11)
   /** Constant _3_6pF for sub-register @ref SR_XTAL_TRIM
    */
#define _3_6pF                   (12)
   /** Constant _3_9pF for sub-register @ref SR_XTAL_TRIM
    */
#define _3_9pF                   (13)
   /** Constant _4_2pF for sub-register @ref SR_XTAL_TRIM
    */
#define _4_2pF                   (14)
   /** Constant _4_5pF for sub-register @ref SR_XTAL_TRIM
    */
#define _4_5pF                   (15)
  /** Access parameters for sub-register XTAL_MODE in register @ref RG_XOSC_CTRL
   */
#define SR_XTAL_MODE                 0x152, 0xf0, 4

/** Offset for register RX_SYN
 */
#define RG_RX_SYN                        (0x155)
  /** Access parameters for sub-register RX_PDT_LEVEL in register @ref RG_RX_SYN
   */
#define SR_RX_PDT_LEVEL              0x155, 0x0f, 0
  /** Access parameters for sub-register RX_PDT_DIS in register @ref RG_RX_SYN
   */
#define SR_RX_PDT_DIS                0x155, 0x80, 7

/** Minimum RSSI sensitivity value in dBm, which is equivalent to the value 0 in sub register SR_RSSI. */
#define RSSI_BASE_VAL_DBM   (-90)

   /** Constant RX_DISABLE for sub-register @ref SR_RX_PDT_DIS
    */
#define RX_DISABLE (1)
   /** Constant RX_ENABLE for sub-register @ref SR_RX_PDT_DIS
    */
#define RX_ENABLE (0)

/** Offset for register XAH_CTRL_1
 */
#define RG_XAH_CTRL_1                    (0x157)
  /** Access parameters for sub-register AACK_PROM_MODE in register @ref RG_XAH_CTRL_1
   */
#define SR_AACK_PROM_MODE            0x157, 0x02, 1
  /** Access parameters for sub-register AACK_ACK_TIME in register @ref RG_XAH_CTRL_1
   */
#define SR_AACK_ACK_TIME             0x157, 0x04, 2
   /** Constant AACK_ACK_TIME_12_SYMBOLS for sub-register @ref SR_AACK_ACK_TIME
    */
#define AACK_ACK_TIME_12_SYMBOLS (0)
   /** Constant AACK_ACK_TIME_2_SYMBOLS for sub-register @ref SR_AACK_ACK_TIME
    */
#define AACK_ACK_TIME_2_SYMBOLS  (1)
  /** Access parameters for sub-register AACK_UPLD_RES_FT in register @ref RG_XAH_CTRL_1
   */
#define SR_AACK_UPLD_RES_FT          0x157, 0x10, 4
  /** Access parameters for sub-register AACK_FLTR_RES_FT in register @ref RG_XAH_CTRL_1
   */
#define SR_AACK_FLTR_RES_FT          0x157, 0x20, 5

/** Offset for register FTN_CTRL
 */
#define RG_FTN_CTRL                      (0x158)
  /** Access parameters for sub-register FTNV in register @ref RG_FTN_CTRL
   */
#define SR_FTNV                      0x158, 0x3f, 0
  /** Access parameters for sub-register FTN_ROUND in register @ref RG_FTN_CTRL
   */
#define SR_FTN_ROUND                 0x158, 0x40, 6
  /** Access parameters for sub-register FTN_START in register @ref RG_FTN_CTRL
   */
#define SR_FTN_START                 0x158, 0x80, 7

/** Offset for register RF_CTRL
 */
#define RG_RF_CTRL                       (0x159)
  /** Access parameters for sub-register LNA_VF1 in register @ref RG_RF_CTRL
   */
#define SR_LNA_VF1                   0x159, 0x03, 0
  /** Access parameters for sub-register LNA_VF2 in register @ref RG_RF_CTRL
   */
#define SR_LNA_VF2                   0x159, 0x0c, 2
  /** Access parameters for sub-register PA_BUF_VF in register @ref RG_RF_CTRL
   */
#define SR_PA_BUF_VF                 0x159, 0x30, 4
  /** Access parameters for sub-register PLL_VCOB_VF in register @ref RG_RF_CTRL
   */
#define SR_PLL_VCOB_VF               0x159, 0xc0, 6

/** Offset for register PLL_CF
 */
#define RG_PLL_CF                        (0x15a)
  /** Access parameters for sub-register PLL_CF in register @ref RG_PLL_CF
   */
#define SR_PLL_CF                    0x15a, 0x0f, 0
  /** Access parameters for sub-register PLL_VMOD_TUNE in register @ref RG_PLL_CF
   */
#define SR_PLL_VMOD_TUNE             0x15a, 0x30, 4
  /** Access parameters for sub-register EN_PLL_CF in register @ref RG_PLL_CF
   */
#define SR_EN_PLL_CF                 0x15a, 0x40, 6
  /** Access parameters for sub-register PLL_CF_START in register @ref RG_PLL_CF
   */
#define SR_PLL_CF_START              0x15a, 0x80, 7

/** Offset for register PLL_DCU
 */
#define RG_PLL_DCU                       (0x15b)
  /** Access parameters for sub-register PLL_DCUW in register @ref RG_PLL_DCU
   */
#define SR_PLL_DCUW                  0x15b, 0x3f, 0
  /** Access parameters for sub-register PLL_DCU_START in register @ref RG_PLL_DCU
   */
#define SR_PLL_DCU_START             0x15b, 0x80, 7

/** Offset for register PART_NUM
 */
#define RG_PART_NUM                      (0x15c)
  /** Access parameters for sub-register PART_NUM in register @ref RG_PART_NUM
   */
#define SR_PART_NUM                  0x15c, 0xff, 0
  /** Constant ATmega128RFA1 for sub-register SR_PART_NUM */
#define ATMEGA128RFA1_PART_NUM           (131)

/** Offset for register VERSION_NUM
 */
#define RG_VERSION_NUM                   (0x15d)
  /** Access parameters for sub-register VERSION_NUM in register @ref RG_VERSION_NUM
   */
#define SR_VERSION_NUM               0x15d, 0xff, 0

/** Offset for register MAN_ID_0
 */
#define RG_MAN_ID_0                      (0x15e)
  /** Access parameters for sub-register MAN_ID_0 in register @ref RG_MAN_ID_0
   */
#define SR_MAN_ID_0                  0x15e, 0xff, 0

/** Offset for register MAN_ID_1
 */
#define RG_MAN_ID_1                      (0x15f)
  /** Access parameters for sub-register MAN_ID_1 in register @ref RG_MAN_ID_1
   */
#define SR_MAN_ID_1                  0x15f, 0xff, 0

/** Offset for register SHORT_ADDR_0
 */
#define RG_SHORT_ADDR_0                  (0x160)
  /** Access parameters for sub-register SHORT_ADDR_0 in register @ref RG_SHORT_ADDR_0
   */
#define SR_SHORT_ADDR_0              0x160, 0xff, 0

/** Offset for register SHORT_ADDR_1
 */
#define RG_SHORT_ADDR_1                  (0x161)
  /** Access parameters for sub-register SHORT_ADDR_1 in register @ref RG_SHORT_ADDR_1
   */
#define SR_SHORT_ADDR_1              0x161, 0xff, 0

/** Offset for register PAN_ID_0
 */
#define RG_PAN_ID_0                      (0x162)
  /** Access parameters for sub-register PAN_ID_0 in register @ref RG_PAN_ID_0
   */
#define SR_PAN_ID_0                  0x162, 0xff, 0

/** Offset for register PAN_ID_1
 */
#define RG_PAN_ID_1                      (0x163)
  /** Access parameters for sub-register PAN_ID_1 in register @ref RG_PAN_ID_1
   */
#define SR_PAN_ID_1                  0x163, 0xff, 0

/** Offset for register IEEE_ADDR_0
 */
#define RG_IEEE_ADDR_0                   (0x164)
  /** Access parameters for sub-register IEEE_ADDR_0 in register @ref RG_IEEE_ADDR_0
   */
#define SR_IEEE_ADDR_0               0x164, 0xff, 0

/** Offset for register IEEE_ADDR_1
 */
#define RG_IEEE_ADDR_1                   (0x165)
  /** Access parameters for sub-register IEEE_ADDR_1 in register @ref RG_IEEE_ADDR_1
   */
#define SR_IEEE_ADDR_1               0x165, 0xff, 0

/** Offset for register IEEE_ADDR_2
 */
#define RG_IEEE_ADDR_2                   (0x166)
  /** Access parameters for sub-register IEEE_ADDR_2 in register @ref RG_IEEE_ADDR_2
   */
#define SR_IEEE_ADDR_2               0x166, 0xff, 0

/** Offset for register IEEE_ADDR_3
 */
#define RG_IEEE_ADDR_3                   (0x167)
  /** Access parameters for sub-register IEEE_ADDR_3 in register @ref RG_IEEE_ADDR_3
   */
#define SR_IEEE_ADDR_3               0x167, 0xff, 0

/** Offset for register IEEE_ADDR_4
 */
#define RG_IEEE_ADDR_4                   (0x168)
  /** Access parameters for sub-register IEEE_ADDR_4 in register @ref RG_IEEE_ADDR_4
   */
#define SR_IEEE_ADDR_4               0x168, 0xff, 0

/** Offset for register IEEE_ADDR_5
 */
#define RG_IEEE_ADDR_5                   (0x169)
  /** Access parameters for sub-register IEEE_ADDR_5 in register @ref RG_IEEE_ADDR_5
   */
#define SR_IEEE_ADDR_5               0x169, 0xff, 0

/** Offset for register IEEE_ADDR_6
 */
#define RG_IEEE_ADDR_6                   (0x16a)
  /** Access parameters for sub-register IEEE_ADDR_6 in register @ref RG_IEEE_ADDR_6
   */
#define SR_IEEE_ADDR_6               0x16a, 0xff, 0

/** Offset for register IEEE_ADDR_7
 */
#define RG_IEEE_ADDR_7                   (0x16b)
  /** Access parameters for sub-register IEEE_ADDR_7 in register @ref RG_IEEE_ADDR_7
   */
#define SR_IEEE_ADDR_7               0x16b, 0xff, 0

/** Offset for register XAH_CTRL_0
 */
#define RG_XAH_CTRL_0                    (0x16c)
  /** Access parameters for sub-register SLOTTED_OPERATION in register @ref RG_XAH_CTRL_0
   */
#define SR_SLOTTED_OPERATION         0x16c, 0x01, 0
  /** Access parameters for sub-register MAX_CSMA_RETRIES in register @ref RG_XAH_CTRL_0
   */
#define SR_MAX_CSMA_RETRIES          0x16c, 0x0e, 1
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__00 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__00 (0)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__10 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__10 (1)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__20 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__20 (2)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__30 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__30 (3)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__40 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__40 (4)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__50 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__50 (5)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__60 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__60 (6)
   /** Constant XAH_CTRL_0_MAX_CSMA_RETRIES__70 for sub-register @ref SR_MAX_CSMA_RETRIES
    */
#define XAH_CTRL_0_MAX_CSMA_RETRIES__70 (7)
  /** Access parameters for sub-register MAX_FRAME_RETRIES in register @ref RG_XAH_CTRL_0
   */
#define SR_MAX_FRAME_RETRIES         0x16c, 0xf0, 4
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__00 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__00 (0)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__10 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__10 (1)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__20 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__20 (2)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__30 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__30 (3)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__40 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__40 (4)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__50 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__50 (5)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__60 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__60 (6)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__70 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__70 (7)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__80 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__80 (8)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__90 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__90 (9)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__100 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__100 (10)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__110 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__110 (11)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__120 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__120 (12)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__130 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__130 (13)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__140 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__140 (14)
   /** Constant XAH_CTRL_0_MAX_FRAME_RETRIES__150 for sub-register @ref SR_MAX_FRAME_RETRIES
    */
#define XAH_CTRL_0_MAX_FRAME_RETRIES__150 (15)

/** Offset for register CSMA_SEED_0
 */
#define RG_CSMA_SEED_0                   (0x16d)
  /** Access parameters for sub-register CSMA_SEED_0 in register @ref RG_CSMA_SEED_0
   */
#define SR_CSMA_SEED_0               0x16d, 0xff, 0
   /** Constant CSMA_SEED_0_CSMA_SEED_0__00 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__00 (0)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__10 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__10 (1)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__20 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__20 (2)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__30 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__30 (3)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__40 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__40 (4)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__50 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__50 (5)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__60 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__60 (6)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__70 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__70 (7)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__80 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__80 (8)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__90 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__90 (9)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__100 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__100 (10)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__110 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__110 (11)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__120 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__120 (12)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__130 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__130 (13)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__140 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__140 (14)
   /** Constant CSMA_SEED_0_CSMA_SEED_0__150 for sub-register @ref SR_CSMA_SEED_0
    */
#define CSMA_SEED_0_CSMA_SEED_0__150 (15)

/** Offset for register CSMA_SEED_1
 */
#define RG_CSMA_SEED_1                   (0x16e)
  /** Access parameters for sub-register CSMA_SEED_1 in register @ref RG_CSMA_SEED_1
   */
#define SR_CSMA_SEED_1               0x16e, 0x07, 0
   /** Constant CSMA_SEED_1_CSMA_SEED_1__00 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__00 (0)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__10 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__10 (1)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__20 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__20 (2)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__30 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__30 (3)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__40 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__40 (4)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__50 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__50 (5)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__60 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__60 (6)
   /** Constant CSMA_SEED_1_CSMA_SEED_1__70 for sub-register @ref SR_CSMA_SEED_1
    */
#define CSMA_SEED_1_CSMA_SEED_1__70 (7)
  /** Access parameters for sub-register AACK_I_AM_COORD in register @ref RG_CSMA_SEED_1
   */
#define SR_AACK_I_AM_COORD           0x16e, 0x08, 3
  /** Access parameters for sub-register AACK_DIS_ACK in register @ref RG_CSMA_SEED_1
   */
#define SR_AACK_DIS_ACK              0x16e, 0x10, 4
  /** Access parameters for sub-register AACK_SET_PD in register @ref RG_CSMA_SEED_1
   */
#define SR_AACK_SET_PD               0x16e, 0x20, 5
  /** Access parameters for sub-register AACK_FVN_MODE in register @ref RG_CSMA_SEED_1
   */
#define SR_AACK_FVN_MODE             0x16e, 0xc0, 6
   /** Constant CSMA_SEED_1_AACK_FVN_MODE__00 for sub-register @ref SR_AACK_FVN_MODE
    */
#define CSMA_SEED_1_AACK_FVN_MODE__00 (0)

/** Constant PD_ACK_BIT_SET_DISABLE for sub-register AACK_SET_PD in register RG_CSMA_SEED_1 */
#define PD_ACK_BIT_SET_DISABLE      (0)

/** Constant PD_ACK_BIT_SET_ENABLE for sub-register AACK_SET_PD in register RG_CSMA_SEED_1 */
#define PD_ACK_BIT_SET_ENABLE      (1)

/** Constant FRAME_VERSION_0 for sub-register AACK_FVN_MODE in register RG_CSMA_SEED_1 */
#define FRAME_VERSION_0         (0)

/** Constant FRAME_VERSION_01 for sub-register AACK_FVN_MODE in register RG_CSMA_SEED_1 */
#define FRAME_VERSION_01         (1)

/** Constant FRAME_VERSION_012 for sub-register AACK_FVN_MODE in register RG_CSMA_SEED_1 */
#define FRAME_VERSION_012         (2)

/** Constant FRAME_VERSION_IGNORED for sub-register AACK_FVN_MODE in register RG_CSMA_SEED_1 */
#define FRAME_VERSION_IGNORED         (3)

/** Offset for register CSMA_BE
 */
#define RG_CSMA_BE                       (0x16f)
  /** Access parameters for sub-register MIN_BE in register @ref RG_CSMA_BE
   */
#define SR_MIN_BE                    0x16f, 0x0f, 0
   /** Constant CSMA_BE_MIN_BE__00 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__00       (0)
   /** Constant CSMA_BE_MIN_BE__10 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__10       (1)
   /** Constant CSMA_BE_MIN_BE__20 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__20       (2)
   /** Constant CSMA_BE_MIN_BE__30 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__30       (3)
   /** Constant CSMA_BE_MIN_BE__40 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__40       (4)
   /** Constant CSMA_BE_MIN_BE__50 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__50       (5)
   /** Constant CSMA_BE_MIN_BE__60 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__60       (6)
   /** Constant CSMA_BE_MIN_BE__70 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__70       (7)
   /** Constant CSMA_BE_MIN_BE__80 for sub-register @ref SR_MIN_BE
    */
#define CSMA_BE_MIN_BE__80       (8)
  /** Access parameters for sub-register MAX_BE in register @ref RG_CSMA_BE
   */
#define SR_MAX_BE                    0x16f, 0xf0, 4
   /** Constant CSMA_BE_MAX_BE__30 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__30       (3)
   /** Constant CSMA_BE_MAX_BE__40 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__40       (4)
   /** Constant CSMA_BE_MAX_BE__50 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__50       (5)
   /** Constant CSMA_BE_MAX_BE__60 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__60       (6)
   /** Constant CSMA_BE_MAX_BE__70 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__70       (7)
   /** Constant CSMA_BE_MAX_BE__80 for sub-register @ref SR_MAX_BE
    */
#define CSMA_BE_MAX_BE__80       (8)

#define RG_TST_RX_LENGTH                 (0x17b)

#endif /* #if (TAL_TYPE == ATMEGARF_TAL_1) */

#endif /* ATMEGA128RFA1_H */
