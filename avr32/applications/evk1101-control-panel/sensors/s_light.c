/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel light sensor module.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#include <string.h>
#include <stdio.h>

#include "compiler.h"
#include "s_light.h"
#include "board.h"
#include "gpio.h"
#include "adc.h"
#include "FreeRTOS.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

/*! ADC cell. */
static volatile avr32_adc_t * adc= (volatile avr32_adc_t *) &AVR32_ADC;

//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Get the current light sensor value.
 *
 *  \param buf char buffer in which the light sensor value is stored.
 *  \param result returns the light sensor value.
 *
 *  \return true upon success, false if error.
 */
bool b_light_get_value( char* buf, U32* result )
{
   int i_current_val;


   /* enable channel for sensor */
   adc_enable( adc, ADC_LIGHT_CHANNEL );
   /* start conversion */
   adc_start( adc );
   /* get value for sensor */
   i_current_val = (
#ifdef EVK1100_REVA
                     ADC_MAX_VALUE -
#endif
                     adc_get_value( adc, ADC_LIGHT_CHANNEL )) * 100 / ADC_MAX_VALUE;
   /* Disable channel for sensor */
   adc_disable( adc, ADC_LIGHT_CHANNEL );

   sprintf( buf, "%d%%\r\n", i_current_val);

   *result= i_current_val;
   return true;
}
