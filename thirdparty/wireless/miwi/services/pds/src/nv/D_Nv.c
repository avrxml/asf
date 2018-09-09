/**
* \file  D_Nv.c
*
* \brief NV component implementation.
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

#if PDS_ENABLE_WEAR_LEVELING 

/******************************************************************************
                   Includes section
******************************************************************************/
#include <D_Nv_Bindings.h>
#include <D_Nv.h>
#include <D_Nv_Init.h>
#include "nvm.h"
#include "common_nvm.h"
#include "compiler.h"
#include "system_interrupt.h"
#include "string.h"
#include "assert.h"

/******************************************************************************
                   Definitions section
******************************************************************************/
#define COMPID "D_Nv"

#ifdef  __IAR_SYSTEMS_ICC__
#pragma segment="D_NV_MEMORY"
#define D_NV_MEMORY_START  ((uint32_t)__sfb("D_NV_MEMORY"))
// Location of last memory element, NOT the first memory location after it
#define D_NV_MEMORY_END  ((uint32_t)__sfe("D_NV_MEMORY") - 1U)
#elif __GNUC__
#define D_NV_MEMORY_START (uint32_t)&__d_nv_mem_start
#define D_NV_MEMORY_END (((uint32_t)&__d_nv_mem_end) - 1U)
#else
  #error "Unsupported compiler"
#endif

/******************************************************************************
                   Extern section
******************************************************************************/
#ifdef __GNUC__
extern uint32_t __d_nv_mem_start;
extern uint32_t __d_nv_mem_end;
#endif

