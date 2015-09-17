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
#include "rs232.h"
#include "error_management.h"

// COM port handle
static HANDLE handle_com = NULL;

// Delay
static COMMTIMEOUTS g_cto =
{
  RS232_MAX_WAIT_READ,
  0,
  RS232_MAX_WAIT_READ,
  0,
  0
};

// Configuration
static DCB g_dcb =
{
  sizeof(DCB),          // DCBlength
  9600,                 // BaudRate
  TRUE,                 // fBinary
  FALSE,                // fParity
  FALSE,                // fOutxCtsFlow
  FALSE,                // fOutxDsrFlow
  DTR_CONTROL_ENABLE,   // fDtrControl
  FALSE,                // fDsrSensitivity
  FALSE,                // fTXContinueOnXoff
  FALSE,                // fOutX
  FALSE,                // fInX
  FALSE,                // fErrorChar
  FALSE,                // fNull
  RTS_CONTROL_ENABLE,   // fRtsControl
  FALSE,                // fAbortOnError
  0,                    // fDummy2
  0,                    // wReserved
  0x100,                // XonLim
  0x100,                // XoffLim
  8,                    // ByteSize
  NOPARITY,             // Parity
  ONESTOPBIT,           // StopBits
  0x11,                 // XonChar
  0x13,                 // XoffChar
    '?',                  // ErrorChar
  0x1A,                 // EofChar
  0x10                // EvtChar
};

// Open the rs232 port
int rs232_open(char *_port, int baud_rate, int byte_size, int parity, int stop_bits)
{
    // Make sure another port is not already opened
    ASSERT(!handle_com);

    // Open the existing COMX file to open the port
  handle_com = CreateFile(
                      _port,
                      GENERIC_READ | GENERIC_WRITE,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_SYSTEM,
                      NULL);

    // Make sure it is opened
    if (handle_com == INVALID_HANDLE_VALUE)
       return 0;

    // buffer size for emission and reception
  SetupComm(handle_com, RS232_RX_SIZE, RS232_TX_SIZE);

    // COM port configuration
  g_dcb.BaudRate = baud_rate;
  g_dcb.ByteSize = byte_size;
  g_dcb.Parity = parity;
  g_dcb.StopBits = stop_bits;
  if(!SetCommTimeouts(handle_com, &g_cto) || !SetCommState(handle_com, &g_dcb))
    {
      CloseHandle(handle_com);
      return 0;
    }

    // Flush buffers for emission and reception
    // DTR = 1
  PurgeComm(handle_com, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
  EscapeCommFunction(handle_com, SETDTR);

  return 1;
}

// Close the previously opened rs232 port
int rs232_close()
{
  CloseHandle(handle_com);
  return 1;
}

// Read data from the rs232 port
int rs232_read(void *buffer, int size, int *_read_bytes)
{
  return ReadFile(handle_com, buffer, size, (DWORD *) _read_bytes, (LPOVERLAPPED) NULL);
}

// Write data through the rs232 port
int rs232_write(void* buffer, int size, int* _written_bytes)
{
  return WriteFile(handle_com, buffer, size, (DWORD *) _written_bytes, (LPOVERLAPPED) NULL);
}
