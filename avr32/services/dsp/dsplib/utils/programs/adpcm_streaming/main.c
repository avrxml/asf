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

#include "rs232.h"

// Port
#define RS232_PORT       "COM1"
// Speed
#define RS232_BAUD_RATE  CBR_115200
// Size of a byte (in the protocol)
#define RS232_BYTE_SIZE  8
// Parity
#define RS232_PARITY     RS232_PARITY_NOPARITY
// Stop bit
#define RS232_STOP_BIT   RS232_STOP_BIT_ONE

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long


// Structures used to parse the WAVE file
//******************************************
#define WAVE_FORMAT_DVI_ADPCM  0x0011

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  BYTE riff_type[4];
}s_wave_riff;

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  WORD compression_code;
  WORD nb_channels;
  DWORD sample_rate;
  DWORD avg_bytes_per_sec;
  WORD block_align;
  WORD bits_per_sample;
  WORD extra_bytes;
}s_wave_fmt;

typedef struct __attribute__((__packed__))
{
  s_wave_fmt fmt;
  WORD samples_per_block;
}s_wave_fmt_dvi;

typedef struct __attribute__((__packed__))
{
  BYTE chunk_id[4];
  DWORD chunk_size;
}s_wave_data;

typedef struct __attribute__((__packed__))
{
  WORD isamp0;
  BYTE step_table_index;
  BYTE reserved;
}s_wave_dvi_block_header;

//******************************************

/*
 * Get a structure from a file starting with the specific string
 */
int fget_struct(FILE *_file, void *_ptr, int size, char *_start_str)
{
  int end;
  char *_str;

  end = 0;
  while(!feof(_file) && !end)
  {
    _str = _start_str;
    while(*_str == fgetc(_file))
    {
      _str++;
      if (!*_str)
      {
        end = 1;
        break;
      }
    }
  }

  if (!end)
    return 0;

  fseek(_file, -strlen(_start_str), SEEK_CUR);
  fread(_ptr, 1, size, _file);

  return 1;
}

