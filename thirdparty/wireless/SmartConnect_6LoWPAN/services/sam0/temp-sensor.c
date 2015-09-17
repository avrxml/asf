#if 0
/*
 * Copyright (c) 2013, Thingsquare, www.thingsquare.com.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COMPANY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COMPANY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "contiki.h"
#include "temp-sensor.h"
#include "asf.h"
#include "adc.h"
#include "instance_adc.h"
#include "samd20-sensors.h"
/*---------------------------------------------------------------------------*/
/* 
 * The temperature sensor (datasheet 32.8.8) has the following characteristics:
 *    0.667 V at T = 25 deg C -> value is ca 2732 (with 12 bit/1V reference)
 *    2.36 mV / deg C
 */
static int
temp_sensor_value(int type)
{
  uint16_t ret = 0;
  samd20_sensors_set_input(SAMD20_SENSOR_ADC_SCALEDCOREVCC);
  ret = samd20_sensors_read();
  return ret;
}
/*---------------------------------------------------------------------------*/
static int
temp_sensor_configure(int type, int value)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
void
temp_sensor_init(void)
{
  samd20_sensors_init();
}
/*---------------------------------------------------------------------------*/
static int
temp_sensor_status(int type)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(temp_sensor, "Temperature sensor",
               temp_sensor_value, temp_sensor_configure, temp_sensor_status);
#endif
