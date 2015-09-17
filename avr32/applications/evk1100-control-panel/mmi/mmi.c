/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel MMI module.
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



#include <stdlib.h>
#include <string.h>

#include "board.h"

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ctrl_access.h"
#include "conf_access.h"
#include "navigation.h"

#include "tracedump.h"
#include "shell.h"
#include "fscmds.h"
#include "supervisor.h"

#include "mmi.h"
#include "dip204.h"
#include "spi.h"

#include "cptime.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________
/*! number of chars per line */
#define MMI_LINE_LENGTH     20
/*! number of lines on display */
#define MMI_NB_LINE         4
/*! definition of the MMI queue size */
#define MMI_QUEUE_SIZE      MMI_NB_LINE
/*! this is the first line */
#define LINE_1              0x01
/*! this is the second line */
#define LINE_2              0x02
/*! this is the third line */
#define LINE_3              0x03
/*! this is the fourth line */
#define LINE_4              0x04
/*! Link date line to the first one */
#define DATE_LINE           LINE_1
/*! Link param line to the second one */
#define PARAM_LINE          LINE_2
/*! Link User Menu line to the third one */
#define USER_MENU_LINE      LINE_3
/*! Link User Mess line to the fourth one */
#define USER_MESS_LINE      LINE_4

/*! define the char used upon USB connection */
#define MMI_USB_CONNECTED      0xB7
/*! define the White space char */
#define MMI_SPACE              0x20

/*! number of items in USB host Menu */
#define MMI_USB_HOST_MENU_MAX_ITEM      4
/*! number of items in USB Device Menu */
#define MMI_USB_DEVICE_MENU_MAX_ITEM    1
/*! number of items in Idle Menu */
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
// Overwrite the previous define on purpose.
#define MMI_IDLE_MENU_MAX_ITEM          7
#else
#define MMI_IDLE_MENU_MAX_ITEM          6
#endif
#else
#define MMI_IDLE_MENU_MAX_ITEM          6
#endif
/*! number of items in Waiting-for-host Menu */
#define MMI_WAIT_MENU_MAX_ITEM          1

/*! Banner for ATMEL product */
#define ATMEL_BANNER                    "  ATMEL  AVR32 UC3  "

/*! definition of the User action function pointer */
typedef bool (*pfUserAction) (void);

//_____ D E C L A R A T I O N S ____________________________________________
#if USB_DEVICE_FEATURE != true
/*! Function to do nothing */
bool b_MMI_do_nothing( void );
#endif

#ifndef USB_ENABLE
/*! Function to do nothing */
bool b_MMI_do_nothing( void );
#endif

static bool b_mmi_format_a( void );    // FORWARD
static bool b_mmi_mkdir_aLOG( void );  // FORWARD
static bool b_mmi_format_b( void );    // FORWARD
static bool b_mmi_mkdir_bLOG( void );  // FORWARD
bool b_MMI_do_nothing( void );

#ifdef USB_ENABLE
/*! lines to display in USB Host menu */
const portCHAR *const MMI_USBHostMenu[MMI_USB_HOST_MENU_MAX_ITEM] = {
"\x20 Copy Logs        \x10",
"\x11 Move Logs        \x10",
"\x11 Upload WEB Files \x10",
"\x11 Upload CFG Files \x20"
};

/*! lines to display in USB Host menu */
const portCHAR *const MMI_USBHostActingMenu[MMI_USB_HOST_MENU_MAX_ITEM] = {
"Copying Logs        ",
"Moving Logs         ",
"Uploading WEB Files ",
"Uploading CFG Files "
};

/*! functions to call in USB Host menu */
pfUserAction const pfUSBHostUserAction[MMI_USB_HOST_MENU_MAX_ITEM] = {
b_USBHostCopyLogs,
b_USBHostMoveLogs,
b_USBHostCopyWeb,
b_USBHostCopyCfg
};

