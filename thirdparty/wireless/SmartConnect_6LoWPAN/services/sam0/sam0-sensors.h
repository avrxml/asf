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
#ifndef _SAMR21_SENSORS_H_
#define _SAMR21_SENSORS_H_

#include "asf.h"
//#include "adc.h"
//#include "instance_adc.h"
//#include "component_adc.h"
//#include "samd20-sensors.h"
/*---------------------------------------------------------------------------*/
/* ADC inputs, refer to section 5.1 (multiplex) in datasheet. */
#if 0
enum samd20_sensor_input {
  SAMD20_SENSOR_ADC_PIN0 = ADC_POSITIVE_INPUT_PIN0,      /* ADC0 pin, PA02 */
  SAMD20_SENSOR_ADC_PIN1 = ADC_POSITIVE_INPUT_PIN1,      /* ADC1 pin, PA03 */
  SAMD20_SENSOR_ADC_PIN2 = ADC_POSITIVE_INPUT_PIN2,      /* ADC2 pin, PB08 */
  SAMD20_SENSOR_ADC_PIN3 = ADC_POSITIVE_INPUT_PIN3,      /* ADC3 pin, PB09 */
  SAMD20_SENSOR_ADC_PIN4 = ADC_POSITIVE_INPUT_PIN4,      /* ADC4 pin */
  SAMD20_SENSOR_ADC_PIN5 = ADC_POSITIVE_INPUT_PIN5,      /* ADC5 pin */
  SAMD20_SENSOR_ADC_PIN6 = ADC_POSITIVE_INPUT_PIN6,      /* ADC6 pin */
  SAMD20_SENSOR_ADC_PIN7 = ADC_POSITIVE_INPUT_PIN7,      /* ADC7 pin */
  SAMD20_SENSOR_ADC_PIN8 = ADC_POSITIVE_INPUT_PIN8,      /* ADC8 pin, PB00 */
  SAMD20_SENSOR_ADC_PIN9 = ADC_POSITIVE_INPUT_PIN9,      /* ADC9 pin, PB01 */
  SAMD20_SENSOR_ADC_PIN10 = ADC_POSITIVE_INPUT_PIN10,    /* ADC10 pin, PB02 */
  SAMD20_SENSOR_ADC_PIN11 = ADC_POSITIVE_INPUT_PIN11,    /* ADC11 pin, PB03 */
  SAMD20_SENSOR_ADC_PIN12 = ADC_POSITIVE_INPUT_PIN12,    /* ADC12 pin */
  SAMD20_SENSOR_ADC_PIN13 = ADC_POSITIVE_INPUT_PIN13,    /* ADC13 pin */
  SAMD20_SENSOR_ADC_PIN14 = ADC_POSITIVE_INPUT_PIN14,    /* ADC14 pin */
  SAMD20_SENSOR_ADC_PIN15 = ADC_POSITIVE_INPUT_PIN15,    /* ADC15 pin */
  SAMD20_SENSOR_ADC_PIN16 = ADC_POSITIVE_INPUT_PIN16,    /* ADC16 pin, PA08 */
  SAMD20_SENSOR_ADC_PIN17 = ADC_POSITIVE_INPUT_PIN17,    /* ADC17 pin, PA09 */
  SAMD20_SENSOR_ADC_PIN18 = ADC_POSITIVE_INPUT_PIN18,    /* ADC18 pin */
  SAMD20_SENSOR_ADC_PIN19 = ADC_POSITIVE_INPUT_PIN19,    /* ADC19 pin */
  SAMD20_SENSOR_ADC_PIN20 = ADC_POSITIVE_INPUT_PIN20,    /* ADC20 pin */
  SAMD20_SENSOR_ADC_PIN21 = ADC_POSITIVE_INPUT_PIN21,    /* ADC21 pin */
  SAMD20_SENSOR_ADC_PIN22 = ADC_POSITIVE_INPUT_PIN22,    /* ADC22 pin */
  SAMD20_SENSOR_ADC_PIN23 = ADC_POSITIVE_INPUT_PIN23,    /* ADC23 pin */
  SAMD20_SENSOR_ADC_TEMP = ADC_POSITIVE_INPUT_TEMP,      /* Temperature reference */
  SAMD20_SENSOR_ADC_BANDGAP = ADC_POSITIVE_INPUT_BANDGAP,/* Bandgap voltage */
  SAMD20_SENSOR_ADC_SCALEDCOREVCC = ADC_POSITIVE_INPUT_SCALEDCOREVCC,    /* 1/4 scaled core supply */
  SAMD20_SENSOR_ADC_SCALEDIOVCC = ADC_POSITIVE_INPUT_SCALEDIOVCC,        /* 1/4 scaled I/O supply */
  SAMD20_SENSOR_ADC_DAC = ADC_POSITIVE_INPUT_DAC,        /* DAC input */
};
/*---------------------------------------------------------------------------*/
void      samd0_sensors_init(void);
void      samd0_sensors_set_input(enum samd0_sensor_input input);
int       samd0_sensors_busy(void);
uint16_t  samd0_sensors_read(void);
#endif
/*---------------------------------------------------------------------------*/
#endif  /* _SAMD20_SENSORS_H_ */
