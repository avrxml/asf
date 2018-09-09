/**
* \file  mimac_at86rf.c
*
* \brief MAC Layer Abstraction for AT86RFx implementation
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

#include <string.h>
#include <stdbool.h>
#include <trx_access.h>
#include "miwi_config.h"
#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
   #include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#endif

#include "delay.h"
#include "sal.h"
#include "phy.h"
#include "mimac_at86rf.h"

#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsDataServer.h"
#endif

#ifdef ENABLE_SECURITY
	const char mySecurityKey[16] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
		SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11,
	SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};
	uint8_t final_mic_value[16] , CBC_mic[16] , CTR_mic[16];
	uint8_t InitVect_in[16];

	const uint8_t myKeySequenceNumber = KEY_SEQUENCE_NUMBER; // The sequence number of security key. Used to identify the security key

	API_UINT32_UNION OutgoingFrameCounter;
#endif


/*- Variables --------------------------------------------------------------*/
uint8_t MACCurrentChannel;
API_UINT16_UNION MAC_PANID;
API_UINT16_UNION myNetworkAddress;
uint8_t IEEESeqNum;
MACINIT_PARAM MACInitParams;
uint8_t BankIndex = 0xFF;
// Mic Size is 4 bytes.
uint8_t calculated_mic_values[AES_BLOCKSIZE/4];
uint8_t received_mic_values[AES_BLOCKSIZE/4];

bool dataConfAvailable = 0;
miwi_status_t dataStatus;
uint8_t dataHandle = 0;
uint8_t* dataPointer = NULL;
DataConf_callback_t dataConfCallback = NULL;

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

bool MiMAC_SetAltAddress( uint8_t *Address,  uint8_t *PanId)
{
	myNetworkAddress.v[0] = Address[0];
	myNetworkAddress.v[1] = Address[1];
	MAC_PANID.v[0] = PanId[0];
	MAC_PANID.v[1] = PanId[1];
	PHY_SetShortAddr(myNetworkAddress.Val);
	PHY_SetPanId(MAC_PANID.Val);
	return true;
}

#if defined (ENABLE_SECURITY)
uint8_t addr[8] = {0xda , 0x98 , 0x22 , 0xfe , 0xff , 0xa3 , 0x04 , 0x00};
void mic_generator (uint8_t *Payloadinfo, uint8_t len , uint8_t frame_control ,  API_UINT32_UNION FrameCounter ,  uint8_t *SourceIEEEAddress)
{
	uint8_t i , j , iterations  , copy_packet1[128] , cbc_header_with_padding[16];
	// Calculating No of blocks in the packet (1 block = 16 bytes of data)
	iterations = len/16;
	if (len % 16 != 0) iterations++;
	InitVect_in[0] = 0x49;//frame_control;
	cbc_header_with_padding[0] = 0x00;  // l(a)
	cbc_header_with_padding[1] = 0x0d;  // l(a)Header Length
	cbc_header_with_padding[2] = frame_control;  //frame_control;  //byte 1
	for (i=0;i<8;i++)
	{
		InitVect_in[i+1] =  SourceIEEEAddress[i];
		cbc_header_with_padding[i+7] = SourceIEEEAddress[i];
	}
	for (i=0;i<4;i++)
	{
		InitVect_in[i+9] = FrameCounter.v[i];
		cbc_header_with_padding[i+3] = FrameCounter.v[i];
	}
	//cbc_header_with_padding[14] = 0x00;  // Padding
	cbc_header_with_padding[15] = 0x00;   // Padding
	InitVect_in[13] = SECURITY_LEVEL; //Security Mode // 0x04 --> CCM-32
	InitVect_in[14] = 0x00; // 15 th byte padded to zero
	InitVect_in[15] = len; // For MIC calculation All preceding blocks will be incremented by 1
	PHY_EncryptReq(&InitVect_in[0], (uint8_t *)mySecurityKey);
	for (i=0;i<16;i++)
	{
		copy_packet1[i] = cbc_header_with_padding[i];
	}
	// Copy the Payload and Do the padding
	for (i=16;i<len+16;i++)
	{
		copy_packet1[i]	= Payloadinfo[i-16];  // Copying the Payload
	}
	for (i=len+16;i<(iterations *16) + 16 ; i++ )
	{
		copy_packet1[i] = 0; // Padding
	}
	for (i=0;i<iterations+1;i++)
	{
		for (j=0;j<16;j++)
		{
			InitVect_in[j]	= InitVect_in[j] ^ copy_packet1[j+(i*16)];
		}
		PHY_EncryptReq(&InitVect_in[0], (uint8_t *)mySecurityKey);
	}
	for (i=0;i<16;i++)
	{
		CBC_mic[i] = InitVect_in[i];
	}

}

