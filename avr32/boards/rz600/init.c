/**
 * \file
 *
 * \brief RZ600 board init.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "rz600.h"
#include "conf_board.h"
#include "gpio.h"
#include "board.h"

#if defined (CONF_BOARD_AT45DBX)
# define AT45DBX_MEM_CNT  1
#endif

void board_init(void)
{
	gpio_configure_pin(LED0_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED1_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

#if defined (CONF_BOARD_AT86RFX)
    // AT86RFX SPI GPIO options.
    static  gpio_map_t AT86RFX_SPI_GPIO_MAP =
    {
        {AT86RFX_SPI_SCK_PIN,  AT86RFX_SPI_SCK_FUNCTION },
        {AT86RFX_SPI_MISO_PIN, AT86RFX_SPI_MISO_FUNCTION},
        {AT86RFX_SPI_MOSI_PIN, AT86RFX_SPI_MOSI_FUNCTION},
        {AT86RFX_SPI_NPCS_PIN, AT86RFX_SPI_NPCS_FUNCTION}
    };

	// Assign GPIO to SPI.
	gpio_enable_module(AT86RFX_SPI_GPIO_MAP, sizeof(AT86RFX_SPI_GPIO_MAP) / sizeof(AT86RFX_SPI_GPIO_MAP[0]));

	gpio_enable_pin_interrupt(AT86RFX_IRQ_PIN, GPIO_RISING_EDGE);
	gpio_clear_pin_interrupt_flag(AT86RFX_IRQ_PIN);

	gpio_configure_pin(AT86RFX_RST_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(AT86RFX_SLP_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
#endif

}
