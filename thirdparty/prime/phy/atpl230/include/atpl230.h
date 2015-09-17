/**
* \file
*
* \brief ATPL230 Physical layer
*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/

#ifndef ATPL230_H_INCLUDED
#define ATPL230_H_INCLUDED

#include "compiler.h"
#include "atpl230_reg.h"
#include "pplc_if.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup plc_group PLC
 *
 * This module provides configuration and utils for Powerline Communications.
 */

/**
 * \ingroup plc_group
 * \defgroup phy_plc_group PRIME Physical Layer
 *
 * This module provides configuration and utils for the PLC PHY layer interface.
 *
 * @{
 */
//#define PHY_OFFSET_SYMBOL_CONTROL

#define ID_TC_PHY_TX_OFFSET_SYM             ID_TC0
#define TC_PHY_TX_OFFSET_SYM                TC0
#define TC_PHY_TX_OFFSET_SYM_CHN            0
#define TC_PHY_TX_OFFSET_SYM_IRQn           TC0_IRQn
#define TC_PHY_TX_OFFSET_SYM_Handler        TC0_Handler

//! Coupling Boards Identifiers
#define ATPLCOUPXXX_NUM	 9

//! \name Coupling Board Definitions
//@{
#define ATPLCOUP000_v1  0x01
#define ATPLCOUP000_v2  0x02
#define ATPLCOUP001_v1  0x11
#define ATPLCOUP002_v1  0x21
#define ATPLCOUP002_v2  0x22
#define ATPLCOUP003_v1  0x31
#define ATPLCOUP004_v1  0x41
#define ATPLCOUP005_v1  0x51
#define ATPLCOUP006_v1  0x61
//@}

//! \name Phy layer reset Types
//@{
#define PHY_RESET_HARD_TYPE  0
#define PHY_RESET_SOFT_TYPE  1
//@}

//! Maximum physical pdu size
#define PHY_MAX_PPDU_SIZE			512


//! Number of transmission buffers
#define PHY_NUM_TX_BUFFERS			4
//! Number of reception buffers
#define PHY_NUM_RX_BUFFERS			4

//! AES 128 Key size
#define AES_128_KEY_SIZE			16

//! Configuration Identifiers
#define PHY_NUM_CHANNELS			8

//! \name Identifier for channel Configuration
//@{
#define PHY_ID_TX_CHN1				0x000150C7
#define PHY_ID_TX_CHN2				0x00026A44
#define PHY_ID_TX_CHN3				0x000383C1
#define PHY_ID_TX_CHN4				0x00049D3D
#define PHY_ID_TX_CHN5				0x0005B6BA
#define PHY_ID_TX_CHN6				0x0006D036
#define PHY_ID_TX_CHN7				0x0007E9B3
#define PHY_ID_TX_CHN8				0x00090330
//@}

//! \name Commands to access configuration parameters
//@{
//!  Read operation
#define PHY_CMD_CFG_READ		        0
//!  Write operation
#define PHY_CMD_CFG_WRITE		        1
//!  AND operation
#define PHY_CMD_CFG_AND			        2
//!  OR operation
#define PHY_CMD_CFG_OR			        3
//!  XOR operation
#define PHY_CMD_CFG_XOR                 4
//@}

//! \name Impedance states
//@{
//! High Impedance
#define HI_STATE				0
//! Low Impedance
#define LO_STATE				1
//! Very Low Impedance
#define VLO_STATE				2
//! Low Impedance + Peak Cut On
#define LO_STATE_PK				3
//@}

//! \name Header Type
//@{
//! Header type: GENERIC PACKET
#define PHY_HT_GENERIC				0
//! Header type: PROMOTION PACKET
#define PHY_HT_PROMOTION			1
//! Header type: BEACON PACKET
#define PHY_HT_BEACON				2
//@}

//! \name CRC types
enum VCRCTypes
{
	CRC_TYPE_8       = 0,
	CRC_TYPE_16      = 1,
	CRC_TYPE_24      = 2,
	CRC_TYPE_32      = 3,
};

//! \name Protocol values
//@{
//! Modulation scheme of the payload: Differential BPSK
#define PROTOCOL_DBPSK				0x00
//! Modulation scheme of the payload: Differential QPSK
#define PROTOCOL_DQPSK				0x01
//! Modulation scheme of the payload: Differential 8PSK
#define PROTOCOL_D8PSK				0x02
//! Modulation scheme of the payload: Differential BPSK with Convolutional Coding
#define PROTOCOL_DBPSK_VTB			0x04
//! Modulation scheme of the payload: Differential QPSK with Convolutional Coding
#define PROTOCOL_DQPSK_VTB			0x05
//! Modulation scheme of the payload: Differential 8PSK with Convolutional Coding
#define PROTOCOL_D8PSK_VTB			0x06
//! Modulation scheme of the payload: Differential BPSK with ROBO Mode
#define PROTOCOL_DBPSK_ROBO			0x0C
//! Modulation scheme of the payload: Differential QPSK with ROBO Mode
#define PROTOCOL_DQPSK_ROBO			0x0D
//@}

//! Emitter Frecuencies Modes
#define MODE_NUM_EF                             3
//! 10 MHz
#define MODE_EF10                               0
//! 20 MHz
#define MODE_EF20                               1
//! 40 MHz
#define MODE_EF40                               2

//! \name Emitter Driver Mode
//@{
//! INTERNAL DRIVER
#define INTERNAL_DRV_MODE                       1
//! EXTERNAL DRIVER
#define EXTERNAL_DRV_MODE                       2
//@}

//!  \name Driver Identification
//@{
//! DRIVER 1
#define DRIVER_1                                1
//! DRIVER 2
#define DRIVER_2                                2
//! DRIVER 1 + 2
#define DRIVER_1_2                              3
//@}

//! \name Driver Polarity
//@{
//! 0 in emission and 1 in reception
#define DRV_POL_TX_0_RX_1                       0
//! 1 in emission and 0 in reception
#define DRV_POL_TX_1_RX_0                       1
//@}

//! \name Mode values
//@{
//! TYPE A FRAME
#define MODE_TYPE_A                             0x00
//! TYPE B FRAME
#define MODE_TYPE_B                             0x02
//! TYPE BACKWARDS COMPATIBILTY FRAME
#define MODE_TYPE_BC                            0x03
//! Noise
#define MODE_NOISE                              0xFE
//! Test
#define MODE_TEST                               0xFF
//@}

