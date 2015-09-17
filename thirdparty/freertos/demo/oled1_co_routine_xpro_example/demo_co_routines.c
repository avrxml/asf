/**
 * \file
 *
 * \brief FreeRTOS demo Co-routines implementations.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
#include "demo_co_routines.h"

/**
 * \addtogroup freertos_sam0_demo_co-routines_group
 *
 * @{
 */

//! \name Co-routines configuration
//@{

#define UART_CO_ROUTINE_PRIORITY      (configMAX_CO_ROUTINE_PRIORITIES - 1)

#define MAIN_CO_ROUTINE_PRIORITY      (configMAX_CO_ROUTINE_PRIORITIES - 2)
#define MAIN_CO_ROUTINE_DELAY         (100 / portTICK_RATE_MS)

#define GRAPH_CO_ROUTINE_PRIORITY     (configMAX_CO_ROUTINE_PRIORITIES - 3)
#define GRAPH_CO_ROUTINE_DELAY        (50 / portTICK_RATE_MS)

#define TERMINAL_CO_ROUTINE_PRIORITY  (configMAX_CO_ROUTINE_PRIORITIES - 3)

#define ABOUT_CO_ROUTINE_PRIORITY     (configMAX_CO_ROUTINE_PRIORITIES - 3)

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
	((GFX_MONO_LCD_HEIGHT) - (MENU_HEIGHT + 1))

//! Width of area in which to draw content
#define CANVAS_WIDTH           (GFX_MONO_LCD_WIDTH)

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

//! Text to display on about screen
static const char about_text[] =
	"FreeRTOS "tskKERNEL_VERSION_NUMBER" demo."
	"                     "
	"Use CDC at 9.6 kBaud.";

/**
 * \brief Instance for \ref oled1_xpro_io_group
 *
 * The extension header to use is configured with \ref OLED1_EXT_HEADER.
 */
static OLED1_CREATE_INSTANCE(oled1, OLED1_EXT_HEADER);

//! Instance for \ref edbg_cdc_rx_group
static struct usart_module cdc_usart;

//! Pseudo-random noise for graph Co-routine
static uint8_t graph_noise = 128;

//! x coord for graph Co-routine
static gfx_coord_t x1 = 0;

//! Buffer for terminal text
static uint8_t terminal_buffer[TERMINAL_BUFFER_LINES][TERMINAL_BUFFER_COLUMNS];

//! Index of latest terminal line (first to be printed)
static uint8_t terminal_line_offset;

//! Queue for incoming terminal characters
static QueueHandle_t terminal_in_queue;

//! Queue for sync graph Co-routine
static QueueHandle_t graph_co_routine_queue;

//! Queue for sync terminal Co-routine
static QueueHandle_t terminal_co_routine_queue;

//! Queue for sync about Co-routine
static QueueHandle_t about_co_routine_queue;

//@}


//! \name Co-routines for demo
//@{

static void main_co_routine(CoRoutineHandle_t, UBaseType_t);
static void graph_co_routine(CoRoutineHandle_t, UBaseType_t);
static void terminal_co_routine(CoRoutineHandle_t, UBaseType_t);
static void about_co_routine(CoRoutineHandle_t, UBaseType_t);
static void uart_co_routine(CoRoutineHandle_t, UBaseType_t);

//@}

//! Interrupt handler for reception from EDBG Virtual COM Port
static void cdc_rx_handler(uint8_t instance);

//! This idle task hook will schedule a co-routine each time it is called.
void vApplicationIdleHook(void)
{
	for( ;; ) {
		vCoRoutineSchedule();
	}
}


/**
 * \brief Initialize Co-routines and resources for demo
 *
 * This function initializes the \ref oled1_xpro_io_group instance and the
 * \ref edbg_cdc_rx_group instance for reception, then creates all
 * the objects for FreeRTOS to run the demo.
 */
