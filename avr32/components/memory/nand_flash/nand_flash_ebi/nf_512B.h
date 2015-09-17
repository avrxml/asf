/*****************************************************************************
 *
 * \file
 *
 * \brief Listing of 512-B NAND FLASH types.
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


#ifndef _NF_512B_H_
#define _NF_512B_H_

#define  NF_TYPE_512B_KNOWN   false // No static mode define by default

//! Configuration index in list
//!
#define  CONF_512B_F1_1    0
#define  CONF_512B_F1_2    1
#define  CONF_512B_F1_3    2
#define  CONF_512B_F2_1    3
#define  CONF_512B_F2_2    4
#define  CONF_512B_F2_3    5
#define  CONF_512B_F3_1    6
#define  CONF_512B_F3_2    7
#define  CONF_512B_F3_3    8
#define  CONF_512B_F3_4    9
#define  CONF_512B_F3_5    10
#define  CONF_512B_F3_6    11
#define  CONF_512B_F3_7    12
#define  CONF_512B_F3_8    13
#define  CONF_512B_F3_9    14
#define  CONF_512B_F3_10   15
#define  CONF_512B_F4_1    16
#define  CONF_512B_F4_2    17
#define  CONF_512B_F4_3    18
#define  CONF_512B_F4_4    19


#if (NF_AUTO_DETECT_512B==true)

//**********  LIST USED FOR GENERIC 512B DRIVER ************

//! List of configuration
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_conf  nf_list_conf[];
#  else
_CONST_TYPE_   St_nf_conf  nf_list_conf[]=
{
  { 1,1,0,1,1, (CLK_DFC_NFC_40MHz)>>5}  // CONF_512B_F1_1
, { 1,1,0,0,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F1_2
, { 1,1,0,0,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F1_3
, { 2,1,0,0,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F2_1
, { 2,1,0,0,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F2_2
, { 2,1,0,0,0, (CLK_DFC_NFC_16MHz)>>5}  // CONF_512B_F2_3
, { 2,2,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_1
, { 2,2,0,1,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F3_2
, { 2,4,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_3
, { 2,4,0,1,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_4
, { 2,4,0,1,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F3_5
, { 1,2,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_6
, { 1,2,0,1,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_7
, { 1,2,0,1,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F3_8
, { 1,4,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_9
, { 1,4,0,1,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F3_10
, { 0,0,0,1,1, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F4_1
, { 0,0,0,0,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F4_2
, { 0,0,0,1,0, (CLK_DFC_NFC_30MHz)>>5}  // CONF_512B_F4_3
, { 0,0,0,1,0, (CLK_DFC_NFC_24MHz)>>5}  // CONF_512B_F4_4
};
#endif


//! List of Nand Flash 512B
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_id    nf_list_id[];
#  else
_CONST_TYPE_   St_nf_id    nf_list_id[]=
{
//  Maker         ,ID    ,Conf               // References
  { M_ID_HYNIX    ,0x35  ,CONF_512B_F2_2 }   // HY27SS08561M
, { M_ID_HYNIX    ,0x36  ,CONF_512B_F2_3 }   // HY27SS08121M
, { M_ID_HYNIX    ,0x75  ,CONF_512B_F2_1 }   // HY27US08561M
, { M_ID_HYNIX    ,0x76  ,CONF_512B_F2_1 }   // HY27US08121M

, { M_ID_INFINEON ,0x66  ,CONF_512B_F1_1 }   // HYF31DS51280[4/5]BT[C/I]

, { M_ID_SAMSUNG  ,0x33  ,CONF_512B_F4_4 }   // K9F2808Q0B
//{ M_ID_SAMSUNG  ,0x35  ,CONF_512B_F3_6 }   // K9F5608Q0B, K9F5608R0D
, { M_ID_SAMSUNG  ,0x35  ,CONF_512B_F3_8 }   // K9F5608Q0C
//{ M_ID_SAMSUNG  ,0x36  ,CONF_512B_F3_9 }   // K9F1208Q0A, K9F1208Q0B
, { M_ID_SAMSUNG  ,0x36  ,CONF_512B_F3_2 }   // K9K1208Q0C

, { M_ID_SAMSUNG  ,0x73  ,CONF_512B_F4_1 }   // K9F2808U0B, K9F2808U0C
//{ M_ID_SAMSUNG  ,0x75  ,CONF_512B_F3_6 }   // K9F5608D0C, K9F5608U0B, K9F5608U0C, K9F5608U0D
, { M_ID_SAMSUNG  ,0x75  ,CONF_512B_F3_7 }   // K9F5608U0A
, { M_ID_SAMSUNG  ,0x76  ,CONF_512B_F4_2 }   // K9F1208R0C, K9F1208U0C
//{ M_ID_SAMSUNG  ,0x76  ,CONF_512B_F3_9 }   // K9F1208D0A, K9F1208D0B, K9F1208U0A, K9F1208U0B
//{ M_ID_SAMSUNG  ,0x76  ,CONF_512B_F3_10}   // K9F1208U0M
//{ M_ID_SAMSUNG  ,0x76  ,CONF_512B_F3_1 }   // K9K1208D0C, K9K1208U0C
, { M_ID_SAMSUNG  ,0x78  ,CONF_512B_F3_5 }   // K9K1G08Q0A
//{ M_ID_SAMSUNG  ,0x79  ,CONF_512B_F3_3 }   // K9K1G08U0A
, { M_ID_SAMSUNG  ,0x79  ,CONF_512B_F3_4 }   // K9K1G08U0M

, { M_ID_ST       ,0x33  ,CONF_512B_F1_3 }   // NAND128R3A
, { M_ID_ST       ,0x35  ,CONF_512B_F1_3 }   // NAND256R3A
, { M_ID_ST       ,0x36  ,CONF_512B_F1_3 }   // NAND512R3A
, { M_ID_ST       ,0x39  ,CONF_512B_F1_3 }   // NAND01GR3A
, { M_ID_ST       ,0x73  ,CONF_512B_F4_2 }   // NAND128W3A
, { M_ID_ST       ,0x75  ,CONF_512B_F1_2 }   // NAND256W3A
, { M_ID_ST       ,0x76  ,CONF_512B_F1_2 }   // NAND512W3A
, { M_ID_ST       ,0x79  ,CONF_512B_F1_2 }   // NAND01GW3A

, { M_ID_TOSHIBA  ,0x73  ,CONF_512B_F4_2 }   // TC58128AFT, TC58128AFTI
//{ M_ID_TOSHIBA  ,0x73  ,CONF_512B_F4_3 }   // TC58DAM72A1FT00, TC58DVM72A1FT00, TC58DVM72A1FTI0
, { M_ID_TOSHIBA  ,0x75  ,CONF_512B_F4_2 }   // TC58256AFT, TC58256AFTI
//{ M_ID_TOSHIBA  ,0x75  ,CONF_512B_F4_3 }   // TC582562AXB, TC58DAM82A1FT00, TC58DVM82A1FT00, TC58DVM82A1FTI0, TC58DVM82A1XBJ1
, { M_ID_TOSHIBA  ,0x76  ,CONF_512B_F4_2 }   // TC58512FT, TC58512FTI
//{ M_ID_TOSHIBA  ,0x76  ,CONF_512B_F4_3 }   // TC58DVM92A1FT00, TC58DWM92A2XGJ5, TC58DWM92A2XGJX, TC58DVM92A1FTI0
, { M_ID_TOSHIBA  ,0x79  ,CONF_512B_F4_2 }   // TH58100FT, TH58100FTI
//{ M_ID_TOSHIBA  ,0x79  ,CONF_512B_F4_3 }   // TC58DVG02A1FT00, TC58DVG02A1FTI0

// !! Nand FLash 512B 16 pages not supported by generic driver 512B
//{ M_ID_SAMSUNG  ,0x39  ,CONF_512B_F4_1 }   // K9F6408U0C
//{ M_ID_SAMSUNG  ,0xE6  ,CONF_512B_F4_1 }   // K9F6408Q0C
//{ M_ID_SAMSUNG  ,0xE6  ,CONF_512B_F4_3 }   // K9F6408U0B
//{ M_ID_TOSHIBA  ,0xE6  ,CONF_512B_F4_2 }   // TC58V64BFT, TC58V64BFTI
};
#  endif // (!defined _nfc_drv_c_)


//! List of link between device ID and number of block
//!
#  if (!defined _nfc_drv_c_)
extern _CONST_TYPE_   St_nf_link_id_block  nf_list_link_id_block[];
#  else
_CONST_TYPE_   St_nf_link_id_block  nf_list_link_id_block[]=
{
// !! Nand FLash 512B 32 pages
  { 0x33	,1 }
, { 0x35	,2 }
, { 0x36	,4 }
, { 0x39	,8 }  // Specific ST
, { 0x66	,4 }
, { 0x73	,1 }
, { 0x75	,2 }
, { 0x76	,4 }
, { 0x78 ,8 }
, { 0x79	,8 }
// !! Nand FLash 512B 16 pages not supported by generic driver 512B
//{ 0x39 ,1 }  // Specific Samsung
//{ 0xE6 ,1 }
};
#  endif // (!defined _nfc_drv_c_)


#else    // (NF_AUTO_DETECT_512B==true)


//**********  LIST USED FOR SINGLE 512B DRIVER ************
#  undef  NF_DEV_CONF   // In case of define by other listing (e.g. 2KB)

// LIST USED FOR SINGLE 512B DRIVER with 16 pages per block

#if    (defined NF_TYPE_K9F6408U0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x39
#  define NF_DEV_CONF   CONF_512B_F4_1
#  define NF_N_ZONES    1
#  define NF_SHIFT_BLOCK_PAGE          4  // (1<<n) size of block (16p), unit in pages

#elif  (defined NF_TYPE_K9F6408Q0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xE6
#  define NF_DEV_CONF   CONF_512B_F4_1
#  define NF_N_ZONES    1
#  define NF_SHIFT_BLOCK_PAGE          4  // (1<<n) size of block (16p), unit in pages

#elif  (defined NF_TYPE_K9F6408U0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0xE6
#  define NF_DEV_CONF   CONF_512B_F4_3
#  define NF_N_ZONES    1
#  define NF_SHIFT_BLOCK_PAGE          4  // (1<<n) size of block (16p), unit in pages

#elif  (defined NF_TYPE_TC58V64BFT) || (defined NF_TYPE_TC58V64BFTI)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0xE6
#  define NF_DEV_CONF   CONF_512B_F4_2
#  define NF_N_ZONES    1
#  define NF_SHIFT_BLOCK_PAGE          4  // (1<<n) size of block (16p), unit in pages


// LIST USED FOR SINGLE 512B DRIVER with 32 pages per block

#elif  (defined NF_TYPE_HY27SS08561M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0x35
#  define NF_DEV_CONF   CONF_512B_F2_2

#elif  (defined NF_TYPE_HY27SS08121M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0x36
#  define NF_DEV_CONF   CONF_512B_F2_3

#elif  (defined NF_TYPE_HY27US08561M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F2_1

#elif  (defined NF_TYPE_HY27US08121M)
#  define NF_DEV_MAKER  M_ID_HYNIX
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F2_1

#elif  (defined NF_TYPE_HYF31DS51280XBT)
#  define NF_DEV_MAKER  M_ID_INFINEON
#  define NF_DEV_ID     0x66
#  define NF_DEV_CONF   CONF_512B_F1_1

#elif  (defined NF_TYPE_K9F2808Q0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x33
#  define NF_DEV_CONF   CONF_512B_F4_4

#elif  (defined NF_TYPE_K9F5608Q0B) || (defined NF_TYPE_K9F5608R0D)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x35
#  define NF_DEV_CONF   CONF_512B_F3_6

#elif  (defined NF_TYPE_K9F5608Q0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x35
#  define NF_DEV_CONF   CONF_512B_F3_8

#elif  (defined NF_TYPE_K9F1208Q0A) || (defined NF_TYPE_K9F1208Q0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x36
#  define NF_DEV_CONF   CONF_512B_F3_9

#elif  (defined NF_TYPE_K9K1208Q0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x36
#  define NF_DEV_CONF   CONF_512B_F3_2

#elif  (defined NF_TYPE_K9F2808U0B) || (defined NF_TYPE_K9F2808U0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x73
#  define NF_DEV_CONF   CONF_512B_F4_1

#elif  (defined NF_TYPE_K9F5608D0C) || (defined NF_TYPE_K9F5608U0B) || (defined NF_TYPE_K9F5608U0C) || (defined NF_TYPE_K9F5608U0D)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F3_6

#elif  (defined NF_TYPE_K9F5608U0A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F3_7

#elif  (defined NF_TYPE_K9F1208R0C) || (defined NF_TYPE_K9F1208U0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_K9F1208D0A) || (defined NF_TYPE_K9F1208D0B) || (defined NF_TYPE_K9F1208U0A) || (defined NF_TYPE_K9F1208U0B)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F3_9

#elif  (defined NF_TYPE_K9F1208U0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F3_10

#elif  (defined NF_TYPE_K9K1208D0C) || (defined NF_TYPE_K9K1208U0C)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F3_1

#elif  (defined NF_TYPE_K9K1G08Q0A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x78
#  define NF_DEV_CONF   CONF_512B_F3_5

#elif  (defined NF_TYPE_K9K1G08U0A)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x79
#  define NF_DEV_CONF   CONF_512B_F3_3

#elif  (defined NF_TYPE_K9K1G08U0M)
#  define NF_DEV_MAKER  M_ID_SAMSUNG
#  define NF_DEV_ID     0x79
#  define NF_DEV_CONF   CONF_512B_F3_4

#elif  (defined NF_TYPE_NAND128R3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x33
#  define NF_DEV_CONF   CONF_512B_F1_3

#elif  (defined NF_TYPE_NAND256R3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x35
#  define NF_DEV_CONF   CONF_512B_F1_3

#elif  (defined NF_TYPE_NAND512R3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x36
#  define NF_DEV_CONF   CONF_512B_F1_3

#elif  (defined NF_TYPE_NAND01GR3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x39
#  define NF_DEV_CONF   CONF_512B_F1_3

#elif  (defined NF_TYPE_NAND128W3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x73
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_NAND256W3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F1_2

#elif  (defined NF_TYPE_NAND512W3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F1_2

#elif  (defined NF_TYPE_NAND01GW3A)
#  define NF_DEV_MAKER  M_ID_ST
#  define NF_DEV_ID     0x79
#  define NF_DEV_CONF   CONF_512B_F1_2

#elif  (defined NF_TYPE_TC58128AFT) || (defined NF_TYPE_TC58128AFTI)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x73
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_TC58DAM72A1FT00) || (defined NF_TYPE_TC58DVM72A1FT00) || (defined NF_TYPE_TC58DVM72A1FTI0)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x73
#  define NF_DEV_CONF   CONF_512B_F4_3

#elif  (defined NF_TYPE_TC58256AFT) || (defined NF_TYPE_TC58256AFTI)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_TC582562AXB) || (defined NF_TYPE_TC58DAM82A1FT00) || (defined NF_TYPE_TC58DVM82A1FT00) || (defined NF_TYPE_TC58DVM82A1FTI0) || (defined NF_TYPE_TC58DVM82A1XBJ1)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x75
#  define NF_DEV_CONF   CONF_512B_F4_3

#elif  (defined NF_TYPE_TC58512FT) || (defined NF_TYPE_TC58512FTI)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_TC58DVM92A1FT00) || (defined NF_TYPE_TC58DVM92A1FTI0) || (defined NF_TYPE_TC58DWM92A2XGJ5) || (defined NF_TYPE_TC58DWM92A2XGJX)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x76
#  define NF_DEV_CONF   CONF_512B_F4_3

#elif  (defined NF_TYPE_TH58100FT) || (defined NF_TYPE_TH58100FTI)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x79
#  define NF_DEV_CONF   CONF_512B_F4_2

#elif  (defined NF_TYPE_TC58DVG02A1FT00) || (defined NF_TYPE_TC58DVG02A1FTI0)
#  define NF_DEV_MAKER  M_ID_TOSHIBA
#  define NF_DEV_ID     0x79
#  define NF_DEV_CONF   CONF_512B_F4_3

#endif


#if (defined NF_DEV_CONF)

#  undef  NF_TYPE_512B_KNOWN
#  define NF_TYPE_512B_KNOWN  true     // A static mode define

   // Determination of number of block
#  if (!defined  NF_N_ZONES)
#  if   (NF_DEV_ID == 0x33)
#     define   NF_N_ZONES       1
#  elif (NF_DEV_ID == 0x73)
#     define   NF_N_ZONES       1
#  elif (NF_DEV_ID == 0x35)
#     define   NF_N_ZONES       2
#  elif (NF_DEV_ID == 0x75)
#     define   NF_N_ZONES       2
#  elif (NF_DEV_ID == 0x36)
#     define   NF_N_ZONES       4
#  elif (NF_DEV_ID == 0x66)
#     define   NF_N_ZONES       4
#  elif (NF_DEV_ID == 0x76)
#     define   NF_N_ZONES       4
#  elif (NF_DEV_ID == 0x39)   // Specific ST
#     define   NF_N_ZONES       8
#  elif (NF_DEV_ID == 0x78)
#     define   NF_N_ZONES       8
#  elif (NF_DEV_ID == 0x79)
#     define   NF_N_ZONES       8
#  else
#     error    BAD DEVICE ID of reference in nf_512B.h
#  endif
#  endif // (!defined  NF_N_ZONES)

#  define   NF_N_BLOCKS          (NF_N_ZONES*1024L)

   // Determination of number of row cycles
#  if   (NF_N_ZONES <= 2)
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
#  if    (NF_DEV_CONF == CONF_512B_F4_1)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F4_2)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F4_3)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F4_4)
#     define NF_COPYBACK_CONT    0
#     define NF_COPYBACK_DISCONT 0
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_512B_F1_1)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_40MHz
#  elif  (NF_DEV_CONF == CONF_512B_F1_2)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F1_3)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_6)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_7)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_8)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_9)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 4
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_10)
#     define NF_COPYBACK_CONT    1
#     define NF_COPYBACK_DISCONT 4
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F2_1)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F2_2)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_512B_F2_3)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 1
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           0
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_16MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_1)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_2)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 2
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_3)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 4
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      1
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_4)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 4
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_30MHz
#  elif  (NF_DEV_CONF == CONF_512B_F3_5)
#     define NF_COPYBACK_CONT    2
#     define NF_COPYBACK_DISCONT 4
#     define NF_CACHE_PROGRAM    0
#     define NF_CE_LOW           1
#     define NF_TIMING_READ      0
#     define CLK_DFC_NFC         CLK_DFC_NFC_24MHz
#  endif   // (defined CONF_512B_X)

#endif   // (defined NF_DEV_CONF) = static mode

#endif   // (NF_AUTO_DETECT_512B==true)


#if ((NF_AUTO_DETECT_512B == true) || (NF_TYPE_512B_KNOWN == true))
//**** HERE A 512B driver with 32 or 16 pages by block is used in generic or single mode ****
#  define NF_SHIFT_PAGE_BYTE           9  // (1<<n) size of page  (512B), unit in bytes
#  if (!defined NF_SHIFT_BLOCK_PAGE)
#  define NF_SHIFT_BLOCK_PAGE          5  // (1<<n) size of block (32p), unit in pages
#  endif // else is define at 16p
#endif

#endif	// _NF_512B_H_
