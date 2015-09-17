/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel Command Exec module.
 *
 * This module provides a Command execution service.
 *
 *
 *                       The example is written for UC3 and EVK1100.
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


/*!
 * Brief description of the module.
 * This module is a central point for commands execution.
 *
 * Detailed description of the module.
 * It proposes only one interface function, Shell_exec(). This function parses
 * a command line string to:
 *    1) identify the command
 *    2) verify that the caller has the right to execute the command
 *    3) tokenize all arguments of the command
 * If the command has been found, and the caller has the right to execute it,
 * the command is executed. The command execution is managed by the target module.
 * Target modules are: sensor, actuator, filesys, supervisor.
 *
 * The input command line string must have the following format:
 * cmd [arg[=val]], 5 (arg,val) maximum.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <string.h>
#include "compiler.h"
#include "board.h"

#include "shell.h"

#include "sensor.h"
#include "actuator.h"
#include "fscmds.h"
#include "syscmds.h"
#include "supervisor.h"
#include "tracedump.h"
#include "ethernet.h"
#include "usbsys.h"
#include "com1shell.h"

//_____ M A C R O S ________________________________________________________


/*! Max number of tokens */
#define SHELL_MAX_NBTOKEN       8

/*! Number of registered commands. */
#if configCTRLPANEL_TRACE == 1
#define SHELL_NB_COMMANDS       37
#else
#define SHELL_NB_COMMANDS       35
#endif

/*! First file system cmd idx in the a_cmd_registration[] array. */
#define SHELL_FS_FIRSTCMDIDX    9

/*! Last file system cmd idx in the a_cmd_registration[] array. */
#define SHELL_FS_LASTCMDIDX     29


//_____ D E F I N I T I O N S ______________________________________________

extern eExecStatus e_supervisor_switch_to_maintenance_mode( eModId xModId,
                              signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply );

static eExecStatus e_Shell_help( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply ); // FORWARD

/*!
 * The cmd status enum.
 */
typedef enum {
   SHELL_CMDSTATUS_FOUND,              // All commands.
   SHELL_CMDSTATUS_NOTFOUND,           // Cmd not found.
   SHELL_CMDSTATUS_PERMISSIONDENIED,   // Cmd not found.
} eCmdStatus;


/*!
 * The type of a command registration entry.
 */
typedef struct st_cmd_registration {
   const signed portCHAR *const pc_string_cmd_name;  // The name of the command
   pfShellCmd      pf_exec_cmd;          // Ptr on the exec function
   portBASE_TYPE   mod_rights_map;       // The module rights map for this command.
                                         // Each bit is assigned to a module.
                                         // Built with eModId enums.
}Cmd_registration;


/*!
 * The array of commands registration.
 */