/************************************************************************************
 * Function:
 *      bool DataEncrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        API_UINT32_UNION FrameCounter, uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:
 *      This is the function to encrypt the transmitting packet. All parameters are
 *      input information used in the encryption process. After encryption is
 *      performed successfully, the result will be put into the buffer that is
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:
 *      Transceiver initialization has been done.
 *
 * Parameters:
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input plain payload and output
 *                            encrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input plain payload and
 *                            output encrypted payload
 *      uint8_t * DestIEEEAddress      - The IEEE address of the packet target
 *      API_UINT32_UNION FrameCounter      - Frame counter of the transmitting packet
 *      uint8_t FrameControl   - The frame control byte of the transmitting packet
 *
 * Returns:
 *      A boolean to indicates if encryption is successful.
 *
 * Example:
 *      <code>
 *      DataEncrypt(key, payload, &payloadLen, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:
 *      None
 *
 *****************************************************************************************/

bool DataEncrypt(uint8_t *Payloadinfo, uint8_t *Payload_len, API_UINT32_UNION FrameCounter,
uint8_t FrameControl)
{
	uint8_t i , iterations , block[16] , j , CTR_Nonce_and_Counter[16];
	// Calculating No of blocks in the packet (1 block = 16 bytes of data)
	iterations = *Payload_len/16;
	if (*Payload_len % 16 != 0) iterations++;

	mic_generator(&Payloadinfo[0] , *Payload_len, FrameControl  , FrameCounter ,  MACInitParams.PAddress);

	for (i=*Payload_len;i<iterations *16 ; i++ )
	{
		Payloadinfo[i] = 0; // Padding
	}
	CTR_Nonce_and_Counter[0] = 0x01;  // L
	for (i=0;i<8;i++)
	{
		CTR_Nonce_and_Counter[i+1] = MACInitParams.PAddress[i];
	}
	for (i=0;i<4;i++)
	{
		CTR_Nonce_and_Counter[i+9] = FrameCounter.v[i];
	}
	CTR_Nonce_and_Counter[13] = SECURITY_LEVEL; //Security Mode // 0x04 --> CCM-32
	CTR_Nonce_and_Counter[14] = 0x00; // 15 th byte padded to zero
	CTR_Nonce_and_Counter[15] = 0x00; // For MIC calculation All preceding blocks will be incremented by 1

	for (i=0;i<iterations+1;i++)
	{
		for (j=0;j<16;j++)
		{
			block[j] = CTR_Nonce_and_Counter[j];
		}
		PHY_EncryptReq(&block[0], (uint8_t *)mySecurityKey);
		for (j=0;j<16;j++)
		{
			if (CTR_Nonce_and_Counter[15] == 0)
			{
				final_mic_value[j] = block[j] ^ CBC_mic[j]; // CTR_MIC XOR CBC_MIC gives final MIC values
			}
			else
			{
				Payloadinfo[j+(i-1)*16] = block[j] ^ Payloadinfo[j+(i-1)*16];
			}
		}
		CTR_Nonce_and_Counter[15]++; // Increment Counter for next opration
	}
	return true;

}
// Validates the Received mic with the mic computed from data packet decryption.
bool validate_mic(void)
{
	if (final_mic_value[0] != received_mic_values[0] || final_mic_value[1] != received_mic_values[1])
	{
		return false;
	}
	else
	{
		return true;
	}
}


