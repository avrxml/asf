/**************************************************************************
 *
 * \file
 *
 * \brief Management of the file system access.
 *
 * This file provides a set of file access functions.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
