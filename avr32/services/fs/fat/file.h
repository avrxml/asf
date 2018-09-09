/*****************************************************************************
 *
 * \file
 *
 * \brief FAT 12/16/32 Services.
 *
 * This file defines a useful set of functions for the file accesses on
 * AVR32 devices.
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _FILE_H_
#define _FILE_H_

#include "fs_com.h"
#include "fat.h"

//_____ D E F I N I T I O N S ______________________________________________

//! \name options for file_seek() routine
//! @{
#define  FS_SEEK_SET       0x00  //!< start at the beginning
#define  FS_SEEK_END       0x01  //!< start at the end of file and rewind
#define  FS_SEEK_CUR_RE    0x02  //!< start at the current position and rewind
#define  FS_SEEK_CUR_FW    0x03  //!< start at the current position and forward
//! @}

//! \name Structure to define a physical memory segment
//! @{
typedef struct {
   uint8_t   u8_lun;            //!< logical unit(drive) number
   uint32_t  u32_addr;          //!< segment address (unit sector)
   uint16_t  u16_size;          //!< segment size (unit sector), or IN parameter for file_write() and file_read() routines
} Fs_file_segment;
//! @}


//_____ D E C L A R A T I O N S ____________________________________________

//! This function checks if a file is selected
//!
//! @return    true, a file is selected
//! @return    false, otherwise
//!
bool  file_ispresent( void );

//! This function opens the selected file
//!
//! @param     fopen_mode  option to open the file : <br>
//!                        FOPEN_MODE_R         R   access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_R_PLUS    R/W access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_W         W   access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_W_PLUS    R/W access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_APPEND    W   access, flux pointer = at the end, size not modify <br>
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  file_open( uint8_t fopen_mode );

//! This function returns a segment (position & size) in a physical memory corresponding at the file
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall contains maximum number of sector to read in file (0 = unlimited) <br>
//!                     ->u32_size_or_pos    OUT,  contain the segment size (unit sector) <br>
//!                     ->other              IN,   ignored <br>
//!                     ->other              OUT,  contains the segment position <br>
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This routine is interesting to read a file via a DMA and avoid the file system decode
//! because this routine returns a physical memory segment without File System information.
//! Note: the file can be fragmented and you must call file_read() for each fragments.
//! @endverbatim
//!
bool  file_read( Fs_file_segment _MEM_TYPE_SLOW_ *segment );

//! This function copies in a buffer the file data corresponding at the current position
//!
//! @param     buffer         buffer to fill
//! @param     u16_buf_size   buffer size
//!
//! @return    number of byte read
//! @return    0, in case of error
//!
uint16_t   file_read_buf( uint8_t _MEM_TYPE_SLOW_ *buffer , uint16_t u16_buf_size );

//! This function returns the next byte of file
//!
//! @return    The byte read
//! @return    EOF, in case of error or end of file
//!
uint16_t   file_getc( void );

//! This function allocs and returns a segment (position & size) in a physical memory corresponding at the file
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall contains the maximum number of sector to write in file <br>
//!                     ->u32_size_or_pos    OUT,  contains the segment size (unit sector) <br>
//!                     ->other              IN,   ignored <br>
//!                     ->other              OUT,  contains the segment position <br>
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This routine is interesting to write a file via a DMA and avoid the file system decode
//! because this routine returns a physical memory segment without File System information.
//! Note: the file can be fragmented and you must call file_write() for each fragments.
//! @endverbatim
//!
bool  file_write( Fs_file_segment _MEM_TYPE_SLOW_ *segment );

//! This function sets the end of file at the current position
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! This routine is usually used after the last file_write() call.
//! The file_write() routine uses the sector unit (512B),
//! and you can set a specific byte size with a file_seek() call and fiel_set_eof() call.
//! @endverbatim
//!
bool  file_set_eof( void );

//! This function transfer a buffer to a file at the current file position
//!
//! @param     buffer         data buffer
//! @param     u16_buf_size   data size
//!
//! @return    number of byte write
//! @return    0, in case of error
//!
uint16_t   file_write_buf( uint8_t _MEM_TYPE_SLOW_ *buffer , uint16_t u16_buf_size );

//! This function writes a byte in the file
//!
//! @param     u8_byte    byte to write
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  file_putc( uint8_t u8_byte );

//! This function returns the position in the file
//!
//! @return    Position in file
//!
uint32_t   file_getpos( void );

//! This function changes the position in the file
//!
//! @param     u32_pos     number of byte to seek
//! @param     u8_whence   direction of seek <br>
//!                        FS_SEEK_SET   , start at the beginning and forward <br>
//!                        FS_SEEK_END   , start at the end of file and rewind <br>
//!                        FS_SEEK_CUR_RE, start at the current position and rewind <br>
//!                        FS_SEEK_CUR_FW, start at the current position and forward <br>
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  file_seek( uint32_t u32_pos , uint8_t u8_whence );

//! This function checks the beginning of file
//!
//! @return    1     the position is at the beginning of file
//! @return    0     the position isn't at the beginning of file
//! @return    FFh   error
//!
uint8_t    file_bof( void );

//! This function checks the end of file
//!
//! @return    1     the position is at the end of file
//! @return    0     the position isn't at the end of file
//! @return    FFh   error
//!
uint8_t    file_eof( void );

//! This function flushes the internal cache (file data and file information)
//!
void  file_flush( void );

//! This function closes the file
//!
void  file_close( void );


#endif  // _FILE_H_
