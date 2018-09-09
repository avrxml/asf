/**
* \file  miwi_api.h
*
* \brief Application Program Interface for MiWi Protocols.
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

#ifndef __MIWI_API_H_
#define __MIWI_API_H_

#include "compiler.h"
#include "miwi_config.h"          //MiWi Application layer configuration file
#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#endif

#define INPUT
#define OUTPUT
#define IOPUT


#define SHORT_ADDR_LEN  2
#define LONG_ADDR_LEN   8

#define COORDINATOR_ADDRESS_MASK    0xFF00
#define ENDEVICE_ADDRESS_MASK       0x00FF
#define RXON_ENDEVICE_ADDRESS_MASK  0x0080

#define MESH_BROADCAST_TO_ALL            (0xFFFF)
#define MESH_BROADCAST_TO_FFDS           (0xFFFE)
#define MESH_BROADCAST_TO_COORDINATORS   (0xFFFD)

typedef union
{
	uint8_t  v[4];
	uint16_t w[2];
	uint32_t Val;
}API_UINT32_UNION;

typedef union
{
	uint8_t  v[8];
	uint16_t w[4];
	uint32_t d[2];
	uint64_t Val;
}API_UINT64_UNION;

typedef union
{
	uint8_t  v[2];
	uint16_t Val;
}API_UINT16_UNION;

typedef enum miwi_status {
	SUCCESS = 0,
	FAILURE,
	CHANNEL_ACCESS_FAILURE,
	NO_ACK,
	TIMER_EXPIRED,
	TRANSACTION_EXPIRED = 5,
	ALREADY_EXISTS,
	NO_ROUTE,
	SCAN_NO_BEACON,
	SCAN_MAX_REACHED,
	MEMORY_UNAVAILABLE = 10,
	ERR_TX_FAIL,
	ERR_TRX_FAIL,
	ERR_INVALID_INPUT,
}miwi_status_t;

enum miwi_params {
	CHANNEL              =  0x00,
	PANID                =  0x01,
	SHORT_ADDRESS        =  0x02,
	PARENT_SHORT_ADDRESS =  0x03,
	DEVICE_TIMEOUT       =  0x04,
	CHANNELMAP           =  0x05,
	CAPABILITYINFO       =  0x06,
	BLOOM_AUTO_JOIN      =  0x80,
	ROUTE_TEST_MODE      =  0x81
};

typedef enum miwi_params miwi_params_t;


#if defined(PROTOCOL_MESH)
#include "miwi_mesh_app.h"      //MiWi Protocol layer configuration file
#endif

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
/***************************************************************************/
// Status information of the connected peer information
//
//      This structure contains the information regarding the status of
//      the connected peer device.
/***************************************************************************/
typedef union __CONNECTION_STATUS
{
	uint8_t Val;
	struct _CONNECTION_STAUTS_bits
	{
		//1 = transceiver always on, 0 = transceiver sleeps when idle
		uint8_t RXOnWhenIdle       :1;
		//1 = can talk to this device directly, 0 = must route to this device
		uint8_t directConnection   :1;
		//1 = long address valid, 0 = long address unknown
		uint8_t longAddressValid   :1;
		//1 = short address valid, 0 = short address unknown
		uint8_t shortAddressValid  :1;
		//1 = already finish joining procedure, 0 = in the process of join
		uint8_t FinishJoin         :1;
		//1 = family member (parent/child), 0 = not family
		uint8_t isFamily           :1;
		uint8_t filler             :1;
		//1 = this entry is valid, 0 = this entry is not valid
		uint8_t isValid            :1;
	} bits;
} CONNECTION_STATUS;


/***************************************************************************
* Peer Device Information in Connection Table
*
*      This structure contains device information about the peer device
*      of current node. It is the element structure for connection table.
*      Due to the bank limitation in PIC18 MCU architecture, the size of
*      CONNECTION_ENTRY must be dividable by 256 in case the array is across
*      the bank. In this case, the user need to make sure that there is no
*      problem
**************************************************************************/
typedef struct __CONNECTION_ENTRY
{
#if !defined(PROTOCOL_P2P)
	API_UINT16_UNION    PANID;                      // PAN Identifier of the peer device. May not necessary in P2P protocol
	API_UINT16_UNION    AltAddress;                 // Alternative address of the peer device. Not necessary in P2P protocol
#endif
	uint8_t        Address[MY_ADDRESS_LENGTH];     // Permanent address of peer device

	CONNECTION_STATUS status;

#if ADDITIONAL_NODE_ID_SIZE > 0
	uint8_t        PeerInfo[ADDITIONAL_NODE_ID_SIZE];  // Additional Node ID information, if defined in application layer
#endif
} CONNECTION_ENTRY;
#endif

