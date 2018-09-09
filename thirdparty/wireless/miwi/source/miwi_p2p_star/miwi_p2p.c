/**
* \file  miwi_p2p.c
*
* \brief MiWi P2P & Star Protocol Implementation
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

/************************ HEADERS **********************************/
#include "system.h"
#include "miwi_config.h"          //MiWi Application layer configuration file
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#include "miwi_p2p.h"
#include "mimac_at86rf.h"
#include "mimem.h"
#include "delay.h"
#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsDataServer.h"
#include "wlPdsTaskManager.h"
#endif
//Global Variables
// In p2p total no of devices , In star used only by PAN COR to calculate no of END Devices
uint8_t conn_size ;
// Used by END_DEVICE to store the index value shared by PAN Co on join
uint8_t MyindexinPC;  
uint8_t myConnectionIndex_in_PanCo; 
MIWI_TICK tick1, tick4;
bool lost_connection = false;
uint8_t temp_bit;
bool FW_Stat;
PacketIndCallback_t pktRxcallback = NULL;
connectionConf_callback_t EstConfCallback; //MATEMP

#if defined(PROTOCOL_STAR)
uint8_t Find_Index (uint8_t *DestAddr);
void handle_lost_connection(void);
void store_connection_tb(uint8_t *payload);
bool Forward_Packet_PANCr_to_EDy(uint8_t fw_payload_len, uint8_t *fw_payload ,uint8_t index , uint8_t *EDxAddress);
#endif

// permanent address definition
#if MY_ADDRESS_LENGTH == 8
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6,EUI_7};
#elif MY_ADDRESS_LENGTH == 7
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6};
#elif MY_ADDRESS_LENGTH == 6
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5};
#elif MY_ADDRESS_LENGTH == 5
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4};
#elif MY_ADDRESS_LENGTH == 4
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3};
#elif MY_ADDRESS_LENGTH == 3
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2};
#elif MY_ADDRESS_LENGTH == 2
    uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1};
#endif

// Evaluate Total No of Peer Connection on a Node
uint8_t Total_Connections(void)
{
    uint8_t count=0 , i;
    for (i=0;i<CONNECTION_SIZE;i++)
    {
    #if defined(PROTOCOL_STAR)
        if (ConnectionTable[i].Address[0] != 0x00 || ConnectionTable[i].Address[1] != 0x00 || ConnectionTable[i].Address[2] != 0x00)
        {
            count++;
        }
    #else
        if (ConnectionTable[i].status.bits.isValid)
        {
            count++;
        }
    #endif      
    }
    return count;
}


#if defined(PROTOCOL_STAR)
    // Static Variables
    // used by PAN COR to generate SW_LAYER ACK
    bool rec_ack ;
    
    // Global Variables
    // used by END_DEVICES to store total no of end_devices in network
    uint8_t end_nodes = 0; 
    END_DEVICES_Unique_Short_Address  END_DEVICES_Short_Address[CONNECTION_SIZE];
    bool role; // Role of Device in Star Network
    bool fix_me;  // true : END_DEVICE if considered InActive by PAN Co but is still active 

    
    #if defined(ENABLE_LINK_STATUS)
            uint8_t link_stat[CONNECTION_SIZE] ;
            uint8_t permanent_connections[CONNECTION_SIZE] ;
    #endif

    // All connections (FFD || RFD)are stored in Connection Table of PAN CO
    // Each Connection is identified by its index no.
    // In case of Data TX , EDx --> PAN CO --> EDy
    // PAN CO will forward the data to EDy , In order to know 
    uint8_t Find_Index (uint8_t *DestAddr)
    {
        uint8_t i;
        uint8_t return_val;
        for (i=0;i<conn_size;i++)
        {
            if (ConnectionTable[i].status.bits.isValid)
            {
                if (DestAddr[1] == ConnectionTable[i].Address[0] && DestAddr[2] == ConnectionTable[i].Address[1] && DestAddr[3] == ConnectionTable[i].Address[2] )
                {
                    return_val = i;
                    break;
                }
                
            }
        }
        if (i==conn_size)
            {
                return_val = 0xff;
            }
		return return_val;
    }
     
    
    void handle_lost_connection(void)
    {
        uint8_t i ;
        bool stat = false;
        if (!role)
        {
            for (i=0;i<end_nodes+1;i++)
            {
                if (myLongAddress[0] == END_DEVICES_Short_Address[i].Address[0] && myLongAddress[1] == END_DEVICES_Short_Address[i].Address[1])
                {
                    stat = true;
                }
            }
            if (!stat)
            {
                lost_connection = true;
            }
        }
        
    }
     
    // Function to store the Connection Table Information which is Broadcasted by PAN CO
    // Used by END_DEVICES (FFD || RFD) only
    void store_connection_tb(uint8_t *payload)
    { 
        uint8_t i , j ;
        for (i=4; i < RX_BUFFER_SIZE; i++)
        {
            if (i % 4 == 0 && (i+3) < RX_BUFFER_SIZE)
            {
                j = payload[i+3];
                END_DEVICES_Short_Address[j].connection_slot = j;
                END_DEVICES_Short_Address[j].Address[0] = payload[i];
                END_DEVICES_Short_Address[j].Address[1] = payload[i+1];
                END_DEVICES_Short_Address[j].Address[2] = payload[i+2];
                // END Device is Active but is being wrongly considered as Inactive by PAN CO
                
            }
            
            
        }
        handle_lost_connection();
        
      
    }
    
    


    /// This function will only be called by Pan co ordinator to forward the packet to the destination end device
    bool Forward_Packet_PANCr_to_EDy(uint8_t fw_payload_len, uint8_t *fw_payload ,uint8_t index , uint8_t *EDxAddress)
    {
        uint8_t i;
        bool status;
        uint8_t* dataPtr = NULL;
        uint8_t dataLen = 0;
        dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(fw_payload_len+3));
        if (NULL == dataPtr)
          return false;
        dataPtr[dataLen++] = EDxAddress[0];    // Unique address of EDy (DEST ED) 
        dataPtr[dataLen++] = EDxAddress[1];    // Unique address of EDy (DEST ED) 
        dataPtr[dataLen++] = EDxAddress[2];    // Unique address of EDy (DEST ED) 
        for(i = 4; i < fw_payload_len; i++)
        {
            dataPtr[dataLen++] = fw_payload[i];
        }
        #if defined(ENABLE_SECURITY)
                status = MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[index].Address, 
				dataLen, dataPtr, 0, true, NULL);
				MiMem_Free(dataPtr);
        #else
                status = MiApp_SendData(LONG_ADDR_LEN, ConnectionTable[index].Address, 
				dataLen, dataPtr, 0, true, NULL);
				MiMem_Free(dataPtr);
        #endif

        return status;
                    
    }

#endif

#if defined(ENABLE_ED_SCAN) || defined(ENABLE_ACTIVE_SCAN) || defined(ENABLE_FREQUENCY_AGILITY)
    // Scan Duration formula for P2P Connection:
    //  60 * 16 * (2 ^ n + 1) symbols.
    /*#define SCAN_DURATION_0 SYMBOLS_TO_TICKS(1920)
    #define SCAN_DURATION_1 SYMBOLS_TO_TICKS(2880)
    #define SCAN_DURATION_2 SYMBOLS_TO_TICKS(4800)
    #define SCAN_DURATION_3 SYMBOLS_TO_TICKS(8640)
    #define SCAN_DURATION_4 SYMBOLS_TO_TICKS(16320)
    #define SCAN_DURATION_5 SYMBOLS_TO_TICKS(31680)
    #define SCAN_DURATION_6 SYMBOLS_TO_TICKS(62400)
    #define SCAN_DURATION_7 SYMBOLS_TO_TICKS(123840)
    #define SCAN_DURATION_8 SYMBOLS_TO_TICKS(246720)
    #define SCAN_DURATION_9 SYMBOLS_TO_TICKS(492480)
    #define SCAN_DURATION_10 SYMBOLS_TO_TICKS(984000)
    #define SCAN_DURATION_11 SYMBOLS_TO_TICKS(1967040)
    #define SCAN_DURATION_12 SYMBOLS_TO_TICKS(3933120)
    #define SCAN_DURATION_13 SYMBOLS_TO_TICKS(7865280)
    #define SCAN_DURATION_14 SYMBOLS_TO_TICKS(15729600)
    const  uint32_t ScanTime[15] = {SCAN_DURATION_0,SCAN_DURATION_1,SCAN_DURATION_2,SCAN_DURATION_3,
        SCAN_DURATION_4,SCAN_DURATION_5,SCAN_DURATION_6,SCAN_DURATION_7,SCAN_DURATION_8,SCAN_DURATION_9,
        SCAN_DURATION_10, SCAN_DURATION_11, SCAN_DURATION_12, SCAN_DURATION_13,SCAN_DURATION_14
    }; */
	
	static inline  uint32_t miwi_scan_duration_ticks(uint8_t scan_duration)
	{
		uint32_t scan_symbols;
		
		scan_symbols =   ABASESUPERFRAMEDURATION *((1<<scan_duration) + 1);
		return SYMBOLS_TO_TICKS(scan_symbols);
	}
#endif

#ifdef ENABLE_INDIRECT_MESSAGE
   
        INDIRECT_MESSAGE indirectMessages[INDIRECT_MESSAGE_SIZE];   // structure to store the indirect messages
                                                                    // for nodes with radio off duing idle time
#endif


    CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE];


#if defined(IEEE_802_15_4)
    API_UINT16_UNION        myPANID;                    // the PAN Identifier for the device
#endif
uint8_t            currentChannel = 0xFF;             // current operating channel for the device
uint8_t            ConnMode = DISABLE_ALL_CONN;
uint8_t            P2PCapacityInfo;
RECEIVED_MESSAGE  rxMessage;                    // structure to store information for the received packet
uint8_t            LatestConnection;
volatile P2P_STATUS P2PStatus;
extern uint8_t     AdditionalNodeID[];             // the additional information regarding the device
                                                // that would like to share with the peer on the 
                                                // other side of P2P connection. This information 
                                                // is applicaiton specific. 
#if defined(ENABLE_ACTIVE_SCAN)
    uint8_t    ActiveScanResultIndex;
    ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];  // The results for active scan, including
                                                                    // the PAN identifier, signal strength and 
                                                                    // operating channel
#endif

#ifdef ENABLE_SLEEP_FEATURE
    MIWI_TICK DataRequestTimer;
#endif

MAC_RECEIVED_PACKET MACRxPacket;

#if defined(ENABLE_SECURITY)
    API_UINT32_UNION IncomingFrameCounter[CONNECTION_SIZE];  // If authentication is used, IncomingFrameCounter can prevent replay attack
#endif

#if defined(ENABLE_NETWORK_FREEZER)
    MIWI_TICK nvmDelayTick;
#endif


#if defined(ENABLE_TIME_SYNC)
    #if defined(ENABLE_SLEEP_FEATURE)
        API_UINT16_UNION WakeupTimes;
        API_UINT16_UNION CounterValue;
    #elif defined(ENABLE_INDIRECT_MESSAGE)
        uint8_t TimeSyncSlot = 0;
        MIWI_TICK TimeSyncTick;
        MIWI_TICK TimeSlotTick;
    #endif
#endif

/************************ FUNCTION DEFINITION ********************************/
uint8_t AddConnection(void);
void CommandConfCallback(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer);

#if defined(IEEE_802_15_4)
    bool SendPacket(INPUT bool Broadcast,
                    API_UINT16_UNION DestinationPANID,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT bool SecurityEnabled,
                    INPUT uint8_t msgLen,
                    INPUT uint8_t* msgPtr,
                    INPUT uint8_t msghandle,
					INPUT bool ackReq,
                    INPUT DataConf_callback_t ConfCallback);
#else
    bool SendPacket(INPUT bool Broadcast,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT uint8_t msgLen,
                    INPUT uint8_t* msgPtr,
                    INPUT uint8_t msghandle,
					INPUT bool ackReq,
                    INPUT DataConf_callback_t ConfCallback);
#endif                    

#ifdef ENABLE_FREQUENCY_AGILITY
    void StartChannelHopping(INPUT uint8_t OptimalChannel);
#endif

bool CheckForData(void);

/************************ FUNCTIONS ********************************/
void CommandConfCallback(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer)
{
    MiMem_Free(msgPointer);
}

/*********************************************************************
 * void P2PTasks( void )
 *
 * Overview:        This function maintains the operation of the stack
 *                  It should be called as often as possible. 
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The stack receives, handles, buffers, and transmits 
 *                  packets.  It also handles all of the joining 
 * 
 ********************************************************************/
