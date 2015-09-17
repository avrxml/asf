/*******************************************************************************
*   $FILE:  eeprom_access.c
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/*  License
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
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
#include "touch_config.h"

#ifdef _DEBUG_INTERFACE_

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include "eeprom_access_mega.h"

/*----------------------------------------------------------------------------
prototypes
----------------------------------------------------------------------------*/
static void eeprom_write(uint16_t ui_address, uint8_t ui_data);
static uint8_t eeprom_read(uint16_t ui_address);

/*----------------------------------------------------------------------------
macros
----------------------------------------------------------------------------*/
#if defined(_ATXMEGA_DEVICE_)
   #define NVM_EXEC() asm( "push r30" "\n\t"	\
                           "push r31"      "\n\t"	\
                           "push r16"      "\n\t"	\
                           "push r18"      "\n\t"	\
                           "ldi r30, 0xCB" "\n\t"	\
                           "ldi r31, 0x01" "\n\t"	\
                           "ldi r16, 0xD8" "\n\t"	\
                           "ldi r18, 0x01" "\n\t"	\
                           "out 0x34, r16" "\n\t"	\
                           "st Z, r18"     "\n\t"	\
                           "pop r18"       "\n\t"	\
                           "pop r16"       "\n\t"	\
                           "pop r31"       "\n\t"	\
                           "pop r30"       "\n\t"	\
                          )
#else
   #define EEPROM_WRITE() asm(   "push r24"      "\n\t"  \
                                 "ldi r24, 0x04" "\n\t"  \
                                 "out 0x1F, r24" "\n\t"  \
                                 "ldi r24, 0x02" "\n\t"  \
                                 "out 0x1F, r24" "\n\t"  \
                                 "pop r24"       "\n\t"  \
                              )
#endif /* defined(_ATXMEGA_DEVICE_) */

/*----------------------------------------------------------------------------
extern variables
----------------------------------------------------------------------------*/
extern uint8_t num_sensors;
extern sensor_t sensors[];

/*==============================================================================
Name    : read_info_from_eeprom
--------------------------------------------------------------------------------
Purpose : Read Info block from EEPROM
Input   :
Output  :
Notes   : Returns library version currently on chip
==============================================================================*/
uint16_t read_info_from_eeprom( void )
{
   uint16_t rtnval;

   /* The first two bytes stored in EEPROM is the library version */
   rtnval = (uint16_t)(eeprom_read(EEPROM_START_ADDRESS));
   rtnval |= (uint16_t)(eeprom_read(EEPROM_START_ADDRESS + 1u)) << 8u;
   return rtnval;
}

/*==============================================================================
Name    : write_info_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save information from library and sign in the EEPROM
Input   :
Output  :
Notes   : Will write the current library version to EEPROM
==============================================================================*/
void write_info_to_eeprom( uint16_t lib_version )
{
   eeprom_write((EEPROM_START_ADDRESS), (uint8_t)(lib_version));
   eeprom_write((EEPROM_START_ADDRESS + 1u), (uint8_t)(lib_version >> 8u));
}

/*==============================================================================
Name    : write_global_settings_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save global parameter settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_global_settings_to_eeprom(void)
{
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 0u),
                qt_config_data.qt_recal_threshold);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 1u),
                qt_config_data.qt_di);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 2u),
                qt_config_data.qt_drift_hold_time);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 3u),
                qt_config_data.qt_max_on_duration);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 4u),
                qt_config_data.qt_neg_drift_rate);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 5u),
                qt_config_data.qt_pos_drift_rate);
   eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 6u),
                qt_config_data.qt_pos_recal_delay);
}

/*==============================================================================
Name    : write_sensor_settings_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save sensor parameter settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_sensor_settings_to_eeprom(void)
{
   uint8_t i;
   for(i = 0u; i < num_sensors; i++)
   {
      eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (i * 2u)),
                   sensors[i].threshold);
      eeprom_write((EEPROM_LIB_CONFIG_START_ADDRESS + 8u + (i * 2u)),
                   sensors[i].type_aks_pos_hyst);
   }
}

#ifdef _QMATRIX_
/*==============================================================================
Name    : write_burst_lenghts_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save burst length settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_burst_lenghts_to_eeprom(void)
{
   uint16_t address = EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (num_sensors * 2u);
   uint8_t i;

   for(i = 0u; i < QT_NUM_CHANNELS; i++)
   {
      eeprom_write((address + i), qt_burst_lengths[i]);
   }
}
#endif

/*==============================================================================
Name    : read_settings_from_eeprom
--------------------------------------------------------------------------------
Purpose : Will read settings from EEPROM
Input   :
Output  :
Notes   : Data is stored in 'eeprom_gv_block[]' in the format shown above
==============================================================================*/
void read_settings_from_eeprom(void)
{
   uint8_t i;
   /* Read Global configuration */
   qt_config_data.qt_recal_threshold =(recal_threshold_t)
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 0u);
   qt_config_data.qt_di              =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 1u);
   qt_config_data.qt_drift_hold_time =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 2u);
   qt_config_data.qt_max_on_duration =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 3u);
   qt_config_data.qt_neg_drift_rate  =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 4u);
   qt_config_data.qt_pos_drift_rate  =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 5u);
   qt_config_data.qt_pos_recal_delay =
      eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 6u);
   /* Read Sensor specific configuration */
   for(i = 0u; i < num_sensors; i++)
   {
      sensors[i].threshold =
         eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (i * 2u));
      sensors[i].type_aks_pos_hyst =
         eeprom_read(EEPROM_LIB_CONFIG_START_ADDRESS + 8u + (i * 2u));
   }
   /* Read burst length for all channels */
   #ifdef _QMATRIX_
      uint16_t address = EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (num_sensors * 2u);
      for(i = 0u; i < QT_NUM_CHANNELS; i++)
      {
         qt_burst_lengths[i] = eeprom_read((address + i));
      }
   #endif
}