#if defined(PROTOCOL_MESH)
#define BLOOM_FILTER_SIZE    8

typedef struct
{
	/**  coordinator hop */
	uint8_t coordinatorHop;

	/**  connection permit */
	uint8_t connectionPermit;

	/**  End device Capacity percentage */
	uint8_t enddeviceCapacity;

	/**  Sleeping End device Capacity percentage */
	uint8_t sleepEnddeviceCapacity;

	/** The current logical channel used by the network */
	uint8_t  logicalChannel;

	/** LQI at which the beacon was received */
	uint8_t LinkQuality;

	/**  PANID  */
	uint16_t panId;

	/**  Short Address  */
	uint16_t shortAddress;

	/**  bloom filter value */
	uint8_t bloomFilterValue[BLOOM_FILTER_SIZE];

} beaconDescriptor_t;

typedef struct
{
	/*  status of the search request */
	uint8_t status;

	/*  the number of results found    */
	uint8_t resultSize;

	/* unscanned channels during search */
	uint32_t unscannedChannels;

	beaconDescriptor_t beaconList[MAX_BEACON_RESULTS];
} searchConf_t;
#endif

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
extern CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE];
#endif
extern uint8_t            currentChannel;

#ifdef MESH_SECURITY
extern API_UINT32_UNION meshOutgoingFrameCounter;
#endif

// Source address when sending a packet // can be MAC_Address or EUI based on Users choice
extern uint8_t myLongAddress[MY_ADDRESS_LENGTH];

// the no of connections on a device.
// In case of Star network conn_size will be shared by pan co to all the end devices.
// In case of p2p network every device will have all its on conn_size.
extern uint8_t conn_size ;

#define SECURITY_KEY_SIZE         16

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
#ifdef ENABLE_SECURITY
   /* As security fields would add additional bytes in multiples of 16bytes,
    memory for the same has to be allocated. */
#define CALC_SEC_PAYLOAD_SIZE(x)	(SECURITY_KEY_SIZE > x)? \
	SECURITY_KEY_SIZE : (x % SECURITY_KEY_SIZE) ? \
	((x / SECURITY_KEY_SIZE) * SECURITY_KEY_SIZE) + SECURITY_KEY_SIZE : \
	(x / SECURITY_KEY_SIZE) * SECURITY_KEY_SIZE
#else
#define CALC_SEC_PAYLOAD_SIZE(x)	(x)
#endif
#endif

#define HOP_TABLE_COUNT   (((NUM_OF_COORDINATORS % 2) == 0)? \
							(NUM_OF_COORDINATORS / 2) : \
							((NUM_OF_COORDINATORS / 2) + 1))

/* Table maintained by the PAN Coordinator to store the list of joiner which it
 * wants to commission*/
typedef struct
{
    uint64_t deviceAddress;
} CommDeviceTable_t;

typedef struct _CoordHopCount_t
{
	uint8_t lsb : 4;
	uint8_t msb : 4;
} CoordHopCount_t;

typedef struct _CoordRouteTable_t
{
	uint8_t nextHopAddr;
	uint8_t lqi : 4;
	uint8_t score : 4;
} CoordRouteTable_t;

typedef struct _RouteEntry_t
{
	uint8_t coordNextHop;
	uint8_t coordHopCount;
	uint8_t coordNextHopLQI;
	uint8_t coordScore;
} RouteEntry_t;

/* Coordinator Table - which holds the joined router details */
typedef struct coordinatorTable_
{
	uint8_t ieeeaddr[LONG_ADDR_LEN];
	uint32_t currentTimeOut;
}CoordinatorTable_t;

/* Capability Information of a device */
typedef union CapabilityInfo
{
	uint8_t        Val;
	struct
	{
		uint8_t    rxonwhenIdle: 1;
		uint8_t    deviceType:	 2;
		uint8_t    reserved:	 5;
	} bits;
} CapabilityInfo_t;

/* Wish to Join As */
typedef union JoinWish
{
	uint8_t        Val;
	struct
	{
		uint8_t    ed:           1;
		uint8_t    coordinator:	 2;
		uint8_t    reserved:	 5;
	} bits;
} JoinWish_t;

/* Device Table - which holds the joined end device RxONdetails */
typedef struct deviceTable_
{
    uint8_t ieeeaddr[LONG_ADDR_LEN];
    uint32_t currentTimeOut;
    CapabilityInfo_t capabilityInfo;
}DeviceTable_t;

