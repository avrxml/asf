/**
 * @file avrtypes.h
 *
 * @brief Compatibility definitions for compilers (IAR, GCC)
 *
 * This file contains AVR type definitions that enable Atmel's 802.15.4
 * stack implementation to build using multiple compilers. This file
 * contains compiler conditional subsystems for AVR functions such as:
 * interrupts, sleep mode, USARTs, etc.
 *
 * $Id: avrtypes.h 27766 2011-07-19 12:14:15Z agasthian.s $
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
#ifndef AVRTYPES_H
#define AVRTYPES_H

/* === Includes ============================================================= */

#include <string.h>

/*
 * The if defined(__ICCAVR__) section below applies to the IAR compiler.
 */
#if defined(__ICCAVR__)

#include <ioavr.h>
#include <intrinsics.h>
#include <pgmspace.h>

/*
 * Map AVR names from IAR to GCC.
 */
#include "mcu_types.h"

/*
 * The elif defined(__GNUC__) section below applies to the GNUC compiler.
 */
#elif defined(__GNUC__)

#include <avr/io.h>
#if !defined(__AVR_ATxmega16A4__) \
&& !defined(__AVR_ATxmega16D4__) \
&& !defined(__AVR_ATxmega32A4__) \
&& !defined(__AVR_ATxmega32D4__) \
&& !defined(__AVR_ATxmega64A1__) \
&& !defined(__AVR_ATxmega64A3__) \
&& !defined(__AVR_ATxmega64D3__) \
&& !defined(__AVR_ATxmega128A1__) \
&& !defined(__AVR_ATxmega128A3__) \
&& !defined(__AVR_ATxmega128D3__) \
&& !defined(__AVR_ATxmega192A3__) \
&& !defined(__AVR_ATxmega192D3__) \
&& !defined(__AVR_ATxmega256A3__) \
&& !defined(__AVR_ATxmega256D3__) \
&& !defined(__AVR_ATxmega256A3B__)
#include <avr/boot.h>
#include <avr/eeprom.h>
#endif
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/crc16.h>

/*
 * The else section below applies to unsupported compiler. This is where support
 * for compilers other than IAR and GNUC would be placed.
 */
#else
#error Unsupported compiler
#endif /* compiler selection */


/* === Externals ============================================================ */


/* === Macros =============================================================== */

#ifndef _BV
/**
 * Bit value -- compute the bitmask for a bit position
 */
#define _BV(x) (1 << (x))
#endif

/**
 * This macro saves the IRQ status and is typically used before cli().
 */
#define save_irq_status() (sreg = SREG)

/**
 * This macro must be used together with save_irq_status() and it will
 * restore the content of the IRQ status.
 */
#define restore_irq_status() (SREG = sreg)

/*
 * This block contains just the documentation for all
 * compiler-dependent macros.
 */
#if defined(DOXYGEN)

/**
 * Start an interrupt handler for vector \c vec.
 */
#define ISR(vec)

/**
 * Globally enable interrupts.
 */
#define sei()
/**
 * Globally disable interrupts.
 */
#define cli()
/**
 * Read contents of EEPROM cell \c addr into variable \c var.
 */
#define EEGET(var, addr)
/**
 * Null operation: just waste one CPU cycle.
 */
#define nop()
/**
 * Clear (i.e. reset to 0) the watchdog timer.
 */
#define WDT_RESET()
/**
 * Disable the watchdog.
 */
#define WDT_DISABLE()
/**
 * Watchdog timeouts
 */
#define WDT_TO_16MS
#define WDT_TO_32MS
#define WDT_TO_64MS
#define WDT_TO_125MS
#define WDT_TO_250MS
#define WDT_TO_500MS
#define WDT_TO_1S
#define WDT_TO_2S
#define WDT_TO_4S
#define WDT_TO_8S

/**
 * Put the CPU to sleep, according to the sleep mode selected
 * with \c set_sleep_mode.
 */
#define SLEEP_MODE()
/**
 * Enter idle sleep mode: turn off the CPU, but keep all clocks running.
 */
#define SLEEP_MODE_IDLE
/**
 * Stop the CPU but keep the ADC running.
 */
#define SLEEP_MODE_ADC
/**
 * Stop the CPU and all oscillators.
 */
#define SLEEP_MODE_PWR_DOWN
/**
 * Stop the CPU and all oscillators but the asynchronous one.
 */
#define SLEEP_MODE_PWR_SAVE
/**
 * Stop the CPU but keep the external oscillator running.
 */
