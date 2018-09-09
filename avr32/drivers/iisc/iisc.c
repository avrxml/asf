/**
 * \file
 *
 * \brief AVR UC3 IISC drivers
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "compiler.h"
#include "iisc.h"

/**
 * \internal
 * \brief IISC Tx Mode Enable
 */
bool tx_is_enabled = false;

/**
 * \internal
 * \brief IISC Rx Mode Enable
 */
bool rx_is_enabled = false;

/*! \brief Sets up registers and initializes IISC for use as I2S.
 *
 *  \param iisc Pointer to the correct volatile avr32_iisc_t struct
 *  \param iisc_opt Option structure for IISC Configuration.
 *  \return Status
 *    \retval IISC_OK when no error occurred.
 *    \retval IISC_ERROR_ARGUMENT when invalid arguments are passed
 */
status_code_t iisc_init(volatile avr32_iisc_t *iisc,
		iisc_opt_t iisc_opt)
{
	unsigned int mr = 0;

	iisc_reset(iisc);

	// In case IISC Master or IISC Slave Mode is selected
	if ((iisc_opt.mode == IISC_MODE_MASTER) ||
			(iisc_opt.mode == IISC_MODE_SLAVE)) {
		mr = (AVR32_IISC_MR_TXSAME_SAME <<
				AVR32_IISC_MR_TXSAME_OFFSET)
				| (AVR32_IISC_MR_TXDMA_SINGLE <<
				AVR32_IISC_MR_TXDMA_OFFSET)
				| (AVR32_IISC_MR_RXLOOP_OFF <<
				AVR32_IISC_MR_RXLOOP_OFFSET)
				| (AVR32_IISC_MR_RXDMA_SINGLE <<
				AVR32_IISC_MR_RXDMA_OFFSET)
				| ( 1 <<  /* stereo: 2 channels -1 */
				AVR32_IISC_NBCHAN_OFFSET)
				| iisc_opt.option;
		if (iisc_opt.mode == IISC_MODE_MASTER) {
			mr |= (AVR32_IISC_MR_IMCKMODE_IMCK <<
					AVR32_IISC_MR_IMCKMODE_OFFSET);
			mr |= (AVR32_IISC_MR_MODE_MASTER <<
					AVR32_IISC_MR_MODE_OFFSET);
		}
		switch (iisc_opt.num_tx_channels) {
		case 1:
			tx_is_enabled = true;
			mr |= AVR32_IISC_MR_TXMONO_MONO << AVR32_IISC_MR_TXMONO_OFFSET;
			break;
		case 2:
			tx_is_enabled = true;
			mr &= ~ AVR32_IISC_MR_TXMONO_MASK;
			mr |= AVR32_IISC_MR_TXMONO_STEREO << AVR32_IISC_MR_TXMONO_OFFSET;
			break;
		default:
			tx_is_enabled = false;
			break;
		}
		switch (iisc_opt.num_rx_channels) {
		case 1:
			rx_is_enabled = true;
			mr |= AVR32_IISC_MR_RXMONO_MONO << AVR32_IISC_MR_RXMONO_OFFSET;
			break;
		case 2:
			rx_is_enabled = true;
			mr &= ~ AVR32_IISC_MR_RXMONO_MASK;
			mr |= AVR32_IISC_MR_RXMONO_STEREO << AVR32_IISC_MR_RXMONO_OFFSET;
			break;
		default:
			rx_is_enabled = false;
			break;
		}
	}
#if (AVR32_IISC_H_VERSION >= 200)
	// In case TDM Mode is selected
	else {
		if (iisc_opt.num_tx_channels != iisc_opt.num_rx_channels) {
			return ERR_INVALID_ARG;
		}
		mr = (AVR32_IISC_MR_IMCKMODE_IMCK <<
				AVR32_IISC_MR_IMCKMODE_OFFSET)
				| (AVR32_IISC_MR_TXSAME_ZERO <<
				AVR32_IISC_MR_TXSAME_OFFSET)
				| (AVR32_IISC_MR_TXDMA_SINGLE <<
				AVR32_IISC_MR_TXDMA_OFFSET)
				| (AVR32_IISC_MR_RXLOOP_OFF <<
				AVR32_IISC_MR_RXLOOP_OFFSET)
				| (AVR32_IISC_MR_RXDMA_SINGLE <<
				AVR32_IISC_MR_RXDMA_OFFSET)
				| (AVR32_IISC_MR_FORMAT_TDM <<
				AVR32_IISC_MR_FORMAT_OFFSET)
				| (iisc_opt.num_tx_channels << AVR32_IISC_MR_NBCHAN_OFFSET)
				| iisc_opt.option;
		if (iisc_opt.mode == IISC_MODE_TDM_MASTER) {
			mr |= (AVR32_IISC_MR_MODE_MASTER <<
					AVR32_IISC_MR_MODE_OFFSET);
		} else {
			mr |= (AVR32_IISC_MR_MODE_SLAVE <<
					AVR32_IISC_MR_MODE_OFFSET);
		}
		tx_is_enabled = true;
		rx_is_enabled = true;
	}
#endif // (AVR32_IISC_H_VERSION >= 200)
	iisc->mr = mr;
	iisc->ier = 0;

	return STATUS_OK;
}