/* Device Table - which holds the joined end device details */
typedef struct SleepDeviceTable_
{
    uint8_t ieeeaddr[LONG_ADDR_LEN];
    uint32_t actualTimeOut;
    uint32_t currentTimeOut;
    CapabilityInfo_t capabilityInfo;
}SleepDeviceTable_t;

typedef struct RebroadcastTable_
{
	uint16_t nwkSrcAddr;
	uint8_t nwkSeqNo;
	uint8_t timeout;
}RebroadcastTable_t;

typedef struct __defaultParametersRomOrRam
{
#if defined(PROTOCOL_MESH)
#ifndef ENDDEVICE
#ifdef PAN_COORDINATOR
	CoordinatorTable_t *coordTable;
	CommDeviceTable_t *commDevTable;
#endif
	DeviceTable_t *devTable;
	SleepDeviceTable_t *sleepingDevTable;
	CoordRouteTable_t *coordinatorRouteTable;
	CoordHopCount_t *coordinatorHopCount;
	RebroadcastTable_t *rebroadcastTable;

	uint16_t keepAliveCoordSendInterval;
	uint16_t keepAliveCoordTimeoutSec;
	uint8_t numOfCoordinators;
	uint8_t maxNumOfDevicesInNetwork;

	uint8_t roleUpgradeIntervalInSec;
	uint8_t numOfRxOnEnddevices;
	uint8_t numOfRxOffEnddevices;

	uint8_t routeUpdateInterval;
	uint8_t routeReqWaitInterval;

	uint8_t indirectDataWaitInterval;

	uint8_t rebroadcastTableSize;
	uint8_t rebroadcastTimeout;
#endif
	uint32_t deviceTimeout;
	uint16_t keepAliveRxOnEdSendInterval;
	uint16_t keepAliveRxOnEdTimeoutSec;
	uint8_t dataRequestInterval;
	
	uint8_t edLinkFailureAttempts;
	uint8_t connRespWaitInSec;
	uint8_t frameAckWaitInterval;
	uint8_t frameRetry;
#ifndef PAN_COORDINATOR
	searchConf_t* searchConfMem;
	uint8_t maxNoOfBeacons;
#endif
#else
   uint8_t dummy;
#endif
} defaultParametersRomOrRam_t;

typedef struct __defaultParametersRamOnly
{
#ifdef MESH_SECURITY
	uint8_t miwiPublicKey[SECURITY_KEY_SIZE];
	uint8_t miwiNetworkKey[SECURITY_KEY_SIZE];
	uint8_t securityLevel;
#endif
	uint8_t dummy;
} defaultParametersRamOnly_t;


extern defaultParametersRomOrRam_t defaultParamsRomOrRam;

extern defaultParametersRamOnly_t defaultParamsRamOnly;

/************************************************************************************
* Function:
*      void MiApp_ProtocolInit(defaultParametersRomOrRam_t *defaultRomOrRamParams,
*                              defaultParametersRamOnly_t *defaultRamOnlyParams)
*
* Summary:
*      This function initialize the Microchip proprietary wireless protocol
*
* Description:
*      This is the primary user interface function to initialize the Microchip
*      proprietary wireless protocol, which is chosen by the application layer.
*      Usually, this function must be called after the hardware initialization,
*      before any other MiApp interface can be called.
*
* PreCondition:
*      Hardware initialization has been done.
*
* Parameters:
*      defaultParametersRomOrRam_t *defaultRomOrRamParams - Default parameters for MiWi Mesh.
*      defaultParametersRamOnly_t *defaultRamOnlyParams - Default parameters for MiWi Mesh.
*                                           Ignored in case of P2P / Star
*
* Returns:
*      None
*
* Example:
*      <code>
*      HardwareInit();
*      MiApp_ProtocolInit();
*      </code>
*
* Remarks:
*      None
*
*********************************************************************************/
bool MiApp_ProtocolInit(defaultParametersRomOrRam_t *defaultRomOrRamParams,
						defaultParametersRamOnly_t *defaultRamOnlyParams);

/************************************************************************************
* Function:
*      bool MiApp_Set(set_params id, uint8_t *value )
*
* Summary:
*      This function sets the information base values
*
* Description:
*      This is the primary user interface function to set the different values in the MiWi
*      stack
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      set_params id -  The identifier of the value to be set
*      value - value to be set
*
* Returns:
*      a boolean to indicate if set operation has been performed successfully
*
* Example:
*      <code>
*      if( true == MiApp_Set(CHANNEL, 15) )
*      {
*          // channel changes successfully
*      }
*      </code>
*
* Remarks:
*      None
*
*********************************************************************************/
bool    MiApp_Set(miwi_params_t id, uint8_t *value);


