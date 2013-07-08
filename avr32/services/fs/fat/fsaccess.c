/**************************************************************************
 *
 * \file
 *
 * \brief Management of the file system access.
 *
 * This file provides a set of file access functions.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


//_____  I N C L U D E S ___________________________________________________


/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fsaccess.h"

/* Scheduler includes. */
#ifdef FREERTOS_USED
  #include "FreeRTOS.h"
  #include "semphr.h"
  #include "task.h"
  #include "portmacro.h"
#endif

/* Demo includes. */
/* Demo app includes. */
#include "compiler.h"


/* FAT includes */
#include "file.h"
#include "navigation.h"
#include "ctrl_access.h"

//_____ M A C R O S ________________________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

//! bitfield for navigator currently used. bit[0..31] is 1 if used, 0 if not.
static unsigned int pvNavUsed = 0;

//! Mutex to access the File System.
#ifdef FREERTOS_USED
static xSemaphoreHandle xFs_Access;
#endif


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
int open(const char *pathname, int flags, ...)
{
int CurrentNavId = -1;


  // take the mutex for nav access
  fsaccess_take_mutex();

  // if no navigator available, return an error
  if ((CurrentNavId = fsaccess_alloc_nav_id()) < 0) goto error_exit;

  // select the navigator
  nav_select( CurrentNavId );

  // the filesystem is now at the root directory
  if ((flags & O_CREAT) == O_CREAT)
  {
    // try to create, the flag is set
    if(nav_setcwd((FS_STRING)pathname, false, true) == false)
    {
       goto error_free_nav;
    }
  }
  else
  {
    // do not try to create, if it doesn't exist, error
    if(nav_setcwd((FS_STRING)pathname, false, false) == false)
    {
       goto error_free_nav;
    }
  }

  // if user wants to append in file
  if ((flags & O_APPEND) == O_APPEND)
  {
    // open the file
    if (file_open(FOPEN_MODE_APPEND) == false) goto error_free_nav;
  }
  else if ((flags & O_RDWR) == O_RDWR)
  {
    // open as read/write
    if (file_open(FOPEN_MODE_R_PLUS) == false) goto error_free_nav;
  }
  else if ((flags & O_WRONLY) == O_WRONLY)
  {
    // open as write
    if (file_open(FOPEN_MODE_W) == false) goto error_free_nav;
  }
  else
  {
    // open as read
    if (file_open(FOPEN_MODE_R) == false) goto error_free_nav;
  }

  // give the mutex for nav access
  fsaccess_give_mutex();

  return (CurrentNavId);

error_free_nav:
  // mark NavId as free
  fsaccess_free_nav_id(CurrentNavId);
error_exit:
  // give the mutex for nav access
  fsaccess_give_mutex();
  return(-1);
}

//!
//! This function returns the opened file size.
//!
//! @param fd   file descriptor.
//!
//! @return size_t : size of the file pointed to by the descriptor
//!
size_t fsaccess_file_get_size(int fd)
{
size_t size;

  if (fd < 0)
  {
    return (0);
  }
  // take the mutex for nav access
  fsaccess_take_mutex();
  // select the navigator
  nav_select( fd );
  // get the file size
  size = nav_file_lgt();
  // give the mutex for nav access
  fsaccess_give_mutex();
  return (size);
}

//!
//! This function reads from a file.
//!
//! @param fd    file descriptor.
//! @param buf   pointer for data that are read.
//! @param count amount of bytes to read
//!
//! @return ssize_t : amount of data read (-1 if error)
//!
ssize_t read(int fd, void *buf, size_t count)
{
ssize_t ReadCount = -1;

  if (fd < 0)
  {
    return (-1);
  }
  // take the mutex for nav access
  fsaccess_take_mutex();

  nav_select( fd );

  if ( file_eof() )
  {
     // give the mutex for nav access
     fsaccess_give_mutex();
     return(-1);
  }

  ReadCount = (ssize_t)file_read_buf((uint8_t *)buf , (uint16_t)count);

  // give the mutex for nav access
  fsaccess_give_mutex();
  return(ReadCount);
}