/* fix for m8515 */
#ifndef EEPE
#define EEPE EEWE
#endif

/*==============================================================================
Name    : eeprom_write
--------------------------------------------------------------------------------
Purpose : Write data to EEPROM
Input   : Address where the data must be saved, data
Output  :
Notes   :
==============================================================================*/
void eeprom_write(uint16_t ui_address, uint8_t ui_data)
{
#if !defined(_ATXMEGA_DEVICE_)
   /* FOR TINY / MEGA EEPROM ACCESS */
   /* Wait for completion of previous write */
   while(EECR & (1<<EEPE));
   /* Set up address and Data Registers */
   #if defined ( __ICCAVR__ )
      EEAR = ui_address;
   #elif defined( __GNUC__ ) /* defined ( __ICCAVR__ ) */
      EEARL = ui_address;
   #endif /* defined( __GNUC__ ) */
   EEDR = ui_data;
   EEPROM_WRITE();
#else
   /* FOR XMEGA EEPROM ACCESS */
   /* Wait for pending NVM operation */
   do{} while(( NVM.STATUS & NVM_NVMBUSY_bm ) == NVM_NVMBUSY_bm );
   if ((NVM.STATUS & NVM_EELOAD_bm) != 0) {
      /* Command - Erase eeprom buffer */
      NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
      /* Command - Execute */
      NVM_EXEC();
   }
   /* Command - Load EEPROM buffer */
   NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
   /* Load address to write to. */
   NVM.ADDR0 = ui_address & 0xFF;
   NVM.ADDR1 = (ui_address >> 8) & 0x1F;
   NVM.ADDR2 = 0x00;
   /* Load data to write */
   NVM.DATA0 = ui_data;
   /* Load Atomic write command */
   NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
   NVM_EXEC();
   /* Wait for pending NVM operation */
   do{} while(( NVM.STATUS & NVM_NVMBUSY_bm ) == NVM_NVMBUSY_bm );
#endif
}

/*==============================================================================
Name    : eeprom_read
--------------------------------------------------------------------------------
Purpose : Reads data from EEPROM
Input   : EEPROM address where the data must be read
Output  : 8 Bit data
Notes   :
==============================================================================*/
uint8_t eeprom_read(uint16_t ui_address)
{
#if !defined(_ATXMEGA_DEVICE_)
   /* FOR TINY / MEGA EEPROM ACCESS */
   /* Wait for completion of previous write */
   while(EECR & (1<<EEPE));
   /* Set up address register */
   #if defined ( __ICCAVR__ )
      EEAR = ui_address;
   #elif defined( __GNUC__ ) /* defined ( __ICCAVR__ ) */
      EEARL = ui_address;
   #endif /* defined( __GNUC__ ) */
   /* Start eeprom read by writing EERE */
   EECR |= (1<<EERE);
   /* Return data from Data Register */
   return EEDR;
#else
   /* FOR XMEGA EEPROM ACCESS */
   /* Wait for pending NVM operation */
   do{} while(( NVM.STATUS & NVM_NVMBUSY_bm ) == NVM_NVMBUSY_bm );
   /* Load address for reading */
   NVM.ADDR0 = ui_address & 0xFF;
   NVM.ADDR1 = (ui_address >> 8) & 0x1F;
   NVM.ADDR2 = 0x00;
   /* Command - Read EEPROM */
   NVM.CMD = NVM_CMD_READ_EEPROM_gc;
   /* Command - Execute */
   NVM_EXEC();
   /* Data available */
   return NVM.DATA0;
#endif
}
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