/************************************************************************************
 * Function:
 *      bool DataDecrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        uint8_t *SourceIEEEAddress, API_UINT32_UNION FrameCounter,
 *                        uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:
 *      This is the function to decrypt the secured packet. All parameters are
 *      input information used in the decryption process. After decryption is
 *      performed successfully, the result will be put into the buffer that is
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:
 *      Transceiver initialization has been done.
 *
 * Parameters:
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input secured payload and output
 *                            decrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input secured payload and
 *                            output decrypted payload
 *      uint8_t * SourceIEEEAddress    - The IEEE address of the package originator
 *      API_UINT32_UNION FrameCounter      - Frame counter of the received packet
 *      uint8_t FrameControl   - The frame control byte of the received packet
 *
 * Returns:
 *      A boolean to indicates if decryption is successful.
 *
 * Example:
 *      <code>
 *      DataDecrypt(key, payload, &payloadLen, SourceIEEEAddr, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:
 *      None
 *
 *****************************************************************************************/
bool DataDecrypt(uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress,
API_UINT32_UNION FrameCounter, uint8_t FrameControl)
{
	uint8_t i , iterations , block[16] , j , CTR_Nonce_and_Counter[16];
	// Calculating No of blocks in the packet (1 block = 16 bytes of data)
	iterations = *PayloadLen/16;
	if (*PayloadLen % 16 != 0) iterations++;

	//mic_generator(&Payloadinfo[0] , *Payload_len);

	for (i=*PayloadLen;i<iterations *16 ; i++ )
	{
		Payload[i] = 0; // Padding
	}
	CTR_Nonce_and_Counter[0] = 0x01;  // L
	for (i=0;i<8;i++)
	{
		CTR_Nonce_and_Counter[i+1] = SourceIEEEAddress[i];
	}
	for (i=0;i<4;i++)
	{
		CTR_Nonce_and_Counter[i+9] = FrameCounter.v[i];
	}
	CTR_Nonce_and_Counter[13] = SECURITY_LEVEL; //Security Mode
	CTR_Nonce_and_Counter[14] = 0x00; // 15 th byte padded to zero
	CTR_Nonce_and_Counter[15] = 0x00; // For MIC calculation All preceding blocks will be incremented by 1


	for (i=0;i<iterations+1;i++)
	{
		for (j=0;j<16;j++)
		{
			block[j] = CTR_Nonce_and_Counter[j];
		}
		PHY_EncryptReq(&block[0], (uint8_t *)mySecurityKey);
		for (j=0;j<16;j++)
		{
			if (CTR_Nonce_and_Counter[15] == 0)
			{
				CTR_mic[j] = block[j]; // CTR_MIC XOR CBC_MIC gives final MIC values
			}
			else
			{
				Payload[j+(i-1)*16] = block[j] ^ Payload[j+(i-1)*16];
			}
		}
		CTR_Nonce_and_Counter[15]++; // Increment Counter for next opration
	}
	*PayloadLen = *PayloadLen-4;
	mic_generator(&Payload[0] , *PayloadLen , FrameControl  , FrameCounter , SourceIEEEAddress);
	for (i=0;i<16;i++)
	{
		final_mic_value[i] = CTR_mic[i] ^ CBC_mic[i];
	}
	return validate_mic();

}
#endif

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
bool MiMAC_PowerState(INPUT uint8_t PowerState)
{
    switch (PowerState)
    {
        case POWER_STATE_DEEP_SLEEP:
        {
            //;clear the WAKE pin in order to allow the device to go to sleep
			PHY_Sleep();
        }
            break;

        case POWER_STATE_OPERATE:
        {
			PHY_Wakeup();
        }
            break;

        default:
            return false;
    }
    return true;
}

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
bool MiMAC_Set(mac_set_params_t id, uint8_t *value)
{
    switch(id)
    {
      case MAC_CHANNEL:
      {
         if(*value > 26) //TODO: this check is necessary here? as we may connect a sub-gig or a 2.4gig?
         {
           return false;
         }
          MACCurrentChannel = *value;
          PHY_SetChannel(MACCurrentChannel);
          return true;
      }
      break;

     default:
     break;
    }
    return false;
}

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
bool MiMAC_Init(MACINIT_PARAM initValue)
{
	uint8_t i;

	PHY_Init();
	MACInitParams = initValue;
	uint16_t x =  PHY_RandomReq();
	// Set RF mode
	PHY_SetRxState(true);
	IEEESeqNum =   x & 0xff;

	// Set Node Address
	PHY_SetIEEEAddr(MACInitParams.PAddress);

	for (i = 0; i < BANK_SIZE; i++)
	{
		RxBuffer[i].PayloadLen = 0;
	}
	#ifdef ENABLE_SECURITY
		#if defined(ENABLE_NETWORK_FREEZER)
			if (initValue.actionFlags.bits.NetworkFreezer)
			{
				PDS_Restore(PDS_OUTGOING_FRAME_COUNTER_ID);
				OutgoingFrameCounter.Val += FRAME_COUNTER_UPDATE_INTERVAL;
				PDS_Store(PDS_OUTGOING_FRAME_COUNTER_ID);
			} else
			{
				OutgoingFrameCounter.Val = 0;
				PDS_Store(PDS_OUTGOING_FRAME_COUNTER_ID);
				OutgoingFrameCounter.Val = 1;
			}
		#else
			OutgoingFrameCounter.Val = 1;
		#endif
	#endif

	return true;
}


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