void P2PTasks(void)
{
    uint8_t i;
    MIWI_TICK  tmpTick;

    #if defined(PROTOCOL_STAR)
    MIWI_TICK  tick2 , tick3 ;
    #endif

    MiMAC_Task();
    tmpTick.Val = 0;
    #if defined(PROTOCOL_STAR) 
        tick2.Val = MiWi_TickGet();
        tick3.Val = MiWi_TickGet();
        #if defined(ENABLE_LINK_STATUS)
            if (role)
            {
                if( MiWi_TickGetDiff(tick3, tick4) > (FIND_INACTIVE_DEVICE_TIMEOUT))
                {
                    Find_InActiveDevices ();
                    tick4 = tick3;
                } 

            }
            else
            {
                if( MiWi_TickGetDiff(tick3, tick1) > (LINK_STATUS_TIMEOUT) )
                {
                        send_link_status ();
                        tick1 = tick3;
                }
            }
        #endif

        #if defined(ENABLE_PERIODIC_CONNECTIONTABLE_SHARE)
            if( MiWi_TickGetDiff(tick2, tick1) > (SHARE_PEER_DEVICE_INFO_TIMEOUT) && role)
            {
                MiApp_BroadcastConnectionTable ();
                tick1 = tick2;
            }
        #endif
    #endif

    
    #ifdef ENABLE_INDIRECT_MESSAGE
        //check indirect message periodically. If an indirect message is not acquired within
        //time of INDIRECT_MESSAGE_TIMEOUT
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid )
            {
                tmpTick.Val = MiWi_TickGet();
                if( MiWi_TickGetDiff(tmpTick, indirectMessages[i].TickStart) > INDIRECT_MESSAGE_TIMEOUT )
                {
                    indirectMessages[i].flags.Val = 0x00;   
                    printf("\r\nIndirect message expired");
                    indirectMessages[i].indirectConfCallback(indirectMessages[i].indirectDataHandle, TRANSACTION_EXPIRED, indirectMessages[i].PayLoad);
                }    
            }    
        }
    #endif
    
    #ifdef ENABLE_SLEEP_FEATURE
        // check if a response for Data Request has been received with in 
        // time of RFD_DATA_WAIT, defined in P2P.h. Expire the Data Request
        // to let device goes to sleep, if no response is received. Save
        // battery power even if something wrong with associated device
        if( P2PStatus.bits.DataRequesting )
        {
            tmpTick.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(tmpTick, DataRequestTimer) > RFD_DATA_WAIT )
            {
#if defined(ENABLE_CONSOLE) 
                printf("Data Request Expired\r\n");
#endif
                P2PStatus.bits.DataRequesting = 0;
                #if defined(ENABLE_TIME_SYNC)
                    WakeupTimes.Val = RFD_WAKEUP_INTERVAL / 16;
                    CounterValue.Val = 0xFFFF - ((uint16_t)4000*(RFD_WAKEUP_INTERVAL % 16));
                #endif
            }
        }
    #endif

    #if defined(ENABLE_NETWORK_FREEZER)
        if( P2PStatus.bits.SaveConnection )
        {
            tmpTick.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(tmpTick, nvmDelayTick) > (ONE_SECOND) )
            {
                P2PStatus.bits.SaveConnection = 0;
#if defined(ENABLE_NETWORK_FREEZER)
				PDS_Store(PDS_CONNECTION_TABLE_ID);
#endif
#if defined(ENABLE_CONSOLE) 
                printf("\r\nSave Connection\r\n");
#endif
            }
        }
    #endif

    #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP_FEATURE) && defined(ENABLE_INDIRECT_MESSAGE)
        tmpTick.Val = MiWi_TickGet();
        if( MiWi_TickGetDiff(tmpTick, TimeSyncTick) > ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) )
        {
            TimeSyncTick.Val += ((uint32_t)(ONE_SECOND) * RFD_WAKEUP_INTERVAL);
            if( TimeSyncTick.Val > tmpTick.Val )
            {
                TimeSyncTick.Val = tmpTick.Val;
            }    
            TimeSyncSlot = 0;
        }    
    #endif

  // Check if transceiver receive any message.
    if( P2PStatus.bits.RxHasUserData == 0 && MiMAC_ReceivedPacket() )
    { 
        FW_Stat = false;  // Used for SW_Generated ACK T PAN CO
        rxMessage.flags.Val = 0;
        //rxMessage.flags.bits.broadcast = MACRxPacket.flags.bits.broadcast;
        
            temp_bit = MACRxPacket.flags.bits.broadcast;
            rxMessage.flags.bits.broadcast = temp_bit;

        rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
        rxMessage.flags.bits.command = (MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND) ? 1:0;
        rxMessage.flags.bits.srcPrsnt = MACRxPacket.flags.bits.sourcePrsnt;
        if( MACRxPacket.flags.bits.sourcePrsnt )
        {
            rxMessage.SourceAddress = MACRxPacket.SourceAddress;
        }
        #if defined(IEEE_802_15_4) && !defined(TARGET_SMALL)
            rxMessage.SourcePANID.Val = MACRxPacket.SourcePANID.Val;
        #endif

        rxMessage.PayloadSize = MACRxPacket.PayloadLen;
        rxMessage.Payload = MACRxPacket.Payload;
        
        
            
               
        
        /************************/
      
        #ifndef TARGET_SMALL
            rxMessage.PacketLQI = MACRxPacket.LQIValue;
            rxMessage.PacketRSSI = MACRxPacket.RSSIValue;
        #endif

        if( rxMessage.flags.bits.command )
        {
            // if comes here, we know it is a command frame
            switch( rxMessage.Payload[0] )
            {
                #if defined(ENABLE_HAND_SHAKE)
              
                    case CMD_P2P_CONNECTION_REQUEST:
                        {
                           #if defined(PROTOCOL_STAR)
                                if(role)   // Important if implementing a star network
                                {    
                            #endif
                                  
                            // if a device goes to sleep, it can only have one
                            // connection, as the result, it cannot accept new
                            // connection request
                            #ifdef ENABLE_SLEEP_FEATURE
                                MiMAC_DiscardPacket();
                                break;
                            #else
                                
                                uint8_t status = STATUS_SUCCESS;
                                
                                // if channel does not math, it may be a 
                                // sub-harmonics signal, ignore the request
                                if( currentChannel != rxMessage.Payload[1] )
                                {
                                    MiMAC_DiscardPacket();
                                    break;
                                }
                                
                                // if new connection is not allowed, ignore 
                                // the request
                                if( ConnMode == DISABLE_ALL_CONN )
                                { 
                                    MiMAC_DiscardPacket();
                                    break;
                                }
                                
                                #if !defined(TARGET_SMALL) && defined(IEEE_802_15_4)
                                    // if PANID does not match, ignore the request
                                    if( rxMessage.SourcePANID.Val != 0xFFFF &&
                                        rxMessage.SourcePANID.Val != myPANID.Val &&
                                        rxMessage.PayloadSize > 2)
                                    {
                                        status = STATUS_NOT_SAME_PAN;
                                    }
                                    else
                                #endif
                                {
                                    // request accepted, try to add the requesting
                                    // device into P2P Connection Entry
                                    status = AddConnection();
                                }
                                #if defined(PROTOCOL_STAR) && defined(ENABLE_LINK_STATUS) 
                                    uint8_t p;
                                    if (rxMessage.Payload[3] == 0xAA)
                                    {
 
                                        for (p = 0 ;p <CONNECTION_SIZE;p++)
                                        {
                                            if (isSameAddress(rxMessage.SourceAddress, ConnectionTable[p].Address) )
                                            {
                                               permanent_connections[p] = 0xFF;

                                            }

                                        }
                                    }
                                #endif
                                if( (ConnMode == ENABLE_PREV_CONN) && (status != STATUS_EXISTS && status != STATUS_ACTIVE_SCAN) )
                                {
                                    status = STATUS_NOT_PERMITTED;
                                }

                                if( (status == STATUS_SUCCESS || status == STATUS_EXISTS ) &&
                                    MiApp_CB_AllowConnection(LatestConnection) == false )
                                {
                                    ConnectionTable[LatestConnection].status.Val = 0;
                                    status = STATUS_NOT_PERMITTED;
                                }
                                uint8_t* dataPtr = NULL;
                                uint8_t dataLen = 0;
                                
                                // prepare the P2P_CONNECTION_RESPONSE command
                                dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(TX_BUFFER_SIZE));
                                if (NULL == dataPtr)
                                  return;
                                dataPtr[dataLen++] = CMD_P2P_CONNECTION_RESPONSE;
                                dataPtr[dataLen++] = status;
                                dataPtr[dataLen++] = MyindexinPC;
                                if( status == STATUS_SUCCESS ||
                                    status == STATUS_EXISTS )
                                {
                                    dataPtr[dataLen++] = P2PCapacityInfo;
                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            dataPtr[dataLen++] = AdditionalNodeID[i];
                                        }
                                    #endif
                                }
                                
                                MiMAC_DiscardPacket();
                                
                                // unicast the response to the requesting device
                                #ifdef TARGET_SMALL
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
										dataLen, dataPtr, 0, true, CommandConfCallback);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
										dataLen, dataPtr, 0, true, CommandConfCallback);
                                    #endif
                                #else
                                        
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
										dataLen, dataPtr, 0, true, CommandConfCallback);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
										dataLen, dataPtr,0, true, CommandConfCallback);
                                    #endif 
                                #endif
								delay_ms (100);
                                #if defined(PROTOCOL_STAR)
                                    MiApp_BroadcastConnectionTable ();  // Broad Cast the CT on join
                                #endif
                                #if defined(ENABLE_NETWORK_FREEZER)
                                    if( status == STATUS_SUCCESS )
                                    {
#if defined(ENABLE_NETWORK_FREEZER)
										PDS_Store(PDS_CONNECTION_TABLE_ID);
#endif
                                    }
                                #endif
      
                                     
                            #endif  // end of ENABLE_SLEEP_FEATURE
                            #if defined(PROTOCOL_STAR)
                                    }     // Important if Implementing a Star Network
                            
                              else
                              {
                                 MiMAC_DiscardPacket();
                              }
                            #endif
                              
                        }
                        break; 
               
                    case CMD_P2P_ACTIVE_SCAN_REQUEST:
                        {
                        uint8_t* dataPtr = NULL;
                        uint8_t dataLen = 0;
                        if(ConnMode > ENABLE_ACTIVE_SCAN_RSP)
                            {
                                MiMAC_DiscardPacket();
                                break;
                            }
                            if( currentChannel != rxMessage.Payload[1] )
                            {
                                MiMAC_DiscardPacket();
                                break;
                            }
                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_ACTIVE_SCAN_RESPONSE));
                            if (NULL == dataPtr)
                              return;
                            dataPtr[dataLen++] = CMD_P2P_ACTIVE_SCAN_RESPONSE;
                            dataPtr[dataLen++] = P2PCapacityInfo;
                            #if ADDITIONAL_NODE_ID_SIZE > 0
                                for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                {
                                    dataPtr[dataLen++] = (AdditionalNodeID[i]);
                                }
                            #endif
                            MiMAC_DiscardPacket();
                            
                            // unicast the response to the requesting device
                            #ifdef TARGET_SMALL
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr, 0, true, CommandConfCallback);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr,0, true, CommandConfCallback);
                                #endif
                            #else
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr, 0, true, CommandConfCallback);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr, 0, true, CommandConfCallback);
                                #endif
                            #endif
                        }
                        break;
                    
                        
                        
                    
                    
                    #ifndef TARGET_SMALL    
                    case CMD_P2P_CONNECTION_REMOVAL_REQUEST:
                        {
                            uint8_t* dataPtr = NULL;
                            uint8_t dataLen = 0;
                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_CONNECTION_REMOVAL_RESPONSE));
                            if (NULL == dataPtr)
                              return;
                            dataPtr[dataLen++] = CMD_P2P_CONNECTION_REMOVAL_RESPONSE;
                            for(i = 0; i < CONNECTION_SIZE; i++)
                            {
                                // if the record is valid
                                if( ConnectionTable[i].status.bits.isValid )
                                {
                                    // if the record is the same as the requesting device
                                    if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                                    {
                                        // find the record. disable the record and
                                        // set status to be SUCCESS
                                        ConnectionTable[i].status.Val = 0;
                                        #if defined(ENABLE_NETWORK_FREEZER)
											PDS_Store(PDS_CONNECTION_TABLE_ID);
                                        #endif
                                        dataPtr[dataLen++] = STATUS_SUCCESS;

                                        break;
                                    }
                                } 
                            }

                            MiMAC_DiscardPacket();

                            if( i == CONNECTION_SIZE ) 
                            {
                                // not found, the requesting device is not my peer
                                dataPtr[dataLen++] = STATUS_ENTRY_NOT_EXIST;
                            }
                            #ifdef TARGET_SMALL
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr,0, true, CommandConfCallback);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr,0, true, CommandConfCallback);
                                #endif
                            #else
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr,0, true, CommandConfCallback);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn, 
									dataLen, dataPtr, 0, true, CommandConfCallback);
                                #endif
                            #endif

                        }
                            break;
                    #endif
                    
                    case CMD_P2P_CONNECTION_RESPONSE:
                        {
                            switch( rxMessage.Payload[1] )
                            {
                                case STATUS_SUCCESS:
                                    if (EstConfCallback)
                                    {
                                        EstConfCallback(SUCCESS);
                                        EstConfCallback = NULL;
                                    }
                                case STATUS_EXISTS:
                                    if (EstConfCallback)
                                    {
                                        EstConfCallback(ALREADY_EXISTS);
                                        EstConfCallback = NULL;
                                    }
                                    #if defined(IEEE_802_15_4)
                                        if( myPANID.Val == 0xFFFF )
                                        {
                                            myPANID.Val = rxMessage.SourcePANID.Val;
                                            {
                                                uint16_t tmp = 0xFFFF;
                                                MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                                            }    
                                            #if defined(ENABLE_NETWORK_FREEZER)
												PDS_Store(PDS_PANID_ID);
                                            #endif
                                        }
                                    #endif
                                    AddConnection();
                                    #if defined(PROTOCOL_STAR)
                                        myConnectionIndex_in_PanCo = rxMessage.Payload[2];
                                        #if defined(ENABLE_NETWORK_FREEZER)
										PDS_Store(PDS_MYINDEX_ID);
										#endif
                                    #endif
                                    
                                    #if defined(ENABLE_NETWORK_FREEZER)
                                        P2PStatus.bits.SaveConnection = 1;
                                        nvmDelayTick.Val = MiWi_TickGet();
                                    #endif
                                    break;
                                default:
                                    break;
                            }                        
                        }
                        MiMAC_DiscardPacket();
                        break; 
                    
                    
                    case CMD_P2P_ACTIVE_SCAN_RESPONSE:
                        {
                            if( P2PStatus.bits.Resync )
                            {
                                P2PStatus.bits.Resync = 0;   
                            }
                            #ifdef ENABLE_ACTIVE_SCAN 
                                else   
                                {
                                    i = 0;
                                    for(; i < ActiveScanResultIndex; i++)
                                    {
                                        if( (ActiveScanResults[i].Channel == currentChannel) &&
                                        #if defined(IEEE_802_15_4)
                                            (ActiveScanResults[i].PANID.Val == rxMessage.SourcePANID.Val) &&
                                        #endif
                                            isSameAddress(ActiveScanResults[i].Address, rxMessage.SourceAddress)
                                        )
                                        {
                                            break;
                                        }
                                    }
                                    if( i == ActiveScanResultIndex && (i < ACTIVE_SCAN_RESULT_SIZE))
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Channel = currentChannel;
                                        ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxMessage.PacketRSSI;
                                        ActiveScanResults[ActiveScanResultIndex].LQIValue = rxMessage.PacketLQI;
                                        #if defined(IEEE_802_15_4)
                                            ActiveScanResults[ActiveScanResultIndex].PANID.Val = rxMessage.SourcePANID.Val;
                                        #endif
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].Address[i] = rxMessage.SourceAddress[i];
                                        }
                                        ActiveScanResults[ActiveScanResultIndex].Capability.Val = rxMessage.Payload[1];
                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                            {
                                                ActiveScanResults[ActiveScanResultIndex].PeerInfo[i] = rxMessage.Payload[2+i];
                                            }
                                        #endif
                                        ActiveScanResultIndex++;
                                    }
                                }
                            #endif

                            MiMAC_DiscardPacket(); 
                        }
                        break;                
                    
                    #ifndef TARGET_SMALL
                    case CMD_P2P_CONNECTION_REMOVAL_RESPONSE:
                        {
                            if( rxMessage.Payload[1] == STATUS_SUCCESS )
                            {
                                for(i = 0; i < CONNECTION_SIZE; i++)
                                {
                                    // if the record is valid
                                    if( ConnectionTable[i].status.bits.isValid )
                                    {
                                        // if the record address is the same as the requesting device
                                        if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                                        {
                                            // invalidate the record
                                            ConnectionTable[i].status.Val = 0;
                                            #if defined(ENABLE_NETWORK_FREEZER)
												PDS_Store(PDS_CONNECTION_TABLE_ID);
                                            #endif
                                            break;
                                        }
                                    } 
                                }
                            }
                        }
                        MiMAC_DiscardPacket();
                        break;
                    #endif
                #endif
                
                #ifdef ENABLE_INDIRECT_MESSAGE
                    case CMD_DATA_REQUEST:
                    case CMD_MAC_DATA_REQUEST: 
                        {
                            bool isCommand = false;
                            uint8_t* dataPtr = NULL;
                            uint8_t dataLen = 0;
                            MIWI_TICK tmpW;
                            
                            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_TIME_SYNC_DATA_PACKET));
                            if (NULL == dataPtr)
                              return;
                            
                            #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP_FEATURE)
                                dataPtr[dataLen++] = CMD_TIME_SYNC_DATA_PACKET;
                                isCommand = true;
                                tmpTick.Val = MiWi_TickGet();
                                //tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / (ONE_SECOND * 16);
                                //tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16);
                                tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16);
                                dataPtr[dataLen++] = tmpW.v[0];
                                dataPtr[dataLen++] = tmpW.v[1];
                                //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - (ONE_SECOND * tmpW.Val * 16) )) * 16 / 250));
                                //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - ((uint32_t)0xFFFF * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                                tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16 * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                                if( TimeSyncSlot < TIME_SYNC_SLOTS )
                                {
                                    TimeSyncSlot++;
                                }    
                                dataPtr[dataLen++] = tmpW.v[0];
                                dataPtr[dataLen++] = tmpW.v[1];

                            #endif
                            
                            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
                            {
                                if( indirectMessages[i].flags.bits.isValid )
                                {    
                                    uint8_t j;

                                    #ifdef ENABLE_BROADCAST
                                        if( indirectMessages[i].flags.bits.isBroadcast )
                                        {
                                            for(j = 0; j < CONNECTION_SIZE; j++)
                                            {
                                                if( indirectMessages[i].DestAddress.DestIndex[j] != 0xFF &&
                                                    isSameAddress(ConnectionTable[indirectMessages[i].DestAddress.DestIndex[j]].Address, rxMessage.SourceAddress) )
                                                {
                                                    indirectMessages[i].DestAddress.DestIndex[j] = 0xFF;
                                                    for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                                    {
                                                        dataPtr[dataLen++] = (indirectMessages[i].PayLoad[j]);
                                                    }
                                                    #if defined(ENABLE_TIME_SYNC)
                                                        if( indirectMessages[i].flags.bits.isCommand )
                                                        {
                                                            dataPtr[0] = CMD_TIME_SYNC_COMMAND_PACKET;
                                                        } 
                                                    #endif   
                                                    #if defined(IEEE_802_15_4)
                                                        SendPacket(false, indirectMessages[i].DestPANID, rxMessage.SourceAddress, isCommand, indirectMessages[i].flags.bits.isSecured,
														 dataLen, dataPtr,0, indirectMessages[i].flags.bits.ackReq, CommandConfCallback);
                                                    #else
                                                        SendPacket(false, rxMessage.SourceAddress, isCommand, indirectMessages[i].flags.bits.isSecured,
														 dataLen, dataPtr,0, indirectMessages[i].flags.bits.ackReq, CommandConfCallback);
                                                    #endif 
                                                    //goto DiscardPacketHere;
                                                    goto END_OF_SENDING_INDIRECT_MESSAGE;
                                                }
                                            }         
                                        }
                                        else 
                                    #endif
                                    if( isSameAddress(indirectMessages[i].DestAddress.DestLongAddress, rxMessage.SourceAddress) )
                                    {                          
                                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                        {
                                            dataPtr[dataLen++] =indirectMessages[i].PayLoad[j];
                                        }
                                        #if defined(ENABLE_TIME_SYNC)
                                            if( indirectMessages[i].flags.bits.isCommand )
                                            {
                                                dataPtr[0] = CMD_TIME_SYNC_COMMAND_PACKET;
                                            } 
                                        #endif 
                                        #if defined(IEEE_802_15_4)
                                            SendPacket(false, indirectMessages[i].DestPANID, indirectMessages[i].DestAddress.DestLongAddress, isCommand, (bool)indirectMessages[i].flags.bits.isSecured,
											 dataLen, dataPtr,0, indirectMessages[i].flags.bits.ackReq, CommandConfCallback);
                                        #else
                                            SendPacket(false, indirectMessages[i].DestAddress.DestLongAddress, isCommand, (bool)indirectMessages[i].flags.bits.isSecured,
											 dataLen, dataPtr,0, indirectMessages[i].flags.bits.ackReq, CommandConfCallback);
                                        #endif
                                        indirectMessages[i].flags.Val = 0;   
                                        goto END_OF_SENDING_INDIRECT_MESSAGE;    
                                    }    
                                }
                            }
                           
                            if( i == INDIRECT_MESSAGE_SIZE )
                            {
                                #ifdef TARGET_SMALL
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, myPANID, rxMessage.SourceAddress, isCommand, false, 
										dataLen, dataPtr,0,true, CommandConfCallback);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, isCommand, false, 
										dataLen, dataPtr,0, true,  CommandConfCallback);
                                    #endif
                                #else
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, isCommand, false, 
										dataLen, dataPtr,0, true, CommandConfCallback);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, isCommand, false, 
										dataLen, dataPtr,0, true, CommandConfCallback);
                                    #endif
                                #endif
                            }
                            
