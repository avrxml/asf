/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel file system basic & shell commands interface.
 *
 * This module provides commands interface to the file system.
 *
 *
 *                       Written for UC3 and EVK1100.
 *
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 * This module provides a set of shell & basic commands to interface with the file system.
 *
 * Detailed description of the module.
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef __FSCMDS_H__
#define __FSCMDS_H__

#include "fs_com.h"

#include "shell.h"

//_____ M A C R O S ________________________________________________________



//_____ D E F I N I T I O N S ______________________________________________

/*!
 * The exec command return status.
 */
typedef enum {
   FSCMDS_STATUS_OK,                // The operation succeeded.
   FSCMDS_STATUS_KO,                // The operation failed.
   FSCMDS_STATUS_ERR_DRIVE,         // Wrong drive number.
   FSCMDS_STATUS_ERR_MOUNT,         // The mount operation failed.
   FSCMDS_STATUS_ERR_NOMOUNT,       // Partition not mounted.
   FSCMDS_STATUS_ERR_UNKNOWNDIR,    // Directory not found.
   FSCMDS_STATUS_ERR_HW,            // Hw driver error.
   FSCMDS_STATUS_ERR_FS,            // File system error.
   FSCMDS_STATUS_ERR_NOFORMAT,      // Selected drive is not formatted.
   FSCMDS_STATUS_ERR_MEMDEVMISSING, // Memory device is missing.
   FSCMDS_STATUS_ERR_NOPARTITION,   // The selected partition doesn't exist.
   FSCMDS_STATUS_ERR_FSNOTSUPPORTED,// The selected partition file system is not supported.
   FSCMDS_STATUS_ERR_FILENOTFOUND,  // File not found.
   FSCMDS_STATUS_ERR_NOTFOUND,      // File or directory not found.
   FSCMDS_STATUS_ERR_NOTAFILE,      // Not a file.
   FSCMDS_STATUS_ERR_FILEWR,        // The file is already opened in write-mode.
   FSCMDS_STATUS_ERR_READONLY,      // The drive is read-only.
   FSCMDS_STATUS_ERR_BADSIZEFAT,    // The disk size is smaller than 4,1MB, or not supported by file system selected
   FSCMDS_STATUS_ERR_OUTOFMEM,      // Out of memory
   FSCMDS_STATUS_ERR_INCORRECTNAME, // Incorrect name, this must be not contain char \/:*?"<>|
   FSCMDS_STATUS_ERR_ISROOTDIR,     // Cannot go up in the dir tree because the current dir is a root dir
   FSCMDS_STATUS_ERR_DIRNOTEMPTY,   // Directory not empty
   FSCMDS_STATUS_ERR_MEMALLOC,      // Memory alloc failure

   FSCMDS_STATUS_ERR_UNKNOWN        // Unexpected error.
} eFsCmdsStatus;


//_____ D E C L A R A T I O N S ____________________________________________

/*  \brief The mount shell command: mount a disk.
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
                                  signed portCHAR **ppcStringReply );


/*  \brief The mount basic command: mount a disk.
 *
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param u8DriveId      Input. The target drive id.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_mount( signed short FsNavId, unsigned short u8DriveId);


/*  \brief The cd command: Change directory.
 *         Takes one parameter, that is the dest directory.
 *         Format: cd dirname
 *         WARNING: paths are not supported; i.e. the parameter must be a
 *         directoryname without path.
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
                               signed portCHAR **ppcStringReply );

/*  \brief The cd basic command
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pcStringDirName   Input. The directory name, without path.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_cd( signed short FsNavId, FS_STRING pcStringDirName );


/*  \brief The ls command: list current directory content.
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
                               signed portCHAR **ppcStringReply );


/*  \brief The cat command: Print the content of a file.
 *         Takes one parameter, that is the filename to cat.
 *         Format: cat filename
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
eExecStatus e_fscmds_shell_cat( eModId xModId, signed short FsNavId,
                                int ac, signed portCHAR *av[],
                                signed portCHAR **ppcStringReply );


/*  \brief The append command: append characters at the end of a file.
 *         Takes one parameter, that is the filename to edit.
 *         Format: append filename
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
eExecStatus e_fscmds_shell_append( eModId xModId, signed short FsNavId,
                                   int ac, signed portCHAR *av[],
                                   signed portCHAR **ppcStringReply );


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
                                 signed portCHAR **ppcStringReply );


/*  \brief The touch command: create a new file or update the date of a file.
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
                                  signed portCHAR **ppcStringReply );


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
                                  signed portCHAR **ppcStringReply );


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
                                 signed portCHAR **ppcStringReply );


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
                               signed portCHAR **ppcStringReply );


/*  \brief The rm command: remove a specified file or a specified empty directory
 *                         or all files and empty directories in current directory.
 *         Takes one parameter, that is the file to delete or the directory to
 *         delete or *.
 *         Format: rm {file,directory,*}
 *         WARNING: paths are not supported; i.e. the parameter must be a
 *         filename or a directoryname or a * without path.
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
eExecStatus e_fscmds_shell_rm( eModId xModId, signed short FsNavId,
                               int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply );


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
                                signed portCHAR **ppcStringReply );


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
                                         signed portCHAR **ppcStringReply );


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
                                         signed portCHAR **ppcStringReply );


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
                                         signed portCHAR **ppcStringReply );


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
                                         signed portCHAR **ppcStringReply );


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
                                 signed portCHAR **ppcStringReply );


/*  \brief The goto command: goto the bookmarked directory.
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
                                 signed portCHAR **ppcStringReply );


/*!
 *  \brief The format basic command: format a disk.
 *
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param u8DriveId      Input. The target drive id.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_format( signed short FsNavId, unsigned short u8DriveId );

/*  \brief The format command: format the specified drive.
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
                                   signed portCHAR **ppcStringReply );


/*  \brief The cp command: copy a specified file to the bookmarked directory.
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
                               signed portCHAR **ppcStringReply );


/*  \brief The mv command: rename a file.
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
 *  \param ac             Input. The argument counter. Should be 1.
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
                               signed portCHAR **ppcStringReply );

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
                               signed portCHAR **ppcStringReply );

/*!
 *  \brief Check the FAT module global var fs_g_status to get the error that
 *         occurred in the FAT module.
 *
 *  \return a status translation of the FAT module error status.
 */
eFsCmdsStatus e_fscmds_CheckNavError( void );


/*!
 *  \brief Map a string message to an error.
 *
 *  \param ErrStatus      Input. The error status.
 *  \param ppcStringReply Input/Output. The response string. The input is not NULL.
 */
void v_fscmds_GetStrMsgFromErr( eFsCmdsStatus ErrStatus, signed portCHAR **ppcStringReply );

/*  \brief The touch basic command
 *
 *  \param FsNavId           Input. The file system navigator id to use.
 *  \param pxStringFilename  Input. The file name.
 *
 *  \return the status of the operation.
 */
eFsCmdsStatus e_fscmds_touch( signed short FsNavId, const FS_STRING pxStringFilename );

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
eFsCmdsStatus e_fscmds_rm_all( signed short FsNavId, bool bOnlyEmpty );

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
                           bool bOnlyEmpty );

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
                                 int ac, signed portCHAR **ppcStringReply );

#endif // __FSCMDS_H__
