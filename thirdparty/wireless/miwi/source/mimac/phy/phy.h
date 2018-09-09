/**
* \file  phy.c
*
* \brief Physical Layer Abstraction for AT86RF212B interface
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

#ifndef _PHY_H_
#define _PHY_H_

#include "miwi_config.h"
#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#endif

#include "miwi_api.h"

#define SEC_LEVEL_CBC_MAC_32    7
#define SEC_LEVEL_CBC_MAC_64    6
#define SEC_LEVEL_CBC_MAC_128   5
#define SEC_LEVEL_CCM_32        4
#define SEC_LEVEL_CCM_64        3
#define SEC_LEVEL_CCM_128       2
#define SEC_LEVEL_CTR           1

/*********************************************************************/
// BANK_SIZE defines the number of packet can be received and stored
// to wait for handling in MiMAC layer.
/*********************************************************************/
#define BANK_SIZE         4

/*********************************************************************/
// SECURITY_KEY_xx defines xxth byte of security key used in the
// block cipher
/*********************************************************************/
#define SECURITY_KEY_00 0x00
#define SECURITY_KEY_01 0x01
#define SECURITY_KEY_02 0x02
#define SECURITY_KEY_03 0x03
#define SECURITY_KEY_04 0x04
#define SECURITY_KEY_05 0x05
#define SECURITY_KEY_06 0x06
#define SECURITY_KEY_07 0x07
#define SECURITY_KEY_08 0x08
#define SECURITY_KEY_09 0x09
#define SECURITY_KEY_10 0x0a
#define SECURITY_KEY_11 0x0b
#define SECURITY_KEY_12 0x0c
#define SECURITY_KEY_13 0x0d
#define SECURITY_KEY_14 0x0e
#define SECURITY_KEY_15 0x0f

/*********************************************************************/
// KEY_SEQUENCE_NUMBER defines the sequence number that is used to
// identify the key. Different key should have different sequence
// number, if multiple security keys are used in the application.
/*********************************************************************/
#define KEY_SEQUENCE_NUMBER 0x00

/*********************************************************************/
// SECURITY_LEVEL defines the security mode used in the application
/*********************************************************************/
#define SECURITY_LEVEL SEC_LEVEL_CCM_32

#define TRX_STATUS_REG    0x01
#define TRX_STATE_REG     0x02
#define TRX_CTRL_0_REG    0x03
#define TRX_CTRL_1_REG    0x04
#define PHY_TX_PWR_REG    0x05
#define PHY_RSSI_REG      0x06
#define PHY_ED_LEVEL_REG  0x07
#define PHY_CC_CCA_REG    0x08
#define CCA_THRES_REG     0x09
#define RX_CTRL_REG       0x0a
#define SFD_VALUE_REG     0x0b
#define TRX_CTRL_2_REG    0x0c
#define ANT_DIV_REG       0x0d
#define IRQ_MASK_REG      0x0e
#define IRQ_STATUS_REG    0x0f
#define VREG_CTRL_REG     0x10
#define BATMON_REG        0x11
#define XOSC_CTRL_REG     0x12
#define RX_SYN_REG        0x15
#define TRX_RPC_REG       0x16
#define XAH_CTRL_1_REG    0x17
#define FTN_CTRL_REG      0x18
#define XAH_CTRL_2_REG    0x19
#define PLL_CF_REG        0x1a
#define PLL_DCU_REG       0x1b
#define PART_NUM_REG      0x1c
#define VERSION_NUM_REG   0x1d
#define MAN_ID_0_REG      0x1e
#define MAN_ID_1_REG      0x1f
#define SHORT_ADDR_0_REG  0x20
#define SHORT_ADDR_1_REG  0x21
#define PAN_ID_0_REG      0x22
#define PAN_ID_1_REG      0x23
#define IEEE_ADDR_0_REG   0x24
#define IEEE_ADDR_1_REG   0x25
#define IEEE_ADDR_2_REG   0x26
#define IEEE_ADDR_3_REG   0x27
#define IEEE_ADDR_4_REG   0x28
#define IEEE_ADDR_5_REG   0x29
#define IEEE_ADDR_6_REG   0x2a
#define IEEE_ADDR_7_REG   0x2b
#define XAH_CTRL_0_REG    0x2c
#define CSMA_SEED_0_REG   0x2d
#define CSMA_SEED_1_REG   0x2e
#define CSMA_BE_REG       0x2f
#define TST_CTRL_DIGI_REG 0x36
#define PHY_TX_TIME_REG   0x3b
#define PHY_PMU_VALUE_REG 0x3b
#define TST_AGC_REG       0x3c
#define TST_SDM_REG       0x3d


