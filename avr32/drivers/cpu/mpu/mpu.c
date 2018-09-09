/*****************************************************************************
 *
 * \file
 *
 * \brief MPU driver.
 *
 * MPU (Memory Protection Unit) driver module for AVR32 devices.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "mpu.h"


/*!
 * Control of the MPU control register MPUCR: enable MPUCR
 */
void enable_mpu(void)
{
  Set_system_register(AVR32_MPUCR, AVR32_MPUCR_E_MASK);
}


/*!
 * Control of the MPU control register MPUCR: disable MPUCR
 */
void disable_mpu(void)
{
  Set_system_register(AVR32_MPUCR, 0);
}


/*!
 * \brief Converts an input region size expressed in kBytes to the corresponding
 *  eRegionSize type value.
 *
 * \param kBSizeValue: input region size expressed in kBytes
 * \param peRegionSizeValue: output region size in the eRegionSize type
 *
 * \return bool true if the conversion succeeded
 *              false if the conversion failed (the input is not a possible protected region size).
 */
bool mpu_convert_kbsize_to_eregionsize(eRegionSize *peRegionSizeValue, U32 kBSizeValue)
{
  U32 Log2kBSize;
  eRegionSize RegVal;

  // The size must be non-zero.
  if (!kBSizeValue)
    return false;

  // Compute the logarithm to base 2 of the size expressed in kB.
  Log2kBSize = 32 - (clz(kBSizeValue) + 1);

  // The size must be a power of 2.
  if (kBSizeValue != 1 << Log2kBSize)
    return false;

  // MPUARx.SIZE is (the logarithm to base 2 of the size expressed in bytes) - 1.
  RegVal = (eRegionSize)(10 + Log2kBSize - 1);

  // The size must be in the allowed range.
  if (RegVal <= MPU_REGION_SIZE_LOWLIMIT_FORBIDDEN ||
      RegVal >= MPU_REGION_SIZE_HIGHLIMIT_FORBIDDEN)
    return false;

  *peRegionSizeValue = RegVal;
  return true;
}


/*!
 * Setup a MPU entry
 * \param mpu_entry: pointer to mpu_entry_t with the MPU settings (base address, size, validity).
 * \param region_number: MPU entry region number (0..7).
 * \return int       MPU_SETUP_ENTRY_OK              if the setup succeeded
 *                   MPU_SETUP_ENTRY_INVALIDBASEADDR if the setup failed because the input base address is not aligned to the size of the region.
 *                   MPU_SETUP_ENTRY_INVALIDSIZE     if the setup failed because the input size is not one of the type eRegionSize.
 */
char set_mpu_entry(const mpu_entry_t *mpu_entry, unsigned int region_number)
{
  /* Set Address Register. */
  avr32_mpuar0_t ar;

  // Check the size: it must (be a power of 2) and (greater than or equal to 4kB) and (less than or equal to 4GB).
  // Based on Table 6.1 in doc32002 "AVR32UC Technical Reference Manual Complete".
  if( ( mpu_entry->size <= MPU_REGION_SIZE_LOWLIMIT_FORBIDDEN )
    ||( mpu_entry->size >= MPU_REGION_SIZE_HIGHLIMIT_FORBIDDEN ) )
    // ERROR: the input size is not of the type eRegionSize.
    return(MPU_SETUP_ENTRY_INVALIDSIZE);

  // Check the base address: it must be aligned to the size of the region.
  // This test is written carefully to also work for the 4-GB case.
  if(!Test_align(mpu_entry->addr, 2 << (U32)mpu_entry->size))
    // ERROR: the input base address is not aligned to the size of the region.
    return(MPU_SETUP_ENTRY_INVALIDBASEADDR);

  // Specify the 20 most significant bits of the start address.
  ar.base = mpu_entry->addr >> 12;
  // Size of this protected region; based on Table 5.1 in doc32000 (AVR32
  // Architecture Manual Complete).
  ar.size = mpu_entry->size;
  ar.v = mpu_entry->valid;

  /* Region entry */
  switch ( region_number & 0x7 )
  {
  default:
  case 0:
    Set_system_register(AVR32_MPUAR0, *((unsigned int *)&ar));
    break;
  case 1:
    Set_system_register(AVR32_MPUAR1, *((unsigned int *)&ar));
    break;
  case 2:
    Set_system_register(AVR32_MPUAR2, *((unsigned int *)&ar));
    break;
  case 3:
    Set_system_register(AVR32_MPUAR3, *((unsigned int *)&ar));
    break;
  case 4:
    Set_system_register(AVR32_MPUAR4, *((unsigned int *)&ar));
    break;
  case 5:
    Set_system_register(AVR32_MPUAR5, *((unsigned int *)&ar));
    break;
  case 6:
    Set_system_register(AVR32_MPUAR6, *((unsigned int *)&ar));
    break;
  case 7:
    Set_system_register(AVR32_MPUAR7, *((unsigned int *)&ar));
    break;
  }

  return(MPU_SETUP_ENTRY_OK);
}


