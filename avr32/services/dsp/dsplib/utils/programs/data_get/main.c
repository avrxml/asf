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

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>

#include "rs232.h"
#include "error_management.h"

#define DEFAULT_COM_PORT    "COM1"
#define DEFAULT_BAUD_RATE   9600
#define DEFAULT_BITS_NUMBER 8
#define DEFAULT_PARITY      RS232_PARITY_NOPARITY
#define DEFAULT_STOPBIT     RS232_STOP_BIT_ONE

#define MAX_ARGS  10

static volatile int sec = 0;

static int refresh_every_n_lines = 0;
static int refresh_every_n_bytes = 0;
static int iskbhit = FALSE;
static HANDLE hthread1, hthread2;

// To calculate the baud rate of the transfer
DWORD WINAPI sec_int(void *arg)
{
  while(1)
  {
    sec++;
    Sleep(1000);
  }
  return 0;
}

// To calculate the baud rate of the transfer
DWORD WINAPI kb_event(void *arg)
{
  while(1)
  {
    if (!iskbhit)
       iskbhit = (_getch())?TRUE:FALSE;
  }
  return 0;
}

// The main function that get data trough a COM port
void get_data(char *_filename, char *_port, int br, int nbits, int parity, int stopbits)
{
  char c;
  int n;
  FILE *_file = NULL;
  int n_line = 0;
  int n_bytes = 0;
  int n_byte = 0;
  DWORD dwGenericThread;

  // Make sure the port is well opened
  ASSERT(rs232_open(_port, br, nbits, parity, stopbits));

  // If the result has to be stored in a file, then initialize the file
  if (_filename)
  {
    // void the file
    ASSERT((_file = fopen(_filename, "w")) != NULL);
    fclose(_file);
  }

  // start a thread to measure the duration of the transfer
  hthread1 = CreateThread(NULL, 100, sec_int, NULL, 0, &dwGenericThread);

  // Wait until no character is received on the line
  // This is to make sure we are well synchronized on the line
  do
  {
    n = 0;
    sec = 0;
    ASSERT(rs232_read(&c, 1, &n));
  }while(n == 1 && sec < 1);

  printf(".");
  fflush(stdout);

  // Wait until the transfer restart/start
  do
  {
    sleep(1);
    n = 0;
    ASSERT(rs232_read(&c, 1, &n));
  }while(n != 1);

  printf(".");
  fflush(stdout);

  // Initialization
  n_line = refresh_every_n_lines;
  n_byte = refresh_every_n_bytes;
  sec = 0;

  // Open the file for writing
  if (_filename)
     ASSERT(_file = fopen(_filename, "wb"));

  // start a thread to measure the duration of the transfer
  hthread2 = CreateThread(NULL, 100, kb_event, NULL, 0, &dwGenericThread);

  iskbhit = FALSE;
  do
  {
     // If the result has to be stored in a file...
     if (_filename)
     {
         // increment the number of byte read
         n_bytes++;
         // Decrement the number of byte left until the file has to be updated
         n_byte--;
         // Message
         printf("\rReceived: %i Byte(s) (%i b/s) <Press ENTER to stop>", n_bytes, (sec)?(n_bytes/sec):0);
         fflush(stdout);

         // Open the file if it is not done yet
         if (!_file && (refresh_every_n_lines || refresh_every_n_bytes))
             ASSERT(_file = fopen(_filename, "a+b"));

         // Print the received character on it
         fputc(c, _file);

         // Decrement the number of line received once a '\n' is received
         if (c == '\n' && refresh_every_n_lines)
           n_line--;

         // Update the file if the number of line received matches with the -l argument
         if (!n_line && refresh_every_n_lines)
         {
               fclose(_file);
               _file = NULL;
               n_line = refresh_every_n_lines;
         }
         // Update the file if the number of line received matches with the -b argument
         else if (!n_byte && refresh_every_n_bytes)
         {
               fclose(_file);
               _file = NULL;
               n_byte = refresh_every_n_bytes;
         }
     }
     // Else just write the the character received on the default output
     else
         putchar(c);
     n = 0;
     // Read a new character
     ASSERT(rs232_read(&c, 1, &n));

  }while(n == 1 && !iskbhit);

  // Close the file
  if (_file)
    fclose(_file);

  // Close the RS232 port
  rs232_close();
}

