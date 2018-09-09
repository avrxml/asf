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