//! \name TX scheduling mode values
//@{
//! Absolute TX scheduling mode (absolute TX time specified)
#define PHY_TX_SCHEDULING_MODE_ABSOLUTE         0
//! Relative TX scheduling mode (delay for TX time specified)
#define PHY_TX_SCHEDULING_MODE_RELATIVE         1
//@}

//! \name TX Result values
//@{
//! Transmission result: already in process
#define PHY_TX_RESULT_PROCESS        ATPL230_TXRXBUF_RESULT_INPROCESS
//! Transmission result: end successfully
#define PHY_TX_RESULT_SUCCESS        ATPL230_TXRXBUF_RESULT_SUCCESSFUL
//! Transmission result: invalid length error
#define PHY_TX_RESULT_INV_LENGTH     ATPL230_TXRXBUF_RESULT_WRONG_LEN
//! Transmission result: busy channel error
#define PHY_TX_RESULT_BUSY_CH        ATPL230_TXRXBUF_RESULT_BUSY_CHANNEL
//! Transmission result: busy transmission error
#define PHY_TX_RESULT_BUSY_TX        ATPL230_TXRXBUF_RESULT_PREV_TX_INPROCESS
//! Transmission result: busy reception error
#define PHY_TX_RESULT_BUSY_RX        ATPL230_TXRXBUF_RESULT_RX_INPROCESS
//! Transmission result: invalid scheme error
#define PHY_TX_RESULT_INV_SCHEME     ATPL230_TXRXBUF_RESULT_INVALID_SCHEME
//! Transmission result: timeout error
#define PHY_TX_RESULT_TIMEOUT        ATPL230_TXRXBUF_RESULT_TIMEOUT
//! Transmission result: invalid buffer identifier error
#define PHY_TX_RESULT_INV_BUFFER     8
//! Transmission result: invalid Prime Mode error
#define PHY_TX_RESULT_INV_PRIME_MODE 9
//@}

//! \name Configuration errors
//@{
//! Set configuration result: success
#define PHY_CFG_SUCCESS                                0
//! Set configuration result: invalid input error or read only
#define PHY_CFG_INVALID_INPUT                          1
//! Set configuration result: read only
#define PHY_CFG_READ_ONLY                              2
//! Set configuration result: invalid channel
#define PHY_CFG_INVALID_CHANNEL                        3
//! Set configuration result: AES not available
#define PHY_CFG_GEN_ERR_INVALID_AES_ENABLE             4
//@}

