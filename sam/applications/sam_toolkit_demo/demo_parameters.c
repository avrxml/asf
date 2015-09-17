/**
 * \file
 *
 * \brief SAM toolkit demo parameter initialization and storage.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>
#include <stdio.h>
#include "demo_parameters.h"
#include "flash_efc.h"
#include "conf_demo.h"
#include "FreeRTOS.h"
#include "task.h"

/** Parameter offset */
const int __offset_parameters__
	= (IFLASH0_ADDR + IFLASH0_SIZE - IFLASH0_PAGE_SIZE*8);

/** Parameters used in the demo */
demo_param_t g_demo_parameters = {
	.magic = DEMO_PARAMETERS_MAGIC,
	.ppt_delay = 5000,
	.hour = 12,
	.minute = 0,
	.second = 0,
	.day = 20,
	.month = 12,
	.year = 2012,
	.backlight = 10,
	.calib_points[0].panel.x = LCD_WIDTH / 10,
	.calib_points[0].panel.y = LCD_HEIGHT / 10,
	.calib_points[0].raw.x = 0,
	.calib_points[0].raw.y = 0,
	.calib_points[1].panel.x = LCD_WIDTH - LCD_WIDTH / 10,
	.calib_points[1].panel.y = LCD_HEIGHT / 10,
	.calib_points[1].raw.x = 0,
	.calib_points[1].raw.y = 0,
	.calib_points[2].panel.x = LCD_WIDTH - LCD_WIDTH / 10,
	.calib_points[2].panel.y = LCD_HEIGHT - LCD_HEIGHT / 10,
	.calib_points[2].raw.x = 0,
	.calib_points[2].raw.y = 0,
	.calib_points[3].panel.x = LCD_WIDTH / 10,
	.calib_points[3].panel.y = LCD_HEIGHT - LCD_HEIGHT / 10,
	.calib_points[3].raw.x = 0,
	.calib_points[3].raw.y = 0,
	.calib_points[4].panel.x = LCD_WIDTH / 2,
	.calib_points[4].panel.y = LCD_HEIGHT / 2,
	.calib_points[4].raw.x = 0,
	.calib_points[4].raw.y = 0,
};

/*
 * \brief Initialize demo parameters.
 */
uint32_t demo_parameters_initialize( void )
{
	volatile demo_param_t *param = (demo_param_t *)__offset_parameters__;

	if (param->magic == DEMO_PARAMETERS_MAGIC) {
		memcpy( &g_demo_parameters, (void const *)param,
				sizeof(demo_param_t));
	} else {
		return demo_parameters_commit_changes();
	}

	return 0;
}

/*
 * \brief Commit demo parameters.
 */
uint32_t demo_parameters_commit_changes( void )
{
	uint32_t ret;
	taskENTER_CRITICAL();
	flash_init(FLASH_ACCESS_MODE_128, 6);
	flash_unlock(IFLASH0_ADDR, __offset_parameters__ + IFLASH0_PAGE_SIZE - 1,
			0, 0);
#if SAM4S
	/* Erase flag page */
	flash_erase_page(__offset_parameters__, IFLASH_ERASE_PAGES_8);
	ret = flash_write((uint32_t)(__offset_parameters__),
			(void *)&g_demo_parameters, sizeof(demo_param_t), 0);
#else
	ret = flash_write((uint32_t)(__offset_parameters__),
			(void *)&g_demo_parameters, sizeof(demo_param_t), 1);
#endif

	taskEXIT_CRITICAL();

	return ret;
}
