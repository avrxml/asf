/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel MMI module.
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

#ifndef __MMI_H__
#define __MMI_H__


/****************************************************************************
on a DIP204 LCD, the display is as follow :
  Line   Space Name          1<----Display---->20
   1    "DateScreen"         MM/DD/YYYY     HH:MM
   2    "ParamScreen"        XXX.XXX.XXX.XXX    Y (when USB connected)
   3    "UserMenuScreen"     < blablabla        > (when other item available)
   4    "UserMessScreen"     blablablablablablabl
****************************************************************************/

/*! types for User Menu screen */
typedef enum {
eUserMenuIdle = 0,
eUserMenuIdleActing,
eUserMenuWaitHost,
eUserMenuUSBHost,
eUserMenuUSBHostActing,
eUserMenuUSBDevice
}eUserMenu;

#define MMI_MAINTAINER_MODE             "Waiting for Host    "

/*! \brief Init MMI, for Man to Machine Interface management.
 *
 */
bool bMMI_start( void );

/*! \brief MMI function for Man to Machine Interface management.
 *
 */
void vMMI_Manage( void );

/*! \brief display IP in Param space
 *
 *  \param IPAddress   Input. IP to display
 *
 */
void vMMI_DisplayIP(portCHAR * IPAddress);

/*! \brief display USB connection status.
 *
 *  \param bConnected   Input. USB connection status.
 *
 */
void vMMI_DisplayUSBState(bool bConnected);


/*! \brief display Date in Date space
 *
 *  \param pcDateTime        Input. date to display
 *
 */
void vMMI_DisplayDateAndTime(portCHAR * pcDateTime);


/*! \brief display next screen in User Menu space
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuDisplayNextItem(bool UnderIT);


/*! \brief display previous screen in User Menu space
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuDisplayPreviousItem(bool UnderIT);


/*! \brief set MMI current Menu
 *
 *  \param UserMenu           Input. User Menu to handle
 *  \param ResetCurrentState  Input. pdFALSE if no need to reset MMI current screen number
 *
 */
void vMMI_SetUserMenuMode(eUserMenu UserMenu, bool ResetCurrentState);

/*! \brief display message in User Menu space
 *
 *  \param Message   Input. Message to display
 *
 */
void vMMI_UserMenuDisplay(portCHAR * Message);

/*! \brief Validate the current item in User Menu
 *
 *  \param UnderIT   Input. True if calling from IT
 *
 */
void vMMI_UserMenuValidateItem(bool UnderIT);



/*! \brief display message in User Mess space
 *
 *  \param Message   Input. Message to display
 *
 */
void vMMI_UserMessDisplay(portCHAR * Message);

#endif