#define START_CONN_DIRECT       0x00
#define START_CONN_ENERGY_SCN   0x01
#define START_CONN_CS_SCN       0x02

typedef void (*connectionConf_callback_t)(miwi_status_t status);

/************************************************************************************
* Function:
*      bool    MiApp_StartConnection(uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap,
*                                                    connectionConf_callback_t ConfCallback)
*
* Summary:
*      This function start a PAN without connected to any other devices
*
* Description:
*      This is the primary user interface function for the application layer to
*      a PAN. Usually, this fucntion is called by the PAN Coordinator who is the
*      first in the PAN. The PAN Coordinator may start the PAN after a noise scan
*      if specified in the input mode.
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      uint8_t Mode - Whether to start a PAN after a noise scan. Possible modes are
*                  * START_CONN_DIRECT Start PAN directly without noise scan
*                  * START_CONN_ENERGY_SCN Perform an energy scan first, then
*                                          start the PAN on the channel with least
*                                          noise.
*                  * START_CONN_CS_SCN     Perform a carrier-sense scan first,
*                                          then start the PAN on the channel with
*                                          least noise.
*      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
*                          value is from 5 to 14. The real time to perform scan can
*                          be calculated in following formula from IEEE 802.15.4
*                          specification:
*                              960 * (2^ScanDuration + 1) * 10^(-6) second
*                          ScanDuration is discarded if the connection mode is
*                          START_CONN_DIRECT.
*      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
*                          double word parameter use one bit to represent corresponding
*                          channels from 0 to 31. For instance, 0x00000003 represent to
*                          scan channel 0 and channel 1. ChannelMap is discarded if the
*                          connection mode is START_CONN_DIRECT.
*
*      connectionConf_callback_t ConfCallback - The callback routine which will be called upon
*                                               the initiated connection procedure is performed
* Returns:
*      a boolean to indicate if PAN has been started successfully.
*
* Example:
*      <code>
*      // start the PAN on the least noisy channel after scanning all possible channels.
*      MiApp_StartConnection(START_CONN_ENERGY_SCN, 10, 0xFFFFFFFF, callback);
*      </code>
*
* Remarks:
*      None
*
*****************************************************************************************/
bool    MiApp_StartConnection( uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap,
connectionConf_callback_t ConfCallback);

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
/***************************************************************************
* Active Scan result
*
*      This structure contains information from active scan. Application
*      layer will depend on this information to decide the way to establish
*      connections.
**************************************************************************/
typedef struct
{
uint8_t		Channel;                        // Operating Channel of the PAN
uint8_t        Address[MY_ADDRESS_LENGTH];     // Responding device address
API_UINT16_UNION    PANID;                          // PAN Identifier
uint8_t       RSSIValue;                      // RSSI value for the response
uint8_t        LQIValue;                       // LQI value for the response
union
{
uint8_t        Val;
struct
{
uint8_t    Role:		2;              // Role of the responding device in the PAN
uint8_t    Sleep:		1;              // Whether the responding device goes to sleep when idle
uint8_t    SecurityEn:	1;              // Whether the responding device is capable of securing the data
uint8_t    RepeatEn:	1;              // Whether the responding device allow repeat
uint8_t    AllowJoin:	1;              // Whether the responding device allows other device to join
uint8_t    Direct:		1;              // Whether the responding device in radio range or through a repeater
uint8_t    altSrcAddr: 1;              // Whether the Address is alternative network address or permanent address
} bits;
} Capability;
#if ADDITIONAL_NODE_ID_SIZE > 0
uint8_t        PeerInfo[ADDITIONAL_NODE_ID_SIZE];  // Additional Node ID information, if defined in application layer
#endif
} ACTIVE_SCAN_RESULT;

#ifdef ENABLE_ACTIVE_SCAN
extern ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];
#endif
#endif

typedef void (*SearchConnectionConf_callback_t)(uint8_t foundScanResults, void* ScanResults);
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
uint8_t    MiApp_SearchConnection(uint8_t ScanDuration, uint32_t ChannelMap,
SearchConnectionConf_callback_t ConfCallback);