Cmd_registration a_cmd_registration[SHELL_NB_COMMANDS] = {
   {(signed portCHAR *)"!!", e_Shell_help, SYS_MODID_COM1SHELL}, // ALWAYS KEEP AS THE FIRST COMMAND IN THE ARRAY.
   {(signed portCHAR *)"get_sensor_value", e_sensor_cmd_get_value, 0xFFFFFFFF},
   {(signed portCHAR *)"get_sensor_config", e_sensor_cmd_get_config, 0xFFFFFFFF},
   {(signed portCHAR *)"get_actuator_value", e_actuator_cmd_get_value, 0xFFFFFFFF},
   {(signed portCHAR *)"get_sys_config", e_syscmds_cmd_get_config, 0xFFFFFFFF},
   {(signed portCHAR *)"set_sensor_config", e_sensor_cmd_set_config, 0xFFFFFFFF},
   {(signed portCHAR *)"set_actuator_value", e_actuator_cmd_set_value, 0xFFFFFFFF},
   {(signed portCHAR *)"set_sys_config", e_syscmds_cmd_set_config, 0xFFFFFFFF},
   {(signed portCHAR *)"reboot", e_syscmds_reboot, SYS_MODID_COM1SHELL},
   {(signed portCHAR *)"mount", e_fscmds_shell_mount, SYS_MODID_COM1SHELL|SYS_MODID_USB}, // MUST ALWAYS BE AT INDEX SHELL_FS_FIRSTCMDIDX
   {(signed portCHAR *)"cd", e_fscmds_shell_cd, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"ls", e_fscmds_shell_ls, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"pwd", e_fscmds_shell_pwd, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"cat", e_fscmds_shell_cat, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"append", e_fscmds_shell_append, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"touch", e_fscmds_shell_touch, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"mkdir", e_fscmds_shell_mkdir, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"disk", e_fscmds_shell_disk, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"df", e_fscmds_shell_df, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"rm", e_fscmds_shell_rm, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"fat", e_fscmds_shell_fat, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"a:", e_fscmds_shell_goto_a_drive, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"b:", e_fscmds_shell_goto_b_drive, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"c:", e_fscmds_shell_goto_c_drive, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"d:", e_fscmds_shell_goto_d_drive, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"mark", e_fscmds_shell_mark, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"goto", e_fscmds_shell_goto, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"mv", e_fscmds_shell_mv, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"cp", e_fscmds_shell_cp, SYS_MODID_COM1SHELL|SYS_MODID_USB},
   {(signed portCHAR *)"format", e_fscmds_shell_format, SYS_MODID_COM1SHELL|SYS_MODID_USB|SYS_MODID_HTTP}, // MUST ALWAYS BE AT INDEX SHELL_FS_LASTCMDIDX
   {(signed portCHAR *)"help", e_Shell_help, 0xFFFFFFFF},
#if configCTRLPANEL_TRACE == 1
   {(signed portCHAR *)"ipstat", e_ip_stat, SYS_MODID_COM1SHELL},
#endif
   {(signed portCHAR *)"lsusb", e_usbsys_lsusb, SYS_MODID_COM1SHELL},
   {(signed portCHAR *)"cp_logs_to_ukey", e_usbsys_cp_logs_to_key, SYS_MODID_COM1SHELL},
   {(signed portCHAR *)"maintain", e_supervisor_switch_to_maintenance_mode, SYS_MODID_COM1SHELL},
#if configCTRLPANEL_TRACE == 1
   {(signed portCHAR *)"dbgtrace", e_syscmds_trace, SYS_MODID_COM1SHELL},
#endif
   {(signed portCHAR *)"version", e_syscmds_version, SYS_MODID_COM1SHELL}
};


/*!
 *  String messages.
 */
/*! Error msg upon command not found. */
const signed portCHAR *const SHELL_MSG_CMDNOTFOUND      = (signed portCHAR *)"Error"CRLF"Command not found"CRLF;
/*! Error msg upon syntax error. */
const signed portCHAR *const SHELL_MSG_SYNTAXERROR      = (signed portCHAR *)"Error"CRLF"Syntax error"CRLF;
/*! Error msg upon command execution permission denied. */
const signed portCHAR *const SHELL_MSG_PERMISSIONDENIED = (signed portCHAR *)"Error"CRLF"Permission denied"CRLF;

const signed portCHAR *const SHELL_MSG_HELP = (signed portCHAR *)"Commands summary"CRLF"help"CRLF"!!: execute the previous command"CRLF;

/*! Error msg if !! is executed and there is no previous command. */
const signed portCHAR *const SHELL_MSG_NOHISTORY = (signed portCHAR *)"Error"CRLF"No previous command"CRLF;

/*! Error msg upon help syntax error. */
const signed portCHAR *const SHELL_ERRMSG_HELP_SYNTAXERROR       = (signed portCHAR *)"Error"CRLF"Usage: help [sensor,actuator,sys,fs]"CRLF;

/*! Previous cmd index. */
static portBASE_TYPE   PrevCmdIdx = -1;
static portBASE_TYPE   PrevCmdAc;
signed portCHAR        *PrevCmdAv[SHELL_MAX_NBTOKEN];

/*! The SHELLFS system mutex. */
extern xSemaphoreHandle   xSHELLFSMutex;