bool MiMAC_SendPacket( MAC_TRANS_PARAM transParam,
         uint8_t *MACPayload,
         uint8_t MACPayloadLen, uint8_t msghandle,
         DataConf_callback_t ConfCallback)
{
    uint8_t headerLength;
    uint8_t loc = 0;
    uint8_t i = 0;
	uint8_t packet[128];
	uint8_t frameControl = 0;
	#ifndef TARGET_SMALL
		bool IntraPAN;
	#endif

    if (transParam.flags.bits.broadcast)
    {
        transParam.altDestAddr = true;
    }

   #if !defined(PROTOCOL_MESH)
    if (transParam.flags.bits.secEn)
    {
        transParam.altSrcAddr = false;
    }
   #endif

    // set the frame control in variable i
    if (transParam.flags.bits.packetType == PACKET_TYPE_COMMAND)
    {
        frameControl = 0x03;
    } else if (transParam.flags.bits.packetType == PACKET_TYPE_DATA)
    {
        frameControl = 0x01;
    }

    // decide the header length for different addressing mode
#ifndef TARGET_SMALL
    if ((transParam.DestPANID.Val == MAC_PANID.Val) && (MAC_PANID.Val != 0xFFFF)) // this is intraPAN
#endif
    {
        headerLength = 5;
        frameControl |= 0x40;
#ifndef TARGET_SMALL
        IntraPAN = true;
#endif
    }
#ifndef TARGET_SMALL
    else
    {
        headerLength = 7;
        IntraPAN = false;
    }
#endif

    if (transParam.altDestAddr)
    {
        headerLength += 2;
    } else
    {
        headerLength += 8;
    }

    if (transParam.altSrcAddr)
    {
        headerLength += 2;
    } else
    {
        headerLength += 8;
    }

    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false)
    {
        frameControl |= 0x20;
    }

    // use PACKET_TYPE_RESERVE to represent beacon. Fixed format for beacon packet
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE)
    {
        frameControl = 0x00;
        headerLength = 7;
#if !defined(TARGET_SMALL)
        IntraPAN = false;
#endif
        transParam.altSrcAddr = true;
        transParam.flags.bits.ackReq = false;
    }

#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn)
    {
        frameControl |= 0x08;
		DataEncrypt(MACPayload, &MACPayloadLen, OutgoingFrameCounter, frameControl);
    }
#endif

    // set packet length
#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn)
    {
		// 4 byte mic and 5 byte outgoing frame counter
		packet[loc++] = MACPayloadLen+headerLength+9;
    } else
