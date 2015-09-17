/**
 * \file
 *
 * \brief FreeRTOS demo task implementations.
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include <conf_demo.h>
#include "demotasks.h"

/**
 * \addtogroup freertos_sam0_demo_tasks_group
 *
 * @{
 */

//! \name Task configuration
//@{

#define UART_TASK_PRIORITY      (tskIDLE_PRIORITY + 3)
#define UART_TASK_DELAY         (10 / portTICK_RATE_MS)

#define MAIN_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define MAIN_TASK_DELAY         (100 / portTICK_RATE_MS)

#define GRAPH_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define GRAPH_TASK_DELAY        (50 / portTICK_RATE_MS)

#define TERMINAL_TASK_PRIORITY  (tskIDLE_PRIORITY + 1)
#define TERMINAL_TASK_DELAY     (1000 / portTICK_RATE_MS)

#define ABOUT_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define ABOUT_TASK_DELAY        (33 / portTICK_RATE_MS)

//@}


//! \name Menu and display configuration
//@{

//! Available selections in menu
enum menu_items {
	MENU_ITEM_GRAPH,
	MENU_ITEM_TERMINAL,
	MENU_ITEM_ABOUT,
	MENU_NUM_ITEMS,
};

//! Height of menu bar
#define MENU_HEIGHT      8

//! Width per menu item
#define MENU_ITEM_WIDTH  \
	(((GFX_MONO_LCD_WIDTH - (MENU_NUM_ITEMS - 1))) / MENU_NUM_ITEMS)

//! Height of area in which to draw content
#define CANVAS_HEIGHT  \
	((GFX_MONO_LCD_HEIGHT / 2) - (MENU_HEIGHT + 1))

//! Width of area in which to draw content
#define CANVAS_WIDTH           (GFX_MONO_LCD_WIDTH)

//! Offset of Y-coordinate for display buffer of graph
#define CANVAS_GRAPH_Y_OFFSET  (GFX_MONO_LCD_HEIGHT / 2)

//! Character lines on display
#define TERMINAL_LINES  \
	(1 + ((CANVAS_HEIGHT - SYSFONT_HEIGHT) / (SYSFONT_HEIGHT + 1)))

//! Character columns on display
#define TERMINAL_COLUMNS         (CANVAS_WIDTH / SYSFONT_WIDTH)

//! Character lines in terminal buffer
#define TERMINAL_BUFFER_LINES    (1 + TERMINAL_LINES)

//! Character columns in terminal buffer
#define TERMINAL_BUFFER_COLUMNS  (1 + TERMINAL_COLUMNS)

//@}


//! \name Global constants and variables
//@{

//! Labels for menu items
static const char menu_items_text[MENU_NUM_ITEMS][6] = {
	"Graph",
	"Term.",
	"About",
};

//! Text to display on about screen when tickless operation is disabled
static const char about_text[] =
	"FreeRTOS "tskKERNEL_VERSION_NUMBER" demo."
	"                     "
	"Use CDC at 9.6 kBaud.";

//! Text to display on about screen when tickless operation is enabled
static const char about_text_tickless[] =
	"FreeRTOS "tskKERNEL_VERSION_NUMBER" demo."
	"      TickLess       "
	"Use CDC at 9.6 kBaud.";

/**
 * \brief Instance for \ref oled1_xpro_io_group
 *
 * The extension header to use is configured with \ref OLED1_EXT_HEADER.
 */
static OLED1_CREATE_INSTANCE(oled1, OLED1_EXT_HEADER);

//! Instance for \ref edbg_cdc_rx_group
static struct usart_module cdc_usart;

//! Pseudo-random noise for graph task
static uint8_t graph_noise = 128;

//! Buffer for terminal text
static uint8_t terminal_buffer[TERMINAL_BUFFER_LINES][TERMINAL_BUFFER_COLUMNS];

//! Index of latest terminal line (first to be printed)
static uint8_t terminal_line_offset;

//! Queue for incoming terminal characters
static xQueueHandle terminal_in_queue;

//! Semaphore to signal busy display
static xSemaphoreHandle display_mutex;

//! Semaphore to signal busy terminal buffer
static xSemaphoreHandle terminal_mutex;

//! Handle for terminal output task
static xTaskHandle terminal_task_handle;

