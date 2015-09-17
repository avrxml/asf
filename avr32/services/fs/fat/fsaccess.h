/**************************************************************************
 *
 * \file
 *
 * \brief Management of the file system access.
 *
 * This file provides a set of file access functions.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _FSACCESS_H_
#define _FSACCESS_H_


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>

#include "file.h"
#include "navigation.h"
#include "ctrl_access.h"

#ifndef	_FCNTL_
#define	O_RDONLY    0
#define	O_WRONLY    0x0001
#define	O_RDWR      0x0002
#define	O_APPEND    0x0008
#define	O_CREAT     0x0200
#endif

typedef signed int ssize_t;

//_____ D E C L A R A T I O N S ____________________________________________


//!
//! This function opens a file.
//!
//! @param pathname   path of the file to open.
//! @param flags      flags to give file access rights
//!                   should be O_CREAT  : create file if not exist
//!                             O_APPEND : add data to the end of file
//!                             O_RDONLY : Read Only
//!                             O_WRONLY : Write Only
//!                             O_RDWR   : Read/Write
//! @return int : file descriptor (>= 0 if OK (NavID), -1 otherwise)
//!
extern int open(const char *pathname, int flags, ...);

//!
//! This function returns the opened file size.
//!
//! @param fd   file descriptor.
//!
//! @return size_t : size of the file pointed to by the descriptor
//!
extern size_t fsaccess_file_get_size(int fd);

//!
//! This function reads from a file.
//!
//! @param fd    file descriptor.
//! @param buf   pointer for data that are read.
//! @param count amount of bytes to read
//!
//! @return ssize_t : amount of data read (-1 if error)
//!
extern ssize_t read(int fd, void *buf, size_t count);

//!
//! This function writes to a file.
//!
//! @param fd    file descriptor.
//! @param buf   pointer from where data are written.
//! @param count amount of bytes to write
//!
//! @return ssize_t : amount of data written (-1 if error)
//!
extern ssize_t write(int fd, const void *buf, size_t count);

//!
//! This function closes a file.
//!
//! @param fd    file descriptor.
//!
//! @return int : -1 if error, 0 otherwise
//!
extern int close(int fd);

//!
//! This function returns a free navigator id.
//!
//! @return long : Id navigator allocated or -1 if none free
//!
extern long fsaccess_alloc_nav_id( void );

//!
//! This function frees a navigator id.
//!
//! @param fd    file descriptor.
//!
extern void fsaccess_free_nav_id(int fd);

//!
//! This function initializes mutex and navigators.
//!
extern bool b_fsaccess_init(void);

//!
//! This function gives the mutex to the caller.
//!
extern void fsaccess_take_mutex(void);

//!
//! This function frees the mutex.
//!
extern void fsaccess_give_mutex(void);

/*!
 * \brief Is a directory present?
 *
 * \param pcStringDirName Input. Directory name string.
 *
 * \return 1 if the directory exists, 0 if the directory doesn't exist, else -1
 */
extern int8_t fsaccess_IsDirPresent( const char *pcStringDirName );


#endif  // _FSACCESS_H_
