/**
 * \file
 *
 * \brief Terminal Target application.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   3. The name of ATMEL may not be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 *   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 *   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 *   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
 */
/**
 * \mainpage
 *
 * \section intro Introduction
 * AVR477 is a Unique reference design with true integration of RF, Touch & MEMS

 * \section Connection Connection Aspects
 * \image html FlowDiagram.jpg

 * \section Sections Sections
 * - AVR477 Remote : This page provides firmware details of the AVR477 Remote.
 * - AVR477 Target : This page provides firmware details of the AVR477 Target.
 *                   The Target application can be run on the STB-RCB128RFA1  
 *                   board or the RZ600-RF231 board.     
 *
 * \section deviceinfo Device Info
 * For the Demo scenario,a Target application that sends QDebug Data,received over air,
 * to QTouch Studio is used.\nThis Target Application can be run on the ATUC3A3256S MCU 
 * RZ600 board or on the ATmega128RFA1 MCU on the Sensor Terminal Board,when used along
 * with the QT600.
 *  
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC,IAR for AVR,IAR for AVR32.\n
 * Supported Versions : AVR Studio 4,AVR Studio 5,IAR for AVR 6.10,IAR for AVR32 4.10 or higher.\n 
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
 
/**
 * \page Tgt AVR477 Target
 *
 * \section intro Introduction
 * AVR477 Target Application is used for demonstration of the capabilities 
 * of the AVR477 Touch Remote Control.

 * \section HW Hardware
 *  1.RZ600 USB board along with the AT86RF231 radio board\n
 * \image html Receiver-1.jpg
 *  2.RCB128RFA1 mounted on the Sensor Terminal Board(STB) connected
 *    over TWI to the QT600 kit       
 * \image html Receiver-2.jpg
 * \section desc Description 
 * The Target Application receives QTouch Data over air as RF4CE packets
 * and transfers them to QTouch Studio for analysis of touch data.\n
 * When RZ600 is used,the USB HID interfacde is used for communication to QTouch Studio.\n
 * When the STB-RCB128RFA1 is used,the touch data is received by the RCB128RFA1 over air 
 * and transferred over TWI to the QT600 kit.
 * QT600,then,transfers this data over USB to QTStudio.
 
 * @defgroup TApp_API Target Application Related API's
 */


/* === INCLUDES ============================================================ */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "pal.h"
#include "app_config.h"
#include "rf4ce.h"
#include "pb_pairing.h"
#include "zrc.h"
#include "vendor_data.h"

#include "qdebug_transport.h"
#include "twi_master.h"
#include "compiler.h"

/* === TYPES =============================================================== */

typedef enum node_status_tag
{
    IDLE = 0,
    RESETTING,
    POWER_SAVE,
    STARTING,
    PUSH_BUTTON_PAIRING,
#ifdef ZRC_CMD_DISCOVERY
    CMD_DISCOVERING,
#endif
    ALL_IN_ONE_START,
    WARM_STARTING,
    PRINTING_PAIRING_TABLE,
    UNPAIRING,
    CH_AGILITY_EXECUTION,
    BASE_CHANNEL_CHANGE,
    GETTING_CH_AG_NIBS
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

#define MAX_PAIRED_DEVICES  NWKC_MAX_PAIRING_TABLE_ENTRIES
#define IEEE_ADDRESS_BYTES  8
#define PAIR_WAIT_PERIOD    500000
#if (NO_OF_LEDS >= 3)
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_1)
#define LED_DATA                        (LED_2)
#elif (NO_OF_LEDS == 2)
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_0)
#define LED_DATA                        (LED_1)
#else
#define LED_START                       (LED_0)
#define LED_NWK_SETUP                   (LED_0)
#define LED_DATA                        (LED_0)
#endif

//! @ingroup QDebug-Target
//! List of defines for QT600 control with STB-RCB
//! @{
#define RESET_QT600_PORT                (PORTD)
#define RESET_QT600_DIR                 (DDRD)
#define RESET_QT600_PIN                 (PD5)

