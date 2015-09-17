/*****************************************************************************
 *
 * \file
 *
 * \brief Debugging functions for the DSP library
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

#include "dsp.h"
#include "dsp_debug.h"
#include "dsp_debug_shared.h"

//! Shared buffer to store temporary data
char dsp_debug_buffer[DSP_DEBUG_BUFFER_SIZE];

char *dsp_debug_print_ui(unsigned int n);
char *dsp_debug_print_si(int n);

// Routine used to print a string
void dsp_debug_print_fct(char *str)
{
  while(*str)
    dsp_debug_write_fct(*str++);
}

// Routine used to print an "unsigned int" into the dsp_debug_buffer
char *dsp_debug_print_ui(unsigned int n)
{
  int i = sizeof(dsp_debug_buffer)-1;

  // Point to the end of the buffer and set a EOL sign
  dsp_debug_buffer[i] = '\0';
  do
  {
    // Get the modulo 10 of the number and print this rest
    dsp_debug_buffer[--i] = '0' + n%10;
    // Divide per 10 this number
    n /= 10;
  // This until n reaches 0
  }while(n);

  // Returns a pointer on the beginning of the generated string
  return &dsp_debug_buffer[i];
}

// Routine used to print an "signed int" into the dsp_debug_buffer
char *dsp_debug_print_si(int n)
{
  char *pdsp_debug_buffer;

  // If the number is negative
  if (n < 0)
  {
    // Print the absolute value of this number
    pdsp_debug_buffer = dsp_debug_print_ui((unsigned int) (-n));
    // Add the character '-' in front of the string
    pdsp_debug_buffer--;
    *pdsp_debug_buffer = '-';
  }
  // Else
  else
    // Print this number as if it was an "unsigned int" number
    pdsp_debug_buffer = dsp_debug_print_ui((unsigned int) n);

  return pdsp_debug_buffer;
}

// Function used to initialize the debug module
void dsp_debug_initialization__(char *date, char *time, int fosc)
{
  // initialization
  dsp_debug_init(fosc);
}

// Function used to print on the default output the value of a dsp16_t variable
void dsp16_debug_print(dsp16_t n)
{
  dsp_debug_print(16, DSP16_QA, (int) n);
}

// Function used to print on the default output the value of a dsp32_t variable
void dsp32_debug_print(dsp32_t n)
{
  dsp_debug_print(32, DSP32_QA, (int) n);
}

// Function used to print on the default output the value of a dsp16_complex_t variable
void dsp16_debug_print_complex(dsp16_complex_t *n)
{
  dsp_debug_print_complex(16, DSP16_QA, n->real, n->imag);
}

// Function used to print on the default output the value of a dsp32_complex_t variable
void dsp32_debug_print_complex(dsp32_complex_t *n)
{
  dsp_debug_print_complex(32, DSP32_QA, n->real, n->imag);
}

// Function used to print on the default output the value of a dsp16_t vector
void dsp16_debug_print_vect(dsp16_t *vect, int size)
{
  int i;
  char *pdsp_debug_buffer;

  // Main loop
  for(i=0; i<size; i++)
  {
    // Print the index of the current value of the vector to print
    pdsp_debug_buffer = dsp_debug_print_ui(i+1);
    dsp_debug_print_fct(pdsp_debug_buffer);
    dsp_debug_print_fct("\t");
    // Print the value of the "i"th element of the vector
    dsp16_debug_print(vect[i]);
    dsp_debug_print_fct("\r\n");
  }
}

// Function used to print on the default output the value of a dsp32_t vector
void dsp32_debug_print_vect(dsp32_t *vect, int size)
{
  int i;
  char *pdsp_debug_buffer;

  // Main loop
  for(i=0; i<size; i++)
  {
    // Print the index of the current value of the vector to print
    pdsp_debug_buffer = dsp_debug_print_ui(i+1);
    dsp_debug_print_fct(pdsp_debug_buffer);
    dsp_debug_print_fct("\t");
    // Print the value of the "i"th element of the vector
    dsp32_debug_print(vect[i]);
    dsp_debug_print_fct("\r\n");
  }
}

// Function used to print on the default output the value of a dsp16_complex_t vector
void dsp16_debug_print_complex_vect(dsp16_complex_t *vect, int size)
{
  int i;
  char *pdsp_debug_buffer;

  // Main loop
  for(i=0; i<size; i++)
  {
    // Print the index of the current value of the vector to print
    pdsp_debug_buffer = dsp_debug_print_ui(i+1);
    dsp_debug_print_fct(pdsp_debug_buffer);
    dsp_debug_print_fct("\t");
    // Print the value of the "i"th element of the vector
    dsp16_debug_print_complex(&vect[i]);
    dsp_debug_print_fct("\r\n");
  }
}

// Function used to print on the default output the value of a dsp32_complex_t vector
void dsp32_debug_print_complex_vect(dsp32_complex_t *vect, int size)
{
  int i;
  char *pdsp_debug_buffer;

  // Main loop
  for(i=0; i<size; i++)
  {
    // Print the index of the current value of the vector to print
    pdsp_debug_buffer = dsp_debug_print_ui(i+1);
    dsp_debug_print_fct(pdsp_debug_buffer);
    dsp_debug_print_fct("\t");
    // Print the value of the "i"th element of the vector
    dsp32_debug_print_complex(&vect[i]);
    dsp_debug_print_fct("\r\n");
  }
}

// Function used to print the numbers after the comma
int dsp_debug_sprint_after_radix(char **out, unsigned int num, unsigned int den, int nb_digits)
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
    n += dsp_debug_sprint_fct(out, temp);

    num -= i*den;
  }

  // Finish the string
  **out = '\0';

  return n;
}

// This function is used to print any complex numbers
// The result follows this pattern: "a + bi" where a and b
// are two fixed point numbers
void dsp_debug_print_complex(int nb_bits, int q, int real, int imag)
{
  // Print the real part of the complex number
  dsp_debug_print(nb_bits, q, real);
  dsp_debug_print_fct(" + ");
  // Print the imaginary part of the complex number
  dsp_debug_print(nb_bits, q, imag);
  dsp_debug_print_fct("i");
}

// Print a string inside a buffer
int dsp_debug_sprint_fct(char **out, char *str)
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

// Print a fixed-point number in a buffer
int dsp_debug_sprint(char **out, int nb_bits, int q, int i)
{
  int range;
  int n = 0;
  char *pdsp_debug_buffer;
  // A table which contains the number of digit after the comma to print
  int precision[33] = { 0 /* 0 */, 2, 2, 2, 3, 3 /* 5 */, 3, 3, 4, 4, 5 /* 10 */, 5, 5, 5, 6, 6 /* 15 */, 6,
                      7, 7, 7, 8 /* 20 */, 8, 8, 9, 9, 9 /*25 */, 10, 10, 10, 11, 11 /* 30 */, 11, 12 };

  // if this is a negative number
  if (i < 0)
  {
    // Particuarly case
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
    n += dsp_debug_sprint_fct(out, "-");
  }
  else
  // range contains the floor value of the fixed-point number
    range = i >> (nb_bits-q);

  // Print the floor value
  pdsp_debug_buffer = dsp_debug_print_ui(range);
  n += dsp_debug_sprint_fct(out, pdsp_debug_buffer);
  // Print the comma
  n += dsp_debug_sprint_fct(out, ".");
  // Print the number after the comma
  n += dsp_debug_sprint_after_radix(out, i, 1 << (nb_bits-q), precision[nb_bits-q]);

  // Returns the length of the string generated
  return n;
}

// Print a fixed-point number on the default output
void dsp_debug_print(int nb_bits, int q, int n)
{
  char *pdsp_debug_buffer;

  // Point on the default buffer
  pdsp_debug_buffer = dsp_debug_buffer;
  // Print the number in this buffer
  dsp_debug_sprint(&pdsp_debug_buffer, nb_bits, q, n);
  // Print the buffer on the default output
  dsp_debug_print_fct(dsp_debug_buffer);
}


