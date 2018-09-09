/**
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