END_OF_SENDING_INDIRECT_MESSAGE:
                            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                                if( MACRxPacket.PayloadLen > 1 )
                                {
                                    rxMessage.Payload = &(MACRxPacket.Payload[1]);
                                    rxMessage.PayloadSize--;
                                    P2PStatus.bits.RxHasUserData = 1;
									pktRxcallback(&rxMessage);
									P2PStatus.bits.RxHasUserData = 0;
									MiMAC_DiscardPacket();
                                }
                                else    
                            #endif                        
                            MiMAC_DiscardPacket();
                        }    
                        break;
                #endif
                
                
                #if defined(ENABLE_TIME_SYNC) && defined(ENABLE_SLEEP_FEATURE)
                    case CMD_TIME_SYNC_DATA_PACKET:
                    case CMD_TIME_SYNC_COMMAND_PACKET:
                        {
                            WakeupTimes.v[0] = rxMessage.Payload[1];
                            WakeupTimes.v[1] = rxMessage.Payload[2];
                            CounterValue.v[0] = rxMessage.Payload[3];
                            CounterValue.v[1] = rxMessage.Payload[4];

                            if( rxMessage.PayloadSize > 5 )
                            {
                                if( rxMessage.Payload[0] == CMD_TIME_SYNC_DATA_PACKET )
                                {
                                    rxMessage.flags.bits.command = 0;
                                }    
                                rxMessage.PayloadSize -= 5;
                                rxMessage.Payload = &(rxMessage.Payload[5]);
                                P2PStatus.bits.RxHasUserData = 1;
								pktRxcallback(&rxMessage);
								P2PStatus.bits.RxHasUserData = 0;
								MiMAC_DiscardPacket();
                            }  
                            else
                            {
                                P2PStatus.bits.DataRequesting = 0;
                                MiMAC_DiscardPacket();
                            }      
                        }
                        break;    
                #endif
                
                
                     
                #if defined(ENABLE_FREQUENCY_AGILITY) 
                    case CMD_CHANNEL_HOPPING:
                        if( rxMessage.Payload[1] != currentChannel )
                        {
                            MiMAC_DiscardPacket();
                            break;
                        }
                        StartChannelHopping(rxMessage.Payload[2]);
                       // #if defined(EIGHT_BIT_WIRELESS_BOARD)
                            printf("\r\nHopping Channel to %d ",rxMessage.Payload[2] );
                       // #endif
                        
                        MiMAC_DiscardPacket();
                        break;
                    
                #endif

                #if defined (PROTOCOL_STAR)
                    case CMD_DATA_TO_ENDDEV_SUCCESS:
                        {
                            if (!role)
                            {
                                rec_ack = true;
                                MiMAC_DiscardPacket();
                            }
                            else
                            {
                                MiMAC_DiscardPacket();
                            }
                        }
                        break;
                    case CMD_SHARE_CONNECTION_TABLE:
                        {
                          if (!role)
                            {
                                // END_devices FFD|| RFD process this Packet
                                end_nodes = rxMessage.Payload[1]-1; 
                                store_connection_tb(rxMessage.Payload);
                                MiMAC_DiscardPacket();
                            }
                          else
                              MiMAC_DiscardPacket();
                        }
                        break;

                    case CMD_FORWRD_PACKET:
                        {
                            if (role)
                            {
                                // PAN CO handles the Packet Forwards 
                                uint8_t final_index; // EDy Index which is the destination address
                                final_index = Find_Index(rxMessage.Payload); 
                                if (final_index != 0xFF)
                                {
                                    FW_Stat = Forward_Packet_PANCr_to_EDy (rxMessage.PayloadSize, rxMessage.Payload , final_index , rxMessage.SourceAddress);
                                }
                                #if defined(ENABLE_APP_LAYER_ACK)
                                    if (FW_Stat)
                                    {
                                            delay_ms(100);
											SW_Ack_SrED (rxMessage.SourceAddress);
                                    }
                                #endif
                                        MiMAC_DiscardPacket();
                           }
                            
                            else
                                MiMAC_DiscardPacket();          
                        }
                        break;
                    #if defined(ENABLE_LINK_STATUS)
                        case CMD_IAM_ALIVE:
                        {
                            if (role)
                            {
                                // PAN CP processes this packet to qualify it as alive , increments the link stat
                                uint8_t p;
                                for (p=0  ; p < CONNECTION_SIZE ; p++)
                                {
                                        if (ConnectionTable[p].Address[0] == rxMessage.SourceAddress[0] && ConnectionTable[p].Address[1] == rxMessage.SourceAddress[1]
                                                && ConnectionTable[p].Address[2] == rxMessage.SourceAddress[2])
                                        {
                                            link_stat[p]++;
                                            break;
                                        }
                                }
                                MiMAC_DiscardPacket();
                            }
                            else
                            {
                                MiMAC_DiscardPacket();
                            }

                        }
                        break;
                    #endif
                #endif
                
                        
                default:
                    // let upper application layer to handle undefined command frame
                    P2PStatus.bits.RxHasUserData = 1;
					pktRxcallback(&rxMessage);
					P2PStatus.bits.RxHasUserData = 0;
					MiMAC_DiscardPacket();
                    break;
            }
        }
        else
        {
            P2PStatus.bits.RxHasUserData = 1;
			pktRxcallback(&rxMessage);
			P2PStatus.bits.RxHasUserData = 0;
			MiMAC_DiscardPacket();
        }

        #ifdef ENABLE_SLEEP_FEATURE
            if( P2PStatus.bits.DataRequesting && P2PStatus.bits.RxHasUserData )
            {
                P2PStatus.bits.DataRequesting = 0;
            }
        #endif       
        if( rxMessage.PayloadSize == 0  || P2PStatus.bits.SearchConnection || P2PStatus.bits.Resync )
        {
            P2PStatus.bits.RxHasUserData = 0;
            MiMAC_DiscardPacket();
        }   
    }