//! \name Configuration Parameters
//@{
//! Product identifier
#define PHY_ID_INFO_PRODUCT										0x0100
//! Model identifier
#define PHY_ID_INFO_MODEL										0x010A
//! Version identifier
#define PHY_ID_INFO_VERSION										0x010C
//! Buffer identifier of received message
#define PHY_ID_RX_BUFFER_ID										0x0111
//! Flag to enable / disable Rx Quality Report Mode
#define PHY_ID_RX_QR_MODE_ID									0x0112
//! Modulation Scheme of last received message
#define PHY_ID_RX_INFO_SCHEME									0x0113
//! Flag to indicate if header has already been received
#define PHY_ID_RX_INFO_HEADER_RCV								0x0114
//! Payload length in OFDM symbols
#define PHY_ID_RX_INFO_MODE										0x0115
//! Buffer identifier of transmitted message
#define PHY_ID_TX_BUFFER_ID										0x0117
//! Level parameter of last transmitted message
#define PHY_ID_TX_INFO_LEVEL									0x0118
//! Modulation scheme of last transmitted message
#define PHY_ID_TX_INFO_SCHEME									0x0119
//! Flag to enable / disable Tx Quality Report Mode
#define PHY_ID_TX_QR_MODE_ID									0x011A
//! Mode PRIME v1.3, PRIME v1.4 or PRIME v1.4 backward compatible
#define PHY_ID_TX_INFO_MODE										0x011B
//! Flag to enable / disable reception at transmission start
#define PHY_ID_TX_INFO_DISABLE_RX								0x011C
//! RX Payload length in bytes
#define PHY_ID_RX_PAYLOAD_LEN									0x011E
//! RX Payload length in OFDM symbols
#define PHY_ID_RX_PAYLOAD_LEN_SYM								0x0120
//! TX Payload length in OFDM symbols
#define PHY_ID_TX_PAYLOAD_LEN_SYM								0x0122
//! Delay for transmission in 10's of us
#define PHY_ID_TX_INFO_TDELAY									0x0128
//! Transmitted correctly messages count
#define PHY_ID_STATS_TX_TOTAL									0x012C
//! Transmitted bytes count
#define PHY_ID_STATS_TX_TOTAL_BYTES								0x0130
//! Transmission errors count
#define PHY_ID_STATS_TX_TOTAL_ERRORS							0x0134
//! Already in transmission
#define PHY_ID_STATS_TX_BAD_BUSY_TX								0x0138
//! Transmission failure owing to busy channel
#define PHY_ID_STATS_TX_BAD_BUSY_CHANNEL						0x013C
//! Bad len in message (too short - too long)
#define PHY_ID_STATS_TX_BAD_LEN									0x0140
//! Message to transmit in bad format
#define PHY_ID_STATS_TX_BAD_FORMAT								0x0144
//! Timeout error in transmission
#define PHY_ID_STATS_TX_TIMEOUT									0x0148
//! Received correctly messages count
#define PHY_ID_STATS_RX_TOTAL									0x014C
//! Received bytes count
#define PHY_ID_STATS_RX_TOTAL_BYTES								0x0150
//! Reception errors count
#define PHY_ID_STATS_RX_TOTAL_ERRORS							0x0154
//! Bad len in message (too short - too long)
#define PHY_ID_STATS_RX_BAD_LEN									0x0158
//! Bad CRC in received message
#define PHY_ID_STATS_RX_BAD_CRC									0x015C
//! Global attenuation
#define PHY_ID_TX_ATT_GLOBAL									0x0200
//! Channel_1 Attenuation chirp in High impedance
#define PHY_ID_TX1_ATT_CHIRP_HIGHZ								0x0202
//! Channel_1 Attenuation signal in High impedance
#define PHY_ID_TX1_ATT_SIGNAL_HIGHZ								0x0203
//! Channel_1 Attenuation chirp in low impedance
#define PHY_ID_TX1_ATT_CHIRP_LOWZ								0x0204
//! Channel_1 Attenuation signal in low impedance
#define PHY_ID_TX1_ATT_SIGNAL_LOWZ								0x0205
//! Channel_1 Attenuation chirp in very low impedance
#define PHY_ID_TX1_ATT_CHIRP_VLOWZ								0x0206
//! Channel_1 Attenuation signal in very low impedance
#define PHY_ID_TX1_ATT_SIGNAL_VLOWZ								0x0207
//! Channel_1 Threshold for RMS calculated to detect load type
#define PHY_ID_TX1_LOAD_THRESHOLD1								0x0208
//! Channel_1 Threshold for RMS calculated to detect load type
#define PHY_ID_TX1_LOAD_THRESHOLD2								0x020A
//! Channel_1 Threshold for RMS calculated to detect load type
#define PHY_ID_TX1_LOAD_THRESHOLD3								0x020C
//! Channel_1 Threshold for RMS calculated to detect load type
#define PHY_ID_TX1_LOAD_THRESHOLD4								0x020E
//! Channel_2 Attenuation chirp in High impedance
#define PHY_ID_TX2_ATT_CHIRP_HIGHZ								0x0210
//! Channel_2 Attenuation signal in High impedance
#define PHY_ID_TX2_ATT_SIGNAL_HIGHZ								0x0211
//! Channel_2 Attenuation chirp in low impedance
#define PHY_ID_TX2_ATT_CHIRP_LOWZ								0x0212
//! Channel_2 Attenuation signal in low impedance
#define PHY_ID_TX2_ATT_SIGNAL_LOWZ								0x0213
//! Channel_2 Attenuation chirp in very low impedance
#define PHY_ID_TX2_ATT_CHIRP_VLOWZ								0x0214
//! Channel_2 Attenuation signal in very low impedance
#define PHY_ID_TX2_ATT_SIGNAL_VLOWZ								0x0215
//! Channel_2 Threshold for RMS calculated to detect load type
#define PHY_ID_TX2_LOAD_THRESHOLD1								0x0216
//! Channel_2 Threshold for RMS calculated to detect load type
#define PHY_ID_TX2_LOAD_THRESHOLD2								0x0218
//! Channel_2 Threshold for RMS calculated to detect load type
#define PHY_ID_TX2_LOAD_THRESHOLD3								0x021A
//! Channel_2 Threshold for RMS calculated to detect load type
#define PHY_ID_TX2_LOAD_THRESHOLD4								0x021C
//! Channel_3 Attenuation chirp in High impedance
#define PHY_ID_TX3_ATT_CHIRP_HIGHZ								0x021E
//! Channel_3 Attenuation signal in High impedance
#define PHY_ID_TX3_ATT_SIGNAL_HIGHZ								0x021F
//! Channel_3 Attenuation chirp in low impedance
#define PHY_ID_TX3_ATT_CHIRP_LOWZ								0x0220
//! Channel_3 Attenuation signal in low impedance
#define PHY_ID_TX3_ATT_SIGNAL_LOWZ								0x0221
//! Channel_3 Attenuation chirp in very low impedance
#define PHY_ID_TX3_ATT_CHIRP_VLOWZ								0x0222
//! Channel_3 Attenuation signal in very low impedance
#define PHY_ID_TX3_ATT_SIGNAL_VLOWZ								0x0223
//! Channel_3 Threshold for RMS calculated to detect load type
#define PHY_ID_TX3_LOAD_THRESHOLD1								0x0224
//! Channel_3 Threshold for RMS calculated to detect load type
#define PHY_ID_TX3_LOAD_THRESHOLD2								0x0226
//! Channel_3 Threshold for RMS calculated to detect load type
#define PHY_ID_TX3_LOAD_THRESHOLD3								0x0228
//! Channel_3 Threshold for RMS calculated to detect load type
#define PHY_ID_TX3_LOAD_THRESHOLD4								0x022A
//! Channel_4 Attenuation chirp in High impedance
#define PHY_ID_TX4_ATT_CHIRP_HIGHZ								0x022C
//! Channel_4 Attenuation signal in High impedance
#define PHY_ID_TX4_ATT_SIGNAL_HIGHZ								0x022D
//! Channel_4 Attenuation chirp in low impedance
#define PHY_ID_TX4_ATT_CHIRP_LOWZ								0x022E
//! Channel_4 Attenuation signal in low impedance
#define PHY_ID_TX4_ATT_SIGNAL_LOWZ								0x022F
//! Channel_4 Attenuation chirp in very low impedance
#define PHY_ID_TX4_ATT_CHIRP_VLOWZ								0x0230
//! Channel_4 Attenuation signal in very low impedance
#define PHY_ID_TX4_ATT_SIGNAL_VLOWZ								0x0231
//! Channel_4 Threshold for RMS calculated to detect load type
#define PHY_ID_TX4_LOAD_THRESHOLD1								0x0232
//! Channel_4 Threshold for RMS calculated to detect load type
#define PHY_ID_TX4_LOAD_THRESHOLD2								0x0234
//! Channel_4 Threshold for RMS calculated to detect load type
#define PHY_ID_TX4_LOAD_THRESHOLD3								0x0236
//! Channel_4 Threshold for RMS calculated to detect load type
#define PHY_ID_TX4_LOAD_THRESHOLD4								0x0238
//! Channel_5 Attenuation chirp in High impedance
#define PHY_ID_TX5_ATT_CHIRP_HIGHZ								0x023A
//! Channel_5 Attenuation signal in High impedance
#define PHY_ID_TX5_ATT_SIGNAL_HIGHZ								0x023B
//! Channel_5 Attenuation chirp in low impedance
#define PHY_ID_TX5_ATT_CHIRP_LOWZ								0x023C
//! Channel_5 Attenuation signal in low impedance
#define PHY_ID_TX5_ATT_SIGNAL_LOWZ								0x023D
//! Channel_5 Attenuation chirp in very low impedance
#define PHY_ID_TX5_ATT_CHIRP_VLOWZ								0x023E
//! Channel_5 Attenuation signal in very low impedance
#define PHY_ID_TX5_ATT_SIGNAL_VLOWZ								0x023F
//! Channel_5 Threshold for RMS calculated to detect load type
#define PHY_ID_TX5_LOAD_THRESHOLD1								0x0240
//! Channel_5 Threshold for RMS calculated to detect load type
#define PHY_ID_TX5_LOAD_THRESHOLD2								0x0242
//! Channel_5 Threshold for RMS calculated to detect load type
#define PHY_ID_TX5_LOAD_THRESHOLD3								0x0244
//! Channel_5 Threshold for RMS calculated to detect load type
#define PHY_ID_TX5_LOAD_THRESHOLD4								0x0246
//! Channel_6 Attenuation chirp in High impedance
#define PHY_ID_TX6_ATT_CHIRP_HIGHZ								0x0248
//! Channel_6 Attenuation signal in High impedance
#define PHY_ID_TX6_ATT_SIGNAL_HIGHZ								0x0249
//! Channel_6 Attenuation chirp in low impedance
#define PHY_ID_TX6_ATT_CHIRP_LOWZ								0x024A
//! Channel_6 Attenuation signal in low impedance
#define PHY_ID_TX6_ATT_SIGNAL_LOWZ								0x024B
//! Channel_6 Attenuation chirp in very low impedance
#define PHY_ID_TX6_ATT_CHIRP_VLOWZ								0x024C
//! Channel_6 Attenuation signal in very low impedance
#define PHY_ID_TX6_ATT_SIGNAL_VLOWZ								0x024D
//! Channel_6 Threshold for RMS calculated to detect load type
#define PHY_ID_TX6_LOAD_THRESHOLD1								0x024E
//! Channel_6 Threshold for RMS calculated to detect load type
#define PHY_ID_TX6_LOAD_THRESHOLD2								0x0250
//! Channel_6 Threshold for RMS calculated to detect load type
#define PHY_ID_TX6_LOAD_THRESHOLD3								0x0252
//! Channel_6 Threshold for RMS calculated to detect load type
#define PHY_ID_TX6_LOAD_THRESHOLD4								0x0254
//! Channel_7 Attenuation chirp in High impedance
#define PHY_ID_TX7_ATT_CHIRP_HIGHZ								0x0256
//! Channel_7 Attenuation signal in High impedance
#define PHY_ID_TX7_ATT_SIGNAL_HIGHZ								0x0257
//! Channel_7 Attenuation chirp in low impedance
#define PHY_ID_TX7_ATT_CHIRP_LOWZ                      0x0258
//! Channel_7 Attenuation signal in low impedance
#define PHY_ID_TX7_ATT_SIGNAL_LOWZ								0x0259
//! Channel_7 Attenuation chirp in very low impedance
#define PHY_ID_TX7_ATT_CHIRP_VLOWZ								0x025A
//! Channel_7 Attenuation signal in very low impedance
#define PHY_ID_TX7_ATT_SIGNAL_VLOWZ								0x025B
//! Channel_7 Threshold for RMS calculated to detect load type
#define PHY_ID_TX7_LOAD_THRESHOLD1								0x025C
//! Channel_7 Threshold for RMS calculated to detect load type
#define PHY_ID_TX7_LOAD_THRESHOLD2								0x025E
//! Channel_7 Threshold for RMS calculated to detect load type
#define PHY_ID_TX7_LOAD_THRESHOLD3								0x0260
//! Channel_7 Threshold for RMS calculated to detect load type
#define PHY_ID_TX7_LOAD_THRESHOLD4								0x0262
//! Channel_8 Attenuation chirp in High impedance
#define PHY_ID_TX8_ATT_CHIRP_HIGHZ								0x0264
//! Channel_8 Attenuation signal in High impedance
#define PHY_ID_TX8_ATT_SIGNAL_HIGHZ								0x0265
//! Channel_8 Attenuation chirp in low impedance
#define PHY_ID_TX8_ATT_CHIRP_LOWZ								0x0266
//! Channel_8 Attenuation signal in low impedance
#define PHY_ID_TX8_ATT_SIGNAL_LOWZ								0x0267
//! Channel_8 Attenuation chirp in very low impedance
#define PHY_ID_TX8_ATT_CHIRP_VLOWZ								0x0268
//! Channel_8 Attenuation signal in very low impedance
#define PHY_ID_TX8_ATT_SIGNAL_VLOWZ								0x0269
//! Channel_8 Threshold for RMS calculated to detect load type
#define PHY_ID_TX8_LOAD_THRESHOLD1								0x026A
//! Channel_8 Threshold for RMS calculated to detect load type
#define PHY_ID_TX8_LOAD_THRESHOLD2								0x026C
//! Channel_8 Threshold for RMS calculated to detect load type
#define PHY_ID_TX8_LOAD_THRESHOLD3								0x026E
//! Channel_8 Threshold for RMS calculated to detect load type
#define PHY_ID_TX8_LOAD_THRESHOLD4								0x0270
//! Flag to indicate if driver 1 is extern driver (1) or intern driver (0)
#define PHY_ID_CFG_DRIVER1_MODE								0x0400
//! Flag to indicate if driver 2 is extern driver (1) or intern driver (0)
#define PHY_ID_CFG_DRIVER2_MODE								0x0401
//! Flag to indicate if txrx1 polarity is high or low active
#define PHY_ID_CFG_TXRX1_POLARITY							0x0402
//! Flag to indicate if txrx2 polarity is high or low active
#define PHY_ID_CFG_TXRX2_POLARITY							0x0403
//! Flag to enable branch auto detection
#define PHY_ID_CFG_AUTODETECT_BRANCH							0x0404
//! When branch auto detection disabled, indicate impedance to use
#define PHY_ID_CFG_IMPEDANCE								0x0405
//! Select driver for high impedance
#define PHY_ID_CFG_HIGH_Z_DRIVER							0x0406
//! Select driver for high impedance
#define PHY_ID_CFG_LOW_Z_DRIVER								0x0407
//! Select driver for high impedance
#define PHY_ID_CFG_VLOW_Z_DRIVER        						0x0408
//! Offset for received signal strength (rssi) according to AGC 0 configuration
#define PHY_ID_CFG_AGC0_KRSSI_OFFSET							0x0409
//! Offset for received signal strength (rssi) according to AGC 1 configuration
#define PHY_ID_CFG_AGC1_KRSSI_OFFSET							0x040A
//! Offset for received signal strength (rssi) according to AGC 2 configuration
#define PHY_ID_CFG_AGC2_KRSSI_OFFSET							0x040B
//! Offset for received signal strength (rssi) according to AGC 3 configuration
#define PHY_ID_CFG_AGC3_KRSSI_OFFSET							0x040C
//! N1 Delay
#define PHY_ID_CFG_N1_DELAY								0x040D
//! P1 Delay
#define PHY_ID_CFG_P1_DELAY								0x040E
//! N2 delay
#define PHY_ID_CFG_N2_DELAY								0x040F
//! P2 delay
#define PHY_ID_CFG_P2_DELAY								0x0410
//! Emit 1 Active
#define PHY_ID_CFG_EMIT1_ACTIVE							        0x0411
//! Emit 2 Active
#define PHY_ID_CFG_EMIT2_ACTIVE								0x0412
//! Emit 3 Active
#define PHY_ID_CFG_EMIT3_ACTIVE								0x0413
//! Emit 4 Active
#define PHY_ID_CFG_EMIT4_ACTIVE								0x0414
//! Transmission/Reception Channel
#define PHY_ID_CFG_TXRX_CHANNEL								0x0415
//! Coupling board in use
#define PHY_ID_CFG_COUPLING_BOARD							0x0416
//! PRIME mode (see mode values in atpl230.h)
#define PHY_ID_CFG_PRIME_MODE								0x0417
//! Time in 10's of us for HIMP pin before transmission with high impedance
#define PHY_ID_CFG_TIME_BEFORE_TX_HIGHZ							0x0418
//! Time in 10's of us for TXRX pin before transmission with low impedance
#define PHY_ID_CFG_TIME_BEFORE_TX_LOWZ							0x041A
//! Time (unit depends on platform) for HIMP pin after transmission with high impedance
#define PHY_ID_CFG_TIME_AFTER_TX_HIGHZ							0x041C
//! Time (unit depends on platform) for TXRX pin after transmission with low impedance
#define PHY_ID_CFG_TIME_AFTER_TX_LOWZ							0x041E
//! Threshold for autocorrelation filter
#define PHY_ID_CFG_RX_CORR_THRESHOLD                   0x0420
//! Flag to enable Peak Cut On in case of low impedance
#define PHY_ID_CFG_ENABLE_VLOW_PK                      0x0422
//@}

