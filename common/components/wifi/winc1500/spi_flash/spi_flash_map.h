/**
 *
 * \file
 *
 * \brief WINC1500 SPI Flash.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

/**
*  @file		spi_flash_map.h
*  @brief		This module contains spi flash CONTENT
*/
#ifndef __SPI_FLASH_MAP_H__
#define __SPI_FLASH_MAP_H__


//#define DOWNLOAD_ROLLBACK
//#define OTA_GEN
#define _PROGRAM_POWER_SAVE_

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
#define FLASH_TOTAL_SZ						(256 * 1024UL)
#define FLASH_2M_TOTAL_SZ					(256 * 1024UL)
#define FLASH_4M_TOTAL_SZ					(512 * 1024UL)
#define FLASH_8M_TOTAL_SZ					(1024 * 1024UL)
/*!<Total Size of Flash Memory
 */


/*
 * Detailed Sizes and locations for Flash Memory:
 *  ____________________ ___________ ___________________________ _______________________________________________
 * | Starting Address	|	Size	|	Location's Name			|	Description						   			|
 * |____________________|___________|___________________________|_______________________________________________|
 * |	  0 K  			|	  4	K	| 	Boot Firmware			|	Firmware to select which version to run		|
 * |	  4	K 			|	  8 K	|	Control Section			|	Structured data used by Boot firmware		|
 * |	 12 K			|     4	K	|	PLL+GAIN :				|	LookUp Table for PLL and Gain calculations	|
 * |	  				|     			PLL  Size =1K		|		PLL				 						|
 * |	  				|     		|	GAIN Size =3K		|		Gain configuration				 		|
 * |	 16	K			|	  4	K	|	CERTIFICATE				|	X.509 Certificate storage					|
 * |	 20 K			|	  2	K	|	Program Firmware		|												|
 * |	 22	K			|	194 K 	|	Main Firmware			|	Main Firmware to run WiFi Chip				|
 * |	216	K			|	  8	K	|	HTTP Files				|	Files used with Provisioning Mode			|
 * |	224	K			|	  0 K	|	PS_Firmware				|	Power Save Firmware							|
 * |	224	K			|	  4	K	|	Connection Parameters	|	Parameters for success connection to AP		|
 * |					|	  		|	 						|		at runtime								|
 * |	228	K			|	  4	K	|	Scratch Section			|	Empty Section								|
 * |	232	K			|	 24	K	|	Application				|	Cortus App									|
 * |____________________|___________|___________________________|_______________________________________________|
 *
 * *End of Memory at Location 	= ( Total Size ) - 1
 * 								= ( 4 + 8 + 1 + 3 + 2 + 182 + 4  + 8 + 12 + 4 + 4 + 24 ) - 1
 * 								= 255 K
 *
 * *Keys for Comments with each MACRO:
 * 		"L:xxxK" -means-> location 	:xxxK
 *		"S:xxxK" -means-> Size is 	:xxxK
 */

/* Boot Firmware: which used to select which firmware to run
 * starting Address L:  0 K |
 * Size				S:  4 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 0 + 4 ) - 1  = 3 K
 */
#define M2M_BOOT_FIRMWARE_STARTING_ADDR			(FLASH_START_ADDR)
#define M2M_BOOT_FIRMWARE_FLASH_SZ				(FLASH_SECTOR_SZ) 									/*S:  4K*/

/* Control Section: which used by Boot firmware
 * starting Address L:  4 K |
 * Size				S:  8 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 4 + 8 ) - 1  = 11 K
 */
#define M2M_CONTROL_FLASH_OFFSET			(M2M_BOOT_FIRMWARE_STARTING_ADDR + M2M_BOOT_FIRMWARE_FLASH_SZ)
#define M2M_CONTROL_FLASH_BKP_OFFSET		(M2M_CONTROL_FLASH_OFFSET + FLASH_SECTOR_SZ)
#define M2M_CONTROL_FLASH_SEC_SZ			(FLASH_SECTOR_SZ)
#define M2M_CONTROL_FLASH_TOTAL_SZ			(FLASH_SECTOR_SZ * 2)									/*S:  8K*/

/* LUT for PLL and TX Gain settings:
 * starting Address L: 12 K |
 * Size				S:  4 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 12 + 4 ) - 1  = 15 K
 */
#define M2M_PLL_FLASH_OFFSET				(M2M_CONTROL_FLASH_OFFSET + M2M_CONTROL_FLASH_TOTAL_SZ)	/*L: 12K*/
#define M2M_PLL_FLASH_SZ					(1024 * 1)												/*S:  1k*/
#define M2M_GAIN_FLASH_OFFSET				(M2M_PLL_FLASH_OFFSET + M2M_PLL_FLASH_SZ)				/*L: 13K*/
#define M2M_GAIN_FLASH_SZ					(M2M_CONFIG_SECT_TOTAL_SZ - M2M_PLL_FLASH_SZ)			/*S:  3K*/
#define M2M_CONFIG_SECT_TOTAL_SZ			(FLASH_SECTOR_SZ)										/*S:  4k*/

/* Certificate:
 * starting Address		L: 16 K |
 * Size for CERTIFICATE	S:  3 K |
 * Size  	RESERVED	S:  1 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 16 + 4 ) -1  = 19 K
 */
