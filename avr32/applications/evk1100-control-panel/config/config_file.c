/**************************************************************************
 *
 * \file
 *
 * \brief Management of the config file access.
 *
 * This file manages the accesses to the config files.
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

/* Standard includes. */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fsaccess.h"
#include "config_file.h"
#include "cptime.h"
#include "shell.h"

/* Scheduler includes. */
#ifdef FREERTOS_USED
  #include "FreeRTOS.h"
  #include "semphr.h"
  #include "task.h"
  #include "portmacro.h"
  #include "tracedump.h"
#endif

/* Demo includes. */
/* Demo app includes. */
#include "compiler.h"


//!
//! @brief This function replace a (or many) line(s) in a file.
//!        Syntax of the line should be : \verbatim field=value\r\n \endverbatim
//!
//! @param filename Input. The filename to write into.
//! @param ac       Input. The argument counter. For this command, should be 1.
//! @param av       Input. The argument vector.
//!
//! @return short : 0 if success or -1 else
//!
signed short config_file_set_value(char * filename, int ac, signed char *av[])
{
int size, i;
char * pcDataToWrite = NULL;
char * pcDataTemp = NULL;
char * pcBegin = NULL;
char * pcEnd = NULL;
char * pcEq = NULL;
signed short xreturn = -1;
size_t  FileLen;
int fd;
char   pcTempoDate[17];

#ifdef FREERTOS_USED
  pcDataToWrite = pvPortMalloc(MAX_CONFIG_FILE_SIZE);
#else
  pcDataToWrite = malloc(MAX_CONFIG_FILE_SIZE);
#endif
  if (pcDataToWrite != NULL)
  {
    if ((fd = open(filename, (O_RDONLY))) >= 0)
    {
      if ((size = fsaccess_file_get_size(fd)) > 0)
      {
        /* get data from file */
        if (read(fd, pcDataToWrite, size) > 0)
        {
          pcDataToWrite[size] = '\0';
          for (i = 0 ; i < ac ; i+=2)
          {
            /* search the field in the file */
            pcBegin = strstr(pcDataToWrite, (char *)av[i]);
            if( NULL == pcBegin )
            {  // The field is not yet in the file.
              strcat( pcDataToWrite, CRLF );  // Add a new line for this field.
              strcat( pcDataToWrite, (char *)av[i] ); // Add the field.
              strcat( pcDataToWrite, "=" );   // Add the =
              strcat( pcDataToWrite, (char *)av[i+1] );  // Add the value.
              strcat( pcDataToWrite, CRLF );  // Add the CRLF.
            }
            else
            {
              /* Search for the = sign. */
              pcEq = strpbrk(pcBegin, "=");
              /* search for end of line */
              pcEnd = strpbrk(pcBegin, "\n");
              if( NULL == pcEnd )
              { // no crlf, we're at the end of the file.
                pcEq++; // WARNING NOTE: we assume the cfg file is well-formed,
                        // i.e. there must be an equal sign after the field name then a value.
                strcpy( pcEq, (char *)av[i+1] );  // Add the value.
              }
              else
              {
                pcEnd++;
                if( ( pcEnd - pcDataToWrite ) < size )
                { // There are other fields after this one.
#ifdef FREERTOS_USED
                  if ((pcDataTemp = pvPortMalloc(MAX_CONFIG_FILE_SIZE)) != NULL)
#else
                  if ((pcDataTemp = malloc(MAX_CONFIG_FILE_SIZE)) != NULL)
#endif
                  {
                    strcpy( pcDataTemp, pcEnd );
                  }
                }
                pcEq++; // WARNING NOTE: we assume the cfg file is well-formed,
                        // i.e. there must be an equal sign after the field name then a value.
                strcpy( pcEq, (char *)av[i+1] );  // Add the value.
                strcat( pcEq, CRLF );
                if (pcDataTemp != NULL)
                {
                  /* add old data to the buffer */
                  strcat( pcEq, pcDataTemp );
#ifdef FREERTOS_USED
                  vPortFree(pcDataTemp);
#else
                  free(pcDataTemp);
#endif
                }
              }
            }
          }// for number of args
        }//read succeeds
      }// size > 0
      close(fd);
    }// open succeeds
    if ((fd = open(filename, O_WRONLY)) >= 0)
    {
      FileLen = strlen( pcDataToWrite );
      if( write( fd, pcDataToWrite, FileLen ) == FileLen )
      {
        xreturn = 0;
      }
      // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
      v_cptime_GetDateInFatStringFormat( pcTempoDate );
      // Set the file date.
      nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );

      close(fd);
    }
#ifdef FREERTOS_USED
    vPortFree(pcDataToWrite);
#else
    free(pcDataToWrite);
#endif
  }// malloc succeeds

  return( xreturn );
}


//!
//! @brief get module config, by parsing config file.
//!
//! @param filename     Input. file to read into.
//! @param pcField      Input. field to find in file.
//! @param pcValue      Output. value read for this field, as string.
//!
//! @return short       0 if success, -1 otherwise.
//!
signed short config_file_get_value(char * filename, char * pcField, char *pcValue)
{
int ErrorConfig = -1;
int size;
portCHAR * pcDataRead = NULL;
portCHAR * current;
portCHAR * end;
int fd;

  if ((fd = open(filename, O_RDONLY)) >= 0)
  {
    /* get the file size */
    size = fsaccess_file_get_size(fd);
    /* if size is consistent */
    if ((size > 0) && (size <= MAX_CONFIG_FILE_SIZE))
    {
#ifdef FREERTOS_USED
      if ((pcDataRead = pvPortMalloc(size+1)) != NULL)
#else
      if ((pcDataRead = malloc(size+1)) != NULL)
#endif
      {
        if (read(fd,pcDataRead,size) == size)
        {
          pcDataRead[size] = '\0';
          // Look for the requested field.
          current = strstr(pcDataRead, pcField );
          if (current != NULL)
          {
            /* goto = */
            current = strpbrk(current, "=" );
            if( NULL != current )
            {   // The equal sign was found
              current++; // Goto just after the = sign.
              // Look for the end of line
              // (Well-formed config file rule is : one param=value pair per line)
              end = strpbrk(current, "\r\n" );
              if( NULL != end )
                *end = '\0';
              // Else the param is the last one in the config file and we'll use
              // the '\0' of pcDataRead).
              strcpy(pcValue, current);
              ErrorConfig = 0;
            }
            // else '=' sign not found => the config file is not well-formed.
            // (Well-formed config file rule is, per line: param=value).
          }
        }// read succeeds
        /* free buffer */
#ifdef FREERTOS_USED
        vPortFree(pcDataRead);
#else
        free(pcDataRead);
#endif
      }// alloc succeeds
    }// size is OK
    close(fd);
  }// open OK
  return (ErrorConfig);
}