// The use of this program
void print_help()
{
  printf("Usage: DataGet [-l number] [-b number] [-f filename] [-h] [%%PORT [%%BAUD_RATE [%%BITS_PER_CHAR [%%PARITY [%%STOPBIT]]]]]\n\r"
    "PARITY = 0: NO PARITY\n\r"
    "         1: PARITY EVEN\n\r"
    "         2: PARITY ODD\n\r"
    "STOPBIT = 1: ONE STOPBIT\n\r"
    "          2: ONE POINT FIVE STOPBIT\n\r"
    "          3: TWO STOPBITS\n\r"
    "OPTIONS:\n\r"
    "        -l number    Refresh the file once \"number\" lines have been received.\n\r"
    "        -b number    Refresh the file once \"number\" bytes have been received.\n\r"
    "        -f filename  Print the results in a file.\n\r"
    "        -h           Display these lines.\n\r"
    "By default, if no argument is passed to this program, the acquisition will be done on the " DEFAULT_COM_PORT ", at 9600 bauds (8-N-1).\n\r"
  );
}

int main(int argc, char *argv[])
{
  int parity[] = {RS232_PARITY_NOPARITY, RS232_PARITY_EVEN,RS232_PARITY_ODD};
  int stopbit[] = {0, RS232_STOP_BIT_ONE, RS232_STOP_BIT_ONE5, RS232_STOP_BIT_TWO};
  int i, argc_n;
  static char *argv_n[MAX_ARGS];
  char *_filename = NULL;

  argc_n = 0;
  // Read the arguments
  for(i=1; i<argc; i++)
  {
    // If an option as been specified
    if (argv[i][0] == '-')
    {
      // Decode it
      switch(argv[i][1])
      {
      // Refresh every N lines
      case 'l':
        if (i+1 < argc)
          refresh_every_n_lines = atoi(argv[++i]);
        else
        {
          print_help();
          return 0;
        }
        break;
      // Refresh every N bytes
      case 'b':
        if (i+1 < argc)
          refresh_every_n_bytes = atoi(argv[++i]);
        else
        {
          print_help();
          return 0;
        }
        break;
      // Print in a file
      case 'f':
        if (i+1 < argc)
          _filename = argv[++i];
        else
        {
          print_help();
          return 0;
        }
        break;
      // Display help
      case 'h':
        print_help();
        return 0;
      // If the option is not recognized, print the usage help
      default:
        printf("Unknown option '-%c'\r\n", argv[i][1]);
        print_help();
        return 0;
      }
    }
    // else it is a classical argument
    else
    {
      if (argc_n < MAX_ARGS)
      {
        argv_n[argc_n] = argv[i];
        argc_n++;
      }
    }
  }

  // Call the get_data function with default options according to the number of arguments passed to this program
  if (argc_n == 0)
     get_data(_filename, DEFAULT_COM_PORT, DEFAULT_BAUD_RATE, DEFAULT_BITS_NUMBER, DEFAULT_PARITY, DEFAULT_STOPBIT);
  else if (argc_n == 1)
     get_data(_filename, argv_n[0], DEFAULT_BAUD_RATE, DEFAULT_BITS_NUMBER, DEFAULT_PARITY, DEFAULT_STOPBIT);
  else if (argc_n == 2)
     get_data(_filename, argv_n[0], atoi(argv_n[1]), DEFAULT_BITS_NUMBER, DEFAULT_PARITY, DEFAULT_STOPBIT);
  else if (argc_n == 3)
     get_data(_filename, argv_n[0], atoi(argv_n[1]), atoi(argv_n[2]), DEFAULT_PARITY, DEFAULT_STOPBIT);
  else if (argc_n == 4)
     get_data(_filename, argv_n[0], atoi(argv_n[1]), atoi(argv_n[2]), parity[atoi(argv_n[3])], DEFAULT_STOPBIT);
  else if (argc_n == 5)
     get_data(_filename, argv_n[0], atoi(argv_n[1]), atoi(argv_n[2]), parity[atoi(argv_n[3])], stopbit[atoi(argv_n[4])]);
  else
     print_help();

  TerminateThread(hthread1, 0);
  TerminateThread(hthread2, 0);

  return 0;
}

