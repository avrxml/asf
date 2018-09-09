/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel supervisor module.
 *
 * This module is in charge of the Ctrl Panel system.
 *
 *
 *                       The example is written for UC3 and EVK1100.
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


#ifndef __SUPERVISOR_H__
#define __SUPERVISOR_H__

#include "FreeRTOS.h"
#include "semphr.h"
#include "conf_usb.h"

/*!
 * Modules status.
 */
typedef enum
{
   SYS_STATUS_STARTUP,  // The module is being created.
   SYS_STATUS_RUNNING,  // The module is up & running.
   SYS_STATUS_DOWN      // The module is not running.
} eStatus;


/*!
 * Modules id.
 */
typedef enum
{
   SYS_MODID_NONE = -1,     // No specific module
   SYS_MODID_COM1SHELL = 1, // The COM1SHELL module.
   SYS_MODID_HTTP = 2,      // The HTTP module.
   SYS_MODID_USB = 4,       // The USB module.
   SYS_MODID_MMI = 8        // The MMI module.
} eModId;


/*! Number of modules id. */
#define SYS_NB_MOD   4


//_____ D E C L A R A T I O N S ____________________________________________

/*!
 * \brief Start the supervisor module.
 *
 * \param uxPriority The priority base of the supervisor tasks.
 */
void vSupervisor_Start( unsigned portBASE_TYPE uxPriority );

/*! \brief Take a system mutex.
 *
 *  \param xSemaphore   A handle to the semaphore being obtained.
 *  \param xBlockTime   The time in ticks to wait for the semaphore to become available.
 *
 *  \return pdTRUE if the semaphore was obtained. pdFALSE if xBlockTime expired without the semaphore becoming available.
 */
portBASE_TYPE x_supervisor_SemaphoreTake( xSemaphoreHandle xSemaphore,portTickType xBlockTime );


/*! \brief Release a system mutex.
 *
 *  \param xSemaphore   A handle to the semaphore being released.
 *
 *  \return pdTRUE if the semaphore was released. pdFALSE if an error occurred.
 */
portBASE_TYPE x_supervisor_SemaphoreGive( xSemaphoreHandle xSemaphore );


/*!
 *  \brief Called upon remote host connect (upon VBUS ON event when we are device)
 */
void v_supervisor_Host_Connects( void );

/*!
 *  \brief Called upon remote host disconnection.
 */
void v_supervisor_Host_Disconnects( void );

/*!
 *  \brief Called upon remote device connection (upon PIN ID CHANGE event when we are host)
 */
void v_supervisor_Device_Connects( void );

/*!
 *  \brief Called upon remote device disconnection.
 */
void v_supervisor_Device_Disconnects( void );


#ifdef USB_ENABLE
bool b_USBHostCopyLogs( void );
bool b_USBHostMoveLogs( void );
bool b_USBHostCopyWeb( void );
bool b_USBHostCopyCfg( void );

#ifdef USB_DEVICE_FEATURE
#if USB_DEVICE_FEATURE == true
bool b_supervisor_switch_to_maintenance_mode( void );
/*!
 *  \brief Leave the waiting-for-host mode while the USB host has not been plugged in yet.
 */
bool b_supervisor_leave_waitingforhost_mode( void );


/*!
 *  \brief Leave the USB Mass Storage mode while the USB host is plugged in.
 *
 */
bool b_supervisor_leave_UsbMassStorage_mode( void );

/*!
 * \brief Maintenance mode status.
 *
 * \return true if the maintenance mode is active else false.
 */
bool b_supervisor_IsInMaintenanceMode( void );

#endif
#endif

#endif

#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_supervisor_trace( void );
#endif

/*############################# TEMPORARY ###############################*/

/*!
 *  \brief The switch-to-maintenance-mode command.
 *         Format: maintain
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Ignored.
 *  \param av             Ignored.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
/* eExecStatus e_supervisor_switch_to_maintenance_mode( eModId xModId,
                              signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply );
*/
/*#######################################################################*/

#endif
