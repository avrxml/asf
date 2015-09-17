/**
 * \file
 *
 * \brief GFX task from the HTTP Netconn Example.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "task_defs.h"
#include "logo_atmel.h"

#include <FreeRTOS.h>
#include <FreeRTOS_CLI.h>
#include <StackMacros.h>
#include <croutine.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>

#include <pmc.h>
#include <sysclk.h>

#include "ili93xx.h"
#include "ili9325_regs.h"
#include "ili9341_regs.h"
#include "aat31xx.h"
#include "smc.h"
#include "ethernet.h"

static void gfx_task(void *pvParameters);
static void gfx_init(void);
static void gfx_draw_bmpfile(const uint8_t *bmpImage);

/* IP Configuration (set IP_SELECTION to either IP_FIXED for fixed IP address */
/* or IP_DHCP for DHCP IP address allocation. */
#define IP_FIXED            1
#define IP_DHCP             2
#define IP_SELECTION        IP_FIXED

/* Chip select number to be set */
#define ILI93XX_LCD_CS       1

/** IRQ priority for PIO (The lower the value, the greater the priority) */
#define IRQ_PRIOR_PIO        13

const portTickType instructions_delay = 50UL / portTICK_RATE_MS;
static const portTickType presentation_delay = 2000UL / portTICK_RATE_MS;
static const portTickType gfx_refresh_rate = 1000UL / portTICK_RATE_MS;

/* Hold the instructions screen and prevent other task from running. */
uint32_t app_hold = 1;

uint32_t g_ip_mode = 0; // 0 unselected, 1 static, 2 dhcp.

int8_t g_c_ipconfig[25];

/* BMP (Windows) Header Format. */
COMPILER_PACK_SET(1)
struct bmpfile_header {
	uint16_t type;		/* signature, must be 4D42 hex */
	uint32_t file_size; /* size of BMP file in bytes (unreliable) */
	uint16_t reserved1; /* reserved, must be zero */
	uint16_t reserved2; /* reserved, must be zero */
	uint32_t offset;	/* offset to start of image data in bytes */
	uint32_t header_size; /* size of BITMAPINFOHEADER structure, must be 40 */
	uint32_t width;		/* image width in pixels */
	uint32_t height;	/* image height in pixels */
	uint16_t planes;	/* number of planes in the image, must be 1 */
	uint16_t bits;		/* number of bits per pixel (1, 4, 8, 16, 24, 32) */
	uint32_t compression; /* compression type (0=none, 1=RLE-8, 2=RLE-4) */
	uint32_t inage_size; /* size of image data in bytes (including padding) */
	uint32_t h_resolution;	/* horizontal resolution in pixels per meter */
							/* (unreliable) */
	uint32_t v_resolution;	/* vertical resolution in pixels per meter */
							/* (unreliable) */
	uint32_t colours;		/* number of colors in image, or zero */
	uint32_t important_colors;	/* number of important colors, or zero */
};
COMPILER_PACK_RESET()

/**
 * \brief Create the GFX task.
 *
 * \param stack_depth_words Task stack size in 32 bits word.
 * \param task_priority Task priority.
 */
void create_gfx_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority)
{
	/* Initialize display. */
	gfx_init();

	/* Create the task as described above. */
	xTaskCreate(gfx_task, (const signed char *const) "GFX",
			stack_depth_words, NULL, task_priority,
			NULL);
}

/**
 * \brief GFX task core function.
 *
 * \param pvParameters Junk parameter.
 */
static void gfx_task(void *pvParameters)
{
	uint32_t blink = 0;

	/* Get rid of this compiler warning. */
	pvParameters = pvParameters;

	/* Enable display, backlight and print ATMEL logo. */
	ili93xx_set_cursor_position(0,0);
	ili93xx_set_foreground_color(COLOR_WHITE);
	ili93xx_draw_filled_rectangle(0, 0, ILI93XX_LCD_WIDTH, ILI93XX_LCD_HEIGHT);
	gfx_draw_bmpfile(logo_atmel_bmp);
	ili93xx_display_on();
	for (uint32_t i = AAT31XX_MIN_BACKLIGHT_LEVEL; i <= AAT31XX_MAX_BACKLIGHT_LEVEL; ++i) {
		aat31xx_set_backlight(i);
		vTaskDelay(40);
	}
	vTaskDelay(presentation_delay);

	/* Draw IP config menu. */
	ili93xx_set_foreground_color(COLOR_WHITE);
	ili93xx_draw_filled_rectangle(0, 0, ILI93XX_LCD_WIDTH, ILI93XX_LCD_HEIGHT);

#if (IP_SELECTION == IP_FIXED)
	/* Static IP button selected. */
	/* Button 1. */
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_rectangle(20,70,220,130);
	ili93xx_set_foreground_color(0xE4E4E4u);
	ili93xx_draw_rectangle(22,72,222,132);
	ili93xx_set_foreground_color(COLOR_LIGHTGREY);
	ili93xx_draw_filled_rectangle(21,71,221,131);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_string(92, 91, (uint8_t *)"Static");

	/* Button 2. */
	ili93xx_set_foreground_color(0xE4E4E4u);
	ili93xx_draw_rectangle(20,170,220,230);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_rectangle(22,172,222,232);
	ili93xx_set_foreground_color(COLOR_LIGHTGREY);
	ili93xx_draw_filled_rectangle(21,171,221,231);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_string(102, 191, (uint8_t *)"DHCP");
#else
	/* DHCP IP button selected. */
	/* Button 1. */
	ili93xx_set_foreground_color(0xE4E4E4u);
	ili93xx_draw_rectangle(20,70,220,130);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_rectangle(22,72,222,132);
	ili93xx_set_foreground_color(COLOR_LIGHTGREY);
	ili93xx_draw_filled_rectangle(21,71,221,131);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_string(92, 91, (uint8_t *)"Static");

	/* Button 2. */
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_rectangle(20,170,220,230);
	ili93xx_set_foreground_color(0xE4E4E4u);
	ili93xx_draw_rectangle(22,172,222,232);
	ili93xx_set_foreground_color(COLOR_LIGHTGREY);
	ili93xx_draw_filled_rectangle(21,171,221,231);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_string(102, 191, (uint8_t *)"DHCP");
#endif

	ili93xx_draw_string(22, 30, (uint8_t *)"IP Configuration");
	ili93xx_draw_string(20, 260, (uint8_t *)"Assigned IP:");
	ili93xx_draw_rectangle(20,280,220,310);

	/* Bring up the ethernet interface & initializes timer0, channel0 */
	ili93xx_draw_string(30, 290, (uint8_t *)"Please wait...");
	init_ethernet();
	app_hold = 0;

	/* Show configured IP and unlock all other waiting tasks. */
	ili93xx_set_foreground_color(COLOR_WHITE);
	ili93xx_draw_filled_rectangle(20,280,220,310);
	ili93xx_draw_filled_rectangle(0,0,240,60);
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_rectangle(20,280,220,310);
	ili93xx_draw_string(30, 290, (uint8_t const*)g_c_ipconfig);

	/* GFX task Loop. */
	while (1)
	{
		/* Make HTTP text to blink to show GFX task activity. */
		if (blink == 0)
		{
			ili93xx_draw_string(5, 30, (uint8_t *)"HTTP server running!");
			blink = 1;
		}
		else
		{
			ili93xx_set_foreground_color(COLOR_WHITE);
			ili93xx_draw_filled_rectangle(0,0,240,60);
			ili93xx_set_foreground_color(COLOR_BLACK);
			blink = 0;
		}

		/* Sleep to reach the expected blink rate. */
		/* Leave some CPU time for other tasks. */
		vTaskDelay(gfx_refresh_rate);
	}
}