typedef struct
{
	char prodId[10];          ///< (r) Product identifier
	uint16_t model;           ///< (r) Model number
	uint32_t version;         ///< (r) Version number

	uint8_t reserved0;        ///< Reserved

	uint8_t rxIdBuff;         ///< (r) Buffer identifier of received message
	uint8_t rxQRMode;         ///< (w/r) Flag to enable / disable Rx Quality Report Mode
	uint8_t rxScheme;         ///< (r) Modulation Scheme of last received message
	uint8_t rxHdrRcv;         ///< (r) Flag to indicate if header has already been received
	uint8_t rxMode;           ///< (w/r) Mode PRIME v1.3 or PRIME v1.4
	uint8_t reserved1;        ///< Reserved

	uint8_t txIdBuff;         ///< (w/r) Buffer identifier of transmitted message
	uint8_t txLevel;          ///< (w/r) Level parameter of last transmitted message
	uint8_t txScheme;         ///< (w/r) Modulation scheme of last transmitted message
	uint8_t txQRMode;         ///< (w/r) Flag to enable / disable Tx Quality Report Mode
	uint8_t txMode;           ///< (w/r) Mode PRIME v1.3 or PRIME v1.4
	uint8_t txDisableRx;      ///< (w/r) Flag to enable / disable reception at transmission start
	uint8_t reserved2;        ///< Reserved

	uint16_t rxPayloadLen;    ///< (r) RX: Payload length in bytes
	uint16_t rxPayloadLenSym; ///< (r) RX: Payload length in OFDM symbols
	uint16_t txPayloadLenSym; ///< (r) TX: Payload length in OFDM symbols
	uint16_t reserved3;       ///< Reserved

	uint32_t txTdelay;        ///< (w/r) Delay for transmission in 10's of us

	uint32_t txTotal;         ///< (r) Transmitted correctly messages count
	uint32_t txTotalBytes;    ///< (r) Transmitted bytes count
	uint32_t txTotalErrors;   ///< (r) Transmission errors count
	uint32_t txBadBusyTx;     ///< (r) Already in transmission
	uint32_t txBadBusyChannel;///< (r) Transmission failure owing to busy channel
	uint32_t txBadLen;        ///< (r) Bad len in message (too short - too long)
	uint32_t txBadFormat;     ///< (r) Message to transmit in bad format
	uint32_t txTimeout;       ///< (r) Timeout error in transmission
	uint32_t rxTotal;         ///< (r) Received correctly messages count
	uint32_t rxTotalBytes;    ///< (r) Received bytes count
	uint32_t rxTotalErrors;   ///< (r) Reception errors count
	uint32_t rxBadLen;        ///< (r) Bad len in message (too short - too long)
	uint32_t rxBadCrc;        ///< (r) Bad CRC in received message (only in Prime v1.3)
} atpl230_t;