#define TRX_CMD_NOP           0
#define TRX_CMD_TX_START      2
#define TRX_CMD_FORCE_TRX_OFF 3
#define TRX_CMD_FORCE_PLL_ON  4
#define TRX_CMD_RX_ON         6
#define TRX_CMD_TRX_OFF       8
#define TRX_CMD_PLL_ON        9
#define TRX_CMD_RX_AACK_ON    22
#define TRX_CMD_TX_ARET_ON    25

#define TRX_STATUS_P_ON               0
#define TRX_STATUS_BUSY_RX            1
#define TRX_STATUS_BUSY_TX            2
#define TRX_STATUS_RX_ON              6
#define TRX_STATUS_TRX_OFF            8
#define TRX_STATUS_PLL_ON             9
#define TRX_STATUS_SLEEP              15
#define TRX_STATUS_BUSY_RX_AACK       17
#define TRX_STATUS_BUSY_TX_ARET       18
#define TRX_STATUS_RX_AACK_ON         22
#define TRX_STATUS_TX_ARET_ON         25
#define TRX_STATUS_RX_ON_NOCLK        28
#define TRX_STATUS_RX_AACK_ON_NOCLK   29
#define TRX_STATUS_BUSY_RX_AACK_NOCLK 30
#define TRX_STATUS_STATE_TRANSITION   31
#define TRX_STATUS_MASK               0x1f

#define PHY_CC_CCA_REG    0x08

#define TRAC_STATUS_SUCCESS                0
#define TRAC_STATUS_SUCCESS_DATA_PENDING   1
#define TRAC_STATUS_SUCCESS_WAIT_FOR_ACK   2
#define TRAC_STATUS_CHANNEL_ACCESS_FAILURE 3
#define TRAC_STATUS_NO_ACK                 5
#define TRAC_STATUS_INVALID                7

/* IRQ_MASK, IRQ_STATUS */
#define BAT_LOW           7
#define TRX_UR            6
#define AMI               5
#define CCA_ED_DONE       4
#define TRX_END           3
#define RX_START          2
#define PLL_UNLOCK        1
#define PLL_LOCK          0

/* TRX_CTRL_2 */
#define RX_SAFE_MODE      7
#define OQPSK_SCRAM_EN    5
#define OQPSK_DATA_RATE   0

/* TRX_CTRL_1 */
#define PA_EXT_EN         7
#define IRQ_2_EXT_EN      6
#define TX_AUTO_CRC_ON    5
#define RX_BL_CTRL        4
#define SPI_CMD_MODE      2
#define IRQ_MASK_MODE     1
#define IRQ_POLARITY      0

/* TRX_STATE */
#define TRAC_STATUS       5
#define TRX_CMD           0

/* PHY_RSSI */
#define RX_CRC_VALID      7
#define RND_VALUE         5
#define RSSI              0

/* PHY_STATUS */
#define PHY_STATUS_SUCCESS                 0
#define PHY_STATUS_ERROR                   1
#define PHY_STATUS_CHANNEL_ACCESS_FAILURE  2
#define PHY_STATUS_NO_ACK                  3