/**
 * \brief Initialize the LCD.
 */
static void gfx_init(void)
{
	struct ili93xx_opt_t g_ili93xx_display_opt;

	/* Enable peripheral clock. */
	pmc_enable_periph_clk(ID_SMC);

	/* Configure SMC interface for LCD. */
	smc_set_setup_timing(SMC,ILI93XX_LCD_CS,SMC_SETUP_NWE_SETUP(2)
			| SMC_SETUP_NCS_WR_SETUP(2)
			| SMC_SETUP_NRD_SETUP(2)
			| SMC_SETUP_NCS_RD_SETUP(2));
	smc_set_pulse_timing(SMC, ILI93XX_LCD_CS , SMC_PULSE_NWE_PULSE(4)
			| SMC_PULSE_NCS_WR_PULSE(4)
			| SMC_PULSE_NRD_PULSE(10)
			| SMC_PULSE_NCS_RD_PULSE(10));
	smc_set_cycle_timing(SMC, ILI93XX_LCD_CS, SMC_CYCLE_NWE_CYCLE(10)
			| SMC_CYCLE_NRD_CYCLE(22));
	smc_set_mode(SMC, ILI93XX_LCD_CS, SMC_MODE_READ_MODE
			| SMC_MODE_WRITE_MODE);

	/* Initialize display parameter. */
	g_ili93xx_display_opt.ul_width = ILI93XX_LCD_WIDTH;
	g_ili93xx_display_opt.ul_height = ILI93XX_LCD_HEIGHT;
	g_ili93xx_display_opt.foreground_color = COLOR_WHITE;
	g_ili93xx_display_opt.background_color = COLOR_WHITE;

	/* Initialize LCD. */
	if (1 == ili93xx_init(&g_ili93xx_display_opt)) {
		while(1);
	}
}

/**
 * \brief Draw a bitmap image.
 * Expected file format is 320x240, 24 bits, RGB888.
 *
 * \param bmpImage Pointer to the bmp file image.
 */
static void gfx_draw_bmpfile(const uint8_t *bmpImage)
{
	struct bmpfile_header *bmp_header;
	uint32_t length;
	uint32_t i = 0;
	uint32_t offset;

	bmp_header = (struct bmpfile_header *) bmpImage;
	length = bmp_header->height * bmp_header->width * 3;
	offset = sizeof(struct bmpfile_header);

	if (ili93xx_device_type() == DEVICE_TYPE_ILI9325) {
		ili93xx_set_cursor_position(0, 0);

		/* Prepare to write in GRAM. */
		LCD_IR(0);
		LCD_IR(ILI9325_GRAM_DATA_REG);
		for (i = offset; i < length; i += 3) {
			/* Invert red and blue. */
			LCD_WD(bmpImage[i + 2]);
			LCD_WD(bmpImage[i + 1]);
			LCD_WD(bmpImage[i]);
		}
	}
	else if (ili93xx_device_type() == DEVICE_TYPE_ILI9341) {
		ili93xx_set_window(0, 0, bmp_header->width - 15, bmp_header->height);
		LCD_IR(ILI9341_CMD_MEMORY_WRITE); /* memory write command (R2Ch) */
		LCD_IR(ILI9341_CMD_WRITE_MEMORY_CONTINUE);

		/* The original image is mirrored. */
		for (i = bmp_header->height - 1 ; (i * bmp_header->width * 3) > offset ; i -= 1)
			for (uint16_t j = 45; j < (bmp_header->width * 3); j += 3) {
				LCD_WD(bmpImage[i * bmp_header->width * 3 + j + 2]);
				LCD_WD(bmpImage[i * bmp_header->width * 3 + j + 1]);
				LCD_WD(bmpImage[i * bmp_header->width * 3 + j]);
			}
	}
}