#define SLEEP_MODE_STANDBY
/**
 * Like power-save mode but keep the external oscillator running.
 */
#define SLEEP_MODE_EXT_STANDBY
/**
 * All sleep mode bits.
 */
#define SLEEP_MODE_MASK
/**
 * Set the sleep mode to enter upon the next \c sleep_mode call.
 */
#define set_sleep_mode(x)

/* program memory space abstraction */
/**
 * Declare object \c x to be placed into flash ROM.
 */
#define FLASH_DECLARE(x)
/**
 * Declare function pointer to point to far addresses.
 */
#define FUNC_PTR
/**
 * Declare string \c x to be placed into flash ROM.
 */
#define FLASH_STRING(x)
/**
 * Type of a string that is located in flash ROM.
 */
#define FLASH_STRING_T
/**
 * Read one byte at address \c x from flash ROM.
 */
#define PGM_READ_BYTE(x)
/**
 * Read one byte at address \c x from flash ROM (>64k).
 */
#define PGM_READ_BYTE_FAR(x)
/**
 * Read two bytes at address \c x from flash ROM.
 */
#define PGM_READ_WORD(x)
/**
 * Read a block of \c len bytes at address \c src from
 * flash ROM to RAM address \c dst.
 */
#define PGM_READ_BLOCK(dst, src, len)
/**
 * Determine the string length of a string locatend in
 * flash ROM at address \c x.
 */
#define PGM_STRLEN(x)
/**
 * Copy a string from flash ROM at address \c src to RAM address \c dst.
 */
#define PGM_STRCPY(dst, src)
/**
 * Wait while SPM instruction is busy
 */
#define SPM_BUSY_WAIT()
/**
 * Enable Read While Write Section
 */
#define ENABLE_RWW_SECTION()
/**
 * Wait while EEPROM is busy
 */
#define EEPROM_BUSY_WAIT()
/**
 * Set to one if the compilation environment supports a vsnprintf_P() function
 * (like vsnprintf() but with the format string in flash ROM).
 */
#define HAS_PGM_VSNPRINTF
/**
 * Define the function to use for printing a string with the format string in
 * flash ROM, parameters are similar to vsnprintf().
 */
#define PGM_VSNPRINTF(dst, n, fmt, ap)
/**
 * Define this to the percent format specifier used to indicate a string that
 * is located in flash ROM rather than in RAM.  Do not define this macro if
 * the compilation environment does not support taking arguments from flash
 * ROM in printf()-like functions.
 */
#define PRINTF_FLASH_STRING
/**
 * Erase the flash page that contains the provided address.
 */
#define FLASH_ERASE(addr)
/**
 * Fill the page buffer
 */
#define FLASH_PAGE_FILL(addr, data)
/**
 * Write the contents from the page buffer to flash
 */
#define FLASH_PAGE_WRITE(addr)

/**
 * Update the 16-bit CCITT \c crc from the next \c data byte, returning
 * the new CRC.
 */
#define CRC_CCITT_UPDATE(crc, data)

/**
 * Attribute to apply to struct tags in order to force it into an 8-bit
 * alignment.
 */
#define ALIGN8BIT

/**
 * Attribute to apply to an enum declaration to force it into the smallest
 * type size required to represent all values.
 */
#define SHORTENUM

/**
 * Attribute to apply to a structure declaration to force it into the smallest
 * type size required to represent all values.
 */
#define PACKED

/**
 * The name of a low-level initialization function that is run before
 * initializing main().
 */
#define ENABLE_XRAM(void)

/**
 * The return value to be returned by ENABLE_XRAM(), if any.
 */
#define RETURN_ENABLE_XRAM

/**
 * Declare a function name of type type to be forced into inline code.
 */
#define FORCE_INLINE(type, name, ...)

#define RAMFUNCTION

/**
 * Perform a delay of \c us microseconds.
 *
 * The macro F_CPU is supposed to be defined to a
 * constant defining the CPU clock frequency (in Hertz).
 *
 * The maximal possible delay is 262.14 ms / F_CPU in MHz.
 *
 * \note For the IAR compiler, currently F_CPU must be a
 * multiple of 1000000UL (1 MHz).
 */
extern void delay_us(double us);

/**
 * This macros tells whether the compiler can properly initialize C99
 * flexible array members at the end of a struct.  If not, in some
 * situations inflexible arrays must be used which can waste memory.
 */
#define CAN_INITIALIZE_FLEXIBLE_ARRAY_MEMBERS 0