#if USB_DEVICE_FEATURE == true
/*! lines to display in USB Device menu */
const portCHAR *const MMI_USBDeviceMenu[MMI_USB_DEVICE_MENU_MAX_ITEM] = {
"USB Mass Storage    "
};

/*! functions to call in USB Device menu */
pfUserAction const pfUSBDeviceUserAction[MMI_USB_DEVICE_MENU_MAX_ITEM] = {
b_supervisor_leave_UsbMassStorage_mode
};
#endif

#endif

/*! lines to display in Idle menu */
const portCHAR *const MMI_IdleMenu[MMI_IDLE_MENU_MAX_ITEM] = {
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
"\x20 Set as USB key   \x10",
"\x11 Format a:(DF)    \x10",
"\x11 Create a:/LOG    \x10",
"\x11 Log to a:(DF)    \x10",
"\x11 Format b:(SDMMC) \x10",
"\x11 Create b:/LOG    \x10",
"\x11 Log to b:(SDMMC) \x20"
#else
"\x20 Format a:(DF)    \x10",
"\x11 Create a:/LOG    \x10",
"\x11 Log to a:(DF)    \x10",
"\x11 Format b:(SDMMC) \x10",
"\x11 Create b:/LOG    \x10",
"\x11 Log to b:(SDMMC) \x20"
#endif
#else
"\x20 Format a:(DF)    \x10",
"\x11 Create a:/LOG    \x10",
"\x11 Log to a:(DF)    \x10",
"\x11 Format b:(SDMMC) \x10",
"\x11 Create b:/LOG    \x10",
"\x11 Log to b:(SDMMC) \x20"
#endif
};

/*! lines to display in Idle menu when an action is performed. */
const portCHAR *const MMI_IdleActingMenu[MMI_IDLE_MENU_MAX_ITEM] = {
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
"", // No message, it is taken care of in another menu mode (the eUserMenuWaitHost menu).
"Formating a:        ",
"Creating a:/LOG     ",
"LOGS set to DF      ",
"Formating b:        ",
"Creating b:/LOG     ",
"LOGS set to SDMMC   "
#else
"Formating a:        ",
"Creating a:/LOG     ",
"LOGS set to DF      ",
"Formating b:        ",
"Creating b:/LOG     ",
"LOGS set to SDMMC   "
#endif
#else
"Formating a:        ",
"Creating a:/LOG     ",
"LOGS set to DF      ",
"Formating b:        ",
"Creating b:/LOG     ",
"LOGS set to SDMMC   "
#endif
};

/*! functions to call in Idle menu */
pfUserAction const pfIdleUserAction[MMI_IDLE_MENU_MAX_ITEM] = {
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
b_supervisor_switch_to_maintenance_mode,
#endif
#endif
b_mmi_format_a,
b_mmi_mkdir_aLOG,
b_datalog_set_current_log_drive_toDF,
b_mmi_format_b,
b_mmi_mkdir_bLOG,
b_datalog_set_current_log_drive_toSDMMC
};

#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*! lines to display in Waiting-for-host menu */
const portCHAR *const MMI_WaitHostMenu[MMI_WAIT_MENU_MAX_ITEM] = {
MMI_MAINTAINER_MODE
};

/*! functions to call in Waiting-for-host menu */
pfUserAction const pfWaitHostUserAction[MMI_WAIT_MENU_MAX_ITEM] = {
b_supervisor_leave_waitingforhost_mode
};
#endif
#endif

/*! memory image of the display */
portCHAR Line[MMI_NB_LINE][MMI_LINE_LENGTH + 1];

/*! pointer to the memory image of the param line */
portCHAR * ParamScreen = Line[PARAM_LINE - 1];
/*! pointer to the memory image of the date line */
portCHAR * DateScreen = Line[DATE_LINE - 1];
/*! pointer to the memory image of the User Menu line */
portCHAR * UserMenuScreen = Line[USER_MENU_LINE - 1];
/*! pointer to the memory image of the User Mess line */
portCHAR * UserMessScreen = Line[USER_MESS_LINE - 1];