static eCmdStatus prvCmdIdentify_Tokenize( signed portCHAR *pcStringCmd,
                                           eModId xModId,
                                           portBASE_TYPE *ac,
                                           signed portCHAR **av,
                                           portBASE_TYPE *pCmdIdx ); // FORWARD


//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief a command line executor.
 *
 *  \param pcStringCmd    Input. The cmd line to execute. NULL-terminated string.
 *                        Format: cmd [arg] [arg=val], with 6 (arg,val) maximum.
 *                        WARNING: this string will be modified.
 *  \param xModId         Input. The module that is calling this exe function.
 *  \param FsNavId        Input. The file system navigator id to use if the cmd
 *                        is a file system command.
 *  \param ppcStringReply Output. The caller must free this string (by calling vportFree())
 *                        only if it is non-NULL and the returned status is OK.
 *
 *  \return the status of the command execution.
 */
eExecStatus Shell_exec( signed portCHAR *pcStringCmd,
                        eModId xModId,
                        signed short FsNavId,
                        signed portCHAR **ppcStringReply)
{
   eCmdStatus        xCmdStatus;
   portBASE_TYPE     ac;
   signed portCHAR   *av[SHELL_MAX_NBTOKEN];
   portBASE_TYPE     CmdIdx;
   int               i;
   eExecStatus       xRet;


   // 1) Identify the command and tokenize the rest of the command line.
   xCmdStatus = prvCmdIdentify_Tokenize( pcStringCmd, xModId, &ac, av, &CmdIdx );
   if(SHELL_CMDSTATUS_NOTFOUND == xCmdStatus)
   {   // Command not found.
      if(ppcStringReply != NULL)
      {
         *ppcStringReply = (signed portCHAR *)SHELL_MSG_CMDNOTFOUND;
      }
      return(SHELL_EXECSTATUS_KO);
   }
   else if(SHELL_CMDSTATUS_PERMISSIONDENIED == xCmdStatus)
   {   // Permission denied.
      if(ppcStringReply != NULL)
      {
         *ppcStringReply = (signed portCHAR *)SHELL_MSG_PERMISSIONDENIED;
      }
      return(SHELL_EXECSTATUS_KO);
   }

   /* Get the SHELLFS mutex for File system access. */
   // We put this check here to reduce the code footprint (normally it should be
   // in each fs cmd function).
   if( ( CmdIdx >= SHELL_FS_FIRSTCMDIDX ) && ( CmdIdx <= SHELL_FS_LASTCMDIDX ) )
   {
      if( pdFALSE == x_supervisor_SemaphoreTake( xSHELLFSMutex, 0 ) )
      {   // Failed to get the SHELLFS mutex.
         if(ppcStringReply != NULL)
         {
            *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MAINTENANCEMODE;
         }
         return(SHELL_EXECSTATUS_KO);
      }
   }

   /* Special case for the !! command. Only for the SYS_MODID_COM1SHELL module,
      cf. a_cmd_registration[]. */
   if( 0 == CmdIdx ) // !! command
   {
      CmdIdx = PrevCmdIdx; // Execute the previous cmd.
      if( -1 == CmdIdx )
      {   // No previous command.
         if(ppcStringReply != NULL)
         {
            *ppcStringReply = (signed portCHAR *)SHELL_MSG_NOHISTORY;
         }
         return(SHELL_EXECSTATUS_KO);
      }
      ac = PrevCmdAc;      // NOTE: we take a shortcut here: we suppose the caller
                           // keeps the same pcStringCmd from one call to another.
      for(i=0; i<ac; i++)   av[i] = PrevCmdAv[i];
   }
   else
   {
      PrevCmdIdx = CmdIdx; // Save the cmd (used by the !! cmd).
      PrevCmdAc = ac;
      for(i=0; i<ac; i++)   PrevCmdAv[i] = av[i];
   }

   // 2) Execute the command.
   if(ppcStringReply != NULL)
      *ppcStringReply = NULL;
   xRet = a_cmd_registration[CmdIdx].pf_exec_cmd( xModId, FsNavId, ac, av, ppcStringReply );

   /* Release the SHELLFS mutex. */
   if( ( CmdIdx >= SHELL_FS_FIRSTCMDIDX ) && ( CmdIdx <= SHELL_FS_LASTCMDIDX ) )
   {
      x_supervisor_SemaphoreGive( xSHELLFSMutex );
   }

   return( xRet );
}