/**
 * Read out the AVR fuse byte number \c n.  \c n can be 0 for the low
 * fuse byte, 1 for the high fuse byte, or 2 for the extended fuse
 * byte.
 */
#define read_avr_fuse(n)

#endif /* defined(DOXYGEN) */


#if defined(__ICCAVR__)

/*
 * Map interrupt vectors.
 */
#if defined(__ATmega128__)
/*
 * ATmega128 vector names are misspelled wrt. the data sheets and XML
 * files.  Map them to their canonical names.
 */
#define USART0_RX_vect USART0_RXC_vect
#define USART0_TX_vect USART0_TXC_vect
#define USART1_RX_vect USART1_RXC_vect
#define USART1_TX_vect USART1_TXC_vect
#endif /* defined(__ATmega128__) */

#if defined (__AT90USB1287__)
#define USB_GEN_vect USB_General_vect
#endif /* defined (__AT90USB1287__) */

#if defined (__ATMEGA128RFA1__)

/* Watchdog Time-out periods */
#define WDT_TO_16MS     (0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0)    // 15ms    0x00
#define WDT_TO_32MS     (0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0)    // 30ms    0x01
#define WDT_TO_64MS     (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(0<<WDP0)    // 60ms    0x02
#define WDT_TO_125MS    (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(1<<WDP0)    // 0.120s  0x03
#define WDT_TO_250MS    (0<<WDP3 )|(1<<WDP2 )|(0<<WDP1)|(0<<WDP0)    // 0.25s   0x04
#define WDT_TO_500MS    (0<<WDP3 )|(1<<WDP2 )|(0<<WDP1)|(1<<WDP0)    // 0.5s    0x05
#define WDT_TO_1S       (0<<WDP3 )|(1<<WDP2 )|(1<<WDP1)|(0<<WDP0)    // 1.0s    0x06
#define WDT_TO_2S       (0<<WDP3 )|(1<<WDP2 )|(1<<WDP1)|(1<<WDP0)    // 2.0s    0x07
#define WDT_TO_4S       (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0)    // 4.0s    0x08
#define WDT_TO_8S       (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0)    // 8.0s    0x09

/**
 * @brief Setup Watchdog.
 * This MACRO is used to set the watchdog in the one of the three modes available and
 * set the Watchdog Time-out period.
 *
 * @param X which is to be set for the Watchdog-Timeout
 */

#define WDT_INTERRUPT_MODE(x)       do{\
                                        MCUSR  &=  ~(1<<WDRF);\
                                        WDTCSR   =  (1<<WDCE )|(1<<WDE  );\
                                        WDTCSR  =  (1<<WDIF)|(1<<WDIE)|(1<<WDCE )|(0<<WDE )|(x);\
                                      }while(0)

#define WDT_SYSTEM_RESET_MODE(x)    do{\
                                        MCUSR = 0;\
                                        MCUSR  &=  ~(1<<WDRF);\
                                        WDTCSR   =  (1<<WDCE )|(1<<WDE  );\
                                        WDTCSR  =  (1<<WDIF)|(0<<WDIE)|(1<<WDCE )|(1<<WDE )|(x);\
                                      }while(0)

#define WDT_INT_AND_SYS_RST_MODE(x) do{\
                                        MCUSR  &=  ~(1<<WDRF);\
                                        WDTCSR   =  (1<<WDCE )|(1<<WDE  );\
                                        WDTCSR  =  (1<<WDIF)|(1<<WDIE)|(1<<WDCE )|(1<<WDE )|(x);\
                                      }while(0)

/**
 * @brief Disable Watchdog.
 * This MACRO is used to Disable the Watchdog
 */
#define WDT_DISABLE()                do { uint8_t sreg = SREG; __disable_interrupt();\
                                          __watchdog_reset(); MCUSR &= ~(1 << WDRF);\
                                          WDTCSR |= (1 << WDCE) | (1 << WDE);  asm ("LDI R16,0x00");\
                                          asm ("STS 0x60,R16");\
                                          SREG = sreg;\
                                        } while(0)


#endif /* (PAL_TYPE == __ATMEGA128RFA1__) */

/*
 * Some preprocessor magic to allow for a header file abstraction of
 * interrupt service routine declarations for the IAR compiler.  This
 * requires the use of the C99 _Pragma() directive (rather than the
 * old #pragma one that could not be used as a macro replacement), as
 * well as two different levels of preprocessor concetanations in
 * order to do both, assign the correct interrupt vector name, as well
 * as construct a unique function name for the ISR.
 *
 * Do *NOT* try to reorder the macros below, or you'll suddenly find
 * out about all kinds of IAR bugs...
 */