/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/
static bool CompareData(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
// The Callback function to handle system integrity checks
static void (*s_pfSystemCheckCallback)(void) = NULL;

/******************************************************************************
                   Implementations section
******************************************************************************/
/** Initializes the component -- checks that parameters are correct.
*/
void D_Nv_Init(void)
{
  // Ensure that the memory range starts on a page boundary
  assert ((D_NV_MEMORY_START % NVMCTRL_ROW_SIZE) == 0U);
  // Ensure that sector size is an integer number of pages
  assert((D_NV_SECTOR_SIZE % NVMCTRL_ROW_SIZE) == 0U);
  // Ensure that segment is as large as we need
  assert((D_NV_MEMORY_END - D_NV_MEMORY_START + 1U) == D_NV_MEMORY_SIZE);
}

/** The function to set system integrity check callback function */
void D_Nv_SetSystemIntegrityCheckFunction(void (*pf)(void))
{
  /* Set callback function */
  s_pfSystemCheckCallback = pf;
}

/** Read bytes from the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start reading at
    \param[out] pBuffer The start address of the buffer to store the read data to
    \param numberOfBytes The number of bytes to read
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
void D_Nv_Read(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;

  assert(address <= D_NV_MEMORY_END);
  assert((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  assert((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  memcpy(pBuffer, (uint8_t FLASH_PTR *)address, numberOfBytes);
}

/** Write bytes to the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start writing to
    \param[in] pBuffer The start address of the buffer that contains the data to write
    \param numberOfBytes The number of bytes to write
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
    \note pBuffer cannot be const because it is passed to HAL_FillFlashPageBuffer()
*/
void D_Nv_Write(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t  address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;
  uint16_t  pageOffset;
  uint32_t  pageStart;
  D_Nv_Size_t numberOfPageBytes;
  uint8_t  page_buf[NVMCTRL_PAGE_SIZE];
  enum status_code error_code = STATUS_OK;

  assert(numberOfBytes);
  assert(address <= D_NV_MEMORY_END);
  assert((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  assert((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  // Let's check the system integrity before accessing the flash chip to prevent NV corruption
  if (s_pfSystemCheckCallback != NULL)
  {
    s_pfSystemCheckCallback();
  }

  pageOffset = address % NVMCTRL_PAGE_SIZE;
  pageStart = address - pageOffset;
  numberOfPageBytes = MIN((NVMCTRL_PAGE_SIZE - pageOffset), numberOfBytes);

  memset (page_buf, 0xFF, NVMCTRL_PAGE_SIZE);
  memcpy (&page_buf[pageOffset], pBuffer, numberOfPageBytes);

  system_interrupt_enter_critical_section();
  do {
    error_code = nvm_write_buffer(pageStart, page_buf, NVMCTRL_PAGE_SIZE);
  } while (error_code == STATUS_BUSY);
  system_interrupt_leave_critical_section();

  numberOfBytes -= numberOfPageBytes;
  address += numberOfPageBytes;
  pBuffer += numberOfPageBytes;

  while (0U < numberOfBytes)
  {
    numberOfPageBytes = MIN(NVMCTRL_PAGE_SIZE, numberOfBytes);

    memset (page_buf, 0xFF, NVMCTRL_PAGE_SIZE);
    memcpy (&page_buf[pageOffset], pBuffer, numberOfPageBytes);

    system_interrupt_enter_critical_section();
    do {
      error_code = nvm_write_buffer(address, page_buf, NVMCTRL_PAGE_SIZE);
    } while (error_code == STATUS_BUSY);
    system_interrupt_leave_critical_section();

    numberOfBytes -= numberOfPageBytes;
    address += numberOfPageBytes;
    pBuffer += numberOfPageBytes;
  }
}

/** Erases a sector of the internal NV.
   \param sector The sector to erase (0..D_NV_SECTOR_COUNT)
*/
void D_Nv_EraseSector(uint8_t sector)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE;

  address &= ~(NVMCTRL_ROW_SIZE - 1);

  assert(address <= D_NV_MEMORY_END);

  for (uint8_t i = 0U; i < (D_NV_SECTOR_SIZE / NVMCTRL_ROW_SIZE); i++)
  {
    nvm_erase_row (address);
    address += NVMCTRL_ROW_SIZE;
  }

}

/** Compare bytes with contents of the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start comparing with
    \param[in] pBuffer If NULL then read buffer will be compared with 0xFF
     Otherwise it is the start address of the buffer that contains the data to compare with
    \param numberOfBytes The number of bytes to compare
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
static bool CompareData(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  uint32_t address = D_NV_MEMORY_START + (uint32_t)(sector - D_NV_FIRST_SECTOR) * D_NV_SECTOR_SIZE + offset;
  uint8_t  buffer[16U];

  assert(numberOfBytes);
  assert(address <= D_NV_MEMORY_END);
  assert((address + numberOfBytes) <= (D_NV_MEMORY_END + 1U));
  assert((offset + numberOfBytes) <= D_NV_SECTOR_SIZE);

  while(numberOfBytes)
  {
    uint16_t bytesToRead = MIN(sizeof(buffer), numberOfBytes);

    memcpy(buffer, (uint8_t FLASH_PTR *)address, bytesToRead);
    for (uint8_t i = 0U; i < bytesToRead; i++)
      if (buffer[i] != ((pBuffer != NULL) ? *(pBuffer++) : 0xFF))
        return false;

    numberOfBytes -= bytesToRead;
    address += bytesToRead;
  }

  return true;
}

/** Checks if the requested range is empty (containing all 0xFF).
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The start offset to check
    \param numberOfBytes The number of bytes to check
    \returns true if the range is empty, FALSE otherwise.
    \note Sector crossing is not permitted
*/
bool D_Nv_IsEmpty(uint8_t sector, uint16_t offset, D_Nv_Size_t numberOfBytes)
{
  return CompareData(sector, offset, NULL, numberOfBytes);
}

/** Compare bytes with contents of the internal NV.
    \param sector The sector to use (0..D_NV_SECTOR_COUNT)
    \param offset The offset to start comparing with
    \param[in] pBuffer The start address of the buffer that contains the data to compare with
    \param numberOfBytes The number of bytes to compare
    \note offset+numberOfBytes must be <= D_NV_SECTOR_SIZE (meaning sector crossing is not permitted)
*/
bool D_Nv_IsEqual(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes)
{
  return CompareData(sector, offset, pBuffer, numberOfBytes);
}

#endif 