/*! \brief IISC Enable function
 *
 *  \param iisc Pointer to the correct volatile avr32_iisc_t struct
 *
 */
void iisc_enable(volatile avr32_iisc_t *iisc)
{
	if (tx_is_enabled) {
		iisc_enable_transmission(iisc);
	}
	if (rx_is_enabled) {
		iisc_enable_reception(iisc);
	}
	iisc_enable_clocks(iisc);
}

/*! \brief IISC Disable function
 *
 *  \param iisc Pointer to the correct volatile avr32_iisc_t struct
 *
 */
void iisc_disable(volatile avr32_iisc_t *iisc)
{
	if (tx_is_enabled == true) {
		iisc_disable_transmission(iisc);
	}
	if (rx_is_enabled == true) {
		iisc_disable_reception(iisc);
	}
	iisc_disable_clocks(iisc);
}

/*! \brief Write a single message of data
 *
 *  \param iisc Pointer to the correct volatile avr32_iisc_t struct
 *  \param data The data to write
 *
 *  \return Status
 *    \retval IISC_OK when no error occurred.
 *    \retval IISC_TIMEOUT when a timeout occurred while trying to transfer
 */
status_code_t iisc_write(volatile avr32_iisc_t *iisc, uint32_t data)
{
	unsigned int timeout = IISC_RETRY_VALUE;

	while (!(iisc->sr & AVR32_IISC_SR_TXRDY_MASK) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return ERR_TIMEOUT;
	}

	iisc->thr = data;

	return STATUS_OK;
}

/*! \brief Read a single message of data
 *
 *  \param iisc Pointer to the correct volatile avr32_iisc_t struct
 *  \param *data The data to read
 *
 *  \return Status
 *    \retval IISC_OK when no error occurred.
 *    \retval IISC_TIMEOUT when a timeout occurred while trying to transfer
 */
status_code_t iisc_read(volatile avr32_iisc_t *iisc, uint32_t *data)
{
	unsigned int timeout = IISC_RETRY_VALUE;

	while (!(iisc->sr & AVR32_IISC_SR_RXRDY_MASK) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return ERR_TIMEOUT;
	}

	*data = iisc->rhr;

	return STATUS_OK;
}

/*! \brief Disables the specified IISC interrupts.
 *
 * \param iisc Base address of the IISC instance.
 * \param int_mask Bit-mask of IISC interrupts (\c AVR32_IDR_x_MASK).
 */
void iisc_disable_interrupts(volatile avr32_iisc_t *iisc,
		unsigned long int_mask)
{
	irqflags_t      iflags;
	iflags = cpu_irq_save();
	iisc->idr = int_mask;
	// Read back the SR register
	iisc->sr;
	cpu_irq_restore(iflags);
}

/*! \brief Enables the specified IISC interrupts.
 *
 * \param iisc Base address of the IISC instance.
 * \param int_mask Bit-mask of IISC interrupts (\c AVR32_IER_x_MASK).
 */
void iisc_enable_interrupts(volatile avr32_iisc_t *iisc,
		unsigned long int_mask)
{
	iisc->ier = int_mask;
}

/*! \brief Returns the IISC status.
 *
 * \param iisc Base address of the IISC instance.
 *
 * \return The IISC Status Register.
 */
unsigned long iisc_get_status(volatile avr32_iisc_t *iisc)
{
	return iisc->sr;
}