#define RESET_QT600_PIN_INIT()          do \
                                        {  \
                                            RESET_QT600_PORT |= 1 << RESET_QT600_PIN;  /* low active */\
                                            RESET_QT600_DIR |= 1 << RESET_QT600_PIN; \
                                        } while (0)

#define RESET_QT600_ON()                    RESET_QT600_PORT &= ~(1 << RESET_QT600_PIN)  /* low active */
#define RESET_QT600_OFF()                   RESET_QT600_PORT |= 1 << RESET_QT600_PIN
//! @}

//! @ingroup QDebug-Target
//! List of the command IDs used in QDebug.
//! @{
#define QT_SIGN_ON                         0x21
#define QT_GLOBAL_CONFIG                   0x22
#define QT_SENSOR_CONFIG                   0x23
#define QT_SIGNALS                         0x24
#define QT_REFERENCES                      0x25
#define QT_DELTAS                          0x26
#define QT_STATES                          0x27
//! @}

/* === GLOBALS ============================================================= */

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
#ifdef ZRC_CMD_DISCOVERY
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;
#endif

static node_status_t node_status;

static bool ch_ag_enabled = true;
static uint8_t target_auto_start = true; /* To start the target automatically without any inputs*/

/* === EXTERNALS =========================================================== */

extern uint64_t tal_pib_IeeeAddress;
extern uint16_t tal_pib_PANId;
extern uint8_t tal_pib_CurrentChannel;

uint8_t rf4ce_new_msg = 0;

/* === PROTOTYPES ========================================================== */

static void app_task(void);
static void led_handling(void *callback_parameter);

/*
 * @brief __unhandled_interrupt
 *
 * This Funciton added in order to avoid linker error messages for AVR32 Latest
 * version - 4.10.2.
 */

#if defined (__ICCAVR32__)
__interrupt void __unhandled_interrupt(void)
{
    pal_alert();
}
#endif
   

/* === IMPLEMENTATION ====================================================== */


/**
 * @brief Main function of the Terminal Target application
 * @ingroup App_API
 */
int main(void)
{
    /* Initialize all layers */
    if (nwk_init() != NWK_SUCCESS)
    {
        /* something went wrong during initialization */
        pal_alert();
    }
    /* disable pull-ups */
    MCUCR |= (1u << PUD);
    
#ifdef FLASH_NVRAM
    pal_ps_set(EE_IEEE_ADDR,IEEE_ADDRESS_BYTES, &tal_pib_IeeeAddress);
#endif
    
    /* Initialize LEDs. */
    pal_led_init();
    pal_led(LED_START, LED_ON);         /* indicating application is started */
    pal_led(LED_NWK_SETUP, LED_OFF);    /* indicating network is started */
    pal_led(LED_DATA, LED_OFF);         /* indicating data reception */

    /*
     * The stack is initialized above, hence the global interrupts are enabled
     * here.
     */
    
    pal_global_irq_enable();
 
    /**
    * @brief TWI and QT600 interface initialization
    */
    int i;
    twi_master_init();
    RESET_QT600_PIN_INIT();
    RESET_QT600_ON();
    for (i = 0; i < 100 ; i++)
        asm("nop");
    
    /* Endless while loop */
    while (1)
    {
        app_task(); /* Application task */
        if(rf4ce_new_msg == 1)
        {
          twi_send_message();
          TX_index = 0;
          rf4ce_new_msg = 0;
        }
 
        nwk_task(); /* RF4CE network layer task */
    }
}


/**
 * @brief Application task
 * @ingroup TApp_API 
 */
static void app_task(void)
{
    /**
     * To start NLME Reset procedure when application starts up
     */
    if (target_auto_start == true)
    {
        target_auto_start = false;
        node_status = ALL_IN_ONE_START;
        ch_ag_enabled = false;
        nlme_reset_request(true);
        return;
    }
    else 
    {
        if(node_status != IDLE)
            return;    
        if (Receive_Message() == 0)
            return;
            /**
             * To send out QTouch Studio commands over air 
             * to Touch Remote  
             */  
            vendor_data_request(0, PROFILE_ID_VENDOR_DATA,
                                NWKC_VENDOR_IDENTIFIER, RF4CE_RX_Buffer[2], RF4CE_RX_Buffer,
                                TXO_UNICAST | TXO_DST_ADDR_IEEE | TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | TXO_CH_NOT_SPEC | TXO_VEND_SPEC);  
                
    }/*end of else*/
       
}

