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
