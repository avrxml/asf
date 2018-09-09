/*****************************************************************************
 *
 * \file
 *
 * \brief Basic TFTP Server for Atmel MCUs.
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
 *****************************************************************************/


#ifndef BASIC_TFTP_SERVER_H
#define BASIC_TFTP_SERVER_H

#include "portmacro.h"

/* tftp_support.h */

/*
 * File transfer modes
 */
#define TFTP_NETASCII   0              // Text files
#define TFTP_OCTET      1              // Binary files

/*
 * Errors
 */

// These initial 7 are passed across the net in "ERROR" packets.
#define TFTP_ENOTFOUND   1   /* file not found */
#define TFTP_EACCESS     2   /* access violation */
#define TFTP_ENOSPACE    3   /* disk full or allocation exceeded */
#define TFTP_EBADOP      4   /* illegal TFTP operation */
#define TFTP_EBADID      5   /* unknown transfer ID */
#define TFTP_EEXISTS     6   /* file already exists */
#define TFTP_ENOUSER     7   /* no such user */
// These extensions are return codes in our API, *never* passed on the net.
#define TFTP_TIMEOUT     8   /* operation timed out */
#define TFTP_NETERR      9   /* some sort of network error */
#define TFTP_INVALID    10   /* invalid parameter */
#define TFTP_PROTOCOL   11   /* protocol violation */
#define TFTP_TOOLARGE   12   /* file is larger than buffer */

#define TFTP_TIMEOUT_PERIOD  5          // Seconds between retries
#define TFTP_TIMEOUT_MAX    50          // Max timeouts over all blocks
#define TFTP_RETRIES_MAX     5          // retries per block before giving up

/* netdb.h */
// Internet services
struct servent {
char *s_name; /* official service name */
char **s_aliases; /* alias list */
int s_port; /* port number */
char *s_proto; /* protocol to use */
};

/* arpa/tftp.h */

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define SEGSIZE   512   /* data segment size */

/*
 * Packet types.
 */

#define th_block  th_u.tu_block
#define th_code   th_u.tu_code
#define th_stuff  th_u.tu_stuff
#define th_msg    th_data

/*
 * Error codes.
 */
#define EUNDEF    0   /* not defined */
#define ENOTFOUND 1   /* file not found */
#define EACCESS   2   /* access violation */
#define ENOSPACE  3   /* disk full or allocation exceeded */
#define EBADOP    4   /* illegal TFTP operation */
#define EBADID    5   /* unknown transfer ID */
#define EEXISTS   6   /* file already exists */
#define ENOUSER   7   /* no such user */



#define RRQ 01      /* read request */
#define WRQ 02      /* write request */
#define DATA  03      /* data packet */
#define ACK 04      /* acknowledgement */
#define ERROR 05      /* error code */

#if (__ICCAVR32__ || __ICCARM__)
#pragma pack(1)
#endif
struct  tftphdr {
  short th_opcode;    /* packet type */
  union {
    unsigned short  tu_block; /* block # */
    short tu_code;  /* error code */
    char  tu_stuff[1];  /* request packet stuff */
  }
#if __GNUC__
 __attribute__ ((packed))
#endif
   th_u;
  char  th_data[1];   /* data or error string */
}
#if __GNUC__
__attribute__ ((packed))
#endif
;
#if (__ICCAVR32__ || __ICCARM__)
#pragma pack()
#endif

/* The function that implements the TFTP server task. */
portTASK_FUNCTION_PROTO( vBasicTFTPServer, pvParameters );



#endif