#if defined(ENABLE_NETWORK_FREEZER)
#if PDS_ENABLE_WEAR_LEVELING
    PDS_TaskHandler();
#endif
#endif
    (void)tmpTick;
}


bool MiApp_ProtocolInit(defaultParametersRomOrRam_t *defaultRomOrRamParams,
                              defaultParametersRamOnly_t *defaultRamOnlyParams)
{
    uint8_t i = 0;
    
    MACINIT_PARAM initValue;
    
    #if defined(ENABLE_NVM)
        #if defined(ENABLE_NVM_MAC)
            uint8_t i;
            if( MY_ADDRESS_LENGTH > 6 )
            {
                for(i = 0; i < 3; i++)
                {
                    EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                }
                myLongAddress[4] = 0xFF;
                if( MY_ADDRESS_LENGTH > 7 )
                {
                    myLongAddress[3] = 0xFE;
                }
                for(i = 0; i < 3; i++)
                {
                    EEPROM_Read(&(myLongAddress[2-i]), EEPROM_MAC_ADDR+3+i, 1);
                }
            }
            else
            {
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                }   
            }
        #endif
        
    #endif
    
    //clear all status bits
    P2PStatus.Val = 0;

    #if defined(ENABLE_NETWORK_FREEZER)
    {
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            ConnectionTable[i].status.Val = 0;
            ConnectionTable[i].Address[0] = 0x00;
            ConnectionTable[i].Address[1] = 0x00;
            ConnectionTable[i].Address[2] = 0x00;
            #if defined(ENABLE_LINK_STATUS)
                link_stat[i] = 0x00;
                permanent_connections[i] = 0x00;
            #endif
        }
    }
	#endif
    
    #ifdef ENABLE_INDIRECT_MESSAGE
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            indirectMessages[i].flags.Val = 0;
        }
    #endif   
        
    #if defined(ENABLE_SECURITY)
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            IncomingFrameCounter[i].Val = 0;
        }
    #endif
    
#if defined(ENABLE_NETWORK_FREEZER)
        #if defined(IEEE_802_15_4)
			PDS_Restore(PDS_PANID_ID);
        #endif
        if (myPANID.Val)
		{
			PDS_Restore(PDS_CURRENT_CHANNEL_ID);
            if( currentChannel >= 32 )
            {
                return false;
            }
            
            #if defined(IEEE_802_15_4)
			PDS_Restore(PDS_PANID_ID);
            #endif
			PDS_Restore(PDS_CONNECTION_MODE_ID);
			PDS_Restore(PDS_CONNECTION_TABLE_ID);

            #if defined(PROTOCOL_STAR)
			PDS_Restore(PDS_ROLE_ID);
			PDS_Restore(PDS_MYINDEX_ID);
            #endif
			PDS_Restore(PDS_EDC_ID);
                        
            #if defined (ENABLE_CONSOLE)
                #if defined(IEEE_802_15_4) 
                printf("\r\nPANID:");
                printf("%x",myPANID.v[1]);
                printf("%x",myPANID.v[0]);
                #endif
                printf(" Channel:");
                printf("%d",currentChannel);
            #endif
        }
        else
        {
            #if defined(IEEE_802_15_4)
                myPANID.Val = MY_PAN_ID;
				PDS_Store(PDS_PANID_ID);
            #endif
			PDS_Store(PDS_CURRENT_CHANNEL_ID);
			PDS_Store(PDS_CONNECTION_MODE_ID);
			PDS_Store(PDS_CONNECTION_TABLE_ID);
        }
    #else
        #if defined(IEEE_802_15_4)
            myPANID.Val = MY_PAN_ID; 
        #endif
    #endif
    
    initValue.PAddress = myLongAddress;
    initValue.actionFlags.bits.CCAEnable = 1;
    initValue.actionFlags.bits.PAddrLength = MY_ADDRESS_LENGTH;
#if defined(ENABLE_NETWORK_FREEZER)	
    initValue.actionFlags.bits.NetworkFreezer = 1;
#else
    initValue.actionFlags.bits.NetworkFreezer = 0;
#endif
    initValue.actionFlags.bits.RepeaterMode = 0;

    MiMAC_Init(initValue);
    
    if (currentChannel != 0xFF)
	    MiApp_Set(CHANNEL, &currentChannel);

	#if defined(IEEE_802_15_4)
        {
            uint16_t tmp = 0xFFFF;
            MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
        }
    #endif

    #if defined(ENABLE_TIME_SYNC)
        #if defined(ENABLE_SLEEP_FEATURE)
            WakeupTimes.Val = 0;
            CounterValue.Val = 61535;   // (0xFFFF - 4000) one second
        #elif defined(ENABLE_INDIRECT_MESSAGE)
            TimeSlotTick.Val = ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) / TIME_SYNC_SLOTS;
        #endif
    #endif

    P2PCapacityInfo = 0;
    #if !defined(ENABLE_SLEEP_FEATURE)
        P2PCapacityInfo |= 0x01;
    #endif
    #if defined(ENABLE_SECURITY)
        P2PCapacityInfo |= 0x08;
    #endif
    P2PCapacityInfo |= (ConnMode << 4);

    (void)i;
    return true;
}