#define PRAGMA(x) _Pragma( #x )
#define ISR(vec) PRAGMA( vector = vec ) __interrupt void handler_##vec(void)

#define sei() __enable_interrupt()
#define cli() __disable_interrupt()
#define EEGET(var, addr) __EEGET(var, addr)
#define EEPUT(addr, val) __EEPUT(addr, val)
#define nop() __no_operation()
#define WDT_RESET() __watchdog_reset()

#define SLEEP_MODE() do { SMCR |= (1 << SE); __sleep(); \
                        SMCR &= ~(1 << SE); } while (0)
#define SLEEP_MODE_IDLE         0
#define SLEEP_MODE_ADC          (1 << SM0)
#define SLEEP_MODE_PWR_DOWN     (1 << SM1)
#define SLEEP_MODE_PWR_SAVE     ((1 << SM0) | (1 << SM1))
#define SLEEP_MODE_STANDBY      ((1 << SM1) | (1 << SM2))
#define SLEEP_MODE_EXT_STANDBY  ((1 << SM0) | (1 << SM1) | (1 << SM2))
#define SLEEP_MODE_MASK         ((1 << SM0) | (1 << SM1) | (1 << SM2))
#define SET_SLEEP_MODE(x) do { SMCR = (SMCR & ~SLEEP_MODE_MASK) | \
                                        (x); } while (0)
typedef enum
{
    clock_div_1 = 0,
    clock_div_2 = 1,
    clock_div_4 = 2,
    clock_div_8 = 3,
    clock_div_16 = 4,
    clock_div_32 = 5,
    clock_div_64 = 6,
    clock_div_128 = 7,
    clock_div_256 = 8
} clock_div_t;
#define clock_prescale_set(x)                       \
            {                                       \
                uint8_t sreg = SREG;                \
                cli();                              \
                CLKPR = 0x80;   /* set CLKPCE */    \
                CLKPR = x;                          \
                SREG = sreg;                        \
            }

/* program memory space abstraction */
#define FLASH_EXTERN(x) extern __flash x
#define FLASH_DECLARE(x) __flash x
#if (FLASHEND > 0x1FFFF)    // Required for program code larger than 128K
    #define FUNC_PTR void __farflash *
#else
    #define FUNC_PTR void *
#endif  /* ENABLE_FAR_FLASH */

#define FLASH_STRING(x) ((__flash const char *)(x))
#define FLASH_STRING_T  char const __flash *
#define PGM_READ_BYTE(x) *(x)
#define PGM_READ_BYTE_FAR(x) __extended_load_program_memory((const unsigned char __farflash *)x)
#define PGM_READ_WORD(x) *(x)
#define PGM_READ_BLOCK(dst, src, len) memcpy_P((dst), (src), (len))
#define PGM_STRLEN(x) strlen_P(x)
#define PGM_STRCPY(dst, src) strcpy_P((dst), (src))
#define SPM_BUSY_WAIT() do{}while((SPMCSR & (uint8_t)_BV(SPMEN)))
#define ENABLE_RWW_SECTION() __DataToR0ByteToSPMCR_SPM(0x00, 0x11)
#define EEPROM_BUSY_WAIT() do{}while(EECR & (1 << EEPE))
/* IAR has no vsnprintf_P(), and no %S format. */
#define HAS_PGM_VSNPRINTF 0
#define PGM_VSNPRINTF(dst, n, fmt, ap) vsnprintf((dst), (n), (fmt), (ap))
#undef PRINTF_FLASH_STRING
/* Flash write abstraction */
#define FLASH_ERASE(addr)  _SPM_24_ERASE(addr)
#define FLASH_PAGE_FILL(addr, data)  _SPM_24_FILLTEMP(addr, data)
#define FLASH_PAGE_WRITE(addr)  _SPM_24_PAGEWRITE(addr)

#define ALIGN8BIT /**/
#define SHORTENUM /**/
#define PACKED    /**/

#define PUTS(s)  { static const char __flash c[] = s; printf_P(c); }
#define PRINTF(fmt, ...) { static const char __flash c[] = fmt; printf_P(c, __VA_ARGS__); }

#define ENABLE_XRAM(void) \
char __low_level_init(void)
#define RETURN_ENABLE_XRAM return 1

#define FORCE_INLINE(type, name, ...) \
static inline type name(__VA_ARGS__)

#define RAMFUNCTION /**/

