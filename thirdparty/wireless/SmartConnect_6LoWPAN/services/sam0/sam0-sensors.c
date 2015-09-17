/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include "contiki.h"
#include "asf.h"
/*#include "adc.h"
  #include "instance_adc.h"*/
#include "temp-sensor.h"
#include "voltage-sensor.h"
#include "button-sensor.h"
#include "battery-sensor.h"
#include "sam0-sensors.h"
/*---------------------------------------------------------------------------*/
//static int first_init = 0;
//static struct adc_module adc_instance;
void sam0_sensors_init(void);
/* This defines the set of sensors used in the SAMD20/D21/R21 Xplained boards. */

SENSORS(&button_sensor,&battery_sensor);
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initiate the sensors and ADC on the SAM D20/D21/R21 X-plained board.
 */
void
sam0_sensors_init(void)
{
#if 0
  if(first_init == 0) {
    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);
    config_adc.resolution = ADC_RESOLUTION_12BIT;
    config_adc.reference = ADC_REFERENCE_INT1V;
    config_adc.clock_source = GCLK_GENERATOR_2;
    
    /* set ADC channel and init */
    config_adc.positive_input = SAMD20_SENSOR_ADC_TEMP;
    adc_init(&adc_instance, ADC, &config_adc);
    first_init = 1;
  }
#endif
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Set ADC input channel
 * \param input    ADC input channel
 *            This function sets the channel to use for the ADC. The result can
 *            be read with sam0_sensors_read();
 */
#if 0
void
sam0_sensors_set_input(enum sam0_sensor_input input)
{
  /* turn on internal temp sensor if necessary */
  if(input == sam0_SENSOR_ADC_TEMP) {
    REG_SYSCTRL_VREF |= 1 << 1;
  } else {
    REG_SYSCTRL_VREF &= ~(1 << 1);
  }

  adc_disable(&adc_instance);
  adc_flush(&adc_instance);
  adc_set_positive_input(&adc_instance, input);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Read the sensor previously selected with sam0_sensors_set_input()
 * \return     Sensor value.
 */
uint16_t
sam0_sensors_read(void)
{
  uint16_t result = 0;
  adc_enable(&adc_instance);
  adc_start_conversion(&adc_instance);
  while(adc_read(&adc_instance, &result) == STATUS_BUSY);
  return result;
}
/*---------------------------------------------------------------------------*/

#endif