/*!
 * Disable a MPU entry
 * \param region_number: MPU entry region number (0..7).
 * \param register_select: register A: '0' -- B: '1'
 */
void disable_mpu_entry(unsigned int region_number, unsigned int register_select)
{
  /* Set Address Register. */
  avr32_mpuar0_t ar;

  ar.base = 0;
  ar.size = 0;
  ar.v = 0;

  /* Region entry */
  switch ( region_number & 0x7 )
  {
  default:
  case 0:
    Set_system_register(AVR32_MPUAR0, *((unsigned int *)&ar));
    break;
  case 1:
    Set_system_register(AVR32_MPUAR1, *((unsigned int *)&ar));
    break;
  case 2:
    Set_system_register(AVR32_MPUAR2, *((unsigned int *)&ar));
    break;
  case 3:
    Set_system_register(AVR32_MPUAR3, *((unsigned int *)&ar));
    break;
  case 4:
    Set_system_register(AVR32_MPUAR4, *((unsigned int *)&ar));
    break;
  case 5:
    Set_system_register(AVR32_MPUAR5, *((unsigned int *)&ar));
    break;
  case 6:
    Set_system_register(AVR32_MPUAR6, *((unsigned int *)&ar));
    break;
  case 7:
    Set_system_register(AVR32_MPUAR7, *((unsigned int *)&ar));
    break;
  }
}


/*!
 * Setup a register A and B
 * \param region_number: MPU entry region number (0..7).
 * \param register_select: register A: '0' -- B: '1'
 * \param right_access: R/W/X see doc32002.pdf (Table 5-3. Access permissions implied by the APn bits)
 */
void set_access_permissions(unsigned int region_number, unsigned int register_select, unsigned int right_access)
{
  avr32_mpuapra_t mpu_regA;
  avr32_mpuaprb_t mpu_regB;

  *(U32 *)&mpu_regA = (U32) Get_system_register(AVR32_MPUAPRA);
  *(U32 *)&mpu_regB = (U32) Get_system_register(AVR32_MPUAPRB);

  /* Region entry */
  if (register_select==0) //Register A
  {
    switch ( region_number & 0x7 )
    {
    default:
    case 0:
      mpu_regA.ap0 = (right_access);
      break;
    case 1:
      mpu_regA.ap1  = (right_access);
      break;
    case 2:
      mpu_regA.ap2  = (right_access);
      break;
    case 3:
      mpu_regA.ap3  = (right_access);
      break;
    case 4:
      mpu_regA.ap4  = (right_access);
      break;
    case 5:
      mpu_regA.ap5  = (right_access);
      break;
    case 6:
      mpu_regA.ap6  = (right_access);
      break;
    case 7:
      mpu_regA.ap7  = (right_access);
      break;
    }
    /* Set permissions */
    Set_system_register(AVR32_MPUAPRA, *((unsigned int *)&mpu_regA));
  }
  else //Register B
  {
    switch ( region_number & 0x7 )
    {
    default:
    case 0:
      mpu_regB.ap0 = (right_access);
      break;
    case 1:
      mpu_regB.ap1  = (right_access);
      break;
    case 2:
      mpu_regB.ap2  = (right_access);
      break;
    case 3:
      mpu_regB.ap3  = (right_access);
      break;
    case 4:
      mpu_regB.ap4  = (right_access);
      break;
    case 5:
      mpu_regB.ap5  = (right_access);
      break;
    case 6:
      mpu_regB.ap6  = (right_access);
      break;
    case 7:
      mpu_regB.ap7  = (right_access);
      break;
    }
    /* Set permissions */
    Set_system_register(AVR32_MPUAPRB, *((unsigned int *)&mpu_regB));
  }
}


/*!
 * Setup a Subregion
 * \param region_number: MPU entry region number (0..7).
 * \param pattern_select: bit-field for the 16 subregions: register A: '0' -- B: '1'
 */
void select_subregion(unsigned int region_number, unsigned int pattern_select)
{
  /* Region entry */
  switch ( region_number & 0x7 )
  {
  default:
  case 0: Set_system_register(AVR32_MPUPSR0, pattern_select );break;
  case 1: Set_system_register(AVR32_MPUPSR1, pattern_select );break;
  case 2: Set_system_register(AVR32_MPUPSR2, pattern_select );break;
  case 3: Set_system_register(AVR32_MPUPSR3, pattern_select );break;
  case 4: Set_system_register(AVR32_MPUPSR4, pattern_select );break;
  case 5: Set_system_register(AVR32_MPUPSR5, pattern_select );break;
  case 6: Set_system_register(AVR32_MPUPSR6, pattern_select );break;
  case 7: Set_system_register(AVR32_MPUPSR7, pattern_select );break;
  }
}