/**
 * @brief Handles the rc command indications at terminal target.
 *
 * @param PairingRef       Pairing reference
 * @param nsduLength       Length of the payload.
 * @param nsdu             Actual payload
 * @param RxLinkQuality    Link quality of received packet.
 * @param RxFlags          Rx Flags.
 */
void zrc_cmd_indication(uint8_t PairingRef, uint8_t nsduLength, uint8_t *nsdu,
                         uint8_t RxLinkQuality, uint8_t RxFlags)
{
    zrc_cmd_frm_t *zrc_frm;
       
    // Switch LED on indicating data reception 
    pal_led(LED_DATA, LED_TOGGLE);
    
    /* Check with frame control field which kind of data is indicated */
    zrc_frm = (zrc_cmd_frm_t *)nsdu;
    switch (zrc_frm->fcf)
    {
#ifdef RF4CE_TARGET
        case USER_CONTROL_PRESSED:
        {

             /**
              * Receive the data on RF4CE Link, Store in Tx_Buffer and Send to
              * TWI interface 
              */
            int i;
            /* Keep compiler happy */
            UNUSED(i);
#ifdef _GENERIC_QDEBUG_ 
            for(i = 1 ; i < nsduLength ; i++)
                PutChar(nsdu[i]);
            rf4ce_new_msg = 1;
#endif /* _GENERIC_QDEBUG_  */
    }
    break;/*case USER_CONTROL_PRESSED */
#endif  /* #ifdef RF4CE_TARGET */

    default:
        break;
    }

    /* Keep compiler happy */
    UNUSED(RxFlags);
    UNUSED(PairingRef);
    UNUSED(RxLinkQuality);
}


/**
 * @brief Notify the application of the status of its request for data tx
 *        (nlde-data.request)
 *
 * @param Status              nwk status.
 * @param PairingRef          Pairing Ref for the destination.
 * @param ProfileId           Profile id
 */
void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t ProfileId)
{
    /* Keep compiler happy. */
    UNUSED(Status);
    UNUSED(PairingRef);
    UNUSED(ProfileId);
}


/**
 * @brief Prints the status of push button pairing and if status is success,
 *        then send the cmd discovery to controller.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref of the new entry.
 */
void pbp_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
   if(Status != NWK_SUCCESS)
    {
        target_auto_start = true;
        node_status = IDLE;
    }
     
#ifdef ZRC_CMD_DISCOVERY
    else if (Status == NWK_SUCCESS)
    {
        pal_led(LED_NWK_SETUP, LED_OFF);
        node_status = CMD_DISCOVERING;
        zrc_cmd_disc_request(PairingRef);
    }
#endif
}


/**
 * @brief This function decides whether push button pairing request should be
 *        allowed.
 *
 * Decision could be based on one of the parameter.
 *
 * @param Status              nwk status
 * @param SrcIEEEAddr         IEEE Address of the source requesting the pair.
 * @param OrgVendorId         Vendor Id of the source requesting the pair.
 * @param OrgVendorString     Vendor string of the source requesting the pair.
 * @param OrgUserString       User string of the source requesting the pair.
 * @param KeyExTransferCount  Number of key seeds to establish key.
 *
 * @return true if pairing is granted; else false
 */
bool pbp_allow_pairing(nwk_enum_t Status, uint64_t SrcIEEEAddr,
                      uint16_t OrgVendorId, uint8_t OrgVendorString[7],
                      uint8_t OrgUserString[15], uint8_t KeyExTransferCount)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(SrcIEEEAddr);
    UNUSED(OrgVendorId);
    UNUSED(OrgVendorString[0]);
    UNUSED(OrgUserString[0]);
    UNUSED(KeyExTransferCount);

    return true;
}


