/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel local time module.
 *
 * The local time module is in charge of the Control Panel local time management.
 *
 * - Supported boards:   EVK1100
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#include "cptime.h"

#include "shell.h"

//_____ M A C R O S ________________________________________________________

/*! Max nb of scheduled commands. */
#define CPTIME_MAX_NB_SCHED_CMDS   10

//_____ D E F I N I T I O N S ______________________________________________

/*! Error msg if there is no scheduling slot left. */
const signed portCHAR *const CPTIME_ERRMSG_SETVAL_UNAVAILSCHEDSLOT = (signed portCHAR *)ERROR_CRLF"No scheduling slot left"CRLF;

/*! Error msg if the time for scheduling is invalid. */
const signed portCHAR *const CPTIME_ERRMSG_SETVAL_INVALIDTIME      = (signed portCHAR *)ERROR_CRLF"Invalid schedule date"CRLF;

/*! Error msg upon set cfg time syntax error. */
const signed portCHAR *const CPTIME_ERRMSG_SETCFG_SYNTAXERROR      = (signed portCHAR *)ERROR_CRLF"Usage: set_sys_config sys=time field=value"CRLF;

/*! Error msg upon set cfg time of an invalid config field. */
const signed portCHAR *const CPTIME_ERRMSG_SETCFG_UNKNOWNFIELD     = (signed portCHAR *)ERROR_CRLF"Unknown configuration field"CRLF;

/*! Error msg upon set_sys_config sys=time curtime="date" where date is invalid. */
const signed portCHAR *const CPTIME_ERRMSG_SETCFG_INVALIDDATE      = (signed portCHAR *)ERROR_CRLF"Invalid date"CRLF;


/*! The local time raw value. */
time_t xcptime_LocalTime;

/*! The OS tick origin. This may not be == 0, if the local time is set through
 a shell command or acquired though the TIME protocol. */
static portTickType xcptime_TickOrigin = 0;

/*!
 * The type of a scheduled command entry.
 */
typedef struct st_scheduled_cmd {
   // The time the cmd is scheduled to execute.
   time_t   xScheduledTime;
   // The Scheduled cmd id
   int      CmdId;
   // Fct ptr to the scheduled function.
   void     (*pfScheduledCmd)( int, void * );
   // The parameter to pass to the scheduled function.
   void     *pxParam;
}Scheduled_Cmd;

/*! Array of scheduled commands. */
static Scheduled_Cmd axCmdsSched[CPTIME_MAX_NB_SCHED_CMDS] = {
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL},
 {0, 0, NULL, NULL} };

/*! Nb of scheduled commands. */
static int NbSchedCmd = 0;

//_____ D E C L A R A T I O N S ____________________________________________
static void prv_v_ShiftScheduledCmdsArray( int SrcIdx, int DstIdx); //FORWARD

/*!
 * \brief Update the local time.
 *
 */
void v_cptime_UpdateLocalTime( void )
{
   xcptime_LocalTime = ( xTaskGetTickCount() - xcptime_TickOrigin )/configTICK_RATE_HZ;
}


/*!
 * \brief Init the time module.
 */
void v_cptime_Init( void )
{
   struct tm xLogDate;


   // Init. of the Control Panel current time to: April 3rd 2007 00:00:00
   xLogDate.tm_mon = 3;    // April
   xLogDate.tm_mday = 3;   // April 3rd
   xLogDate.tm_year = 107; // 2007
   xLogDate.tm_hour = 0;
   xLogDate.tm_min = 0;
   xLogDate.tm_sec = 0;
   xLogDate.tm_isdst = 0; // Daylight saving time info is not in effect.
   // Convert time broken-down representation to arithmetic representation.
   xcptime_LocalTime = mktime( &xLogDate );
}


#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_cptime_trace( void )
{
   NAKED_TRACE_COM2( "CPTIME:<%d><%d>", NbSchedCmd, xcptime_LocalTime );
}
#endif