void demo_co_routines_init(void)
{
	// Initialize hardware for the OLED1 Xplained Pro driver instance
	oled1_init(&oled1);

	// Configure SERCOM USART for reception from EDBG Virtual COM Port
	cdc_rx_init(&cdc_usart, &cdc_rx_handler);

	terminal_in_queue = xQueueCreate(4, sizeof(uint8_t));
	graph_co_routine_queue = xQueueCreate(4, sizeof(uint8_t));
	terminal_co_routine_queue = xQueueCreate(4, sizeof(uint8_t));
	about_co_routine_queue = xQueueCreate(4, sizeof(uint8_t));

	xCoRoutineCreate(uart_co_routine,
			UART_CO_ROUTINE_PRIORITY,
			0);

	xCoRoutineCreate(graph_co_routine,
			GRAPH_CO_ROUTINE_PRIORITY,
			0);

	xCoRoutineCreate(terminal_co_routine,
			TERMINAL_CO_ROUTINE_PRIORITY,
			0);

	xCoRoutineCreate(about_co_routine,
			ABOUT_CO_ROUTINE_PRIORITY,
			0);

	xCoRoutineCreate(main_co_routine,
		MAIN_CO_ROUTINE_PRIORITY,
		0);
}


/**
 * \brief Main demo Co-routine
 *
 * This Co-routine keeps track of which screen the user has selected, which
 * resume/suspend to draw the selected screen, and also draws the menu bar.
 *
 * The menu bar shows which screens the user can select by clicking the
 * corresponding buttons on the OLED1 Xplained Pro:
 * - \ref graph_co_routine() "graph" (selected at start-up)
 * - \ref terminal_co_routine() "term."
 * - \ref about_co_routine() "about"
 *
 * \param xHandle Type by which co-routines are referenced.
 * \param uxIndex Parameter for the Co-routine.
 */