#ifdef ENABLE_SLEEP_FEATURE
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_TransceiverPowerState(uint8_t Mode)
     *
     * Summary:
     *      This function put the RF transceiver into different power state. i.e. Put the 
     *      RF transceiver into sleep or wake it up.
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      put RF transceiver into sleep or wake it up. This function is only available
     *      to those wireless nodes that may have to disable the transceiver to save 
     *      battery power.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint8_t Mode - The mode of power state for the RF transceiver to be set. The possible
     *                  power states are following
     *                  * POWER_STATE_SLEEP     The deep sleep mode for RF transceiver
     *                  * POWER_STATE_WAKEUP    Wake up state, or operating state for RF transceiver
     *                  * POWER_STATE_WAKEUP_DR Put device into wakeup mode and then transmit a 
     *                                          data request to the device's associated device
     *
     * Returns: 
     *      The status of the operation. The following are the possible status
     *      * SUCCESS           Operation successful
     *      * ERR_TRX_FAIL      Transceiver fails to go to sleep or wake up
     *      * ERR_TX_FAIL       Transmission of Data Request command failed. Only available if the
     *                          input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_RX_FAIL       Failed to receive any response to Data Request command. Only available
     *                          if input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_INVLAID_INPUT Invalid input mode. 
     *
     * Example:
     *      <code>
     *      // put RF transceiver into sleep
     *      MiApp_TransceiverPowerState(POWER_STATE_SLEEP;
     *
     *      // Put the MCU into sleep
     *      Sleep();    
     *
     *      // wakes up the MCU by WDT, external interrupt or any other means
     *      
     *      // make sure that RF transceiver to wake up and send out Data Request
     *      MiApp_TransceiverPowerState(POWER_STATE_WAKEUP_DR);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    uint8_t MiApp_TransceiverPowerState(INPUT uint8_t Mode)
    {
        //uint8_t status;
        
        switch(Mode)
        {
            case POWER_STATE_SLEEP:
                {
                    #if defined(ENABLE_NETWORK_FREEZER)
                        if( P2PStatus.bits.SaveConnection )
                        {
							PDS_Store(PDS_CONNECTION_TABLE_ID);

                            P2PStatus.bits.SaveConnection = 0;
                        }
                    #endif
                    if( MiMAC_PowerState(POWER_STATE_DEEP_SLEEP) )
                    {
                        P2PStatus.bits.Sleeping = 1;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
                
            case POWER_STATE_WAKEUP:
                {
                    if( MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        P2PStatus.bits.Sleeping = 0;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
               
            case POWER_STATE_WAKEUP_DR:
                {
                    if( false == MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        return ERR_TRX_FAIL;
                    }
                    P2PStatus.bits.Sleeping = 0;
                    if( CheckForData() == false )
                    {
                        return ERR_TX_FAIL;
                    }
                    while( P2PStatus.bits.DataRequesting ) 
                    {
                        P2PTasks();
                    }
                    return SUCCESS;
                }
                
             default:
                break;

        }
        
        return ERR_INVALID_INPUT;    
    }

     
     /*********************************************************************
     * BOOL CheckForData(void)
     *
     * Overview:        This function sends out a Data Request to the peer
     *                  device of the first P2P connection. 
     *
     * PreCondition:    Transceiver is initialized and fully waken up
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The P2P stack is waiting for the response from
     *                  the peer device. A data request timer has been
     *                  started. In case there is no response from the
     *                  peer device, the data request will time-out itself
     *
     ********************************************************************/
     bool CheckForData(void)
     {
        uint8_t* dataPtr = NULL;
        uint8_t dataLen = 0;
		uint8_t tmpTxData = dataLen;
        uint8_t firstuint8_t = dataPtr[0];

        dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_MAC_DATA_REQUEST));
        if (NULL == dataPtr)
          return false;
        dataPtr[dataLen++] = CMD_MAC_DATA_REQUEST;

        #if defined(ENABLE_ENHANCED_DATA_REQUEST)
            if( tmpTxData > 0 )
            {
                uint8_t i;
                
                for( i = tmpTxData; i > 1; i--)
                {
                    dataPtr[dataLen++] = dataPtr[i-1];
                }
                dataPtr[1] = firstuint8_t;
                dataLen = tmpTxData + 1;  
            }    
        #endif
                    
        #if defined(IEEE_802_15_4)
            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                if( SendPacket(false, myPANID, ConnectionTable[0].Address, true, P2PStatus.bits.Enhanced_DR_SecEn, 
				dataLen, dataPtr,0, true, CommandConfCallback) )
            #else
                if( SendPacket(false, myPANID, ConnectionTable[0].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback) )
            #endif
        #else
            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                if( SendPacket(false, ConnectionTable[0].Address, true, P2PStatus.bits.Enhanced_DR_SecEn, 
				dataLen, dataPtr,0, true, CommandConfCallback) )
            #else
                if( SendPacket(false, ConnectionTable[0].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback) )
            #endif
        #endif
        {
            P2PStatus.bits.DataRequesting = 1; 
            #if defined(ENABLE_ENHANCED_DATA_REQUEST) 
                P2PStatus.bits.Enhanced_DR_SecEn = 0;
            #endif
            DataRequestTimer.Val = MiWi_TickGet();
            dataPtr[0] = firstuint8_t;
            dataLen = tmpTxData;
            #if defined(ENABLE_TIME_SYNC)
                #if defined(__18CXX)
                    TMR3H = 0;
                    TMR3L = 0;
                #elif defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__)
                    PR1 = 0xFFFF;
                    TMR1 = 0;
                #elif defined(__PIC32MX__)
                    PR1 = 0xFFFF;
                    while(T1CONbits.TWIP) ;
                    TMR1 = 0;
                #endif
            #endif
            return true;
        }
        dataPtr[0] = firstuint8_t;
        dataLen = tmpTxData;
        #if defined(ENABLE_ENHANCED_DATA_REQUEST)
            P2PStatus.bits.Enhanced_DR_SecEn = 0;
        #endif
        return false;
     }
 #endif
 

#ifdef ENABLE_INDIRECT_MESSAGE
    
    /*********************************************************************
     * BOOL IndirectPacket(BOOL Broadcast, 
     *                     uint16_t_VAL DestinationPANID,
     *                     uint8_t *DestinationAddress,
     *                     BOOL isCommand,
	 *                     INPUT uint8_t msghandle,
	 *                     INPUT bool ackReq,
     *                     BOOL SecurityEnabled)
     *
     * Overview:        This function store the indirect message for node
     *                  that turns off radio when idle     
     *
     * PreCondition:    None
     *
     * Input:           Broadcast           - Boolean to indicate if the indirect
     *                                        message a broadcast message
     *                  DestinationPANID    - The PAN Identifier of the 
     *                                        destination node
     *                  DestinationAddress  - The pointer to the destination
     *                                        long address
     *                  isCommand           - The boolean to indicate if the packet
     *                                        is command
     *                  SecurityEnabled     - The boolean to indicate if the 
     *                                        packet needs encryption
     *
     * Output:          boolean to indicate if operation successful
     *
     * Side Effects:    An indirect message stored and waiting to deliever
     *                  to sleeping device. An indirect message timer
     *                  has started to expire the indirect message in case
     *                  RFD does not acquire data in predefined interval
     *
     ********************************************************************/
    #if defined(IEEE_802_15_4)
        bool IndirectPacket(INPUT bool Broadcast, 
                            API_UINT16_UNION  DestinationPANID,
                            INPUT uint8_t *DestinationAddress,
                            INPUT bool isCommand, 
                            INPUT bool SecurityEnabled,
                            INPUT uint8_t msgLen,
                            INPUT uint8_t* msgPtr,
							INPUT uint8_t msghandle,
							INPUT bool ackReq,
                            DataConf_callback_t ConfCallback)
    #else
        bool IndirectPacket(INPUT bool Broadcast, 
                            INPUT uint8_t *DestinationAddress,
                            INPUT bool isCommand, 
                            INPUT bool SecurityEnabled,
							INPUT uint8_t msgLen,
							INPUT uint8_t* msgPtr,
							INPUT uint8_t msghandle,
							INPUT bool ackReq,
                            DataConf_callback_t ConfCallback)
    #endif                            
    { 
        uint8_t i;
        
        #ifndef ENABLE_BROADCAST
            if( Broadcast )
            {
                return false;
            }
        #endif

        // loop through the available indirect message buffer and locate
        // the empty message slot
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid == 0 )
            {
                uint8_t j;
                
                // store the message
                indirectMessages[i].flags.bits.isValid          = true;
                indirectMessages[i].flags.bits.isBroadcast      = Broadcast;
                indirectMessages[i].flags.bits.isCommand        = isCommand;
                indirectMessages[i].flags.bits.isSecured        = SecurityEnabled;
				indirectMessages[i].flags.bits.ackReq           = ackReq;
                #if defined(IEEE_802_15_4)
                    indirectMessages[i].DestPANID.Val           = DestinationPANID.Val;
                #endif
                if( DestinationAddress != NULL )
                {
                    for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                    {
                        indirectMessages[i].DestAddress.DestLongAddress[j] = DestinationAddress[j];
                    }
                }
                #ifdef ENABLE_BROADCAST
                    else
                    {
                        uint8_t k = 0;
    
                        for(j = 0; j < CONNECTION_SIZE; j++)
                        {
                            //if( (ConnectionTable[j].PeerInfo[0] & 0x83) == 0x82 )
                            if( ConnectionTable[j].status.bits.isValid &&
                                ConnectionTable[j].status.bits.RXOnWhenIdle == 0 )
                            {
                                indirectMessages[i].DestAddress.DestIndex[k++] = j;
                            }
                        }
                        for(; k < CONNECTION_SIZE; k++)
                        {
                            indirectMessages[i].DestAddress.DestIndex[k] = 0xFF;
                        }
                    }
                #endif
                
                indirectMessages[i].PayLoadSize = msgLen;
                for(j = 0; j < msgLen; j++)
                {
                    indirectMessages[i].PayLoad[j] = msgPtr[j];
                }
                indirectMessages[i].indirectDataHandle = msghandle;
				indirectMessages[i].indirectConfCallback = ConfCallback;
				indirectMessages[i].TickStart.Val = MiWi_TickGet();
                return true;
            }
        }
        return false;
    }
#endif


/*********************************************************************
 * BOOL SendPacket(BOOL Broadcast, 
 *                 uint16_t_VAL DestinationPANID,
 *                 uint8_t *DestinationAddress,
 *                 BOOL isCommand, 
 *                 BOOL SecurityEnabled)
 *
 * Overview:        This function sends the packet  
 *
 * PreCondition:    Transceiver is initialized
 *
 * Input:     
 *          BOOL        Broadcast           If packet to send needs to be broadcast
 *          uint16_t_VAL    DestinationPANID    Destination PAN Identifier
 *          uint8_t *      DestinationAddress  Pointer to destination long address
 *          BOOL        isCommand           If packet to send is a command packet
 *          BOOL        SecurityEnabled     If packet to send needs encryption
 *                  
 * Output: 
 *          BOOL                            If operation successful
 *
 * Side Effects:    Transceiver is triggered to transmit a packet
 *
 ********************************************************************/
#if defined(IEEE_802_15_4)
    bool SendPacket(INPUT bool Broadcast,
                    API_UINT16_UNION DestinationPANID,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT bool SecurityEnabled,
                    INPUT uint8_t msgLen,
                    INPUT uint8_t* msgPtr,
                    INPUT uint8_t msghandle,
					INPUT bool ackReq,
                    INPUT DataConf_callback_t ConfCallback)
#else
    bool SendPacket(INPUT bool Broadcast,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT uint8_t msgLen,
                    INPUT uint8_t* msgPtr,
                    INPUT uint8_t msghandle,
					INPUT bool ackReq,
                    INPUT DataConf_callback_t ConfCallback)
#endif                                        
{ 
    MAC_TRANS_PARAM tParam;
    bool status;

    tParam.flags.Val = 0;
    tParam.flags.bits.packetType = (isCommand) ? PACKET_TYPE_COMMAND : PACKET_TYPE_DATA;
    tParam.flags.bits.ackReq = (Broadcast) ? 0 : ackReq;
    tParam.flags.bits.broadcast = Broadcast;
    tParam.flags.bits.secEn = SecurityEnabled;
    #if defined(IEEE_802_15_4)
        tParam.altSrcAddr = 0;
        tParam.altDestAddr = (Broadcast) ? true : false;
    #endif
    
    #if defined(INFER_DEST_ADDRESS)
        tParam.flags.bits.destPrsnt = 0;
    #else
        tParam.flags.bits.destPrsnt = (Broadcast) ? 0:1;
    #endif
    
    #if defined(SOURCE_ADDRESS_ABSENT)
        if( tParam.flags.bits.packetType == PACKET_TYPE_COMMAND )
        {
            tParam.flags.bits.sourcePrsnt = 1;
        }
        else
        {
            tParam.flags.bits.sourcePrsnt = 0;
        }
    #else
        tParam.flags.bits.sourcePrsnt = 1;
    #endif
    
    tParam.DestAddress = DestinationAddress;

    #if defined(IEEE_802_15_4)
        tParam.DestPANID.Val = DestinationPANID.Val;
    #endif

    status = MiMAC_SendPacket(tParam, msgPtr, msgLen, msghandle, ConfCallback);
    
    return status;
}

    /************************************************************************************
     * Function:
     * bool MiApp_SendData(uint8_t addr_len, uint8_t *addr, uint8_t msglen, uint8_t *msgpointer, 
                                                        uint8_t msghandle, DataConf_callback_t ConfCallback);
     *
     * Summary:
     *      This function unicast a message in the msgpointer to the device with DestinationAddress 
     *
     * Description:        
     *      This is one of the primary user interface functions for the application layer to 
     *      unicast a message. The destination device is specified by the input parameter 
     *      DestinationAddress. The application payload is filled in msgpointer.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint8_t addr_len - destionation address length
     *      uint8_t *addr  - destionation address
     *      uint8_t msglen - length of the message    
     *      uint8_t *msgpointer - message/frame pointer
	 *      uint8_t msghandle - message handle
     *      DataConf_callback_t ConfCallback - The callback routine which will be called upon
     *                                               the initiated data procedure is performed	
     *
     * Returns: 
     *      A boolean to indicates if the unicast procedure is succcessful.
     *
     * Example:
     *      <code>
     *      // Secure and then broadcast the message stored in msgpointer to the permanent address
     *      // specified in the input parameter.
     *      MiApp_SendData(SHORT_ADDR_LEN, 0x0004, len, frameptr,1, callback);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/      
    bool MiApp_SendData(uint8_t addr_len, uint8_t *addr, uint8_t msglen, uint8_t *msgpointer, uint8_t msghandle,
                                                            bool ackReq, DataConf_callback_t ConfCallback)
   {
	    bool broadcast = false;
		uint16_t DestinationAddress16 = ((addr[1] << 8) + addr[0]);	
		if(addr_len == 2 && (DestinationAddress16 == 0xFFFF))
		{
			broadcast = true;
			    #ifdef ENABLE_INDIRECT_MESSAGE
			    uint8_t i;
			    
			    for(i = 0; i < CONNECTION_SIZE; i++)
			    {
				    if( ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.RXOnWhenIdle == 0 )
				    {
					    #if defined(IEEE_802_15_4)
					    IndirectPacket(true, myPANID, NULL, false, true, msglen, msgpointer, msghandle, ackReq, ConfCallback);
					    #else
					    IndirectPacket(true, NULL, false, true, msglen, msgpointer, msghandle, ackReq, ConfCallback);
					    #endif
					    break;
				    }
			    }
			    #endif
		}
		else
		{
    #ifdef ENABLE_INDIRECT_MESSAGE
        uint8_t i;
        
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            // check if RX on when idle
            if( ConnectionTable[i].status.bits.isValid && (ConnectionTable[i].status.bits.RXOnWhenIdle == 0) &&
                isSameAddress(addr, ConnectionTable[i].Address) )
            {
                #if defined(IEEE_802_15_4)
                    return IndirectPacket(broadcast, myPANID, addr, false, true, msglen,
					 msgpointer, msghandle, ackReq, ConfCallback);
                #else
                    return IndirectPacket(broadcast, addr, false, true, msglen,
					 msgpointer, msghandle, ackReq, ConfCallback);
                #endif
            }
        }
    #endif
		}
    
    #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEPING)
        if( P2PStatus.bits.Sleeping )
        {
            P2PStatus.bits.Enhanced_DR_SecEn = SecEn;
            return true;
        }    
    #endif
    
    #if defined(IEEE_802_15_4)
        return SendPacket(broadcast, myPANID, addr, false, true, msglen,
		 msgpointer, msghandle, ackReq, ConfCallback);
    #else
        return SendPacket(broadcast, addr, false, true, msglen,
		 msgpointer, msghandle, ackReq, ConfCallback);
    #endif
}

#if defined(PROTOCOL_STAR)
    /************************************************************************************
    * Function:
    *      void MiApp_BroadcastConnectionTable(void)
    *
    * Summary:
    *      This function is  used for command type packet TX (Only used by PAN CO)
    *
    * Description:        
    *      This function is used by only PAN CO in a Star network and is a cmd
    *      type packet. PAN CO in Star Network , holds the responsibility to Share 
    *      peer end devices connection table.
    *
    * PreCondition:    
    *      Protocol initialization has been done. 
    *
    * Returns: 
    *      None.
    *
    * Remarks:    
    *      None
    *
    *****************************************************************************************/    
    void MiApp_BroadcastConnectionTable(void)
    {
        uint8_t i,j , k , count;
        // Based on Connection Size in Network braodcast the connection details Multiple Times 
        // so that all the END_DEVICES in Star Network Receive the packet 
        uint8_t broadcast_count = 0;
        uint8_t* dataPtr = NULL;
        uint8_t dataLen = 0;

        if ((conn_size  * 4 ) + 4 < TX_BUFFER_SIZE)
        {
            broadcast_count = 1;
        }
        else
        {
            broadcast_count = ((conn_size * 4) + 4 )/ TX_BUFFER_SIZE;
            if ((conn_size *4) + 4 % TX_BUFFER_SIZE != 0)
            {
                broadcast_count = broadcast_count + ((conn_size *4) + 4 )% TX_BUFFER_SIZE;
            }
            
        }
        
        for (i = 0 ; i < broadcast_count ; i++)
        {
            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(TX_BUFFER_SIZE));
            if (NULL == dataPtr)
               return;
            dataPtr[dataLen++] = CMD_SHARE_CONNECTION_TABLE;
            dataPtr[dataLen++] = conn_size; // No of end devices in network
            dataPtr[dataLen++] = (((TX_BUFFER_SIZE-4)/4)*i);
            dataPtr[dataLen++] = (((TX_BUFFER_SIZE-4)/4)*(i+1));
            count = 4;
            for (j= ((TX_BUFFER_SIZE-4)/4)*i ;j<((TX_BUFFER_SIZE-4)/4)*(i+1);j++)
            {
                if (j < conn_size)
                {
                    if (ConnectionTable[j].status.bits.isValid)
                    {

                        dataPtr[dataLen++] = (ConnectionTable[j].Address[0]);
                        dataPtr[dataLen++] = (ConnectionTable[j].Address[1]);
                        dataPtr[dataLen++] = (ConnectionTable[j].Address[2]);
                        dataPtr[dataLen++] = j;

                    }

                    else
                    {

                        dataPtr[dataLen++] = 0xff;
                        dataPtr[dataLen++] = 0xff;
                        dataPtr[dataLen++] = 0xff;
                        dataPtr[dataLen++] = j;             
                    }
                    count = count + 4;
                }
            }
            // Fill the remaining buffer with garbage value
            for (k=count;k<TX_BUFFER_SIZE;k++)
            {
                dataPtr[dataLen++] = 0xFF;   // Garbage Value 
            }



            #if defined(IEEE_802_15_4)

                SendPacket(true, myPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
            #else
                SendPacket(true, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
            #endif

 
        }
    }

    bool MiApp_UnicastStar  (uint8_t dataLen, uint8_t* dataPtr, bool SecEn)
    {
        rec_ack = false;
        bool status;
        
        #if defined(IEEE_802_15_4)
            status = SendPacket(false, myPANID, ConnectionTable[0].Address, true, true, 
			dataLen, dataPtr, 0, true, CommandConfCallback);
        #else
            status = SendPacket(false,  ConnectionTable[0].Address, true, 
			dataLen, dataPtr, 0, true, CommandConfCallback);
        #endif


        #if defined(ENABLE_APP_LAYER_ACK)
            MIWI_TICK t1, t2;  // Timers which Handle Ack from the end devices to denote successfull Transmission of Data
            // from EDx to EDy  (End Devices)
            t1.Val = MiWi_TickGet();   // Get current time
            while (!rec_ack)
            {
                t2.Val = MiWi_TickGet();   // Get current time
                P2PTasks(); // False Input parameter as we co ordinator is trying to send Data
                //do
                if(MiWi_TickGetDiff(t2, t1) > (SW_ACK_TIMEOUT))
                {
                    if (rec_ack != true)
                    {
                        break;
                    }
                }
            }
        #endif

        #if defined(ENABLE_APP_LAYER_ACK)
            (void)status;
            return rec_ack; 
        #else
            return status;
        #endif
    }
    
    void MiApp_leave_network( void)
    {
        uint8_t* dataPtr = NULL;
        uint8_t dataLen = 0;
        dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_CONNECTION_REMOVAL_REQUEST));
        if (NULL == dataPtr)
          return;
        dataPtr[dataLen++] = CMD_P2P_CONNECTION_REMOVAL_REQUEST; 
        // Pan Co is @ index 0 of connection table of END_Device
        // In a Star Network
        #if defined(IEEE_802_15_4)
            SendPacket(false, myPANID, ConnectionTable[0].Address, true, false, 
			dataLen, dataPtr,0, true, CommandConfCallback);
        #else
            SendPacket(false,  ConnectionTable[0].Address, true, false, 
			dataLen, dataPtr,0, true, CommandConfCallback);
        #endif
        
    }
 

    #if defined(ENABLE_LINK_STATUS)

        
        void send_link_status (void)
        {
            bool send_status;
            uint8_t* dataPtr = NULL;
            uint8_t dataLen = 0;
            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_CMD_IAM_ALIVE));
            if (NULL == dataPtr)
              return;			
            dataPtr[dataLen++] = CMD_IAM_ALIVE; 
            // Pan Co is @ index 0 of connection table of END_Device
            // In a Star Network
            #if defined(IEEE_802_15_4)
                send_status = SendPacket(false, myPANID, ConnectionTable[0].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback);
            #else
                send_status = SendPacket(false,  ConnectionTable[0].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback);
            #endif
                if(!send_status)
                {
                    lost_connection = true;
                }
        }
        
        
        void Find_InActiveDevices(void)
        {
            uint8_t i;
            for (i=0;i<CONNECTION_SIZE;i++)
            {
                if (ConnectionTable[i].status.bits.isValid)
                {
                    if (link_stat[i] == 0 && permanent_connections[i] != 0xFF)
                    {
                        MiApp_RemoveConnection(i);
                    }
                    else
                    {
                        link_stat[i] = 0; 
                    }
                }
            }
        }


    #endif
    #if defined(ENABLE_APP_LAYER_ACK)
        /************************************************************************************
         * Function:
         *      bool SW_Ack_SrED(uint8_t *)
         *
         * Summary:
         *      Pan CO uses this function to send APP_layer_ACK 
         *
         * Description:        
         *      This function is used by PAN Co to send a App_layer ACK to source END_DEVICE to denote 
         *      a successful Data Reception by END_DEVICE
         *      Data TX from one END_DEVICE to another END_DEVICE
         *      EDx --> PAN CO --> EDy
         *      PAN CO generates a APP_Layer_ACK for EDx to denote a Successful TX.
         *      APP_LAYER_ACK    :   EDx    <--PANC0
      * PreCondition:    
      *      MiApp_Send_Data_Packet_EDx_to_EDy   
      *
      * Returns: 
      *      bool - true or false Packet Sending Success or Fail
      *
      * Remarks:    
      *      None
      *
      *****************************************************************************************/ 
        bool SW_Ack_SrED(uint8_t * Dest_Addr )
        {
            uint8_t* dataPtr = NULL;
            uint8_t dataLen = 0;
            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_CMD_DATA_TO_ENDDEV_SUCCESS));
            if (NULL == dataPtr)
                return false;
            dataPtr[dataLen++] = CMD_DATA_TO_ENDDEV_SUCCESS; 
            #if defined(IEEE_802_15_4)
                return SendPacket(false, myPANID, Dest_Addr, true, true, dataLen, dataPtr, 0, true, CommandConfCallback);
            #else
                return SendPacket(false, Dest_Addr, true, , dataLen, dataPtr, 0, true, CommandConfCallback);
            #endif           
        }
    #endif
    

