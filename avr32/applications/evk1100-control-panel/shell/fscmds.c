/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel file system shell commands module.
 *
 * This module provides a shell commands interface to the file system.
 *
 *
 *                       Written for UC3 and EVK1100.
 *
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
 *****************************************************************************/


/*
 * Brief description of the module.
 * This module provides a set of shell commands to interface with the file system.
 *
 * Detailed description of the module.
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <string.h>
#include <math.h>
#include "compiler.h"
#include "board.h"

#include "shell.h"
#include "sensor.h"
#include "actuator.h"
#include "fsaccess.h"
#include "fscmds.h"
#include "supervisor.h"
#include "com1shell.h"
#include "tracedump.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


/*!
 * String messages.
 */
const signed portCHAR *const FSCMDS_MSG_APPEND_WELCOME  = (signed portCHAR *)CRLF"Simple text editor, enter char to append, ^q to exit and save"CRLF;
const signed portCHAR *const FSCMDS_MSG_HELP            = (signed portCHAR *)"\
"CRLF"disk: get the number of drives"CRLF"\
a:, b:, etc.: go to selected drive"CRLF"\
mount drivename (a, b, etc.):  go to selected drive"CRLF"\
format drivename (a, b, etc.): format selected drive"CRLF"\
fat: get FAT type used by current drive"CRLF"\
df: get free space information for all connected drives"CRLF"\
cd dirname: go to selected directory"CRLF"\
cd ..: go to upper directory"CRLF"\
mark: bookmark current directory"CRLF"\
goto: go to bookmarked directory"CRLF"\
ls: list files and directories in current directory"CRLF"\
pwd: get current path"CRLF"\
rm filename: remove selected file or empty directory"CRLF"\
rm -R foldername: remove selected directory and its content"CRLF"\
rm *: remove all files or empty directories in current directory"CRLF"\
cp filename: copy filename to bookmarked directory"CRLF"\
mv src dst: rename selected file or directory"CRLF"\
mkdir dirname: make directory"CRLF"\
touch filename: create file"CRLF"\
append filename: append to selected file from terminal input"CRLF"\
cat filename: list file contents"CRLF;


/*!
 *  String error messages.
 */
/*! Error msg upon mount syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MOUNT_SYNTAXERROR      = (signed portCHAR *)ERROR_CRLF"Usage: mount {a,b,c,d}"CRLF;

/*! Error msg upon {a:,b:,c:,d:} syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MOUNTSHORT_SYNTAXERROR = (signed portCHAR *)ERROR_CRLF"Usage: {a:,b:,c:,d:}"CRLF;

/*! Error msg upon mount of an unknown drive. */
const signed portCHAR *const FSCMDS_ERRMSG_MOUNT_DRIVENOTFOUND    = (signed portCHAR *)ERROR_CRLF"Drive not found."CRLF;

/*! Error msg upon mount failure. */
const signed portCHAR *const FSCMDS_ERRMSG_MOUNT_FAILED           = (signed portCHAR *)ERROR_CRLF"Unable to mount drive"CRLF;

/*! Error msg upon hardware error. */
const signed portCHAR *const FSCMDS_ERRMSG_HW                     = (signed portCHAR *)ERROR_CRLF"Hardware error"CRLF;

/*! Error msg upon file system error. */
const signed portCHAR *const FSCMDS_ERRMSG_FS                     = (signed portCHAR *)ERROR_CRLF"File system error"CRLF;

/*! Error msg upon unformatted drive error. */
const signed portCHAR *const FSCMDS_ERRMSG_NOFORMAT               = (signed portCHAR *)ERROR_CRLF"Current drive is not formatted"CRLF;

/*! Error msg upon memory device miss error. */
const signed portCHAR *const FSCMDS_ERRMSG_MEMDEVMISSING          = (signed portCHAR *)ERROR_CRLF"Memory device is missing"CRLF;

/*! Error msg upon selected partition not found error. */
const signed portCHAR *const FSCMDS_ERRMSG_NOPARTITION            = (signed portCHAR *)ERROR_CRLF"The selected partition doesn't exist"CRLF;

/*! Error msg upon unsupported file system error. */
const signed portCHAR *const FSCMDS_ERRMSG_FSNOTSUPPORTED         = (signed portCHAR *)ERROR_CRLF"Unsupported file system"CRLF;

/*! Unexpected error msg: SHOULD NEVER HAPPEN. */
const signed portCHAR *const FSCMDS_ERRMSG_UNEXPECTED             = (signed portCHAR *)ERROR_CRLF"Unexpected error"CRLF;

/*! Error msg if no drive is currently mounted. */
const signed portCHAR *const FSCMDS_ERRMSG_NOMOUNT                = (signed portCHAR *)ERROR_CRLF"No drive mounted"CRLF;

/*! Error msg upon file not found. */
const signed portCHAR *const FSCMDS_ERRMSG_FILENOTFOUND           = (signed portCHAR *)ERROR_CRLF"File not found"CRLF;

/*! Error msg upon file or directory not found. */
const signed portCHAR *const FSCMDS_ERRMSG_NOTFOUND               = (signed portCHAR *)ERROR_CRLF"File or directory not found"CRLF;

/*! Error msg if a file was expected and it's not. */
const signed portCHAR *const FSCMDS_ERRMSG_NOTAFILE               = (signed portCHAR *)ERROR_CRLF"Not a file"CRLF;

/*! Error msg if we try to read from or write to a file that is currently already opened in write mode. */
const signed portCHAR *const FSCMDS_ERRMSG_FILEWR                 = (signed portCHAR *)ERROR_CRLF"File currently opened in write-mode"CRLF;

/*! Error msg if a drive we wanted to write to is read-only . */
const signed portCHAR *const FSCMDS_ERRMSG_READONLYDRIVE          = (signed portCHAR *)ERROR_CRLF"Drive is read-only"CRLF;

/*! Error msg if the disk size is smaller than 4,1MB, or not supported by file system selected. */
const signed portCHAR *const FSCMDS_ERRMSG_BADSIZEFAT             = (signed portCHAR *)ERROR_CRLF"Unsupported drive size"CRLF;

/*! Error msg upon cd syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_CD_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: cd dirname"CRLF;
/*! Error msg upon unknown directory. */
const signed portCHAR *const FSCMDS_ERRMSG_CD_UNKNOWNDIR          = (signed portCHAR *)ERROR_CRLF"Unknown directory"CRLF;

/*! Error msg upon ls syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_LS_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: ls"CRLF;

/*! Error msg upon cat syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_CAT_SYNTAXERROR        = (signed portCHAR *)ERROR_CRLF"Usage: cat filename"CRLF;

/*! Error msg upon append syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_APPEND_SYNTAXERROR     = (signed portCHAR *)ERROR_CRLF"Usage: append filename"CRLF;

/*! Error msg upon help syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_HELP_SYNTAXERROR       = (signed portCHAR *)ERROR_CRLF"Usage: help"CRLF;

/*! Error msg when in maintenance mode. */
const signed portCHAR *const FSCMDS_ERRMSG_MAINTENANCE            = (signed portCHAR *)ERROR_CRLF"File system access forbidden(Maintenance mode)"CRLF;

