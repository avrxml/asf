/**
 *
 * \file
 *
 * \brief SPI Flash.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef __SPI_FLASH_MAP_H__
#define __SPI_FLASH_MAP_H__

//#define DOWNLOAD_ROLLBACK
//#define OTA_GEN
#define _PROGRAM_POWER_SAVE_
#define BT_IMAGE_PRESENT

/* =======*=======*=======*=======*=======
 * 	  General Sizes for Flash Memory
 * =======*=======*=======*=======*=======
 */

#define FLASH_START_ADDR					(0UL)
/*!<Starting Address of Flash Memory
 *
 */
#define FLASH_BLOCK_SIZE					(32UL * 1024)
/*!<Block Size in Flash Memory
 */
#define FLASH_SECTOR_SZ						(4 * 1024UL)
/*!<Sector Size in Flash Memory
 */
#define FLASH_PAGE_SZ						(256)
/*!<Page Size in Flash Memory
 */

#define FLASH_2M_TOTAL_SZ					(256 * 1024UL)
/*!<Total Size of 2M Flash Memory
 */
#define FLASH_4M_TOTAL_SZ					(512 * 1024UL)
/*!<Total Size of 4M Flash Memory
 */
#define FLASH_8M_TOTAL_SZ					(1024 * 1024UL)
/*!<Total Size of 8M Flash Memory
 */


#if 0 // MTODO: Remove me
#ifdef BT_IMAGE_PRESENT
#define FLASH_TOTAL_SZ						FLASH_4M_TOTAL_SZ
/*!<Total Size of Flash Memory
*/
#else
#define FLASH_TOTAL_SZ						FLASH_2M_TOTAL_SZ
/*!<Total Size of Flash Memory
*/
#endif
#endif

/*
 * Detailed Sizes and locations for 8M Flash Memory:
 *  ____________________ ___________ ___________________________ _______________________________________________
 * | Starting Address	|	Size	|	Location's Name			|	Description						   			|
 * |____________________|___________|___________________________|_______________________________________________|
 * |	  0 K  			|	  4	K	| 	Boot Firmware			|	Firmware to select which version to run		|
 * |	  4	K 			|	  8 K	|	Control Section			|	Structured data used by Boot firmware		|
 * |	 12 K			|     4	K	|	PLL+GAIN :				|	LookUp Table for PLL and Gain calculations	|
 * |	  				|     			PLL  Size = 1K			|		PLL				 						|
 * |	  				|     		|	GAIN Size = 3K			|		Gain configuration				 		|
 * |	 16	K			|	  4	K	|	CERTIFICATE				|	X.509 Certificate storage					|
 * |	 20 K			|	  4	K	|	Scratch Section			|	Empty Section								|
 * |	 24 K			|	  2	K	|	Program Firmware		|												|
 * |	 26	K			|	194 K 	|	Main Firmware			|	Main Firmware to run WiFi Chip				|
 * |	220	K			|	  8	K	|	HTTP Files				|	Files used with Provisioning Mode			|
 * |	228	K			|	  0 K	|	PS_Firmware				|	Power Save Firmware							|
 * |	228	K			|	  4	K	|	Connection Parameters	|	Parameters for success connection to AP		|
 * |	232	K			|	128	K	|	BT Firmware (Optional)	|	BT Firmware for BT Cortus if present		|
 * |	(512 - 40)	K	|	 40	K	|	Application				|	Cortus App									|
 * |____________________|___________|___________________________|_______________________________________________|
 *
 *
 * *Keys for Comments with each MACRO:
 * 		"L:xxxK" -means-> location 	:xxxK
 *		"S:xxxK" -means-> Size is 	:xxxK
 */

/* Boot Firmware: which used to select which firmware to run
 * starting Address L:  0 K |
 * Size				S:  4 K |
 *
 */
#define M2M_BOOT_FIRMWARE_STARTING_ADDR		(FLASH_START_ADDR)
#define M2M_BOOT_FIRMWARE_FLASH_SZ			(FLASH_SECTOR_SZ)

/* Control Section: which used by Boot firmware
 * starting Address L:  4 K |
 * Size				S:  8 K |
 *
 */
#define M2M_CONTROL_FLASH_OFFSET			(M2M_BOOT_FIRMWARE_STARTING_ADDR + M2M_BOOT_FIRMWARE_FLASH_SZ)
#define M2M_CONTROL_FLASH_BKP_OFFSET		(M2M_CONTROL_FLASH_OFFSET + FLASH_SECTOR_SZ)
#define M2M_CONTROL_FLASH_SEC_SZ			(FLASH_SECTOR_SZ)
#define M2M_CONTROL_FLASH_TOTAL_SZ			(FLASH_SECTOR_SZ * 2)