#endif
    {
		packet[loc++] = MACPayloadLen+headerLength;
    }

    // set frame control LSB
	packet[loc++] = frameControl;

    // set frame control MSB
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE)
    {
		packet[loc++] = 0x80;
        // sequence number
		packet[loc++] = IEEESeqNum++;
    } else
    {
        if (transParam.altDestAddr && transParam.altSrcAddr)
        {
			packet[loc++] = 0x88;

        } else if (transParam.altDestAddr && transParam.altSrcAddr == 0)
        {
			packet[loc++] = 0xC8;
        } else if (transParam.altDestAddr == 0 && transParam.altSrcAddr == 1)
        {
			packet[loc++] = 0x8C;
        } else
        {
			packet[loc++] = 0xCC;
        }

        // sequence number
		packet[loc++] = IEEESeqNum++;

        // destination PANID
		packet[loc++] = transParam.DestPANID.v[0];
		packet[loc++] = transParam.DestPANID.v[1];

        // destination address
        if (transParam.flags.bits.broadcast)
        {
			packet[loc++] = 0xFF;
			packet[loc++] = 0xFF;
        } else
        {
            if (transParam.altDestAddr)
            {
				packet[loc++] = transParam.DestAddress[0];
				packet[loc++] = transParam.DestAddress[1];
            } else
            {
                for (i = 0; i < 8; i++)
                {
					packet[loc++] = transParam.DestAddress[i];
                }
            }
        }
    }

#ifndef TARGET_SMALL
    // source PANID if necessary
    if (IntraPAN == false)
    {
		packet[loc++] = MAC_PANID.v[0];
		packet[loc++] = MAC_PANID.v[1];
    }
#endif

    // source address
    if (transParam.altSrcAddr)
    {
		packet[loc++] = myNetworkAddress.v[0];
		packet[loc++] = myNetworkAddress.v[1];
    } else
    {
        for (i = 0; i < 8; i++)
        {
			packet[loc++] = MACInitParams.PAddress[i];
        }
    }
#ifdef ENABLE_SECURITY
if (transParam.flags.bits.secEn)
{
	// fill the additional security aux header
	for (i = 0; i < 4; i++)
	{
		packet[loc++] = OutgoingFrameCounter.v[i];
	}
	OutgoingFrameCounter.Val++;

	#if defined(ENABLE_NETWORK_FREEZER)
	if ((OutgoingFrameCounter.v[0] == 0) && ((OutgoingFrameCounter.v[1] & 0x03) == 0))
	{
		PDS_Store(PDS_OUTGOING_FRAME_COUNTER_ID);
	}
	#endif
	//copy myKeySequenceNumber
	packet[loc++] = myKeySequenceNumber;

}
#endif


#ifndef ENABLE_SECURITY
    // write the payload
    for (i = 0; i < MACPayloadLen; i++)
    {
		packet[loc++] = MACPayload[i];
    }
