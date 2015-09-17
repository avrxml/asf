#include <stdio.h>
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#define HEADER_SIZE       0x36

#define RED               2
#define GREEN             1
#define BLUE              0

#define WIDTH             320
#define HEIGHT            240
#define FILENAME_IN       "AT32UC3C-EK.bmp"
#define FILENAME_OUT      "../background_image.c"
#define NAME              BACKGROUND_IMAGE
#define BRIEF             "Application background image."

#define ALIGN   ((-WIDTH * 3) & 3)
#define STRIDE  ((WIDTH * 2 + 3) & ~3)

#define STRINGZ(s)    #s
#define ASTRINGZ(s)   STRINGZ(s)

unsigned char buf[WIDTH * HEIGHT][3];

static inline unsigned int min(unsigned int a, unsigned int b)
{
  return (a < b) ? a : b;
}

int main(void)
{
  unsigned int r, c;

  FILE *Fin = fopen(FILENAME_IN, "rb");
  FILE *Fout = fopen(FILENAME_OUT, "w");

  fseek(Fin, HEADER_SIZE, SEEK_SET);
  for (r = 0; r < HEIGHT; r++)
  {
    fread(&buf[r * WIDTH], sizeof(buf[0]), WIDTH, Fin);
    fseek(Fin, ALIGN, SEEK_CUR);
  }

  fprintf(Fout, "/*! \\file *********************************************************************\r\n"
                " *\r\n"
                " * \\brief "BRIEF"\r\n"
                " *\r\n"
                " * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32\r\n"
                " * - Supported devices:  All AVR32 devices can be used.\r\n"
                " * - AppNote:\r\n"
                " *\r\n"
                " * \author               Atmel Corporation: http://www.atmel.com \\n\r\n"
                " *                       Support and FAQ: http://www.atmel.com/design-support/\r\n"
                " *\r\n"
                " ******************************************************************************/\r\n"
                "\r\n"
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
                "\r\n"
                "#ifndef _"ASTRINGZ(NAME)"_H_\r\n"
                "#define _"ASTRINGZ(NAME)"_H_\r\n"
                "\r\n"
                "\r\n"
                "const unsigned short int background_image[] =\r\n"
                "{\r\n"
                "  ");

  for (r = 0; r < HEIGHT; r++)
  {
    for (c = 0; c < STRIDE; )
    {
      if (c < WIDTH * 2)
      {
        unsigned short pixel = (min(buf[(HEIGHT - r - 1) * WIDTH + c / 2][RED  ] + 0x04, 0xFF) >> (8 - 5)) << (6 + 5) |
                               (min(buf[(HEIGHT - r - 1) * WIDTH + c / 2][GREEN] + 0x02, 0xFF) >> (8 - 6)) << 5 |
                               (min(buf[(HEIGHT - r - 1) * WIDTH + c / 2][BLUE ] + 0x04, 0xFF) >> (8 - 5));
        fprintf(Fout, "0x%.2hhX%.2hhX, ", (unsigned char)(pixel >> 8), (unsigned char)pixel);
        c += 2;
      }
      else
      {
        fputs("0x0000, ", Fout);
        c++;
      }
      if (!((r * STRIDE + c) % 16) && (r != HEIGHT - 1 || c != STRIDE)) fputs("\r\n  ", Fout);
    }
  }

  fputs("\r\n"
        "};\r\n"
        "\r\n"
        "#endif\r\n"
        "\r\n", Fout);

  fclose(Fout);
  fclose(Fin);
}
