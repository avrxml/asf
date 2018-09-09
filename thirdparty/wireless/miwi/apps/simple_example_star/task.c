/**
* \file  task.c
*
* \brief Implementation of Tasks for Demo Application on MiWi Star*
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
/***********************Headers***************************************/
#include "task.h"
#include "star_demo.h"
#include "asf.h"

#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsDataServer.h"
#include "wlPdsTaskManager.h"
#endif

#if defined(ENABLE_SLEEP_FEATURE)
#include "sleep_mgr.h"
#endif
#include "phy.h"
/************************** VARIABLES ************************************/
#define LIGHT   0x01
#define SWITCH  0x02

/*************************************************************************/
// AdditionalNodeID variable array defines the additional 
// information to identify a device on a PAN. This array
// will be transmitted when initiate the connection between 
// the two devices. This  variable array will be stored in 
// the Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_NODE_ID_SIZE, defined in 
// ConfigApp.h.
// In this demo, this variable array is set to be empty.
/*************************************************************************/
#if ADDITIONAL_NODE_ID_SIZE > 0
    uint8_t AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {LIGHT};
#endif


/*************************************************************************/
// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
/*************************************************************************/
#if defined(PHY_AT86RF233)
uint8_t myChannel = 25;
/* Range: 11 to 26 */
#elif defined(PHY_AT86RF212B)
uint8_t myChannel = 8;
/* Range for default configuration: 1 to 10
 Note: TX Power and PHY Mode Setting needs to be modified as per the 
 recommendation from Data Sheet for European band (ie.,Channel 0)*/
#endif

static void Connection_Confirm(miwi_status_t status)
{
	//printf("\r\nConnect Operation Status: "); //MATEMP
	//printf("%d\n",status);
}
	
bool freezer_feature(void)
{
    MIWI_TICK tick1, tick2;
    uint8_t switch_val;
    tick1.Val = MiWi_TickGet();
    while(1)
    {
        tick2.Val = MiWi_TickGet();
        if(MiWi_TickGetDiff(tick2, tick1) > (ONE_SECOND * 4))
            break;
        switch_val = ButtonPressed ();
        if(switch_val == 1)
        {
#if defined (ENABLE_LCD)
            LCDDisplay((char *)"Restoring Network !!", 0, false);
			delay_ms(1000);
#endif
            return true;
        }
        else
        {
            return false;
        }

    }
	return false;
}



