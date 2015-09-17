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