#define CONN_MODE_DIRECT        0x00
#define CONN_MODE_INDIRECT      0x01
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
uint8_t    MiApp_EstablishConnection(uint8_t Channel, uint8_t addr_len, uint8_t *addr, uint8_t Capability_info,
connectionConf_callback_t ConfCallback);

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
void    MiApp_RemoveConnection(uint8_t ConnectionIndex);

#define ENABLE_ALL_CONN         0x00
#define ENABLE_PREV_CONN        0x01
#define ENABLE_ACTIVE_SCAN_RSP  0x02
#define DISABLE_ALL_CONN        0x03
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
void    MiApp_ConnectionMode(uint8_t Mode);

typedef void (*DataConf_callback_t)(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer);

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
*      DestinationAddress. The application payload is filled using msgpointer.
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
*      bool ackReq - set to receive network level ack (Note- Discarded for broadcast data)
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
bool ackReq, DataConf_callback_t ConfCallback);

#define BROADCAST_TO_ALL            0x01
#define MULTICAST_TO_COORDINATORS   0x02
#define MULTICAST_TO_FFDS           0x03
/***************************************************************************
* Received Message information
*
*      This structure contains information about the received application
*      message.
**************************************************************************/
typedef struct
{
union
{
uint8_t Val;
struct
{
uint8_t		broadcast:  2;      // 1: broadcast message
uint8_t		ackReq:     1;      // 1: sender request acknowledgement in MAC.
uint8_t		secEn:      1;      // 1: application payload has been secured
uint8_t		repeat:     1;      // 1: message received through a repeater
uint8_t		command:    1;      // 1: message is a command frame
uint8_t        srcPrsnt:   1;      // 1: source address present in the packet
uint8_t        altSrcAddr: 1;      // 1: source address is alternative network address
} bits;
}  flags;

API_UINT16_UNION    SourcePANID;                // PAN Identifier of the sender
uint8_t		*SourceAddress;             // pointer to the source address
uint8_t		*Payload;                   // pointer to the application payload
uint8_t		PayloadSize;                // application payload length
uint8_t		PacketRSSI;                 // RSSI value of the receive message
uint8_t 	PacketLQI;                  // LQI value of the received message

} RECEIVED_MESSAGE;

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
typedef void (*PacketIndCallback_t)(RECEIVED_MESSAGE *ind);
#endif

#if defined(PROTOCOL_MESH)
typedef struct
{
uint16_t	sourceAddress;           // source address
uint8_t		*payload;                   // pointer to the application payload
uint8_t		payloadSize;                // application payload length
uint8_t		packetRSSI;                 // RSSI value of the receive message
uint8_t 	packetLQI;                  // LQI value of the received message
} RECEIVED_MESH_MESSAGE;

#endif 

#if defined(PROTOCOL_MESH)
typedef void (*PacketIndCallback_t)(RECEIVED_MESH_MESSAGE *ind);
#endif
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
bool  MiApp_SubscribeDataIndicationCallback(PacketIndCallback_t callback);


#define NOISE_DETECT_ENERGY 0x00
#define NOISE_DETECT_CS     0x01
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
*                          double word parameter use one bit to represent corresponding
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
uint8_t    MiApp_NoiseDetection(uint32_t ChannelMap, uint8_t ScanDuration, uint8_t DetectionMode, OUTPUT uint8_t *NoiseLevel);

#define POWER_STATE_SLEEP       0x00
#define POWER_STATE_WAKEUP      0x01
#define POWER_STATE_WAKEUP_DR   0x02
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
uint8_t    MiApp_TransceiverPowerState(uint8_t Mode);

/*******************************************************************************************
* Function:
*      bool MiApp_InitChannelHopping(uint32_t ChannelMap)
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
*      uint32_t ChannelMap -  The bit map of the candidate channels
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
bool MiApp_InitChannelHopping( uint32_t ChannelMap);

/********************************************************************************************
* Function:
*      bool MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap)
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
*                          double word parameter use one bit to represent corresponding
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
bool MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap);

/********************************************************************************************
* Function:
*      uint8_t Total_Connections(void)
*
* Summary:
*      This function will tell the total number of active and non active connections in Network
*
* Description:
*        In order to find the no of devices which are connected to a device in p2p and star , we use
*        this function , this will give the total no of active and in active devices in Network
*      By In active -- ? When a device status bit is in valid , that device is considered dead
*        in case of star protocol
* PreCondition:
*      None
*
* Parameters:
*      None
*
* Returns:
*                  Total no of connections available in Connection Table
*
*********************************************************************************************/
uint8_t Total_Connections(void);

