/**
 * @file pal.h
 *
 * @brief PAL related APIs
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
 */

/*
 * Copyright (c) 2013 - 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PAL_H
#define PAL_H

/*
 * NOTE:- Include 'return_val.h' before this file, as return_val.h has the
 *        all return value enums.
 */

/**
 * \defgroup group_pal PAL
 * This module acts as a wrapper layer between the Wireless stack and the ASF
 * drivers
 * All hardwar level acess to the ASF drivers from the stack happens through
 * this module
 * @{
 */
/* === Includes ============================================================ */

#include "compiler.h"
#include "pal_generic.h"
#include "return_val.h"
#include "common_sw_timer.h"
#include "board.h"
#include "asf.h"

#if (PAL_USE_SPI_TRX == 1)
#ifdef MULTI_TRX_SUPPORT
#include "trx_access_2.h"
#else
#include "trx_access.h"
#endif
#else
#include "sysclk.h"
#endif /* #if (PAL_USE_SPI_TRX = 1) */

/* === Macros =============================================================== */

/**
 * @brief Generates blocking delay
 *
 * This functions generates a blocking delay of specified time.
 *
 * @param delay in microseconds
 */
#define pal_timer_delay(delay)    delay_us(delay)

/* Wait for 1 us. */
#define PAL_WAIT_1_US()               delay_us(1);

/* Wait for 65 ns. */
#define PAL_WAIT_65_NS()  {nop(); nop(); }

/**
 * This macro is used for handling endianness among the different CPUs.
 */
#if (UC3)
#define U16_TO_TARGET(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0x00FF))
#else
#define U16_TO_TARGET(x) (x)
#endif

/* Enables the global interrupt */
#define ENABLE_GLOBAL_IRQ()                  Enable_global_interrupt()

/* Disables the global interrupt */
#define DISABLE_GLOBAL_IRQ()                 Disable_global_interrupt()

/* This macro saves the global interrupt status */
#define ENTER_CRITICAL_REGION()              {uint8_t flags = cpu_irq_save();

/* This macro restores the global interrupt status */
#define LEAVE_CRITICAL_REGION()              cpu_irq_restore(flags); }

#define STACK_FLASH_SIZE (1024)

/* === Types =============================================================== */

/*
 * The smallest timeout in microseconds
 */
#define MIN_TIMEOUT                     (0x80)

/**
 * Timer clock source type
 */
typedef enum source_type_tag {
	/** Identifier for timer clock source during sleep */
	TMR_CLK_SRC_DURING_TRX_SLEEP,
	/** Identifier for timer clock source while being awake */
	TMR_CLK_SRC_DURING_TRX_AWAKE
} SHORTENUM source_type_t;

/**
 * Timer clock source while radio is awake.
 *
 * T0 & T1 of PORTC
 * clk source is event channel 2 triggered by transceiver clock (CLKM, 1MHz)
 */
#define TIMER_SRC_DURING_TRX_AWAKE()

/**
 * Timer clock source while radio is sleeping.
 *
 * T0 & T1 of PORTC
 * clk source is event channel 0 triggered by system clock with corresponding
 * event system prescaler (see function event_system_init()).
 */
#define TIMER_SRC_DURING_TRX_SLEEP()

/**
 * @brief IDs for persistence storage access
 */
typedef enum ps_id_tag {
	PS_IEEE_ADDR,
	PS_XTAL_TRIM
} SHORTENUM ps_id_t;

/**
 * @brief Power modes
 */
typedef enum pwr_mode_tag {
	SYSTEM_SLEEP
} pwr_mode_t;

#ifdef TEST_HARNESS
#if (_DEBUG_ > 0)

/**
 * @brief This is the ALERT.indication message structure.
 */
typedef struct assert_tag {
	/**< The total length of this message. */
	uint8_t size ALIGN8BIT;
	/**< This identifies the message as ALERT_INDICATION */
	uint8_t assert_cmdcode ALIGN8BIT;
	/**< Bytes to read to frame, data[0] gives the length */
	uint8_t data[1]         ALIGN8BIT;
} assert_t;
#endif  /* (_DEBUG_ > 0) */
#endif  /* TEST_HARNESS */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of PAL
 *
 * This function initializes the PAL. The RC Oscillator is calibrated.
 *
 * @return MAC_SUCCESS  if PAL initialization is successful, FAILURE otherwise
 */