/*! Error msg upon format syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_FORMAT_SYNTAXERROR     = (signed portCHAR *)ERROR_CRLF"Usage: format {a,b,c,d}"CRLF;

/*! Error msg upon touch syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_TOUCH_SYNTAXERROR      = (signed portCHAR *)ERROR_CRLF"Usage: touch filename"CRLF;

/*! Error msg upon out of memory event. */
const signed portCHAR *const FSCMDS_ERRMSG_OUTOFMEM               = (signed portCHAR *)ERROR_CRLF"Operation cancelled: out of memory"CRLF;

/*! Error msg upon incorrect file name format error. */
const signed portCHAR *const FSCMDS_ERRMSG_INCORRECTNAME          = (signed portCHAR *)ERROR_CRLF"Incorrect name, this must be not contain char \\/:*?\"<>|"CRLF;

/*! Error msg upon mkdir syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MKDIR_SYNTAXERROR      = (signed portCHAR *)ERROR_CRLF"Usage: mkdir dirname"CRLF;

/*! Error msg upon mkdir syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MKDIR_NAMEEXISTS       = (signed portCHAR *)ERROR_CRLF"Name already used."CRLF;

/*! Error msg upon mkdir syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_ISROOTDIR              = (signed portCHAR *)ERROR_CRLF"Current dir is a root dir."CRLF;

/*! Error msg upon rm syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_RM_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: rm {-R} {filename,dirname,*}"CRLF;

/*! Error msg upon rm of a non-empty directory error. */
const signed portCHAR *const FSCMDS_ERRMSG_DIRNOTEMPTY            = (signed portCHAR *)ERROR_CRLF"Directory not empty."CRLF;

/*! Error msg upon disk syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_DISK_SYNTAXERROR       = (signed portCHAR *)ERROR_CRLF"Usage: disk"CRLF;

/*! Error msg upon df syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_DF_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: df"CRLF;

/*! Error msg upon fat syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_FAT_SYNTAXERROR        = (signed portCHAR *)ERROR_CRLF"Usage: fat"CRLF;

/*! Error msg upon mv syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MV_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: mv src dst"CRLF;

/*! Error msg upon mv of different file types. */
const signed portCHAR *const FSCMDS_ERRMSG_MV_TYPEDIFFER          = (signed portCHAR *)ERROR_CRLF"Src & dst must be of the same type."CRLF;

/*! Error msg upon mark syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_MARK_SYNTAXERROR       = (signed portCHAR *)ERROR_CRLF"Usage: mark"CRLF;

/*! Error msg upon goto syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_GOTO_SYNTAXERROR       = (signed portCHAR *)ERROR_CRLF"Usage: goto"CRLF;

/*! Error msg upon cp syntax error. */
const signed portCHAR *const FSCMDS_ERRMSG_CP_SYNTAXERROR         = (signed portCHAR *)ERROR_CRLF"Usage: cp filename"CRLF;


/*! Temporary buffer holding a string. */
static portCHAR str_ascii[MAX_FILE_PATH_LENGTH];

/*! Array of index for each possible fs shell clients. */
static Fs_index ax_mark_index[SYS_NB_MOD];


static eFsCmdsStatus prv_ls( eModId xModId, signed short FsNavId );    // FORWARD
static eFsCmdsStatus prv_cat( eModId xModId, signed short FsNavId,
                              const FS_STRING pcStringFilename );      // FORWARD
static eFsCmdsStatus prv_append( eModId xModId, signed short FsNavId,
                                 const FS_STRING pcStringFilename );   // FORWARD
static eFsCmdsStatus prvCommonErrorLeave( void );                      // FORWARD
static bool prv_NameExists( const FS_STRING pxStringName );            // FORWARD
static eFsCmdsStatus prv_df( eModId xModId, signed short FsNavId );    // FORWARD
static eFsCmdsStatus prv_cp( eModId xModId, signed short FsNavId,
                             const FS_STRING pStringFilename );       // FORWARD
static void prv_Print_String_To_Requester_Stream( eModId xModId,
                             const portCHAR * StringToPrint );         // FORWARD


//_____ D E C L A R A T I O N S ____________________________________________


/***********
 ** MOUNT
 ***********/

/*!
 *  \brief The mount basic command: mount a disk.
 *
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param u8DriveId      Input. The target drive id.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_mount( signed short FsNavId, unsigned short u8DriveId)
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;


   if( u8DriveId >= nav_drive_nb() )
   {
      eFsStatus = FSCMDS_STATUS_ERR_DRIVE;
   }
   else
   {
      fsaccess_take_mutex();
      nav_select(FsNavId);
      nav_drive_set(u8DriveId);
      if (false == nav_partition_mount())
      {   // The operation failed.
         eFsStatus = e_fscmds_CheckNavError();
      }
      fsaccess_give_mutex();
   }
   return( eFsStatus );
}


/*!
 *  \brief The mount command: mount a disk.
 *         Takes one parameter, that is the disk letter.
 *         Format: mount {a,b,c,d}
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector. For this command, only av[0] is considered.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_mount( eModId xModId, signed short FsNavId,
                                  int ac, signed portCHAR *av[],
                                  signed portCHAR **ppcStringReply )
{
   unsigned int  u8DriveId;
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) One argument only.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNT_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   //  ii) The arg must be one of {a,b,c,d}. Accept the ':' char after the drive letter.
   u8DriveId = av[0][0] - 'a';
   if( ( av[0][1] != '\0' ) && ( av[0][1] != ':' ) )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNT_DRIVENOTFOUND;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   eFsStatus = e_fscmds_mount(FsNavId, u8DriveId);
   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}



/***********
 ** CD
 ***********/
/*!
 *  \brief The cd basic command
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pxStringDirName   Input. The directory name.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_cd( signed short FsNavId, FS_STRING pxStringDirName )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;


   fsaccess_take_mutex();
   nav_select(FsNavId);

   // try to enter the directory
   if(nav_setcwd(pxStringDirName, true, false)==false)
   {
      eFsStatus = e_fscmds_CheckNavError();
   }

   fsaccess_give_mutex();
   return( eFsStatus );
}

/*!
 *  \brief The cd command: Change directory.
 *         Takes one parameter, that is the dest directory.
 *         Format: cd dirname
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector. For this command, only av[0] is considered.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_cd( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus = FSCMDS_STATUS_OK;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;
   FS_STRING     pxDirName;


   // 1) Check the input.
   //  i) One argument only.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_CD_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // Go to a sub-directory == cd dirname
   // Alloc the string. need 2 bytes more : one for \ and one for \0
   pxDirName = (FS_STRING)pvPortMalloc( ( strlen( (char *)av[0] ) +2 ) * sizeof( FS_STRING ) );
   if( NULL == pxDirName )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }
   strcpy((char *)pxDirName, (char *)av[0]);
   strcat((char *)pxDirName, "\\");

   eFsStatus = e_fscmds_cd( FsNavId, pxDirName );
   // Free the string.
   vPortFree( pxDirName );

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}

/***********
 ** PWD
 ***********/
