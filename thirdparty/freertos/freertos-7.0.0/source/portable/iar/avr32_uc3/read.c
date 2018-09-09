/*****************************************************************************
 *
 * \file
 *
 * \brief System-specific implementation of the \ref __read function used by
          the standard library.
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
 ******************************************************************************/


#include <yfuns.h>
#include <avr32/io.h>
#include <usart.h>


_STD_BEGIN


#pragma module_name = "?__read"


extern volatile avr32_usart_t *volatile stdio_usart_base;


/*! \brief Reads a number of bytes, at most \a size, into the memory area
 *         pointed to by \a buffer.
 *
 * \param handle File handle to read from.
 * \param buffer Pointer to buffer to write read bytes to.
 * \param size Number of bytes to read.
 *
 * \return The number of bytes read, \c 0 at the end of the file, or
 *         \c _LLIO_ERROR on failure.
 */
size_t __read(int handle, unsigned char *buffer, size_t size)
{
  int nChars = 0;

  // This implementation only reads from stdin.
  // For all other file handles, it returns failure.
  if (handle != _LLIO_STDIN)
  {
    return _LLIO_ERROR;
  }

  for (; size > 0; --size)
  {
    int c = usart_getchar(stdio_usart_base);
    if (c < 0)
      break;

    *buffer++ = c;
    ++nChars;
  }

  return nChars;
}


_STD_END
