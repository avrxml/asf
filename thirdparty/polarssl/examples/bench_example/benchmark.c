/*
 *  Benchmark demonstration program
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include "gpio.h"
#include "intc.h"
#include "stdio_serial.h"
#include "conf_stdio_serial.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cycle_counter.h"
#include "polarssl/config.h"

#include "polarssl/md4.h"
#include "polarssl/md5.h"
#include "polarssl/sha1.h"
#include "polarssl/sha2.h"
#include "polarssl/sha4.h"
#include "polarssl/arc4.h"
#include "polarssl/des.h"
#include "polarssl/aes.h"
#include "polarssl/camellia.h"
#include "polarssl/rsa.h"
#include "polarssl/timing.h"

#define BUFSIZE 1024

static int myrand( void *rng_state )
{
    if( rng_state != NULL )
        rng_state  = NULL;

    return( rand() );
}

unsigned char buf[BUFSIZE];

int main( void )
{
    int keysize;
    unsigned long i, j, tsc;
    unsigned char tmp[64];
    t_cpu_time timer;

    /* Keep compiler happy */
    UNUSED(keysize);
    UNUSED(i);
    UNUSED(j);
    UNUSED(tsc);
    UNUSED(tmp[0]);
    UNUSED(timer);


    // USART options.
    static usart_serial_options_t USART_SERIAL_OPTIONS =
    {
            .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
            .charlength   = USART_SERIAL_CHAR_LENGTH,
            .paritytype   = USART_SERIAL_PARITY,
            .stopbits     = USART_SERIAL_STOP_BIT
    };

    sysclk_init();

    // Initialize the board.
    // The board-specific conf_board.h file contains the configuration of the board
    // initialization.
    board_init();

    // Initialize Serial Interface using Stdio Library
    stdio_serial_init(USART_SERIAL_EXAMPLE,&USART_SERIAL_OPTIONS);

    printf( "Start Benchmark\n");

#if defined(POLARSSL_ARC4_C)
    arc4_context arc4;
#endif
#if defined(POLARSSL_DES_C)
    des3_context des3;
    des_context des;
#endif
#if defined(POLARSSL_AES_C)
    aes_context aes;
#endif
#if defined(POLARSSL_CAMELLIA_C)
    camellia_context camellia;
#endif
#if defined(POLARSSL_RSA_C)
    rsa_context rsa;
#endif

    memset( buf, 0xAA, sizeof( buf ) );

    printf( "\n" );

#if defined(POLARSSL_MD4_C)
    printf( "  MD4       :  " );
    fflush( stdout );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        md4( buf, BUFSIZE, tmp );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        md4( buf, BUFSIZE, tmp );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_MD5_C)
    printf( "  MD5       :  " );
    fflush( stdout );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        md5( buf, BUFSIZE, tmp );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        md5( buf, BUFSIZE, tmp );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_SHA1_C)
    printf( "  SHA-1     :  " );
    fflush( stdout );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        sha1( buf, BUFSIZE, tmp );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        sha1( buf, BUFSIZE, tmp );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_SHA2_C)
    printf( "  SHA-256   :  " );
    fflush( stdout );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        sha2( buf, BUFSIZE, tmp, 0 );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        sha2( buf, BUFSIZE, tmp, 0 );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_SHA4_C)
    printf( "  SHA-512   :  " );
    fflush( stdout );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        sha4( buf, BUFSIZE, tmp, 0 );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        sha4( buf, BUFSIZE, tmp, 0 );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_ARC4_C)
    printf( "  ARC4      :  " );
    fflush( stdout );

    arc4_setup( &arc4, tmp, 32 );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        arc4_crypt( &arc4, BUFSIZE, buf, buf );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        arc4_crypt( &arc4, BUFSIZE, buf, buf );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_DES_C)
    printf( "  3DES      :  " );
    fflush( stdout );

    des3_set3key_enc( &des3, tmp );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        des3_crypt_cbc( &des3, DES_ENCRYPT, BUFSIZE, tmp, buf, buf );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        des3_crypt_cbc( &des3, DES_ENCRYPT, BUFSIZE, tmp, buf, buf );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );

    printf( "  DES       :  " );
    fflush( stdout );

    des_setkey_enc( &des, tmp );

    cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);
    for( i = 1; !cpu_is_timeout(&timer); i++ )
        des_crypt_cbc( &des, DES_ENCRYPT, BUFSIZE, tmp, buf, buf );

    tsc = hardclock();
    for( j = 0; j < 1024; j++ )
        des_crypt_cbc( &des, DES_ENCRYPT, BUFSIZE, tmp, buf, buf );

    printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                    ( hardclock() - tsc ) / ( j * BUFSIZE ) );
