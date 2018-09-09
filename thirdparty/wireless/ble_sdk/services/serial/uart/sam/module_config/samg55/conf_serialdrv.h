/**
 * \file
 *
 * \brief SAM G55 serial driver configuration.
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_SERIALDRV_H_INCLUDED
#define CONF_SERIALDRV_H_INCLUDED

#if ((UART_FLOWCONTROL_4WIRE_MODE == true) || (UART_FLOWCONTROL_6WIRE_MODE == true))

#if (BLE_MODULE == BTLC1000_MR)	
	/* BTLC1000 Wakeup Pin */
	#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_4)
	/* BTLC1000 Chip Enable Pin */
	#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_10)
	
	#if SAMG55
	#warning "EXT1 PIN6 is configured as BTLC1000 Wakeup Pin. \
	Inorder to Use USART0 Hardware Flowcontrol, BTLC1000 Wakeup \
	Pin moved to EXT1 PIN 4 and BTLC1000 Chip Enable Pin moved to ETX1 PIN10"
	#endif

# elif (BLE_MODULE == BTLC1000_ZR)
	/* BTLC1000 Wakeup Pin */
	#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_3)
	/* BTLC1000 Chip Enable Pin */
	#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_7)

#endif // BLE_MODULE

#else
/* BTLC1000 Wakeup Pin */
#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_6)

/* BTLC1000 Chip Enable Pin */
#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_4)
#endif

/* BTLC1000 50ms Reset Duration */
#define BTLC1000_RESET_MS			(50)

/* set port pin high */
#define IOPORT_PIN_LEVEL_HIGH		(true)
/* Set port pin low */
#define IOPORT_PIN_LEVEL_LOW		(false)

/** UART Interface */
#define BLE_UART            EXT1_UART_MODULE
#define BLE_UART_ID		    ID_FLEXCOM0
#define BLE_USART_FLEXCOM   FLEXCOM0
#define BLE_UART_IRQn		FLEXCOM0_IRQn
/* Configuration for console uart IRQ handler */
#define BLE_UART_Handler    FLEXCOM0_Handler

/** UART Flow Control Interface */
#define BLE_PATCH_UART            USART5
#define BLE_PATCH_UART_ID	      ID_FLEXCOM5
#define BLE_PATCH_USART_FLEXCOM   FLEXCOM5
#define BLE_PATCH_UART_IRQn       FLEXCOM5_IRQn
/* Configuration for console uart IRQ handler */
#define BLE_PATCH_UART_Handler    FLEXCOM5_Handler

/* This value used to get Rx Timeout at end of Rx frame @115200 3.5Character Timeout used */
#define RX_TIMEOUT_VALUE	35

/** Baudrate setting */
#define CONF_UART_BAUDRATE   (115200UL)
/** Character length setting */
#define CONF_UART_CHAR_LENGTH  US_MR_CHRL_8_BIT
/** Parity setting */
#define CONF_UART_PARITY     US_MR_PAR_NO
/** Stop bits setting */
#define CONF_UART_STOP_BITS    US_MR_NBSTOP_1_BIT

#define SERIAL_DRV_RX_CB plaform_ble_rx_callback
#define SERIAL_DRV_RX_CB_ENABLE  true

#ifdef BLE_OBSERVER_APP
#define BLE_MAX_RX_PAYLOAD_SIZE 512
#else
#define BLE_MAX_RX_PAYLOAD_SIZE 1024
#endif
#define BLE_MAX_TX_PAYLOAD_SIZE 1024

/**
 * BTLC1000 Host Wakeup Control
 * @{
 */

/* BTLC1000 Host Wakeup Pin */
#define BTLC1000_HOST_WAKEUP_PIN				(EXT1_PIN_9)
#define BLE_HOST_UART_RTS_PIN					(EXT1_PIN_5)
#define PIN_BTLC1000_HOST_WAKEUP_PIO			PIOA
#define PIN_BTLC1000_HOST_WAKEUP_MASK			PIO_PA24
#define PIN_BTLC1000_HOST_WAKEUP_ID				ID_PIOA
#define PIN_BTLC1000_HOST_WAKEUP_ATTR			PIO_DEFAULT
#define BTLC1000_HOST_WAKEUP_WAIT_INPUT_ID		(1u << 11)
#define BTLC1000_HOST_WAKEUP_BACKUP_INPUT_ID	(1u << 11)