/*!
 *  \brief Identify the cmd and tokenize the rest of the command line string.
 *
 *  \param pcStringCmd    Input. The cmd line to parse. NULL-terminated string.
 *                        Format: cmd [arg] [arg=val], with 6 (arg,val) maximum.
 *                        WARNING: this string will be modified.
 *  \param xModId         Input. The module that is requesting the exe of this cmd line.
 *  \param ac             Output. Argument count, in [0,SHELL_MAX_NBTOKEN].
 *  \param av             Output. Argument vector, made of SHELL_MAX_NBTOKEN string ptrs maximum.
 *  \param pCmdIdx        Output. The index of the command, in one of the cmd array.
 *
 *  \return the command status(eCmdStatus), indicating the Identify & Tokenize status.
 */
static eCmdStatus prvCmdIdentify_Tokenize( signed portCHAR *pcStringCmd, eModId xModId,
                                           portBASE_TYPE *ac, signed portCHAR **av,
                                           portBASE_TYPE *pCmdIdx )
{
   signed portCHAR   *pcStringPtr = pcStringCmd;
   size_t            token_len, parsed_len, tempo_len;
   portBASE_TYPE     cmd_len = strlen( (char *)pcStringCmd );

   /*
    * 1) Identify the command.
    */
   //    i) skip space.
   pcStringPtr += parsed_len = strspn((char *)pcStringCmd, " ");
   //    ii) Get the len of the cmd name (i.e. the token we're on).
   // NOTE: a cmd name is delimited by: {space,\0}.
   parsed_len += token_len = strcspn((char *)pcStringPtr," ");
   // Insert a \0 on the space or on \0.
   pcStringPtr[token_len] = '\0';

   //    iii) Spot the cmd in the cmd registration array.
   for(*pCmdIdx = 0; *pCmdIdx < SHELL_NB_COMMANDS; (*pCmdIdx)++)
   {   // Search in the regular commands array.
      if(0 == strcmp( (char *)pcStringPtr,
                      (char *)a_cmd_registration[*pCmdIdx].pc_string_cmd_name ) )
         break;
   }
   if(SHELL_NB_COMMANDS == *pCmdIdx)
      return(SHELL_CMDSTATUS_NOTFOUND);   // Command not found.

   /*
    * 2) Check the module rights on this cmd.
    */
   if( 0 == ( xModId & a_cmd_registration[*pCmdIdx].mod_rights_map ) )
      return(SHELL_CMDSTATUS_PERMISSIONDENIED);

   pcStringPtr += token_len+1;
   parsed_len++;
   // pcStringPtr now points just after the \0 after the cmd name.

   /*
    * 3) Tokenize the rest of the string.
    */
   *ac = 0; // Init arg count to 0 (no args yet).
   while( ( parsed_len < cmd_len ) && (*pcStringPtr != '\0') && (*ac <= SHELL_MAX_NBTOKEN) )
   {
      /**
       ** Parse one arg.
       **/
      // Skip space.
      parsed_len += tempo_len = strspn((char *)pcStringPtr, " =");
      pcStringPtr += tempo_len;
      // pcStringPtr now points to the beginning of a token or at the end of the
      // cmd line.
      if(*pcStringPtr == '\0')
         break; // End of command line.
      // pcStringPtr now points to the beginning of a token.

      // Get the len of this token.
      if(*pcStringPtr == '"')
      {
         pcStringPtr++;   // Dump the ".
         parsed_len++;
         // Deal with string values, beginning with a " and ending with a ".
         // Strings can hold spaces, thus a space should not be considered a
         // delimiter when between "".
         // => a token is delimited by: {",\0}.
         token_len = strcspn((char *)pcStringPtr,"\"");
         // NOTE: token_len is == 0 if the string is just "". We consider this
         // a parameter (i.e. a string value is set to empty). So ac will be
         // incremented and the corresponding av will be set to an empty string.
      }
      else
      {
         // NOTE: a token is delimited by: {space,equal-sign,\0}.
         token_len = strcspn((char *)pcStringPtr," =");
      }

      // One more token.
      av[*ac] = pcStringPtr;
      (*ac)++;
      pcStringPtr += token_len;
      parsed_len += token_len;
      if(*pcStringPtr == '\0')
         break; // End of command line.
      else
         *pcStringPtr++ = '\0';
      // pcStringPtr now points on a space or on an equal-sign or at the end
      // of the cmd line.
   }

   return(SHELL_CMDSTATUS_FOUND);
}