#if defined (PROTOCOL_STAR)
// End devices in Star Network get the total no of devices in Ntwrk
// from PAN Co and they save that in variable end_nodes
extern uint8_t end_nodes ;
// Role == PAN CO or END Device
extern bool role;
// In case of star network an FFD||RFD End device also saves its index value.
// Pan Co has connection table where in it store all the FFD || RFD device Information.
// Each FFD and RFD device is provided a Index by PAN CO on Connection Request
// this Index is saved by the FFD || RFD end device
extern  uint8_t  MyindexinPC;

/***************************************************************************
* All Peer FFD's && RFD device Information
* In Star Network PAN CO , broadcasts its Connection Table Information to all its child
* FFD's && RFD's. This information is gives FFD's && RFD's idea of Peer FFD's || RFD's
* Presence
* . It is the element structure.
* Note : The below structure is used by only end devices(FFD's || RFD's) in a star network
**************************************************************************/
typedef struct __END_DEVICES_Unique_Short_Address
{
// Short address of peer END device
uint8_t        Address[3];
//Each end device belongs to a particular connection slot in PAN CO
uint8_t        connection_slot;

} END_DEVICES_Unique_Short_Address;
extern  END_DEVICES_Unique_Short_Address  END_DEVICES_Short_Address[CONNECTION_SIZE];
// lost connection is true if a END device is considered In active by PAN CO
// though the device is active
extern bool lost_connection;
//Used by END_DEVICE to store the index value shared by PAN Co on join
extern uint8_t myConnectionIndex_in_PanCo;

/************************************************************************************
* Function:
*      void MiApp_BroadcastConnectionTable(void)
*
* Summary:
*      This function is  a command type packet which is used by Pan Co
*
* Description:
*      This function is used by only PAN CO in a Star network and is a cmd
*      type packet. PAN CO in our demo is braodcasting Connection Table details
*      with every other END_DEVICE (FFD's || RFD's)in the Network, A new peer
*     END_DEVICE joins PAN C0 , then Pan Co will again broadcast the new connection
*     Table , As its the duty of PAN Co to keep END_DEVICES Updated of the new nodes
*     added to network
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
void MiApp_BroadcastConnectionTable(void);



#if defined(ENABLE_APP_LAYER_ACK)
/************************************************************************************
* Function:
*      bool SW_Ack_SrED(uint8_t *)
*
* Summary:
*      Pan CO uses this function to send sw generated ACK to source end device
*
* Description:
*      This function is used by PAN Co to send a App_layer ACK to source END_DEVICE to denote
*      a successful Data Reception by END_DEVICE
*      Data TX from one END_DEVICE to another END_DEVICE
*      EDx --> PAN CO --> EDy
*      PAN CO generates a sw ACK for EDx to denote a Successful TX.
*      sw generated ACK    :   EDx    <--PANC0
* PreCondition:
*      MiApp_Send_Data_Packet_EDx_to_EDy call at SrcEND_Device
*
* Returns:
*      bool - true or false Ack Sending Success or Fail
*
* Remarks:
*      None
*
*****************************************************************************************/
bool SW_Ack_SrED(uint8_t * );
#endif

#if defined(ENABLE_LINK_STATUS)
/*********************************************************************
* Function:        void send_link_status(void)
*
* PreCondition:     This function will only be used by End devices(FFD's || RFD's)
*
* Input:             None

*
* Output:           None
*
* Side Effects:	    Will Qualify a Device to be active in PAN Co's connection Table
*
* Description :     Function will send a link status , (heart beat ) to PAN CO , which
*                         which denotes that the Device is active participant in Star Network
*
**********************************************************************/
void send_link_status(void);


/*********************************************************************
* Function:        void Find_InActiveDevices(void)
*
* PreCondition:     This function is valid for Pan Co
*
* Input:             None

*
* Output:           None
*
* Side Effects:	    Inactive devices in Network , will be found
*
*
* Description :     This function will check which devices in the Network have been inactive.
*                      If a device link stat is 0x00 , it is qualified as inactive. It is the duty of END
*                      Devices (FFD's || RFD's) to send link status , whenever a END Device , sends
*                      link status , link_stat[ED] increments by 1 , denoting its a Active part of the
*                      Star Network
*
* Note : If a End device has  requested PAN Co to make it a permanently active device , this function
*          does not evaluate that particular END_DEVICE's link status.
**********************************************************************/
void Find_InActiveDevices(void);

#endif