typedef struct
{
	uint8_t txAttGlobal;					///< (w/r)	Global attenuation
	uint8_t reserved;						///<		Reserved

	uint8_t tx1AttChirpHighZ; 				///< (w/r)	Channel 1 Attenuation chirp in High impedance
	uint8_t tx1AttSignalHighZ; 				///< (w/r)	Channel 1 Attenuation signal in High impedance
	uint8_t tx1AttChirpLowZ; 				///< (w/r)	Channel 1 Attenuation chirp in low impedance
	uint8_t tx1AttSignalLowZ; 				///< (w/r)	Channel 1 Attenuation signal in low impedance
	uint8_t tx1AttChirpVLowZ; 				///< (w/r)	Channel 1 Attenuation chirp in very low impedance
	uint8_t tx1AttSignalVLowZ;				///< (w/r)	Channel 1 Attenuation signal in very low impedance
	uint16_t tx1LoadThreshold1;				///< (w/r)	Channel 1 Threshold for RMS calculated to detect load type
	uint16_t tx1LoadThreshold2;				///< (w/r)	Channel 1 Threshold for RMS calculated to detect load type
	uint16_t tx1LoadThreshold3;				///< (w/r)	Channel 1 Threshold for RMS calculated to detect load type
	uint16_t tx1LoadThreshold4;				///< (w/r)	Channel 1 Threshold for RMS calculated to detect load type

	uint8_t tx2AttChirpHighZ; 				///< (w/r)	Channel 2 Attenuation chirp in High impedance
	uint8_t tx2AttSignalHighZ; 				///< (w/r)	Channel 2 Attenuation signal in High impedance
	uint8_t tx2AttChirpLowZ; 				///< (w/r)	Channel 2 Attenuation chirp in low impedance
	uint8_t tx2AttSignalLowZ; 				///< (w/r)	Channel 2 Attenuation signal in low impedance
	uint8_t tx2AttChirpVLowZ; 				///< (w/r)	Channel 2 Attenuation chirp in very low impedance
	uint8_t tx2AttSignalVLowZ;				///< (w/r)	Channel 2 Attenuation signal in very low impedance
	uint16_t tx2LoadThreshold1;				///< (w/r)	Channel 2 Threshold for RMS calculated to detect load type
	uint16_t tx2LoadThreshold2;				///< (w/r)	Channel 2 Threshold for RMS calculated to detect load type
	uint16_t tx2LoadThreshold3;				///< (w/r)	Channel 2 Threshold for RMS calculated to detect load type
	uint16_t tx2LoadThreshold4;				///< (w/r)	Channel 2 Threshold for RMS calculated to detect load type

	uint8_t tx3AttChirpHighZ; 				///< (w/r)	Channel 3 Attenuation chirp in High impedance
	uint8_t tx3AttSignalHighZ; 				///< (w/r)	Channel 3 Attenuation signal in High impedance
	uint8_t tx3AttChirpLowZ; 				///< (w/r)	Channel 3 Attenuation chirp in low impedance
	uint8_t tx3AttSignalLowZ; 				///< (w/r)	Channel 3 Attenuation signal in low impedance
	uint8_t tx3AttChirpVLowZ; 				///< (w/r)	Channel 3 Attenuation chirp in very low impedance
	uint8_t tx3AttSignalVLowZ;				///< (w/r)	Channel 3 Attenuation signal in very low impedance
	uint16_t tx3LoadThreshold1;				///< (w/r)	Channel 3 Threshold for RMS calculated to detect load type
	uint16_t tx3LoadThreshold2;				///< (w/r)	Channel 3 Threshold for RMS calculated to detect load type
	uint16_t tx3LoadThreshold3;				///< (w/r)	Channel 3 Threshold for RMS calculated to detect load type
	uint16_t tx3LoadThreshold4;				///< (w/r)	Channel 3 Threshold for RMS calculated to detect load type

	uint8_t tx4AttChirpHighZ; 				///< (w/r)	Channel 4 Attenuation chirp in High impedance
	uint8_t tx4AttSignalHighZ; 				///< (w/r)	Channel 4 Attenuation signal in High impedance
	uint8_t tx4AttChirpLowZ; 				///< (w/r)	Channel 4 Attenuation chirp in low impedance
	uint8_t tx4AttSignalLowZ; 				///< (w/r)	Channel 4 Attenuation signal in low impedance
	uint8_t tx4AttChirpVLowZ; 				///< (w/r)	Channel 4 Attenuation chirp in very low impedance
	uint8_t tx4AttSignalVLowZ;				///< (w/r)	Channel 4 Attenuation signal in very low impedance
	uint16_t tx4LoadThreshold1;				///< (w/r)	Channel 4 Threshold for RMS calculated to detect load type
	uint16_t tx4LoadThreshold2;				///< (w/r)	Channel 4 Threshold for RMS calculated to detect load type
	uint16_t tx4LoadThreshold3;				///< (w/r)	Channel 4 Threshold for RMS calculated to detect load type
	uint16_t tx4LoadThreshold4;				///< (w/r)	Channel 4 Threshold for RMS calculated to detect load type

	uint8_t tx5AttChirpHighZ; 				///< (w/r)	Channel 5 Attenuation chirp in High impedance
	uint8_t tx5AttSignalHighZ; 				///< (w/r)	Channel 5 Attenuation signal in High impedance
	uint8_t tx5AttChirpLowZ; 				///< (w/r)	Channel 5 Attenuation chirp in low impedance
	uint8_t tx5AttSignalLowZ; 				///< (w/r)	Channel 5 Attenuation signal in low impedance
	uint8_t tx5AttChirpVLowZ; 				///< (w/r)	Channel 5 Attenuation chirp in very low impedance
	uint8_t tx5AttSignalVLowZ;				///< (w/r)	Channel 5 Attenuation signal in very low impedance
	uint16_t tx5LoadThreshold1;				///< (w/r)	Channel 5 Threshold for RMS calculated to detect load type
	uint16_t tx5LoadThreshold2;				///< (w/r)	Channel 5 Threshold for RMS calculated to detect load type
	uint16_t tx5LoadThreshold3;				///< (w/r)	Channel 5 Threshold for RMS calculated to detect load type
	uint16_t tx5LoadThreshold4;				///< (w/r)	Channel 5 Threshold for RMS calculated to detect load type

	uint8_t tx6AttChirpHighZ; 				///< (w/r)	Channel 6 Attenuation chirp in High impedance
	uint8_t tx6AttSignalHighZ; 				///< (w/r)	Channel 6 Attenuation signal in High impedance
	uint8_t tx6AttChirpLowZ; 				///< (w/r)	Channel 6 Attenuation chirp in low impedance
	uint8_t tx6AttSignalLowZ; 				///< (w/r)	Channel 6 Attenuation signal in low impedance
	uint8_t tx6AttChirpVLowZ; 				///< (w/r)	Channel 6 Attenuation chirp in very low impedance
	uint8_t tx6AttSignalVLowZ;				///< (w/r)	Channel 6 Attenuation signal in very low impedance
	uint16_t tx6LoadThreshold1;				///< (w/r)	Channel 6 Threshold for RMS calculated to detect load type
	uint16_t tx6LoadThreshold2;				///< (w/r)	Channel 6 Threshold for RMS calculated to detect load type
	uint16_t tx6LoadThreshold3;				///< (w/r)	Channel 6 Threshold for RMS calculated to detect load type
	uint16_t tx6LoadThreshold4;				///< (w/r)	Channel 6 Threshold for RMS calculated to detect load type

	uint8_t tx7AttChirpHighZ; 				///< (w/r)	Channel 7 Attenuation chirp in High impedance
	uint8_t tx7AttSignalHighZ; 				///< (w/r)	Channel 7 Attenuation signal in High impedance
	uint8_t tx7AttChirpLowZ; 				///< (w/r)	Channel 7 Attenuation chirp in low impedance
	uint8_t tx7AttSignalLowZ; 				///< (w/r)	Channel 7 Attenuation signal in low impedance
	uint8_t tx7AttChirpVLowZ; 				///< (w/r)	Channel 7 Attenuation chirp in very low impedance
	uint8_t tx7AttSignalVLowZ;				///< (w/r)	Channel 7 Attenuation signal in very low impedance
	uint16_t tx7LoadThreshold1;				///< (w/r)	Channel 7 Threshold for RMS calculated to detect load type
	uint16_t tx7LoadThreshold2;				///< (w/r)	Channel 7 Threshold for RMS calculated to detect load type
	uint16_t tx7LoadThreshold3;				///< (w/r)	Channel 7 Threshold for RMS calculated to detect load type
	uint16_t tx7LoadThreshold4;				///< (w/r)	Channel 7 Threshold for RMS calculated to detect load type

	uint8_t tx8AttChirpHighZ; 				///< (w/r)	Channel 8 Attenuation chirp in High impedance
	uint8_t tx8AttSignalHighZ; 				///< (w/r)	Channel 8 Attenuation signal in High impedance
	uint8_t tx8AttChirpLowZ; 				///< (w/r)	Channel 8 Attenuation chirp in low impedance
	uint8_t tx8AttSignalLowZ; 				///< (w/r)	Channel 8 Attenuation signal in low impedance
	uint8_t tx8AttChirpVLowZ; 				///< (w/r)	Channel 8 Attenuation chirp in very low impedance
	uint8_t tx8AttSignalVLowZ;				///< (w/r)	Channel 8 Attenuation signal in very low impedance
	uint16_t tx8LoadThreshold1;				///< (w/r)	Channel 8 Threshold for RMS calculated to detect load type
	uint16_t tx8LoadThreshold2;				///< (w/r)	Channel 8 Threshold for RMS calculated to detect load type
	uint16_t tx8LoadThreshold3;				///< (w/r)	Channel 8 Threshold for RMS calculated to detect load type
	uint16_t tx8LoadThreshold4;				///< (w/r)	Channel 8 Threshold for RMS calculated to detect load type

} atpl230ChnCfg_t;