/* LUT for PLL and TX Gain settings:
 * starting Address L: 12 K |
 * Size				S:  8 K |
 *
 */
#define M2M_PLL_FLASH_OFFSET				(M2M_CONTROL_FLASH_OFFSET + M2M_CONTROL_FLASH_TOTAL_SZ)
#define M2M_PLL_MAGIC_NUMBER_FLASH_SZ		(2*4)		// 2 32bit values
#define M2M_PLL_WIFI_CHAN_FLASH_OFFSET		(M2M_PLL_FLASH_OFFSET + M2M_PLL_MAGIC_NUMBER_FLASH_SZ)
#define M2M_PLL_WIFI_CHAN_FLASH_SZ			(14*8*4)	// Wifi Channels 1 to 14 inclusive, 8 32bit values for each channel.
#define M2M_PLL_FREQ_FLASH_OFFSET			(M2M_PLL_WIFI_CHAN_FLASH_OFFSET + M2M_PLL_WIFI_CHAN_FLASH_SZ)
#define M2M_PLL_FREQ_FLASH_SZ				((1+84)*4)	// Frequencies 2401 to 2484MHz inclusive, also 1920 used for cpll compensate.
#define M2M_PLL_FLASH_SZ					(1024 * 2)
#define M2M_GAIN_FLASH_OFFSET				(M2M_PLL_FLASH_OFFSET + M2M_PLL_FLASH_SZ)
#define M2M_GAIN_FLASH_SZ					(M2M_CONFIG_SECT_TOTAL_SZ - M2M_PLL_FLASH_SZ)
#define M2M_CONFIG_SECT_TOTAL_SZ			(FLASH_SECTOR_SZ * 2)

/* Certificate:
 * starting Address		L: 16 K |
 * Size for CERTIFICATE	S:  3 K |
 * Size  	RESERVED	S:  1 K |
 *
 */
#define M2M_TLS_FLASH_ROOTCERT_CACHE_OFFSET			(M2M_PLL_FLASH_OFFSET + M2M_CONFIG_SECT_TOTAL_SZ)
#define M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE			(FLASH_SECTOR_SZ * 1)

/* Scratch:
 * starting Address		L: 20 K |
 * Size for Scratch		S:  4 K |
 *
 */
#define M2M_TLS_FLASH_SESSION_CACHE_OFFSET		(M2M_TLS_FLASH_ROOTCERT_CACHE_OFFSET + M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE)
#define M2M_TLS_FLASH_SESSION_CACHE_SIZE		(FLASH_SECTOR_SZ * 1)

/*
 *
 * OTA image1 Offset
 */
#define M2M_OTA_IMAGE1_OFFSET					(M2M_TLS_FLASH_SESSION_CACHE_OFFSET + M2M_TLS_FLASH_SESSION_CACHE_SIZE)
/*
 * Firmware Offset
 *
 */


/**
 *
 * OTA image 2 offset
 */
#ifdef BT_IMAGE_PRESENT
#define M2M_OTA_IMAGE2_OFFSET				(512 * 1024UL)
#else
#define M2M_OTA_IMAGE2_OFFSET				(256 * 1024UL)
#endif

#if (defined _FIRMWARE_)||(defined OTA_GEN)
#define M2M_FIRMWARE_FLASH_OFFSET			(0UL)
#else
#if (defined DOWNLOAD_ROLLBACK)
#define M2M_FIRMWARE_FLASH_OFFSET			(M2M_OTA_IMAGE2_OFFSET)
#else
#define M2M_FIRMWARE_FLASH_OFFSET			(M2M_OTA_IMAGE1_OFFSET)
#endif
#endif
/*
 * To define Starting Address and size for Firmware
 *  ____________________ _______________________ ___________________________
 * |--------------------|	Power Save Program	|	Non Power Save	Program	|
 * |____________________|_______________________|___________________________|
 * |M2M_PROGRAM_FLASH_SZ| 	  	  2 K			|		  	  0 K			|
 * |Starting Address	|	  	  2 K			|		  	  0 K			|
 * |Size				|		194 K			|			196 K			|
 * |____________________|_______________________|___________________________|
 *
 */
