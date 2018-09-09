/**
* \file  miwi_mesh_app.c
*
* \brief MiWi Mesh Protocol Application Handling implementation
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
#include "string.h"
#include "miwi_config_mesh.h"
#include "miwi_api.h"

CommDeviceTable_t commnDeviceTable[MAX_NUMBER_OF_DEVICES_IN_NETWORK];

RebroadcastTable_t reBroadcastTable[REBROADCAST_TABLE_SIZE];

/* Network Tables */
DeviceTable_t deviceTable[NUM_OF_NONSLEEPING_ENDDEVICES];
SleepDeviceTable_t sleepdeviceTable[NUM_OF_SLEEPING_ENDDEVICES];
CoordinatorTable_t coordinatorTable[NUM_OF_COORDINATORS];

CoordRouteTable_t coordRouteTable[NUM_OF_COORDINATORS];
CoordHopCount_t coordHopCount[HOP_TABLE_COUNT];

/* Holds the Network Key of the device if device is in network state */
uint8_t networkKey[SECURITY_KEY_SIZE] = NETWORK_KEY_DEFAULT;

/* Holds the default public key of the network */
uint8_t publicKey[SECURITY_KEY_SIZE] = PUBLIC_KEY_DEFAULT;

#ifndef PAN_COORDINATOR
/* Holds the memory for storing beacon results */
searchConf_t searchConfirmMem;
#endif


defaultParametersRomOrRam_t defaultParamsRomOrRam = {
#ifndef ENDDEVICE
#ifdef PAN_COORDINATOR
	.coordTable = &coordinatorTable[0],
	.commDevTable = &commnDeviceTable[0],
#endif
	.devTable = &deviceTable[0],
	.sleepingDevTable = &sleepdeviceTable[0],
	.coordinatorRouteTable = &coordRouteTable[0],
	.coordinatorHopCount = &coordHopCount[0],
	.rebroadcastTable = &reBroadcastTable[0],

	.numOfCoordinators = NUM_OF_COORDINATORS,

	.maxNumOfDevicesInNetwork = MAX_NUMBER_OF_DEVICES_IN_NETWORK,

	.keepAliveCoordSendInterval = KEEP_ALIVE_COORDINATOR_SEND_INTERVAL,
	.keepAliveCoordTimeoutSec = KEEP_ALIVE_COORDINATOR_TIMEOUT_IN_SEC ,
	.roleUpgradeIntervalInSec = ROLE_UPGRADE_INTERVAL_IN_SEC,
	.numOfRxOnEnddevices = NUM_OF_NONSLEEPING_ENDDEVICES,
	.numOfRxOffEnddevices = NUM_OF_SLEEPING_ENDDEVICES,

	.routeUpdateInterval = ROUTE_UPDATE_INTERVAL,
	.routeReqWaitInterval = ROUTE_REQ_WAIT_INTERVAL,
	.indirectDataWaitInterval = INDIRECT_DATA_WAIT_INTERVAL,
	.rebroadcastTableSize = REBROADCAST_TABLE_SIZE,
	.rebroadcastTimeout = REBROADCAST_TIMEOUT,
#endif

	.keepAliveRxOnEdSendInterval = KEEP_ALIVE_RXONENDDEVICE_SEND_INTERVAL,
	.keepAliveRxOnEdTimeoutSec = KEEP_ALIVE_RXONENDDEVICE_TIMEOUT_IN_SEC,
	.deviceTimeout = RXOFF_DEVICE_TIMEOUT_IN_SEC,
	.dataRequestInterval = DATA_REQUEST_SEND_INTERVAL,
	.edLinkFailureAttempts = ED_LINK_FAILURE_ATTEMPTS,
	.connRespWaitInSec = CONNECTION_RESPONSE_WAIT_IN_SEC,

	.frameAckWaitInterval = FRAME_ACK_WAIT_INTERVAL,
	.frameRetry = FRAME_RETRY,
#ifndef PAN_COORDINATOR
    .searchConfMem = &searchConfirmMem,
	.maxNoOfBeacons = MAX_BEACON_RESULTS,
#endif
};

defaultParametersRamOnly_t defaultParamsRamOnly = {
#ifdef MESH_SECURITY
	.miwiPublicKey = PUBLIC_KEY_DEFAULT,
	.miwiNetworkKey = NETWORK_KEY_DEFAULT,
	.securityLevel = MESH_SECURITY_LEVEL,
#endif
    .dummy = 0,
};
