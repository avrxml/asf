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
