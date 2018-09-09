/**
* \file  phy_at86rf212b.h
*
* \brief Physical Layer Abstraction for AT86RF212B addon interface
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
#ifndef _DRV_AT86RF212B_H_
#define _DRV_AT86RF212B_H_



#define AES_BLOCK_SIZE                 16
#define AES_CORE_CYCLE_TIME            24 /* us */
#define RANDOM_NUMBER_UPDATE_INTERVAL  1 /* us */

/*- Types ------------------------------------------------------------------*/

#define CC_CTRL_0_REG     0x13
#define CC_CTRL_1_REG     0x14
#define RF_CTRL_0_REG     0x16

#define RF_CTRL_1_REG     0x19 /* Only in AT86RF212 */


#define AES_STATUS_REG    0x82
#define AES_CTRL_REG      0x83
#define AES_KEY_REG       0x84
#define AES_STATE_REG     0x84
#define AES_CTRL_M_REG    0x94

/* TRX_STATUS */
#define CCA_DONE          7
#define CCA_STATUS        6
#define TRX_STATUS        0

/* TRX_STATE */
#define TRAC_STATUS       5
#define TRX_CMD           0

/* TRX_CTRL_0 */
#define PAD_IO            6
#define PAD_IO_CLKM       4
#define CLKM_SHA_SEL      3
#define CLKM_CTRL         0



/* PHY_TX_PWR */
#define PA_BOOST          7
#define GC_PA             5
#define TX_PWR            0

/* PHY_RSSI */
#define RX_CRC_VALID      7
#define RND_VALUE         5
#define RSSI              0

/* PHY_CC_CCA */
#define CCA_REQUEST       7
#define CCA_MODE          5
#define CHANNEL           0

/* CCA_THRES */
#define CCA_CS_THRES      4 /* Only in AT86RF212B */
#define CCA_ED_THRES      0

/* RX_CTRL */
#define JCM_EN            5

/* TRX_CTRL_2 */
#define RX_SAFE_MODE      7
#define TRX_OFF_AVDD_EN   6
#define OQPSK_SCRAM_EN    5
#define OQPSK_SUB1_RC_EN  4 /* Only in AT86RF212 */
#define ALT_SPECTRUM      4 /* Only in AT86RF212B */
#define BPSK_OQPSK        3
#define SUB_MODE          2
#define OQPSK_DATA_RATE   0

#define PHY_MOD_BPSK20_CHAN_0   (0x00) //((0x00) || (0<<SUB_MODE) || (0 << BPSK_OQPSK) || (0 << ALT_SPECTRUM))
#define PHY_MOD_BPSK40_CHAN_N	(0x04) //((0x00) || (1<<SUB_MODE) || (0 << BPSK_OQPSK) || (0 << ALT_SPECTRUM))
#define PWR_BPSK_OFFSET			(0x03)

/* ANT_DIV */
#define ANT_SEL           7 /* Only in AT86RF212B */
#define ANT_DIV_EN        3 /* Only in AT86RF212B */
#define ANT_EXT_SW_EN     2
#define ANT_CTRL          0



/* VREG_CTRL */
#define AVREG_EXT         7
#define AVDD_OK           6
#define DVREG_EXT         3
#define DVDD_OK           2

/* BATMON */
#define PLL_LOCK_CP       7
#define BATMON_OK         5
#define BATMON_HR         4
#define BATMON_VTH        0

/* XOSC_CTRL */
#define XTAL_MODE         4
#define XTAL_TRIM         0

/* CC_CTRL_1 */
#define CC_BAND           0

/* RX_SYN */
#define RX_PDT_DIS        7
#define RX_OVERRIDE       4 /* Only in AT86RF212B */
#define RX_PDT_LEVEL      0

/* RF_CTRL_0 */
#define PA_LT             6
#define F_SHIFT_MODE      2 /* Only in AT86RF212B */
#define GC_TX_OFFS        0

/* XAH_CTRL_1 */
#define CSMA_LBT_MODE     6
#define AACK_FLTR_RES_FT  5
#define AACK_UPLD_RES_FT  4
#define AACK_ACK_TIME     2
#define AACK_PROM_MODE    1

/* FTN_CTRL */
#define FTN_START         7

/* RF_CTRL_1 */
#define RF_MC             4 /* Only in AT86RF212 */

/* PLL_CF */
#define PLL_CF_START      7
#define PLL_CF            0 /* Only in AT86RF212 */

/* PLL_DCU */
#define PLL_DCU_START     7

/* XAH_CTRL_0 */
#define MAX_FRAME_RETRES  4
#define MAX_CSMA_RETRES   1
#define SLOTTED_OPERATION 0

/* CSMA_SEED_1 */
#define AACK_FVN_MODE     6
#define AACK_SET_PD       5
#define AACK_DIS_ACK      4
#define AACK_I_AM_COORD   3
#define CSMA_SEED_1       0

/* CSMA_BE */
#define MAX_BE            4
#define MIN_BE            0

/* AES_CTRL */
#define AES_CTRL_DIR      3
#define AES_CTRL_MODE     4
#define AES_CTRL_REQUEST  7

/* AES_STATUS */
#define AES_STATUS_DONE   0
#define AES_STATUS_ER     7

#define RF_CMD_REG_W      ((1 << 7) | (1 << 6))
#define RF_CMD_REG_R      ((1 << 7) | (0 << 6))
#define RF_CMD_FRAME_W    ((0 << 7) | (1 << 6) | (1 << 5))
#define RF_CMD_FRAME_R    ((0 << 7) | (0 << 6) | (1 << 5))
#define RF_CMD_SRAM_W     ((0 << 7) | (1 << 6) | (0 << 5))
#define RF_CMD_SRAM_R     ((0 << 7) | (0 << 6) | (0 << 5))





/* TRX_STATE */
/*#define TRAC_STATUS       5
#define TRX_CMD           0*/

/* PHY_RSSI */
/*#define RX_CRC_VALID      7
#define RND_VALUE         5
#define RSSI              0*/

/*- Definitions ------------------------------------------------------------*/
#define PHY_RSSI_BASE_VAL_BPSK_20             (-100)
#define PHY_RSSI_BASE_VAL_BPSK_40             (-99)
#define PHY_RSSI_BASE_VAL_OQPSK_SIN_RC_100    (-98)
#define PHY_RSSI_BASE_VAL_OQPSK_SIN_250       (-98)
#define PHY_RSSI_BASE_VAL_OQPSK_RC_250        (-97)

/*- Prototypes -------------------------------------------------------------*/

void PHY_SetBand(uint8_t band);
void PHY_SetModulation(uint8_t modulation);



//TODO
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
} AT86RF212B_IFREG;


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
#endif