static void main_co_routine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex)
{
	static bool selection_changed = true;
	static enum menu_items current_selection = MENU_ITEM_GRAPH;
	static gfx_coord_t x, y;
	static portBASE_TYPE result;

	crSTART(xHandle);

	for(;;) {
		// Show that Co-routine is executing
		oled1_set_led_state(&oled1, OLED1_LED3_ID, true);

		// Check buttons to see if user changed the selection
		if (oled1_get_button_state(&oled1, OLED1_BUTTON1_ID)
					&& (current_selection != MENU_ITEM_GRAPH)) {
			current_selection = MENU_ITEM_GRAPH;
			x1 = 0;
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

		// Select the new drawing Co-routine and corresponding display buffer
		if (current_selection == MENU_ITEM_GRAPH) {
			crQUEUE_SEND(xHandle, graph_co_routine_queue, (void *)&uxIndex,
					0, &result);
		} else if (current_selection == MENU_ITEM_TERMINAL) {
			crQUEUE_SEND(xHandle, terminal_co_routine_queue, (void *)&uxIndex,
					0, &result);
		}

		if(result != pdPASS) {
			/* For the reasons stated at the top of the file we should always
			find that we can post to the queue.  If we could not then an error
			has occurred. */
			Assert(false);
		}

		// If selection changed, handle the selection, Draw the menu bar 
		if (selection_changed) {
			// Clear the selected display buffer first
			gfx_mono_draw_filled_rect(0, 0,
					GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT,
					GFX_PIXEL_CLR);

			// Draw menu lines, each item with height MENU_HEIGHT pixels
			y = CANVAS_HEIGHT;
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

			crDELAY(xHandle, MAIN_CO_ROUTINE_DELAY);

			if (current_selection == MENU_ITEM_ABOUT) {
				crQUEUE_SEND(xHandle, about_co_routine_queue, (void *)&uxIndex,
						0, &result);
			}

			if(result != pdPASS) {
				/* For the reasons stated at the top of the file we should always
				find that we can post to the queue.  If we could not then an error
				has occurred. */
				Assert(false);
			}

			selection_changed = false;
		}

		// Show that Co-routine is done
		oled1_set_led_state(&oled1, OLED1_LED3_ID, false);

		crDELAY(xHandle, MAIN_CO_ROUTINE_DELAY);
	}

	crEND();
}


/**
 * \brief Graph Co-routine
 *
 * This Co-routine runs in the background to draw a pseudo-random graph to a dedicated
 * display buffer. If the user selects a different screen than the graph, it
 * will continue to update even though it is not visible until the graph screen
 * is selected again.
 *
 * \param xHandle Type by which co-routines are referenced.
 * \param uxIndex Parameter for the Co-routine.
 */
static void graph_co_routine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex)
{
	static gfx_coord_t y1, old_y;
	static uint8_t current_value = 128;
	portBASE_TYPE queue_data;
	static BaseType_t result1;

	crSTART(xHandle);

	for(;;) {
		crQUEUE_RECEIVE(xHandle, graph_co_routine_queue, &queue_data,
								portMAX_DELAY, &result1);

		if (result1 == pdPASS) {
			oled1_set_led_state(&oled1, OLED1_LED1_ID, true);

			// Compute new noise sample and value of current graph sample
			graph_noise = (graph_noise * 173) + 1;
			current_value = (graph_noise + current_value) / 2;

			// Scale graph value so it fits within the canvas
			y1 = ((uint16_t)(CANVAS_HEIGHT * current_value)) / 256;

			// Clear previous graph point..
			gfx_mono_draw_vertical_line(x1, 0, CANVAS_HEIGHT,
					GFX_PIXEL_CLR);

			// ..and draw a continuous graph using lines
			if (x1 == 0) {
				gfx_mono_draw_pixel(x1, y1, GFX_PIXEL_SET);
			} else {
				gfx_mono_draw_line(x1 - 1, old_y, x1, y1, GFX_PIXEL_SET);
			}

			old_y = y1;
			if (++x1 >= CANVAS_WIDTH) {
				x1 = 0;
			}

			oled1_set_led_state(&oled1, OLED1_LED1_ID, false);

			crDELAY(xHandle, GRAPH_CO_ROUTINE_DELAY);
		}
	}

	crEND();
}


/**
 * \brief Terminal Co-routine
 *
 * This Co-routine prints the terminal text buffer to the display.
 *
 * \param xHandle Type by which co-routines are referenced.
 * \param uxIndex Parameter for the Co-routine.
 */
static void terminal_co_routine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex)
{
	static gfx_coord_t x2, y2;
	static char current_char;
	static uint8_t current_column1;
	static uint8_t current_line;
	static uint8_t printed_lines;
	portBASE_TYPE queue_data;
	static BaseType_t result2;

	crSTART(xHandle);

	for (;;) {
		crQUEUE_RECEIVE(xHandle, terminal_co_routine_queue, &queue_data,
						portMAX_DELAY, &result2);

		if (result2 == pdPASS) {
			oled1_set_led_state(&oled1, OLED1_LED2_ID, true);

			y2 = (TERMINAL_LINES - 1) * (SYSFONT_HEIGHT + 1);
			current_line = terminal_line_offset;

			for (printed_lines = 0; printed_lines < TERMINAL_LINES; printed_lines++)
					{
				x2 = 0;
				current_column1 = 0;

				// Print characters of string until zero terminator is encountered
				current_char = terminal_buffer[current_line][current_column1];
				while (current_char != '\0') {
					gfx_mono_draw_char(current_char, x2, y2, &sysfont);

					// Move to next character on display and in buffer
					x2 += SYSFONT_WIDTH;
					current_column1++;
					current_char = terminal_buffer[current_line][current_column1];
				}

				// Erase remaining part of line on display
				if (current_column1 < TERMINAL_COLUMNS) {
					gfx_mono_draw_filled_rect(x2, y2,
							CANVAS_WIDTH - (current_column1 * SYSFONT_WIDTH),
							SYSFONT_HEIGHT, GFX_PIXEL_CLR);
				}

				// Move to previous line on display and in buffer
				y2 -= 1 + SYSFONT_HEIGHT;
				current_line += TERMINAL_BUFFER_LINES - 1;
				current_line %= TERMINAL_BUFFER_LINES;
			}

			oled1_set_led_state(&oled1, OLED1_LED2_ID, false);
		}
	}

	crEND();
}