/*!
 *  \brief The pwd command: return current path.
 *         Format: pwd
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector. Ignored.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_pwd( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus = FSCMDS_STATUS_OK;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;
   signed portCHAR *pcStringToPrint;


   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_LS_SYNTAXERROR;
      return( eRetStatus );
   }

   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select( FsNavId );

   pcStringToPrint = (signed portCHAR *)pvPortMalloc( SHELL_MAX_MSGOUT_LEN ); // Alloc
   if( NULL == pcStringToPrint )
   {
      fsaccess_give_mutex(); // Release the fs resource.
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   if (false == nav_getcwd((FS_STRING)pcStringToPrint, SHELL_MAX_MSGOUT_LEN, false))
   {
      // The operation failed.
      eFsStatus = e_fscmds_CheckNavError();
   }
   else
   {
      prv_Print_String_To_Requester_Stream( xModId, (portCHAR *)pcStringToPrint );
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );
   }

   vPortFree( pcStringToPrint ); // Free.
   // Release mutex on fs module.
   fsaccess_give_mutex();

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** LS
 ***********/
/*!
 *  \brief The ls command: list current directory content.
 *         Format: ls
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector. Ignored.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_ls( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_LS_SYNTAXERROR;
      return( eRetStatus );
   }

   // 2) Perform the command.
   eFsStatus = prv_ls( xModId, FsNavId );
   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}



/***********
 ** CAT
 ***********/
/*!
 *  \brief The cat command: Print the content of a file.
 *         Takes one parameter, that is the filename to cat.
 *         Format: cat filename
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_cat( eModId xModId, signed short FsNavId,
                                int ac, signed portCHAR *av[],
                                signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;

   // 1) Check the input.
   //  i) Exactly one argument.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_CAT_SYNTAXERROR;
      return( eRetStatus );
   }

   // 2) Perform the command.
   eFsStatus = prv_cat( xModId, FsNavId, (FS_STRING)av[0] );

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}



/***********
 ** APPEND
 ***********/

/*  \brief The append command: append characters at the end of a file.
 *         Takes one parameter, that is the filename to edit.
 *         Format: append filename
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_append( eModId xModId, signed short FsNavId,
                                   int ac, signed portCHAR *av[],
                                   signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) Exactly one argument.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_APPEND_SYNTAXERROR;
      return( eRetStatus );
   }

   // 2) Perform the command.
   eFsStatus = prv_append( xModId, FsNavId, (FS_STRING)av[0] );

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** HELP
 ***********/

/*  \brief The help command: display a list of shell cmds supported by the fs.
 *         Format: help
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_help( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Perform the command.
   *ppcStringReply = (signed portCHAR *)FSCMDS_MSG_HELP;

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}


/***********
 ** TOUCH
 ***********/

/*  \brief The touch basic command
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pxStringFilename  Input. The file name.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_touch( signed short FsNavId, const FS_STRING pxStringFilename )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;
   Fs_index        sav_index;
   char            pcTempoDate[17];

   fsaccess_take_mutex();
   nav_select(FsNavId);
   sav_index = nav_getindex(); // Save the current nav position.

   // Set the navigator to the corresponding file, create the file if it doesn't exist.
   if( false == nav_setcwd(pxStringFilename, true, true))
   {
      file_close();  // Close the file.
   }
   // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
   v_cptime_GetDateInFatStringFormat( pcTempoDate );
   // Set the file date.
   nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );

   nav_gotoindex( &sav_index ); // Restore the initial nav position.
   fsaccess_give_mutex();
   return( eFsStatus );
}

/*
 *  \brief The touch command: create a new file.
 *         Takes one parameter, that is the filename.
 *         Format: touch filename
 *         WARNING: paths are not supported; i.e. the parameter must be a
 *         filename without path.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_touch( eModId xModId, signed short FsNavId,
                                  int ac, signed portCHAR *av[],
                                  signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) One argument only.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_TOUCH_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   eFsStatus = e_fscmds_touch( FsNavId, (FS_STRING)av[0] );

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** MKDIR
 ***********/

/*  \brief The mkdir command: create a new directory.
 *         Takes one parameter, that is the directory name.
 *         Format: mkdir directoryname
 *         WARNING: paths are not supported; i.e. the parameter must be a
 *         directoryname without path.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_mkdir( eModId xModId, signed short FsNavId,
                                  int ac, signed portCHAR *av[],
                                  signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;
   char          pcTempoDate[17];

   // 1) Check the input.
   //  i) One argument only.
   //  ii) Check that there is no path in the filename.
   if( ( 1 != ac ) || ( NULL != strchr( (char *)av[0], '/' ) ) )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MKDIR_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   fsaccess_take_mutex(); // Take the fs resource.
   nav_select(FsNavId);
   // Check if a file with that name already exists.
   if( true == prv_NameExists( (FS_STRING)av[0] ) )
   {
      *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MKDIR_NAMEEXISTS;
   }
   else if( false == nav_dir_make( (FS_STRING)av[0]) ) // Create the directory.
   {
      eFsStatus = e_fscmds_CheckNavError();
      if( ppcStringReply != NULL )
      {
         v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
      }
   }
   else
   {
      // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
      v_cptime_GetDateInFatStringFormat( pcTempoDate );
      // Set the directory date.
      nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );

      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }

   fsaccess_give_mutex(); // Release the fs resource.

   return( eRetStatus );
}


/***********
 ** DISK
 ***********/

/*  \brief The disk command: displays the number of drives.
 *         Format: disk
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_disk( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_DISK_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 3) Perform the command.
   *ppcStringReply = (signed portCHAR *)pvPortMalloc( SHELL_MAX_MSGOUT_LEN ); // Alloc
   if( NULL == *ppcStringReply )
   {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }
   sprintf((char *)*ppcStringReply, "Nb Drive(s): %d"CRLF, nav_drive_nb());

   return( SHELL_EXECSTATUS_OK );
}


/***********
 ** DF
 ***********/

/*  \brief The df command: display free space information for all connected drives.
 *         Format: df
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_df( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_DF_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   prv_df( xModId, FsNavId );

   if(ppcStringReply != NULL)
      *ppcStringReply = NULL; // Nothing to display in case of success.
   return( SHELL_EXECSTATUS_OK );
}


/***********
 ** RM
 ***********/

