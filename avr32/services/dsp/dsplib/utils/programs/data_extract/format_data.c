/**
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "format_data.h"
#include "script.h"
#include "error_management.h"

#define ADD_DATA(data_i)    if (cur_arg == n_arg) \
                            { \
                              f = (float) data_i; \
                              value_to_print = f; \
                            }

// Returns a pointer on a string that point just after an integer
char *format_data_after_int(char *_buffer)
{
     int stop = 0;

     // If there is a negative character, skip it
     if (*_buffer == '-')
        _buffer++;
     // loop until the character is in the range [0;9]
     while(*_buffer && !stop)
     {
          if (*_buffer < '0' || *_buffer > '9')
              stop = 1;
          else
              _buffer++;
     }

     // Returns the pointer
     return _buffer;
}

// Returns a pointer on a string that point just after a float
char *format_data_after_float(char *_buffer)
{
     int stop = 0;

     // Point after the first integer
     _buffer = format_data_after_int(_buffer);
     // if the next character is a comma, increment the pointer
     if (*_buffer == '.')
        _buffer++;
     // if the next character is a comma, increment the pointer
     if (*_buffer == '-')
        return _buffer;
     // Point after the next integer (corresponding to the decimals).
     _buffer = format_data_after_int(_buffer);

     return _buffer;
}

// Extract data from a buffer according to a specified pattern
int format_data_get_data(char *_input, char *_pattern, int n_arg)
{
     char _str[256];
     char _cur_pattern[256];
     char *_ppattern, *_pcur_pattern, *_pstr;
     int stop = 0;
     int cur_arg;
     int i_buffer = 0;
     int i;
     float f, value_to_print;
     char *_end_of_line;

     // Main loop
     while(*_input)
     {
         // To compute one line
         _ppattern = _pattern;
         // Initialization
         cur_arg = 0;
         stop = 0;

         _end_of_line = _input;
         // Look for the end of line
         while(*_end_of_line && *_end_of_line != '\n')
            _end_of_line++;

         // While the pattern matches with the current line
         while(*_ppattern && !stop)
         {
             // If the current pointer overflows, then break
             if (_input > _end_of_line)
             {
                stop = 1;
                break;
             }
             // Get the first sequence of characters
             _input = templates_get_string(_input, _str);
             // Get the first pattern that has to match with the string
             _ppattern = templates_get_string(_ppattern, _cur_pattern);

             _pstr = _str;
             _pcur_pattern = _cur_pattern;
             // Pattern matching loop
             while(*_pstr && !stop)
             {
                   // Check the current pattern
                   switch(*_pcur_pattern)
                   {
                   // If it is a special pattern ("%i", "%f", ...)
                   case '%':
                        _pcur_pattern++;
                        cur_arg++;
                        switch(*_pcur_pattern)
                        {
                        // For "%i" and "%d" patterns
                        case 'd':
                        case 'i':
                             // If the pattern does not match with the line, then break
                             if (!sscanf(_pstr, "%f", &i))
                                stop = 1;
                             // Else
                             else
                             {
                                 // Read the integer data
                                 _pstr = format_data_after_int(_pstr);
                                 // Cast it
                                 ADD_DATA(i);
                             }
                             break;
                        // For "%f" pattern
                        case 'f':
                             // If the pattern does not match with the line, then break
                             if (!sscanf(_pstr, "%f", &f))
                                stop = 1;
                             // Else
                             else
                             {
                                 // Read the integer data
                                 _pstr = format_data_after_float(_pstr);
                                 // Cast it
                                 ADD_DATA(f);
                             }
                             break;
                        // If it is the character %
                        case '%':
                             // Then just check if the line matches
                             cur_arg--;
                             if (*_pstr != '%')
                                stop = 1;
                             else
                                _pstr++;
                             break;
                        default:
                             // Set an error
                             SET_ERROR("Pattern not supported!");
                             stop = 1;
                             break;
                        }
                        // Next pattern
                        _pcur_pattern++;
                        break;
                   // Else for classical characters
                   default:
                        // Just check if it matches with the line
                        if (*_pstr != *_pcur_pattern)
                           stop = 1;
                        else
                        {
                            _pstr++;
                            _pcur_pattern++;
                        }
                   }
             }
         }
         // If no error occurred, print the number on the default output
         if (!stop)
            printf("%.10f\r\n", value_to_print);
    }

    return i_buffer/sizeof(float);
}
