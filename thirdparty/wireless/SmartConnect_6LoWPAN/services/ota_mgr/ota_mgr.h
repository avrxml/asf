/**
 * \file ota_mgr.h
 *
 * \brief ota_mgr module implementation
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 *
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the LWMesh OtauSvrMgr Application
 * //TODO
 */

#ifndef OTA_MGR_H
#define OTA_MGR_H


#include "ota.h"

/************************************************************************/
/* Over-the-air Configuration parameters                                */
/************************************************************************/

/* Upgrade mode enable*/
#define UPGRADE_MODE        1
/* Debug mode enable*/
#define DEBUG_MODE          0
/* Broadcast Address size */
#define BROADCAST_ADDR_SIZE 2
/* Native Address size */
#define NATIVE_ADDR_SIZE    8
/* Extended address size */
#define EXTENDED_ADDR_SIZE  8
/* IPv6 Prefix address size */
#define PREFIX_ADDR_SIZE    8

#define APP_MAX_PAYLOAD_SIZE  (85)

#define PHY_MAX_PAYLOAD_SIZE  (100)

/**
 * The mask for the ACK request bit of the FCF
 */
#define FCF_ACK_REQUEST                 (1 << 5)

/**
 * The mask for the PAN ID compression bit of the FCF
 */
#define FCF_PAN_ID_COMPRESSION          (1 << 6)

/**
 * Address Mode: NO ADDRESS
 */
#define FCF_NO_ADDR                     (0x00)

/**
 * Address Mode: RESERVED
 */
#define FCF_RESERVED_ADDR               (0x01)

/**
 * Address Mode: SHORT
 */
#define FCF_SHORT_ADDR                  (0x02)

/**
 * Address Mode: LONG
 */
#define FCF_LONG_ADDR                   (0x03)

/**
 * Defines the offset of the destination address
 */
#define FCF_DEST_ADDR_OFFSET            (10)

/**
 * Defines the offset of the source address
 */
#define FCF_SOURCE_ADDR_OFFSET          (14)

/**
 * Macro to set the source address mode
 */
#define FCF_SET_SOURCE_ADDR_MODE(x)     ((unsigned int)((x) << \
	FCF_SOURCE_ADDR_OFFSET))

/**
 * Macro to set the destination address mode
 */
#define FCF_SET_DEST_ADDR_MODE(x)       ((unsigned int)((x) << \
	FCF_DEST_ADDR_OFFSET))

/**
 * A macro to set the frame type.
 */
#define FCF_SET_FRAMETYPE(x)            (x)

/**
 * The mask for the security enable bit of the FCF.
 */
#define FCF_SECURITY_ENABLED            (1 << 3)

/*
 * Defines the mask for the FCF address mode
 */
#define FCF_ADDR_MASK                   (3)

/*
 * Macro to get the source address mode.
 */
#define FCF_GET_SOURCE_ADDR_MODE(x) \
	(((x) >> FCF_SOURCE_ADDR_OFFSET) & FCF_ADDR_MASK)

/*
 * Macro to get the destination address mode.
 */
#define FCF_GET_DEST_ADDR_MODE(x) \
	(((x) >> FCF_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)
/**
 * Length (in octets) of FCF
 */
#define FCF_LEN                             (2)

/**
 * Length (in octets) of FCS
 */
#define FCS_LEN                             (2)

/**
 * Length of the sequence number field
 */
#define SEQ_NUM_LEN                         (1)

/**
 * Length (in octets) of extended address
 */
#define EXT_ADDR_LEN                        (8)

/**
 * Length (in octets) of short address
 */
#define SHORT_ADDR_LEN                      (8)

/**
 * Length (in octets) of PAN ID
 */
#define PAN_ID_LEN                          (2)

/**
 * Default value for PIB macShortAddress
 */
#define macShortAddress_def             (0xFFFF)

#define FRAME_OVERHEAD                      (3)

#define NVM_APP_SHORT_ADDR            (0x80400EL)
#define NVM_APP_IEEE_ADDR             (0x804008L)

#define NVM_DEVICE_INFO_LOCATION      ((FLASH_ADDR + FLASH_SIZE) - 64UL)

COMPILER_PACK_SET (1)
typedef struct {
	uint64_t extended_addr;
	uint64_t native_addr;
}node_address_t;

typedef struct {
	uint32_t image_start;
	uint32_t image_size;
	uint8_t new_image;
	uint8_t supported_mode;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
}node_info_t;
COMPILER_PACK_RESET ()



void ota_mgr_init(void);
void ota_mgr_data_req(module_code_t msg_code, addr_mode_t addr_mode, uint8_t *addr, void *payload, uint8_t len);
void ota_mgr_rcvd_frame(uint8_t addr_mode, uint8_t *src_addr, uint8_t length, uint8_t *payload);
void ota_mgr_sent_frame(uint8_t status);
void ota_mgr_task(void);
void ota_mgr_timer_start(module_code_t msg_code, uint32_t interval, ota_timer_mode_t mode, void *handler);
void ota_mgr_timer_stop(module_code_t msg_code);
uint8_t *get_node_address(addr_mode_t addr_mode);
void ota_mgr_set_channel(uint8_t channel);
void ota_mgr_led(ota_led_t led_state);
void ota_mgr_reset_device(void);
uint16_t *get_pan_id(void);

void get_node_info(node_info_t * client_info);
#endif /* OTA_MGR_H */

