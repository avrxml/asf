/**
* \file  D_XNv.h
*
* \brief D_XNv Driver Interface
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

#ifndef D_XNV_H
#define D_XNV_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

#if defined BOOTLOADER
typedef uint8_t D_XNv_Size_t;
#else
typedef uint16_t D_XNv_Size_t;
#endif

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

#if defined(TESTHARNESS)
#define D_XNV_SECTOR_SIZE             (4096uL)
#else
#define D_XNV_SECTOR_SIZE             (65536uL)
#endif

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Read bytes from the external NV.
    \param sector The sector to use (0..7)
    \param offset The offset to start reading at
    \param[out] pBuffer The start address of the buffer to store the read data to
    \param numberOfBytes The number of bytes to read
    \note offset+numberOfBytes must be <=0x10000 (meaning sector crossing is not permitted)
*/
void D_XNv_Read(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes);

/** Write bytes to the external NV.
    \param sector The sector to use (0..7)
    \param offset The offset to start writing to
    \param[in] pBuffer The start address of the buffer that contains the data to write
    \param numberOfBytes The number of bytes to write
    \note offset+numberOfBytes must be <=0x10000 (meaning sector crossing is not permitted)
*/
void D_XNv_Write(uint8_t sector, uint16_t offset, const uint8_t *pBuffer, D_XNv_Size_t numberOfBytes);

/** Erases a sector of the external NV.
    \note The erase operation can take very long, depending on the actual chip that is used.
          Typical erase times are 600-1500 ms. This function will return immediately, however.
          The next request to D_XNv will first wait until the erase has finished.
*/
void D_XNv_EraseSector(uint8_t sector);

/** Checks if the requested range is empty (containing all 0xFF).
    \param sector The sector to use (0..7)
    \param offset The start offset to check
    \param numberOfBytes The number of bytes to check
    \returns TRUE if the range is empty, FALSE otherwise.
    \note A special case is when numberOfBytes==0, which requests to check 0x10000 bytes
    \note Sector crossing is not permitted
*/
bool D_XNv_IsEmpty(uint8_t sector, uint16_t offset, D_XNv_Size_t numberOfBytes);

/** Compare bytes with contents of the external NV.
    \param sector The sector to use (0..7)
    \param offset The offset to start comparing with
    \param[in] pBuffer The start address of the buffer that contains the data to compare with
    \param numberOfBytes The number of bytes to compare
    \note offset+numberOfBytes must be <=0x10000 (meaning sector crossing is not permitted)
*/
bool D_XNv_IsEqual(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // D_XNV_H