/*! current MMI user menu to handle */
eUserMenu eCurrentUserMenu = eUserMenuIdle;

/*! current item in user menu */
portCHAR cMMI_SubMenu_CurrentItem = 0;

/*! The handle of the queue of MMI. */
static xQueueHandle xMMIQueue = 0;

/*! Temporary array to set the creation date of the LOG directory. */
static char      pcTempoDate[17];

/*! The handle of the queue of SUPERVISOR. */
extern xQueueHandle xSUPERVISORQueue;

/*! MMI navigator ID. */
extern signed short sMmiNavId;

static void prvMMI_Init(void); // Forward declaration

/*! The "LOG" string. */
const signed portCHAR *const pcStringLogDirName = (signed portCHAR *)"LOG";

/*! The \<drive letter>:/LOG strings. */
extern const signed portCHAR *const pcStringLogDirectoryNameA;
extern const signed portCHAR *const pcStringLogDirectoryNameB;

/*-----------------------------------------------------------*/
/*! \brief Init MMI, for Man to Machine Interface management.
 *
 */
bool bMMI_start( void )
{
unsigned short i;

  for (i = 0 ; i < MMI_NB_LINE ; i++)
  {
    memset(Line[i], MMI_SPACE, MMI_LINE_LENGTH);
    Line[i][MMI_LINE_LENGTH] = '\0';
  }

  xMMIQueue = xQueueCreate( MMI_QUEUE_SIZE, sizeof(portCHAR *) );

  if( 0 == xMMIQueue )
  {
     return pdFALSE;
  }

  // Init MMI
  prvMMI_Init();

  /* go to Idle state */
  vMMI_SetUserMenuMode(eUserMenuIdle, pdTRUE);
  vMMI_UserMessDisplay(ATMEL_BANNER);
  return pdTRUE;
}

/*! \brief display IP in Param space
 *
 *  \param IPAddress   Input. IP to display
 *
 */
void vMMI_DisplayIP(portCHAR * IPAddress)
{
unsigned short i = 0;
  // clear previous line but not the last char (we should be connected)
  memset(UserMenuScreen, MMI_SPACE, MMI_LINE_LENGTH - 1);
  // set new value
  do
  {
    ParamScreen[i++] = *IPAddress++;
  }while (*IPAddress != '\0' && i < 15);
  // Add the refresh need to the xMMIQueue.
  xQueueSend( xMMIQueue, (void *)&ParamScreen, 0 );
}


/*! \brief display USB connection status.
 *
 *  \param bConnected   Input. USB connection status.
 *
 */
void vMMI_DisplayUSBState(bool bConnected)
{
  if (bConnected == pdTRUE)
  {
    ParamScreen[MMI_LINE_LENGTH - 1] = MMI_USB_CONNECTED;
  }
  else
  {
    ParamScreen[MMI_LINE_LENGTH - 1] = MMI_SPACE;
  }
  // Add the refresh need to the xMMIQueue.
  xQueueSend( xMMIQueue, (void *)&ParamScreen, 0 );
}


/*! \brief display Date in Date space
 *
 *  \param pcDateTime        Input. date to display
 *
 */
void vMMI_DisplayDateAndTime(portCHAR * pcDateTime)
{
  strcpy(DateScreen, pcDateTime);
  // Add the refresh need to the xMMIQueue.
  xQueueSend( xMMIQueue, (void *)&DateScreen, 0 );
}