//!
//! This function writes to a file.
//!
//! @param fd    file descriptor.
//! @param buf   pointer from where data are written.
//! @param count amount of bytes to write
//!
//! @return ssize_t : amount of data written (-1 if error)
//!
ssize_t write(int fd, const void *buf, size_t count)
{
ssize_t WriteCount = -1;

  if (fd < 0)
  {
    return (-1);
  }
  // take the mutex for nav access
  fsaccess_take_mutex();

  nav_select( fd );

  WriteCount = (ssize_t)file_write_buf((uint8_t *)buf , (uint16_t)count);

  // give the mutex for nav access
  fsaccess_give_mutex();
  return(WriteCount);
}

//!
//! This function closes a file.
//!
//! @param fd    file descriptor.
//!
//! @return int : -1 if error, 0 otherwise
//!
int close(int fd)
{
  if (fd < 0)
  {
    return (-1);
  }
  // take the mutex for nav access
  fsaccess_take_mutex();

  nav_select( fd );

  file_close();


  fsaccess_free_nav_id(fd);

  // give the mutex for nav access
  fsaccess_give_mutex();

  return (0);
}

//!
//! This function returns a free navigator id.
//!
//! @return long : Id navigator allocated or -1 if none free
//!
long fsaccess_alloc_nav_id( void )
{
unsigned int j;
  // get a free nav id
  // NOTE: we start at FS_NB_RESERVED_NAV, because 2 is usually used as
  // the navigator for file copy. 0 & 1 may be used independently of the fsaccess module.
  for (j = FS_NB_RESERVED_NAV ; j < FS_NB_NAVIGATOR ; j++)
  {
    if (!Tst_bits(pvNavUsed, (1 << j)))
    {
      Set_bits(pvNavUsed, (1 << j));
      return (j);
    }
  }
  // no nav free for now
  return(-1);
}


//!
//! This function frees a navigator id.
//!
//! @param fd    file descriptor.
//!
void fsaccess_free_nav_id(int fd)
{
  // mark NavId as free
  Clr_bits(pvNavUsed, (1 << fd));
}

//!
//! This function initializes mutex and navigators.
//!
bool b_fsaccess_init(void)
{
  nav_reset();
#ifdef FREERTOS_USED
  if (xFs_Access == NULL)
  {
    vSemaphoreCreateBinary( xFs_Access );
    if( xFs_Access == NULL )
      return( false );
    else
      return( true );
  }
#endif
  return( true );
}


//!
//! This function gives the mutex to the caller.
//!
void fsaccess_take_mutex(void)
{
#ifdef FREERTOS_USED
  // wait for semaphore
  while( xSemaphoreTake( xFs_Access, portMAX_DELAY ) != pdTRUE );
#endif
}

//!
//! This function frees the mutex.
//!
void fsaccess_give_mutex(void)
{
#ifdef FREERTOS_USED
  // Release the mutex.
  xSemaphoreGive( xFs_Access );
#endif
}


/*!
 * \brief Is a directory present?
 *
 * \param pcStringDirName Input. Directory name string.
 *
 * \return 1 if the directory exists, 0 if the directory doesn't exist, else -1
 */
int8_t fsaccess_IsDirPresent( const char *pcStringDirName )
{
   signed short TempoNavId;
   int8_t           RetVal;


   fsaccess_take_mutex(); // Take the mutex on the file system access.

   TempoNavId = fsaccess_alloc_nav_id(); // Get a navigator.
   if( -1 == TempoNavId ) // No navigator.
   {
      // give the mutex for nav access
      fsaccess_give_mutex();
      return( -1 );
   }

   // select the navigator
   nav_select( TempoNavId );

   // Try to enter in the directory.
   RetVal = (int8_t)nav_setcwd( (FS_STRING)pcStringDirName, true, false );

   fsaccess_free_nav_id( TempoNavId ); // mark NavId as free
   fsaccess_give_mutex(); // give the mutex for nav access

   return( RetVal );
}