#endif



/*********************************************************************
 * BOOL    isSameAddress(uint8_t *Address1, uint8_t *Address2)
 *
 * Overview:        This function compares two long addresses and returns
 *                  the boolean to indicate if they are the same
 *
 * PreCondition:    
 *
 * Input:  
 *          Address1    - Pointer to the first long address to be compared
 *          Address2    - Pointer to the second long address to be compared
 *                  
 * Output: 
 *          If the two address are the same
 *
 * Side Effects:    
 *
 ********************************************************************/
bool    isSameAddress(INPUT uint8_t *Address1, INPUT uint8_t *Address2)
{
    uint8_t i;
    
    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
    {
        if( Address1[i] != Address2[i] )
        {
            return false;
        }
    }
    return true;
}

#if defined(ENABLE_HAND_SHAKE)
     
    bool MiApp_StartConnection(uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap,
	                                                  connectionConf_callback_t ConfCallback)
    {
		#if defined (PROTOCOL_STAR)
		role=true;
		#endif
        switch(Mode)
        {
            case START_CONN_DIRECT:
			{
			
                uint8_t channel = 0;
                uint32_t index = 1;
                #if defined(IEEE_802_15_4)
                    #if MY_PAN_ID == 0xFFFF
                        myPANID.v[0] = TMRL;
                        myPANID.v[1] = TMRL+0x51;
                    #else
                        myPANID.Val = MY_PAN_ID;
                    #endif
                    {
                        uint16_t tmp = 0xFFFF;
                        MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                    }
                #endif
				while (!(index & ChannelMap))
				{
				// Unset current bit and set the next bit in 'i'
				index = index << 1;

				// increment position
				++channel;
				}
				/* Set the best channel */
				MiApp_Set(CHANNEL, &channel);
                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP_FEATURE) && defined(ENABLE_INDIRECT_MESSAGE)
                    TimeSyncTick.Val = MiWi_TickGet();
                #endif
                tick1.Val = MiWi_TickGet();
                tick4.Val = MiWi_TickGet();
				ConfCallback(SUCCESS);
                return true;
			}
                
            case START_CONN_ENERGY_SCN:
                #if defined(ENABLE_ED_SCAN)
                {
                    uint8_t channel;
                    uint8_t RSSIValue;
                    
                    #if defined(IEEE_802_15_4)
                        #if MY_PAN_ID == 0xFFFF
                            myPANID.v[0] = TMRL;
                            myPANID.v[1] = TMRL+0x51;
                        #else
                            myPANID.Val = MY_PAN_ID;
                        #endif
                        {
                            uint16_t tmp = 0xFFFF;
                            MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                        }
                    #endif
                    channel = MiApp_NoiseDetection(ChannelMap, ScanDuration, NOISE_DETECT_ENERGY, &RSSIValue);
                    MiApp_Set(CHANNEL, &channel);
                    printf("\r\nStart Wireless Communication on Channel ");
                    printf("%u",channel);
                    printf("\r\n");
                    #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP_FEATURE) && defined(ENABLE_INDIRECT_MESSAGE)
                        TimeSyncTick.Val = MiWi_TickGet();
                    #endif
                    ConfCallback(SUCCESS);
                    return true;
                }
                #else
                    ConfCallback(FAILURE);
                    return false;
                #endif
                
            case START_CONN_CS_SCN:
                // Carrier sense scan is not supported for current available transceivers
                ConfCallback(FAILURE);
                return false;
            
            default:
                break;
        }    
        ConfCallback(FAILURE);
        return false;
    }
    
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_EstablishConnection(uint8_t Channel, uint8_t addr_len, uint8_t *addr, uint8_t Capability_info, 
     *                                                                    connectionConf_callback_t ConfCallback)
     *
     * Summary:
     *      This function establish a connection with one or more nodes in an existing
     *      PAN.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      start communication with an existing PAN. For P2P protocol, this function
     *      call can establish one or more connections. For network protocol, this 
     *      function can be used to join the network, or establish a virtual socket
     *      connection with a node out of the radio range. There are multiple ways to
     *      establish connection(s), all depends on the input parameters.
     *
     * PreCondition:    
     *      Protocol initialization has been done. If only to establish connection with
     *      a predefined device, an active scan must be performed before and valid active
     *      scan result has been saved.
     *
     * Parameters:           
     *      uint8_t channel -  The selected channel to invoke join procedure.
     *      uint8_t addr_len - Address length
     *      uint8_t *addr  - address of the parent
     *      uint8_t Capability_info - capability information of the device 
     *      connectionConf_callback_t ConfCallback - The callback routine which will be called upon
     *                                               the initiated connection procedure is performed
     *                  
     * Returns: 
     *      The index of the peer device on the connection table.
     *
     * Example:
     *      <code>
     *      // Establish one or more connections with any device
     *      PeerIndex = MiApp_EstablishConnection(14, 8, 0x12345678901234567,0x80, callback);
     *      </code>
     *
     * Remarks:    
     *      If more than one connections have been established through this function call, the
     *      return value points to the index of one of the peer devices.
     *
     *****************************************************************************************/                
     uint8_t MiApp_EstablishConnection(uint8_t Channel, uint8_t addr_len, uint8_t *addr, uint8_t Capability_info,
                                                                         connectionConf_callback_t ConfCallback)
    {
        uint8_t    tmpConnectionMode = ConnMode;
        uint8_t    retry = CONNECTION_RETRY_TIMES;
        uint8_t    connectionInterval = 0;
        MIWI_TICK    t1, t2;
        tick1.Val = MiWi_TickGet();
        t1.Val = MiWi_TickGet();
        t1.Val -= (ONE_SECOND);
        ConnMode = ENABLE_ALL_CONN;
        P2PStatus.bits.SearchConnection = 1;
        EstConfCallback = ConfCallback;
        while( P2PStatus.bits.SearchConnection )
        {
            t2.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(t2, t1) > (ONE_SECOND) )
            {   
                t1.Val = t2.Val;

                if( connectionInterval-- > 0 )
                {
                    continue;
                }
                connectionInterval = CONNECTION_INTERVAL-1;
                if( retry-- == 0 )
                {

                    P2PStatus.bits.SearchConnection = 0;
                    return 0xFF;
                }
                MiApp_Set(CHANNEL, &Channel);
                uint8_t* dataPtr = NULL;
                uint8_t dataLen = 0;
                dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_CONNECTION_REQUEST));
                if (NULL == dataPtr)
                   return 0;
                dataPtr[dataLen++] = CMD_P2P_CONNECTION_REQUEST;
                dataPtr[dataLen++] = currentChannel;
                dataPtr[dataLen++] = P2PCapacityInfo;
                #if defined(PROTOCOL_STAR) && defined(MAKE_ENDDEVICE_PERMANENT)
                {
                    dataPtr[dataLen++] = 0xAA;
                }
                #endif
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    {
						uint8_t i;
                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                        {
                            dataPtr[dataLen++] = AdditionalNodeID[i];
                        }
                    }
                #endif

                #if defined(IEEE_802_15_4)
                    #if defined(ENABLE_ACTIVE_SCAN)
                        uint16_t DestinationAddress16 = ((addr[1] << 8) + addr[0]);	
                        if( DestinationAddress16 == 0xFFFF )
                        {
                            SendPacket(true, myPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                        }
                        else
                        {
							uint8_t i,j;
							bool deviceFound = false;
							MiApp_Set(CHANNEL, &Channel);
							for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
							{
								deviceFound = true;
								for(j = 0; j < MY_ADDRESS_LENGTH; j++)
								{
								  if (addr[j] != ActiveScanResults[i].Address[j])
								  {
								      deviceFound = false;
								      break;
								  }
								}
								if (deviceFound)
								{
									break;
								}
							}
                            if (deviceFound)
							{
								SendPacket(false, ActiveScanResults[i].PANID, ActiveScanResults[i].Address, true, false, 
							    dataLen, dataPtr,0, true, CommandConfCallback);
							}
                        }
                    #else
                        SendPacket(true, myPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                    #endif
                #else
                    #if defined(ENABLE_ACTIVE_SCAN)
                        if( addr == NULL )
                        {
                            SendPacket(true, myPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                        }
                        else
                        {
							uint8_t i,j;
							for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
							{
								for(j = 0; j < MY_ADDRESS_LENGTH; j++)
								{
									if (addr[j] != ActiveScanResults[i].Address[j])
									break;
								}
							}
							MiApp_Set(CHANNEL, &ActiveScanResults[i].Channel);
                            SendPacket(false, ActiveScanResults[i].Address, true, false, 
							dataLen, dataPtr,0,true, CommandConfCallback);
                        }
                    #else
                        SendPacket(true, NULL, true, false, dataLen, dataPtr, 0, true, CommandConfCallback);
                    #endif
                #endif
            }
     
            P2PTasks();
        }
      
        ConnMode = tmpConnectionMode;
        
        #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP_FEATURE) && defined(ENABLE_INDIRECT_MESSAGE)
            TimeSyncTick.Val = MiWi_TickGet();
        #endif
        return LatestConnection;
        
    }