/**
 * @brief The command discovery confirm for the previous command discovery request.
 *        request command was receiced.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref for destination.
 * @param SupportedCmd     Supported commands by destination node.
 */
#ifdef ZRC_CMD_DISCOVERY
void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef, uint8_t *SupportedCmd)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
    UNUSED(SupportedCmd);
}
#endif


/**
 * @brief The command discovery indication callback indicates that a command discovery
 *        request command was receiced.
 *
 * @param PairingRef          Pairing Ref for the source.
 */
#ifdef ZRC_CMD_DISCOVERY
void zrc_cmd_disc_indication(uint8_t PairingRef)
{
    /**
     * Reset QT600-Interface board 
     */
    int i;
    RESET_QT600_OFF();
    for (i = 0; i < 100 ; i++)
        asm("nop");

        
    /* Send back the response */
    uint8_t cec_cmds[32];
#if (PAL_GENERIC_TYPE == AVR32)
    PGM_READ_BLOCK(cec_cmds, supported_cec_cmds, 32);
#else
    PGM_READ_BLOCK(cec_cmds, supported_cec_cmds, 32);
#endif
    zrc_cmd_disc_response(PairingRef, cec_cmds);
    node_status = IDLE;
        
#if CHANNEL_AGILITY_ENABLE
     /* Enable Channel Agility */
    node_status = CH_AGILITY_EXECUTION;
    nwk_ch_agility_request(AG_PERIODIC);
#endif
    
}
#endif


/**
 * @brief Notify the application of the status of its request for the value of a
 *        NIB attribute
 *
 * @param Status              nwk status
 * @param NIBAttribute        NIBAttribute
 * @param NIBAttributeIndex   NIBAttributeIndex
 * @param NIBAttributeValue   Value of the NIB attribute.
 */
void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
                      uint8_t NIBAttributeIndex, void *NIBAttributeValue)
{
    if (Status == NWK_SUCCESS)
    {
        switch (NIBAttribute)
        {
             default:
                break;

        }
    }
}
/**
 * @brief Prints the new channel after channel agility took place.
 *
 * @param LogicalChannel  changed logical channel.
 */
void nwk_ch_agility_indication(uint8_t LogicalChannel)
{
    /* keep compiler happy */
    UNUSED(LogicalChannel);
}
/**
 * @brief Notify the application of the status of its request to reset the NWK
 *        layer.
 *
 * @param Status              nwk status
 */
void nlme_reset_confirm(nwk_enum_t Status)
{
   if (node_status == ALL_IN_ONE_START)
    {
        nlme_start_request();
    }
}


/**
 * @brief Notify the application of the status of its request to enable or
 *        disable the receiver.
 *
 * @param Status              nwk status
 */
void nlme_rx_enable_confirm(nwk_enum_t Status)
{
    /* Keep compiler happy */
    UNUSED(Status);
}


/**
 * @brief Notify the application of the status of its request to to change the
 *        value of a NIB attribute.
 *
 * @param Status              nwk status
 * @param NIBAttribute        NIBAttribute
 * @param NIBAttributeIndex   NIBAttributeIndex
 */
void nlme_set_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex)
{
  if (Status != NWK_SUCCESS)
    {
        return;
    }

    if (NIBAttribute == nwkBaseChannel)
    {
        pal_timer_start(T_LED_TIMER,
                        PAIR_WAIT_PERIOD,
                        TIMEOUT_RELATIVE,
                        (FUNC_PTR)led_handling,
                        NULL);
        pal_led(LED_NWK_SETUP, LED_ON);

        dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
        profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

        RecDevTypeList[0]   = SUPPORTED_DEV_TYPE_0;
        RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;

        pbp_rec_pair_request(APP_CAPABILITIES, RecDevTypeList, RecProfileIdList);
        return;
    }

  
    /* Keep compiler happy */
    UNUSED(NIBAttributeIndex);
}


