/**
* \file  main.c
*
* \brief Main file for WSN Demo Example on MiWi Mesh.
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

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the WSN Demo Application
* The WSNDemo application implements a typical wireless sensor network scenario,
* in which one central node collects the data from a network of sensors and
* passes this data over a serial connection for further processing.
* In the case of the WSNDemo this processing is performed by the WSNMonitor PC
* application. The MiWi™ Quick Start Guide  provides a detailed description
* of the WSNDemo application scenario, and instructions on how to use WSNMonitor.
* <P>• Device types (PAN Coordinator, Coordinator and End Device) and its address in 
* MiWi™ Mesh network is displayed on the nodes.</P>
* <P>• The value of the extended address field is set equal to the value of the
* short address field.</P>
* <P>• For all frames, the LQI and RSSI fields are filled in by the coordinator
* with the values of LQI and RSSI from the received frame. This means that nodes
* that are not connected to the coordinator directly will have the same values
* as the last node on the route to the coordinator.</P>
* <P>• Sensor data values are generated randomly on all platforms.</P>
* <P>• Sending data to the nodes can be triggered when the light button on the 
* node is clicked. This also blinks the LED in node.
* </P>
*/


/************************ HEADERS ****************************************/
#include "asf.h"
#include "sio2host.h"
#include "wsndemo.h"
#include "miwi_api.h"

#if ((BOARD == SAMR30_XPLAINED_PRO) || (BOARD == SAMR21_XPLAINED_PRO))
#include "edbg-eui.h"
#endif

#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsMemIds.h"
#include "pdsDataServer.h"
#include "wlPdsTaskManager.h"
#endif

#if defined(OTAU_ENABLED)
#include "otau.h"
#endif

#if defined(PAN_COORDINATOR) && defined(OTAU_SERVER)
#error "PAN Coordinator in WSN Demo cannot be OTAU Server, as both needs Serial to communicate with Tool..."
#endif

/************************** DEFINITIONS **********************************/
#if (BOARD == SAMR21ZLL_EK)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x00804008U))
#endif

#if (BOARD == SAMR30_MODULE_XPLAINED_PRO)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x0080400AU))
#endif

/************************** PROTOTYPES **********************************/
void ReadMacAddress(void);

/*********************************************************************
* Function:         void main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    This is the main function that runs the simple 
*                   example demo. The purpose of this example is to
*                   demonstrate the simple application programming
*                   interface for the MiWi(TM) Development 
*                   Environment. By virtually total of less than 30 
*                   lines of code, we can develop a complete 
*                   application using MiApp interface. The 
*                   application will first try to establish a
*                   link with another device and then process the 
*                   received information as well as transmit its own 
*                   information.
*                   MiWi(TM) DE also support a set of rich 
*                   features. Example code FeatureExample will
*                   demonstrate how to implement the rich features 
*                   through MiApp programming interfaces.
*
* Note:			    
**********************************************************************/
int main ( void )
{
	irq_initialize_vectors();

#if SAMD || SAMR21 || SAML21 || SAMR30
	system_init();
	delay_init();
#else
	sysclk_init();
	board_init();
#endif

	cpu_irq_enable();	

#if defined (ENABLE_LCD)	
	LCD_Initialize();
#endif
	
	/* Read the MAC address from either flash or EDBG */
	ReadMacAddress();
	
    /* Initialize system Timer */
    SYS_TimerInit();

#if defined(ENABLE_NETWORK_FREEZER)
    nvm_init(INT_FLASH);
    PDS_Init();
#endif

    /* Initialize the demo */
	wsndemo_init();

    while(1)
    {
		wsndemo_task();
#if defined(OTAU_ENABLED)
		otauTask();
#endif
    }
}

/*********************************************************************
* Function:         void ReadMacAddress()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    Reads MAC Address from MAC Address EEPROM
*
* Side Effects:	    none
*
* Overview:		    Uses the MAC Address from the EEPROM for addressing
*
* Note:			    
**********************************************************************/
void ReadMacAddress(void)
{
#if ((BOARD == SAMR21ZLL_EK) || (BOARD == SAMR30_MODULE_XPLAINED_PRO))
   uint8_t i = 0, j = 0;
   for (i = 0; i < 8; i += 2, j++)
   {
     myLongAddress[i] = (NVM_UID_ADDRESS[j] & 0xFF);
	 myLongAddress[i + 1] = (NVM_UID_ADDRESS[j] >> 8);
   }
#elif ((BOARD == SAMR30_XPLAINED_PRO) || (BOARD == SAMR21_XPLAINED_PRO))
   uint8_t* peui64 = edbg_eui_read_eui64();
	for(uint8_t k=0; k<MY_ADDRESS_LENGTH; k++)
   {
		myLongAddress[k] = peui64[MY_ADDRESS_LENGTH-k-1];
   }
#endif
}

