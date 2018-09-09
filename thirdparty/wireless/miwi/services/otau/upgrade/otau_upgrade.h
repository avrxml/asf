/**
* \file otau_upgrade.c
*
* \brief OTAU Upgrade interface
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

#ifndef OTAU_UPGRADE_H
#define OTAU_UPGRADE_H

#include "otau.h"
#include "sysTimer.h"

#define IMAGE_REQ_RETRY_COUNT               (10)

#define MIN_IMAGE_REQ_INTERVAL_MILLI_SEC    (100)

#define IMAGE_RESP_WAIT_INTERVAL_MILLI_SEC  (10000)

#define SWITCH_REQ_INTERVAL_MILLI_SEC       (3000)

#define SWITCH_INTERVAL_MILLI_SEC           (1000)


#define TYPE_EXTERNAL 0x01
#define TYPE_INTERNAL 0x00

#define APP_INFO_START_ADDRESS (0x0003FF00ul)

#define UPGRADE_OFFSET_ADDRESS (0x00020000ul)

#define APP_START_ADDRESS      (0x00002000ul)

typedef enum {
	OTA_SERVER_DISCOVERY = 0,
	OTA_SERVER_NOTIFY,
	OTA_QUERY_IMAGE,
	OTA_IMAGE_NOTIFY,
	OTA_IMAGE_REQUEST,
	OTA_IMAGE_RESPONSE,
	OTA_SWITCH_REQUEST,
	OTA_SWITCH_RESPONSE
}otauUpgradeMsgCode_t;

COMPILER_PACK_SET(1)
typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint8_t imageType;
	uint32_t imageStart;
	uint32_t imageSize;
	uint8_t imageCrc;
	uint16_t reqInterval;
	uint8_t firmwareName[10];
	uint8_t firmwareVersion[5];
	uint8_t boardName[10];
	uint8_t boardVersion[5];
}otauImageNotifyRequest_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint8_t imageRespType;
	uint32_t blockAddr;
	uint16_t blockSize;
	uint8_t block[APP_MAX_PAYLOAD_SIZE];
}otauImageResponse_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint32_t switchTime;
}otauSwitchImageResponse_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
}otauSwitchImageRequest_t;

typedef struct {
	uint32_t imageStart;
	uint32_t imageSize;
	uint8_t firmwareName[10];
	uint8_t firmwareVersion[5];
	uint8_t boardName[10];
	uint8_t boardVersion[5];
}otauCurrentImage_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint32_t imageStart;
	uint32_t imageSize;
	uint8_t firmwareName[10];
	uint8_t firmwareVersion[5];
	uint8_t boardName[10];
	uint8_t boardVersion[5];
}otauQueryImageRequest_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint8_t reqType;
	uint32_t blockAddr;
	uint32_t blockSize;
}otauImageRequest_t;

/* Data structures of bootloader */

typedef struct 
{
  uint16_t   memtype:1;            // Internal or External
  uint16_t   active:1;             // Whether this app is active
  uint16_t   reserved2:14;         // Place-holder
}ExistingImageInfo_t;

typedef struct 
{
  uint16_t   update_action:1;       // Whether an image update is pending
  uint16_t   src_memtype:1;         // Whether the downloaded image is in internal or external
  uint16_t   dest_memtype:1;        // Whether the executable location is in internal or external
  uint16_t   img_type:2;            // Type of the downloaded image: Encrypted app, Plain app, not-an-executable-app
  uint16_t   reserved:11;            // Place-holder
}BootInfo_t;

// App-specific fields in APP_INFO
typedef struct app_table_tag {
  char app_name[16];               // Description string for the app
  uint32_t  app_start_addr;        // Start address of the app image
  uint32_t  app_size;              // Size of the app image
  uint32_t  src_addr;              // Address where the image is downloaded
  uint32_t  dest_addr;             // Address where the image should be placed at the end of upgrade
  uint32_t  skip_pds_start;
  uint32_t  skip_pds_end;
  uint32_t  img_size;              // Size of the downloaded image
  ExistingImageInfo_t existingImageInfo;  // 2 Byte flag to indicate information abt existing image
  BootInfo_t bootInfo;             // 2 byte flag
  uint8_t   security_key [16];     // Security key for recovering the encryted image after download
} app_table_t;

/* This structures needs to be alligned*/
typedef struct {
	uint16_t  multiApps       :1;   //  BIT_0
	uint16_t  imgSecurity     :1;   //  BIT_1
	uint16_t  rfInterface     :1;   //  BIT_2
	uint16_t  usart0Interface :1;   //  BIT_3
	uint16_t  usart1Interface :1;   //  BIT_4
	uint16_t  spiInterface    :1;   //  BIT_5
	uint16_t  twiInterface    :1;   //  BIT_6
	uint16_t  usbInterface    :1;   //  BIT_7
	uint16_t  reserved        :8;   //  BIT_8 to BIT_15
} featureBits_t;

typedef union {
	uint16_t        mask;
	featureBits_t   bits;
} featureMask_t;


// APP_INFO table 
typedef struct {
  uint8_t         crc;                   // CRC over the entire app-info (excluding this field)
  /*
  Specifies default application index for the bootloader to jump to, 
  if the Active fields in application-related fields are in inconsistent state 
  (e.g. all set as non-active).
  */
  uint8_t         default_app;           // Index of the app (0 or 1) that should be considered the default in case of ambiguity while jumping to app
  
  /*Application-configurable bit-mask indicating enabled bootloader features and interfaces:
    Bit 0: multi-image support 
    Bit 1: RF interface support
    Bit 2: USART 0 interface support
    Bit 3: USART 1 interface support
    Bit 4: SPI interface support
    Bit 5: TWI interface support
    Bit 6: USB interface support
    Bit 7-15: Reserved
  */
  featureMask_t   required_features;     // Bit-mask for features requested by app (See boot_version.h for bit positions) 
  uint8_t         app_cnt;               // Number of valid applications in internal flash. Shall be at least 1.
  uint8_t         reserved1[3];          // Place-holder
  app_table_t     appTable[2];           // App-specific sub-table for each app (upto 2)
} app_info_t;

COMPILER_PACK_RESET()

void handle_upgrade_otau_msg(otau_domain_msg_t *otau_domain_msg);
void otauUpgradeTimerHandler(SYS_Timer_t *timer);
void otauUpgradeInit(void);
void otauUpgradeSentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status);
void otauUpgradeRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload);
#endif /* OTAU_UPGRADE_H */
