/**
 * \file
 *
 * \brief RZ600 board init.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
