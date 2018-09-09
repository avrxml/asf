/*****************************************************************************
 *
 * \file
 *
 * \brief SPI driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the SPI interface on AVR32
 * devices.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _UC3_SPI_H_
#define _UC3_SPI_H_

/**
 * \defgroup group_avr32_drivers_spi SPI - Serial Peripheral Interface
 *
 * See \ref avr32_drivers_spi_quick_start
 *
 * Driver for the SPI (Serial Peripheral Interface).
 * The SPI circuit is a synchronous serial data link that provides communication
 * with external devices in Master or Slave mode. Connection to Peripheral DMA 
 * Controller channel capabilities optimizes data transfers.
 *
 * @{
 */

#include "compiler.h"

#ifdef FREERTOS_USED
#  include "FreeRTOS.h"
#  include "semphr.h"

/** The SPI mutex. */
extern xSemaphoreHandle xSPIMutex;
#endif

/** Time-out value (number of attempts). */
#define SPI_TIMEOUT       15000

/** SPI Mode 0. */
#define SPI_MODE_0        0

/** SPI Mode 1. */
#define SPI_MODE_1        1

/** SPI Mode 2. */
#define SPI_MODE_2        2

/** SPI Mode 3. */
#define SPI_MODE_3        3

/** Status codes used by the SPI driver. */
typedef enum {
	SPI_ERROR = -1,
	SPI_OK = 0,
	SPI_ERROR_TIMEOUT = 1,
	SPI_ERROR_ARGUMENT,
	SPI_ERROR_OVERRUN,
	SPI_ERROR_MODE_FAULT,
	SPI_ERROR_OVERRUN_AND_MODE_FAULT
} spi_status_t;

/** Option structure for SPI channels. */
typedef struct {
	/** The SPI channel to set up. */
	uint8_t reg;

	/** Preferred baudrate for the SPI. */
	uint32_t baudrate;

	/** Number of bits in each character (8 to 16). */
	uint8_t bits;

	/** Delay before first clock pulse after selecting slave (in PBA clock
	 *  periods). */
	uint8_t spck_delay;

	/** Delay between each transfer/character (in PBA clock periods). */
	uint8_t trans_delay;

	/** Sets this chip to stay active after last transfer to it. */
	uint8_t stay_act;

	/** Which SPI mode to use when transmitting. */
	uint8_t spi_mode;

	/** Disables the mode fault detection. With this bit cleared, the SPI
	 *  master mode will disable itself if another  master tries to address
	 *  it. */
	uint8_t modfdis;
} spi_options_t;

/** \brief Reset the SPI.
 *
 * \param spi       Base address of the SPI instance.
 *
 */
static inline void spi_reset(volatile avr32_spi_t *spi)
{
	spi->cr = AVR32_SPI_CR_SWRST_MASK;
}

/** \brief Set Master Mode of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 */
static inline void spi_set_master_mode(volatile avr32_spi_t *spi)
{
	spi->MR.mstr = 1;
}

/** \brief Set Slave Mode of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 */
static inline void spi_set_slave_mode(volatile avr32_spi_t *spi)
{
	spi->MR.mstr = 0;
}

/** \brief Enable Modfault of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_modfault(volatile avr32_spi_t *spi)
{
	spi->MR.modfdis = 0;
}

/** \brief Disable Modfault of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_disable_modfault(volatile avr32_spi_t *spi)
{
	spi->MR.modfdis = 1;
}

/** \brief Enable Loopback of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_loopback(volatile avr32_spi_t *spi)
{
	spi->MR.llb = 1;
}

/** \brief Disable Loopback of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_disable_loopback(volatile avr32_spi_t *spi)
{
	spi->MR.llb = 0;
}

/** \brief Enable Chip Select Decoding of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_enable_chipselect_decoding(volatile avr32_spi_t *spi)
{
	spi->MR.pcsdec = 1;
}

/** \brief Disable Chip Select Decoding of the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
static inline void spi_disable_chipselect_decoding(volatile avr32_spi_t *spi)
{
	spi->MR.pcsdec = 0;
}

/** \brief Set Chip Select of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 */
static inline void spi_set_chipselect(volatile avr32_spi_t *spi,
		uint8_t chip_select)
{
	spi->MR.pcs = chip_select;
}

/** \brief Enable Variable Chip Select of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 */
static inline void spi_enable_variable_chipselect(volatile avr32_spi_t *spi)
{
	spi->MR.ps = 1;
}