#define delay_us(us)   __delay_cycles((F_CPU/1000000UL) * us)

#define CAN_INITIALIZE_FLEXIBLE_ARRAY_MEMBERS 0

/*
 * <intrinsics.h> contains an _SPM_GET_FUSEBITS() macro, but this
 * macro can only read the low fuse byte, so it is redefined here for
 * the complete range.
 */
#define read_avr_fuse(n) \
    __AddrToZByteToSPMCR_LPM((void __flash*) \
                             (((n) == 0? 0x0000: ((n) == 1? 0x0003: 0x0002))), \
                             0x09)

#define ADDR_COPY_DST_SRC_16(dst, src)  ((dst) = (src))
#define ADDR_COPY_DST_SRC_64(dst, src)  ((dst) = (src))

/* Converts a 2 Byte array into a 16-Bit value */
#define convert_byte_array_to_16_bit(data) \
    (*(uint16_t *)(data))

/* Converts a 4 Byte array into a 32-Bit value */
#define convert_byte_array_to_32_bit(data) \
    (*(uint32_t *)(data))

/* Converts a 8 Byte array into a 64-Bit value */
#define convert_byte_array_to_64_bit(data) \
    (*(uint64_t *)(data))

/* Converts a 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_spec_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_address(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts a 32-Bit value into a 4 Byte array */
#define convert_32_bit_to_byte_array(value, data) \
    ((*(uint32_t *)(data)) = (uint32_t)(value))

/* Converts a 64-Bit value into  a 8 Byte array */
#define convert_64_bit_to_byte_array(value, data) \
    ((*(uint64_t *)(data)) = (uint64_t)(value))

#endif /* defined(__ICCAVR__) */

#if defined(__GNUC__)

#define nop() do { __asm__ __volatile__ ("nop"); } while (0)
#define EEGET(var, addr) (var) = eeprom_read_byte((uint8_t *)(addr))
#define EEPUT(addr, val) eeprom_write_byte((uint8_t *)(addr), (uint8_t)(val))
#define ALIGN8BIT /* AVR-GCC uses an 8-bit alignment anyway */
#define SHORTENUM __attribute__ ((packed))
#define PACKED __attribute__((packed))
#define SLEEP_MODE() sleep_mode()
#define SET_SLEEP_MODE(x) set_sleep_mode(x)
#define WDT_RESET()    wdt_reset()
#define WDT_DISABLE()  wdt_disable()

#if defined (__ATMEGA128RFA1__)

/* Watchdog Time-out periods */
#define WDT_TO_16MS   WDTO_15MS
#define WDT_TO_32MS   WDTO_30MS
#define WDT_TO_64MS   WDTO_60MS
#define WDT_TO_125MS  WDTO_120MS
#define WDT_TO_250MS  WDTO_250MS
#define WDT_TO_500MS  WDTO_500MS
#define WDT_TO_1S     WDTO_1S
#define WDT_TO_2S     WDTO_2S
#define WDT_TO_4S     WDTO_4S
#define WDT_TO_8S     WDTO_8S

/**
 * @brief Setup Watchdog.
 * This MACRO is used to set the watchdog in the one of the three modes available and
 * set the Watchdog Time-out period.
 *
 * @param X which is to be set for the Watchdog-Timeout
 */

#define WDT_INTERRUPT_MODE(x)       do{\
                                        wdt_disable();\
                                        wdt_enable(x);\
                                        WDTCSR |= (1<<WDIE);\
                                      }while(0)


#define WDT_SYSTEM_RESET_MODE(x)    do{\
                                        wdt_disable();\
                                        wdt_enable(x);\
                                      }while(0)

#define WDT_INT_AND_SYS_RST_MODE(x) do{\
                                        wdt_disable();\
                                        wdt_enable(x);\
                                        WDTCSR |= (1<<WDIE);\
                                      }while(0)

#endif /* (PAL_TYPE == __ATMEGA128RFA1__) */

