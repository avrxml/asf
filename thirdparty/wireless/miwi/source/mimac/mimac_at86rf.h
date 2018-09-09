/**
* \file  mimac_at86rf.h
*
* \brief MAC Layer Abstraction for AT86RFx interface
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

#ifndef __MIMAC_AT86RF_H
#define __MIMAC_AT86RF_H

	#include "miwi_config.h"
	#include "sysTimer.h"


	#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
        #include "miwi_p2p.h"
		#include "miwi_config_p2p.h"
    #endif

    #if defined(PROTOCOL_MESH)
		//#include "miwi_config_mesh.h"
	   //#include "miwi_mesh.h"
	#endif

	#if (defined(PHY_AT86RF212B))
	#define FULL_CHANNEL_MAP        0x000007FF
	#define FIRST_CHANNEL_NUM      0
	#define LAST_CHANNEL_NUM       10
	#elif (defined(PHY_AT86RF233))
	#define FULL_CHANNEL_MAP        0x07FFF800
	#define FIRST_CHANNEL_NUM      11
	#define LAST_CHANNEL_NUM       26
	#endif
	#define ANUMSUPERFRAMESLOTS			(16)
	#define ABASESLOTDURATION			(60)
	#define ABASESUPERFRAMEDURATION		(ABASESLOTDURATION * ANUMSUPERFRAMESLOTS )

	#ifdef PHY_AT86RF233
	#define SYMBOLS_TO_TICKS(a)     ((a)*(16*ONE_MICRO_SECOND))
	#define TICKS_TO_SYMBOLS(a)     ((a)/(16*ONE_MICRO_SECOND))
	#elif (defined PHY_AT86RF212B )
	#define SYMBOLS_TO_TICKS(a)     ((currentChannel == 0)?((a)*(50*ONE_MICRO_SECOND)):((a)*(25*ONE_MICRO_SECOND)))
	#define TICKS_TO_SYMBOLS(a)     ((currentChannel == 0)?((a)/(50*ONE_MICRO_SECOND)):((a)/(25*ONE_MICRO_SECOND)))
	#endif

    #define CHANNEL_ASSESSMENT_CARRIER_SENSE    0x00
    #define CHANNEL_ASSESSMENT_ENERGY_DETECT    0x01

    #define POWER_STATE_DEEP_SLEEP              0x00
    #define POWER_STATE_OPERATE                 0xFF


    #define PACKET_TYPE_MASK        0x03
    #define BROADCAST_MASK          0x04
    #define SECURITY_MASK           0x08
    #define REPEAT_MASK             0x10
    #define ACK_MASK                0x20
    #define DSTPRSNT_MASK           0x40
    #define SRCPRSNT_MASK           0x80

    #define PACKET_TYPE_DATA        0x00
    #define PACKET_TYPE_COMMAND     0x01
    #define PACKET_TYPE_ACK         0x02
    #define PACKET_TYPE_RESERVE     0x03


    // DOM-IGNORE-BEGIN
    /*********************************************************************
     Overview: Data types for drivers. This will facilitate easy
               access smaller chunks of larger data types when sending
               or receiving data (for example byte sized send/receive
               over parallel 8-bit interface.
    *********************************************************************/
    // DOM-IGNORE-END


    typedef union _DRIVER_UINT8_UNION_
    {
        uint8_t Val;
        struct
        {
        uint8_t b0:1;
        uint8_t b1:1;
        uint8_t b2:1;
        uint8_t b3:1;
        uint8_t b4:1;
        uint8_t b5:1;
        uint8_t b6:1;
        uint8_t b7:1;
        } bits;

    } DRIVER_UINT8_UNION;



	/*- Types ------------------------------------------------------------------*/
	typedef struct PHY_DataInd_t {
		uint8_t *data; // Pointer to Data
		uint8_t size;   // Size of the frame
		uint8_t lqi;    // Link Quality Index
		int8_t rssi;    // RSSI
	} PHY_DataInd_t;


    /***************************************************************************
     * Parameters to Transmit a Packet
     *
     *      This structure contains configurations to transmit a packet
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t    Val;
            struct
            {
                uint8_t        packetType      : 2;        // type of packet. Possible types are
                                                        // * PACKET_TYPE_DATA - Data type
                                                        // * PACKET_TYPE_COMMAND -  Command type
                                                        // * PACKET_TYPE_ACK -  Acknowledgement type
                                                        // * PACKET_TYPE_RESERVE - Reserved type
                uint8_t        broadcast       : 1;        // 1: broadcast, 0: unicast
                uint8_t        secEn           : 1;        // 1: secure the MAC payload, 0: send plain text
                uint8_t        repeat          : 1;        // 1: allow repeaters to forward the message, 0: send message directly
                uint8_t        ackReq          : 1;        // 1: acknowledgement required, 0: no acknowldgement
                uint8_t        destPrsnt       : 1;        // 1: destination address in the packet, 0: destination address not in the packet
                uint8_t        sourcePrsnt     : 1;        // 1: source address in the packet, 0: source address not in the packet
            } bits;
        } flags;

        uint8_t        *DestAddress;           // destination address
        #if defined(IEEE_802_15_4)
            bool                        altDestAddr;        // use the alternative network address as destination in the packet
            bool                        altSrcAddr;         // use the alternative network address as source in the packet
            API_UINT16_UNION         DestPANID;          // PAN identifier of the destination
        #endif

    } MAC_TRANS_PARAM;


    /***************************************************************************
     * Content of the Received Message
     *
     *      This structure contains all information of the received message
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t        Val;
            struct
            {
                uint8_t    packetType      :2;             // type of packet. Possible types are
                                                        // * PACKET_TYPE_DATA - Data type
                                                        // * PACKET_TYPE_COMMAND -  Command type
                                                        // * PACKET_TYPE_ACK -  Acknowledgement type
                                                        // * PACKET_TYPE_RESERVE - Reserved type
                uint8_t    broadcast       :1;             // 1: broadcast, 0: unicast
                uint8_t    secEn           :1;             // 1: secure the MAC payload, 0: send plain text
                uint8_t    repeat          :1;             // 1: allow repeaters to forward the message, 0: send message directly
                uint8_t    ackReq          :1;             // 1: acknowledgement required, 0: no acknowldgement
                uint8_t    destPrsnt       :1;             // 1: destination address in the packet, 0: destination address not in the packet
                uint8_t    sourcePrsnt     :1;             // 1: source address in the packet, 0: source address not in the packet
            } bits;
        } flags;

        uint8_t *      SourceAddress;                      // Address of the Sender
        uint8_t *      Payload;                            // Pointer to the payload
        uint8_t        PayloadLen;                         // Payload size
        uint8_t        RSSIValue;                          // RSSI value for the received packet
        uint8_t        LQIValue;                           // LQI value for the received packet
        #if defined(IEEE_802_15_4)
            bool                    altSourceAddress;               // Source address is the alternative network address
            API_UINT16_UNION     SourcePANID;                    // PAN ID of the sender
        #endif
    } MAC_RECEIVED_PACKET;

    /***************************************************************************
     * Initialization Parameters for MAC
     *
     *      This structure contains parameters in the initialization
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t        Val;
            struct
            {
                uint8_t    RepeaterMode    :1;             // 1: Allow to act as repeater, 0: disable repeater function
                uint8_t    CCAEnable       :1;             // 1: Enable Clear Channel Assessment, 0: Disable CCA
                uint8_t    NetworkFreezer  :1;
                uint8_t    PAddrLength     :4;             // Length of the permanent address, range from 1 to 8.
            } bits;
        } actionFlags;

        uint8_t *PAddress;                                 // Permenet transceiver address

    } MACINIT_PARAM;

    extern MAC_RECEIVED_PACKET  MACRxPacket;

	enum mac_set_params
   {
      MAC_CHANNEL
    };

    typedef enum mac_set_params mac_set_params_t;

    /************************************************************************************
     * Function:
     *      bool MiMAC_Set(mac_set_params_t id, uint8_t *value);
     *
     * Summary:
     *      This function sets the values
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      set the valuesr. for ex:Valid channel
     *      number are from 0 to 31. For different frequency band, data rate
     *      and other RF settings, some channels from 0 to 31 might be
     *      unavailable. Paramater offsetFreq is used to fine tune the center
     *      frequency across the frequency band. For transceivers that follow
     *      strict definition of channels, this parameter may be discarded.
     *      The center frequency is calculated as
     *      (LowestFrequency + Channel * ChannelGap + offsetFreq)
     *
     * PreCondition:
     *      Hardware initialization on MCU has been done.
     *
     * Parameters:
     *      set_params id -  The identifier of the value to be set
     *      value - value to be set
     *
     * Returns:
     *      A boolean to indicates if channel setting is successful.
     *
     * Example:
     *      <code>
     *      // Set center frequency to be exactly channel 12
     *      MiMAC_Set(CHANNEL, &channel);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
	bool MiMAC_Set(mac_set_params_t id, uint8_t *value);


    /************************************************************************************
     * Function:
     *      bool MiMAC_SetPower(uint8_t outputPower)
     *
     * Summary:
     *      This function set the output power for the RF transceiver
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      set the output power for the RF transceiver. Whether the RF
     *      transceiver can adjust output power depends on the hardware
     *      implementation.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      uint8_t outputPower -  RF transceiver output power.
     *
     * Returns:
     *      A boolean to indicates if setting output power is successful.
     *
     * Example:
     *      <code>
     *      // Set output power to be 0dBm
     *      MiMAC_SetPower(TX_POWER_0_DB);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_SetPower(uint8_t outputPower);


    /************************************************************************************
     * Function:
     *      bool MiMAC_SetAltAddress(uint8_t *Address, uint8_t *PANID)
     *
     * Summary:
     *      This function set the alternative network address and PAN identifier if
     *      applicable
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      set alternative network address and/or PAN identifier. This function
     *      call applies to only IEEE 802.15.4 compliant RF transceivers. In case
     *      alternative network address is not supported, this function will return
     *      FALSE.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      uint8_t * Address -    The alternative network address of the host device.
     *      uint8_t * PANID -      The PAN identifier of the host device
     *
     * Returns:
     *      A boolean to indicates if setting alternative network address is successful.
     *
     * Example:
     *      <code>
     *      uint16_t NetworkAddress = 0x0000;
     *      uint16_t PANID = 0x1234;
     *      MiMAC_SetAltAddress(&NetworkAddress, &PANID);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_SetAltAddress(uint8_t *Address, uint8_t *PANID);

    /************************************************************************************
     * Function:
     *      bool MiMAC_Init(MACINIT_PARAM initValue)
     *
     * Summary:
     *      This function initialize MiMAC layer
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      initialize the MiMAC layer. The initialization parameter is
     *      assigned in the format of structure MACINIT_PARAM.
     *
     * PreCondition:
     *      MCU initialization has been done.
     *
     * Parameters:
     *      MACINIT_PARAM initValue -   Initialization value for MiMAC layer
     *
     * Returns:
     *      A boolean to indicates if initialization is successful.
     *
     * Example:
     *      <code>
     *      MiMAC_Init(initParameter);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_Init(MACINIT_PARAM initValue);

    /************************************************************************************
     * Function:
     *      uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode)
     *
     * Summary:
     *      This function perform the noise detection on current operating channel
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      perform the noise detection scan. Not all assessment modes are supported
     *      for all RF transceivers.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      uint8_t AssessmentMode -   The mode to perform noise assessment. The possible
     *                              assessment modes are
     *                              * CHANNEL_ASSESSMENT_CARRIER_SENSE Carrier sense detection mode
     *                              * CHANNEL_ASSESSMENT_ENERGY_DETECT Energy detection mode
     *
     * Returns:
     *      A byte to indicate the noise level at current channel.
     *
     * Example:
     *      <code>
     *      NoiseLevel = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_CARRIER_SENSE);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode);

	/************************************************************************************
	* Function:
	*      uint32_t MiMAC_SymbolToTicks(uint32_t symbols)
	*
	* Summary:
	*      This function converts symbol to ticks
	*
	* Description:
	*      This is the primary MiMAC interface for the protocol layer to
	*      convert symbol to ticks for all RF transceivers.
	*
	* Parameters:
	*      uint8_t symbols - The number of symbols to convert
	*
	* Returns:
	*      converted value in uint32.
	*****************************************************************************************/
	uint32_t MiMAC_SymbolToTicks(uint32_t symbols);
	
	/************************************************************************************
	* Function:
	*      uint32_t MiMAC_GetPHYChannelInfo(uint32_t supportedChannelMap)
	*
	* Summary:
	*      This function gets the supported channel map
	*
	* Description:
	*      This is the primary MiMAC interface for the protocol layer to
	*      get the supported channel mask
	*
	* Parameters:
	*      None
	*
	* Returns:
	*      channel map uint32.
	*****************************************************************************************/
	uint32_t MiMAC_GetPHYChannelInfo(void);


    /************************************************************************************
     * Function:
     *      void MiMAC_DiscardPacket(void)
     *
     * Summary:
     *      This function discard the current packet received from the RF transceiver
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      discard the current packet received from the RF transceiver.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      None
     *
     * Returns:
     *      None
     *
     * Example:
     *      <code>
     *      if( TRUE == MiMAC_ReceivedPacket() )
     *      {
     *          // handle the raw data from RF transceiver
     *
     *          // discard the current packet
     *          MiMAC_DiscardPacket();
     *      }
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    void MiMAC_DiscardPacket(void);


    /************************************************************************************
     * Function:
     *      bool MiMAC_ReceivedPacket(void)
     *
     * Summary:
     *      This function check if a new packet has been received by the RF transceiver
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      check if a packet has been received by the RF transceiver. When a packet has
     *      been received, all information will be stored in the global variable
     *      MACRxPacket in the format of MAC_RECEIVED_PACKET;
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      None
     *
     * Returns:
     *      A boolean to indicate if a packet has been received by the RF transceiver.
     *
     * Example:
     *      <code>
     *      if( true == MiMAC_ReceivedPacket() )
     *      {
     *          // handle the raw data from RF transceiver
     *
     *          // discard the current packet
     *          MiMAC_DiscardPacket();
     *      }
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_ReceivedPacket(void);


    /************************************************************************************
     * Function:
     *      BOOL MiMAC_SendPacket(  MAC_TRANS_PARAM transParam,
     *                              uint8_t *MACPayload, uint8_t MACPayloadLen,
                                    uint8_t msghandle,MiMacDataConf_callback_t ConfCallback)
     *
     * Summary:
     *      This function transmit a packet
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      send a packet. Input parameter transParam configure the way
     *      to transmit the packet.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      MAC_TRANS_PARAM transParam -    The struture to configure the transmission way
     *      uint8_t * MACPaylaod -             Pointer to the buffer of MAC payload
     *      uint8_t MACPayloadLen -            The size of the MAC payload
     *      uint8_t msghandle                   Message handle
     *      MiMacDataConf_callback_t ConfCallback Callback function to be called once packet is sent
     *
     * Returns:
     *      A boolean to indicate if a packet has been received by the RF transceiver.
     *
     * Example:
     *      <code>
     *      MiMAC_SendPacket(transParam, MACPayload, MACPayloadLen, 6, callback);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_SendPacket(MAC_TRANS_PARAM transParam, uint8_t *MACPayload, uint8_t MACPayloadLen, uint8_t msghandle,
                                                                             DataConf_callback_t ConfCallback);


    /************************************************************************************
     * Function:
     *      bool MiMAC_PowerState(uint8_t PowerState)
     *
     * Summary:
     *      This function puts the RF transceiver into sleep or wake it up
     *
     * Description:
     *      This is the primary MiMAC interface for the protocol layer to
     *      set different power state for the RF transceiver. There are minimal
     *      power states defined as deep sleep and operating mode. Additional
     *      power states can be defined for individual RF transceiver depends
     *      on hardware design.
     *
     * PreCondition:
     *      MiMAC initialization has been done.
     *
     * Parameters:
     *      uint8_t PowerState -   The power state of the RF transceiver to be set to.
     *                          The minimum definitions for all RF transceivers are
     *                          * POWER_STATE_DEEP_SLEEP RF transceiver deep sleep mode.
     *                          * POWER_STATE_OPERATE RF transceiver operating mode.
     * Returns:
     *      A boolean to indicate if chaning power state of RF transceiver is successful.
     *
     * Example:
     *      <code>
     *      // Put RF transceiver into sleep
     *      MiMAC_PowerState(POWER_STATE_DEEP_SLEEP);
     *      // Put MCU to sleep
     *      Sleep();
     *      // Wake up the MCU by WDT, external interrupt or any other means
     *
     *      // Wake up the RF transceiver
     *      MiMAC_PowerState(POWER_STATE_OPERATE);
     *      </code>
     *
     * Remarks:
     *      None
     *
     *****************************************************************************************/
    bool MiMAC_PowerState(uint8_t PowerState);

    #if defined(IEEE_802_15_4)
        #undef MY_ADDRESS_LENGTH
        #define MY_ADDRESS_LENGTH       8
    #endif

    #if defined(ENABLE_SECURITY)
        extern API_UINT32_UNION IncomingFrameCounter[CONNECTION_SIZE];
    #endif

    void UserInterruptHandler(void);

	void MiMAC_Task(void);

#endif