int main(int argc, char *_argv[])
{
  FILE *_file;
  s_wave_riff header_riff;
  s_wave_fmt_dvi header_dvi;
  s_wave_data header_data;
  s_wave_dvi_block_header header_block;
  short step_index;
  short predicted_value;
  char _buffer[4];
  int i, j, k, nb_bytes_per_block;
  int block_size;
  char c;
  int block_sent = 0;
  int end = 0;
  char _progress_bar[33];

  if (argc != 2)
  {
    printf("usage: ADPCM_IMA_DVI filename\n");
    return 0;
  }

  _file = fopen(_argv[1], "rb");

  if (!_file)
  {
    printf("Unable to open file.\n");
    return 0;
  }

  printf("IMA/DVI ADPCM decoder\n");
  printf("");

  if (!fget_struct(_file, &header_riff, sizeof(s_wave_riff), "RIFF"))
  {
    printf("Invalid RIFF File.\n");
    fclose(_file);
    return 0;
  }
  if (!fget_struct(_file, &header_dvi, sizeof(s_wave_fmt_dvi), "fmt "))
  {
    printf("Invalid RIFF Format.\n");
    fclose(_file);
    return 0;
  }
  if (!fget_struct(_file, &header_data, sizeof(s_wave_data), "data"))
  {
    printf("Invalid RIFF Data.\n");
    fclose(_file);
    return 0;
  }

  if (header_dvi.fmt.compression_code != WAVE_FORMAT_DVI_ADPCM)
  {
    printf("Invalid IMA/DVI ADPCM File.\n");
    fclose(_file);
    return 0;
  }

  if (header_dvi.fmt.bits_per_sample != 4)
  {
    printf("Error! The input adpcm wav file must have a number of bits per sample equals to 4.\n");
    fclose(_file);
    return 0;
  }

  printf("File name: %s\n", _argv[1]);
  printf("Number of channels: %i\n", header_dvi.fmt.nb_channels);
  printf("Sample rate: %i Hz\n", header_dvi.fmt.sample_rate);
  printf("Total average data rate: %i\n", header_dvi.fmt.avg_bytes_per_sec);
  printf("Block alignment: %i bytes\n", header_dvi.fmt.block_align);
  printf("Number of bits per sample: %i bits\n", header_dvi.fmt.bits_per_sample);
  printf("Number of samples per channel per Block: %i samples\n", header_dvi.samples_per_block);

  if (header_dvi.fmt.nb_channels > 1)
     printf("This program will only decode the last channel of the input wav file.\n");

  printf("Opening serial port %s...", RS232_PORT);
  fflush(stdout);

  // Open the desired rs232 port
  if (!rs232_open(RS232_PORT, RS232_BAUD_RATE, RS232_BYTE_SIZE, RS232_PARITY, RS232_STOP_BIT))
  {
    printf("\t[ FAILED ]\n");
    fclose(_file);
    return 0;
  }

  printf("\t[ OK ]\n");

  // Support only the 4 bits per sample format.
  nb_bytes_per_block = (header_dvi.fmt.block_align/(4*header_dvi.fmt.nb_channels)-1);
  block_size = nb_bytes_per_block*4;

  printf("Waiting for serial communication with the interface...");
  fflush(stdout);

  // Wait until the start sending command is received
  do
  {
    while(!rs232_read(&c, 1, &k));
  }while(k != 1 && c != 'S');

  // Send the length of the data block (4 bytes)
  rs232_write(&((char *) &block_size)[3], 1, &k);
  rs232_write(&((char *) &block_size)[2], 1, &k);
  rs232_write(&((char *) &block_size)[1], 1, &k);
  rs232_write(&((char *) &block_size)[0], 1, &k);

  // Send the sample rate of the sound (4 bytes)
  rs232_write(&((char *) &(header_dvi.fmt.sample_rate))[3], 1, &k);
  rs232_write(&((char *) &(header_dvi.fmt.sample_rate))[2], 1, &k);
  rs232_write(&((char *) &(header_dvi.fmt.sample_rate))[1], 1, &k);
  rs232_write(&((char *) &(header_dvi.fmt.sample_rate))[0], 1, &k);

  printf("\t[ OK ]\nSending initialization parameters.\n");

  for(j=0; j<header_data.chunk_size/header_dvi.fmt.block_align && !end; j++)
  {
    // Read the last channel
    for(i=0; i<header_dvi.fmt.nb_channels; i++)
      fread(&header_block, 1, sizeof(s_wave_dvi_block_header), _file);

    predicted_value = header_block.isamp0;
    step_index = header_block.step_table_index;

    // Wait until the start sending command is received
    do
    {
      while(!rs232_read(&c, 1, &k));
      if (kbhit())
        end = 1;
    }while((k != 1 || c != 1) && !end);

    if (!end)
    {
      ++block_sent;
      k = (block_sent*block_size*32)/header_data.chunk_size;
      for(i=0; i<k; i++)
        _progress_bar[i] = '=';
      for(;i<32; i++)
        _progress_bar[i] = ' ';
      _progress_bar[i] = '\0';
      printf("\r[%32s] %i bytes", _progress_bar, block_sent*block_size);
      fflush(stdout);

      // send the initialization parameters
      // The predicted value (2 bytes)
      rs232_write(&((char *) &predicted_value)[1], 1, &k);
      rs232_write(&((char *) &predicted_value)[0], 1, &k);
      // The step index (2 bytes)
      rs232_write(&((char *) &step_index)[1], 1, &k);
      rs232_write(&((char *) &step_index)[0], 1, &k);

      // Send data
      for(i=0; i<nb_bytes_per_block; i++)
      {
        // Ignore the first channels
        for(k=0; k<(header_dvi.fmt.nb_channels-1); k++)
          fread(_buffer, 1, 4, _file);

        fread(_buffer, 1, 4, _file);
        rs232_write(&_buffer[0], 1, &k);
        rs232_write(&_buffer[1], 1, &k);
        rs232_write(&_buffer[2], 1, &k);
        rs232_write(&_buffer[3], 1, &k);
      }
    }
  }

  // Close the rs232 port
  rs232_close();
  fclose(_file);

  return 1;
}