/*! \brief Validate the current item in User Menu
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuValidateItem(bool UnderIT)
{
  portBASE_TYPE xYieldRequired = pdFALSE;
#ifdef USB_ENABLE
  // HOST mode : we have something to do
  if (eCurrentUserMenu == eUserMenuUSBHost)
  {
    // caller is under IT
    if (UnderIT)
    {
      portENTER_CRITICAL();
      // Add the function need to the xSUPERVISORQueue.
      xQueueSendFromISR( xSUPERVISORQueue, (void *)&(pfUSBHostUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), &xYieldRequired );
      portEXIT_CRITICAL();
    }
    else
    {
      // Add the function need to the xSUPERVISORQueue.
      xQueueSend( xSUPERVISORQueue, (void *)&(pfUSBHostUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), 0 );
    }
  }
  // USB Mass Storage mode
  else if (eCurrentUserMenu == eUserMenuUSBDevice)
  {
    // Caller is under IT (joystick press)
    if (UnderIT)
    {
      portENTER_CRITICAL();
      // Add the function need to the xSUPERVISORQueue.
      xQueueSendFromISR( xSUPERVISORQueue, (void *)&(pfUSBDeviceUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), &xYieldRequired );
      portEXIT_CRITICAL();
    }
  }
  // Waiting-for-host mode
  else if (eCurrentUserMenu == eUserMenuWaitHost)
  {
    // caller is under IT
    if (UnderIT)
    {
      portENTER_CRITICAL();
      // Add the function need to the xSUPERVISORQueue.
      xQueueSendFromISR( xSUPERVISORQueue, (void *)&(pfWaitHostUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), &xYieldRequired );
      portEXIT_CRITICAL();
    }
    else
    {
      // Add the function need to the xSUPERVISORQueue.
      xQueueSend( xSUPERVISORQueue, (void *)&(pfWaitHostUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), 0 );
    }
  }
  // Idle mode : we have something to do
  else if (eCurrentUserMenu == eUserMenuIdle)
#else
  // Idle mode : we have something to do
  if (eCurrentUserMenu == eUserMenuIdle)
#endif
  {
    // caller is under IT
    if (UnderIT)
    {
      portENTER_CRITICAL();
      // Add the function need to the xSUPERVISORQueue.
      xQueueSendFromISR( xSUPERVISORQueue, (void *)&(pfIdleUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), &xYieldRequired );
      portEXIT_CRITICAL();
    }
    else
    {
      // Add the function need to the xSUPERVISORQueue.
      xQueueSend( xSUPERVISORQueue, (void *)&(pfIdleUserAction[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]), 0 );
    }
  }
  // other mode : nothing to do...
  else
  {
    return;
  }
}

/*! \brief set MMI current Menu
 *
 *  \param UserMenu           Input. User Menu to handle
 *  \param ResetCurrentState  Input. pdFALSE if no need to reset MMI current screen number
 *
 */