//! Handle for about screen task
static xTaskHandle about_task_handle;

//@}


//! \name Tasks for demo
//@{

static void main_task(void *params);
static void graph_task(void *params);
static void terminal_task(void *params);
static void about_task(void *params);
static void uart_task(void *params);

//@}

//! Interrupt handler for reception from EDBG Virtual COM Port
static void cdc_rx_handler(uint8_t instance);


/**
 * \brief Initialize tasks and resources for demo
 *
 * This function initializes the \ref oled1_xpro_io_group instance and the
 * \ref edbg_cdc_rx_group instance for reception, then creates all
 * the objects for FreeRTOS to run the demo.
 */
void demotasks_init(void)
{
	// Initialize hardware for the OLED1 Xplained Pro driver instance
	oled1_init(&oled1);

	// Configure SERCOM USART for reception from EDBG Virtual COM Port
	cdc_rx_init(&cdc_usart, &cdc_rx_handler);

	display_mutex  = xSemaphoreCreateMutex();
	terminal_mutex = xSemaphoreCreateMutex();
	terminal_in_queue = xQueueCreate(64, sizeof(uint8_t));

	xTaskCreate(about_task,
			(const char *)"About",
			configMINIMAL_STACK_SIZE,
			NULL,
			ABOUT_TASK_PRIORITY,
			&about_task_handle);

	xTaskCreate(graph_task,
			(const char *)"Graph",
			configMINIMAL_STACK_SIZE,
			NULL,
			GRAPH_TASK_PRIORITY,
			NULL);

	xTaskCreate(main_task,
			(const char *) "Main",
			configMINIMAL_STACK_SIZE,
			NULL,
			MAIN_TASK_PRIORITY,
			NULL);

	xTaskCreate(terminal_task,
			(const char *)"Term.",
			configMINIMAL_STACK_SIZE,
			NULL,
			TERMINAL_TASK_PRIORITY,
			&terminal_task_handle);

	xTaskCreate(uart_task,
			(const char *) "UART",
			configMINIMAL_STACK_SIZE,
			NULL,
			UART_TASK_PRIORITY,
			NULL);

	// Suspend these since the main task will control their execution
	vTaskSuspend(about_task_handle);
	vTaskSuspend(terminal_task_handle);
}


/**
 * \brief Main demo task
 *
 * This task keeps track of which screen the user has selected, which tasks
 * to resume/suspend to draw the selected screen, and also draws the menu bar.
 *
 * The menu bar shows which screens the user can select by clicking the
 * corresponding buttons on the OLED1 Xplained Pro:
 * - \ref graph_task() "graph" (selected at start-up)
 * - \ref terminal_task() "term."
 * - \ref about_task() "about"
 *
 * \param params Parameters for the task. (Not used.)
 */
