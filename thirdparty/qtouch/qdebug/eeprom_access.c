/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/******************************************************************************
 *
 * \file
 *
 *                        $FILE:  eeprom_access.c
 * \author                Microchip Technology Inc: http://www.microchip.com \n
 *                        Support and FAQ: https://www.microchip.com/support/
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/

#ifdef _DEBUG_INTERFACE_

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include "eeprom_access.h"
#include "nvm.h"

/*----------------------------------------------------------------------------
extern variables
----------------------------------------------------------------------------*/
extern uint8_t num_sensors;
extern sensor_t sensors[];

/*! \brief read_info_from_eeprom
 *
 * This function is used to read Info block from EEPROM
 *
 * \param 
 * \note Returns library version currently on chip
 */

uint16_t
read_info_from_eeprom (void)
{
  uint16_t rtnval;

  /* The first two bytes stored in EEPROM is the library version */
  rtnval = (uint16_t) (nvm_eeprom_read_byte (EEPROM_START_ADDRESS));
  rtnval |=
    (uint16_t) (nvm_eeprom_read_byte (EEPROM_START_ADDRESS + 1u)) << 8u;
  return rtnval;
}

/*! \brief write_info_to_eeprom
 *
 * This function is used to Save information from library and sign in the EEPROM
 *
 * \param 
 * \note Will write the current library version to EEPROM
 */
void
write_info_to_eeprom (uint16_t lib_version)
{
  nvm_eeprom_write_byte ((EEPROM_START_ADDRESS), (uint8_t) (lib_version));
  nvm_eeprom_write_byte ((EEPROM_START_ADDRESS + 1u),
			 (uint8_t) (lib_version >> 8u));
}

/*! \brief write_global_settings_to_eeprom
 *
 * This function is used to Save global parameter settings to EEPROM
 *
 * \param 
 * \note 
 */
void
write_global_settings_to_eeprom (void)
{
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 0u),
			 qt_config_data.qt_recal_threshold);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 1u),
			 qt_config_data.qt_di);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 2u),
			 qt_config_data.qt_drift_hold_time);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 3u),
			 qt_config_data.qt_max_on_duration);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 4u),
			 qt_config_data.qt_neg_drift_rate);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 5u),
			 qt_config_data.qt_pos_drift_rate);
  nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 6u),
			 qt_config_data.qt_pos_recal_delay);
}

/*! \brief write_sensor_settings_to_eeprom
 *
 * This function is used to Save sensor parameter settings to EEPROM
 *
 * \param 
 * \note 
 */
void
write_sensor_settings_to_eeprom (void)
{
  uint8_t i;
  for (i = 0u; i < num_sensors; i++)
    {
      nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 7u +
			      (i * 2u)), sensors[i].threshold);
      nvm_eeprom_write_byte ((EEPROM_LIB_CONFIG_START_ADDRESS + 8u +
			      (i * 2u)), sensors[i].type_aks_pos_hyst);
    }
}

#ifdef _QMATRIX_
/*! \brief write_burst_lenghts_to_eeprom
 *
 * This function is used to Save burst length settings to EEPROM
 *
 * \param 
 * \note 
 */
void
write_burst_lenghts_to_eeprom (void)
{
  uint16_t address =
    EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (num_sensors * 2u);
  uint8_t i;

  for (i = 0u; i < QT_NUM_CHANNELS; i++)
    {
      nvm_eeprom_write_byte ((address + i), qt_burst_lengths[i]);
    }
}
#endif
/*! \brief read_settings_from_eeprom
 *
 * This function is used to read settings from EEPROM
 *
 * \param 
 * \note  Data is stored in 'eeprom_gv_block[]' in the format shown above
 */
void
read_settings_from_eeprom (void)
{
  uint8_t i;
  /* Read Global configuration */
  qt_config_data.qt_recal_threshold = (recal_threshold_t)
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 0u);
  qt_config_data.qt_di =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 1u);
  qt_config_data.qt_drift_hold_time =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 2u);
  qt_config_data.qt_max_on_duration =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 3u);
  qt_config_data.qt_neg_drift_rate =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 4u);
  qt_config_data.qt_pos_drift_rate =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 5u);
  qt_config_data.qt_pos_recal_delay =
    nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 6u);
  /* Read Sensor specific configuration */
  for (i = 0u; i < num_sensors; i++)
    {
      sensors[i].threshold =
	nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 7u +
			      (i * 2u));
      sensors[i].type_aks_pos_hyst =
	nvm_eeprom_read_byte (EEPROM_LIB_CONFIG_START_ADDRESS + 8u +
			      (i * 2u));
    }
  /* Read burst length for all channels */
#ifdef _QMATRIX_
  uint16_t address =
    EEPROM_LIB_CONFIG_START_ADDRESS + 7u + (num_sensors * 2u);
  for (i = 0u; i < QT_NUM_CHANNELS; i++)
    {
      qt_burst_lengths[i] = nvm_eeprom_read_byte ((address + i));
    }
#endif
}


/* fix for m8515 */
#ifndef EEPE
#define EEPE EEWE
#endif
#endif
