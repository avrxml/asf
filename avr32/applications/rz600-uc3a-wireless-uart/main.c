/**
 * \file
 *
 * \brief Main file of the AVR UC3 A3 Wireless UART application.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This application is a Wireless-UART using the USB CDC class (Virtual
 * Communication Port).
 * If a byte is received via the serial connection from one node, the node
 * transmits the byte to the other node. If the transmission was successful,
 * the entered bytes are forwarded to the terminal indicating successful
 * transmission.
 *
 * \section files Main files:
 * - main.c: main file with
 *   - setup the clocks,
 *   - setup the USB
 *   - main loop.
 * - rz600.h: board configuration for the UC3 A3 MCU (SPI config).
 *
 * \section deviceinfo Device Info
 * This application is targeted for the UC3 A3 MCU on RZ600 boards.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <string.h>
#include "compiler.h"
#include "board.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "conf_at86rfx.h"
#include "udd.h"
#include "udc.h"
#include "udi_cdc.h"
#include "at86rfx_driver.h"


/*
 * Buffer size for application to accommodate USB bytes and frame to be
 * transmitted.
*/
#define BUFFER_SIZE       (200)


/**
 * \brief Application task responsible for transferring bytes received from
 *        USB to RF module.
 */
static void app_task(void);

/**
 * \brief USB task responsible for extracting the bytes received from USB
 *        buffer.
 */
static void usb_task(void);

/**
 * Enum to store the current TX state of application.
 */
typedef enum tx_state_tag {
	TX_IDLE = 0,
	TX_ONGOING,
} tx_state_t;

static volatile bool main_b_cdc_enable = false, usb_rx_byte_rcvd = false;
static tx_state_t tx_state = TX_IDLE;;

/* Buffer to hold frame to be transmitted. */
static uint8_t tx_buffer[BUFFER_SIZE];

/* Buffer to hold the bytes from USB. */
static uint8_t usb_rx_buffer[BUFFER_SIZE];
static uint8_t usb_rx_buff_len = 0;


/*! \brief Main function. Execution starts here. */
int main(void)
{
	/* System clock is initialized. */
	sysclk_init();

	/* Board related init such as SPI and GPIO. */
	board_init();

	/* IRQs init. */
	irq_initialize_vectors();

	/* Transceiver init. */
	if (at86rfx_init() != AT86RFX_SUCCESS) {
		Assert("Transceiver initialization failed" == 0);
	}

	/* Interrupt enabled before USB init as it uses IRQ for enumeration. */
	cpu_irq_enable();

	/* Start USB stack */
	udc_start();

	/* Continuous looping of available tasks, starts here. */
	while (true) {
		usb_task();
		app_task();
		at86rfx_task();
	}
}

void at86rfx_tal_rx_frame_cb(uint8_t * rx_frame_array)
{
	/* Frame received from transceiver is sent to terminal through USB. */
	udi_cdc_write_buf((const int *)&rx_frame_array[1],
			rx_frame_array[0] - FCS_LEN);
	/* LED indication for frame reception. */
	LED_Toggle(LED0);
}

void at86rfx_tal_tx_status_cb(uint8_t status)
{
	if (status == AT86RFX_SUCCESS) {
		/* LED indication for successful frame transmission. */
		LED_Toggle(LED1);
		udi_cdc_write_buf((const int *)&tx_buffer[1],
				tx_buffer[0] - FCS_LEN);
		tx_state = TX_IDLE;
	} else if (status == AT86RFX_CHANNEL_ACCESS_FAILURE) {
		/*
		 * Channel access failure is the only transmission failure that is handled
		 * within this application.
		 *
		 * In case of channel access failure the frame is retransmitted
		 */
		at86rfx_tx_frame(tx_buffer);
	} else {
		/*
		 * Other transmission status codes are not handled
		 * within this application.
		 * The transmission is considered as been completed for this frame.
		 */
		tx_state = TX_IDLE;
	}
}

static void app_task(void)
{
	if (tx_state == TX_IDLE) {
		/* If bytes are received via USB, transmit the bytes.  */
		if (usb_rx_buff_len > 0) {
			tx_state = TX_ONGOING;

			/* Introducing critical section to avoid buffer corruption. */
			cpu_irq_disable();

			/* Check for maximum allowed frame length as per IEEE 802.15.4. */
			if (usb_rx_buff_len > (PHY_MAX_LENGTH - FCS_LEN)) {
				usb_rx_buff_len = PHY_MAX_LENGTH - FCS_LEN;
			}

			/* Update ppdu length within frame. */
			tx_buffer[0] = usb_rx_buff_len + FCS_LEN;

			/*
			 * Copy PSDU (actual PHY payload) into frame.
			 * Length of frame has to be first byte during transmission.
			*/
			memcpy(&tx_buffer[1], usb_rx_buffer, usb_rx_buff_len);
			usb_rx_buff_len = 0;

			cpu_irq_enable();

			/* AT86RFx API to transmit the frame. */
			at86rfx_tx_frame(tx_buffer);
		}
	}
}

static void usb_task(void)
{
	if (main_b_cdc_enable) {
		/* Check for any previous call back for Rx from USB. */
		if (usb_rx_byte_rcvd) {
			/* Loop to get all the bytes from the USB to application buffer. */
			while (udi_cdc_is_rx_ready()) {
				uint8_t temp;
				temp = udi_cdc_getc();

				/* Introducing critical section to avoid buffer corruption. */
				cpu_irq_disable();

				usb_rx_buffer[usb_rx_buff_len] = temp;
				usb_rx_buff_len++;

				cpu_irq_enable();

				if (usb_rx_buff_len >= BUFFER_SIZE) {
					break;
				}
			}
			/* Restore the flag after successful copy to application buffer. */
			usb_rx_byte_rcvd = false;
		}
	}
}

bool main_cdc_enable(void)
{
	/* Indication to application when CDC is enabled successfully */
	main_b_cdc_enable = true;
	return true;
}

void main_cdc_disable(void)
{
	main_b_cdc_enable = false;
}

void usb_rx_notify(void)
{
	/* Flag to indicate byte(s) reception in USB. */
	usb_rx_byte_rcvd = true;
}
