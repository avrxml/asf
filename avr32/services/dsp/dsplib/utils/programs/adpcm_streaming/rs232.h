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
#ifndef __RS232_H
#define __RS232_H

#include <windows.h>

#define RS232_RX_SIZE          4096    // buffer size for reception
#define RS232_TX_SIZE          4096    // buffer size for emission
#define RS232_MAX_WAIT_READ    1000    // Timeout (in ms)

#define RS232_BAUD_RATE_110    CBR_110
#define RS232_BAUD_RATE_300    CBR_300
#define RS232_BAUD_RATE_600    CBR_600
#define RS232_BAUD_RATE_1200   CBR_1200
#define RS232_BAUD_RATE_2400   CBR_2400
#define RS232_BAUD_RATE_4800   CBR_4800
#define RS232_BAUD_RATE_9600   CBR_9600
#define RS232_BAUD_RATE_14400  CBR_14400
#define RS232_BAUD_RATE_19200  CBR_19200
#define RS232_BAUD_RATE_38400  CBR_38400
#define RS232_BAUD_RATE_56000  CBR_56000
#define RS232_BAUD_RATE_57600  CBR_57600
#define RS232_BAUD_RATE_115200 CBR_115200
#define RS232_BAUD_RATE_128000 CBR_128000
#define RS232_BAUD_RATE_256000 CBR_256000

#define RS232_PARITY_EVEN      EVENPARITY
#define RS232_PARITY_MARK      MARKPARITY
#define RS232_PARITY_NOPARITY  NOPARITY
#define RS232_PARITY_ODD       ODDPARITY
#define RS232_PARITY_SPACE     SPACEPARITY

#define RS232_STOP_BIT_ONE     ONESTOPBIT      // 1 stop bit
#define RS232_STOP_BIT_ONE5    ONE5STOPBITS    // 1.5 stop bits
#define RS232_STOP_BIT_TWO     TWOSTOPBITS     // 2 stop bits

// Open the rs232 port
int rs232_open(char *_port, int baud_rate, int byte_size, int parity, int stop_bits);
// Close the previously opened rs232 port
int rs232_close();
// Read data from the rs232 port
int rs232_read(void *buffer, int size, int *_read_bytes);
// Write data through the rs232 port
int rs232_write(void* buffer, int size, int* _written_bytes);

#endif
