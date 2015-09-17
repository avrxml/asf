/**
 * @file pal_mcu_generic.h
 *
 * @brief PAL configuration generic for ATmega128RFA1
 *
 * This header file contains generic configuration parameters for ATmega128RFA1.
 *
 * $Id: pal_mcu_generic.h 22792 2010-08-09 06:22:00Z uwalter $
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
#ifndef PAL_MCU_GENERIC_H
#define PAL_MCU_GENERIC_H

/* === Includes =============================================================*/

#include "pal_types.h"

#if (PAL_TYPE == ATMEGA128RFA1)

/*
 * This header file is required since pal_trx_frame_read() and
 * pal_trx_frame_write() depend on memcpy()
 */
#include <string.h>

/* === Types ================================================================*/

/** Enumerations used to identify ports
 * Note: ports A and C are not present on the ATmega128RFA1.
 */

typedef enum port_type_tag
{
    PORT_B,
    PORT_D,
    PORT_E,
    PORT_F,
    PORT_G
} port_type_t;

/** Enumerations used to identify port directions
 * Note: ports A and C are not present on the ATmega128RFA1.
 */
typedef enum port_pin_direction_type_tag
{
    DDR_B,
    DDR_D,
    DDR_E,
    DDR_F,
    DDR_G
} port_pin_direction_type_t;

/* === Externals ============================================================*/


/* === Macros ===============================================================*/

/*
 * Set TRX GPIO pins.
 */
#define RST_HIGH()                      (TRXPR |= _BV(TRXRST))  /**< Set Reset Bit. */
#define RST_LOW()                       (TRXPR &= ~_BV(TRXRST)) /**< Clear Reset Bit. */
#define SLP_TR_HIGH()                   (TRXPR |= _BV(SLPTR))   /**< Set Sleep/TR Bit. */
#define SLP_TR_LOW()                    (TRXPR &= ~_BV(SLPTR))  /**< Clear Sleep/TR Bit. */


/** Clears TRX TX_END interrupt. */
#define CLEAR_TRX_IRQ_TX_END()          (IRQ_STATUS = _BV(TX_END))

/** Clear RX TIME STAMP interrupt. */
#define CLEAR_TRX_IRQ_TSTAMP()          (IRQ_STATUS = _BV(RX_START))

/** Clear TRX RX_END interrupt. */
#define CLEAR_TRX_IRQ_RX_END()          (IRQ_STATUS = _BV(RX_END))

/** Clear TRX CCA_ED_DONE interrupt. */
#define CLEAR_TRX_IRQ_CCA_ED()          (IRQ_STATUS = _BV(CCA_ED_DONE_EN))

/** Clear AMI interrupt. */
#define CLEAR_TRX_IRQ_AMI()             (IRQ_STATUS = _BV(AMI))

/** Clear TRX BAT LOW interrupt. */
#define CLEAR_TRX_IRQ_BATMON()          (BATMON = _BV(BAT_LOW))

/** Clear TRX AWAKE interrupt. */
#define CLEAR_TRX_IRQ_AWAKE()           (IRQ_STATUS = _BV(AWAKE))

/** Clear TRX PLL LOCK interrupt. */
#define CLEAR_TRX_IRQ_PLL_LOCK()        (IRQ_STATUS = _BV(PLL_LOCK))

/** Clear TRX PLL UNLOCK interrupt. */                \
#define CLEAR_TRX_IRQ_PLL_UNLOCK()      (IRQ_STATUS = _BV(PLL_UNLOCK))

/*
 * Note: TRX AES RDY is cleared on read/write of
 * AES_STATE, which has to be done in the AES
 * application.
 */


/** Enables the global interrupts. */
#define ENABLE_GLOBAL_IRQ()             sei()

/** Disables the global interrupts. */
#define DISABLE_GLOBAL_IRQ()            cli()

/** This macro saves the global interrupt status. */
#define ENTER_CRITICAL_REGION()         {uint8_t sreg = SREG; cli()

/** This macro restores the global interrupt status. */
#define LEAVE_CRITICAL_REGION()         SREG = sreg;}


/**
 * @brief Writes data into a transceiver register
 *
 * This macro writes a value into transceiver register.
 *
 * @param addr Address of the trx register
 * @param data Data to be written to trx register
 *
 * @ingroup apiPalApi
 */
#define pal_trx_reg_write(addr, data) \
    (*(volatile uint8_t *)(addr)) = (data)

/**
 * @brief Reads current value from a transceiver register
 *
 * This macro reads the current value from a transceiver register.
 *
 * @param addr Specifies the address of the trx register
 * from which the data shall be read
 *
 * @return value of the register read
 * @ingroup apiPalApi
 */
#define pal_trx_reg_read(addr) \
    (*(volatile uint8_t *)(addr))

/**
 * @brief Reads frame buffer of the transceiver
 *
 * This macro reads the frame buffer of the transceiver.
 *
 * @param[out] data Pointer to the location to store frame
 * @param[in] length Number of bytes to be read from the frame
 * buffer.
 * @ingroup apiPalApi
 */
#define pal_trx_frame_read(data, length) \
    memcpy((data), (void *)&TRXFBST, (length))

/**
 * @brief Writes data into frame buffer of the transceiver
 *
 * This macro writes data into the frame buffer of the transceiver
 *
 * @param[in] data Pointer to data to be written into frame buffer
 * @param[in] length Number of bytes to be written into frame buffer
 * @ingroup apiPalApi
 */
#define pal_trx_frame_write(data, length) \
    memcpy((void *)&TRXFBST, (data), (length))

#ifndef DOXYGEN
#define _pal_trx_bit_read(addr, mask, pos) \
    (((*(volatile uint8_t *)(addr)) & (mask)) >> (pos))
#endif
/**
 * @brief Subregister read
 *
 * @param   arg Subregister
 *
 * @return  Value of the read subregister
 * @ingroup apiPalApi
 */
#define pal_trx_bit_read(arg) \
    _pal_trx_bit_read(arg)

#ifndef DOXYGEN
#define _pal_trx_bit_write(addr, mask, pos, val) do {   \
        (*(volatile uint8_t *)(addr)) =                 \
            ((*(volatile uint8_t *)(addr)) & ~(mask)) | \
            (((val) << (pos)) & (mask));                \
    } while (0)
#endif
/**
 * @brief Subregister write
 *
 * @param[in]   arg1  Subregister
 * @param[out]  val  Data, which is muxed into the register
 * @ingroup apiPalApi
 */
#define pal_trx_bit_write(arg1, val) \
    _pal_trx_bit_write(arg1, val)

/** The smallest timeout in microseconds */
#define MIN_TIMEOUT                     (0x80)

/** The largest timeout in microseconds */
#define MAX_TIMEOUT                     (0x7FFFFFFF)


/** Maximum numbers of software timers running at a time. */
#define MAX_NO_OF_TIMERS                (25)
#if (MAX_NO_OF_TIMERS > 255)
#error "MAX_NO_OF_TIMERS must not be larger than 255"
#endif

/*
 *  This macro is a place holder for delay function for high speed processors
 */
#if ((F_CPU == (16000000UL)) || (F_CPU == (15384600UL)))
#define PAL_WAIT_1_US()                 {nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                                         nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); }
#elif (F_CPU == (8000000UL))
#define PAL_WAIT_1_US()                 {nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); }
#else
#error "Unsupported F_CPU value"
#endif




/* === Prototypes ===========================================================*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* (PAL_TYPE == ATMEGA128RFA1) */

#endif /* PAL_MCU_GENERIC_H */

/* EOF */