/*!
 *  \brief The rm all basic command. \n
 *  Remove all files and directories in current directory == rm {-R} *
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param bOnlyEmpty        Input. Boolean switch between:\n
 *                           false(=="non-empty directories can be deleted")\n
 *                           and true(=="non-empty directories cannot be deleted")
 *  WARNING: paths are not supported with *; i.e. the command may only be
 *           rm -R * or rm *
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_rm_all( signed short FsNavId, bool bOnlyEmpty )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;


   fsaccess_take_mutex(); // Take the fs resource.
   nav_select(FsNavId);   // Select the navigator.

   if( false == nav_filelist_first( FS_DIR ) )
   {
      nav_filelist_first( FS_FILE );
   }

   nav_filelist_reset();

   while ( nav_filelist_set(0,FS_FIND_NEXT) )
   {
      if( false == nav_file_del(bOnlyEmpty) )
      {
         eFsStatus = e_fscmds_CheckNavError();
         break;
      }
   }
   fsaccess_give_mutex(); // Release the fs resource.

   return( eFsStatus );
}


/*!
 *  \brief The rm basic command. \n
 *  Remove a specified file or a specified directory == rm {-R} {filename,dirname}
 *  NOTE: file and directory names can be used with a path.
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pxStringName      Input. The name.
 *  \param bOnlyEmpty        Input. Boolean switch between:\n
 *                           false(=="non-empty directories can be deleted")\n
 *                           and true(=="non-empty directories cannot be deleted")
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_rm( signed short FsNavId, const FS_STRING pxStringName,
                           bool bOnlyEmpty )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;
   Fs_index        sav_index;


   fsaccess_take_mutex(); // Take the fs resource.
   nav_select( FsNavId ); // Select the navigator.

   sav_index = nav_getindex(); // Save the current nav position.
   // Set the navigator to the corresponding file or directory.
   if( false == nav_setcwd(pxStringName, true, false))
   {
      eFsStatus = FSCMDS_STATUS_ERR_NOTFOUND;
   }
   else if( false == nav_file_del(bOnlyEmpty) ) // Delete the file or directory.
   {
      eFsStatus = e_fscmds_CheckNavError();
   }
   nav_gotoindex( &sav_index ); // Restore the initial nav position.

   fsaccess_give_mutex(); // Release the fs resource.

   return( eFsStatus );
}


/*!
 *  \brief The rm command: remove a specified file or a specified directory
 *                         or all files and directories.
 *         Takes up to two parameters, first an optional parameter to recursively
 *         delete directories(i.e. non empty directories may be deleted), and
 *         always the parameter which is the file to delete or the directory to
 *         delete or *.
 *         Format: rm {-R} {file,directory,*}
 *         NOTE: file and directory names can be used with a path.
 *         WARNING: paths are not supported with *; i.e. the command may only be
 *         rm -R * or rm *
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1 or 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_rm( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;

   // 1) Check the input.
   //  i) One argument only.
   if (( 1 != ac ) && ( 2 != ac ))
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_RM_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   if (!strcmp((char *)av[0], "-R"))
   {
      if( '*' == av[1][0] )
      {
      	 // Remove all files or directories in current directory == rm -R *
         eFsStatus = e_fscmds_rm_all( FsNavId, false );
      }
      else
      {
	     // Remove a specified file or directory.
         eFsStatus = e_fscmds_rm( FsNavId, (FS_STRING)av[1], false );
      }
   }
   else if( '*' == av[0][0] )
   {
   	  // Remove all files or empty directories in current directory == rm *
      eFsStatus = e_fscmds_rm_all( FsNavId, true );
   }
   else
   {
       // Remove a specified file or a specified empty directory.
       eFsStatus = e_fscmds_rm( FsNavId, (FS_STRING)av[0], true );
   }

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** FAT
 ***********/

/*  \brief The fat command: displays the fat type of the currently selected drive.
 *         Format: fat
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_fat( eModId xModId, signed short FsNavId,
                                int ac, signed portCHAR *av[],
                                signed portCHAR **ppcStringReply )
{
   int i=0;


   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FAT_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   fsaccess_take_mutex(); // Take the fs resource.
   nav_select( FsNavId ); // Select the navigator.

   // 2) Perform the command.
#if FS_FAT_12 == true
   if( Is_fat12 )
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)"Drive uses FAT12"CRLF );
   else i++;
#endif
#if FS_FAT_16 == true
   if( Is_fat16 )
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)"Drive uses FAT16"CRLF );
   else i++;
#endif
#if FS_FAT_32 == true
   if( Is_fat32 )
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)"Drive uses FAT32"CRLF );
   else i++;
#endif

   if(3 == i)
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)FSCMDS_ERRMSG_NOMOUNT );

   fsaccess_give_mutex(); // Release the fs resource.

   if(ppcStringReply != NULL)
      *ppcStringReply = NULL; // Nothing to display in case of success.
   return( SHELL_EXECSTATUS_OK );
}


/***********
 ** a:, b:, c:, d:
 ***********/
/*!
 *  \brief Shared mount drive command.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param u8DriveId      Input. The drive id to mount.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_goto_drive( unsigned int u8DriveId, signed short FsNavId,
                                 int ac, signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNTSHORT_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   eFsStatus = e_fscmds_mount(FsNavId, u8DriveId);
   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}

/*  \brief The a: command: mount and go to drive a.
 *         Format: a:
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_goto_a_drive( eModId xModId, signed short FsNavId,
                                         int ac, signed portCHAR *av[],
                                         signed portCHAR **ppcStringReply )
{
   return( e_fscmds_goto_drive( 'a'-'a', FsNavId, ac, ppcStringReply ) );
}

/*  \brief The b: command: mount and go to drive b.
 *         Format: b:
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_goto_b_drive( eModId xModId, signed short FsNavId,
                                         int ac, signed portCHAR *av[],
                                         signed portCHAR **ppcStringReply )
{
   return( e_fscmds_goto_drive( 'b'-'a', FsNavId, ac, ppcStringReply ) );
}

/*  \brief The c: command: mount and go to drive c.
 *         Format: c:
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_goto_c_drive( eModId xModId, signed short FsNavId,
                                         int ac, signed portCHAR *av[],
                                         signed portCHAR **ppcStringReply )
{
   return( e_fscmds_goto_drive( 'c'-'a', FsNavId, ac, ppcStringReply ) );
}

/*  \brief The d: command: mount and go to drive d.
 *         Format: d:
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_goto_d_drive( eModId xModId, signed short FsNavId,
                                         int ac, signed portCHAR *av[],
                                         signed portCHAR **ppcStringReply )
{
   return( e_fscmds_goto_drive( 'd'-'a', FsNavId, ac, ppcStringReply ) );
}


/***********
 ** MARK
 ***********/

/*!
 * \brief return x from a 2powx number.
 *
 * \param xModId Module id, which is a 2powx number.
 *
 * \return the x of 2powx
 */
static int prv_i_GetIdxFromModid( eModId xModId )
{
   int  exp;


   frexp(xModId,&exp);
   return(exp-1);
}

/*  \brief The mark command: bookmark current directory.
 *         Format: mark
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_mark( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply )
{
   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MARK_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   fsaccess_take_mutex(); // Take the fs resource.
   nav_select( FsNavId ); // Select the navigator.

   ax_mark_index[prv_i_GetIdxFromModid(xModId)] = nav_getindex();

   fsaccess_give_mutex(); // Release the fs resource.
   if(ppcStringReply != NULL)
      *ppcStringReply = NULL; // Nothing to display in case of success.
   return( SHELL_EXECSTATUS_OK );
}


/***********
 ** GOTO
 ***********/
