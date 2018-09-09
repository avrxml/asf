/**
* \file  main.c
*
* \brief Simple demo application for wireless chat.
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
* The chat demo P2P application code focuses on the simplicity of the 
* MiWi™ DE protocol stack application programming interfaces. 
* It provides a clean and straightforward wireless communication between 
* two devices with less than 30 lines of effective C code to run the stack 
* in application layer for both devices. In this application, following 
* features of MiWi™ DE protocol stack have been demonstrated:
* <P>• Establish connection automatically between two devices.</P>
* <P>• Unicast a packet.</P>
* <P>• Apply security to the transmitted packet.</P>
*/

/************************ HEADERS ****************************************/
#include "system.h"
#include "miwi_config.h"          //MiWi Application layer configuration file
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#include "sysTimer.h"
#include "miwi_api.h"
#include "mimem.h"
#include "sio2host.h"
#include "asf.h"
#include "phy.h"

#if ((BOARD == SAMR30_XPLAINED_PRO) || (BOARD == SAMR21_XPLAINED_PRO))
#include "edbg-eui.h"
#endif

/************************** DEFINITIONS **********************************/
#define MAX_MESSAGE_LEN      35
#define KEY_ENTER            0x0D
#define KEY_BACKSPACE        0x08
#define APP_DEBOUNCE_TIME_MS 50

#if (BOARD == SAMR21ZLL_EK)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x00804008U))
#endif

#if (BOARD == SAMR30_MODULE_XPLAINED_PRO)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x0080400AU))
#endif

/************************** VARIABLES ************************************/

/*************************************************************************/

// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).

/*************************************************************************/
#ifdef PHY_AT86RF212B
uint8_t        myChannel = 3;
/* Range for default configuration: 1 to 10
 Note: TX Power and PHY Mode Setting needs to be modified as per the 
 recommendation from Data Sheet for European band (ie.,Channel 0)*/
#else 
uint8_t        myChannel = 26;
/* Range: 11 to 26 */
#endif

//Chat Window Application Variables
uint8_t        TxMessage[MAX_MESSAGE_LEN];
uint8_t        TxMessageSize = 0;

//Chat Application status variables
bool    messagePending = false;
bool    transmitPending = false;

//Timers used in the application to determine 'no activity'
MIWI_TICK    tickCurrent;
MIWI_TICK    tickPrevious;
uint8_t msghandledemo = 0;

static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer);


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
uint8_t        AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {0x00};
#endif

void ReadMacAddress(void);
void ProcessRxMessage(void);
void FormatTxMessage(void);
void TransmitMessage(void);
void ReceivedDataIndication (RECEIVED_MESSAGE *ind);
static void Connection_Confirm(miwi_status_t status);
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
* Overview:		    This is the main function that runs the Chat application
*
* Note:			    
**********************************************************************/

int main (void)
{
	uint8_t i;
	uint64_t ieeeAddr;
	uint64_t invalidIEEEAddr;
    irq_initialize_vectors();
    /*******************************************************************
    * System related initialization based on the API provided by board
    *******************************************************************/
    #if SAMD || SAMR21 || SAMR30
    system_init();
    delay_init();
    #else
    sysclk_init();
    board_init();
    #endif

    cpu_irq_enable();
    sio2host_init();
    /*******************************************************************/

    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in miwi_config.h

    /*******************************************************************/
    // Clear the screen (VT100)
    printf("\x1b[2J");
    // Send the cursor home (VT100)
    printf("\x1b[H");
    printf("\r\nChat Demo");
    printf("\r\n\r\nDemo Instruction:");
    printf("\r\nUse Console to Chat with the Peer Device");
    printf("\r\n");

    // Read the MAC address from either flash or EDBG
    ReadMacAddress();
	
    // Initialize system Timer
    SYS_TimerInit();
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
	while ((ieeeAddr == 0x00UL) || (ieeeAddr == invalidIEEEAddr))
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
	MiApp_SubscribeDataIndicationCallback(ReceivedDataIndication);
    // Set default channel
    if( MiApp_Set(CHANNEL, &myChannel) == false )
    {
        printf("!!!ERROR - Channel configuration for %d failed...\r\n", myChannel);
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

    printf("\r\n\r\nConnecting to Peer...\r\n");

    /*******************************************************************/

    // Function MiApp_StartConnection will enable a node to start operating
    // in a variety of ways. Usually, this fucntion is called by the
    // PAN Coordinator who is the first in the PAN.
    //
    // The first parameter defines the mode to start the PAN in
    //
    // The second parameter defines the scan duration (if energy/carrier
    // sense scan is enabled). 0 if START_CONN_DIRECT used.
    //
    // The third parameter is a bit map of of the channels to perform the
    // noise scan on. 0 if START_CONN_DIRECT used.

    /*******************************************************************/

    MiApp_StartConnection(START_CONN_DIRECT, 10, (1L << myChannel), Connection_Confirm) ;
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
#ifdef ENABLE_HAND_SHAKE
    i = 0xFF;
    while(i == 0xFF)
    {
        i = MiApp_EstablishConnection(myChannel, 0, NULL, 0, Connection_Confirm);
    }
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
#ifdef ENABLE_DUMP
    DumpConnection(0xFF);
#endif

    #ifndef ENABLE_POWERSAVE
    // Turn on LED 1 to indicate P2P connection established
	port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
    #endif

    printf("-------------------------------------------------------\r\n");
    printf("Chat Window: \r\n");
    printf("-------------------------------------------------------\r\n");
    printf("$$");
	
    while(1)
    {
		
        P2PTasks();

        FormatTxMessage();
            
        if(messagePending)
        {
            tickCurrent.Val = MiWi_TickGet();
            if
            (
                (MiWi_TickGetDiff(tickCurrent, tickPrevious) > (ONE_SECOND * 30)) ||
                (TxMessageSize >= MAX_MESSAGE_LEN) ||
                (transmitPending == true)
            )
            {
                TransmitMessage();
            }
        }

        // Display connection table if Button is pressed
#ifdef BUTTON_0_PIN
        if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE)
        {
	        /* Wait for button de-bounce time */
	        delay_ms(APP_DEBOUNCE_TIME_MS);
	        /* Check whether button is in default state */
	        while(port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE);
        	printf("\r\n\r\nDumping Connection Table...\r\n");
        	DumpConnection(0xFF);
        	printf("-------------------------------------------------------\r\n");
        	printf("Chat Window: \r\n");
        	printf("-------------------------------------------------------\r\n");
        	printf("$$ ");
        }
#endif
    }

    //Enable device to foward the received packet information to the console
}                           //end of main
        
