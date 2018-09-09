/**
* \file  task.h
*
* \brief Interface of Tasks for Demo Application on MiWi Star 
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#ifndef TASK_H
#define	TASK_H

#include "system.h"

#include "miwi_api.h"
#include "demo_output.h"



/*********************************************************************
* Function: void freezer_feature(void)
*
* Overview: Wait for User Inputs from Switches on Board 
*
* PreCondition: Hardware_Init()
*
* Input:  None
*
* Output:Boolean True : New Network --False: Restore the Old Network Stored on EEPROM
 *                                     // False : Using Network Freezer Feature
*
********************************************************************/
bool freezer_feature(void);
/*********************************************************************
* Function: void Initialize_Demo(void)
*
* Overview: MAC Layer Protocol Initialization , Ch selection and Network Roles 
 *                  Pan Co or End Device In case of Star
 *              
*
* PreCondition: Hardware_Init()
*
* Input:  None
*
* Output:None 
*           
********************************************************************/
bool Initialize_Demo(bool);
/*********************************************************************
* Function: void Run_Demo(void)
*
* Overview: Start P2p or Star Demo depending on the protocol choosen in 
 *                  miwi_config.h
*
* PreCondition: Hardware_Init && Demo_Init
*
* Input:  None
*
* Output:None 
*           
********************************************************************/
void Run_Demo(void);

/*********************************************************************
* Function: void Run_Demo(void)
*
* Overview: Display the PAN ID , channel and its 3 byte long address 
*
* PreCondition: None 
*
* Input:  None
*
* Output:None 
*           
********************************************************************/
void dev_info(void);
/*********************************************************************
* Function: bool Time_out(MIWI_TICK a, MIWI_TICK b , no_of_seconds);
*
* Overview: User Defined Time out
*
* PreCondition: hardware_init 
*
* Input:  Time a and b , no_of_seconds :TimeDifference > how many seconds ?
*
* Output:true : if Timeout , false : if Timeout condition not met  
*           
********************************************************************/
bool Time_out(MIWI_TICK, MIWI_TICK , uint8_t);

#endif	/* TASK_H */