#ifdef _PROGRAM_POWER_SAVE_
#define M2M_PROGRAM_FLASH_SZ				(1024 * 2UL)
#else
#define M2M_PROGRAM_FLASH_SZ				(0UL)
#endif	/* _PROGRAM_POWER_SAVE_ */
#define M2M_FIRMWARE_FLASH_SZ				(196*1024UL)

/* HTTP Files
 * starting Address L:204 K |
 * Size				S:  8 K |
 *
 */
#define M2M_HTTP_MEM_FLASH_OFFSET			(M2M_FIRMWARE_FLASH_OFFSET + M2M_FIRMWARE_FLASH_SZ)
#define M2M_HTTP_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 2)

/* ps_Firmware(Power Save Firmware): App. which runs for power saving purpose
 * starting Address L:212 K |
 * Size				S: 0 K |
 *
 */
#define M2M_PS_FIRMWARE_FLASH_OFFSET		(M2M_HTTP_MEM_FLASH_OFFSET + M2M_HTTP_MEM_FLASH_SZ)
#define M2M_PS_FIRMWARE_FLASH_SZ			(FLASH_SECTOR_SZ * 0)

/* Saved Connection Parameters:
 * starting Address L:224 K |
 * Size				S:  4 K |
 *
 */
#define M2M_CACHED_CONNS_FLASH_OFFSET		(M2M_PS_FIRMWARE_FLASH_OFFSET + M2M_PS_FIRMWARE_FLASH_SZ)
#define M2M_CACHED_CONNS_FLASH_SZ			(FLASH_SECTOR_SZ * 1)

/* App(Cortus App): App. which runs over firmware
 * starting Address L:232 K |
 * Size for Code 	S: 20 K |
 * Size for Data	S:  4 K |
 *
 */
#define M2M_APP_2M_MEM_FLASH_OFFSET			(M2M_CACHED_CONNS_FLASH_OFFSET + M2M_CACHED_CONNS_FLASH_SZ)
#define M2M_APP_2M_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 6)
#define M2M_APP_4M_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 10)
/* OTA image does not include APP */
#define M2M_APP_4M_MEM_FLASH_OFFSET			(FLASH_4M_TOTAL_SZ - M2M_APP_4M_MEM_FLASH_SZ)

// MERGEBUG: BT image requires 4M flash, but we should not otherwise need to assume flash size here.
#ifdef BT_IMAGE_PRESENT
/* BT(Bluetooth Firmware): BT/BLE firmware to run on the BT cortus
* starting Address L:255 K |
* Size 			   S:128 K |
*
* End Location = ( Starting Address + Total Size ) - 1 = ( 255 + 128 ) - 1  = 382 K
*/
#define M2M_BT_FIRMWARE_FLASH_OFFSET			(M2M_CACHED_CONNS_FLASH_OFFSET + M2M_CACHED_CONNS_FLASH_SZ)
#define M2M_BT_FIRMWARE_FLASH_SZ				(128*1024UL)
#else
#define M2M_BT_FIRMWARE_FLASH_OFFSET			(M2M_CACHED_CONNS_FLASH_OFFSET + M2M_CACHED_CONNS_FLASH_SZ)
#define M2M_BT_FIRMWARE_FLASH_SZ				(0)
#endif

#define M2M_TRANSPORTER_FLASH_OFFSET			(M2M_BT_FIRMWARE_FLASH_OFFSET + M2M_BT_FIRMWARE_FLASH_SZ)
#define M2M_TRANSPORTER_FLASH_SZ				(10*1024UL)


/**
 *
 * OTA image Size
 */
#define OTA_IMAGE_SIZE						(M2M_CACHED_CONNS_FLASH_SZ + M2M_HTTP_MEM_FLASH_SZ + M2M_FIRMWARE_FLASH_SZ \
											+ M2M_BT_FIRMWARE_FLASH_SZ + M2M_TRANSPORTER_FLASH_SZ)


// MERGEBUG: This definition does not work for 3400
/**
 *
 * Flash Total size

#define FLASH_IMAGE1_CONTENT_SZ 			(M2M_BOOT_FIRMWARE_FLASH_SZ + M2M_CONTROL_FLASH_TOTAL_SZ + M2M_CONFIG_SECT_TOTAL_SZ + \
											M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE + M2M_TLS_FLASH_SESSION_CACHE_SIZE + OTA_IMAGE_SIZE + \
											M2M_PS_FIRMWARE_FLASH_SZ + M2M_APP_2M_MEM_FLASH_SZ + M2M_BT_FIRMWARE_FLASH_SZ)

 */


#endif /* __SPI_FLASH_MAP_H__ */