typedef struct
{
	uint8_t uc_buff_id;					///<	Buffer identifier
	uint8_t att_level;					///<	Attenuation level with which the message must be transmitted
	uint8_t scheme;						///<	Modulation scheme of last transmitted message
	uint8_t disable_rx;					///<	TX Forced
	uint8_t mode;						///<	Mode PRIME v1.3 or PRIME PLUS
	uint16_t data_len;					///<	Length of the data buffer.
	uint8_t *data_buf;					///<	Pointer to data buffer
	uint32_t tdelay;					///<	Delay for transmission in 10's of us
	uint8_t tmode;						///<	Time mode: 0: Absolute mode, 1: Differential mode
} xPhyMsgTx_t;

typedef struct
{
	uint8_t uc_buff_id;           ///< Buffer identifier
	uint8_t scheme;               ///< Modulation scheme of the last received message
	uint8_t mode;                 ///< Mode PRIME v1.3, PRIME PLUS or Noise Capture
	uint8_t header_type;          ///< Header Type of the last received message
	uint8_t noise_result;         ///< Noise result in case of noise capture mode
	uint8_t rssi_avg;             ///< Average RSSI (Received Signal Strength Indication) (valid only when rxQRMode is enable)
	uint8_t cinr_avg;             ///< Average CNIR (Carrier to Interference + Noise ratio) (valid only when rxQRMode is enable)
	uint16_t evm_header;          ///< Error Vector Magnitude for header (valid only when rxQRMode is enable)
	uint16_t evm_payload;         ///< Error Vector Magnitude for payload (valid only when rxQRMode is enable)
	uint16_t data_len;            ///< Length of the data buffer.
	uint32_t evm_header_acum;     ///< Accumulated Error Vector Magnitude for header (valid only when rxQRMode is enable)
	uint32_t evm_payload_acum;    ///< Accumulated Error Vector Magnitude for payload (valid only when rxQRMode is enable)
	uint8_t *data_buf;            ///< Pointer to data buffer
	uint32_t rx_time;             ///< Reception time in 10us
} xPhyMsgRx_t;