/*!
 *  \brief The help command: displays all available commands.
 *         Format: help
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this exe function.
 *  \param FsNavId        Input. The file system navigator id to use if the cmd
 *                        is a file system command. NOT USED.
 *  \param ac             Input. The argument counter. Not considered for this command.
 *  \param av             Input. The argument vector. Not considered for this command.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
static eExecStatus e_Shell_help( eModId xModId, signed short FsNavId,
                                 int ac, signed portCHAR *av[],
                                 signed portCHAR **ppcStringReply )
{
   signed portCHAR *pcStringSensor;
   signed portCHAR *pcStringActuator;
   signed portCHAR *pcStringFs;
   signed portCHAR *pcStringSys;
   int             ShellMsgLen = strlen((char *)SHELL_MSG_HELP);
   eExecStatus     eRetStatus = SHELL_EXECSTATUS_KO;


   // 0) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( eRetStatus );

   // TODO: add one possible parameter, one of sensor, actuator, sys, fs.
   // i.e.: help, help sensor, help actuator, help sys, help fs.
   // Check the input: no argument.
   if( 0 != ac )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_HELP_SYNTAXERROR;
      return( eRetStatus );
   }

   // Ask for the file system help.
   e_fscmds_shell_help( xModId, FsNavId, ac, av, &pcStringFs );
   // Ask for the sensors help.
   e_sensor_help( xModId, FsNavId, ac, av, &pcStringSensor );
   // Ask for the actuators help.
   e_actuator_help(xModId, FsNavId, ac, av, &pcStringActuator );
   // Ask for the sys help.
   e_syscmds_help(xModId, FsNavId, ac, av, &pcStringSys );

   // Malloc the necessary memory to concatenate the messages.
   ShellMsgLen += strlen( (char *)pcStringFs ) + strlen( (char *)pcStringSensor )
                  + strlen( (char *)pcStringActuator ) + strlen( (char *)pcStringSys ) +1;
   *ppcStringReply = (signed portCHAR *)pvPortMalloc(ShellMsgLen);

   // Build the message.
   strcpy((char *)*ppcStringReply, (char *)SHELL_MSG_HELP);
   strcat((char *)*ppcStringReply, (char *)pcStringSensor);
   strcat((char *)*ppcStringReply, (char *)pcStringActuator);
   strcat((char *)*ppcStringReply, (char *)pcStringFs);
   strcat((char *)*ppcStringReply, (char *)pcStringSys);

   // NOTE: we know that the strings returned from the misc _help functions
   // are const. So we don't free these strings.

   return(SHELL_EXECSTATUS_OK);
}


/*!
 *  \brief Print a string directly to the output stream of a module.
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param pcStringToDisplay Input. The string to print.
 *
 */
void v_shell_Print_String_To_Requester_Stream( eModId xModId,
                                               const signed portCHAR *pcStringToDisplay )
{
   switch( xModId )
   {
      case SYS_MODID_COM1SHELL:
         vcom1shell_PrintMsg(pcStringToDisplay);
         break;
      case SYS_MODID_USB:
         TRACE_COM2((char *)"Unimplemented stream redirection (from fscmds to USB)");
         break;
      default: // SHOULD NEVER HAPPEN
         TRACE_COM2((char *) "Unexpected stream redirection");
         break;
   }
}