/*!
 *  \brief The goto command: goto the bookmarked directory.
 *         Format: goto
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 0.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_goto( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply )
{
eFsCmdsStatus   eFsStatus;
   // 1) Check the input.
   //  i) No argument.
   if( 0 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_GOTO_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   fsaccess_take_mutex(); // Take the fs resource.
   nav_select( FsNavId ); // Select the navigator.
   if( false == nav_gotoindex( &(ax_mark_index[prv_i_GetIdxFromModid( xModId )]) ) )
   {
     eFsStatus = prvCommonErrorLeave();
     if( ppcStringReply != NULL )
     {
       v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
     }
     return( SHELL_EXECSTATUS_KO );
   }
   else
      fsaccess_give_mutex(); // Release the fs resource.

   if(ppcStringReply != NULL)
      *ppcStringReply = NULL; // Nothing to display in case of success.
   return( SHELL_EXECSTATUS_OK );
}


/***********
 ** FORMAT
 ***********/
/*!
 *  \brief The format basic command: format a disk.
 *
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param u8DriveId      Input. The target drive id.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_format( signed short FsNavId, unsigned short u8DriveId )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;
   unsigned short  u8CurrentDriveId;


   if( u8DriveId >= nav_drive_nb() )
   {
      eFsStatus = FSCMDS_STATUS_ERR_DRIVE;
   }
   else
   {
      fsaccess_take_mutex();  // Take the fs resource.
      nav_select(FsNavId); // Select the navigator.
      u8CurrentDriveId = nav_drive_get(); // Save the current drive.
      nav_drive_set(u8DriveId); // Select the drive to format.
      if( false == nav_drive_format( FS_FORMAT_DEFAULT ) )
      {   // The operation failed.
         eFsStatus = e_fscmds_CheckNavError();
         nav_drive_set( u8CurrentDriveId ); // Restore the current drive.
      }
      else if( u8DriveId == u8CurrentDriveId )
      {  // The format operation succeeded and it was done on the current drive.
         // Since the format operation was done on the currently mounted drive,
         // we reset the navigator & re-mount that drive.
         nav_filelist_reset();
         nav_drive_set(u8DriveId);
         if (false == nav_partition_mount())
         {   // The operation failed.
            eFsStatus = e_fscmds_CheckNavError();
         }
      }
      else // The format operation succeeded and it was done on another drive
           // than the currently selected one.
         nav_drive_set( u8CurrentDriveId ); // Restore the current drive.
      fsaccess_give_mutex(); // Release the fs resource.
   }
   return( eFsStatus );
}


/*!
 *  \brief The format command: format the specified drive.
 *         Takes one parameter, that is the drive to format.
 *         Format: format drive
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_format( eModId xModId, signed short FsNavId,
                                   int ac, signed portCHAR *av[],
                                   signed portCHAR **ppcStringReply )
{
   unsigned int  u8DriveId;
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;


   // 1) Check the input.
   //  i) One argument only.
   if( 1 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FORMAT_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   //  ii) The arg must be one of {a,b,c,d}. Accept the ':' char after the drive letter.
   u8DriveId = av[0][0] - 'a';
   if( ( av[0][1] != '\0' ) && ( av[0][1] != ':' ) )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNT_DRIVENOTFOUND;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   eFsStatus = e_fscmds_format(FsNavId, u8DriveId);
   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** CP
 ***********/
/*!
 *  \brief The cp command: copy a specified file to the bookmarked directory.
 *         Takes one parameter, that is the file to copy.
 *         Format: cp filename
 *         WARNING: paths are not supported; i.e. the parameter must be a
 *         filename without path.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_cp( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;

   // 1) Check the input.
   if( 1 != ac )
   {
   	  // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_CP_SYNTAXERROR;
      return( eRetStatus );
   }

   // 2) Perform the command.
   eFsStatus = prv_cp( xModId, FsNavId, (FS_STRING)av[0] );

   if( FSCMDS_STATUS_OK == eFsStatus )
   {
      if(ppcStringReply != NULL)
         *ppcStringReply = NULL;   // Nothing to display in case of success.
      eRetStatus = SHELL_EXECSTATUS_OK;
   }
   else if( ppcStringReply != NULL )
   {
      v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
   }

   return( eRetStatus );
}


/***********
 ** MV
 ***********/
/*!
 *  \brief The mv command: rename a file or a directory.
 *         Takes two parameters, the file to rename as the first parameter and
 *         the new name as second parameter.
 *         Format: mv src dst
 *         WARNING: paths are not supported; i.e. parameters must be filenames
 *         without path.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_fscmds_shell_mv( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   eFsCmdsStatus eFsStatus = FSCMDS_STATUS_OK;
   eExecStatus   eRetStatus = SHELL_EXECSTATUS_KO;
   Fs_index      sav_index;
   bool          bSrcType, bDstType;


   // 1) Check the input.
   //  i) Two arguments exactly.
   if( 2 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MV_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // 2) Perform the command.
   fsaccess_take_mutex(); // Take the fs resource.
   nav_select(FsNavId);

   sav_index = nav_getindex(); // Save the current nav position.

   // Set the navigator on the src file.
   if( false == nav_setcwd((FS_STRING)av[0], true, false))
   {
      eFsStatus = e_fscmds_CheckNavError();
      if( ppcStringReply != NULL )
      {
         v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
      }
   }
   else
   {
      // First we want to forbid the cases "mv srcfile dstdir-that-already-exists"
      // and "mv srcdir dstfile-that-already-exists"
      bSrcType = nav_file_isdir();
      if( true == nav_setcwd((FS_STRING)av[1], true, false))
      {   // The dst already exists.
         bDstType = nav_file_isdir();
         if( bSrcType != bDstType )
         {
            eFsStatus = FSCMDS_STATUS_KO;
            if( ppcStringReply != NULL )
               *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MV_TYPEDIFFER;
         }
         else
         {
            fsaccess_give_mutex(); // Release the fs resource.
            eFsStatus = e_fscmds_rm( FsNavId, (FS_STRING)av[1], true );
            fsaccess_take_mutex(); // Take the fs resource.
            if( FSCMDS_STATUS_OK != eFsStatus )
            {
               if( ppcStringReply != NULL )
               {
                  v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
               }
            }
         }
      }

      if( FSCMDS_STATUS_OK == eFsStatus )
      {
         // Rename the pxSrcName to pxDstName.
         if( false == nav_file_rename( (FS_STRING)av[1] ) )
         {
            eFsStatus = e_fscmds_CheckNavError();
            if( ppcStringReply != NULL )
            {
               v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
            }
         }
         else
         {
            if(ppcStringReply != NULL)
               *ppcStringReply = NULL;   // Nothing to display in case of success.
            eRetStatus = SHELL_EXECSTATUS_OK;
         }
      }

   }
   // The previous check was successful.
   nav_gotoindex( &sav_index ); // Restore the nav to point on the src.

   fsaccess_give_mutex(); // Release the fs resource.

   return( eRetStatus );
}


/*!
 *  \brief Check the FAT module global var fs_g_status to get the error that
 *         occurred in the FAT module.
 *
 *  \return a status translation of the FAT module error status.
 */