/*********************************************************************
* Function:        void MiApp_leave_network(void)
*
* PreCondition:     This function is valid for End Devices (FFD's && RFD's)
*
* Input:            none

*
* Output:           None
*
* Side Effects:	    Mandatory Leave
*
*
* Description :     This function will be used by End Devices to leave
*                   the Network.The device will be considered inactive
*                   by PAN COR Immediately
*
* Note :               The Device can join back in the Ntwk Again ,
**********************************************************************/
void MiApp_leave_network(void);

/*********************************************************************
* Function:          bool MiApp_UnicastStar  (uint8_t dataLen, uint8_t* dataPtr, bool SecEn)
*
* PreCondition:     This function is valid for End Devices
*
* Input:            byte -- Input from APP layer
*                      Unique index for each END_DEVICE in Star Network
*                       This value is stored as magic_no in the END_DEVICE
*                      *pointer--> User Data Pointer
*
* Output:               True/ Fale -- Success TX or Fail TX
*
* Side Effects:     None
*
*
* Description :     This function will be used by End Devices to TX data to another
*                          END_DEVICE in a STAR Network
*
* Note :              TX happens EDx--> PAN CO--> EDy
*                          First 4 bytes are occupied by the cmd and DestEndDevice address
*                          Remaining space is user data
**********************************************************************/
bool MiApp_UnicastStar  (uint8_t dataLen, uint8_t* dataPtr, bool SecEn);

#endif

/************************************************************************************
* Function:
*      bool MiApp_Get(set_params id, uint8_t *value )
*
* Summary:
*      This function gets the information base values
*
* Description:
*      This is the primary user interface function to get the different values in the MiWi
*      stack
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      get_params id -  The identifier of the value to be set
*
* Returns:
*      a boolean to indicate if get operation has been performed successfully
*
* Example:
*      <code>
*      value = MiApp_get(CHANNEL)
*      </code>
*
* Remarks:
*      None
*
*********************************************************************************/
bool  MiApp_Get(miwi_params_t id, uint8_t *value);

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
bool MiApp_ResetToFactoryNew(void);
#endif

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
void P2PTasks(void);
void DumpConnection(INPUT uint8_t index);
#endif
#if defined(PROTOCOL_MESH)
void MeshTasks(void);
#endif

typedef void (*roleUpgrade_callback_t)(uint16_t newShortAddress);
#ifdef COORDINATOR
/************************************************************************************
* Function:
* bool MiApp_RoleUpgradeNotification_Subscribe(roleUpgrade_callback_t callback)
*
* Summary:
*      This function subscribes for role upgrade notification
*
* Description:
*      This is used to subscribe to notify the role upgrade. Upon successful role
*      upgrade, callback will be called with new short address
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      roleUpgrade_callback_t callback - The callback routine which will be called upon
*                                               the role upgrade done
*
* Returns:
*      A boolean to indicates if the subscription is success or not
*
*****************************************************************************************/
bool MiApp_RoleUpgradeNotification_Subscribe(roleUpgrade_callback_t callback);
#endif

/************************************************************************************
* Function:
*      bool MiApp_Commissioning_AddNewDevice(uint64_t joinerAddress, bool triggerBloomUpdate)
*
* Summary:
*      This function adds the given address into the commissioning device address list
*      if free entry available
*
* Parameters:
*      uint64_t joinerAddress - the ieee address to be added.
*      bool triggerBloomUpdate - if set to true then bloom update is sent immediately
*
* Returns:
*      true if found, false otherwise.
*****************************************************************************************/
bool MiApp_Commissioning_AddNewDevice(uint64_t joinerAddress, bool triggerBloomUpdate);

#if defined(PROTOCOL_MESH)
/************************************************************************************
* Function:
* uint16_t MiApp_MeshGetNextHopAddr(uint16_t destAddress)
*
* Summary:
*      This function gets the next hop address for the given destination address
*
* Description:
*      This is used to get the next hop address for the given destination address
*  from the routing table.
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      uint16_t destAddress - any valid network address
*
* Returns:
*      the next hop address if available, otherwise 0xFFFF
*
*****************************************************************************************/
uint16_t MiApp_MeshGetNextHopAddr(uint16_t destAddress);

typedef void (*LinkFailureCallback_t)(void);
/************************************************************************************
* Function:
* bool MiApp_SubscribeLinkFailureCallback(LinkFailureCallback_t callback)
*
* Summary:
*      This function subscribes for link failure notification
*
* Description:
*      This is used to subscribe to notify the link failure. Upon link failure in
*  coordinator or end device, this callback will be called.
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      LinkFailureCallback_t callback - The callback routine which will be called upon
*                                               link failure
*
* Returns:
*      A boolean to indicates if the subscription is success or not
*
*****************************************************************************************/
bool MiApp_SubscribeLinkFailureCallback(LinkFailureCallback_t callback);

