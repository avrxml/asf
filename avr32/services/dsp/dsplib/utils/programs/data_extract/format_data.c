/**
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