/*!
 * \brief Get the current time in the "YYYYMMDDHHMMSSMS" string format.
 *
 * \param pcDate  Input/Output. Input allocated array. Output is the current time
 *                expressed in the "YYYYMMDDHHMMSSMS" string format.
 */
void v_cptime_GetDateInFatStringFormat( char *pcDate )
{
  struct tm *pxDate;

  // Get the broken-down representation of the current date.
  pxDate = gmtime( &xcptime_LocalTime );

  // WARNING: pxDate->tm_year == number of years since 1900.
  // For years >= 2000, we'll display the last 2 digits only.
  /*       if( pxDate->tm_year >= 100 )
            pxDate->tm_year -= 100;*/
  if( pxDate->tm_year >= 100 )
    pxDate->tm_year = 2000 + ( pxDate->tm_year - 100 );
  else
    pxDate->tm_year = 1900 + pxDate->tm_year;
  sprintf( pcDate, "%.4d%.2d%.2d%.2d%.2d0000", pxDate->tm_year,
           pxDate->tm_mon +1, pxDate->tm_mday, pxDate->tm_hour, pxDate->tm_min );
}

/*!
 *  \brief Record a scheduled command.
 *
 *  \param pcDate         Input. The date the cmd is scheduled at.
 *                        Format: seconds OR mm/dd/yy hh:mm:ss
 *  \param CmdId          Scheduled command id.
 *  \param pfScheduledCmd pointer on fct to be called at expiration date
 *  \param pvCmdParams    pointer on struct to give as param to pfScheduledCmd
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_RecordScheduledCmd( char *pcDate, int CmdId,
                                         void (*pfScheduledCmd)( int, void * ),
                                         void *pvCmdParams,
                                         signed portCHAR **ppcStringReply )
{
   time_t   xTimeOffsetSec;
   int      i, j, k;


   // Check if there is at least one spot left in the scheduled cmd array.
   if( CPTIME_MAX_NB_SCHED_CMDS == NbSchedCmd )
   {
      *ppcStringReply = (signed portCHAR *)CPTIME_ERRMSG_SETVAL_UNAVAILSCHEDSLOT;
      return( SHELL_EXECSTATUS_KO );
   }

   // First implementation: the timevalue is an offset in seconds.
   xTimeOffsetSec = xcptime_LocalTime + atoi( pcDate );
   if( xTimeOffsetSec < ( xcptime_LocalTime +3 ) )
   {
      *ppcStringReply = (signed portCHAR *)CPTIME_ERRMSG_SETVAL_INVALIDTIME;
      return( SHELL_EXECSTATUS_KO );
   }

   // Find a spot where to insert the new record.
   for( i=0; (i<CPTIME_MAX_NB_SCHED_CMDS)&&(0 != axCmdsSched[i].xScheduledTime)&&(axCmdsSched[i].xScheduledTime <= xTimeOffsetSec); i++ );

   NbSchedCmd++; // One more scheduled cmd.

   // Shift the cells if the spot was used.
   if(i<CPTIME_MAX_NB_SCHED_CMDS) // Not necessary because of the first test done in this function: added to remove a compiler warning.
   {
     if( 0!= axCmdsSched[i].xScheduledTime )
     {
        for(j=NbSchedCmd-1, k=NbSchedCmd-2; k>=i; j--, k--)
        {
           if( 0 != axCmdsSched[k].xScheduledTime )
           {
              axCmdsSched[j].xScheduledTime = axCmdsSched[k].xScheduledTime;
              axCmdsSched[j].CmdId = axCmdsSched[k].CmdId;
              axCmdsSched[j].pfScheduledCmd = axCmdsSched[k].pfScheduledCmd;
              axCmdsSched[j].pxParam = axCmdsSched[k].pxParam;
           }
        }
     }

     // Record the scheduled cmd.
     axCmdsSched[i].xScheduledTime = xTimeOffsetSec;
     axCmdsSched[i].CmdId = CmdId;
     axCmdsSched[i].pfScheduledCmd = pfScheduledCmd;
     axCmdsSched[i].pxParam = pvCmdParams;
   }

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}


/*!
 * \brief Execute a scheduled command if expiration date is up.
 */