/**
 * \brief About Co-routine
 *
 * This Co-routine prints a short text about the demo, with a simple zooming
 * animation.
 *
 * \param xHandle Type by which co-routines are referenced.
 * \param uxIndex Parameter for the Co-routine.
 */
static void about_co_routine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex)
{
	static char c;
	static gfx_coord_t x3, y3;
	static uint8_t i, shift = 1;
	portBASE_TYPE queue_data;
	static BaseType_t result3;

	const uint8_t max_shift = 8;

	crSTART(xHandle);

	for (;;) {
		crQUEUE_RECEIVE(xHandle, about_co_routine_queue, &queue_data,
				portMAX_DELAY, &result3);

		if (result3 == pdPASS) {
			oled1_set_led_state(&oled1, OLED1_LED2_ID, true);

			// Print the about text in an expanding area
			while (shift <= max_shift) {
				for (i = 0; i < (sizeof(about_text) - 1); i++) {
					c = about_text[i];
					x3 = (((i % TERMINAL_COLUMNS) * SYSFONT_WIDTH) * shift
							+ (CANVAS_WIDTH / 2) * (max_shift - shift))
							/ max_shift;
					y3 = (((i / TERMINAL_COLUMNS) * SYSFONT_HEIGHT) * shift
							+ (CANVAS_HEIGHT / 2) * (max_shift - shift))
							/ max_shift;
					gfx_mono_draw_char(c, x3, y3, &sysfont);
				}
				shift++;
			}
			shift = 0;

			oled1_set_led_state(&oled1, OLED1_LED2_ID, false);
		}
	}

	crEND();
}


/**
 * \brief UART Co-routine
 *
 * This Co-routine runs in the background to handle the queued, incoming terminal
 * characters and write them to the terminal text buffer. It does not print
 * anything to the display -- that is done by \ref terminal_co_routine().
 *
 * \param xHandle Type by which co-routines are referenced.
 * \param uxIndex Parameter for the Co-routine.
 */
static void uart_co_routine(CoRoutineHandle_t xHandle, UBaseType_t uxIndex)
{
	static uint8_t *current_line_ptr;
	static uint8_t *current_char_ptr;
	static uint8_t current_column = 0;
	static BaseType_t result4;

	crSTART(xHandle);

	for (;;) {
		current_line_ptr = terminal_buffer[terminal_line_offset];
		current_char_ptr = current_line_ptr + current_column;

		crQUEUE_RECEIVE(xHandle,
				terminal_in_queue,
				current_char_ptr,
				portMAX_DELAY,
				&result4);

		// Any characters queued? Handle them!
		if (result4 == pdPASS) {
			// Show that Co-routine is executing
			oled1_set_led_state(&oled1, OLED1_LED1_ID, true);

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

		oled1_set_led_state(&oled1, OLED1_LED1_ID, false);
	}

	crEND();
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
	BaseType_t x_cr_woken_by_post = pdFALSE;

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

			x_cr_woken_by_post = crQUEUE_SEND_FROM_ISR(terminal_in_queue,
					(uint8_t *)&data, x_cr_woken_by_post);

			// Echo back! Data reg. should empty fast since this is the
			// only place anything is sent.
			while (!(interrupt_status & SERCOM_USART_INTFLAG_DRE)) {
				interrupt_status = usart_hw->INTFLAG.reg;
			}
			usart_hw->DATA.reg = (uint8_t)data;
		}
	} else {
		// Error: only RX interrupt should be enabled
	}
}

/** @} */
