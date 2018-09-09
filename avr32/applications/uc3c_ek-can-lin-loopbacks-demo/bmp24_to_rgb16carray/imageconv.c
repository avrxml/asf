#include <stdio.h>
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
                " * \author               Microchip Technology Inc: http://www.microchip.com \\n\r\n"
                " *                       Support and FAQ: https://www.microchip.com/support/\r\n"
                " *\r\n"
                " ******************************************************************************/\r\n"
                "\r\n"
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