/* program memory space abstraction */
#define FLASH_EXTERN(x) extern x __attribute__((__progmem__))
#define FLASH_DECLARE(x) const x __attribute__((__progmem__))
#define FUNC_PTR void *
#define FLASH_STRING(x) PSTR(x)
#define FLASH_STRING_T  PGM_P
#define PGM_READ_BYTE(x) pgm_read_byte(x)
#define PGM_READ_BYTE_FAR(x) pgm_read_byte_far(x)
#define PGM_READ_WORD(x) pgm_read_word(x)
#define PGM_READ_BLOCK(dst, src, len) memcpy_P((dst), (src), (len))
#define PGM_STRLEN(x) strlen_P(x)
#define PGM_STRCPY(dst, src) strcpy_P((dst), (src))
#define SPM_BUSY_WAIT() boot_spm_busy_wait()
#define ENABLE_RWW_SECTION() boot_rww_enable()
#define EEPROM_BUSY_WAIT() eeprom_busy_wait()
/* avr-libc has vsnprintf_P(), and %S. */
#define HAS_PGM_VSNPRINTF 1
#define PGM_VSNPRINTF(dst, n, fmt, ap) vsnprintf_P((dst), (n), (fmt), (ap))
#define PRINTF_FLASH_STRING "%S"
/* Flash write abstraction */
#define FLASH_ERASE(addr)  boot_page_erase(addr)
#define FLASH_PAGE_FILL(addr, data)  boot_page_fill(addr, data)
#define FLASH_PAGE_WRITE(addr)  boot_page_write(addr)

#define PUTS(s) { static const char c[] PROGMEM = s; printf_P(c); }
#define PRINTF(fmt, ...) { static const char c[] PROGMEM = fmt; printf_P(c, __VA_ARGS__); }

#define ENABLE_XRAM(void) \
void xram_init(void) __attribute__((naked)) __attribute((section(".init3")));\
void xram_init(void)
#define RETURN_ENABLE_XRAM /**/

#define FORCE_INLINE(type, name, ...) \
static inline type name(__VA_ARGS__) __attribute__((always_inline)); \
static inline type name(__VA_ARGS__)

#define RAMFUNCTION

#define CAN_INITIALIZE_FLEXIBLE_ARRAY_MEMBERS 1

#define read_avr_fuse(n) \
    boot_lock_fuse_bits_get(((n) == 0? GET_LOW_FUSE_BITS:\
                             ((n) == 1? GET_HIGH_FUSE_BITS: GET_EXTENDED_FUSE_BITS)))

#define ADDR_COPY_DST_SRC_16(dst, src)  memcpy((&(dst)), (&(src)), sizeof(uint16_t))
#define ADDR_COPY_DST_SRC_64(dst, src)  memcpy((&(dst)), (&(src)), sizeof(uint64_t))

/* Converts a 2 Byte array into a 16-Bit value */
#define convert_byte_array_to_16_bit(data) \
    (*(uint16_t *)(data))

/* Converts a 4 Byte array into a 32-Bit value */
#define convert_byte_array_to_32_bit(data) \
    (*(uint32_t *)(data))

/* Converts a 8 Byte array into a 64-Bit value */
#define convert_byte_array_to_64_bit(data) \
    (*(uint64_t *)(data))

/* Converts a 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_spec_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_address(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts a 32-Bit value into a 4 Byte array */
#define convert_32_bit_to_byte_array(value, data) \
    ((*(uint32_t *)(data)) = (uint32_t)(value))

/* Converts a 64-Bit value into  a 8 Byte array */
/* Here memcpy requires much less footprint */
#define convert_64_bit_to_byte_array(value, data) \
    memcpy((data), (&(value)), sizeof(uint64_t))

#endif /* defined(__GNUC__) */

/*Defines the Flash Storage for the request and response of MAC*/
#define CMD_ID_OCTET    (0)

/* Converting of values from CPU endian to little endian. */
#define CPU_ENDIAN_TO_LE16(x)   (x)
#define CPU_ENDIAN_TO_LE32(x)   (x)
#define CPU_ENDIAN_TO_LE64(x)   (x)

/* Converting of values from little endian to CPU endian. */
#define LE16_TO_CPU_ENDIAN(x)   (x)
#define LE32_TO_CPU_ENDIAN(x)   (x)
#define LE64_TO_CPU_ENDIAN(x)   (x)

/* Converting of constants from little endian to CPU endian. */
#define CLE16_TO_CPU_ENDIAN(x)  (x)
#define CLE32_TO_CPU_ENDIAN(x)  (x)
#define CLE64_TO_CPU_ENDIAN(x)  (x)

/* Converting of constants from CPU endian to little endian. */
#define CCPU_ENDIAN_TO_LE16(x)  (x)
#define CCPU_ENDIAN_TO_LE32(x)  (x)
#define CCPU_ENDIAN_TO_LE64(x)  (x)

#define MEMCPY_ENDIAN memcpy


/* === Types ================================================================ */


/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef cplusplus
} /* extern "C" */
#endif

#endif /* AVRTYPES_H */
/* EOF */