#endif

bool MiApp_Set(miwi_params_t id, uint8_t *value)
{
    switch(id)
   {
      case CHANNEL:
     {
        if( MiMAC_Set(MAC_CHANNEL, value))
        {
          currentChannel = *value;
          #if defined(ENABLE_NETWORK_FREEZER)
			  PDS_Store(PDS_CURRENT_CHANNEL_ID);
          #endif
          return true;
        }
     }
     break;

     default:
     break;
   }
    return false;
}

bool MiApp_Get(miwi_params_t id, uint8_t *value)
{
	switch(id)
	{
		case CHANNEL:
		{
			*value = currentChannel;
			return true;
		}
		break;
		
		case PANID:
		{
			value[0] = myPANID.Val;
			value[1] = myPANID.Val >> 8;
			return true;
		}
		break;
		default:
		break;
	}
	return false;
}
#ifdef ENABLE_DUMP
    /*********************************************************************
     * void DumpConnection(uint8_t index)
     *
     * Overview:        This function prints out the content of the connection 
     *                  with the input index of the P2P Connection Entry
     *
     * PreCondition:    
     *
     * Input:  
     *          index   - The index of the P2P Connection Entry to be printed out
     *                  
     * Output:  None
     *
     * Side Effects:    The content of the connection pointed by the index 
     *                  of the P2P Connection Entry will be printed out
     *
     ********************************************************************/
    void DumpConnection(INPUT uint8_t index)
    {
        uint8_t i, j;
        
        if( index > CONNECTION_SIZE )
        {
            printf("\r\n\r\nMy Address: 0x");
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
				printf("%02x",myLongAddress[MY_ADDRESS_LENGTH-1-i]);
            }
            #if defined(IEEE_802_15_4)
                printf("  PANID: 0x");
                printf("%x",myPANID.v[1]);
                printf("%x",myPANID.v[0]);
            #endif
            printf("  Channel: ");
            printf("%d",currentChannel);
        }
            
        if( index < CONNECTION_SIZE )
        {
            printf("\r\nConnection \tPeerLongAddress \tPeerInfo\r\n");  
            if( ConnectionTable[index].status.bits.isValid )
            {
                printf("%02x",index);
                printf("\t\t\t");
                for(i = 0; i < 8; i++)
                {
                    if(i < MY_ADDRESS_LENGTH)
                    {
                        printf("%02x", ConnectionTable[index].Address[MY_ADDRESS_LENGTH-1-i] );
                    }
                    else
                    {
                        printf("\t");
                    }
                }
                printf("/t");
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                    {
                        printf("%02x", ConnectionTable[index].PeerInfo[i] );
                    }
                #endif
                printf("\r\n");
            }
        }
        else
        {
            printf("\r\n\r\nConnection     PeerLongAddress     PeerInfo\r\n");  
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                
                if( ConnectionTable[i].status.bits.isValid )
                {
                    printf("%02x",i);
                    printf("             ");
                    for(j = 0; j < 8; j++)
                    {
                        if( j < MY_ADDRESS_LENGTH )
                        {
                            printf("%02x", ConnectionTable[i].Address[MY_ADDRESS_LENGTH-1-j] );
                        }
                        else
                        {
                            printf("  ");
                        }
                    }
                    printf("    ");
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            printf("%02x", ConnectionTable[i].PeerInfo[j] );
                        }
                    #endif
                    printf("\r\n");
                }  
            }
        }
    }
#endif


#if defined(ENABLE_HAND_SHAKE)
    /*********************************************************************
     * uint8_t AddConnection(void)
     *
     * Overview:        This function create a new P2P connection entry
     *
     * PreCondition:    A P2P Connection Request or Response has been 
     *                  received and stored in rxMessage structure
     *
     * Input:  None
     *                  
     * Output: 
     *          The index of the P2P Connection Entry for the newly added
     *          connection
     *
     * Side Effects:    A new P2P Connection Entry is created. The search 
     *                  connection operation ends if an entry is added 
     *                  successfully
     *
     ********************************************************************/
    uint8_t AddConnection(void)
    {
        uint8_t i;
        uint8_t status = STATUS_SUCCESS;
        uint8_t connectionSlot = 0xFF;
    
        // if no peerinfo attached, this is only an active scan request,
        // so do not save the source device's info
        #ifdef ENABLE_ACTIVE_SCAN
            if( rxMessage.PayloadSize < 3 )
            {
                return STATUS_ACTIVE_SCAN;
            }
        #endif
        
        // loop through all entry and locate an proper slot
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            // check if the entry is valid
            if( ConnectionTable[i].status.bits.isValid )
            {
                // check if the entry address matches source address of current received packet
                if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                {
                    connectionSlot = i;
                    status = STATUS_EXISTS;
                    break;
                }
            }
            else if( connectionSlot == 0xFF )
            {
                // store the first empty slot
                connectionSlot = i;
            }  
        }
            
        if( connectionSlot == 0xFF )
        {
            return STATUS_NOT_ENOUGH_SPACE;
        }
        else 
        {
            if( ConnMode >= ENABLE_PREV_CONN )
            {
                return status;
            }
            MyindexinPC = connectionSlot;    
            // store the source address
            for(i = 0; i < 8; i++)
            {
                ConnectionTable[connectionSlot].Address[i] = rxMessage.SourceAddress[i];
            }
            
            // store the capacity info and validate the entry
            ConnectionTable[connectionSlot].status.bits.isValid = 1;
            ConnectionTable[connectionSlot].status.bits.RXOnWhenIdle = (rxMessage.Payload[2] & 0x01);
            
            // store possible additional connection payload
            #if ADDITIONAL_NODE_ID_SIZE > 0
                for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                {
                    ConnectionTable[connectionSlot].PeerInfo[i] = rxMessage.Payload[3+i];
                }
            #endif
    
            #ifdef ENABLE_SECURITY
                // if security is enabled, clear the incoming frame control
                IncomingFrameCounter[connectionSlot].Val = 0;
            #endif
            LatestConnection = connectionSlot;
            P2PStatus.bits.SearchConnection = 0;   
        }
        conn_size = Total_Connections();
    #if defined (ENABLE_NETWORK_FREEZER)
		PDS_Store(PDS_EDC_ID);
    #endif

        return status;
    }
#endif


#ifdef ENABLE_ACTIVE_SCAN
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_SearchConnection(uint8_t ScanDuartion, uint32_t ChannelMap,
     *                                             SearchConnectionConf_callback_t ConfCallback)
     *
     * Summary:
     *      This function perform an active scan to locate operating PANs in the
     *      neighborhood.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      perform an active scan. After this function call, all active scan response
     *      will be stored in the global variable ActiveScanResults in the format of 
     *      structure ACTIVE_SCAN_RESULT. The return value indicates the total number
     *      of valid active scan response in the active scan result array.
     *
     * PreCondition:    
     *      Protocol initialization has been done.
     *
     * Parameters:           
     *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
     *                          value is from 5 to 14. The real time to perform scan can
     *                          be calculated in following formula from IEEE 802.15.4 
     *                          specification 
     *                              960 * (2^ScanDuration + 1) * 10^(-6) second
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double word parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
	 *      SearchConnectionConf_callback_t ConfCallback - The callback routine which will be called upon
	 *                                               the initiated connection procedure is performed	 
     *                  
     * Returns: 
     *      The number of valid active scan response stored in the global variable ActiveScanResults.
     *
     * Example:
     *      <code>
     *      // Perform an active scan on all possible channels
     *      NumOfActiveScanResponse = MiApp_SearchConnection(10, 0xFFFFFFFF, callback);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
	uint8_t MiApp_SearchConnection(INPUT uint8_t ScanDuration, INPUT uint32_t ChannelMap, 
	                                                    SearchConnectionConf_callback_t ConfCallback)
    {
        uint8_t i;
        uint32_t channelMask = 0x00000001;
        uint8_t backupChannel = currentChannel;
        MIWI_TICK t1, t2;
        
        for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
        {
            ActiveScanResults[i].Channel = 0xFF;
        }
        
        ActiveScanResultIndex = 0;
        i = 0;
        while( i < 32 )
        {
            if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
            {
                uint8_t* dataPtr = NULL;
                uint8_t dataLen = 0;
                #if defined(IEEE_802_15_4)
                    API_UINT16_UNION tmpPANID;
                #endif

                printf("\r\nScan Channel ");
                printf("%d",i);
                /* choose appropriate channel */
                MiApp_Set(CHANNEL, &i);
                dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_ACTIVE_SCAN_REQUEST));
                if (NULL == dataPtr)
                   return 0;
                dataPtr[dataLen++] = CMD_P2P_ACTIVE_SCAN_REQUEST;
                dataPtr[dataLen++] = currentChannel;
                #if defined(IEEE_802_15_4)
                    tmpPANID.Val = 0xFFFF;
                    SendPacket(true, tmpPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                #else
                    SendPacket(true, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                #endif
                
                t1.Val = MiWi_TickGet();
                while(1)
                {                  
                    P2PTasks();
                    t2.Val = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t2, t1) > ((uint32_t)(miwi_scan_duration_ticks(ScanDuration))) )
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
                }          
            }  
            i++;
        }
        
        MiApp_Set(CHANNEL, &backupChannel);
        ConfCallback(ActiveScanResultIndex, (uint8_t*)ActiveScanResults);
        return ActiveScanResultIndex;
    }   

#endif

#ifdef ENABLE_ED_SCAN

    /************************************************************************************
     * Function:
     *      uint8_t MiApp_NoiseDetection(  uint32_t ChannelMap, uint8_t ScanDuration,
     *                                  uint8_t DetectionMode, uint8_t *NoiseLevel)
     *
     * Summary:
     *      This function perform a noise scan and returns the channel with least noise
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      perform noise detection on multiple channels.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double uint16_t parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
     *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
     *                          value is from 5 to 14. The real time to perform scan can
     *                          be calculated in following formula from IEEE 802.15.4 
     *                          specification 
     *                              960 * (2^ScanDuration + 1) * 10^(-6) second
     *      uint8_t DetectionMode -    The noise detection mode to perform the scan. The two possible
     *                              scan modes are
     *                              * NOISE_DETECT_ENERGY   Energy detection scan mode
     *                              * NOISE_DETECT_CS       Carrier sense detection scan mode
     *      uint8_t *NoiseLevel -  The noise level at the channel with least noise level
     *
     * Returns: 
     *      The channel that has the lowest noise level
     *
     * Example:
     *      <code>
     *      uint8_t NoiseLevel;
     *      OptimalChannel = MiApp_NoiseDetection(0xFFFFFFFF, 10, NOISE_DETECT_ENERGY, &NoiseLevel);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    uint8_t MiApp_NoiseDetection(INPUT uint32_t ChannelMap, INPUT uint8_t ScanDuration, INPUT uint8_t DetectionMode, OUTPUT uint8_t *RSSIValue)
    {
        uint8_t i;
        uint8_t OptimalChannel;
        uint8_t minRSSI = 0xFF;
        uint32_t channelMask = 0x00000001;
        MIWI_TICK t1, t2;
        
        if( DetectionMode != NOISE_DETECT_ENERGY )
        {
            return 0xFF;
        }
        printf("\r\n For Channel map: %0x\r\n", ChannelMap );
        printf("\r\nEnergy Scan Results:");
        i = 0;
        while( i < 32 )
        {
            if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
            {
                uint8_t RSSIcheck;
                uint8_t maxRSSI = 0;
                uint8_t j, k;

                /* choose appropriate channel */
                MiApp_Set(CHANNEL, &i);
                
                t1.Val = MiWi_TickGet();
                
                while(1)
                {
                    RSSIcheck = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_ENERGY_DETECT); 
                    if( RSSIcheck > maxRSSI )
                    {
                        maxRSSI = RSSIcheck;
                    }

                    t2.Val = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t2, t1) > ((uint32_t)(miwi_scan_duration_ticks(ScanDuration)) ))
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
					
                } 
                
                printf("\r\nChannel %u",i);
                printf(": ");
                j = maxRSSI/5;
                for(k = 0; k < j; k++)
                {
                    printf("-");
                }
                printf(" ");
                printf("%u",maxRSSI);
                
                if( maxRSSI < minRSSI )
                {
                    minRSSI = maxRSSI;
                    OptimalChannel = i;
                    if( RSSIValue )
                    {
                        *RSSIValue = minRSSI;
                    }   
                }              
            }  
            i++;
        }        
       
        return OptimalChannel;
    }
   