eFsCmdsStatus e_fscmds_CheckNavError( void )
{
   switch(fs_g_status)
   {
      case FS_ERR_HW:              // hw driver error
         return( FSCMDS_STATUS_ERR_HW );
      case FS_ERR_FS:              // File system error
         return( FSCMDS_STATUS_ERR_FS );
      case FS_ERR_NO_MOUNT:        // The partition is not mounted.
         return( FSCMDS_STATUS_ERR_NOMOUNT );
      case FS_ERR_NO_FORMAT:       // The selected drive isn't formatted
         return( FSCMDS_STATUS_ERR_NOFORMAT );
      case FS_ERR_HW_NO_PRESENT:   // Memory device is missing.
         return( FSCMDS_STATUS_ERR_MEMDEVMISSING );
      case FS_ERR_NO_PART:         // The partition selected doesn't exist
         return( FSCMDS_STATUS_ERR_NOPARTITION );
      case FS_ERR_NO_SUPPORT_PART: // The selected partition file system isn't supported
         return( FSCMDS_STATUS_ERR_FSNOTSUPPORTED );
      case FS_ERR_NO_FIND:         // File not found.
         return( FSCMDS_STATUS_ERR_FILENOTFOUND );
      case FS_ERR_NO_FILE:         // Not a file.
         return( FSCMDS_STATUS_ERR_NOTAFILE );
      case FS_ERR_FILE_OPEN_WR:    // The file is already opened in write-mode.
         return( FSCMDS_STATUS_ERR_FILEWR );
      case FS_LUN_WP:              // The drive is read-only
         return( FSCMDS_STATUS_ERR_READONLY );
      case FS_ERR_BAD_SIZE_FAT:    // The disk size is smaller than 4,1MB, or not supported by file system selected
         return( FSCMDS_STATUS_ERR_BADSIZEFAT );
      case FS_ERR_OUT_LIST:        // NOTE: Too specific message; we include it in the out-of-memory category.
      case FS_ERR_NO_FREE_SPACE:   // Out of memory
         return( FSCMDS_STATUS_ERR_OUTOFMEM );
      case FS_ERR_INCORRECT_NAME:  // Incorrect name, this must be not contain char \/:*?"<>|
         return( FSCMDS_STATUS_ERR_INCORRECTNAME );
      case FS_ERR_IS_ROOT:         // Cannot go up in the dir tree because the current dir is a root dir.
         return( FSCMDS_STATUS_ERR_ISROOTDIR );
      case FS_ERR_DIR_NOT_EMPTY:   // Directory not empty.
         return( FSCMDS_STATUS_ERR_DIRNOTEMPTY );

      default:                     // Should never happen.
         NAKED_TRACE_COM2( "Unexpected fs_g_status:%d", fs_g_status );
         return( FSCMDS_STATUS_ERR_UNKNOWN );
   }
}


/*!
 *  \brief Map a string message to an error.
 *
 *  \param ErrStatus      Input. The error status.
 *  \param ppcStringReply Input/Output. The response string. The input is not NULL.
 */
void v_fscmds_GetStrMsgFromErr( eFsCmdsStatus ErrStatus, signed portCHAR **ppcStringReply )
{
   switch( ErrStatus )
   {
      case FSCMDS_STATUS_ERR_DRIVE:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNT_DRIVENOTFOUND;
         break;
      case FSCMDS_STATUS_ERR_MOUNT:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MOUNT_FAILED;
         break;
      case FSCMDS_STATUS_ERR_UNKNOWNDIR:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_CD_UNKNOWNDIR;
         break;
      case FSCMDS_STATUS_ERR_HW:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_HW;
         break;
      case FSCMDS_STATUS_ERR_FS:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FS;
         break;
      case FSCMDS_STATUS_ERR_NOFORMAT:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_NOFORMAT;
         break;
      case FSCMDS_STATUS_ERR_NOMOUNT:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_NOMOUNT;
         break;
      case FSCMDS_STATUS_ERR_MEMDEVMISSING:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_MEMDEVMISSING;
         break;
      case FSCMDS_STATUS_ERR_NOPARTITION:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_NOPARTITION;
         break;
      case FSCMDS_STATUS_ERR_FSNOTSUPPORTED:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FSNOTSUPPORTED;
         break;
      case FSCMDS_STATUS_ERR_FILENOTFOUND:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FILENOTFOUND;
         break;
      case FSCMDS_STATUS_ERR_NOTFOUND:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_NOTFOUND;
         break;
      case FSCMDS_STATUS_ERR_NOTAFILE:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_NOTAFILE;
         break;
      case FSCMDS_STATUS_ERR_FILEWR:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_FILEWR;
         break;
      case FSCMDS_STATUS_ERR_READONLY:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_READONLYDRIVE;
         break;
      case FSCMDS_STATUS_ERR_BADSIZEFAT:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_BADSIZEFAT;
         break;
      case FSCMDS_STATUS_ERR_OUTOFMEM:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_OUTOFMEM;
         break;
      case FSCMDS_STATUS_ERR_INCORRECTNAME:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_INCORRECTNAME;
         break;
      case FSCMDS_STATUS_ERR_ISROOTDIR:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_ISROOTDIR;
         break;
      case FSCMDS_STATUS_ERR_DIRNOTEMPTY:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_DIRNOTEMPTY;
         break;
      case FSCMDS_STATUS_ERR_MEMALLOC:
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
         break;

      case FSCMDS_STATUS_ERR_UNKNOWN:
         *ppcStringReply = (signed portCHAR *)FSCMDS_ERRMSG_UNEXPECTED;
         break;

      default: // SHOULD NEVER HAPPEN.
         break;
   }
}


/************************************ PRIVATE ********************************/

/*!
 *  \brief Print a string directly to the output stream of a module.
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param pStringToPrint   Input. The string to print.
 *
 */
static void prv_Print_String_To_Requester_Stream( eModId xModId, const portCHAR * pStringToPrint )
{
   switch( xModId )
   {
      case SYS_MODID_COM1SHELL:
         vcom1shell_PrintMsg((signed portCHAR *) pStringToPrint );
         break;
      case SYS_MODID_USB:
         TRACE_COM2((char *)"Unimplemented stream redirection (from fscmds to USB)");
         break;
      default: // SHOULD NEVER HAPPEN
         TRACE_COM2((char *)"Unexpected stream redirection");
         break;
   }
}

/*!
 *  \brief Put a char to the output stream of a module.
 *
 *  \param xModId       Input. The module associated with the target output stream.
 *  \param c            Input. The char to put to the target output stream.
 *
 */
static void prv_PutChar_To_Requester_Stream( eModId xModId, signed portCHAR c )
{
   switch( xModId )
   {
      case SYS_MODID_COM1SHELL:
         vcom1shell_PutChar( c );
         break;
      case SYS_MODID_USB:
         TRACE_COM2((char *)"Unimplemented stream redirection (from fscmds to USB)");
         break;
      default: // SHOULD NEVER HAPPEN
         TRACE_COM2((char *)"Unexpected stream redirection");
         break;
   }
}

/*!
 *  \brief Get a char from the input stream of a module.
 *
 *  \param xModId   Input. The module associated with the target input stream.
 *
 *  \return the character
 *
 */
