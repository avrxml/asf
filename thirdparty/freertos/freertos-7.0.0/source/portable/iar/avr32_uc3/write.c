/*****************************************************************************
 *
 * \file
 *
 * \brief System-specific implementation of the \ref __write function used by
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


#pragma module_name = "?__write"


//! Pointer to the base of the USART module instance to use for stdio.
__no_init volatile avr32_usart_t *volatile stdio_usart_base;


/*! \brief Writes a number of bytes, at most \a size, from the memory area
 *         pointed to by \a buffer.
 *
 * If \a buffer is zero then \ref __write performs flushing of internal buffers,
 * if any. In this case, \a handle can be \c -1 to indicate that all handles
 * should be flushed.
 *
 * \param handle File handle to write to.
 * \param buffer Pointer to buffer to read bytes to write from.
 * \param size Number of bytes to write.
 *
 * \return The number of bytes written, or \c _LLIO_ERROR on failure.
 */
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
  size_t nChars = 0;

  if (buffer == 0)
  {
    // This means that we should flush internal buffers.
    return 0;
  }

  // This implementation only writes to stdout and stderr.
  // For all other file handles, it returns failure.
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for (; size != 0; --size)
  {
    if (usart_putchar(stdio_usart_base, *buffer++) < 0)
    {
      return _LLIO_ERROR;
    }

    ++nChars;
  }

  return nChars;
}


_STD_END