void vMMI_SetUserMenuMode(eUserMenu UserMenu, bool ResetCurrentState)
{
  if (ResetCurrentState)
  {
    // reset the screen number to display
    cMMI_SubMenu_CurrentItem = 0;
  }
#ifdef USB_ENABLE
  // HOST menu
  if (UserMenu == eUserMenuUSBHost)
  {
    // set current user menu
    eCurrentUserMenu = eUserMenuUSBHost;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBHostMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  // HOST Acting menu
  else if (UserMenu == eUserMenuUSBHostActing)
  {
    // set current user menu
    eCurrentUserMenu = eUserMenuUSBHostActing;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBHostActingMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  // DEVICE menu
#if USB_DEVICE_FEATURE == true
  else if (UserMenu == eUserMenuUSBDevice)
  {
    // set current user menu
    eCurrentUserMenu = eUserMenuUSBDevice;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBDeviceMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  else if (UserMenu == eUserMenuWaitHost)
  {
    // set current user menu
    eCurrentUserMenu = eUserMenuWaitHost;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_WaitHostMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#endif
  else if( UserMenu == eUserMenuIdle )
#endif
  {
    // reset current user menu
    eCurrentUserMenu = eUserMenuIdle;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_IdleMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  else // eUserMenuIdleActing
  {
    // reset current user menu
    eCurrentUserMenu = eUserMenuIdleActing;
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_IdleActingMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  // Add the refresh need to the xMMIQueue.
  xQueueSend( xMMIQueue, (void *)&UserMenuScreen, 0 );
}


/*! \brief display next screen in User Menu space
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuDisplayNextItem(bool UnderIT)
{
#ifdef USB_ENABLE
  // HOST menu
  if (eCurrentUserMenu == eUserMenuUSBHost)
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Min(cMMI_SubMenu_CurrentItem + 1, MMI_USB_HOST_MENU_MAX_ITEM - 1);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBHostMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#if USB_DEVICE_FEATURE == true
  // DEVICE menu
  else if (eCurrentUserMenu == eUserMenuUSBDevice)
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Min(cMMI_SubMenu_CurrentItem + 1, MMI_USB_DEVICE_MENU_MAX_ITEM - 1);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBDeviceMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#endif
  else
#endif
  if( eCurrentUserMenu == eUserMenuIdle )
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Min(cMMI_SubMenu_CurrentItem + 1, MMI_IDLE_MENU_MAX_ITEM - 1);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_IdleMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#ifdef USB_ENABLE
  // caller is under IT
  if (UnderIT)
  {
    portBASE_TYPE xYieldRequired = pdFALSE;
    portENTER_CRITICAL();
    // Add the refresh need to the xMMIQueue.
    xQueueSendFromISR( xMMIQueue, (void *)&UserMenuScreen, &xYieldRequired );
    portEXIT_CRITICAL();
  }
  else
  {
    // Add the refresh need to the xMMIQueue.
    xQueueSend( xMMIQueue, (void *)&UserMenuScreen, 0 );
  }
#endif
}


/*! \brief display previous screen in User Menu space
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuDisplayPreviousItem(bool UnderIT)
{
#ifdef USB_ENABLE
  // HOST menu
  if (eCurrentUserMenu == eUserMenuUSBHost)
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Max(cMMI_SubMenu_CurrentItem - 1, 0);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBHostMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
  // DEVICE menu
#if USB_DEVICE_FEATURE == true
  else if (eCurrentUserMenu == eUserMenuUSBDevice)
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Max(cMMI_SubMenu_CurrentItem - 1, 0);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_USBDeviceMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#endif
  else
#endif
  if( eCurrentUserMenu == eUserMenuIdle )
  {
    // set the value for the screen number to display
    cMMI_SubMenu_CurrentItem = Max(cMMI_SubMenu_CurrentItem - 1, 0);
    // prepare the line for the display
    strcpy(UserMenuScreen, MMI_IdleMenu[(unsigned portCHAR)cMMI_SubMenu_CurrentItem]);
  }
#ifdef USB_ENABLE
  // caller is under IT
  if (UnderIT)
  {
    portBASE_TYPE xYieldRequired = pdFALSE;
    portENTER_CRITICAL();
    // Add the refresh need to the xMMIQueue.
    xQueueSendFromISR( xMMIQueue, (void *)&UserMenuScreen, &xYieldRequired );
    portEXIT_CRITICAL();
  }
  else
  {
    // Add the refresh need to the xMMIQueue.
    xQueueSend( xMMIQueue, (void *)&UserMenuScreen, 0 );
  }
#endif
}


/*! \brief display message in User Menu space
 *
 *  \param Message   Input. Message to display
 *
 */
void vMMI_UserMenuDisplay(portCHAR * Message)
{
unsigned short i = 0;

  // if MMI is ini IDLE state, display the message
  if (eCurrentUserMenu == eUserMenuIdle)
  {
    // add the received data to the line
    do
    {
      UserMenuScreen[i++] = *Message++;
    }while (*Message != '\0' && i < MMI_LINE_LENGTH);
    // fill in with white spaces to clear older data
    do
    {
      UserMenuScreen[i++] = MMI_SPACE;
    }while (i < MMI_LINE_LENGTH);
    // add EOL
    UserMenuScreen[MMI_LINE_LENGTH] = '\0';
     // Add the refresh need to the xMMIQueue.
    xQueueSend( xMMIQueue, (void *)&UserMenuScreen, 0 );
  }
}


/*! \brief display message in User Mess space
 *
 *  \param Message   Input. Message to display
 *
 */
void vMMI_UserMessDisplay(portCHAR * Message)
{
unsigned short i = 0;

  // add the received data to the line
  do
  {
    UserMessScreen[i++] = *Message++;
  }while (*Message != '\0' && i < MMI_LINE_LENGTH);
  // fill in with white spaces to clear older data
  do
  {
    UserMessScreen[i++] = MMI_SPACE;
  }while (i < MMI_LINE_LENGTH);
  // add EOL
  UserMessScreen[MMI_LINE_LENGTH] = '\0';
  // Add the refresh need to the xMMIQueue.
  xQueueSend( xMMIQueue, (void *)&UserMessScreen, 0 );
}


/*! \brief MMI function for Man to Machine Interface management.
 *
 */
void vMMI_Manage( void )
{
portCHAR * Line;

  // get queue information
  while ( pdTRUE == xQueueReceive( xMMIQueue, &Line, ( portTickType ) 0 ) )
  {
    // refresh line Param
    if (Line == ParamScreen)
    {
      dip204_set_cursor_position(1,PARAM_LINE);
      dip204_write_string(ParamScreen);
    }
    // refresh line Date
    else if (Line == DateScreen)
    {
      dip204_set_cursor_position(1,DATE_LINE);
      dip204_write_string(DateScreen);
    }
    // refresh line User Menu
    else if (Line == UserMenuScreen)
    {
      dip204_set_cursor_position(1,USER_MENU_LINE);
      dip204_write_string(UserMenuScreen);
    }
    // refresh line User Mess
    else if (Line == UserMessScreen)
    {
      dip204_set_cursor_position(1,USER_MESS_LINE);
      dip204_write_string(UserMessScreen);
    }
  }
}



/*! \brief MMI function for initialisation.
 *
 */
static void prvMMI_Init( void )
{
  spi_options_t spiOptions =
  {
    .reg          = DIP204_SPI_NPCS,
    .baudrate     = 1000000,
    .bits         = 8,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

#if BOARD == EVK1100
//#warning DIP204 and AT45DBX are sharing the same SPI (but both CS pins are different)
  // Only assign CS I/Os to SPI
  gpio_enable_module_pin(DIP204_SPI_NPCS_PIN,  DIP204_SPI_NPCS_FUNCTION); // Chip Select NPCS

#else
  static const gpio_map_t DIP204_SPI_GPIO_MAP =
  {
    {DIP204_SPI_SCK_PIN,  DIP204_SPI_SCK_FUNCTION },  // SPI Clock.
    {DIP204_SPI_MISO_PIN, DIP204_SPI_MISO_FUNCTION},  // MISO.
    {DIP204_SPI_MOSI_PIN, DIP204_SPI_MOSI_FUNCTION},  // MOSI.
    {DIP204_SPI_NPCS_PIN, DIP204_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

  // Assign I/Os to SPI
  gpio_enable_module(DIP204_SPI_GPIO_MAP,
                     sizeof(DIP204_SPI_GPIO_MAP) / sizeof(DIP204_SPI_GPIO_MAP[0]));

  // Initialise as master
  spi_initMaster(DIP204_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay
  spi_selectionMode(DIP204_SPI, 0, 0, 0);

  // Enable SPI
  spi_enable(DIP204_SPI);
#endif

  // setup chip registers
  spi_setupChipReg(DIP204_SPI, &spiOptions, CP_PBA_SPEED);

  /* initialise LCD */
  dip204_init( backlight_PWM, true ); // Use the PWM to control the LCD backlight pin.

  /* hide the cursor for the rest of application */
  dip204_hide_cursor();
}

/*! \brief function to do nothing upon user action
 *
 */
bool b_MMI_do_nothing( void )
{
  NAKED_TRACE_COM2( "Nothing to do for this action" );
  return (pdTRUE);
}

/*!
 *  \brief Format the a drive.
 *
 */
static bool b_mmi_format_a( void )
{
  bool bRet = pdTRUE;


  // Display User Menu Acting
  vMMI_SetUserMenuMode(eUserMenuIdleActing, pdFALSE);
  vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
  // WARNING : we assume LUN_ID_AT45DBX_MEM <-> a
  if( FSCMDS_STATUS_OK != e_fscmds_format( sMmiNavId, LUN_ID_AT45DBX_MEM ) )
    bRet = pdFALSE;
  // Back to display User Menu
  vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
  return( bRet );
}


/*!
 *  \brief Format the b drive.
 *
 */
static bool b_mmi_format_b( void )
{
  bool bRet = pdTRUE;


  // Display User Menu Acting
  vMMI_SetUserMenuMode(eUserMenuIdleActing, pdFALSE);
  vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
  // WARNING : we assume LUN_ID_SD_MMC_MEM <-> b
  if( FSCMDS_STATUS_OK != e_fscmds_format( sMmiNavId, LUN_ID_SD_MMC_SPI_MEM ) )
    bRet = pdFALSE;
  // Back to display User Menu
  vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
  return( bRet );
}


/*!
 *  \brief Create the /LOG directory under a.
 *
 */
static bool b_mmi_mkdir_aLOG( void )
{
  bool             bRet = pdFALSE;


  // Display User Menu Acting
  vMMI_SetUserMenuMode(eUserMenuIdleActing, pdFALSE);
  vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
  nav_select( sMmiNavId );
  // WARNING : we assume LUN_ID_AT45DBX_MEM <-> a
  if( !nav_drive_set( LUN_ID_AT45DBX_MEM ))
  {
    // Back to display User Menu
    vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
    return( bRet );
  }
  if( !nav_partition_mount() )
  {
    // Back to display User Menu
    vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
    return( bRet );
  }
  // Here, Pwd == a:
  if( SHELL_EXECSTATUS_OK == e_fscmds_shell_mkdir( SYS_MODID_MMI, sMmiNavId, 1,
                                                   (signed portCHAR **)&pcStringLogDirName, NULL ) )
    bRet = pdTRUE;
  // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
  v_cptime_GetDateInFatStringFormat( pcTempoDate );
  // Set the directory date.
  nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );
  // Back to display User Menu
  vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
  return( bRet );
}


/*!
 *  \brief Create the /LOG directory under b.
 *
 */
static bool b_mmi_mkdir_bLOG( void )
{
  bool             bRet = pdFALSE;


  // Display User Menu Acting
  vMMI_SetUserMenuMode(eUserMenuIdleActing, pdFALSE);
  vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
  nav_select( sMmiNavId );
  // WARNING : we assume LUN_ID_SD_MMC_MEM <-> b
  if( !nav_drive_set( LUN_ID_SD_MMC_SPI_MEM ))
  {
    // Back to display User Menu
    vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
    return( bRet );
  }
  if( !nav_partition_mount() )
  {
    // Back to display User Menu
    vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
    return( bRet );
  }
  // Here, Pwd == a:
  if( SHELL_EXECSTATUS_OK == e_fscmds_shell_mkdir( SYS_MODID_MMI, sMmiNavId, 1,
                                                   (signed portCHAR **)&pcStringLogDirName, NULL ) )
    bRet = pdTRUE;
  // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
  v_cptime_GetDateInFatStringFormat( pcTempoDate );
  // Set the directory date.
  nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );
  // Back to display User Menu
  vMMI_SetUserMenuMode(eUserMenuIdle, pdFALSE);
  return( bRet );
}
