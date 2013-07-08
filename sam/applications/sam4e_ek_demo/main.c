/**
 * \file
 *
 * \brief FreeRTOS Web/DSP Demo.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage web_dsp_demo FreeRTOS Web/DSP Demo
 *
 * \section Purpose
 *
 * This demo demonstrates how to use the DACC, TC, GMAC, SPI, SMC in a real-time
 * context, using the FreeRTOS kernel. The demo also features the LwIP stack on
 * top of the GMAC driver.
 *
 * \section Requirements
 *
 * This example can be used on the following boards:<br>
 * - SAM4E-EK<br>
 *
 * \section Description
 *
 * The demo relies on several FreeRTOS task to work:
 * - DSP task : based on the selected source mode, the DSP task will process data
 *   from a wave file (loopback mode) or from a generated sinus wave (its frequency
 *   can be changed using the potentiometer). The input signal is then modulated
 *   based on the QTouch slider last known position.
 *   The DSP task then performs a Fast Fourrier Transform to compute the magnitude
 *   of the signal.
 * - GFX task : draws the IP configuration menu, then the input signal source and
 *   magnitude on screen.
 * - Webserver task : handles the LwIP stack and HTTPD callbacks to answer remote
 *   client request.
 *   The device IP can either be configured statically (192.168.0.100) or
 *   dynamically using DHCP.
 * - QTouch task : used to compute the slider's position.
 *
 * Once the IP address is configured from the touch screen interface, the board
 * ressources can be accessed from any web browser providing the IP address. For
 * instance, when the IP address is configured statically :
 * <a href="http://192.168.0.100/">
 *
 * The web page gives the following information about the board status:
 * - CPU core type.
 * - Chip name.
 * - SRAM size.
 * - Flash size.
 * - Audio out connector.
 * - Boot from memory.
 * - Board IP.
 * - Your IP.
 * - Total number of HTTP status requests.
 * - FreeRTOS uptime.
 * - Temperature sensor value.
 * - Led status.
 * - Potentiometer value.
 * - Graph of the FFT input signal magnitude (range from 0 to 10 khz).
 *
 * The web page is periodically refreshed to keep the board status up-to-date.
 * With Chrome browser (V25), sometimes two http webpage requests are needed to
 * get correct status periodical update on browser. This is a known issue.
 * And please refer to the datasheet for the temperature sensor accuracy.
 *
 * For IAR Embedded Workbench IDE project, the source supports both FPU on
 * and FPU off cases. FPU on or FPU off can be configured in
 * IAR project "General Options" by selecting "VFPv4" or "None" respectively.
 * With FPU on, hardware floating-point instruction is used, which can improve
 * the performance of floating-point arithmetic in the DSP task.
 *
 */
#include "ethernet_sam.h"
#include "status_codes.h"
#include "conf_uart_serial.h"
#include "uart_serial.h"
#include "stdio_serial.h"
#include "task_demo.h"

#include "compiler.h"
#include "board.h"
#include "conf_board.h"
#include "ioport.h"

static void prvSetupHardware(void);

/** The priorities at which various tasks will get created. */
#define mainDSP_TASK_PRIORITY          (tskIDLE_PRIORITY + 4)

/** The stack sizes allocated to the DSP stack: (2048 * 4) = 8192 bytes. */
#define mainDSP_TASK_STACK_SIZE        (2048)

/** The priorities at which various tasks will get created. */
#define mainGFX_TASK_PRIORITY          (tskIDLE_PRIORITY + 2)

/** The stack sizes allocated to the DSP stack: (1048 * 4) = 4096 bytes. */
#define mainGFX_TASK_STACK_SIZE        (1048)

/** The priorities at which various tasks will get created. */
#define mainQTOUCH_TASK_PRIORITY       (tskIDLE_PRIORITY + 2)

/** The stack sizes allocated to the DSP stack: (1048 * 4) = 4096 bytes. */
#define mainQTOUCH_TASK_STACK_SIZE     (1048)

/** The priorities at which various tasks will get created. */
#define mainWEBSERVER_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)

/** The stack sizes allocated to the DSP stack: (1048 * 4) = 4096 bytes. */
#define mainWEBSERVER_TASK_STACK_SIZE  (1048)