void platform_host_wake_interrupt_handler(void);
static inline void btlc1000_host_wakeup_config(void);
static inline void btlc1000_host_wakeup_handler(uint32_t ul_id, uint32_t ul_mask);


/* BTLC1000 Host Wakeup Initialization */
static inline void btlc1000_host_wakeup_config(void)
{
	/* Adjust PIO de-bounce filter parameters, Wakeup Immediately */
	pio_set_debounce_filter(PIN_BTLC1000_HOST_WAKEUP_PIO,
						PIN_BTLC1000_HOST_WAKEUP_MASK, 0);

	/* Initialize PIO interrupt handlers, see PIO definition in board.h. */
	pio_handler_set(PIN_BTLC1000_HOST_WAKEUP_PIO, PIN_BTLC1000_HOST_WAKEUP_ID,
					PIN_BTLC1000_HOST_WAKEUP_MASK, PIN_BTLC1000_HOST_WAKEUP_ATTR,
					btlc1000_host_wakeup_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)PIN_BTLC1000_HOST_WAKEUP_ID);

	/* Enable PIO line interrupts. */
	pio_enable_interrupt(PIN_BTLC1000_HOST_WAKEUP_PIO,
				         PIN_BTLC1000_HOST_WAKEUP_MASK);
						 
	/* System Controller Wake-up input for Wakeup Pin-11 and Falling Edge */			
	supc_set_wakeup_inputs(SUPC, SUPC_WUIR_WKUPEN11, SUPC_WUIR_WKUPT11_LOW);
	
	/* set pmc to wakeup from wait mode sleep */
	pmc_set_fast_startup_input(PMC_FSMR_FSTT11);				 
}

/**
 * \brief Handler for button interrupt.
 *
 * \note This interrupt is for waking up from sleep mode or exiting from active
 * mode.
 */
static inline void btlc1000_host_wakeup_handler(uint32_t ul_id, uint32_t ul_mask)
{
	if (PIN_BTLC1000_HOST_WAKEUP_ID == ul_id &&
			PIN_BTLC1000_HOST_WAKEUP_MASK == ul_mask) {
		platform_host_wake_interrupt_handler();
	}
}

static inline bool host_event_data_ready_pin_level(void)
{
	return (ioport_get_pin_level(BTLC1000_HOST_WAKEUP_PIN));
}

static inline bool ble_host_rts_pin_level(void)
{
	return (ioport_get_pin_level(BLE_HOST_UART_RTS_PIN));
}


/* Set BLE Wakeup pin to be low */
static inline bool ble_wakeup_pin_level(void)
{
	return (ioport_get_pin_level(BTLC1000_WAKEUP_PIN));
}

/* Set BLE Wakeup pin to be low */
static inline void ble_wakeup_pin_set_low(void)
{
	ioport_set_pin_level(BTLC1000_WAKEUP_PIN,
	IOPORT_PIN_LEVEL_LOW);
}

/* Set wakeup pin to high */
static inline void ble_wakeup_pin_set_high(void)
{
	ioport_set_pin_level(BTLC1000_WAKEUP_PIN,
	IOPORT_PIN_LEVEL_HIGH);
}

/* Set enable pin to Low */
static inline void ble_enable_pin_set_low(void)
{
	ioport_set_pin_level(BTLC1000_CHIP_ENABLE_PIN,
	IOPORT_PIN_LEVEL_LOW);
}

/* Set enable pin to high */
static inline void ble_enable_pin_set_high(void)
{
	ioport_set_pin_level(BTLC1000_CHIP_ENABLE_PIN,
	IOPORT_PIN_LEVEL_HIGH);
}

/* Configure the BTLC1000 control(chip_enable, wakeup) pins */
static inline void ble_configure_control_pin(void)
{
	/* initialize the delay before use */
	delay_init();

	/* Configure control pins as output */
	ioport_init();
	
	ioport_reset_pin_mode(BTLC1000_WAKEUP_PIN);
	ioport_enable_pin(BTLC1000_WAKEUP_PIN);
	ioport_set_pin_dir(BTLC1000_WAKEUP_PIN, IOPORT_DIR_OUTPUT);

	ioport_reset_pin_mode(BTLC1000_CHIP_ENABLE_PIN);
	ioport_enable_pin(BTLC1000_CHIP_ENABLE_PIN);
	ioport_set_pin_dir(BTLC1000_CHIP_ENABLE_PIN, IOPORT_DIR_OUTPUT);
}

#endif /* CONF_SERIALDRV_H_INCLUDED */