/** \brief Disable Variable Chip Select of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 */
static inline void spi_disable_variable_chipselect(volatile avr32_spi_t *spi)
{
	spi->MR.ps = 0;
}

/** \brief Set Delay Between Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param delay       Delay.
 */
static inline void spi_set_delay(volatile avr32_spi_t *spi, uint8_t delay)
{
	spi->MR.dlybcs = delay;
}

/** \brief Set Delay Between Consecutive Transfer on a Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 * \param delay       Delay.
 */

static inline void spi_set_chipselect_delay_bct(volatile avr32_spi_t *spi,
		uint8_t chip_select, uint8_t delay)
{
	Assert(chip_select <= 3);

	switch (chip_select) {
	case 0:
		spi->CSR0.dlybct = delay;
		break;

	case 1:
		spi->CSR1.dlybct  = delay;
		break;

	case 2:
		spi->CSR2.dlybct  = delay;
		break;

	case 3:
		spi->CSR3.dlybct  = delay;
		break;
	}
}

/** \brief Set Delay Before SPCK on a Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 * \param delay       Delay.
 */
static inline void spi_set_chipselect_delay_bs(volatile avr32_spi_t *spi,
		uint8_t chip_select, uint8_t delay)
{
	Assert(chip_select <= 3);

	switch (chip_select) {
	case 0:
		spi->CSR0.dlybs = delay;
		break;

	case 1:
		spi->CSR1.dlybs  = delay;
		break;

	case 2:
		spi->CSR2.dlybs  = delay;
		break;

	case 3:
		spi->CSR3.dlybs  = delay;
		break;
	}
}

/** \brief Set Delay Before SPCK on a Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 * \param len         Bits per Transfer [8...16].
 */
static inline void spi_set_bits_per_transfer(volatile avr32_spi_t *spi,
		uint8_t chip_select,
		uint8_t len)
{
	Assert((len >= 8) && (len <= 16));

	switch (chip_select) {
	case 0:
		spi->CSR0.bits = len - 8;
		break;

	case 1:
		spi->CSR1.bits  = len - 8;
		break;

	case 2:
		spi->CSR2.bits  = len - 8;
		break;

	case 3:
		spi->CSR3.bits  = len - 8;
		break;
	}
}

/** \brief Set baudrate for a Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 * \param scbr        Baudrate Register.
 */
static inline void spi_set_baudrate_register(volatile avr32_spi_t *spi,
		uint8_t chip_select,
		uint8_t scbr)
{
	switch (chip_select) {
	case 0:
		spi->CSR0.scbr = scbr;
		break;

	case 1:
		spi->CSR1.scbr  = scbr;
		break;

	case 2:
		spi->CSR2.scbr  = scbr;
		break;

	case 3:
		spi->CSR3.scbr  = scbr;
		break;
	}
}

/** \brief Enable Active mode of a Chip Selects of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 */
static inline void spi_enable_active_mode(volatile avr32_spi_t *spi,
		uint8_t chip_select)
{
	switch (chip_select) {
	case 0:
		spi->CSR0.csaat = 1;
		break;

	case 1:
		spi->CSR1.csaat  = 1;
		break;

	case 2:
		spi->CSR2.csaat  = 1;
		break;

	case 3:
		spi->CSR3.csaat  = 1;
		break;
	}
}

/** \brief Set Mode of the SPI.
 *
 * \param spi         Base address of the SPI instance.
 * \param chip_select Chip Select.
 * \param flags       SPI Mode.
 */
static inline void spi_set_mode(volatile avr32_spi_t *spi, uint8_t chip_select,
		uint8_t flags)
{
	switch (chip_select) {
	case 0:
		spi->CSR0.cpol = flags >> 1;
		spi->CSR0.ncpha = (flags & 0x1) ^ 0x1;
		break;

	case 1:
		spi->CSR1.cpol  = flags >> 1;
		spi->CSR1.ncpha = (flags & 0x1) ^ 0x1;
		break;

	case 2:
		spi->CSR2.cpol  = flags >> 1;
		spi->CSR2.ncpha = (flags & 0x1) ^ 0x1;
		break;

	case 3:
		spi->CSR3.cpol  = flags >> 1;
		spi->CSR3.ncpha = (flags & 0x1) ^ 0x1;
		break;
	}
}

/** \brief Put one data to a SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \param data The data byte to be loaded
 *
 */
