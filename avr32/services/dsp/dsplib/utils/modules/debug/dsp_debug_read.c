/*****************************************************************************
 *
 * \file
 *
 * \brief Reading functions for the DSP library
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp_debug.h"
#include "dsp_debug_shared.h"

static int dsp_debug_parse_int(char *str)
{
  int i = -1, sign = 1;
  char c;

  if (*str == '-')
  {
    sign = -1;
    str++;
  }

  while(*str)
  {
    c = *str++;
    if (c < '0' || c > '9')
      break;
    if (i == -1)
      i = 0;
    i *= 10;
    i += c - '0';
  }

  return i*sign;
}

//! This function is used to get a string.
void dsp_debug_read(char *str, int size, char end_char)
{
  int i = 0;
  char c;

  while(i < size-1)
  {
    c = dsp_debug_read_fct();
    if (c == end_char)
      break;
    str[i++] = c;
  }
  str[i] = '\0';
}

//! This function is used to get an unsigned integer
int dsp_debug_read_ui()
{
  int i = 0;
  char c;

  while(1)
  {
    c = dsp_debug_read_fct();
    dsp_debug_buffer[i++] = c;
    if (c < '0' || c > '9')
      break;
  }

  return dsp_debug_parse_int(dsp_debug_buffer);
}

//! This function is used to get a Q formatted number
int dsp_debug_read_q(int a, int b)
{
  int i = 0;
  int zeros = 1;
  int q_num;
  char c;
  int integer = 0, decimal = 0, sign = 1;
  int _10log10;
  char data[32], *pdata;
  S64 temp;

  while(i < sizeof(data))
  {
    c = dsp_debug_read_fct();
    data[i++] = c;
    if ((c < '0' || c > '9') && (c != '.' && c != ',' && c != '-'))
      break;
  }
  // Take care of the sign
  pdata = data;
  if (*pdata == '-')
  {
    sign = -1;
    pdata++;
  }

  if ((integer = dsp_debug_parse_int(pdata)) == -1)
    integer = 0;

  // If overflow
  if (integer >= (1 << (a-1)))
  {
    if (sign == 1)
      return DSP_Q_MAX(a, b);
    else
      return DSP_Q_MIN(a, b);
  }

  // Switch to decimal data
  for(i=0; (pdata[i] >= '0' && pdata[i] <= '9') || pdata[i] <= '-'; i++);

  if (pdata[i] == '.' || pdata[i] == ',')
  {
    // Count the number of zeros before the first plain number
    for(; pdata[i+1] == '0'; i++, zeros *= 10);
    if ((decimal = dsp_debug_parse_int(&pdata[i+1])) == -1)
      decimal = 0;
  }

  // decimal/(10^(log10(decimal)+1))
  // Calculation of the integer part of log10
  _10log10 = 1;
  while(_10log10 <= decimal)
    _10log10 *= 10;
  _10log10 *= zeros;

  temp = decimal;
  temp <<= b;
  temp /= _10log10;

  q_num = temp + (integer << b);
  q_num *= sign;

  return q_num;
}
