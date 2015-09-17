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

#include <stdio.h>
#include <stdlib.h>
#include "error_management.h"
#include "format_data.h"

void format_data(char *_path, char *_pattern, int n_arg)
{
     char *_buffer;
     int n, n_data;
     FILE *_file;

     ASSERT(_path);

     // Open the file
     _file = fopen(_path, "rb");
     ASSERT(_file);
     // Get the length of the file and store it into n
     fseek(_file, 0, SEEK_END);
     n = ftell(_file);
     // allocate a buffer of the size of the file
     _buffer = (char *) malloc(n + 1);
     ASSERT(_buffer);
     _buffer[n] = '\0';
     // Go to the beginning of the file
     fseek(_file, 0, SEEK_SET);
     // Read it
     fread(_buffer, 1, n, _file);
     fclose(_file);

     // Extract data from a buffer
     n_data = format_data_get_data(_buffer, _pattern, n_arg);

     free(_buffer);
}

// Main function
int main(int argc, char *argv[])
{

  // If there arguments passed to this program
  if (argc == 4)
     format_data(argv[1], argv[2], atoi(argv[3]));
  // Else print the usage
  else
     printf("Usage: DataExtract input_buffer_file_path pattern n_arg\n\r");

  return 0;
}