static inline void spi_put(volatile avr32_spi_t *spi, uint16_t data)
{
	spi->tdr = data << AVR32_SPI_TDR_TD_OFFSET;
}

/** \brief Get one data to a SPI peripheral.
 *
 * \param spi Base address of the SPI instance.
 * \return The data byte
 *
 */
static inline uint16_t spi_get(volatile avr32_spi_t *spi)
{
	return (spi->rdr >> AVR32_SPI_RDR_RD_OFFSET);
}

/** \brief Checks if all transmissions are complete.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  All transmissions complete.
 *   \retval 0  Transmissions not complete.
 */
static inline bool spi_is_tx_empty(volatile avr32_spi_t *spi)
{
	return (spi->sr & AVR32_SPI_SR_TXEMPTY_MASK) != 0;
}

/** \brief Checks if all transmissions is ready.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  All transmissions complete.
 *   \retval 0  Transmissions not complete.
 */
static inline bool spi_is_tx_ready(volatile avr32_spi_t *spi)
{
	return (spi->sr & AVR32_SPI_SR_TDRE_MASK) != 0;
}

/** \brief Check if the SPI contains a received character.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI Receive Holding Register is full, otherwise \c 0.
 */
static inline bool spi_is_rx_full(volatile avr32_spi_t *spi)
{
	return (spi->sr & AVR32_SPI_SR_RDRF_MASK) != 0;
}

/** \brief Checks if all reception is ready.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c 1 if the SPI Receiver is ready, otherwise \c 0.
 */
