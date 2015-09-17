/**
 * @file pal.h
 *
 * @brief PAL related APIs
 *
 * This header file declares prototypes of PAL APIs, enumerations
 * used by TAL and MAC.
 *
 * $Id: pal.h 27828 2011-07-25 09:33:49Z agasthian.s $
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
#ifndef PAL_H
#define PAL_H

/*
 * NOTE:- Include 'return_val.h' before this file, as return_val.h has the
 *        all return value enums.
 */

/* === Includes ============================================================ */

#include <stdbool.h>
#include <stdint.h>
#include "pal_types.h"

#include "pal_config.h"

#if (PAL_GENERIC_TYPE == MEGA_RF_)
#include "pal_mcu_generic.h"
#endif
#include "app_config.h"

/* === Macros =============================================================== */

/**
 * Adds two time values
 */
#define ADD_TIME(a, b)                  ((a) + (b))

/**
 * Subtracts two time values
 */
#define SUB_TIME(a, b)                  ((a) - (b))

/**
 * This macro is used for handling endianness among the different CPUs.
 */
#if (PAL_GENERIC_TYPE == AVR32_)
#define U16_TO_TARGET(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0x00FF))
#else
#define U16_TO_TARGET(x) (x)
#endif

/* === Types =============================================================== */

/**
 * Pin State
 */
/* Please dont change this type. */
typedef enum pin_state_tag
{
    /** Pin state low */
    LOW,
    /** Pin state high */
    HIGH
} pin_state_t;


#if defined (SIO_HUB) || defined(DOXYGEN)
/**
 * Stream I/O to serial mapping enumeration
 */
typedef enum sio_serial_tag
{
    /* Serial unit used by user */
    /** UART 0 */
    SIO_0,
    /** UART 1 */
    SIO_1,
    /** USB 0 */
    SIO_2,
    /** UART 2 */
    SIO_3,
    /** UART 3 */
    SIO_4
} sio_serial_t;
#endif


/**
 * Timeout type
 */
typedef enum
timeout_type_tag
{
    /** The timeout is relative to the current time. */
    TIMEOUT_RELATIVE,
    /** The timeout is an absolute value. */
    TIMEOUT_ABSOLUTE
} timeout_type_t;


/**
 * Timer clock source type
 */
typedef enum source_type_tag
{
    /** Identifier for timer clock source during sleep */
    TMR_CLK_SRC_DURING_TRX_SLEEP,
    /** Identifier for timer clock source while being awake */
    TMR_CLK_SRC_DURING_TRX_AWAKE
} source_type_t;

/**
 * @brief IDs for persistence storage access
 */
typedef enum ps_id_tag
{
    PS_IEEE_ADDR,
    PS_XTAL_TRIM
} ps_id_t;


/**
 * @brief IDs for persistence storage type
 */
typedef enum ps_type_tag
{
    /** MCU Internal EEPROM */
    INTERN_EEPROM,
    /** External EEPROM */
    EXTERN_EEPROM
} ps_type_t;


/**
 * @brief LED action
 */
typedef enum led_action_tag
{
    /** Switch LED on. */
    LED_ON,
    /** Switch LED off. */
    LED_OFF,
    /** Toggle LED. */
    LED_TOGGLE
} led_action_t;


/**
 * @brief Button action
 */
typedef enum button_state_tag
{
    /** Button is pressed. */
    BUTTON_PRESSED,
    /** Button is released. */
    BUTTON_OFF
} button_state_t;


/**
 * @brief Power modes
 */
typedef enum pwr_mode_tag
{
    SYSTEM_SLEEP
} pwr_mode_t;



#ifdef TEST_HARNESS
#if (DEBUG > 0)
/**
 * @brief This is the ALERT.indication message structure.
 */
typedef struct assert_tag
{
    /**< The total length of this message. */
    uint8_t size            ALIGN8BIT;
    /**< This identifies the message as ALERT_INDICATION */
    uint8_t assert_cmdcode  ALIGN8BIT;
    /**< Bytes to read to frame, data[0] gives the length */
    uint8_t data[1]         ALIGN8BIT;
} assert_t;
#endif  /* (DEBUG > 0) */
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
 * @ingroup apiPalApi
 */