static void main_task(void *params)
{
	bool graph_buffer_initialized = false;
	bool selection_changed = true;
	bool select_graph_buffer;
	enum menu_items current_selection = MENU_ITEM_GRAPH;
	gfx_coord_t x, y, display_y_offset;
	xTaskHandle temp_task_handle = NULL;

	for(;;) {
		// Show that task is executing
		oled1_set_led_state(&oled1, OLED1_LED3_ID, true);

		// Check buttons to see if user changed the selection
		if (oled1_get_button_state(&oled1, OLED1_BUTTON1_ID)
					&& (current_selection != MENU_ITEM_GRAPH)) {
			current_selection = MENU_ITEM_GRAPH;
			selection_changed = true;
		} else if (oled1_get_button_state(&oled1, OLED1_BUTTON2_ID)
					&& (current_selection != MENU_ITEM_TERMINAL)) {
			current_selection = MENU_ITEM_TERMINAL;
			selection_changed = true;
		} else if (oled1_get_button_state(&oled1, OLED1_BUTTON3_ID)
					&& (current_selection != MENU_ITEM_ABOUT)) {
			current_selection = MENU_ITEM_ABOUT;
			selection_changed = true;
		}

		// If selection changed, handle the selection
		if (selection_changed) {
			// Wait for and take the display semaphore before doing any changes.
			xSemaphoreTake(display_mutex, portMAX_DELAY);

			// We can now safely suspend the previously resumed task
			if (temp_task_handle) {
				vTaskSuspend(temp_task_handle);
				temp_task_handle = NULL;
			}

			// Select the new drawing task and corresponding display buffer
			switch (current_selection) {
			case MENU_ITEM_GRAPH:
				// Graph task runs continuously, no need to set task handle
				select_graph_buffer = true;
				break;

			case MENU_ITEM_TERMINAL:
				temp_task_handle = terminal_task_handle;
				select_graph_buffer = false;
				break;

			default:
			case MENU_ITEM_ABOUT:
				temp_task_handle = about_task_handle;
				select_graph_buffer = false;
			}

			// Select and initialize display buffer to use.
			display_y_offset = select_graph_buffer ? CANVAS_GRAPH_Y_OFFSET : 0;

			// Draw the menu bar (only needs to be done once for graph)
			if (!select_graph_buffer || !graph_buffer_initialized) {
				// Clear the selected display buffer first
				gfx_mono_draw_filled_rect(0, display_y_offset,
						GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT / 2,
						GFX_PIXEL_CLR);

				// Draw menu lines, each item with height MENU_HEIGHT pixels
				y = display_y_offset + CANVAS_HEIGHT;
				gfx_mono_draw_horizontal_line(0, y, GFX_MONO_LCD_WIDTH,
						GFX_PIXEL_SET);

				x = MENU_ITEM_WIDTH;
				y++;

				for (uint8_t i = 0; i < (MENU_NUM_ITEMS - 1); i++) {
					gfx_mono_draw_vertical_line(x, y, MENU_HEIGHT,
							GFX_PIXEL_SET);
					x += 1 + MENU_ITEM_WIDTH;
				}

				// Highlight the current selection
				gfx_mono_draw_rect(current_selection * (1 + MENU_ITEM_WIDTH), y,
						MENU_ITEM_WIDTH, MENU_HEIGHT, GFX_PIXEL_SET);

				// Draw the menu item text
				x = (MENU_ITEM_WIDTH / 2) - ((5 * SYSFONT_WIDTH) / 2);
				y += (MENU_HEIGHT / 2) - (SYSFONT_HEIGHT / 2);

				for (uint8_t i = 0; i < MENU_NUM_ITEMS; i++) {
					gfx_mono_draw_string(menu_items_text[i], x, y, &sysfont);
					x += 1 + MENU_ITEM_WIDTH;
				}

				graph_buffer_initialized = true;
			}

			// Set display controller to output the new buffer
			ssd1306_set_display_start_line_address(display_y_offset);

			// We are done modifying the display, so give back the mutex
			xSemaphoreGive(display_mutex);

			selection_changed = false;

			// If a task handle was specified, resume it now
			if (temp_task_handle) {
				vTaskResume(temp_task_handle);
			}
		}

		// Show that task is done
		oled1_set_led_state(&oled1, OLED1_LED3_ID, false);

		vTaskDelay(MAIN_TASK_DELAY);
	}
}


/**
 * \brief Graph task
 *
 * This task runs in the background to draw a pseudo-random graph to a dedicated
 * display buffer. If the user selects a different screen than the graph, it
 * will continue to update even though it is not visible until the graph screen
 * is selected again.
 *
 * \param params Parameters for the task. (Not used.)
 */
static void graph_task(void *params)
{
	gfx_coord_t x, y, old_y;
	uint8_t current_value;

	x = 0;
	current_value = graph_noise;

	for(;;) {
		oled1_set_led_state(&oled1, OLED1_LED1_ID, true);

		// Compute new noise sample and value of current graph sample
		graph_noise = (graph_noise * 173) + 1;
		current_value = ((uint16_t)graph_noise + current_value) / 2;

		xSemaphoreTake(display_mutex, portMAX_DELAY);

		// Scale graph value so it fits within the canvas
		y = CANVAS_GRAPH_Y_OFFSET
				+ ((uint16_t)CANVAS_HEIGHT * current_value) / 256;

		// Clear previous graph point..
		gfx_mono_draw_vertical_line(x, CANVAS_GRAPH_Y_OFFSET, CANVAS_HEIGHT,
				GFX_PIXEL_CLR);

		// ..and draw a continuous graph using lines
		if (x == 0) {
			gfx_mono_draw_pixel(x, y, GFX_PIXEL_SET);
		} else {
			gfx_mono_draw_line(x - 1, old_y, x, y, GFX_PIXEL_SET);
		}

		xSemaphoreGive(display_mutex);

		old_y = y;
		if (++x >= CANVAS_WIDTH) {
			x = 0;
		}

		oled1_set_led_state(&oled1, OLED1_LED1_ID, false);

		vTaskDelay(GRAPH_TASK_DELAY);
	}
}