typedef struct
{
	uint8_t uc_id_buffer;				///<	Number of the buffer used to tx
	uint8_t uc_result;					///<	Result (see atpl230reg.h "Values of TXRXBUF_RESULT_TX register" )
	uint16_t rms_calc;					///<	RMS value emitted (valid only when txQRMode is enable)
} xPhyMsgTxResult_t;


//! \name Macros
//@{
#define ATPL230_REG_PARAM(val)                  (val & 0xF000)
#define ATPL230_CFG_PARAM(val)                  (val & 0xF400)
#define ATPL230_ATT_PARAM(val)                  (val & 0xF200)
#define ATPL230_PARAM(val)                      (val & 0xF100)

#define ATPL230_REG_PARAM_MSK                   0xF000
#define ATPL230_CFG_PARAM_MSK                   0x0400
#define ATPL230_ATT_PARAM_MSK                   0x0200
#define ATPL230_PARAM_MSK                       0x0100

#define ATPL230_GET_HEADER_TYPE(val)            ((val >> 4) & 0x03)

#define ATPL230_GET_SFR_BCH_ERR(val)            (val & ATPL230_SFR_BCH_ERR_Msk)
#define ATPL230_GET_SFR_ERR_PYL(val)            (val & ATPL230_SFR_ERR_PYL_Msk)
#define ATPL230_GET_SFR_CD(val)                 (val & ATPL230_SFR_CD_Msk)
#define ATPL230_GET_SFR_UMD(val)                (val & ATPL230_SFR_UMD_Msk)

#define ATPL230_GET_TXINT_TX0(val)              (val & ATPL230_TXRXBUF_TX_INT_TX0_Msk)
#define ATPL230_GET_TXINT_TX1(val)              (val & ATPL230_TXRXBUF_TX_INT_TX1_Msk)
#define ATPL230_GET_TXINT_TX2(val)              (val & ATPL230_TXRXBUF_TX_INT_TX2_Msk)
#define ATPL230_GET_TXINT_TX3(val)              (val & ATPL230_TXRXBUF_TX_INT_TX3_Msk)

#define ATPL230_GET_RXINT_PRX0(val)             (val & ATPL230_TXRXBUF_RX_INT_PRX0_Msk)
#define ATPL230_GET_RXINT_PRX1(val)             (val & ATPL230_TXRXBUF_RX_INT_PRX1_Msk)
#define ATPL230_GET_RXINT_PRX2(val)             (val & ATPL230_TXRXBUF_RX_INT_PRX2_Msk)
#define ATPL230_GET_RXINT_PRX3(val)             (val & ATPL230_TXRXBUF_RX_INT_PRX3_Msk)

#define ATPL230_GET_RXINT_HRX0(val)             (val & ATPL230_TXRXBUF_RX_INT_HRX0_Msk)
#define ATPL230_GET_RXINT_HRX1(val)             (val & ATPL230_TXRXBUF_RX_INT_HRX1_Msk)
#define ATPL230_GET_RXINT_HRX2(val)             (val & ATPL230_TXRXBUF_RX_INT_HRX2_Msk)
#define ATPL230_GET_RXINT_HRX3(val)             (val & ATPL230_TXRXBUF_RX_INT_HRX3_Msk)

