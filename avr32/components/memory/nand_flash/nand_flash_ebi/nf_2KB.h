/*****************************************************************************
 *
 * \file
 *
 * \brief Listing of 2-kB NAND FLASH types.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _NF_2KB_H_
#define _NF_2KB_H_

#define  NF_TYPE_2KB_KNOWN    false // No static mode define by default

//! Configuration index in list
//!
#define  CONF_2KB_F1_1     0
#define  CONF_2KB_F1_2     1
#define  CONF_2KB_F1_3     2
#define  CONF_2KB_F1_4     3
#define  CONF_2KB_F1_5     4
#define  CONF_2KB_F1_6     5
#define  CONF_2KB_F1_7     6
#define  CONF_2KB_F1_8     7
#define  CONF_2KB_F2_1     8
#define  CONF_2KB_F2_2     9
#define  CONF_2KB_F2_3     10
#define  CONF_2KB_F2_4     11
#define  CONF_2KB_F2_5     12
#define  CONF_2KB_F3_1     13
#define  CONF_2KB_F3_2     14
#define  CONF_2KB_F3_3     15
#define  CONF_2KB_F3_4     16
#define  CONF_2KB_F4_0     17
#define  CONF_2KB_F4_1     18


#if (NF_AUTO_DETECT_2KB==true)

//**********  LIST USED FOR GENERIC 2KB DRIVER ************

//! List of configuration
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_conf  nf_list_conf[];
#  else
_CONST_TYPE_   St_nf_conf  nf_list_conf[]=
{
  { 1,1,1,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F1_1
, { 1,1,1,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F1_2
, { 1,1,1,1,1, (CLK_DFC_NFC_24MHz)>>5}  // CONF_2KB_F1_3
, { 1,1,1,0,1, (CLK_DFC_NFC_24MHz)>>5}  // CONF_2KB_F1_4
, { 1,1,1,1,0, (CLK_DFC_NFC_16MHz)>>5}  // CONF_2KB_F1_5
, { 1,1,0,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F1_6
, { 1,1,0,1,1, (CLK_DFC_NFC_40MHz)>>5}  // CONF_2KB_F1_7
, { 1,1,1,0,1, (CLK_DFC_NFC_16MHz)>>5}  // CONF_2KB_F1_8
, { 2,1,1,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F2_1
, { 2,1,1,1,1, (CLK_DFC_NFC_40MHz)>>5}  // CONF_2KB_F2_2
, { 2,1,1,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F2_2
, { 2,1,1,1,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_2KB_F2_3
, { 4,1,1,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F2_4
, { 4,1,1,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F2_5
, { 1,2,1,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F3_1
, { 1,2,0,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F3_2
, { 1,2,0,1,1, (CLK_DFC_NFC_40MHz)>>5}  // CONF_2KB_F3_3
, { 2,2,0,1,1, (CLK_DFC_NFC_60MHz)>>5}  // CONF_2KB_F3_4
, { 0,0,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F4_0
, { 0,0,0,0,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_2KB_F4_1
};
#endif


//! List of Nand Flash 2KB
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_id    nf_list_id[];
#  else
_CONST_TYPE_   St_nf_id    nf_list_id[]=
{
//  Maker         ,ID    ,Conf            // References
  { M_ID_HYNIX    ,0xA1  ,CONF_2KB_F1_3 } // HY27SF081G2M
, { M_ID_HYNIX    ,0xAA  ,CONF_2KB_F1_3 } // HY27SG082G2M
, { M_ID_HYNIX    ,0xD3  ,CONF_2KB_F2_5 } // Configuration compatible for all references
//{ M_ID_HYNIX    ,0xD3  ,CONF_2KB_F1_2 } // HY27UH088G2M
//{ M_ID_HYNIX    ,0xD3  ,CONF_2KB_F2_4 } // HY27UH08AG5M, HY27UH08AGDM
, { M_ID_HYNIX    ,0xDA  ,CONF_2KB_F1_3 } // HY27UG082G2M
//{ M_ID_HYNIX    ,0xDA  ,CONF_2KB_F1_2 } // HY27UG084GDM
, { M_ID_HYNIX    ,0xDC  ,CONF_2KB_F2_2 } // Configuration compatible for all references
//{ M_ID_HYNIX    ,0xDC  ,CONF_2KB_F1_2 } // HY27UG084G2M, HY27UH088GDM
//{ M_ID_HYNIX    ,0xDC  ,CONF_2KB_F2_1 } // HY27UF084G2M
, { M_ID_HYNIX    ,0xF1  ,CONF_2KB_F1_4 } // HY27UF081G2M

, { M_ID_MICRON   ,0xA1  ,CONF_2KB_F1_2 } // MT29F1G08ABAWP
, { M_ID_MICRON   ,0xAA  ,CONF_2KB_F1_2 } // MT29F2G08ABCWG, MT29F2G08ABCWP
, { M_ID_MICRON   ,0xAC  ,CONF_2KB_F2_2 } // MT29F4G08BBCWG
, { M_ID_MICRON   ,0xD3  ,CONF_2KB_F3_1 } // MT29F8G08BAAWP, MT29F16G08FAAWP, MT29F16G08FAAWC
, { M_ID_MICRON   ,0xDA  ,CONF_2KB_F1_1 } // MT29F2G08AABWP, MT29F2G08AACWG, MT29F2G08AACWP
//{ M_ID_MICRON   ,0xDC  ,CONF_2KB_F3_1 } // MT29F4G08AAAWP, MT29F8G08DAAWP, MT29F8G08DAAWC (Not supported in generic driver)
, { M_ID_MICRON   ,0xDC  ,CONF_2KB_F2_1 } // MT29F4G08BABWP, MT29F8G08FABWP
//{ M_ID_MICRON   ,0xDC  ,CONF_2KB_F1_1 } // MT29F4G08BACWG, MT29F8G08FACWG

//{ M_ID_SAMSUNG  ,0xA1  ,CONF_2KB_F1_5 } // K9F1G08Q0M (Not supported in generic driver)
, { M_ID_SAMSUNG  ,0xA1  ,CONF_2KB_F4_0 } // K9F1G08R0A, K9F1G08Q0A, K9F1G08R0B
//{ M_ID_SAMSUNG  ,0xAA  ,CONF_2KB_F2_2 } // K9K2G08Q0M
, { M_ID_SAMSUNG  ,0xAA  ,CONF_2KB_F4_0 } // K9K2G08R0B
//{ M_ID_SAMSUNG  ,0xAA  ,CONF_2KB_F1_1 } // K9F2G08Q0M
, { M_ID_SAMSUNG  ,0xAC  ,CONF_2KB_F2_2 } // K9K4G08Q0M
, { M_ID_SAMSUNG  ,0xD3  ,CONF_2KB_F3_4 } // K9K8G08U0A, K9K8G08U0M, K9WAG08U1A, K9NBG08U5A
, { M_ID_SAMSUNG  ,0xDA  ,CONF_2KB_F4_0 } // Configuration compatible for all references
//{ M_ID_SAMSUNG  ,0xDA  ,CONF_2KB_F3_3 } // K9F2G08R0A, K9F2G08U0A
//{ M_ID_SAMSUNG  ,0xDA  ,CONF_2KB_F2_2 } // K9K2G08U0M, K9W4G08U1M
//{ M_ID_SAMSUNG  ,0xDA  ,CONF_2KB_F1_2 } // K9F2G08U0M
, { M_ID_SAMSUNG  ,0xDC  ,CONF_2KB_F4_0 } // Configuration compatible for all references
//{ M_ID_SAMSUNG  ,0xDC  ,CONF_2KB_F3_2 } // K9F4G08U0A, K9F4G08U0M, K9K8G08U1A
//{ M_ID_SAMSUNG  ,0xDC  ,CONF_2KB_F2_2 } // K9K4G08U0M, K9W8G08U1M
, { M_ID_SAMSUNG  ,0xF1  ,CONF_2KB_F1_7 } // Configuration compatible for all references
//{ M_ID_SAMSUNG  ,0xF1  ,CONF_2KB_F1_1 } // K9F1G08U0A
//{ M_ID_SAMSUNG  ,0xF1  ,CONF_2KB_F1_2 } // K9F1G08D0M, K9F1G08U0M
//{ M_ID_SAMSUNG  ,0xF1  ,CONF_2KB_F1_6 } // K9F1G08U0B

, { M_ID_ST       ,0xA1  ,CONF_2KB_F1_8 } // NAND01GR3B2
, { M_ID_ST       ,0xA3  ,CONF_2KB_F2_5 } // NAND08GR3B2
//{ M_ID_ST       ,0xAA  ,CONF_2KB_F1_2 } // NAND02GR3B2_one_die
, { M_ID_ST       ,0xAA  ,CONF_2KB_F2_2 } // NAND02GR3B2_two_die
, { M_ID_ST       ,0xAC  ,CONF_2KB_F2_2 } // NAND04GR3B2
, { M_ID_ST       ,0xD3  ,CONF_2KB_F2_1 } // NAND08GW3B2
//{ M_ID_ST       ,0xDA  ,CONF_2KB_F1_2 } // NAND02GW3B2_one_die
, { M_ID_ST       ,0xDA  ,CONF_2KB_F2_3 } // NAND02GW3B2_two_die
, { M_ID_ST       ,0xDC  ,CONF_2KB_F2_1 } // NAND04GW3B2
, { M_ID_ST       ,0xF1  ,CONF_2KB_F1_8 } // NAND01GW3B2

, { M_ID_TOSHIBA96,0xF1  ,CONF_2KB_F4_1 } // TC58NVG0S3AFT05
, { M_ID_TOSHIBA  ,0xF1  ,CONF_2KB_F4_1 } // TC58NVG0S3BFTI0
, { M_ID_TOSHIBA  ,0xDA  ,CONF_2KB_F4_1 } // TH58NVG1S3AFT05
};
#  endif // (!defined _nfc_drv_c_)


//! List of link between device ID and number of block
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_link_id_block  nf_list_link_id_block[];
#  else
_CONST_TYPE_   St_nf_link_id_block  nf_list_link_id_block[]=
{
  { 0xA1 ,1 }
, { 0xF1 ,1 }
, { 0xAA ,2 }
, { 0xDA ,2 }
, { 0xDC ,4 }
, { 0xAC ,4 }
, { 0xD3 ,8 }
, { 0xA3 ,8 }
};
#  endif // (!defined _nfc_drv_c_)


#else    // (NF_AUTO_DETECT_2KB==true)

//**********  LIST USED FOR SINGLE 2KB DRIVER ************
#  undef  NF_DEV_CONF   // In case of define by other listing (e.g. 512B)

#if    (defined NF_TYPE_HY27SF081G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xA1
#  define NF_DEV_CONF   CONF_2KB_F1_3

#elif  (defined NF_TYPE_HY27UF081G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F1_4

#elif  (defined NF_TYPE_HY27SG082G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F1_3

#elif  (defined NF_TYPE_HY27UG082G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F1_3

#elif  (defined NF_TYPE_HY27UG084GDM)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_HY27UF084G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F2_1

#elif  (defined NF_TYPE_HY27UG084G2M) || (defined NF_TYPE_HY27UH088GDM)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_HY27UH08AG5M) || (defined NF_TYPE_HY27UH08AGDM)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xD3
#  define NF_DEV_CONF   CONF_2KB_F2_4

#elif  (defined NF_TYPE_HY27UH088G2M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0xD3
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_MT29F1G08ABAWP)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xA1
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_MT29F2G08AABWP) || (defined NF_TYPE_MT29F2G08AACWG) || (defined NF_TYPE_MT29F2G08AACWP)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F1_1

#elif  (defined NF_TYPE_MT29F2G08ABCWG) || (defined NF_TYPE_MT29F2G08ABCWP)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_MT29F4G08AAAWP) || (defined NF_TYPE_MT29F8G08DAAWP) || (defined NF_TYPE_MT29F8G08DAAWC)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F3_1

#elif  (defined NF_TYPE_MT29F4G08BABWP) || (defined NF_TYPE_MT29F8G08FABWP)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F2_1

#elif  (defined NF_TYPE_MT29F4G08BBCWG)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xAC
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_MT29F4G08BACWG) || (defined NF_TYPE_MT29F8G08FACWG)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F1_1

#elif  (defined NF_TYPE_MT29F8G08BAAWP) || (defined NF_TYPE_MT29F16G08FAAWP) || (defined NF_TYPE_MT29F16G08FAAWC)
#  define NF_DEV_MAKER  M_ID_MICRON
#  define NF_DEV_ID     0xD3
#  define NF_DEV_CONF   CONF_2KB_F3_1

#elif  (defined NF_TYPE_K9F1G08R0A) || (defined NF_TYPE_K9F1G08Q0A) || (defined NF_TYPE_K9F1G08R0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xA1
#  define NF_DEV_CONF   CONF_2KB_F4_0

#elif  (defined NF_TYPE_K9F1G08U0A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F1_1

#elif  (defined NF_TYPE_K9F1G08D0M) || (defined NF_TYPE_K9F1G08U0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F1_2
#define toto

#elif  (defined NF_TYPE_K9F1G08Q0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xA1
#  define NF_DEV_CONF   CONF_2KB_F1_5

#elif  (defined NF_TYPE_K9F1G08U0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F1_6

#elif  (defined NF_TYPE_K9F2G08R0A) || (defined NF_TYPE_K9F2G08U0A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F3_3

#elif  (defined NF_TYPE_K9K2G08Q0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_K9K2G08U0M) || (defined NF_TYPE_K9W4G08U1M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_K9K2G08R0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F4_0

#elif  (defined NF_TYPE_K9F2G08Q0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F1_1

#elif  (defined NF_TYPE_K9F2G08U0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_K9F4G08U0A) || (defined NF_TYPE_K9F4G08U0M) || (defined NF_TYPE_K9K8G08U1A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F3_2

#elif  (defined NF_TYPE_K9K4G08Q0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xAC
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_K9K4G08U0M) || (defined NF_TYPE_K9W8G08U1M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_K9K8G08U0A) || (defined NF_TYPE_K9K8G08U0M) || (defined NF_TYPE_K9WAG08U1A) || (defined NF_TYPE_K9NBG08U5A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xD3
#  define NF_DEV_CONF   CONF_2KB_F3_4

#elif  (defined NF_TYPE_NAND01GR3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xA1
#  define NF_DEV_CONF   CONF_2KB_F1_8

#elif  (defined NF_TYPE_NAND01GW3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F1_8

#elif  (defined NF_TYPE_NAND02GR3B2_two_die)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_NAND02GW3B2_two_die)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F2_3

#elif  (defined NF_TYPE_NAND02GW3B2_one_die)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_NAND02GR3B2_one_die)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xAA
#  define NF_DEV_CONF   CONF_2KB_F1_2

#elif  (defined NF_TYPE_NAND04GW3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xDC
#  define NF_DEV_CONF   CONF_2KB_F2_1

#elif  (defined NF_TYPE_NAND04GR3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xAC
#  define NF_DEV_CONF   CONF_2KB_F2_2

#elif  (defined NF_TYPE_NAND08GR3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xA3
#  define NF_DEV_CONF   CONF_2KB_F2_5

#elif  (defined NF_TYPE_NAND08GW3B2)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0xD3
#  define NF_DEV_CONF   CONF_2KB_F2_1

#elif  (defined NF_TYPE_TC58NVG0S3AFT05)
#  define NF_DEV_MAKER  M_ID_TOSHIBA96
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F4_1

#elif  (defined NF_TYPE_TC58NVG0S3BFTI0)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0xF1
#  define NF_DEV_CONF   CONF_2KB_F4_1

#elif  (defined NF_TYPE_TH58NVG1S3AFT05)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0xDA
#  define NF_DEV_CONF   CONF_2KB_F4_1

#endif


#if (defined NF_DEV_CONF)

#  undef  NF_TYPE_2KB_KNOWN
#  define NF_TYPE_2KB_KNOWN  true     // A static mode define

   // Determination of number of block
#  if   (NF_DEV_ID == 0xA1)
#     define   NF_N_ZONES       1
#  elif (NF_DEV_ID == 0xF1)
#     define   NF_N_ZONES       1
#  elif (NF_DEV_ID == 0xAA)
#     define   NF_N_ZONES       2
#  elif (NF_DEV_ID == 0xDA)
#     define   NF_N_ZONES       2
#  elif (NF_DEV_ID == 0xDC)
#     define   NF_N_ZONES       4
#  elif (NF_DEV_ID == 0xAC)
#     define   NF_N_ZONES       4
#  elif (NF_DEV_ID == 0xD3)
#     define   NF_N_ZONES       8
#  elif (NF_DEV_ID == 0xA3)
#     define   NF_N_ZONES       8
#  else
#     error    BAD DEVICE ID of reference in nf_2KB.h
#  endif

#  define   NF_N_BLOCKS          (NF_N_ZONES*1024L)

   // Determination of number of row cycles
#  if   (NF_N_ZONES == 1)
#     define   NF_N_ROW_CYCLES   2
#  else
#     define   NF_N_ROW_CYCLES   3
#  endif

   // Select configuration
/*
#  define NF_COPYBACK_CONT       // 0 = copy back not supported, N = number of    CONTINUE subdivision contraint on copyback
#  define NF_COPYBACK_DISCONT    // 0 = copy back not supported, N = number of DISCONTINUE subdivision contraint on copyback
#  define NF_CACHE_PROGRAM       // 1 = cache program supported, 0 = not supported
#  define NF_CE_LOW              // 1 = CE toggle supported, 0 = not supported - CE must be always LOW
#  define NF_TIMING_READ         // mode 1: 1 cycle high, 1 cycle low for read cycle
                                 // mode 0: 1.5 cycle high, 0.5 cycle low for read cycle
#  define CLK_DFC_NFC            // Freq max supported,  CLK_DFC_NFC_xxMHz
*/
#  if    (NF_DEV_CONF == CONF_2KB_F1_1)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_2)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_3)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_4)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_5)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_16MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_6)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F1_8)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_16MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F3_1)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F3_2)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F3_3)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_40MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F2_1)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F2_2)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F2_3)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F3_4)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F2_4)
#     define NF_COPYBACK_CONT    4
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_60MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F2_5)
#     define NF_COPYBACK_CONT    4
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    1
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F4_0)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_2KB_F4_1)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  endif   // (defined CONF_2KB_X)

#endif   // (defined NF_DEV_CONF) = static mode

#endif   // (NF_AUTO_DETECT_2KB==true)


#if ((NF_AUTO_DETECT_2KB == true) || (NF_TYPE_2KB_KNOWN == true))
//**** HERE A 2KB driver with 64pages by block is used in generic or single mode ****
#  define NF_SHIFT_PAGE_BYTE          11  // (1<<n) size of page  (2KB), unit in bytes
#  define NF_SHIFT_BLOCK_PAGE          6  // (1<<n) size of block (64p), unit in pages
#endif

#endif	// _NF_2KB_H_