/**
 * \brief Terminal task
 *
 * This task prints the terminal text buffer to the display.
 *
 * \param params Parameters for the task. (Not used.)
 */
static void terminal_task(void *params)
{
	gfx_coord_t x, y;
	char current_char;
	uint8_t current_column;
	uint8_t current_line;
	uint8_t printed_lines;

	for (;;) {
		oled1_set_led_state(&oled1, OLED1_LED2_ID, true);

		// Grab both display and terminal mutexes before doing anything
		xSemaphoreTake(display_mutex, portMAX_DELAY);
		xSemaphoreTake(terminal_mutex, portMAX_DELAY);

		y = (TERMINAL_LINES - 1) * (SYSFONT_HEIGHT + 1);
		current_line = terminal_line_offset;

		for (printed_lines = 0; printed_lines < TERMINAL_LINES; printed_lines++)
				{
			x = 0;
			current_column = 0;

			// Print characters of string until zero terminator is encountered
			current_char = terminal_buffer[current_line][current_column];
			while (current_char != '\0') {
				gfx_mono_draw_char(current_char, x, y, &sysfont);

				// Move to next character on display and in buffer
				x += SYSFONT_WIDTH;
				current_column++;
				current_char = terminal_buffer[current_line][current_column];
			}

			// Erase remaining part of line on display
			if (current_column < TERMINAL_COLUMNS) {
				gfx_mono_draw_filled_rect(x, y,
						CANVAS_WIDTH - (current_column * SYSFONT_WIDTH),
						SYSFONT_HEIGHT, GFX_PIXEL_CLR);
			}

			// Move to previous line on display and in buffer
			y -= 1 + SYSFONT_HEIGHT;
			current_line += TERMINAL_BUFFER_LINES - 1;
			current_line %= TERMINAL_BUFFER_LINES;
		}

		xSemaphoreGive(terminal_mutex);
		xSemaphoreGive(display_mutex);

		oled1_set_led_state(&oled1, OLED1_LED2_ID, false);

		vTaskDelay(TERMINAL_TASK_DELAY);
	}
}


/**
 * \brief About task
 *
 * This task prints a short text about the demo, with a simple zooming
 * animation.
 *
 * \param params Parameters for the task. (Not used.)
 */
static void about_task(void *params)
{
	char c;
	gfx_coord_t x, y;
	uint8_t i, shift;
	char * text_to_use = (char *)&about_text;

	const uint8_t max_shift = 8;
	shift = 1;

	for (;;) {
		oled1_set_led_state(&oled1, OLED1_LED2_ID, true);

		xSemaphoreTake(display_mutex, portMAX_DELAY);

		// Print the about text in an expanding area
		for (i = 0; i < (sizeof(about_text) - 1); i++) {
			c = text_to_use[i];
			x = (((i % TERMINAL_COLUMNS) * SYSFONT_WIDTH) * shift
					+ (CANVAS_WIDTH / 2) * (max_shift - shift))
					/ max_shift;
			y = (((i / TERMINAL_COLUMNS) * SYSFONT_HEIGHT) * shift
					+ (CANVAS_HEIGHT / 2) * (max_shift - shift))
					/ max_shift;
			gfx_mono_draw_char(c, x, y, &sysfont);
		}

		xSemaphoreGive(display_mutex);

		oled1_set_led_state(&oled1, OLED1_LED2_ID, false);

		// Repeat task until we're displaying the text in full size
		if (shift < max_shift) {
			shift++;
			vTaskDelay(ABOUT_TASK_DELAY);
		} else {
			shift = 0;
			vTaskSuspend(NULL);
			if (tickless_enable) {
				text_to_use = (char *)&about_text;
			} else {
				text_to_use = (char *)&about_text_tickless;
			}
			tickless_enable = !tickless_enable;
		}
	}
}