static void Connection_Confirm(miwi_status_t status)
{

}

static void dataConfcb(uint8_t handle, miwi_status_t status, uint8_t* msgPointer)
{
    MiMem_Free(msgPointer);
}

void FormatTxMessage()
{
    uint8_t    inputChar;
	uint8_t retValue;
    
    retValue = sio2host_rx(&inputChar, 1);
	if (0 != retValue)
	{
		sio2host_tx(&inputChar, 1);

		if(inputChar == KEY_ENTER)
		{
			printf("\r\n$$");
			messagePending = true;
			transmitPending = true;
		}
		else if(inputChar == KEY_BACKSPACE)
		{
			TxMessageSize--;
		}
		else
		{
			if(TxMessageSize < MAX_MESSAGE_LEN)
			{
				TxMessage[TxMessageSize] = inputChar;
				TxMessageSize++;
				tickPrevious.Val = MiWi_TickGet();
				//messagePending = true;
			}
		}
	}
}
     
     
void TransmitMessage()
{
    uint8_t index;
    uint8_t* dataPtr = NULL;
    uint8_t dataLen = 0;
    //Send message

    /******************************************************************/

    // First call function MiApp_FlushTx to reset the Transmit buffer.
    //  Then fill the buffer one byte by one byte by calling function
    //  MiApp_WriteData

    /*******************************************************************/
    //+1 to add TxMessageSize also in payload
   dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(TxMessageSize + 1)); 
   if (NULL == dataPtr)
       return;

    dataPtr[dataLen++] = TxMessageSize;
    for(index = 0; index < TxMessageSize; index++)
    {
        dataPtr[dataLen++] = TxMessage[index];
    }

    //Unicast Message

    /*******************************************************************/

    // Function MiApp_UnicastConnection is one of the functions to
    // unicast a message.
    //    The first parameter is the index of connection table for
    //       the peer device. In this lab, the chat is always sent to the
    //       first P2P Connection Entry of the connection table. If that
    //		 node is down (eg. student is programming new firmare into it),
    //		 the user is prompted to reset the node to re-establish the
    //       connection table with a new peer.
    //
    //    The second parameter is the boolean to indicate if we need
    //       to secure the frame. If encryption is applied, the
    //       security level and security key are defined in the
    //       configuration file for the transceiver
    //
    // Another way to unicast a message is by calling function
    // MiApp_UnicastAddress. Instead of supplying the index of the
    // connection table of the peer device, this function requires the
    // input parameter of destination address directly.

    /*******************************************************************/
    if(MiApp_SendData(8, ConnectionTable[0].Address, dataLen, dataPtr, msghandledemo++, true, dataConfcb) == false )
    {
        //Message TX Failed (peer node 00 was likely being re-programmed by student)
        //Should reset the node to establish new peer connection to send chat to

        printf("Transmit to Peer 00 Failed. Press MCLR to establish new connections");
    }


	//Reset Chat Application state variables
    messagePending = false;
    transmitPending = false;
    TxMessageSize = 0;


}

/*********************************************************************
* Function: void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
*
* Overview: Process a Received Message
*
* PreCondition: MiApp_ProtocolInit
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ReceivedDataIndication (RECEIVED_MESSAGE *ind)
{
    uint8_t index;
    if(rxMessage.flags.bits.broadcast)
    {
	    printf("\n -------- From group:");
    }
    else
    {
	    printf("\n -------- From [");
    }

    if(rxMessage.flags.bits.srcPrsnt)
    
    {
	    if(rxMessage.flags.bits.altSrcAddr)
	    {
		    printf("%x",rxMessage.SourceAddress[1]);
		    printf("%x",rxMessage.SourceAddress[0]);
	    }
	    else
	    {
		    for(index = 0; index < MY_ADDRESS_LENGTH; index++)
		    {
			    printf("%x",rxMessage.SourceAddress[MY_ADDRESS_LENGTH - 1 - index]);
		    }
	    }

	    printf("] : ");
    }

    for(index = 1; index < rxMessage.PayloadSize; index++)
    {
	    sio2host_tx(&rxMessage.Payload[index], 1);
    }

    printf("\r\n$$");

    // Toggle LED2 to indicate receiving a packet.
    //LED_2 ^= 1;
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
	for (i = 0; i < MY_ADDRESS_LENGTH; i += 2, j++)
	{
		myLongAddress[i] = (NVM_UID_ADDRESS[j] & 0xFF);
		myLongAddress[i + 1] = (NVM_UID_ADDRESS[j] >> 8);
	}
#elif ((BOARD == SAMR30_XPLAINED_PRO) || (BOARD == SAMR21_XPLAINED_PRO))
	uint8_t* peui64 = edbg_eui_read_eui64();
	for(uint8_t i = 0; i<MY_ADDRESS_LENGTH; i++)
	{
		myLongAddress[i] = peui64[MY_ADDRESS_LENGTH-i-1];
	}
#endif
}
 