typedef void (*ReconnectionCallback_t)(miwi_status_t status);
/************************************************************************************
* Function:
* bool MiApp_SubscribeReConnectionCallback(ReconnectionCallback_t callback)
*
* Summary:
*      This function subscribes for reconnection notification
*
* Description:
*      This is used to subscribe to notify the reconnection. Upon reconnection in
*  coordinator or end device, this callback will be called.
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      ReconnectionCallback_t callback - The callback routine which will be called upon
*                                              reconnection
*
* Returns:
*      A boolean to indicates if the subscription is success or not
*
*****************************************************************************************/
bool MiApp_SubscribeReConnectionCallback(ReconnectionCallback_t callback);

/************************************************************************************
* Function:
* bool MiApp_ManuSpecSendData(uint8_t addr_len, uint8_t *addr, uint8_t msglen, 
* uint8_t *msgpointer, uint8_t msghandle, DataConf_callback_t ConfCallback);
*
* Summary:
*      This function unicast a message in the msgpointer to the device with DestinationAddress
*
* Description:
*      This is one of the primary user interface functions for the application layer to
*      unicast a message. The destination device is specified by the input parameter
*      DestinationAddress. The application payload is filled using msgpointer.
*
* PreCondition:
*      Protocol initialization has been done.
*
* Parameters:
*      uint8_t addr_len - destination address length
*      uint8_t *addr  - destionation address
*      uint8_t msglen - length of the message
*      uint8_t *msgpointer - message/frame pointer
*      uint8_t msghandle - message handle
*      bool ackReq - set to receive network level ack (Note- Discarded for broadcast data)
*      DataConf_callback_t ConfCallback - The callback routine which will be called upon
*                                               the initiated data procedure is performed
*
* Returns:
*      A boolean to indicates if the unicast procedure is successful.
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
bool MiApp_ManuSpecSendData(uint8_t addr_len, uint8_t *addr, uint8_t msglen, uint8_t *msgpointer,
uint8_t msghandle, bool ackReq, DataConf_callback_t ConfCallback);

/************************************************************************************
* Function:
*      bool  MiApp_SubscribeMAnuSpecDataIndicationCallback(PacketIndCallback_t callback)
*
* Summary:
*      This function return a boolean if subscription for rx message is successful
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
*      if( true == MiApp_SubscribeMAnuSpecDataIndicationCallback(ind) )
*      {
*      }
*      </code\
*
* Remarks:
*      None
*
*****************************************************************************************/
bool  MiApp_SubscribeManuSpecDataIndicationCallback(PacketIndCallback_t callback);

bool MiApp_IsConnected(void);

#ifdef MIWI_MESH_TOPOLOGY_SIMULATION_MODE
void MiApp_MeshSetRouteEntry(uint8_t coordIndex, RouteEntry_t *routeEntry);
void MiApp_MeshGetRouteEntry(uint8_t coordIndex, RouteEntry_t *routeEntry);
#endif
#endif

#ifdef ENDDEVICE
/************************************************************************************
* Function:
* bool MiApp_ReadyToSleep(uint32_t* sleepTime)
*
* Summary:
*      This function informs whether the device can go sleep or not and how much time
*      it can sleep
*
* Description:
*      This is used to understand the stack is ready to sleep and how much time stack
*      allows to sleep if it is ready.
*
* Parameters:
*      uint32_t* sleepTime - Pointer to sleep time which specifies the sleepable time
*                            when stack is ready to sleep
*
* Returns:
*      A boolean to indicates that stack is ready to sleep or not
*
*****************************************************************************************/
bool MiApp_ReadyToSleep(uint32_t* sleepTime);
#endif

// Callback functions
#define MiApp_CB_AllowConnection(handleInConnectionTable) true
//BOOL MiApp_CB_AllowConnection(uint8_t handleInConnectionTable);
#define MiApp_CB_RFDAcknowledgement(SourceShortAddress, AcknowledgementSeqNum)
//void MiApp_CB_RFDAcknowledgement(uint16_t sourceAddr, uint8_t Seq);
extern RECEIVED_MESSAGE rxMessage;
#if defined(ENABLE_TIME_SYNC)
extern API_UINT16_UNION CounterValue;
extern API_UINT16_UNION WakeupTimes;
#endif
#endif
