/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel light sensor module.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
