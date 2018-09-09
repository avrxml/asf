/**
* \file
*
* \brief The functions of EPD GPIO
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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