static signed portCHAR prv_GetChar_From_Requester_Stream( eModId xModId )
{
   signed portCHAR cByte;


   switch( xModId )
   {
      case SYS_MODID_COM1SHELL:
         while( pdFALSE == com1shell_GetChar( &cByte ) );
         break;
      case SYS_MODID_USB:
         TRACE_COM2((char *)"Unimplemented stream redirection (from USB to fscmds)");
         break;
      default: // SHOULD NEVER HAPPEN
         TRACE_COM2((char *)"Unexpected stream redirection");
         break;
   }
   return( cByte );
}

/*!
 *  \brief The ls basic command
 *
 *  \param xModId            Input. The module that is calling this function.
 *  \param FsNavId           Input. The file system navigator id to use.
 *
 *  \return the status of the operation.
 *
 */
static eFsCmdsStatus prv_ls( eModId xModId, signed short FsNavId )
{
   eFsCmdsStatus   eFsStatus = FSCMDS_STATUS_OK;
   signed portCHAR *pcStringToPrint;


   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select( FsNavId );

   if( false == nav_dir_name( (FS_STRING) str_ascii, MAX_FILE_PATH_LENGTH ) ) // Get the current dir name
   {
      return( prvCommonErrorLeave() );
   }

   pcStringToPrint = (signed portCHAR *)pvPortMalloc( SHELL_MAX_MSGOUT_LEN ); // Alloc
   if( NULL == pcStringToPrint )
   {
      fsaccess_give_mutex(); // Release the fs resource.
      return( FSCMDS_STATUS_ERR_MEMALLOC );
   }

   // 1) Print info on the current directory.
   sprintf( (char *)pcStringToPrint, CRLF"Volume is %c: (%s)"CRLF"Dir name is ",
             'A'+nav_drive_get(), mem_name( nav_drive_get() ) );
   v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
   prv_Print_String_To_Requester_Stream( xModId, str_ascii );
   v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );

   // 2) Print size and name info for each file and directory in the current directory.
   if( false == nav_filelist_first( FS_DIR ) )
   {
      nav_filelist_first( FS_FILE );
   }
   v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)"\tSize (Bytes)\tName"CRLF );
   nav_filelist_reset();
   while ( nav_filelist_set(0,FS_FIND_NEXT) )
   {
      sprintf( (char *)pcStringToPrint, "%3s\t%u\t\t",
                (nav_file_isdir()) ? "Dir" : "",
                (unsigned int)nav_file_lgt() );
      v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
      nav_file_name( (FS_STRING)str_ascii, MAX_FILE_PATH_LENGTH, FS_NAME_GET, false );
      prv_Print_String_To_Requester_Stream( xModId, str_ascii );
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );
   }

   // 3) Print the number of files and subdirectories in the current directory.
   sprintf( (char *)pcStringToPrint, "%d  Files"CRLF"%d  Dir"CRLF,
             nav_filelist_nb( FS_FILE ),
             nav_filelist_nb( FS_DIR ) );
   v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );

   vPortFree( pcStringToPrint ); // Free.

   // Release mutex on fs module.
   fsaccess_give_mutex();
   return( eFsStatus );
}


/*!
 *  \brief The cat basic command
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pStringFilename  Input. The filename.
 *
 *  \return the status of the operation.
 *
 */
static eFsCmdsStatus prv_cat( eModId xModId, signed short FsNavId, const FS_STRING pStringFilename )
{
   signed portCHAR *pcOutString;
   size_t          filesize;
   size_t          RequestedSize;
   size_t          ReadSize;
   Fs_index        sav_index;

   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select(FsNavId);

   sav_index = nav_getindex(); // Save the current nav position.

   /* 1) Set the nav to point on the target file. */
   if ( false == nav_setcwd(pStringFilename, true, false) )
   {
      fsaccess_give_mutex();          // Release the fs mutex.
      return( FSCMDS_STATUS_ERR_FILENOTFOUND );
   }

   /* 2) Open the target file. */
   if( false == file_open( FOPEN_MODE_R ) )
   {
      nav_gotoindex( &sav_index ); // Restore the initial nav position.
      return( prvCommonErrorLeave() );
   }

   /* 3) Print the content of the file. */
   // The fsaccess functions need the fs mutex => we have to release it.
   fsaccess_give_mutex();
   // Get the file size.
   filesize = fsaccess_file_get_size( FsNavId );
   // Malloc the output string.
   if(filesize < FS_SIZE_OF_SECTOR)
   {
      pcOutString = (signed portCHAR *)pvPortMalloc( filesize +1 );
      if( NULL == pcOutString )
      {
         return( FSCMDS_STATUS_ERR_MEMALLOC );
      }
      RequestedSize = filesize;
   }
   else
   {
      pcOutString = (signed portCHAR *)pvPortMalloc( FS_SIZE_OF_SECTOR +1 );
      if( NULL == pcOutString )
      {
         return( FSCMDS_STATUS_ERR_MEMALLOC );
      }
      RequestedSize = FS_SIZE_OF_SECTOR;
   }
   // Read and print the file content.
   while( filesize )
   {
      // Read a chunk of characters.
      ReadSize = read( FsNavId, pcOutString, RequestedSize );
      if( ReadSize != RequestedSize )
      {
         TRACE_COM2("");
         vPortFree( pcOutString );
         // Take mutex on fs module.
         fsaccess_take_mutex();
         nav_select( FsNavId );
         file_close();
         nav_gotoindex( &sav_index ); // Restore the initial nav position.
         return( prvCommonErrorLeave() );
      }
      else
         filesize -= ReadSize; // Update the remaining nb of characters to read.
      // Update the size of the next chunk request.
      RequestedSize = (filesize < FS_SIZE_OF_SECTOR) ? filesize : FS_SIZE_OF_SECTOR;
      // Print the chunk to the stream associated with xModId.
      *(pcOutString +ReadSize) = '\0'; // NULL-terminate the chunk to make a string.
      v_shell_Print_String_To_Requester_Stream( xModId, pcOutString );
   }
   v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );

   vPortFree( pcOutString );

   /* Close the file. */
   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select( FsNavId );
   file_close();

   nav_gotoindex( &sav_index ); // Restore the initial nav position.

   // Release mutex on fs module.
   fsaccess_give_mutex();

   return( FSCMDS_STATUS_OK );
}

/*!
 *  \brief The append basic command
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pxStringFilename  Input. The filename.
 *
 *  \return the status of the operation.
 *
 */