/**
 * @brief Notify the application of the status of its request to start a network.
 *
 * The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 *
 * @param Status       nwk status
 */
void nlme_start_confirm(nwk_enum_t Status)
{
   if (node_status == ALL_IN_ONE_START)
    {
       /* Fix a channel, basic functionality */
        uint8_t channel = 25;
        nlme_set_request(nwkBaseChannel, 0, &channel);
    }
}


/**
 * @brief Notify the application of the removal of link by another device.
 *
 * The NLME-UNPAIR.indication primitive allows the NLME to notify the application
 * of the removal of a pairing link by another device.
 *
 * @param PairingRef       Pairing Ref for which entry is removed from pairing table.
 */
void nlme_unpair_indication(uint8_t PairingRef)
{
    /* Keep compiler happy */
    UNUSED(PairingRef);
}


/**
 * @brief Notify the application for the previous unpair request.
 *
 * The NLME-UNPAIR.confirm primitive allows the NLME to notify the application of
 * the status of its request to remove a pair with another device.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref for which entry is removed from pairing table.
 */
void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef)
{
    if (node_status == UNPAIRING)
    {
        node_status = IDLE;
    }

    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
}


/**
 * @brief Confirms the previous channel agility request, i.e. nwk_ch_agility_req()
 *
 * @param Status           nwk status
 * @param ChannelChanged   whether channel is changed.
 * @param LogicalChannel   changed logical channel.
 */
void nwk_ch_agility_confirm(nwk_enum_t Status, bool ChannelChanged, uint8_t LogicalChannel)
{
    node_status = IDLE;

    /*Keep compiler happy */
    UNUSED(Status);
    UNUSED(LogicalChannel);
}

/**
 * @brief Notify the application about the key update.
 *
 * The NLME-UPDATE-KEY.confirm primitive allows the NLME to notify the
 * application of the status of its request to change the security link key of
 * a pairing table entry.
 *
 * @param status     nwk status (constants defined by nwk layer)
 * @param PairingRef Pairing reference
 *
 */
#ifdef RF4CE_SECURITY
void nlme_update_key_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
}
#endif



/**
 * @brief Post processing of the vendor data response.
 *
 * @param PairingRef       Pairing reference
 * @param ProfileId        Profile id.
 * @param VendorId         Vendor ID.
 * @param nsduLength       Length of the payload.
 * @param nsdu             Actual payload
 * @param RxLinkQuality    Link quality of received packet.
 * @param RxFlags          Rx Flags.
 */
void vendor_data_ind(uint8_t PairingRef, profile_id_t ProfileId, uint16_t VendorId,
                     uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
                     uint8_t RxFlags)
{

    /* Keep compiler happy */
    UNUSED(PairingRef);
    UNUSED(ProfileId);
    UNUSED(VendorId);
    UNUSED(nsduLength);
    UNUSED(RxLinkQuality);
    UNUSED(RxFlags);
}


/**
 * @brief Post processing of the vendor data confirm.
 *
 * @param status     nwk status (constants defined by nwk layer)
 * @param PairingRef Pairing reference
 *
 */
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
    UNUSED(Status);
    UNUSED(PairingRef);
 
}

//! @ingroup TApp_API
//! Application Task related APIs
//! @{
/**
 * @brief LED handling including timer control .
 */
static void led_handling(void *callback_parameter)
{
    switch (node_status)
    {
        case PUSH_BUTTON_PAIRING:
        case ALL_IN_ONE_START:
            pal_timer_start(T_LED_TIMER,
                            PAIR_WAIT_PERIOD,
                            TIMEOUT_RELATIVE,
                            (FUNC_PTR)led_handling,
                            NULL);
            pal_led(LED_NWK_SETUP, LED_TOGGLE);
            break;

        default:
            pal_timer_stop(T_LED_TIMER);
            pal_led(LED_DATA, LED_OFF);
            pal_led(LED_NWK_SETUP, LED_OFF);
            break;
    }

    /* Keep compiler happy */
    UNUSED(callback_parameter);
}
//! @}

/* EOF */