retval_t pal_init(void);


/**
 * @brief Services timer and sio handler
 *
 * This function calls sio & timer handling functions.
 */
void pal_task(void);

#if (PAL_GENERIC_TYPE != MEGA_RF)
/**
 * @brief Initializes the transceiver main interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver main interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver main interrupt
 * @ingroup apiPalApi
 */
void pal_trx_irq_init(FUNC_PTR trx_irq_cb);
#endif /* (PAL_GENERIC_TYPE != MEGA_RF) */

/**
 * @brief Initializes the transceiver timestamp interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver timestamp interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver timestamp interrupt
 * @ingroup apiPalApi
 */
void pal_trx_irq_init_tstamp(FUNC_PTR trx_irq_cb);

#if (PAL_GENERIC_TYPE == MEGA_RF_)
/**
 * @brief Initializes the transceiver RX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver RX END interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver RX END interrupt
 */
void pal_trx_irq_init_rx_end(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver TX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver TX END interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver TX END interrupt
 */
void pal_trx_irq_init_tx_end(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver CCA ED END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver CCA ED END interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver CCA ED END interrupt
 */
void pal_trx_irq_init_cca_ed(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver AWAKE interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AWAKE interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver AWAKE interrupt
 */
void pal_trx_irq_init_awake(FUNC_PTR trx_irq_cb);

#ifdef ENABLE_ALL_TRX_IRQS
/**
 * @brief Initializes the transceiver AMI interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AMI interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver AMI interrupt
 */
void pal_trx_irq_init_ami(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver BATMON interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver BATMON interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver BATMON interrupt
 */
void pal_trx_irq_init_batmon(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver PLL_LOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_LOCK interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver PLL_LOCK interrupt
 */
void pal_trx_irq_init_pll_lock(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver PLL_UNLOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_UNLOCK interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver PLL_UNLOCK interrupt
 */
void pal_trx_irq_init_pll_unlock(FUNC_PTR trx_irq_cb);

/**
 * @brief Initializes the transceiver AES_READY interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AES_READY interrupt
 *
 * @param trx_irq_cb Callback function for the transceiver AES_READY interrupt
 */
void pal_trx_irq_init_aes_ready(FUNC_PTR trx_irq_cb);
#endif  /* ENABLE_ALL_TRX_IRQS */
#endif  /* #if (PAL_GENERIC_TYPE == MEGA_RF_) */

#if (PAL_GENERIC_TYPE != MEGA_RF)
/**
 * @brief Enables the transceiver main interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between enabling
 * transceiver interrupts at the transceiver, and setting the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * enabled by setting the MCU IRQ mask.
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_en()                ENABLE_TRX_IRQ()

/**
 * @brief Enables the transceiver timestamp interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between enabling
 * transceiver interrupts at the transceiver, and setting the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * enabled by setting the MCU IRQ mask.
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_en_tstamp()         ENABLE_TRX_IRQ_TSTAMP()

/**
 * @brief Disables the transceiver main interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between disabling
 * transceiver interrupts at the transceiver, and clearing the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * disabled by clearing the MCU IRQ mask.
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_dis()               DISABLE_TRX_IRQ()

/**
 * @brief Disables the transceiver timestamp interrupt
 *
 * This macro is only available for non-single chip transceivers, since
 * in single chip transceivers there is no separation between disabling
 * transceiver interrupts at the transceiver, and clearing the IRQ mask
 * at the MCU. Therefore the transceiver interrupts in single chips are
 * disabled by clearing the MCU IRQ mask.
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_dis_tstamp()        DISABLE_TRX_IRQ_TSTAMP()
#endif /* #if (PAL_GENERIC_TYPE != MEGA_RF) */

/**
 * @brief Clears the transceiver main interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr()          CLEAR_TRX_IRQ()

/**
 * @brief Clears the transceiver timestamp interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_tstamp()   CLEAR_TRX_IRQ_TSTAMP()

#if (PAL_GENERIC_TYPE == MEGA_RF_)
/**
 * @brief Clears the transceiver RX_END interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_rx_end()   (CLEAR_TRX_IRQ_RX_END())

/**
 * @brief Clears the transceiver TX_END interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_tx_end()   (CLEAR_TRX_IRQ_TX_END())

/**
 * @brief Clears the transceiver CCA_ED_END interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_cca_ed()   (CLEAR_TRX_IRQ_CCA_ED())

/**
 * @brief Clears the transceiver AWAKE interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_awake()    (CLEAR_TRX_IRQ_AWAKE())

#ifdef ENABLE_ALL_TRX_IRQS
/**
 * @brief Clears the transceiver AMI interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_ami()      (CLEAR_TRX_IRQ_AMI())

/**
 * @brief Clears the transceiver BATMON interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_batmon()   (CLEAR_TRX_IRQ_BATMON())

/**
 * @brief Clears the transceiver PLL_LOCK interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_pll_lock() (CLEAR_TRX_IRQ_PLL_LOCK())

/**
 * @brief Clears the transceiver PLL_UNLOCK interrupt
 *
 * @ingroup apiPalApi
 */
#define pal_trx_irq_flag_clr_pll_unlock()   (CLEAR_TRX_IRQ_PLL_UNLOCK())
#endif  /* ENABLE_ALL_TRX_IRQS */
#endif  /* #if (PAL_GENERIC_TYPE == MEGA_RF_) */


/**
 * @brief Enables the global interrupt
 * @ingroup apiPalApi
 */
static inline void pal_global_irq_enable(void)
{
    ENABLE_GLOBAL_IRQ();
}


/**
 * @brief Disables the global interrupt
 * @ingroup apiPalApi
 */
static inline void pal_global_irq_disable(void)
{
    DISABLE_GLOBAL_IRQ();
}



#if defined(ENABLE_HIGH_PRIO_TMR) || defined(DOXYGEN)
/*
 * Internal prototypes for pal_timer module
 */
/**
 * @brief Starts high priority timer
 *
 * This function starts a high priority timer for the specified timeout.
 *
 * @param timer_id Timer identifier
 * @param timer_count Timeout in microseconds
 * @param timer_cb Callback handler invoked upon timer expiry
 * @param param_cb Argument for the callback handler
 *
 * @return
 * - @ref PAL_TMR_INVALID_ID if the identifier is undefined,
 * - @ref MAC_INVALID_PARAMETER if the callback function for this timer is NULL,
 * - @ref PAL_TMR_ALREADY_RUNNING if the timer is already running, or
 * - @ref MAC_SUCCESS if timer is started successfully.
 * @ingroup apiPalApi
 */
retval_t pal_start_high_priority_timer(uint8_t timer_id,
                                       uint16_t timer_count,
                                       FUNC_PTR timer_cb,
                                       void *param_cb);

/**
 * @brief Stops a high priority timer
 *
 * This function stops a high priority timer.
 *
 * @param timer_id Timer identifier
 *
 * @return
 * - @ref PAL_TMR_NOT_RUNNING if the timer id does not match with the high priority
 * timer register, or
 * - @ref MAC_SUCCESS otherwise.
 * @ingroup apiPalApi
 */
retval_t pal_stop_high_priority_timer(uint8_t timer_id);
#endif

/**
 * @brief Generates blocking delay
 *
 * This functions generates a blocking delay of specified time.
 *
 * @param delay in microseconds
 * @ingroup apiPalApi
 */
void pal_timer_delay(uint16_t delay);

/**
 * @brief Selects timer clock source
 *
 * This function selects the clock source of the timer.
 *
 * @param source
 * - @ref TMR_CLK_SRC_DURING_TRX_SLEEP if clock source during sleep is to be selected, and
 * - @ref TMR_CLK_SRC_DURING_TRX_AWAKE if clock source while being awake is selected.
 * @ingroup apiPalApi
 */
void pal_timer_source_select(source_type_t source);

#if (TOTAL_NUMBER_OF_TIMERS > 0) || defined(DOXYGEN)
/**
 * @brief Checks if the timer of requested timer identifier is running
 *
 * This function checks if the timer of requested timer identifier is running.
 *
 * @param timer_id Timer identifier
 *
 * @return
 * - true if timer with requested timer id is running,
 * - false otherwise.
 * @ingroup apiPalApi
 */
bool pal_is_timer_running(uint8_t timer_id);
#endif

/** @cond DOXYGEN_PAL_DEBUG */
bool pal_are_all_timers_stopped(void);
/** @endcond */


/**
 * @brief Adds two time values
 *
 * @param a Time value 1
 * @param b Time value 2
 *
 * @return Addition of a and b
 * @ingroup apiPalApi
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
 * @ingroup apiPalApi
 */
static inline uint32_t pal_sub_time_us(uint32_t a, uint32_t b)
{
    return (SUB_TIME(a, b));
}


/*
 * Macros for TRX GPIO pins access.
 */
/** Macro to set Reset pin to high */
#define PAL_RST_HIGH()                  RST_HIGH()
/** Macro to set Reset pin to low */
#define PAL_RST_LOW()                   RST_LOW()
/** Macro to set SLP_TR pin to high */
#define PAL_SLP_TR_HIGH()               SLP_TR_HIGH()
/** Macro to set SLP_TR pin to low */
#define PAL_SLP_TR_LOW()                SLP_TR_LOW()
/** Macro to get the transceiver's main IRQ status */
#define PAL_TRX_IRQ_HIGH()              IRQ_PINGET()

/**
 * @brief Provides timestamp of the last received frame
 *
 * This function provides the timestamp (in microseconds)
 * of the last received frame.
 *
 * @param[out] timestamp in microseconds
 * @ingroup apiPalApi
 */
void pal_trx_read_timestamp(uint32_t *timestamp);

#ifdef SNIFFER
/**
 * @brief write the timestamp value
 *
 * This function writes the timestamp in the register
 *
 * @param[In] Timestamp in microseconds
 * @ingroup apiPalApi
 */
void pal_trx_write_timestamp(uint32_t *timestamp);
#endif  /* SNIFFER */

/*
 * Prototypes for transceiver access.  Some PALs define these as
 * macros instead of implementing them as functions, so only declare
 * them here if they are not implemented as macros.
 */
#if defined(PAL_USE_SPI_TRX) || defined(DOXYGEN)
/**
 * @brief Reads frame buffer of the transceiver
 *
 * This function reads the frame buffer of the transceiver.
 *
 * @param[out] data Pointer to the location to store frame
 * @param[in] length Number of bytes to be read from the frame
 * buffer.
 * @ingroup apiPalApi
 */
void pal_trx_frame_read(uint8_t* data, uint8_t length);

/**
 * @brief Writes data into frame buffer of the transceiver
 *
 * This function writes data into the frame buffer of the transceiver
 *
 * @param[in] data Pointer to data to be written into frame buffer
 * @param[in] length Number of bytes to be written into frame buffer
 * @ingroup apiPalApi
 */
void pal_trx_frame_write(uint8_t* data, uint8_t length);

/**
 * @brief Reads current value from a transceiver register
 *
 * This function reads the current value from a transceiver register.
 *
 * @param addr Specifies the address of the trx register
 * from which the data shall be read
 *
 * @return value of the register read
 * @ingroup apiPalApi
 */
uint8_t pal_trx_reg_read(uint8_t addr);

/**
 * @brief Writes data into a transceiver register
 *
 * This function writes a value into transceiver register.
 *
 * @param addr Address of the trx register
 * @param data Data to be written to trx register
 *
 * @ingroup apiPalApi
 */
void pal_trx_reg_write(uint8_t addr, uint8_t data);

/**
 * @brief Subregister read
 *
 * @param   addr  offset of the register
 * @param   mask  bit mask of the subregister
 * @param   pos   bit position of the subregister
 *
 * @return  value of the read bit(s)
 * @ingroup apiPalApi
 */
uint8_t pal_trx_bit_read(uint8_t addr, uint8_t mask, uint8_t pos);

/**
 * @brief Subregister write
 *
 * @param[in]   reg_addr  Offset of the register
 * @param[in]   mask  Bit mask of the subregister
 * @param[in]   pos   Bit position of the subregister
 * @param[out]  new_value  Data, which is muxed into the register
 * @ingroup apiPalApi
 */
void pal_trx_bit_write(uint8_t reg_addr, uint8_t mask, uint8_t pos, uint8_t new_value);

#if defined(ENABLE_TRX_SRAM) || defined(DOXYGEN)
/**
 * @brief Reads data from SRAM of the transceiver
 *
 * This function reads from the SRAM of the transceiver
 *
 * @param[in] addr Start address in SRAM for read operation
 * @param[out] data Pointer to the location where data stored
 * @param[in] length Number of bytes to be read from SRAM
 * @ingroup apiPalApi
 */
void pal_trx_sram_read(uint8_t addr, uint8_t *data, uint8_t length);

/**
 * @brief Writes data into SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver
 *
 * @param addr Start address in the SRAM for the write operation
 * @param data Pointer to the data to be written into SRAM
 * @param length Number of bytes to be written into SRAM
 * @ingroup apiPalApi
 */
void pal_trx_sram_write(uint8_t addr, uint8_t *data, uint8_t length);

/**
 * @brief Writes and reads data into/from SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver and
 * simultaneously reads the bytes.
 *
 * @param addr Start address in the SRAM for the write operation
 * @param idata Pointer to the data written/read into/from SRAM
 * @param length Number of bytes written/read into/from SRAM
 * @ingroup apiPalApi
 */
void pal_trx_aes_wrrd(uint8_t addr, uint8_t *idata, uint8_t length);
#endif  /* #ifdef ENABLE_TRX_SRAM */

#endif  /* defined(PAL_USE_SPI_TRX) */


#if defined(SIO_HUB) || defined(DOXYGEN)
/*
 * Prototypes for Stream I/O interface
 */
/**
 * @brief Initializes the requested SIO unit
 *
 * This function initializes the requested SIO unit.
 *
 * @param sio_unit Specifies the SIO uint to be initialized
 *
 * @return MAC_SUCCESS  if SIO unit is initialized successfully, FAILURE otherwise
 * @ingroup apiPalApi
 */
retval_t pal_sio_init(uint8_t sio_unit);

/**
 * @brief Transmits data through selected SIO unit
 *
 * This function transmits data through the selected SIO unit.
 *
 * @param sio_unit Specifies the SIO unit
 * @param data Pointer to the data to be transmitted is present
 * @param length Number of bytes to be transmitted
 *
 * @return Actual number of transmitted bytes
 * @ingroup apiPalApi
 */
uint8_t pal_sio_tx(uint8_t sio_unit, uint8_t *data, uint8_t length);

/**
 * @brief Receives data from selected SIO unit
 *
 * This function receives data from the selected SIO unit.
 *
 * @param sio_unit Specifies SIO unit
 * @param[out] data Pointer to the buffer to store received data
 * @param[in] max_length Maximum number of bytes to be received
 *
 * @return Actual number of received bytes
 * @ingroup apiPalApi
 */
uint8_t pal_sio_rx(uint8_t sio_unit, uint8_t *data, uint8_t max_length);
#endif  /* SIO_HUB */


/**
 * @brief Calibrates the internal RC oscillator
 *
 * @return True if calibration is successful, false otherwise.
 * @ingroup apiPalApi
 */
bool pal_calibrate_rc_osc(void);


/*
 * Prototypes for persistence storage
 */
/**
 * @brief Get data from persistence storage
 *
 * @param[in]  ps_type Persistence storage type
 * @param[in]  start_addr Start offset within EEPROM
 * @param[in]  length Number of bytes to read from EEPROM
 * @param[out] value Data from persistence storage
 *
 * @return MAC_SUCCESS  if everything went OK else FAILURE
 * @ingroup apiPalApi
 */
retval_t pal_ps_get(ps_type_t ps_type, uint16_t start_addr, uint16_t length, void *value);

/**
 * @brief Write data to persistence storage
 *
 * @param[in]  start_addr Start address offset within EEPROM
 * @param[in]  length Number of bytes to be written to EEPROM
 * @param[in]  value Data to persistence storage
 *
 * @return MAC_SUCCESS  if everything went OK else FAILURE
 * @ingroup apiPalApi
 */
retval_t pal_ps_set(uint16_t start_addr, uint16_t length, void *value);


/**
 * @brief Alert indication
 *
 * This Function can be used by any application to indicate an error condition.
 * The function is blocking and does never return.
 * @ingroup apiPalApi
 */
void pal_alert(void);


/** @cond DOXYGEN_PAL_DEBUG */
#define ASSERT(expr)
/** @endcond */


/*
 * Public prototypes for pal_timer module
 */

#if (TOTAL_NUMBER_OF_TIMERS > 0) || defined(DOXYGEN)
/**
 * @brief Start regular timer
 *
 * This function starts a regular timer and installs the corresponding
 * callback function handle the timeout event.
 *
 * @param timer_id Timer identifier
 * @param timer_count Timeout in microseconds
 * @param timeout_type @ref TIMEOUT_RELATIVE or @ref TIMEOUT_ABSOLUTE
 * @param timer_cb Callback handler invoked upon timer expiry
 * @param param_cb Argument for the callback handler
 *
 * @return
 *          - @ref PAL_TMR_INVALID_ID  if the timer identifier is undefined,
 *          - @ref MAC_INVALID_PARAMETER if the callback function for this timer
 *                 is NULL,
 *          - @ref PAL_TMR_ALREADY_RUNNING if the timer is already running.
 *          - @ref MAC_SUCCESS if timer is started or
 *          - @ref PAL_TMR_INVALID_TIMEOUT if timeout is not within timeout range.
 * @ingroup apiPalApi
 */
retval_t pal_timer_start(uint8_t timer_id,
                         uint32_t timer_count,
                         timeout_type_t timeout_type,
                         FUNC_PTR timer_cb,
                         void *param_cb);
#endif

#if (TOTAL_NUMBER_OF_TIMERS > 0) || defined(DOXYGEN)
/**
 * @brief Stops a running timer
 *
 * This function stops a running timer with specified timer_id
 *
 * @param timer_id Timer identifier
 *
 * @return
 *          - @ref MAC_SUCCESS if timer stopped successfully,
 *          - @ref PAL_TMR_NOT_RUNNING if specified timer is not running,
 *          - @ref PAL_TMR_INVALID_ID if the specifed timer id is undefined.
 * @ingroup apiPalApi
 */
retval_t pal_timer_stop(uint8_t timer_id);
#endif

/**
 * @brief Gets current time
 *
 * This function returns the current time.
 *
 * @param[out] current_time Returns current system time
 * @ingroup apiPalApi
 */
void pal_get_current_time(uint32_t *current_time);


/**
 * @brief LED initialization
 */
void pal_led_init(void);

/**
 * @brief LED handling
 *
 * @param led_no LED id
 * @param led_setting LED action
 * @ingroup apiPalApi
 */
void pal_led(led_id_t led_no, led_action_t led_setting);


/**
 * @brief Button initialization
 */
void pal_button_init(void);


/**
 * @brief Button handling
 *
 * @param button_no Button id
 *
 * @return
 *          - @ref BUTTON_PRESSED if button is pressed,
 *          - @ref BUTTON_OFF if button is not pressed.
 * @ingroup apiPalApi
 */
button_state_t pal_button_read(button_id_t button_no);


/**
 * @brief Sleep/Power handling
 *
 * Handles sleep/power modes for entire system
 *
 * @param sleep_mode desired sleep/power mode, see sleep.h
 *
 * @ingroup apiPalApi
 */
void pal_sleep_mode(uint8_t sleep_mode);


#if defined(NON_BLOCKING_SPI) || defined(DOXYGEN)
/**
 * @brief SPI done callback initialization
 *
 * @param spi_done_cb Pointer to SPI done callback function
 * @ingroup apiPalApi
 */
void pal_spi_done_cb_init(void *spi_done_cb);
#endif

#if defined(WATCHDOG) || defined(SLEEPING_TIMER) || defined(DOXYGEN)
#if (PAL_GENERIC_TYPE == XMEGA_)

/**
 * @brief Initialize the watchdog timer of the ATxmega1281
 */
void wdt_init(void);

/**
 * @brief Parallel Software timer initialization if the Sleeping timer (RTC) is
 * not enabled to reset the Watchdog timer periodically.
 */
void wdt_parallel_timer_init(void);

/**
 * @brief Initialization of Sleeping timer (RTC) of the Atxmega1281 when Watchdog Timer
 * is not enabled. The period for the Sleeping timer should be given by the user in this case.
 */
void sleeping_timer_without_wdt(void);

/**
 * @brief Initialization of Sleeping timer (RTC) of the Atxmega1281 when Watchdog Timer
 * is enabled. The period for the Sleeping timer should be given by the user in this case
 * and the period of the sleeping timer should be less than the time-out period of the
 * watchdog timer.
 * @note It is required that the period of the Sleeping Timer (RTC) is less than the Time-out period
 * of Watchdog Timer (If enabled) to avoid the unintentional reset of the device.
 */
void sleeping_timer_with_wdt(void);

/**
 * @brief Initialization of sleeping timer (RTC) of the Atxmega1281
 */
void sleeping_timer_init(void);

/**
 * @brief Resets the watchdog timer of the ATxmega1281 before it
 * reaches its time-out period
 */
void wdt_clear(void);

/** @brief Enable Watchdog and set prescaler.
 *
 *  This function enables the Watchdog and applies prescaler settings.
 *  The Watchdog will be reset automatically.
 *
 *  The function writes the correct signature to the Configuration
 *  Change Protection register before writing the CTRL register. Interrupts are
 *  automatically ignored during the change enable period. The function will
 *  wait for the watchdog to be synchronized to the other clock domains before
 *  proceeding
 *
 *  @param  period  Watchdog Timer timeout period
 */
void pal_wdt_enable( WDT_PER_t period );

/** @brief This function initializes the RTC with period, initial count,
 *         compare value and clock prescaler.
 *
 *  All the synchronized registers are written at the same time to save time.
 *  The program code need to check if the RTC is busy before calling this
 *  function. The RTC_Busy() function can be used to do this.
 *
 *  @param period         RTC period. Topvalue = Period - 1.
 *  @param count          Initial RTC count.
 *  @param compareValue   Compare value.
 *  @param prescaler      Clock prescaler setting.
 *  @note It is required that the period of the Sleeping Timer (RTC) is less than the Time-out period
 *  of Watchdog Timer (If enabled) to avoid the unintentional reset of the device.
 */
void RTC_Initialize( uint16_t period,
                     uint16_t count,
                     uint16_t compareValue,
                     RTC_PRESCALER_t prescaler );

/** @brief This function sets both compare and overflow interrupt levels in
 *         one go.
 *
 *  @param ovfIntLevel  The overflow interrupt level.
 *  @param compIntLevel The compare interrupt level.
 */
void RTC_SetIntLevels( RTC_OVFINTLVL_t ovfIntLevel,
                       RTC_COMPINTLVL_t compIntLevel );
#endif /* (PAL_GENERIC_TYPE == XMEGA_) */
#endif /* (defined(WATCHDOG) || defined(SLEEPING_TIMER) || defined(DOXYGEN)) */

/**
 * @brief Generation of random seed for function srand() in case this
 *        is not supported by the transceiver (e.g. AT86RF230)
 *
 * @return uint16_t Random number to be used as seed for function srand()
 * @ingroup apiPalApi
 */
uint16_t pal_generate_rand_seed(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* PAL_H */
/* EOF */