retval_t pal_init(void);

/**
 * @brief Initializes the transceiver timestamp interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver timestamp interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver timestamp interrupt
 */
void pal_trx_irq_init_tstamp(FUNC_PTR trx_irq_cb);

/**
 * \brief Enables the transceiver main interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between enabling
 * transceiver interrupts at the transceiver, and setting the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * enabled by setting the MCU IRQ mask.
 *
 */
#define pal_trx_irq_en()                ENABLE_TRX_IRQ()

/**
 * \brief Enables the transceiver timestamp interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between enabling
 * transceiver interrupts at the transceiver, and setting the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * enabled by setting the MCU IRQ mask.
 *
 */
#define pal_trx_irq_en_tstamp()         ENABLE_TRX_IRQ_TSTAMP()

/**
 * \brief Disables the transceiver main interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between disabling
 * transceiver interrupts at the transceiver, and clearing the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * disabled by clearing the MCU IRQ mask.
 *
 */
#define pal_trx_irq_dis()               DISABLE_TRX_IRQ()

/**
 * \brief Disables the transceiver timestamp interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between disabling
 * transceiver interrupts at the transceiver, and clearing the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * disabled by clearing the MCU IRQ mask.
 *
 */
#define pal_trx_irq_dis_tstamp()        DISABLE_TRX_IRQ_TSTAMP()

/**
 * @brief Clears the transceiver main interrupt
 *
 */
#define pal_trx_irq_flag_clr()          CLEAR_TRX_IRQ()

/**
 * @brief Clears the transceiver timestamp interrupt
 *
 */
#define pal_trx_irq_flag_clr_tstamp()   CLEAR_TRX_IRQ_TSTAMP()

/**
 * @brief Enables the global interrupt
 */
static inline void pal_global_irq_enable(void)
{
	ENABLE_GLOBAL_IRQ();
}

/**
 * @brief Disables the global interrupt
 */
static inline void pal_global_irq_disable(void)
{
	DISABLE_GLOBAL_IRQ();
}

/**
 * \brief Initializes the transceiver main interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver main interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver main interrupt
 */
void pal_trx_irq_init(FUNC_PTR trx_irq_cb);

/**
 * @brief Provides timestamp of the last received frame
 *
 * This function provides the timestamp (in microseconds)
 * of the last received frame.
 *
 * @param[out] timestamp in microseconds
 */
void pal_trx_read_timestamp(uint32_t *timestamp);

/*
 * Prototypes for transceiver access.  Some PALs define these as
 * macros instead of implementing them as functions, so only declare
 * them here if they are not implemented as macros.
 */

/**
 * @brief Calibrates the internal RC oscillator
 *
 * @return True if calibration is successful, false otherwise.
 */
bool pal_calibrate_rc_osc(void);

/**
 * @brief Selects timer clock source
 *
 * This function selects the clock source of the timer.
 *
 * @param source
 * - @ref TMR_CLK_SRC_DURING_TRX_SLEEP if clock source during sleep is to be
 * selected, and
 * - @ref TMR_CLK_SRC_DURING_TRX_AWAKE if clock source while being awake is
 * selected.
 */
void pal_timer_source_select(source_type_t source);

/**
 * @brief Adds two time values
 *
 * @param a Time value 1
 * @param b Time value 2
 *
 * @return Addition of a and b
 */
static inline uint32_t pal_add_time_us(uint32_t a, uint32_t b)
{
	return (ADD_TIME(a, b));
}

/**
 * @brief Subtracts two time values
 *
 * @param a Time value 1
 * @param b Time value 2
 *
 * @return Difference between a and b
 */
static inline uint32_t pal_sub_time_us(uint32_t a, uint32_t b)
{
	return (SUB_TIME(a, b));
}

/* ! @} */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* PAL_H */
/* EOF */