#define ATPL230_GET_TX_RESULT_TX0(val)          ((val & ATPL230_TXRXBUF_RESULT_TX0_Msk)>>ATPL230_TXRXBUF_RESULT_TX0_Pos)
#define ATPL230_GET_TX_RESULT_TX1(val)          ((val & ATPL230_TXRXBUF_RESULT_TX1_Msk)>>ATPL230_TXRXBUF_RESULT_TX1_Pos)
#define ATPL230_GET_TX_RESULT_TX2(val)          ((val & ATPL230_TXRXBUF_RESULT_TX2_Msk)>>ATPL230_TXRXBUF_RESULT_TX2_Pos)
#define ATPL230_GET_TX_RESULT_TX3(val)          ((val & ATPL230_TXRXBUF_RESULT_TX3_Msk)>>ATPL230_TXRXBUF_RESULT_TX3_Pos)

#define ATPL230_GET_ROBO_MODE_RX(id, val)       (val >> (id<<1)) & ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0_Msk;

#define ATPL230_GET_NOISE_RESULT(val)           ((val & ATPL230_TXRXBUF_NOISECONF_ETN_Msk)>>ATPL230_TXRXBUF_NOISECONF_ETN_Pos)
#define ATPL230_GET_NOISE_NS(val)               (val & ATPL230_TXRXBUF_NOISECONF_NS_Msk)
//@}

//! \name Serialization Addons
//@{
#define DISABLE_SERIAL         0
#define SERIAL_IF_ENABLE       0x1
#define SNIFFER_IF_ENABLE      0x2
//@}

//! Valid chip configuration key
#define ATPL230_VALID_CFG_KEY  0xBA

/**
 * \brief Clear global interruption
 *
 */
static inline void phy_clear_global_interrupt(void)
{
	pplc_if_or8(REG_ATPL230_PHY_SFR, ATPL230_SFR_PHY_INT_Msk);
}

/**
 * \brief Get MAC coproc
 *
 */
static inline uint8_t phy_get_mac_en(void)
{
	return (pplc_if_read8(REG_ATPL230_PHY_CONFIG) & ATPL230_MAC_EN_Msk);
}

/**
 * \brief Get PHY SFR -1 flag
 *
 */
static inline uint8_t phy_get_sfr_err(void)
{
	return (pplc_if_read8(REG_ATPL230_PHY_SFR) & ATPL230_SFR_ERR_PYL_Msk);
}

/**
 * \brief Clear PHY SFR -1
 *
 */
static inline void phy_clear_sfr_err(void)
{
	pplc_if_and8(REG_ATPL230_PHY_SFR, (uint8_t)~ATPL230_SFR_ERR_PYL_Msk);
}

/**
 * \brief Enable MAC CRC processing
 *
 */
static inline void phy_mac_crc_enable(void)
{
	pplc_if_or8(REG_ATPL230_PHY_CONFIG, (uint8_t)ATPL230_MAC_EN_Msk);
}

/**
 * \brief Disable MAC CRC processing
 *
 */
static inline void phy_mac_crc_disable(void)
{
	pplc_if_and8(REG_ATPL230_PHY_CONFIG, (uint8_t)~ATPL230_MAC_EN_Msk);
}

/**
 * \brief Get Carrier Detect
 *
 */
static inline uint8_t phy_get_carrier_detect(void)
{
	return (pplc_if_read8(REG_ATPL230_PHY_SFR) & ATPL230_SFR_CD_Msk);
}

/**
 * \brief Enable Carrier Detect
 *
 */
static inline void phy_carrier_detect_enable(void)
{
	pplc_if_or8(REG_ATPL230_PHY_SFR, (uint8_t) ATPL230_SFR_CD_Msk);
}

/**
 * \brief Disable Carrier Detect
 *
 */
static inline void phy_carrier_detect_disable(void)
{
	pplc_if_and8(REG_ATPL230_PHY_SFR, (uint8_t)~ATPL230_SFR_CD_Msk);
}

/**
 * \brief Enable forced transmission in specific buffer
 *
 */
static inline void phy_force_tx_buff_enable(uint8_t idBuf)
{
	pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t) ATPL230_TXRXBUF_TXCONF_FE_Msk);
}

/**
 * \brief Disable forced transmission in specific buffer
 *
 */
static inline void phy_force_tx_buff_disable(uint8_t idBuf)
{
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t)~ATPL230_TXRXBUF_TXCONF_FE_Msk);
}

/**
 * \brief Disable carrier detect in specific buffer
 *
 */
static inline void phy_carrier_detect_buff_disable(uint8_t idBuf)
{
	pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t) ATPL230_TXRXBUF_TXCONF_DC_Msk);
}

/**
 * \brief Enable carrier detect in specific buffer
 *
 */
static inline void phy_carrier_detect_buff_enable(uint8_t idBuf)
{
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t)~ATPL230_TXRXBUF_TXCONF_DC_Msk);
}


/**
 * \brief Disable reception in specific buffer
 *
 */
static inline void phy_reception_buff_disable(uint8_t idBuf)
{
	  pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t) ATPL230_TXRXBUF_TXCONF_DR_Msk);
	}

/**
 * \brief Enable reception in specific buffer
 *
 */
static inline void phy_reception_buff_enable(uint8_t idBuf)
{
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t)~ATPL230_TXRXBUF_TXCONF_DR_Msk);
}

/**
 * \brief Disable transmission in specific buffer
 *
 */
static inline void phy_transmission_buff_disable(uint8_t idBuf)
{
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf, (uint8_t)~ATPL230_TXRXBUF_TXCONF_EB_Msk);
}



/**
 * \brief Get status of specific buffer
 *
 * \retval true if buffer is enabled
 * \retval false if buffer is disabled
 */
static inline uint8_t phy_transmission_buff_is_enable(uint8_t idBuf)
{
	uint8_t uc_enable;

	uc_enable = pplc_if_read8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + idBuf);
	uc_enable &= ATPL230_TXRXBUF_TXCONF_EB_Msk;
	uc_enable >>= ATPL230_TXRXBUF_TXCONF_EB_Pos;

	return uc_enable;
}

//! \name ATPL230 Physical Layer Interface
//@{
void phy_init (uint8_t uc_ifaceEnable);
uint8_t phy_get_cfg_param (uint16_t us_id, void *p_val, uint16_t uc_len);
uint8_t phy_set_cfg_param (uint16_t us_id, void *p_val, uint16_t uc_len);
uint8_t phy_cmd_cfg_param (uint16_t us_id, uint8_t uc_cmd, uint8_t uc_mask);
uint8_t phy_tx_frame (xPhyMsgTx_t *px_msg);
void phy_rx_frame_cb (xPhyMsgRx_t *px_msg);
void phy_tx_frame_result_cb (xPhyMsgTxResult_t *px_tx_result);
void phy_reset (uint8_t uc_reset_type);
//@}

//! @}


/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* ATPL230_H_INCLUDED */
