/*****************************************************************************
 *
 * \file
 *
 * \brief sprintf with DSP type support
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

#include <string.h>
#include <stdarg.h>
#include "dsp.h"
#include "dsp_sprintf.h"

static int print(int size, char **out, const char *format, va_list args);
static int prints(char **out, const char *string, int width, int pad, int printlimit, bool IsNumber);
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase, int printlimit);
static void printchar(char **str, int c);
static int dsp_sprint_fct(char **out, char *str);
static int dsp_sprint(char **out, int nb_bits, int q, int i);
static char *dsp_sprint_ui(char *out, unsigned int n);
static int dsp_sprint_after_radix(char **out, unsigned int num, unsigned int den, int nb_digits);

// Print a string inside a buffer
static int dsp_sprint_fct(char **out, char *str)
{
  int n = 0;

  // Main loop
  while(*str)
  {
    n++;
    *(*out)++ = *str++;
  }
  **out = '\0';

  // Return the length of the string
  return n;
}

#define TEMP_BUFFER_SIZE  32

static char *dsp_sprint_ui(char *out, unsigned int n)
{
  char *dsp_debug_buffer = out;
  int i = TEMP_BUFFER_SIZE-1;

  // Point to the end of the buffer and set a EOL sign
  dsp_debug_buffer[i] = '\0';
  do
  {
    // Get the modulo 10 of the number and print this rest
    dsp_debug_buffer[--i] = '0' + n%10;
    // Divide per 10 this number
    n /= 10;
  // This until n reaches 0
  } while (n);

  // Returns a pointer on the beginning of the generated string
  return &dsp_debug_buffer[i];
}

// Function used to print the numbers after the comma
static int dsp_sprint_after_radix(char **out, unsigned int num, unsigned int den, int nb_digits)
{
  unsigned int i;
  char temp[2];
  int n = 0;

  temp[1] = '\0';

  // Go until reaching desired numbers (the one after the comma)
  i = num/den;
  num -= i*den;

  // Then print the numbers after the radix
  while(nb_digits-- && num)
  {
    // This is to minimize the overflows
    // If the numerator is not too high, then multiply its value per 10
    // Else divide the value of the denominator per 10 which has the same effect
    if (num <= 0xFFFFFFFF/10)
      num *= 10;
    else
      den /= 10;

    // Get the quotient of the division of num/den
    i = num/den;
    // Saturate i
    if (i > 9)
      i = 9;

    // Print the number on the default output
    temp[0] = (char) i + '0';
    n += dsp_sprint_fct(out, temp);

    num -= i*den;
  }

  // Finish the string
  **out = '\0';

  return n;
}

// Print a fixed-point number in a buffer
static int dsp_sprint(char **out, int nb_bits, int q, int i)
{
  int range;
  int n = 0;
  char *pdsp_debug_buffer;
  char buffer[TEMP_BUFFER_SIZE];

  // if this is a negative number
  if (i < 0)
  {
    // Particular case
    if (i == 0x80000000)
    {
      i = 0;
      range = 1;
    }
    else
    {
    // Get its absolute value
      i = -i;
    // range contains the floor value of the fixed-point number
      range = i >> (nb_bits-q);
    }
  // Print the '-' character to specify that it is a negative number
    n += dsp_sprint_fct(out, "-");
  }
  else
  // range contains the floor value of the fixed-point number
    range = i >> (nb_bits-q);

  // Print the floor value
  pdsp_debug_buffer = dsp_sprint_ui(buffer, range);
  n += dsp_sprint_fct(out, pdsp_debug_buffer);


  // Print the comma
  n += dsp_sprint_fct(out, ".");
  // Print the number after the comma
  n += dsp_sprint_after_radix(out, i, 1 << (nb_bits-q), 4);


  // Returns the length of the string generated
  return n;
}

// Print a character
static void printchar(char **str, int c)
{
  if (str)
  {
    **str = c;
    ++(*str);
  }
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

// Print a string
static int prints(char **out, const char *string, int width, int pad, int printlimit, bool IsNumber)
{
  register int pc = 0, padchar = ' ';
  int i, len;
  register const char *ptr;

  if (width > 0)
  {
    register int len = 0;
    for (ptr = string; *ptr; ++ptr)
      ++len;
    if (len >= width)
      width = 0;
    else
      width -= len;
    if (pad & PAD_ZERO)
      padchar = '0';
  }
  if (!(pad & PAD_RIGHT))
  {
    for(; width > 0; --width)
    {
      printchar (out, padchar);
      ++pc;
    }
  }
  if (false == IsNumber)
  {
    // The string to print is not the result of a number conversion to ascii.
    // For a string, printlimit is the max number of characters to display.
    for ( ; printlimit && *string ; ++string, --printlimit)
    {
      printchar (out, *string);
      ++pc;
    }
  }
  if (true == IsNumber)
  {
    // The string to print represents an integer number.
    // In this case, printlimit is the min number of digits to print.
    // If the length of the number to print is less than the min nb of i
    // digits to display, we add 0 before printing the number.
    len = strlen(string);
    if (len < printlimit)
    {
      i = printlimit - len;
      for(; i; i--)
      {
        printchar (out, '0');
        ++pc;
      }
    }
  }
  // Else: The string to print is not the result of a number conversion to ascii.
  // For a string, printlimit is the max number of characters to display.
  for(; printlimit && *string ; ++string, --printlimit)
  {
    printchar (out, *string);
    ++pc;
  }

  for(; width > 0; --width)
  {
    printchar (out, padchar);
    ++pc;
  }

  return pc;
}

// the following should be enough for 32 bit int
#define PRINT_BUF_LEN 12

// Permits to print a signed integer including many parameters
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase, int printlimit)
{
  char print_buf[PRINT_BUF_LEN];
  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;

  // If the number equals to 0
  if (i == 0)
  {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints(out, print_buf, width, pad, printlimit, true);
  }

  // in case the result has to be displayed as a negative number
  if (sg && b == 10 && i < 0)
  {
    neg = 1;
    u = -i;
  }

  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';

  // Print the number with the specified base
  while (u)
  {
    t = u % b;
    if( t >= 10 )
      t += letbase - '0' - 10;
    *--s = t + '0';
    u /= b;
  }

  // Negate the result
  if (neg)
  {
    if(width && (pad & PAD_ZERO))
    {
      printchar (out, '-');
      ++pc;
      --width;
    }
    else
      *--s = '-';
  }

  return pc + prints(out, s, width, pad, printlimit, true);
}

// Main function that print any data
static int print(int size, char **out, const char *format, va_list args)
{
  register int width, pad, printlimit;
  register int pc = 0;
  char scr[2];

  for(; *format != 0; ++format)
  {
    if (*format == '%')
    {
      ++format;
      width = pad = printlimit = 0;
      // End of string
      if (*format == '\0')
        break;
      // For the % character
      if (*format == '%')
        goto out;
      // Right padding
      if (*format == '-')
      {
        ++format;
        pad = PAD_RIGHT;
      }
      // Zero padding
      while(*format == '0')
      {
        ++format;
        pad |= PAD_ZERO;
      }
      // width of the result
      for(; *format >= '0' && *format <= '9'; ++format)
      {
        width *= 10;
        width += *format - '0';
      }
      // Limit width of the result
      if (*format == '.')
      {
        ++format;
        for(; *format >= '0' && *format <= '9'; ++format)
        {
          printlimit *= 10;
          printlimit += *format - '0';
        }
      }
      if (0 == printlimit)
        printlimit--;
      // To print a string
      if (*format == 's')
      {
        register char *s = (char *) va_arg( args, int );
        pc += prints(out, s?s:"(null)", width, pad, printlimit, false);
        continue;
      }
      // To print a signed integer
      if (*format == 'd' || *format == 'i')
      {
        pc += printi(out, va_arg( args, int ), 10, 1, width, pad, 'a', printlimit);
        continue;
      }
      // To print in octal format
      if (*format == 'o')
      {
        pc += printi(out, va_arg( args, int ), 8, 0, width, pad, 'a', printlimit);
        continue;
      }
      // To print in hexadecimal format (lower case)
      if (*format == 'x' )
      {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a', printlimit);
        continue;
      }
      // To print in hexadecimal format (upper case)
      if (*format == 'X')
      {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A', printlimit);
        continue;
      }
      // To print unsigned integer
      if (*format == 'u')
      {
        pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a', printlimit);
        continue;
      }
      // To print a character
      if (*format == 'c')
      {
        // char are converted to int then pushed on the stack
        scr[0] = (char) va_arg( args, int );
        scr[1] = '\0';
        pc += prints (out, scr, width, pad, printlimit, false);
        continue;
      }
      // To print a fixed-point number
      if (*format == 'f')
      {
        switch(size)
        {
        // a 16-bit fixed-point data
        case 16:
          pc += dsp_sprint(out, 16, DSP16_QA, va_arg(args, int));
          continue;
        // a 32-bit fixed-point data
        case 32:
          pc += dsp_sprint(out, 32, DSP32_QA, va_arg(args, int));
          continue;
        }
      }
    }
    else
    {
    // Print the character
    out:
      printchar (out, *format);
      ++pc;
    }
  }

  if (out)
    **out = '\0';
  va_end( args );
  return pc;
}

// sprintf for 16-bit fixed-point values
int dsp16_sprintf(char *out, const char *format, ...)
{
  va_list args;
  int n;
  char *pdsp_debug_buffer;

  // Point on the default buffer
  pdsp_debug_buffer = out;

  // Compute the printf with specifics arguments
  va_start(args, format);
  n = print(16, &pdsp_debug_buffer, format, args);
  va_end(args);

  return n;
}

// sprintf for 32-bit fixed-point values
int dsp32_sprintf(char *out, const char *format, ...)
{
  va_list args;
  int n;
  char *pdsp_debug_buffer;

  // Point on the default buffer
  pdsp_debug_buffer = out;

  // Compute the printf with specifics arguments
  va_start(args, format);
  n = print(32, &pdsp_debug_buffer, format, args);
  va_end(args);

  return n;
}