/*
 * FreeRTOS hook (or callback) functions that are defined in this file.
 */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle pxTask,
		signed char *pcTaskName);
void vApplicationTickHook(void);

extern void xPortSysTickHandler(void);

/**
 * \brief Handler for Sytem Tick interrupt.
 */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}

/**
 * Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/** Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

int main(void)
{
	/** Initilize the SAM system */
	sysclk_init();

	/** Hardware port initialization*/
	prvSetupHardware();

	/** Initialize the console uart */
	configure_console();

	/** Create DSP task. */
	create_dsp_task(mainDSP_TASK_STACK_SIZE, mainDSP_TASK_PRIORITY);

	/** Create GFX task. */
	create_gfx_task(mainGFX_TASK_STACK_SIZE, mainGFX_TASK_PRIORITY);

	/** Create QTouch task. */
	create_qtouch_task(mainQTOUCH_TASK_STACK_SIZE, mainQTOUCH_TASK_PRIORITY);

	/** Create WebServer task. */
	create_webserver_task(mainWEBSERVER_TASK_STACK_SIZE,
			mainWEBSERVER_TASK_PRIORITY);

	/** Start the RTOS scheduler. */
	vTaskStartScheduler();

	/**
	 * If all is well, the scheduler will now be running, and the following
	 * line will never be reached.  If the following line does execute, then
	 * there was insufficient FreeRTOS heap memory available for the idle
	 * and/or timer tasks to be created.  See the memory management section
	 * on the FreeRTOS web site for more details.
	 */
	return 0;
}
/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 * \param sense Sense for interrupt detection (\ref ioport_sense)
 */
#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)

static void prvSetupHardware(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* Initialize IOPORTs */
	ioport_init();

	/* Configure the pins connected to LEDs as output and set their
	 * default initial state to high (LEDs off).
	 */
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);
	ioport_set_pin_dir(LED1_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED1_GPIO, LED0_INACTIVE_LEVEL);
	ioport_set_pin_dir(LED2_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED2_GPIO, LED0_INACTIVE_LEVEL);

	/* Configure Push Button pins */
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_1, GPIO_PUSH_BUTTON_1_FLAGS,
			GPIO_PUSH_BUTTON_1_SENSE);
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_2, GPIO_PUSH_BUTTON_2_FLAGS,
			GPIO_PUSH_BUTTON_2_SENSE);
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_3, GPIO_PUSH_BUTTON_3_FLAGS,
			GPIO_PUSH_BUTTON_3_SENSE);
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_4, GPIO_PUSH_BUTTON_4_FLAGS,
			GPIO_PUSH_BUTTON_4_SENSE);

#ifdef CONF_BOARD_UART_CONSOLE
	/* Configure UART pins */
	ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0,
			PINS_UART0_MASK);
#endif

#ifdef CONF_BOARD_ADS7843
	/* Configure Touchscreen SPI pins */
	ioport_set_pin_dir(BOARD_ADS7843_IRQ_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BOARD_ADS7843_IRQ_GPIO, BOARD_ADS7843_IRQ_FLAGS);
	ioport_set_pin_dir(BOARD_ADS7843_BUSY_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BOARD_ADS7843_BUSY_GPIO, BOARD_ADS7843_BUSY_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
#endif

#ifdef CONF_BOARD_ILI93XX
	/* Configure LCD EBI pins */
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D0,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D1,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D2,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D3,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D4,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D5,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D6,PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D7,PIN_EBI_DATA_BUS_FLAGS);

	ioport_set_pin_peripheral_mode(PIN_EBI_NRD,PIN_EBI_NRD_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NWE,PIN_EBI_NWE_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NCS1,PIN_EBI_NCS1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_LCD_RS,PIN_EBI_LCD_RS_FLAGS);
#endif

#ifdef CONF_BOARD_AAT3155
	/* Configure Backlight control pin */
	ioport_set_pin_dir(BOARD_AAT31XX_SET_GPIO, IOPORT_DIR_OUTPUT);
#endif

#ifdef CONF_BOARD_SPI
	ioport_set_pin_peripheral_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);

#ifdef CONF_BOARD_SPI_NPCS0
	ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
#endif

