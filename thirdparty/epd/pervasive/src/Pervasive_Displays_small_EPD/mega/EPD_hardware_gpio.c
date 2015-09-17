/**
* \file
*
* \brief The functions of EPD GPIO
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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
*/

#include "EPD_hardware_gpio.h"
#include "conf_EPD.h"

/**
* \brief Set EPD_CS pin to high
*/
void EPD_cs_high (void) {
	set_gpio_high(EPD_SPI_CS_PIN);
}

/**
* \brief Set EPD_CS pin to low
*/
void EPD_cs_low (void) {
	set_gpio_low(EPD_SPI_CS_PIN);
}

/**
* \brief Set Flash_CS pin to high
*/
void EPD_flash_cs_high(void) {
	/** not used at present 
	set_gpio_high(Flash_CS_PIN);
	*/
}

/**
* \brief Set Flash_CS pin to low
*/
void EPD_flash_cs_low (void) {
	/** not used at present 
	set_gpio_low(Flash_CS_PIN);
	*/
}

/**
* \brief Set /RESET pin to high
*/
void EPD_rst_high (void) {
	set_gpio_high(EPD_RST_PIN);
}

/**
* \brief Set /RESET pin to low
*/
void EPD_rst_low (void) {
	set_gpio_low(EPD_RST_PIN);
}

/**
* \brief Set DISCHARGE pin to high
*/
void EPD_discharge_high (void) {
	set_gpio_high(EPD_DISCHARGE_PIN);
}

/**
* \brief Set DISCHARGE pin to low
*/
void EPD_discharge_low (void) {
	set_gpio_low(EPD_DISCHARGE_PIN);
}

/**
* \brief Set Vcc (PANEL_ON) to high
*/
void EPD_Vcc_turn_on (void) {
	set_gpio_high(EPD_PANELON_PIN);
}

/**
* \brief Set Vcc (PANEL_ON) to low
*/
void EPD_Vcc_turn_off (void) {
	set_gpio_low(EPD_PANELON_PIN);
}

/**
* \brief Set BORDER_CONTROL pin to high
*/
void EPD_border_high(void) {
	set_gpio_high(EPD_BORDER_PIN);
}

/**
* \brief Set BORDER_CONTROL pin to low
*/
void EPD_border_low (void) {
	set_gpio_low(EPD_BORDER_PIN);
}

/**
* \brief Set PWM pin to high
*/
void EPD_pwm_high(void) {
	set_gpio_high(EPD_PWM_PIN);
}

/**
* \brief Set PWM pin to low
*/
void EPD_pwm_low (void) {
	set_gpio_low(EPD_PWM_PIN);
}

/**
* \brief Set MISO pin of SPI to low
*/
void SPIMISO_low(void) {
	config_gpio_dir_o(EPD_SPI_MISO_PIN);
	set_gpio_low(EPD_SPI_MISO_PIN);
}

/**
* \brief Set MOSI pin of SPI to low
*/
void SPIMOSI_low(void) {
	set_gpio_low(EPD_SPI_MOSI_PIN);
}

/**
* \brief Set Clock of SPI to low
*/
void SPICLK_low(void) {
	set_gpio_low(EPD_SPI_CLK_PIN);
}

/**
* \brief Get BUSY pin status
*/
bool EPD_IsBusy(void) {
	return (bool)input_get(EPD_BUSY_PIN);
}

/**
* \brief Configure GPIO
*/
void EPD_initialize_gpio(void) {
		
	config_gpio_dir_i( EPD_BUSY_PIN);			
	config_gpio_dir_o( EPD_SPI_CS_PIN);				
	config_gpio_dir_o( EPD_RST_PIN);			
	config_gpio_dir_o( EPD_PANELON_PIN);
	config_gpio_dir_o( EPD_DISCHARGE_PIN);
	config_gpio_dir_o( EPD_BORDER_PIN);
	EPD_flash_cs_high();					// dumy
	EPD_border_low();
	
	/** not used at present 
	config_gpio_dir_o( Flash_CS_PIN);
	config_gpio_dir_i( Temper_PIN);	
	*/
}