bool Initialize_Demo(bool freezer_enable)
{
    uint8_t i;   
	uint64_t ieeeAddr;
	uint64_t invalidIEEEAddr;

	MiApp_SubscribeDataIndicationCallback(ReceivedDataIndication);

#ifdef ENABLE_SLEEP_FEATURE
    sm_init();
#endif

    if (freezer_enable)
    {
        /*******************************************************************/
        // Initialize Microchip proprietary protocol. Which protocol to use
        // depends on the configuration in ConfigApp.h
        /*******************************************************************/
        /*******************************************************************/
        // Function MiApp_ProtocolInit initialize the protocol stack. The
        // only input parameter indicates if previous network configuration
        // should be restored. In this simple example, we assume that the 
        // network starts from scratch.
        /*******************************************************************/
        MiApp_ProtocolInit(NULL, NULL);
		/* Check if a valid IEEE address is available. */
		memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
		memset((uint8_t *)&invalidIEEEAddr, 0xFF, sizeof(invalidIEEEAddr));
		srand(PHY_RandomReq());
		/*
		 * This while loop is on purpose, since just in the
		 * rare case that such an address is randomly
		 * generated again, we must repeat this.
		 */
		while ((ieeeAddr == 0x0000000000000000) || (ieeeAddr == invalidIEEEAddr))
		{
			/*
			 * In case no valid IEEE address is available, a random
			 * IEEE address will be generated to be able to run the
			 * applications for demonstration purposes.
			 * In production code this can be omitted.
			 */
			uint8_t* peui64 = (uint8_t *)&myLongAddress;
			for(i = 0; i<MY_ADDRESS_LENGTH; i++)
			{
				*peui64++ = (uint8_t)rand();
			}
			memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
		}
		PHY_SetIEEEAddr((uint8_t *)&ieeeAddr);
        #if defined(PROTOCOL_P2P)
            DemoOutput_Instruction();
            #else
            STAR_DEMO_OPTIONS_MESSAGE (role);
        #endif
        
    }
    else
    {
		LED_Off(LED0);

        /*******************************************************************/
        // Initialize Microchip proprietary protocol. Which protocol to use
        // depends on the configuration in ConfigApp.h
        /*******************************************************************/
        /*******************************************************************/
        // Function MiApp_ProtocolInit initialize the protocol stack. The
        // only input parameter indicates if previous network configuration
        // should be restored. In this simple example, we assume that the 
        // network starts from scratch.
        /*******************************************************************/
        MiApp_ProtocolInit(NULL, NULL);
		/* Check if a valid IEEE address is available. */
		memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
		memset((uint8_t *)&invalidIEEEAddr, 0xFF, sizeof(invalidIEEEAddr));
		srand(PHY_RandomReq());
		/*
		 * This while loop is on purpose, since just in the
		 * rare case that such an address is randomly
		 * generated again, we must repeat this.
		 */
		while ((ieeeAddr == 0x0000000000000000) || (ieeeAddr == invalidIEEEAddr))
		{
			/*
			 * In case no valid IEEE address is available, a random
			 * IEEE address will be generated to be able to run the
			 * applications for demonstration purposes.
			 * In production code this can be omitted.
			 */
			uint8_t* peui64 = (uint8_t *)&myLongAddress;
			for(i = 0; i<MY_ADDRESS_LENGTH; i++)
			{
				*peui64++ = (uint8_t)rand();
			}
			memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
		}
		PHY_SetIEEEAddr((uint8_t *)&ieeeAddr);
        // Set default channel
        if( MiApp_Set(CHANNEL, &myChannel) == false )
        {
            DemoOutput_ChannelError(myChannel);
        }

        /*******************************************************************/
        // Function MiApp_ConnectionMode defines the connection mode. The
        // possible connection modes are:
        //  ENABLE_ALL_CONN:    Enable all kinds of connection
        //  ENABLE_PREV_CONN:   Only allow connection already exists in 
        //                      connection table
        //  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
        //  DISABLE_ALL_CONN:   Disable all connections. 
        /*******************************************************************/
        MiApp_ConnectionMode(ENABLE_ALL_CONN);
        DemoOutput_Channel(myChannel, 0);

        /*******************************************************************/
        // Function MiApp_EstablishConnection try to establish a new 
        // connection with peer device. 
        // The first parameter is the index to the active scan result, 
        //      which is acquired by discovery process (active scan). If 
        //      the value of the index is 0xFF, try to establish a 
        //      connection with any peer.
        // The second parameter is the mode to establish connection, 
        //      either direct or indirect. Direct mode means connection 
        //      within the radio range; indirect mode means connection 
        //      may or may not in the radio range. 
        /*******************************************************************/
        i = MiApp_EstablishConnection(myChannel, 0, NULL, 0, Connection_Confirm);

        /*******************************************************************/
        // Display current opertion on LCD of demo board, if applicable
        /*******************************************************************/
        if( i != 0xFF )
        {
            DemoOutput_Channel(myChannel, 1);
            #if defined(PROTOCOL_STAR)
                role = false;  // denotes its  a End device
            #endif
        }
        else
        {
            /*******************************************************************/
            // If no network can be found and join, we need to start a new 
            // network by calling function MiApp_StartConnection
            //
            // The first parameter is the mode of start connection. There are 
            // two valid connection modes:
            //   - START_CONN_DIRECT        start the connection on current 
            //                              channel
            //   - START_CONN_ENERGY_SCN    perform an energy scan first, 
            //                              before starting the connection on 
            //                              the channel with least noise
            //   - START_CONN_CS_SCN        perform a carrier sense scan 
            //                              first, before starting the 
            //                              connection on the channel with 
            //                              least carrier sense noise. Not
            //                              supported for current radios
            //
            // The second parameter is the scan duration, which has the same 
            //     definition in Energy Scan. 10 is roughly 1 second. 9 is a 
            //     half second and 11 is 2 seconds. Maximum scan duration is 
            //     14, or roughly 16 seconds.
            //
            // The third parameter is the channel map. Bit 0 of the 
            //     double word parameter represents channel 0. For the 2.4GHz 
            //     frequency band, all possible channels are channel 11 to 
            //     channel 26. As the result, the bit map is 0x07FFF800. Stack 
            //     will filter out all invalid channels, so the application 
            //     only needs to pay attention to the channels that are not 
            //     preferred.
            /*******************************************************************/
            MiApp_StartConnection(START_CONN_DIRECT, 10, (1L << myChannel), Connection_Confirm);
        #if defined(PROTOCOL_STAR)
            role = true;  // Denotes its a Pan Co
        #endif
        }
        #if defined(PROTOCOL_STAR) && defined(ENABLE_NETWORK_FREEZER)
            PDS_Store(PDS_ROLE_ID); // Saving the Role of the device			
        #endif
        /*******************************************************************/
        // Function DumpConnection is used to print out the content of the
        //  Connection Entry on the hyperterminal. It may be useful in 
        //  the debugging phase.
        // The only parameter of this function is the index of the  
        //  Connection Entry. The value of 0xFF means to print out all
        //  valid Connection Entry; otherwise, the Connection Entry
        //  of the input index will be printed out.
        /*******************************************************************/
        #if defined(ENABLE_CONSOLE)
        DumpConnection(0xFF);
        #endif


        // Turn on LED 1 to indicate connection established
		LED_On(LED0);
        #if defined(PROTOCOL_P2P)
            DemoOutput_Instruction();
        #else
            STAR_DEMO_OPTIONS_MESSAGE (role);
        #endif    
        
    }
	return true;
}


void Run_Demo(void)
{
      P2PTasks();
    #if defined(PROTOCOL_P2P)
        run_p2p_demo();
    #else
        run_star_demo();
    #endif
    
    
}