/**
 * \brief UART task
 *
 * This task runs in the background to handle the queued, incoming terminal
 * characters and write them to the terminal text buffer. It does not print
 * anything to the display -- that is done by \ref terminal_task().
 *
 * \param params Parameters for the task. (Not used.)
 */
static void uart_task(void *params)
{
	uint8_t *current_line_ptr;
	uint8_t *current_char_ptr;
	uint8_t current_column = 0;

	for (;;) {
		// Show that task is executing
		oled1_set_led_state(&oled1, OLED1_LED1_ID, true);

		// Grab terminal mutex
		xSemaphoreTake(terminal_mutex, portMAX_DELAY);

		current_line_ptr = terminal_buffer[terminal_line_offset];
		current_char_ptr = current_line_ptr + current_column;

		// Any characters queued? Handle them!
		while (xQueueReceive(terminal_in_queue, current_char_ptr, 0)) {
			/* Newline-handling is difficult because all terminal emulators
			 * seem to do it their own way. The method below seems to work
			 * with Putty and Realterm out of the box.
			 */
			switch (*current_char_ptr) {
			case '\r':
				// Replace \r with \0 and move head to next line
				*current_char_ptr = '\0';

				current_column = 0;
				terminal_line_offset = (terminal_line_offset + 1)
						% TERMINAL_BUFFER_LINES;
				current_line_ptr = terminal_buffer[terminal_line_offset];
				current_char_ptr = current_line_ptr + current_column;
				break;

			case '\n':
				// For \n, do nothing -- it is replaced with \0 later
				break;

			default:
				// For all other characters, just move head to next char
				current_column++;
				if (current_column >= TERMINAL_COLUMNS) {
					current_column = 0;
					terminal_line_offset = (terminal_line_offset + 1)
							% TERMINAL_BUFFER_LINES;
					current_line_ptr = terminal_buffer[terminal_line_offset];
				}
				current_char_ptr = current_line_ptr + current_column;
			}

			// Set zero-terminator at head
			*current_char_ptr = '\0';
		}

		xSemaphoreGive(terminal_mutex);

		oled1_set_led_state(&oled1, OLED1_LED1_ID, false);

		vTaskDelay(UART_TASK_DELAY);
	}
}


/**
 * \internal
 * \brief UART interrupt handler for reception on EDBG CDC UART
 *
 * This function is based on the interrupt handler of the SERCOM USART callback
 * driver (\ref _usart_interrupt_handler()). It has been modified to only handle
 * the receive interrupt and to push the received data directly into the queue
 * for terminal characters (\ref terminal_in_queue), and echo the character back
 * to the sender.
 *
 * \param instance Instance number of SERCOM that generated interrupt.
 */
static void cdc_rx_handler(uint8_t instance)
{
	SercomUsart *const usart_hw = (SercomUsart *)EDBG_CDC_MODULE;
	uint16_t interrupt_status;
	uint16_t data;
	uint8_t error_code;

	// Wait for synch to complete
#if defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1)
	while (usart_hw->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY) {
	}
#elif defined(FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2)
	while (usart_hw->SYNCBUSY.reg) {
	}
#endif

	// Read and mask interrupt flag register
	interrupt_status = usart_hw->INTFLAG.reg;

	if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {
		// Check for errors
		error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
		if (error_code) {
			// Only frame error and buffer overflow should be possible
			if (error_code &
					(SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_BUFOVF)) {
				usart_hw->STATUS.reg =
						SERCOM_USART_STATUS_FERR | SERCOM_USART_STATUS_BUFOVF;
			} else {
				// Error: unknown failure
			}
		// All is fine, so push the received character into our queue
		} else {
			data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

			if (!xQueueSendFromISR(terminal_in_queue, (uint8_t *)&data,
						NULL)) {
				// Error: could not enqueue character
			} else {
				// Echo back! Data reg. should empty fast since this is the
				// only place anything is sent.
				while (!(interrupt_status & SERCOM_USART_INTFLAG_DRE)) {
					interrupt_status = usart_hw->INTFLAG.reg;
				}
				usart_hw->DATA.reg = (uint8_t)data;
			}
		}
	} else {
		// Error: only RX interrupt should be enabled
	}
}

/** @} */