static eFsCmdsStatus prv_append( eModId xModId, signed short FsNavId, const FS_STRING pxStringFilename )
{
   signed portCHAR cTempoChar;
   signed portCHAR cPrevChar=0;
   Fs_index        sav_index;
   char            pcTempoDate[17];


   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select(FsNavId);

   sav_index = nav_getindex(); // Save the current nav position.

   /* 1) Set the nav to point on the target file. */
   if(nav_setcwd((FS_STRING)pxStringFilename, true, false)==false)
   {
      fsaccess_give_mutex();          // Release the fs mutex.
      return( FSCMDS_STATUS_ERR_FILENOTFOUND );
   }

   /* 2) Open the target file. */
   if( false == file_open( FOPEN_MODE_APPEND ) )
   {
      nav_gotoindex( &sav_index ); // Restore the initial nav position.
      return( prvCommonErrorLeave() );
   }

   /* 3) Switch to editor mode. */
   // Print a welcome banner to the output stream associated with xModId.
   v_shell_Print_String_To_Requester_Stream( xModId, FSCMDS_MSG_APPEND_WELCOME );
   // Editor loop: get a char, echo it, write it at the end of the file.
   while( 1 )
   {
      cTempoChar = prv_GetChar_From_Requester_Stream( xModId );
      if( cTempoChar == QUIT_APPEND )
      {
         v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );
         break;
      }
      if( ( cTempoChar == LF ) && ( cPrevChar != CR ) )
      {
         prv_PutChar_To_Requester_Stream( xModId, CR );
         file_putc( CR );
      }
      prv_PutChar_To_Requester_Stream( xModId, cTempoChar ); // Echo.
      // Write to file
      if( false == file_putc( cTempoChar ) )
      {
         file_close();
         nav_gotoindex( &sav_index ); // Restore the initial nav position.
         return( prvCommonErrorLeave() );
      }
      if( cTempoChar == CR ) // Special for CRLF.
      {
         prv_PutChar_To_Requester_Stream( xModId, LF );
         file_putc( LF );
      }
      cPrevChar = cTempoChar;
   }

   // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
   v_cptime_GetDateInFatStringFormat( pcTempoDate );
   // Set the file date.
   nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );

   /* 4) Close the file. */
   file_close();

   nav_gotoindex( &sav_index ); // Restore the initial nav position.

   // Release mutex on fs module.
   fsaccess_give_mutex();

   return( FSCMDS_STATUS_OK );
}


/*!
 *  \brief The df basic command \n
 *  Display free space information for all connected drives.
 *
 *  \param xModId            Input. The module that is calling this function.
 *  \param FsNavId           Input. The file system navigator id to use.
 *
 *  \return the status of the operation.
 *
 */
static eFsCmdsStatus prv_df( eModId xModId, signed short FsNavId )
{
   signed portCHAR *pcStringToPrint;
   signed portCHAR *pcStringErrMsg;
   int             i,NbDrive;
   Fs_index        sav_index;
   eFsCmdsStatus   eFsStatus;


   fsaccess_take_mutex(); // Take mutex on fs module.
   nav_select( FsNavId );

   pcStringToPrint = (signed portCHAR *)pvPortMalloc( SHELL_MAX_MSGOUT_LEN ); // Alloc
   if( NULL == pcStringToPrint )
   {
      fsaccess_give_mutex(); // Release mutex on fs module.
      return( FSCMDS_STATUS_ERR_MEMALLOC );
   }

   sav_index = nav_getindex();   // Save current nav position
   NbDrive = nav_drive_nb();     // Get the number of drives.
   // Display free space information for each connected drive.
   for( i = 0; i < NbDrive; i++ )
   {
      if( false == nav_drive_set( i ) ) // Select drive
      {
         eFsStatus = e_fscmds_CheckNavError();
         v_fscmds_GetStrMsgFromErr( eFsStatus, &pcStringErrMsg );
         sprintf( (char *)pcStringToPrint, "%s (%c:) %s"CRLF,
                  mem_name(i),
                  (char)('a' + i),
                  pcStringErrMsg );
         v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
      }
      else if( false == nav_partition_mount() ) // Mount the drive
      {
         eFsStatus = e_fscmds_CheckNavError();
         v_fscmds_GetStrMsgFromErr( eFsStatus, &pcStringErrMsg );
         sprintf( (char *)pcStringToPrint, "%s (%c:) %s"CRLF,
                  mem_name(i),
                  (char)('a' + i),
                  pcStringErrMsg );
         v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
      }
      else
      {
         sprintf( (char *)pcStringToPrint, "%s (%c:) Free Space: %llu Bytes / %llu Bytes"CRLF,
                  mem_name(i),
                  (char)('a' + i),
                  (unsigned long long int)(nav_partition_freespace() << FS_SHIFT_B_TO_SECTOR),
                  (unsigned long long int)(nav_partition_space() << FS_SHIFT_B_TO_SECTOR) );
         v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
      }
   }
   nav_gotoindex( &sav_index );  // Restore nav position

   vPortFree( pcStringToPrint ); // Free.

   fsaccess_give_mutex(); // Release mutex on fs module.
   return( FSCMDS_STATUS_OK );
}

/*!
 *  \brief The cp basic command
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pStringFilename  Input. The filename to copy.
 *
 *  \return the status of the operation.
 *
 */
static eFsCmdsStatus prv_cp( eModId xModId, signed short FsNavId, const FS_STRING pStringFilename )
{
   U16             file_size;
   Fs_index        sav_index;


   // Take mutex on fs module.
   fsaccess_take_mutex();
   nav_select(FsNavId);

   // Set the nav to point on the target file.
   if(nav_setcwd(pStringFilename, true, false)==false)
   {
      fsaccess_give_mutex();          // Release the fs mutex.
      return( FSCMDS_STATUS_ERR_FILENOTFOUND );
   }
   else
   {
      // Get name of source to be used as same destination name
      if( false == nav_file_name( pStringFilename, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true) )
      {
         return( prvCommonErrorLeave() );
      }
      // Get file size
      file_size = nav_file_lgtsector();
      // Mark source
      nav_file_copy();
      // Save current source position
      sav_index = nav_getindex();
      // Goto destination
      nav_gotoindex( &(ax_mark_index[prv_i_GetIdxFromModid( xModId )]) );
      // free space check
      if (nav_partition_space() > file_size)
      {
         // Paste
         nav_file_paste_start( pStringFilename );
         // Restore previous nav position
         nav_gotoindex( &sav_index );
         // Perform copy
         while(nav_file_paste_state( false ) == COPY_BUSY);
         // Restore previous nav position
      }
      nav_gotoindex(&sav_index);
   }
   fsaccess_give_mutex();          // Release the fs mutex.
   return( FSCMDS_STATUS_OK );
}


/*!
 *  \brief Common upon-error-prologue of several functions.
 *
 *  \return cmd execution status.
 */
static eFsCmdsStatus prvCommonErrorLeave( void )
{
   eFsCmdsStatus eFsStatus;

   eFsStatus = e_fscmds_CheckNavError(); // Get the fs error.
   fsaccess_give_mutex();          // Release the fs mutex.
   return( eFsStatus );
}

/*!
 *  \brief Check if a file and/or directory already exists in the current directory.
 *
 *  \param pxStringName  Input. The filename to compare against.
 *
 *  \note The mutex must have already been taken by the calling function
 *  and the navigator is already set by the calling function.
 *
 *  \return true if the file already exists.
 */
static bool prv_NameExists( const FS_STRING pxStringName )
{
   int    i;


   if( false == nav_filelist_first( FS_DIR ) )
   {
      nav_filelist_first( FS_FILE );
   }
   i = nav_filelist_nb( FS_DIR ) + nav_filelist_nb( FS_FILE );
   while ( i )
   {
      nav_file_name( (FS_STRING)str_ascii, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true );
      if( 0 == strcmp( (char *)str_ascii, (char *)pxStringName ) )
         return( true );
      nav_filelist_set( 0, FS_FIND_NEXT );
      i--;
   }
   return( false );
}