#else
    // write the payload
    for (i = 0; i < MACPayloadLen; i++) // MIC added
    {
	    packet[loc++] = MACPayload[i];
    }
	packet[loc++] = final_mic_value[0];
	packet[loc++] = final_mic_value[1];
	packet[loc++] = final_mic_value[2];
	packet[loc++] = final_mic_value[3];
	#endif

    // set the trigger value
    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false)
    {
        i = 0x05;
    } else
    {
        i = 0x01;
    }

    dataPointer = MACPayload;
	dataConfCallback = ConfCallback;
    dataHandle = msghandle;

    // Now Trigger the Transmission of packet
    PHY_DataReq(packet);
    return true;
}

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
void MiMAC_DiscardPacket(void)
{
	//re-enable buffer for next packets
	if (BankIndex < BANK_SIZE)
	{
		RxBuffer[BankIndex].PayloadLen = 0;
	}
}

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
bool MiMAC_ReceivedPacket(void)
{
	uint8_t i;

	BankIndex = 0xFF;
	for (i = 0; i < BANK_SIZE; i++)
	{
		if (RxBuffer[i].PayloadLen > 0)
		{
			BankIndex = i;
			break;
		}
	}

	if (BankIndex < BANK_SIZE)
	{
		uint8_t addrMode;
		#ifndef TARGET_SMALL
		bool bIntraPAN = true;

		if ((RxBuffer[BankIndex].Payload[0] & 0x40) == 0)
		{
			bIntraPAN = false;
		}
		#endif
		MACRxPacket.flags.Val = 0;
		MACRxPacket.altSourceAddress = false;

		//Determine the start of the MAC payload
		addrMode = RxBuffer[BankIndex].Payload[1] & 0xCC;
		switch (addrMode)
		{
			case 0xC8: //short dest, long source
			// for P2P only broadcast allows short destination address
			if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
			{
				MACRxPacket.flags.bits.broadcast = 1;
			}
			MACRxPacket.flags.bits.sourcePrsnt = 1;

			#ifndef TARGET_SMALL
			if (bIntraPAN) // check if it is intraPAN
			#endif
			{
				#ifndef TARGET_SMALL
				MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
				MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
				#endif
				MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);

				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
			}
			#ifndef TARGET_SMALL
			else
			{
				MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
				MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
				MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
			}
			#endif

			break;

			case 0xCC: // long dest, long source
			MACRxPacket.flags.bits.sourcePrsnt = 1;
			#ifndef TARGET_SMALL
			if (bIntraPAN) // check if it is intraPAN
			#endif
			{
				//rxFrame.flags.bits.intraPAN = 1;
				#ifndef TARGET_SMALL
				MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
				MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
				#endif
				MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[13]);
				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 25;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[21]);
			}
			#ifndef TARGET_SMALL
			else
			{
				MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[13];
				MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[14];
				MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[15]);
				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 27;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[23]);
			}
			#endif
			break;

			case 0x80: // short source only. used in beacon
			{
				MACRxPacket.flags.bits.broadcast = 1;
				MACRxPacket.flags.bits.sourcePrsnt = 1;
				MACRxPacket.altSourceAddress = true;
				MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
				MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
				MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[5]);
				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 11;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
			}
			break;

			case 0x88: // short dest, short source
			{
				if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
				{
					MACRxPacket.flags.bits.broadcast = 1;
				}
				MACRxPacket.flags.bits.sourcePrsnt = 1;
				MACRxPacket.altSourceAddress = true;
				#ifndef TARGET_SMALL
				if (bIntraPAN == false)
				{
					MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
					MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
					MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
					MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 15;
					MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[11]);
				} else
				#endif
				{
					MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
					MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
					MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);
					MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 13;
					MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[9]);
				}
			}
			break;

			case 0x8C: // long dest, short source
			{
				MACRxPacket.flags.bits.sourcePrsnt = 1;
				MACRxPacket.altSourceAddress = true;
				#ifndef TARGET_SMALL
				if (bIntraPAN) // check if it is intraPAN
				#endif
				{
					#ifndef TARGET_SMALL
					MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
					MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
					#endif
					#if defined(PROTOCOL_MESH)
					MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[13]);
					#else
					MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[12]);
					#endif

					MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
					MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
				}
				#ifndef TARGET_SMALL
				else
				{
					MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[12];
					MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[13];
					MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[14]);
					MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
					MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
				}
				#endif
			}
			break;


			case 0x08: //dest-short, source-none
			{
				if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
				{
					MACRxPacket.flags.bits.broadcast = 1;
				}
				MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 10;
				MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
			}
			break;

			// all other addressing mode will not be supported in P2P
			default:
			// not valid addressing mode or no addressing info
			MiMAC_DiscardPacket();
			return false;
		}

		#ifdef ENABLE_SECURITY
		if (RxBuffer[BankIndex].Payload[0] & 0x08)
		{
			API_UINT32_UNION FrameCounter;

			if (addrMode < 0xC0)
			{
				MiMAC_DiscardPacket();
				return false;
			}

			FrameCounter.v[0] = MACRxPacket.Payload[0];
			FrameCounter.v[1] = MACRxPacket.Payload[1];
			FrameCounter.v[2] = MACRxPacket.Payload[2];
			FrameCounter.v[3] = MACRxPacket.Payload[3];

			for (i = 0; i < CONNECTION_SIZE; i++)
			{
				if ((ConnectionTable[i].status.bits.isValid) &&
				isSameAddress(ConnectionTable[i].Address, MACRxPacket.SourceAddress))
				{
					break;
				}
			}

			if (i < CONNECTION_SIZE)
			{
				if (IncomingFrameCounter[i].Val > FrameCounter.Val)
				{
					MiMAC_DiscardPacket();
					return false;
				} else
				{
					IncomingFrameCounter[i].Val = FrameCounter.Val;
				}
			}


			MACRxPacket.PayloadLen -= 5;  // used to 5 for frame counter now -4 also added for MIC integrity

			received_mic_values[0] = MACRxPacket.Payload[MACRxPacket.PayloadLen+1];
			received_mic_values[1] = MACRxPacket.Payload[MACRxPacket.PayloadLen+2];
			received_mic_values[2] = MACRxPacket.Payload[MACRxPacket.PayloadLen+3];
			received_mic_values[3] = MACRxPacket.Payload[MACRxPacket.PayloadLen+4];
 					//MACRxPacket.PayloadLen -= 4;
			if (false == DataDecrypt(&(MACRxPacket.Payload[5]), &(MACRxPacket.PayloadLen), MACRxPacket.SourceAddress, FrameCounter, RxBuffer[BankIndex].Payload[0]))
			{
				MiMAC_DiscardPacket();
				return false;
			}

			// remove the security header from the payload
			MACRxPacket.Payload = &(MACRxPacket.Payload[5]);
			MACRxPacket.flags.bits.secEn = 1;
		}
		#else
		if (RxBuffer[BankIndex].Payload[0] & 0x08)
		{
			MiMAC_DiscardPacket();
			return false;
		}
		#endif

		// check the frame type. Only the data and command frame type
		// are supported. Acknowledgement frame type is handled in
		// AT96RF233 transceiver hardware.
		switch (RxBuffer[BankIndex].Payload[0] & 0x07) // check frame type
		{
			case 0x01: // data frame
			MACRxPacket.flags.bits.packetType = PACKET_TYPE_DATA;
			break;
			case 0x03: // command frame
			MACRxPacket.flags.bits.packetType = PACKET_TYPE_COMMAND;
			break;
			case 0x00:
			// use reserved packet type to represent beacon packet
			MACRxPacket.flags.bits.packetType = PACKET_TYPE_RESERVE;
			break;
			default: // not support frame type
			MiMAC_DiscardPacket();
			return false;
		}
		#ifndef TARGET_SMALL
		MACRxPacket.LQIValue = RxBuffer[BankIndex].Payload[RxBuffer[BankIndex].PayloadLen - 2];
		MACRxPacket.RSSIValue = RxBuffer[BankIndex].Payload[RxBuffer[BankIndex].PayloadLen - 1];
		#endif

		return true;
	}
	return false;
}
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
uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode)
{
	if( AssessmentMode == CHANNEL_ASSESSMENT_ENERGY_DETECT)
	{
		uint8_t ed;
		ed = PHY_EdReq();
		//printf(" %u ", ed);
		return ed;
	}
	return 0;
}

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
uint32_t MiMAC_SymbolToTicks(uint32_t symbols)
{
	return SYMBOLS_TO_TICKS(symbols);
}

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
uint32_t MiMAC_GetPHYChannelInfo(void)
{
	uint32_t channelMap = FULL_CHANNEL_MAP;
	return channelMap;
}

/************************************************************************************
 * Function:
 *      void PHY_DataConf(uint8_t status)
 *
 * Summary:
 *      This function returns the confirmation of the packet sent
 *
 * PreCondition:
 *      MCU initialization has been done.
 *
 * Parameters:
 *      status -  status of the transmit operation
 *
 * Returns:
 *      None.
 *
 * Remarks:
 *      None
 *
 *****************************************************************************************/
void PHY_DataConf(uint8_t status)
{
	dataStatus = (miwi_status_t)status;
	dataConfAvailable = true;
}

void MiMAC_Task(void)
{
  PHY_TaskHandler();
  if(dataConfCallback && dataConfAvailable)
  {
	  dataConfCallback(dataHandle, dataStatus, dataPointer);
	  dataConfAvailable = false;
  }
}