/*- Prototypes -------------------------------------------------------------*/
void PHY_Init(void);
void PHY_SetRxState(bool rx);
void PHY_SetChannel(uint8_t channel);
void PHY_SetPanId(uint16_t panId);
void PHY_SetShortAddr(uint16_t addr);
void PHY_SetTxPower(uint8_t txPower);
void PHY_Sleep(void);
void PHY_Wakeup(void);
void PHY_DataReq(uint8_t *data);
void PHY_DataConf(uint8_t status);
void print_rx_message(void);
void CONSOLE_PrintHex(uint8_t toPrint);
void CONSOLE_PutString(char* str);
void PHY_TaskHandler(void);
void PHY_SetIEEEAddr(uint8_t *ieee_addr);
uint16_t PHY_RandomReq(void);
uint8_t PHY_EdReq(void);
void PHY_EncryptReq(uint8_t *text, uint8_t *key);
void PHY_EncryptReqCBC(uint8_t *text, uint8_t *key);
void PHY_DecryptReq(uint8_t *text, uint8_t *key);
bool DataEncrypt(uint8_t *, uint8_t *, API_UINT32_UNION,
uint8_t );
bool DataDecrypt(uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress,
API_UINT32_UNION FrameCounter, uint8_t FrameControl);
void mic_generator (uint8_t *Payloadinfo, uint8_t  , uint8_t frame_control ,  API_UINT32_UNION FrameCounter , uint8_t * Address);
bool validate_mic(void);

typedef union
{
    uint8_t Val;               // value of interrupts
    struct
    {
        uint8_t RF_TXIF :1;    // transmission finish interrupt
        uint8_t :2;
        uint8_t RF_RXIF :1;    // receiving a packet interrupt
        uint8_t SECIF :1;      // receiving a secured packet interrupt
        uint8_t :4;
    }bits;                  // bit map of interrupts
} AT86RF233_IFREG;

typedef union
{
    uint8_t Val;
    struct
    {
        uint8_t        TX_BUSY             : 1;
        uint8_t        TX_PENDING_ACK      : 1;
        uint8_t        TX_FAIL             : 1;
        uint8_t        RX_SECURITY         : 1;
        uint8_t        RX_IGNORE_SECURITY  : 1;
        uint8_t        RX_BUFFERED         : 1;
        uint8_t        SEC_IF              : 1;
    } bits;
} RADIO_STATUS;


#define SEC_LEVEL_CBC_MAC_32    7
#define SEC_LEVEL_CBC_MAC_64    6
#define SEC_LEVEL_CBC_MAC_128   5
#define SEC_LEVEL_CCM_32        4
#define SEC_LEVEL_CCM_64        3
#define SEC_LEVEL_CCM_128       2
#define SEC_LEVEL_CTR           1


#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
    #define PROTOCOL_HEADER_SIZE 0
#endif

#if defined(PROTOCOL_MESH)
#define PROTOCOL_HEADER_SIZE 32
#endif

#if defined(ENABLE_SECURITY)

    #if SECURITY_LEVEL == 0x01
        #define MIC_SIZE 0
    #elif (SECURITY_LEVEL == 0x02) || (SECURITY_LEVEL == 0x05)
        #define MIC_SIZE 16
    #elif (SECURITY_LEVEL == 0x03) || (SECURITY_LEVEL == 0x06)
        #define MIC_SIZE 8
    #elif (SECURITY_LEVEL == 0x04) || (SECURITY_LEVEL == 0x07)
        #define MIC_SIZE 4
    #endif

    #define RX_PACKET_SIZE (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+MIC_SIZE+17)

    #if (RX_PACKET_SIZE > 127)
        #warning  "Maximum application payload RX BUFFER SIZE is 94"
    #endif

#else

    #define RX_PACKET_SIZE (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+12)

    #if (RX_PACKET_SIZE > 127)
        #warning "Maximum application payload RX BUFFER SIZE is 99"
    #endif

#endif

#if RX_PACKET_SIZE > 127
    #undef RX_PACKET_SIZE
    #define RX_PACKET_SIZE 127
#endif

typedef struct
{
	uint8_t PayloadLen;
	uint8_t Payload[RX_PACKET_SIZE];
} RxBuffer_t;

extern RxBuffer_t RxBuffer[];

#endif