static inline bool spi_is_rx_ready(volatile avr32_spi_t *spi)
{
	return (spi->sr &
	       (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) ==
	       (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK);
}

/** \brief Resets the SPI controller.
 *
 * \param spi Base address of the SPI instance.
 */
void spi_reset(volatile avr32_spi_t *spi);

/** \brief Initializes the SPI in slave mode.
 *
 * \param spi       Base address of the SPI instance.
 * \param bits      Number of bits in each transmitted character (8 to 16).
 * \param spi_mode  Clock polarity and phase.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
spi_status_t spi_initSlave(volatile avr32_spi_t *spi,
		uint8_t bits,
		uint8_t spi_mode);

/** \brief Sets up the SPI in a test mode where the transmitter is connected to
 *         the receiver (local loopback).
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval SPI_OK Success.
 */
spi_status_t spi_initTest(volatile avr32_spi_t *spi);

/** \brief Initializes the SPI in master mode.
 *
 * \param spi     Base address of the SPI instance.
 * \param options Pointer to a structure containing initialization options.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
spi_status_t spi_initMaster(volatile avr32_spi_t *spi,
		const spi_options_t *options);

/** \brief Calculates the baudrate divider.
 *
 * \param baudrate Baudrate value.
 * \param pb_hz  SPI module input clock frequency (PBA clock, Hz).
 *
 * \return Divider or error code.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
int16_t getBaudDiv(const uint32_t baudrate, uint32_t pb_hz);

/** \brief Sets up how and when the slave chips are selected (master mode only).
 *
 * \param spi         Base address of the SPI instance.
 * \param variable_ps Target slave is selected in transfer register for every
 *                    character to transmit.
 * \param pcs_decode  The four chip select lines are decoded externally. Values
 *                    0 to 14 can be given to \ref spi_selectChip.
 * \param delay       Delay in PBA periods between chip selects.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
spi_status_t spi_selectionMode(volatile avr32_spi_t *spi,
		uint8_t variable_ps,
		uint8_t pcs_decode,
		uint8_t delay);

/** \brief Selects slave chip.
 *
 * \param spi   Base address of the SPI instance.
 * \param chip  Slave chip number (normal: 0 to 3, externally decoded signal: 0
 *              to 14).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
spi_status_t spi_selectChip(volatile avr32_spi_t *spi,
		uint8_t chip);

/** \brief Unselects slave chip.
 *
 * \param spi   Base address of the SPI instance.
 * \param chip  Slave chip number (normal: 0 to 3, externally decoded signal: 0
 *              to 14).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if last transmission
 *       is not complete. Invoke \ref spi_writeEndCheck beforehand if needed.
 */
spi_status_t spi_unselectChip(volatile avr32_spi_t *spi,
		uint8_t chip);

/** \brief Sets options for a specific slave chip.
 *
 * The baudrate field has to be written before transfer in master mode. Four
 * similar registers exist, one for each slave. When using encoded slave
 * addressing, reg=0 sets options for slaves 0 to 3, reg=1 for slaves 4 to 7 and
 * so on.
 *
 * \param spi     Base address of the SPI instance.
 * \param options Pointer to a structure containing initialization options for
 *                an SPI channel.
 * \param pb_hz  SPI module input clock frequency (PBA clock, Hz).
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 */
spi_status_t spi_setupChipReg(volatile avr32_spi_t *spi,
		const spi_options_t *options,
		uint32_t pb_hz);

/** \brief Enables the SPI.
 *
 * \param spi Base address of the SPI instance.
 */
void spi_enable(volatile avr32_spi_t *spi);

/** \brief Disables the SPI.
 *
 * Ensures that nothing is transferred while setting up buffers.
 *
 * \param spi Base address of the SPI instance.
 *
 * \warning This may cause data loss if used on a slave SPI.
 */
void spi_disable(volatile avr32_spi_t *spi);

/** \brief Tests if the SPI is enabled.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return \c true if the SPI is enabled, otherwise \c false.
 */
bool spi_is_enabled(volatile avr32_spi_t *spi);

/** \brief Checks if there is no data in the transmit register.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  No data in TDR.
 *   \retval 0  Some data in TDR.
 */
uint8_t spi_writeRegisterEmptyCheck(volatile avr32_spi_t *spi);

/** \brief Writes one data word in master fixed peripheral select mode or in
 *         slave mode.
 *
 * \param spi   Base address of the SPI instance.
 * \param data  The data word to write.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if transmitter is
 *       busy and transmit buffer is full. Invoke
 *       \ref spi_writeRegisterEmptyCheck beforehand if needed.
 *
 * \note Once the data has been written to the transmit buffer, the end of
 *       transmission is not waited for. Invoke \ref spi_writeEndCheck if
 *       needed.
 */
spi_status_t spi_write(volatile avr32_spi_t *spi, uint16_t data);

/** \brief Selects a slave in master variable peripheral select mode and writes
 *         one data word to it.
 *
 * \param spi       Base address of the SPI instance.
 * \param data      The data word to write.
 * \param pcs       Slave selector (bit 0 -> nCS line 0, bit 1 -> nCS line 1,
 *                  etc.).
 * \param lastxfer  Boolean indicating whether this is the last data word
 *                  transfer.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *   \retval SPI_ERROR_ARGUMENT Invalid argument(s) passed.
 *
 * \note Will block program execution until time-out occurs if transmitter is
 *       busy and transmit buffer is full. Invoke
 *       \ref spi_writeRegisterEmptyCheck beforehand if needed.
 *
 * \note Once the data has been written to the transmit buffer, the end of
 *       transmission is not waited for. Invoke \ref spi_writeEndCheck if
 *       needed.
 */
spi_status_t spi_variableSlaveWrite(volatile avr32_spi_t *spi,
		uint16_t data,
		uint8_t pcs,
		uint8_t lastxfer);

/** \brief Checks if all transmissions are complete.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  All transmissions complete.
 *   \retval 0  Transmissions not complete.
 */
uint8_t spi_writeEndCheck(volatile avr32_spi_t *spi);

/** \brief Checks if there is data in the receive register.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval 1  Some data in RDR.
 *   \retval 0  No data in RDR.
 */
uint8_t spi_readRegisterFullCheck(volatile avr32_spi_t *spi);

/** \brief Reads one data word in master mode or in slave mode.
 *
 * \param spi   Base address of the SPI instance.
 * \param data  Pointer to the location where to store the received data word.
 *
 * \return Status.
 *   \retval SPI_OK             Success.
 *   \retval SPI_ERROR_TIMEOUT  Time-out.
 *
 * \note Will block program execution until time-out occurs if no data is
 *       received or last transmission is not complete. Invoke
 *       \ref spi_writeEndCheck or \ref spi_readRegisterFullCheck beforehand if
 *       needed.
 */
spi_status_t spi_read(volatile avr32_spi_t *spi, uint16_t *data);

/** \brief Gets status information from the SPI.
 *
 * \param spi Base address of the SPI instance.
 *
 * \return Status.
 *   \retval SPI_OK                           Success.
 *   \retval SPI_ERROR_OVERRUN                Overrun error.
 *   \retval SPI_ERROR_MODE_FAULT             Mode fault (SPI addressed as slave
 *                                            while in master mode).
 *   \retval SPI_ERROR_OVERRUN_AND_MODE_FAULT Overrun error and mode fault.
 */
uint8_t spi_getStatus(volatile avr32_spi_t *spi);

/**
 * @}
 */

/**
 * \page avr32_drivers_spi_quick_start Quick start guide for SPI driver on AVR32 devices
 *
 * This is the quick start guide for the \ref group_avr32_drivers_spi,
 * with step-by-step instructions on how to configure and use the driver for a
 * specific use case.
 * The code examples can be copied into e.g the main application loop or any
 * other function that will need to control the SPI
 *
 * \section driver_spi_basic Basic setup
 * The SPI module will be set up as master:
 *  - SPI on module SPI0
 *  - 1MHz SPI clock speed
 *  - Slave Chip Select connected on NPCS1
 *  - 8 bits per transfer
 *  - SPI mode 0 (data on rising clock edge)
 *
 * \subsection driver_spi_basic_prereq Prerequisites
 *
 * This module requires the following drivers
 * - \ref sysclk_driver
 * \section module_basic_setup Setup steps
 * \subsection driver_spi_basic_setup_code Example code
 * Add to application C-file (e.g. main.c):
 * \code
	 #define SPI_EXAMPLE             (&AVR32_SPI0)
	 #define SPI_SLAVECHIP_NUMBER    (1)

	  spi_options_t my_spi_options={
	    // The SPI channel to set up : Memory is connected to CS1
	    SPI_SLAVECHIP_NUMBER,
	    // Preferred baudrate for the SPI.
	    1000000,
	    // Number of bits in each character (8 to 16).
	    8,
	    // Delay before first clock pulse after selecting slave (in PBA clock
	 periods).
	    0,
	    // Delay between each transfer/character (in PBA clock periods).
	    0,
	    // Sets this chip to stay active after last transfer to it.
	    1,
	    // Which SPI mode to use when transmitting.
	    SPI_MODE_0,
	    // Disables the mode fault detection.
	    // With this bit cleared, the SPI master mode will disable itself if
	 another
	    // master tries to address it.
	    1
	  };

	 void spi_init_module(void)
	 {
	   //Init SPI module as master
	   spi_initMaster(SPI_EXAMPLE,&my_spi_options);
	   //Setup configuration for chip connected to CS1
	   spi_setupChipReg(SPI_EXAMPLE,&my_spi_options,sysclk_get_pba_hz());
	   //Allow the module to transfer data
	   spi_enable(SPI_EXAMPLE);
	 }
\endcode
 *
 * \subsection driver_spi_basic_setup Workflow
 * -# Ensure that board_init() has configured selected I/Os for SPI function.
 * -# Ensure that sysclk_init() is called at the beginning of the main function.
 * -# Define an alias for the SPI module you want to use :
 * \code
	#define SPI_EXAMPLE             (&AVR32_SPI0)
\endcode
 * -# Define an alias for the slave device SPI module you want to use :
 * \code
	#define SPI_SLAVECHIP_NUMBER    (1)
\endcode
 * -# Create an spi_options_t structure for the your slave device :
 * This configure the SPI module to suit the SPI format of the slave device you
 * want to communicate with
 * \code
	spi_options_t my_spi_options={
	  // The SPI channel to set up : Memory is connected to CS1
	  SPI_SLAVECHIP_NUMBER,
	  // Preferred baudrate for the SPI.
	  1000000,
	  // Number of bits in each character (8 to 16).
	  8,
	  // Delay before first clock pulse after selecting slave (in PBA clock periods).
	  0,
	  // Delay between each transfer/character (in PBA clock periods).
	  0,
	  // Sets this chip to stay active after last transfer to it.
	  1,
	  // Which SPI mode to use when transmitting.
	  SPI_MODE_0,
	  // Disables the mode fault detection.
	  // With this bit cleared, the SPI master mode will disable itself if another
	  // master tries to address it.
	  1
	};
\endcode
 * -# Write the initialization function to setup the module :
 * \code
	void spi_init_module(void)
	{
	  //Init SPI module as master
	  spi_initMaster(SPI_EXAMPLE,&my_spi_options);
	  //Setup configuration for chip connected to CS1
	  spi_setupChipReg(SPI_EXAMPLE,&my_spi_options,sysclk_get_pba_hz());
	  //Allow the module to transfer data
	  spi_enable(SPI_EXAMPLE);
	}
\endcode
 *  - \note The last argument of spi_setupChipReg, is the PBA Clock frequency
 *  This frequency is given by the sysclk module and is used to compute the
 *  correct baudrate for the SPI.
 *
 * -# Call the initialization routine from your application.
 *  - \code
	spi_init_module();
\endcode
 *
 *
 * \section driver_spi_basic_usage Usage steps
 * \subsection driver_spi_basic_usage_code Example code
 * Use in application C-file:
 * \code
	 //Buffer to send data to SPI slave
	 uint16_t txdata;
	 //Buffer to receive data from SPI slave
	 uint16_t rxdata;
	 ...
	 //Select given device on the SPI bus
	 spi_selectChip(SPI_EXAMPLE, SPI_SLAVECHIP_NUMBER);
	 //Wait for the transmitter to be ready
	 while(!spi_is_tx_ready(SPI_EXAMPLE))
	   ;
	 // Send the data to slave (ie = AT45DBX_CMDC_RD_STATUS_REG)
	 txdata=0xD7;
	 spi_put(SPI_EXAMPLE,txdata);
	 //Wait for a complete transmission
	 while(!spi_is_tx_empty(SPI_EXAMPLE))

	 //Wait for the transmitter to be ready
	 while(!spi_is_tx_ready(SPI_EXAMPLE))
	   ;
	 // Send dummy data to slave (ie = 0x00)
	 txdata=0x00;
	 spi_put(SPI_EXAMPLE,txdata);
	 //Wait for a complete transmission
	 while(!spi_is_tx_empty(SPI_EXAMPLE))
	   ;
	 //Now simply read the data in the receive register
	 rxdata=spi_get(SPI_EXAMPLE);

	 // Deselect the slave
	 spi_unselectChip(SPI_EXAMPLE,SPI_SLAVECHIP_NUMBER);
\endcode
 *
 * \subsection driver_spi_basic_usage_flow Workflow
 * -# Create two buffers for data to be sent/received on the SPI bus,
 *  - \code
	//Buffer to send data to SPI slave
	uint16_t txdata;
	//Buffer to receive data from SPI slave
	uint16_t rxdata;
\endcode
 * -# Call the spi_selectChip routine to enable the chip select line for
 * the slave you want to communicate with.
 * \code
	spi_selectChip(SPI_EXAMPLE,SPI_SLAVECHIP_NUMBER);
\endcode
 * -# Wait for the SPI transmitter to be ready before sending data
 *  - \code
	//Wait for the transmitter to be ready
	while(!spi_is_tx_ready(SPI_EXAMPLE))
	  ;
\endcode
 * -# Set and send the data to slave
 * txdata is set 0xD7 (= AT45DBX_CMDC_RD_STATUS_REG) which is a status read
 * command to an Atmel SPI memory chip.
 *  - \code
	txdata=0xD7;
	spi_put(SPI_EXAMPLE,txdata);
\endcode
 *
 * -# Then wait for the data to be sent by SPI module
 *  - \code
	//Wait for a complete transmission
	while(!spi_is_tx_empty(SPI_EXAMPLE))
\endcode
 * -# As the SPI works as a shift register, data is shifted in at
 * the same time as data is shifted out. A read operation will mean that a
 * dummy byte is written to the SPI bus and data is read at the same time.
 * As we are sending, we have to check again for the SPI module to be ready
 * before sending and also check for the completion of this send operation.
 *  - \code
	//Wait for the transmitter to be ready
	while(!spi_is_tx_ready(SPI_EXAMPLE))
	  ;
	// Send dummy data to slave (ie = 0x00)
	txdata=0x00;
	spi_put(SPI_EXAMPLE,txdata);
	//Wait for a complete transmission
	while(!spi_is_tx_empty(SPI_EXAMPLE))
	  ;
\endcode
 * -# Now the dummy data is sent the value of the receive register
 * is the data from slave
 *  - \code
	//Now simply read the data in the receive register
	rxdata=spi_get(SPI_EXAMPLE);
\endcode
 * -# When read and write operations is done, de-select the slave:
 *  - \code
	spi_unselectChip(SPI_EXAMPLE,SPI_SLAVECHIP_NUMBER);
\endcode
 * -# Now you can use the data read from the slave which is in rxdata
 *  - \code
	//Check read content
	if(rxdata<0x3C)
	  do_something();
\endcode
 */

#endif  /* _UC3_SPI_H_ */
