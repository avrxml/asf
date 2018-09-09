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

#include <windows.h>

#include "format_data.h"
#include "script.h"
#include "gestion_erreurs.h"

#define ADD_DATA(data_i)    if (cur_arg == n_arg) \
                            { \
                                if (i_buffer >= size) \
                                   stop = 1; \
                                else \
                                { \                                    f = (float) data_i; \
                                    *((float *) &_buffer[i_buffer]) = f; \
                                    i_buffer += sizeof(float); \
                                } \
                            }


char *format_data_after_int(char *_buffer)
{
     int stop = 0;

     if (*_buffer == '-')
        _buffer++;

     while(*_buffer && !stop)
     {
          if (*_buffer < '0' || *_buffer > '9')
              stop = 1;
          else
              _buffer++;
     }

     return _buffer;
}

char *format_data_after_float(char *_buffer)
{
     int stop = 0;

     _buffer = format_data_after_int(_buffer);
     if (*_buffer == '.')
        _buffer++;
     if (*_buffer == '-')
        return _buffer;
     _buffer = format_data_after_int(_buffer);

     return _buffer;
}

int format_data_get_data(char *_input, char *_buffer, char *_pattern, int size, int n_arg)
{
     char _str[256];
     char _cur_pattern[256];
     char *_ppattern, *_pcur_pattern, *_pstr;
     int stop = 0;
     int cur_arg;
     int i_buffer = 0;
     int i;
     float f;

     size *= sizeof(float);

     while(*_input && !stop)
     {
         // to compute one line
         _ppattern = _pattern;
         cur_arg = 0;
         while(*_ppattern && !stop)
         {
             _input = templates_get_string(_input, _str);
             _ppattern = templates_get_string(_ppattern, _cur_pattern);

             _pstr = _str;
             _pcur_pattern = _cur_pattern;
             while(*_pstr && !stop)
             {
                   switch(*_pcur_pattern)
                   {
                   case '%':
                        _pcur_pattern++;
                        cur_arg++;
                        switch(*_pcur_pattern)
                        {
                        case 'd':
                        case 'i':
                             // MessageBox(NULL, "%i", _pstr, 0);
                             if (!sscanf(_pstr, "%f", &i))
                                stop = 1;
                             else
                             {
                                 _pstr = format_data_after_int(_pstr);
                                 ADD_DATA(i);
                             }
                             break;
                        case 'f':
                            //  MessageBox(NULL, "%f", _pstr, 0);
                             if (!sscanf(_pstr, "%f", &f))
                                stop = 1;
                             else
                             {
                                 _pstr = format_data_after_float(_pstr);
                                 ADD_DATA(f);
                             }
                             break;
                        case '%':
                             cur_arg--;
                             if (*_pstr != '%')
                                stop = 1;
                             else
                                _pstr++;
                             break;
                        default:
                             SET_ERROR("Pattern not supported!");
                             stop = 1;
                             break;
                        }
                        _pcur_pattern++;
                        break;
                   default:
                        // MessageBox(NULL, _pstr, _pcur_pattern, 0);
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
    }

    return i_buffer/sizeof(float);
}