#endif

#if defined(POLARSSL_AES_C)
    for( keysize = 128; keysize <= 256; keysize += 64 )
    {
        printf( "  AES-%d   :  ", keysize );
        fflush( stdout );

        memset( buf, 0, sizeof( buf ) );
        memset( tmp, 0, sizeof( tmp ) );
        aes_setkey_enc( &aes, tmp, keysize );

        cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);

        for( i = 1; !cpu_is_timeout(&timer); i++ )
            aes_crypt_cbc( &aes, AES_ENCRYPT, BUFSIZE, tmp, buf, buf );

        tsc = hardclock();
        for( j = 0; j < 4096; j++ )
            aes_crypt_cbc( &aes, AES_ENCRYPT, BUFSIZE, tmp, buf, buf );

        printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                        ( hardclock() - tsc ) / ( j * BUFSIZE ) );
    }
#endif

#if defined(POLARSSL_CAMELLIA_C)
    for( keysize = 128; keysize <= 256; keysize += 64 )
    {
        printf( "  CAMELLIA-%d   :  ", keysize );
        fflush( stdout );

        memset( buf, 0, sizeof( buf ) );
        memset( tmp, 0, sizeof( tmp ) );
        camellia_setkey_enc( &camellia, tmp, keysize );

        cpu_set_timeout(cpu_ms_2_cy(1000, CPU_HZ),&timer);

        for( i = 1; !cpu_is_timeout(&timer); i++ )
            camellia_crypt_cbc( &camellia, CAMELLIA_ENCRYPT, BUFSIZE, tmp, buf, buf );

        tsc = hardclock();
        for( j = 0; j < 4096; j++ )
            camellia_crypt_cbc( &camellia, CAMELLIA_ENCRYPT, BUFSIZE, tmp, buf, buf );

        printf( "%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
                        ( hardclock() - tsc ) / ( j * BUFSIZE ) );
    }
#endif

#if defined(POLARSSL_RSA_C)
    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    rsa_gen_key( &rsa, myrand, NULL, 1024, 65537 );

    printf( "  RSA-1024  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; !cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_public( &rsa, buf, buf );
    }

    printf( "%9lu  public/s\n", i / 3 );

    printf( "  RSA-1024  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; !cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_private( &rsa, buf, buf );
    }

    printf( "%9lu private/s\n", i / 3 );

    rsa_free( &rsa );

    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    rsa_gen_key( &rsa, myrand, NULL, 2048, 65537 );

    printf( "  RSA-2048  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; !cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_public( &rsa, buf, buf );
    }

    printf( "%9lu  public/s\n", i / 3 );

    printf( "  RSA-2048  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; ! cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_private( &rsa, buf, buf );
    }

    printf( "%9lu private/s\n", i / 3 );

    rsa_free( &rsa );

    rsa_init( &rsa, RSA_PKCS_V15, 0 );
    rsa_gen_key( &rsa, myrand, NULL, 4096, 65537 );

    printf( "  RSA-4096  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; !cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_public( &rsa, buf, buf );
    }

    printf( "%9lu  public/s\n", i / 3 );

    printf( "  RSA-4096  :  " );
    fflush( stdout );
    cpu_set_timeout(cpu_ms_2_cy(3000, CPU_HZ),&timer);

    for( i = 1; ! cpu_is_timeout(&timer); i++ )
    {
        buf[0] = 0;
        rsa_private( &rsa, buf, buf );
    }

    printf( "%9lu private/s\n", i / 3 );

    rsa_free( &rsa );
#endif

    printf( "\n" );

#ifdef WIN32
    printf( "  Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    return( 0 );
}
