/*****************************************************************************
 *
 * \file
 *
 * \brief Reading functions for the DSP library
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