#endif


#ifdef ENABLE_FREQUENCY_AGILITY

    /*********************************************************************
     * void StartChannelHopping(uint8_t OptimalChannel)
     *
     * Overview:        This function broadcast the channel hopping command
     *                  and after that, change operating channel to the 
     *                  input optimal channel     
     *
     * PreCondition:    Transceiver has been initialized
     *
     * Input:           OptimalChannel  - The channel to hop to
     *                  
     * Output: 
     *          None
     *
     * Side Effects:    The operating channel for current device will change
     *                  to the specified channel
     *
     ********************************************************************/
    void StartChannelHopping(INPUT uint8_t OptimalChannel)
    {
        uint8_t i;
        MIWI_TICK t1, t2;
        
        for( i = 0; i < FA_BROADCAST_TIME; i++)
        {
            t1.Val = MiWi_TickGet();
            while(1)
            {
                t2.Val = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > miwi_scan_duration_ticks(9) )
                {
                    uint8_t* dataPtr = NULL;
                    uint8_t dataLen = 0;
                    dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_CMD_CHANNEL_HOPPING));
                    if (NULL == dataPtr)
                        return;
                    dataPtr[dataLen++] = CMD_CHANNEL_HOPPING;
                    dataPtr[dataLen++] = currentChannel;
                    dataPtr[dataLen++] = OptimalChannel;
                    #if defined(IEEE_802_15_4)
                        SendPacket(true, myPANID, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                    #else
                        SendPacket(true, NULL, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                    #endif
                    break;
                }
            }
        }
        delay_ms(500);
        MiApp_Set(CHANNEL, &OptimalChannel);
    }
    
    
    /********************************************************************************************
     * Function:
     *      BOOL MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap)
     *
     * Summary:
     *      This function tries to resynchronize the lost connection with 
     *      peers, probably due to channel hopping
     *
     * Description:        
     *      This is the primary user interface function for the application to resynchronize a 
     *      lost connection. For a RFD device that goes to sleep periodically, it may not 
     *      receive the channel hopping command that is sent when it is sleep. The sleeping 
     *      RFD device depends on this function to hop to the channel that the rest of
     *      the PAN has jumped to. This function call is usually triggered by continously 
     *      communication failure with the peers.
     *
     * PreCondition:    
     *      Transceiver has been initialized
     *
     * Parameters:      
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double uint16_t parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
     *                  
     * Returns: 
     *                  a boolean to indicate if resynchronization of connection is successful
     *
     * Example:
     *      <code>
     *      // Sleeping RFD device resync with its associated device, usually the first peer
     *      // in the connection table
     *      MiApp_ResyncConnection(0, 0xFFFFFFFF);
     *      </code>
     *
     * Remark:          
     *      If operation is successful, the wireless node will be hopped to the channel that 
     *      the rest of the PAN is operating on.
     *
     *********************************************************************************************/ 
    bool MiApp_ResyncConnection(INPUT uint8_t ConnectionIndex, INPUT uint32_t ChannelMap)
    {
        uint8_t i;
        uint8_t j;
        uint8_t backupChannel = currentChannel;
        MIWI_TICK t1, t2;
        
        t1.Val = MiWi_TickGet();
        P2PStatus.bits.Resync = 1;
        for(i = 0; i < RESYNC_TIMES; i++)
        {
            uint32_t ChannelMask = 0x00000001;
            
            j = 0;
            while(P2PStatus.bits.Resync)
            {
                t2.Val = MiWi_TickGet();
                
                if( MiWi_TickGetDiff(t2, t1) > miwi_scan_duration_ticks(9) )
                {
                    uint8_t* dataPtr = NULL;
                    uint8_t dataLen = 0;
                    t1.Val = t2.Val;
                    
                    if( j > 31 )
                    {
                        break;
                    }
                    while( (ChannelMap & FULL_CHANNEL_MAP & (ChannelMask << j)) == 0 )
                    {
                        if( ++j > 31 )
                        {
                            goto GetOutOfLoop;
                        }
                    }
                    
                    printf("\r\nChecking Channel ");
                    printf("%d",j);
                    MiApp_Set(CHANNEL, &j);
                    j++;
                    
                    dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_ACTIVE_SCAN_REQUEST));
                    if (NULL == dataPtr)
                        return false;
                    dataPtr[dataLen++] = CMD_P2P_ACTIVE_SCAN_REQUEST;
                    dataPtr[dataLen++] = currentChannel;
        
                    #if defined(IEEE_802_15_4)
                        SendPacket(false, myPANID, ConnectionTable[ConnectionIndex].Address, true, false, 
						dataLen, dataPtr,0,true, CommandConfCallback);
                    #else
                        SendPacket(false, ConnectionTable[ConnectionIndex].Address, true, false, 
						dataLen, dataPtr,0, true, CommandConfCallback);
                    #endif
                }        
                P2PTasks();
            }
            if( P2PStatus.bits.Resync == 0 )
            {
                printf("\r\nResynchronized Connection to Channel ");
                printf("%d",currentChannel);
                printf("\r\n");
                return true;
            }
GetOutOfLoop:
            nop();
        }
        
        MiApp_Set(CHANNEL, &backupChannel);
        P2PStatus.bits.Resync = 0;
        return false;
    }        

    #ifdef FREQUENCY_AGILITY_STARTER
        /*******************************************************************************************
         * Function:
         *      BOOL MiApp_InitChannelHopping(uint32_t ChannelMap)
         *
         * Summary:
         *      
         *      This function tries to start a channel hopping (frequency agility) procedure
         *
         * Description:        
         *      This is the primary user interface function for the application to do energy 
         *      scan to locate the channel with least noise. If the channel is not current 
         *      operating channel, process of channel hopping will be started.
         *
         * PreCondition:    
         *      Transceiver has been initialized
         *
         * Parameters:      
         *      uint32_t ChannelMap -  The bit map of the candicate channels
         *                          which can be hopped to
         *                  
         * Returns: 
         *                  a boolean to indicate if channel hopping is initiated
         *
         * Example:
         *      <code>
         *      // if condition meets, scan all possible channels and hop 
         *      // to the one with least noise
         *      MiApp_InitChannelHopping(0xFFFFFFFF);
         *      </code>
         *
         * Remark:          The operating channel will change to the optimal 
         *                  channel with least noise
         *
         ******************************************************************************************/
        bool MiApp_InitChannelHopping( INPUT uint32_t ChannelMap)
        {
            uint8_t RSSIValue;
            uint8_t backupChannel = currentChannel;
            uint8_t backupConnMode = ConnMode;
            uint8_t optimalChannel;
            
            MiApp_ConnectionMode(DISABLE_ALL_CONN);
            optimalChannel = MiApp_NoiseDetection(ChannelMap, 0, NOISE_DETECT_ENERGY, &RSSIValue);
            MiApp_ConnectionMode(backupConnMode);
            
            MiApp_Set(CHANNEL, &backupChannel);
            if( optimalChannel == backupChannel )
            {
                return false;
            }
            
            printf("\r\nHopping to Channel ");
            printf("%d",optimalChannel);
            printf("\r\n");
            StartChannelHopping(optimalChannel);
            return true;
        }
    #endif

#endif



#if !defined(TARGET_SMALL)
    /*********************************************************************
     * Function:
     *      void MiApp_RemoveConnection(uint8_t ConnectionIndex)
     *
     * Summary:
     *      This function remove connection(s) in connection table
     *
     * Description:        
     *      This is the primary user interface function to disconnect connection(s).
     *      For a P2P protocol, it simply remove the connection. For a network protocol,
     *      if the device referred by the input parameter is the parent of the device
     *      calling this function, the calling device will get out of network along with
     *      its children. If the device referred by the input parameter is children of
     *      the device calling this function, the target device will get out of network.
     * 
     * PreCondition:    
     *      Transceiver has been initialized. Node has establish
     *      one or more connections
     *
     * Parameters:           
     *      uint8_t ConnectionIndex -  The index of the connection in the
     *                              connection table to be removed
     *                  
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      MiApp_RemoveConnection(0x00);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     ********************************************************************/
    void MiApp_RemoveConnection(INPUT uint8_t ConnectionIndex)
    {   
        if( ConnectionIndex == 0xFF )
        {
            uint8_t i;
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                uint16_t j;
                if( ConnectionTable[i].status.bits.isValid )
                {
                    uint8_t* dataPtr = NULL;
                    uint8_t dataLen = 0;
                    dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_CONNECTION_REMOVAL_REQUEST));
                    if (NULL == dataPtr)
                        return;
                    dataPtr[dataLen++] = CMD_P2P_CONNECTION_REMOVAL_REQUEST;
                    #if defined(IEEE_802_15_4)
                        SendPacket(false, myPANID, ConnectionTable[i].Address, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                    #else
                        SendPacket(false, ConnectionTable[i].Address, true, false, dataLen, dataPtr,0, true, CommandConfCallback);
                    #endif
                    for(j = 0; j < 0xFFF; j++) {}   // delay
                }
                ConnectionTable[i].status.Val = 0;
                #if defined(ENABLE_NETWORK_FREEZER)
					PDS_Store(PDS_CONNECTION_TABLE_ID);
                #endif
            } 
        }
        else if( ConnectionTable[ConnectionIndex].status.bits.isValid )
        {
            uint16_t j;
            uint8_t* dataPtr = NULL;
            uint8_t dataLen = 0;
            dataPtr = MiMem_Alloc(CALC_SEC_PAYLOAD_SIZE(PACKETLEN_P2P_CONNECTION_REMOVAL_REQUEST));
            if (NULL == dataPtr)
                return;			
            dataPtr[dataLen++] = CMD_P2P_CONNECTION_REMOVAL_REQUEST;
            #if defined(IEEE_802_15_4)
                SendPacket(false, myPANID, ConnectionTable[ConnectionIndex].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback);
            #else
                SendPacket(false, ConnectionTable[ConnectionIndex].Address, true, false, 
				dataLen, dataPtr,0, true, CommandConfCallback);
            #endif
            for(j = 0; j < 0xFFF; j++) {}   // delay
            ConnectionTable[ConnectionIndex].status.Val = 0; 
            #if defined(ENABLE_NETWORK_FREEZER)
				PDS_Store(PDS_CONNECTION_TABLE_ID);
            #endif
        }
    }
#endif

/************************************************************************************
 * Function:
 *      void    MiApp_ConnectionMode(uint8_t Mode)
 *
 * Summary:
 *      This function set the current connection mode.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      configure the way that the host device accept connection request.
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters:           
 *      uint8_t Mode -     The mode to accept connection request. The privilege for those modes
 *                      decreases gradually as defined. The higher privilege mode has all the 
 *                      rights of the lower privilege modes.
 *                      The possible modes are
 *                      * ENABLE_ALL_CONN       Enable response to all connection request
 *                      * ENABLE_PREV_CONN      Enable response to connection request
 *                                              from device already in the connection
 *                                              table.
 *                      * ENABLE_ACTIVE_SCAN_RSP    Enable response to active scan only
 *                      * DISABLE_ALL_CONN      Disable response to connection request, including
 *                                              an acitve scan request.
 *
 * Returns: 
 *      None
 *
 * Example:
 *      <code>
 *      // Enable all connection request
 *      MiApp_ConnectionMode(ENABLE_ALL_CONN);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
void MiApp_ConnectionMode(INPUT uint8_t Mode)
{
    if( Mode > 3 )
    {
        return;
    }
    ConnMode = Mode;
    P2PCapacityInfo = (P2PCapacityInfo & 0x0F) | (ConnMode << 4);
    
    #if defined(ENABLE_NETWORK_FREEZER)
		PDS_Store(PDS_CONNECTION_MODE_ID);
    #endif
}

/************************************************************************************
 * Function:
 *      bool  MiApp_SubscribeDataIndicationCallback(PacketIndCallback_t callback)
 *
 * Summary:
 *      This function return a boolean if subscribtion for rx message is successful
 *
 * Description:        
 *      This is the primary user interface functions for the application layer to 
 *      call the Microchip proprietary protocol stack to register for message indication
 *      callback to the application. The function will call the protocol stack state machine
 *      to keep the stack running. 
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters: 
 *      None
 *
 * Returns: 
 *      A boolean to indicates if the subscription operation is successful or not.
 *
 * Example:
 *      <code>
 *      if( true == MiApp_SubscribeDataIndicationCallback(ind) )
 *      {
 *      }
 *      </code\
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/      
bool  MiApp_SubscribeDataIndicationCallback(PacketIndCallback_t callback)
{
    if (NULL != callback)
    {
        pktRxcallback = callback;
        return true;
    }
    return false;
}

#if defined(ENABLE_NETWORK_FREEZER)
/************************************************************************************
* Function:
* bool MiApp_ResetToFactoryNew(void)
*
* Summary:
*      This function makes the device to factory new device
*
* Description:
*      This is used to erase all the persistent items in the non-volatile memory and resets the system.
*
* Returns:
*      A boolean to indicate the operation is success or not
*
*****************************************************************************************/
bool MiApp_ResetToFactoryNew(void)
{
    if (PDS_DeleteAll(true))
	{
		NVIC_SystemReset();
		return true;
	}
	else
	{
		return false;
	}
}
#endif