#ifdef CONF_BOARD_SPI_NPCS3
#if defined(CONF_BOARD_SPI_NPCS3_GPIO) && defined(CONF_BOARD_SPI_NPCS3_FLAGS)
	ioport_set_pin_peripheral_mode(CONF_BOARD_SPI_NPCS3_GPIO,
			CONF_BOARD_SPI_NPCS3_FLAGS);
#else
	ioport_set_pin_peripheral_mode(SPI_NPCS3_PA5_GPIO, SPI_NPCS3_PA5_FLAGS);
#endif
#endif
#endif

#if (defined(CONF_BOARD_TWI0) || defined(CONF_BOARD_QTOUCH))
	ioport_set_pin_peripheral_mode(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	ioport_set_pin_peripheral_mode(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
#endif

#ifdef CONF_BOARD_KSZ8051MNL
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXC_IDX,
			PIN_KSZ8051MNL_RXC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXC_IDX,
			PIN_KSZ8051MNL_TXC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXEN_IDX,
			PIN_KSZ8051MNL_TXEN_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD3_IDX,
			PIN_KSZ8051MNL_TXD3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD2_IDX,
			PIN_KSZ8051MNL_TXD2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD1_IDX,
			PIN_KSZ8051MNL_TXD1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD0_IDX,
			PIN_KSZ8051MNL_TXD0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD3_IDX,
			PIN_KSZ8051MNL_RXD3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD2_IDX,
			PIN_KSZ8051MNL_RXD2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD1_IDX,
			PIN_KSZ8051MNL_RXD1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD0_IDX,
			PIN_KSZ8051MNL_RXD0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXER_IDX,
			PIN_KSZ8051MNL_RXER_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXDV_IDX,
			PIN_KSZ8051MNL_RXDV_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_CRS_IDX,
			PIN_KSZ8051MNL_CRS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_COL_IDX,
			PIN_KSZ8051MNL_COL_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDC_IDX,
			PIN_KSZ8051MNL_MDC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDIO_IDX,
			PIN_KSZ8051MNL_MDIO_FLAGS);
	ioport_set_pin_dir(PIN_KSZ8051MNL_INTRP_IDX, IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_QTOUCH
	/* Configure CHANGE pin for QTouch device */
	ioport_set_pin_input_mode(BOARD_QT_CHANGE_PIN_IDX, BOARD_QT_CHANGE_PIN_FLAGS,
			BOARD_QT_CHANGE_PIN_SENSE);
#endif
}
void vApplicationMallocFailedHook(void)
{
	/**
	 * vApplicationMallocFailedHook() will only be called if
	 * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.
	 * It is a hook function that will get called if a call to
	 * pvPortMalloc() fails. pvPortMalloc() is called internally by
	 * the kernel whenever a task, queue, timer or semaphore is created.
	 * It is also called by various parts of the demo application.
	 * If heap_1.c or heap_2.c are used, then the size of the heap
	 * available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can
	 * be used to query the size of free heap space that remains
	 * (although it does not provide information on how the remaining heap
	 * might be fragmented).
	 */
	taskDISABLE_INTERRUPTS();
	for (;;) {
	}
}

void vApplicationIdleHook(void)
{
	/**
	 * vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK
	 * is set to 1 in FreeRTOSConfig.h.  It will be called on each iteration
	 * of the idle task.  It is essential that code added to this hook
	 * function never attempts to block in any way (for example, call
	 * xQueueReceive() with a block time specified, or call vTaskDelay()).
	 * If the application makes use of the vTaskDelete() API function
	 * (as this demo application does) then it is also important that
	 * vApplicationIdleHook() is permitted to return to its calling
	 * function, because it is the responsibility of the idle task to
	 * clean up memory allocated by the kernel to any task that has
	 * since been deleted.
	 */
}

void vApplicationStackOverflowHook(xTaskHandle pxTask,
		signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	/**
	 * Run time stack overflow checking is performed if
	 * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.
	 * This hook function is called if a stack overflow is
	 * detected.
	 */
	taskDISABLE_INTERRUPTS();
	for (;;) {
	}
}

void vApplicationTickHook(void)
{
	/**
	 * This function will be called by each tick interrupt if
	 * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.
	 * User code can be added here, but the tick hook is called from
	 * an interrupt context, so code must not attempt to block,
	 * and only the interrupt safe FreeRTOS API
	 * functions can be used (those that end in FromISR()).
	 */
}
