/**
 * \file
 *
 * \brief Basic Pulse Width Modulation Controller (PWMA) driver.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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


#ifndef _PWMA_H_
#define _PWMA_H_

/**
 * \defgroup group_avr32_drivers_pwma PWMA - Pulse Width Modulation A
 *
 * The Pulse Width Modulation A Controller (PWMA) controls several 
 * pulse width modulation (PWM) channels.
 * This driver provides an API to get access to the main features of 
 * the PWMA controller.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"

#if (UC3L0 || UC3L0128 || UC3L0256)
#define PWMA_MAXIMUM_TOP     0xff
#define PWMA_MAXIMUM_SPREAD  0x1f
#elif (UC3D || UC3L3_L4)    
#define PWMA_MAXIMUM_TOP     0xfff
#define PWMA_MAXIMUM_SPREAD  0x1ff
#else
# error Unsupported chip 
#endif 
//Number of channel limited while using interlinked multi value mode 
#define PWMA_CHANNEL_LIMIT   4


/*! \brief Configure and enable the PWMA.
 *
 * This function will enable the PWMA module and configure a mask of channels.
 *
 * \param pwma          Pointer to the PWMA module.
 * \param channel_mask  Bit mask of channels to set period_cycles for.
 * \param period_cycles Period cycles for the PWMA module, i.e. the TOP value 
 *                      (and eventually the SPREAD value).
 * \param duty_cycles   Number of cycles for the active period for the channels
 *                      provided by channel_mask.
 * \warning This function is deprecated, please rather use
 * 			pwma_config_enable() function.
 */
void pwma_config_and_enable(volatile avr32_pwma_t *pwma,
                            unsigned long long int channel_mask,
                            int period_cycles, int duty_cycles);


/*! \brief Configure and enable the PWMA.
 *
 * This function will enable the PWMA module and configure a mask of channels.
 *
 * \param pwma         Pointer to the PWMA module.
 * \param pwma_freq_hz Required PWMA frequency in hertz.
 * \param gclk_freq_hz GCLK_PWMA frequency in hertz.
 * \param spread       Spread value to be loaded.
 *
 * \retval PASS if configuration is successfully done.
 * \retval FAIL for invalid arguments i.e. TOP or SPREAD exceed the limit value.
 */
bool pwma_config_enable(volatile avr32_pwma_t *pwma, uint32_t pwma_freq_hz, 
						uint32_t gclk_freq_hz, uint16_t spread);


/*! \brief Load the duty cycle value to the channels.
 * This function uses Interlinked Single Value Mode where several channels 
 * are interlinked to allow multiple channels to be updated simultaneously 
 * with the same duty cycle value.
 *
 * \param pwma         Pointer to the PWMA module.
 * \param channel_mask Channels for which the value should be loaded.
 * \param duty_cycles  Number of cycles for the active period for the channels
 *                     provided by channel_mask.
 * \retval PASS if the duty cycle value is with in the limit.
 * \retval FAIL if the duty cycle value exceeds TOP [or] TOP-SPREAD.
 * \warning The duty cycle value should be less than the TOP value(normal mode).
 *			If spread value is loaded, the duty cycle value 
 *			should not exceed TOP-SPREAD.
 */
bool pwma_set_channels_value(volatile avr32_pwma_t *pwma,
							uint64_t channel_mask,
							uint16_t duty_cycle);


/*! \brief Load multiple duty cycle values to multiple channels.
 * This function uses Interlinked Multi Value Mode where 4 channels 
 * can be updated simultaneously with 4 different duty cycle values. 
 *
 * \param pwma                Pointer to the PWMA module.
 * \param channel_mask        Channels for which the value should be loaded.
 * \param channel_duty_cycles Pointer to an array of duty cycle values.
 * \retval PASS if the duty cycle value is with in the limit.
 * \retval FAIL if the duty cycle value exceeds TOP [or] TOP-SPREAD.
 * \warning The duty cycle value should be less than the TOP value(normal mode). 
 *			If spread value is loaded, the duty cycle value 
 *			should not exceed TOP-SPREAD.
 */
bool pwma_set_multiple_values(volatile avr32_pwma_t *pwma,
							uint32_t channel_mask,
							uint16_t *channel_duty_cycles);


/*! \brief Disable a PWMA module.
 *
 * \param pwma Pointer to the PWMA module.
 */
static inline void pwma_disable(volatile avr32_pwma_t *pwma)
{
	pwma->cr &= ~AVR32_PWMA_EN_MASK;
}


/*! \brief Wait for PWMA Module to be ready for writing.
 *
 * \param pwma Pointer to the PWMA module.
 */
static inline void pwma_wait_for_ready(volatile avr32_pwma_t *pwma)
{
	 while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);
}

/*! \brief Load TOP value.
 *
 * \param pwma      Pointer to the PWMA Module.
 * \param top_value TOP value to be loaded.
 * \retval PASS if the TOP value is within the limit.
 * \retval FAIL if the TOP value exceeds its maximum limit.
 */
 bool pwma_write_top_value(volatile avr32_pwma_t *pwma,uint16_t top_value);


/*! \brief Enable the waveform output for the channels.
 * 
 * \param pwma         Pointer to the PWMA module.
 * \param channel_mask Channels for which the waveform output
 *                     mode should be enabled. 
 */
#if (defined(_AVR32_PWMA_201_H_INCLUDED) || defined(AVR32_PWMA_200_H_INCLUDED))

void pwma_enable_cwg_mode(volatile avr32_pwma_t *pwma,
						 uint32_t channel_mask);
#endif


/*! \brief Enable the input peripheral event for the channels.
 * 
 * \param pwma         Pointer to the PWMA module.
 * \param channel_mask Channels for which the input peripheral
 *                     event should be enabled. 
 * \warning The channels supporting the input peripheral events are device 
 *			specific. Please refer to the Module Configuration of PWMA module in 
 *			the respective device datasheet.
 */
#if (defined(_AVR32_PWMA_201_H_INCLUDED) || defined(AVR32_PWMA_101_H_INCLUDED))

void pwma_enable_input_peripheral_event(volatile avr32_pwma_t *pwma,
										uint64_t channel_mask);
#endif


/*! \brief Configure the input peripheral event for the channels.
 * 
 * \param pwma         Pointer to the PWMA module.
 * \param channel_mask Channels for which event response should be enabled.
 */
#if (defined(_AVR32_PWMA_201_H_INCLUDED))

void pwma_config_event_response(volatile avr32_pwma_t *pwma,
								uint64_t channel_mask);
#endif

/**
 * \}
 */

#endif /* _PWMA_H_ */