void v_cptime_ExecuteScheduledCmd( void )
{
   if( ( 0 != axCmdsSched[0].xScheduledTime )
       && ( axCmdsSched[0].xScheduledTime <= xcptime_LocalTime ) )
   {
      // Execute the command.
      axCmdsSched[0].pfScheduledCmd( axCmdsSched[0].CmdId, axCmdsSched[0].pxParam );

      axCmdsSched[0].xScheduledTime = 0; // Mark this cell as unused.
      NbSchedCmd--; // One scheduled cmd removed.

      // Shift the array of scheduled commands.
      prv_v_ShiftScheduledCmdsArray(1,0);
   }
}


/*! \brief The set time config command: set each config field value of the time module.
 *         Takes one parameter : field=value
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
   /* int           fd;
   eFsCmdsStatus eFsStatus; */
   struct tm     xLogDate;
   time_t        xRawDate;
   int           i;
   int           SrcIdx=0;


   // NOTE: currently the only configurable item of the time module is the
   // current time. "curtime=mm/dd/yy hh:mm:ss"

   // 1) Check the input.
   //  i) Exactly two arguments (the field and the value).
   if( 2 != ac )
   {   // Syntax error.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)CPTIME_ERRMSG_SETCFG_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // The field must be one of : {curtime}
   if( !strcmp("curtime", (char *)av[0]) )
   {
      // 2) Read the current time string
      sscanf( (char *)av[1], "%02d/%02d/%02d %02d:%02d:%02d",
              &(xLogDate.tm_mon), &(xLogDate.tm_mday), &(xLogDate.tm_year),
              &(xLogDate.tm_hour), &(xLogDate.tm_min), &(xLogDate.tm_sec) );
      (xLogDate.tm_mon)--; // Adjust to the logic where January index is 0.
      // WARNING: pxLogDate->tm_year == number of years since 1900.
      // We get the last 2 digits only : we suppose the year is >= 2000.
      xLogDate.tm_year += 100;
      xLogDate.tm_isdst = 0; // Daylight saving time info is not in effect.

      // Convert time broken-down representation to arithmetic representation.
      xRawDate = mktime( &xLogDate );

      if( -1 == xRawDate )
      {
         if(ppcStringReply != NULL)
            *ppcStringReply = (signed portCHAR *)CPTIME_ERRMSG_SETCFG_INVALIDDATE;
         return( SHELL_EXECSTATUS_KO );
      }
      else
      {
         NAKED_TRACE_COM2( "CPTIME:NbSchedCmd=%d", NbSchedCmd );
         // Update all cells of the scheduled commands array: cancel the schedules
         // that are in the past but keep the schedules that are still in the future.
         for( i=NbSchedCmd-1; i>=0; i-- )
         {
            if( axCmdsSched[i].xScheduledTime <= xRawDate )
            {    // This schedule is in the past => cancel it and all other cells
                 // that are further down the past.
               // NOTE: all below scheduled commands are obsolete if this one is.
               // => Cancel them too.
               SrcIdx = i+1; // Remember the last active scheduled commands index.
               for( i=0; i<SrcIdx; i++ )
               {
                  axCmdsSched[i].xScheduledTime = 0; // Mark this cell as unused.
                  NbSchedCmd--; // One scheduled cmd removed.
               }
               NAKED_TRACE_COM2( "CPTIME:NbSchedCmd=%d, SrcIdx=%d", NbSchedCmd, SrcIdx );
               break;
            }
         }

         // Shift the Scheduled Commands array down to the index 0.
         if( ( 0 != NbSchedCmd ) && ( CPTIME_MAX_NB_SCHED_CMDS > SrcIdx ) )
         {
            NAKED_TRACE_COM2( "CPTIME:Shifting array from SrcIdx=%d to 0 idx", SrcIdx );
            prv_v_ShiftScheduledCmdsArray( SrcIdx, 0 );
         }

         // Switch to new time.
         xcptime_LocalTime = xRawDate;
      }
   }
   else
   {   // Unsupported field.
      if(ppcStringReply != NULL)
         *ppcStringReply = (signed portCHAR *)CPTIME_ERRMSG_SETCFG_UNKNOWNFIELD;
      return( SHELL_EXECSTATUS_KO );
   }
   return( SHELL_EXECSTATUS_OK );


/****** CODE TO USE WHEN THERE ARE OTHER FIELDS THAN CURTIME.
   if ((fd = open(TIME_CONFIG_FILE, (O_RDWR))) >= 0)
   {
      if (config_file_set_value(fd, ac, av) != 0)
      {
         if(ppcStringReply != NULL)
         {
            *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
         }
         return( SHELL_EXECSTATUS_KO );
      }
      close (fd);

      return( SHELL_EXECSTATUS_OK );
   }
   else
   {
      eFsStatus = e_fscmds_CheckNavError(); // Get the fs error.
      if( ppcStringReply != NULL )
      {
         v_fscmds_GetStrMsgFromErr(eFsStatus, ppcStringReply);
      }
      return( SHELL_EXECSTATUS_KO );
   }
******/

}


