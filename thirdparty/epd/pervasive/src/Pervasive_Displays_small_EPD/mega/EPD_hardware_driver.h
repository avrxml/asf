/**
* \file
*
* \brief The SPI, PWM, Temperature definitions of COG hardware driver
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

#ifndef 	DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_
#define 	DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_

#include "Pervasive_Displays_small_EPD.h"

void epd_spi_init (void);
void epd_spi_attach (void);
void epd_spi_detach (void);
void epd_spi_send (unsigned char Register, unsigned char *Data, unsigned Length);
void epd_spi_send_byte (uint8_t Register, uint8_t Data);
uint8_t epd_spi_read(unsigned char RDATA);
void epd_spi_write (unsigned char Data);
void sys_delay_ms(unsigned int ms);
void start_EPD_timer(void);
void stop_EPD_timer(void);
uint32_t get_current_time_tick(void);
void SysTick_Handler(void);
void PWM_start_toggle(void);
void PWM_stop_toggle(void);
void PWM_run(uint16_t time);
void initialize_temperature(void);
int16_t get_temperature(void);
void EPD_display_hardware_init (void);
#if (defined COG_V110_G2) || (defined COG_V230_G2)
uint8_t SPI_R(uint8_t Register, uint8_t Data);
#endif

#endif 	//DISPLAY_HARDWARE_DRIVCE_H_INCLUDED_