#define M2M_TLS_FLASH_CACHE_OFFSET			(M2M_PLL_FLASH_OFFSET + M2M_CONFIG_SECT_TOTAL_SZ)			/*L: 16K*/
#define M2M_TLS_FLASH_CACHE_SZ				(FLASH_SECTOR_SZ * 1)										/*S:  4K*/
#define M2M_TLS_FLASH_SESSION_CACHE_SIZE	(1 * (1024UL))												/*S:  1K*/
#define M2M_TLS_FLASH_ROOTCERT_CACHE_SIZE	(M2M_TLS_FLASH_CACHE_SZ - M2M_TLS_FLASH_SESSION_CACHE_SIZE)	/*S:  3K*/

#define M2M_TLS_FLASH_CACHE_SCRATCH_OFFSET	(M2M_TLS_FLASH_CACHE_OFFSET + M2M_TLS_FLASH_CACHE_SZ)										/*L:248K*/
#define M2M_TLS_FLASH_CACHE_SCRATCH_SZ		(FLASH_SECTOR_SZ * 1)										/*S:  4K*/

#define M2M_OTA_IMAGE1						(M2M_TLS_FLASH_CACHE_SCRATCH_OFFSET + M2M_TLS_FLASH_CACHE_SCRATCH_SZ)
#define M2M_OTA_IMAGE2						(256*1024UL)

#if (defined _FIRMWARE_)||(defined OTA_GEN)
#define M2M_FIRMWARE_FLASH_OFFSET			(0UL)
#else
#if (defined DOWNLOAD_ROLLBACK)
#define M2M_FIRMWARE_FLASH_OFFSET			(M2M_OTA_IMAGE2)
#else
#define M2M_FIRMWARE_FLASH_OFFSET			(M2M_OTA_IMAGE1)
#endif
#endif
/*
 * To define Starting Address and size for Firmware
 *  ____________________ _______________________ ___________________________
 * |--------------------|	Power Save Program	|	Non Power Save	Program	|
 * |____________________|_______________________|___________________________|
 * |M2M_PROGRAM_FLASH_SZ| 	  	  2 K			|		  	  0 K			|
 * |Starting Address	|	  	  2 K			|		  	  0 K			|
 * |Size				|		182 K			|			184 K			|
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
 * End Location = ( Starting Address + Total Size ) - 1 = ( 204 + 8 ) - 1  = 211 K
 */
//#define M2M_USER_MEM_FLASH_STARTING_ADDR	(0UL)
#define M2M_HTTP_MEM_FLASH_OFFSET			(M2M_FIRMWARE_FLASH_OFFSET + M2M_FIRMWARE_FLASH_SZ)
#define M2M_HTTP_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 2) 										/*S:  8K*/

/* ps_Firmware(Power Save Firmware): App. which runs for power saving purpose
 * starting Address L:212 K |
 * Size				S: 12 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 212 + 12 ) - 1  = 223 K
 */
#define M2M_PS_FIRMWARE_FLASH_OFFSET		(M2M_HTTP_MEM_FLASH_OFFSET + M2M_HTTP_MEM_FLASH_SZ) 		/*L:212*/
#define M2M_PS_FIRMWARE_FLASH_SZ			(FLASH_SECTOR_SZ * 0)										/*S: 12K*/

/* Saved Connection Parameters:
 * starting Address L:224 K |
 * Size				S:  4 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 224 + 4 ) -1  = 227 K
 */
#define M2M_CACHED_CONNS_FLASH_OFFSET		(M2M_PS_FIRMWARE_FLASH_OFFSET + M2M_PS_FIRMWARE_FLASH_SZ)					/*L:244K*/
#define M2M_CACHED_CONNS_FLASH_SZ			(FLASH_SECTOR_SZ * 1)										/*S:  4K*/



/* App(Cortus App): App. which runs over firmware
 * starting Address L:232 K |
 * Size for Code 	S: 20 K |
 * Size for Data	S:  4 K |
 *
 * End Location = ( Starting Address + Total Size ) - 1 = ( 232 + 24 ) - 1  = 255 K
 */
#define M2M_APP_2M_MEM_FLASH_OFFSET			(M2M_CACHED_CONNS_FLASH_OFFSET + M2M_CACHED_CONNS_FLASH_SZ) 		/*L:192K*/
#define M2M_APP_2M_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 6)

#define M2M_APP_4M_MEM_FLASH_SZ				(FLASH_SECTOR_SZ * 10)
#define M2M_APP_4M_MEM_FLASH_OFFSET			(FLASH_4M_TOTAL_SZ - M2M_APP_4M_MEM_FLASH_SZ)


#define FLASH_CONTENT_SZ (M2M_BOOT_FIRMWARE_FLASH_SZ + M2M_FIRMWARE_FLASH_SZ + \
	 M2M_APP_2M_MEM_FLASH_SZ + M2M_PLL_FLASH_SZ + M2M_PS_FIRMWARE_FLASH_SZ)


/* Check if total size of content
 *  don't exceed total size of memory allowed
 **/
#if (FLASH_CONTENT_SZ > FLASH_TOTAL_SZ)
#error "Excced Flash Size"
#endif /* (FLASH_CONTENT_SZ > FLASH_TOTAL_SZ) */


#endif /* __SPI_FLASH_MAP_H__ */