/*! \brief The get time config command: get the config fields value of the time module
 *         Takes no parameter
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
   struct tm *pxDate;


   // NOTE: currently the only configurable item of the time module is the
   // current time. So, we're only returning the current time like this:
   // "curtime=mm/dd/yy hh:mm:ss\r\n" => 31 Bytes (including the \0 character).


   if(ppcStringReply != NULL)
   {
      // Allocate a buffer for answer
      *ppcStringReply = (signed portCHAR *)pvPortMalloc(31);
      if( NULL == *ppcStringReply )
      {
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
         return( SHELL_EXECSTATUS_KO );
      }

      // Get the broken-down representation of the current date.
      pxDate = gmtime( &xcptime_LocalTime );

      // WARNING: pxDate->tm_year == number of years since 1900.
      // For years >= 2000, we'll display the last 2 digits only.
      if( pxDate->tm_year >= 100 )  pxDate->tm_year -= 100;
      sprintf( (char *)*ppcStringReply, "curtime=%02d/%02d/%02d  %02d:%02d:%02d\r\n",
                pxDate->tm_mon +1, pxDate->tm_mday, pxDate->tm_year,
                pxDate->tm_hour, pxDate->tm_min, pxDate->tm_sec );

      /* no error, return */
      return( SHELL_EXECSTATUS_OK );
   }
   return( SHELL_EXECSTATUS_KO );
}

/************************************ PRIVATE ********************************/

/*!
 * \brief Shift the Scheduled Cmds array from the SrcIdx index down to the DstIdx index.
 *
 * \param SrcIdx   Input. The source index.
 * \param DstIdx   Input. The Destination index.
 *
 */
static void prv_v_ShiftScheduledCmdsArray( int SrcIdx, int DstIdx)
{


   if( SrcIdx == DstIdx )
      return;
   // Shift the array of scheduled commands.
   for( ;
        (SrcIdx<CPTIME_MAX_NB_SCHED_CMDS)&&(0 != axCmdsSched[SrcIdx].xScheduledTime);
        SrcIdx++,DstIdx++ )
   {
      axCmdsSched[DstIdx].xScheduledTime = axCmdsSched[SrcIdx].xScheduledTime;
      axCmdsSched[DstIdx].CmdId = axCmdsSched[SrcIdx].CmdId;
      axCmdsSched[DstIdx].pfScheduledCmd = axCmdsSched[SrcIdx].pfScheduledCmd;
      axCmdsSched[DstIdx].pxParam = axCmdsSched[SrcIdx].pxParam;
      // Mark this cell as unused now that its content has been moved.
      axCmdsSched[SrcIdx].xScheduledTime = 0;
   }
}

