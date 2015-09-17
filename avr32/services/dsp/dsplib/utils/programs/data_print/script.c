/**
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <windows.h>

#include "error_management.h"
#include "script.h"

_fct_is_separator script_is_separator = NULL;

int templates_is_space(char c)
{
    int result;

    switch(c)
    {
    // If the character is a basic space
    case ' ':
    case '\t':
    case '\n':
    case '\r':
         // Return 1
         result = 1;
         break;
    // Else
    default:
         // If the extension function is set returns its result
         if (script_is_separator)
            result = script_is_separator(c);
         // Else returns 0
         else
            result = 0;
    }

    return result;
}

void script_set_is_separator_fct(_fct_is_separator is_separator)
{
     // Set the new is_separator function as an extension of the templates_is_space function
     script_is_separator = is_separator;
}

char *templates_skip_spaces(char *_str)
{
     char *_pstr;

     ASSERT(_str);

     _pstr = _str;
     // Skip spaces
     while(templates_is_space(*_pstr))
         _pstr++;

     return _pstr;
}

char *templates_get_arg(char *_args, int num)
{
     char *_new_args;
     int i;

     ASSERT(_args);

     _new_args = _args;
     // Skip the first spaces
     _new_args = templates_skip_spaces(_new_args);

     // This loop permits to point on the "num"th argument
     for(i=0; i<num; i++)
     {
         // Skip the non-spaces
         while(!templates_is_space(*_new_args))
             _new_args++;
         // skip the spaces
         _new_args = templates_skip_spaces(_new_args);
     }

     // Return a pointer on the argument
     return _new_args;
}

char *templates_get_string(char *_src, char *_dst)
{
     BOOL hold;

     ASSERT(_src);
     // Skip spaces
     _src = templates_skip_spaces(_src);

     hold = FALSE;
     // While it is not a space or we are inside a quotes and the end of the string is not reached
     while((!templates_is_space(*_src) || hold) && *_src)
     {
          // Test the character
          switch(*_src)
          {
          // If it is a " character
          case '\"':
               // hold the string
                hold = !hold;
                break;
          // Else copy the string
          default:
                *_dst++ = *_src;
          }
          *_src++;
     }
     *_dst = '\0';
     // Skip spaces
     _src = templates_skip_spaces(_src);

     return _src;
}

int script_get_value(char *_str)
{
    int value;
    char *_pstr;

    ASSERT(_str);
    // Skip the first spaces
    _str = templates_skip_spaces(_str);

    value = 0;

    // If the number is an hexadecimal value
    if (_str[0] == '0' && _str[1] == 'x')
        sscanf(&_str[2], "%X", &value);
    // The same
    else if (_pstr = strchr(_str, 'h'))
    {
         *_pstr = '\0';
         sscanf(_str, "%X", &value);
         *_pstr = 'h';
    }
    // Else it is decimal value
    else
         sscanf(_str, "%i", &value);

    return value;
}
