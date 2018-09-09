/*****************************************************************************
 *
 * \file
 *
 * \brief Audio Interface SD/MMC Card Services.
 *
 * This function should be connected to the audio interface to give access to the SD/MMC Card audio player
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


#include "conf_audio_player.h"
#include "audio_interface.h"
#include "board.h"
#include "gpio.h"
#if defined(SUPPORT_SD_MMC_MCI) && SUPPORT_SD_MMC_MCI == true
  #include "sd_mmc_mci.h"
  #include "conf_sd_mmc_mci.h"
#else
  #include "sd_mmc_spi.h"
#endif
#include "ctrl_access.h"
#include "fat.h"
#include "ai_usb_ms.h"

static ai_device_status_t ai_sd_mmc_get_device_status(ai_async_status_t *cmd_ai_status)
{
  *cmd_ai_status = CMD_DONE;

#if defined(SD_MMC_CARD_DETECT_PIN)
  if (gpio_get_pin_value(SD_MMC_CARD_DETECT_PIN))
#endif
#if defined(SUPPORT_SD_MMC_MCI) && SUPPORT_SD_MMC_MCI == true
  if (sd_mmc_mci_mem_check(SD_SLOT))
  {
    volatile uint32_t card_size;
    sd_mmc_mci_read_capacity(SD_SLOT, (uint32_t *) &card_size);
    return AI_DEVICE_STATUS_CONNECTED;
  }
#else
  if (sd_mmc_spi_check_presence() == true)
    return AI_DEVICE_STATUS_CONNECTED;
#endif

  return AI_DEVICE_STATUS_NOT_PRESENT;
}

static uint8_t ai_sd_mmc_nav_drive_nb(ai_async_status_t *status)
{
  *status = CMD_DONE;
  return 1;
}

static bool ai_sd_mmc_nav_drive_set(uint8_t n, ai_async_status_t *status)
{
  extern void ai_usb_ms_init_drive(void);
  bool s;
  *status = CMD_DONE;
  // Notify the audio interface layer that a new connection is performed
  ai_usb_ms_new_connection();
  ai_usb_ms_init_drive();
  s = nav_drive_set(LUN_ID_2);
  if (!s)
  {
    *status = CMD_ERROR;
    return false;
  }
  return true;
}

static uint8_t ai_sd_mmc_nav_drive_get(ai_async_status_t *status)
{
  *status = CMD_DONE;
  return 0;
}

const ai_fct_set ai_sd_mmc_fct_set =
{
  ai_usb_ms_init,
  ai_sd_mmc_get_device_status,
  ai_get_product_id_null,
  ai_get_vendor_id_null,
  ai_get_serial_number_null,
  ai_sd_mmc_nav_drive_nb,
  ai_sd_mmc_nav_drive_set,
  ai_sd_mmc_nav_drive_get,
  ai_usb_ms_nav_drive_mount,
  ai_usb_ms_nav_drive_total_space,
  ai_usb_ms_nav_drive_free_space,
  ai_usb_ms_nav_dir_root,
  ai_usb_ms_nav_dir_cd,
  ai_usb_ms_nav_dir_gotoparent,
  ai_usb_ms_nav_file_isdir,
  ai_usb_ms_nav_file_next,
  ai_usb_ms_nav_file_previous,
  ai_usb_ms_nav_file_goto,
  ai_usb_ms_nav_file_pos,
  ai_usb_ms_nav_file_nb,
  ai_usb_ms_nav_dir_nb,
  ai_usb_ms_nav_playlist_nb,
  ai_usb_ms_nav_dir_name,
  ai_usb_ms_nav_file_name,
  ai_usb_ms_nav_file_info_type,
  ai_usb_ms_nav_file_info_version,
  ai_usb_ms_nav_file_info_title,
  ai_usb_ms_nav_file_info_artist,
  ai_usb_ms_nav_file_info_album,
  ai_usb_ms_nav_file_info_year,
  ai_usb_ms_nav_file_info_track,
  ai_usb_ms_nav_file_info_genre,
  ai_usb_ms_nav_file_info_duration,
  ai_usb_ms_nav_file_info_image,
  ai_usb_ms_nav_getplayfile,
  ai_usb_ms_audio_nav_playfile,
  ai_usb_ms_audio_context_save,
  ai_usb_ms_audio_context_restore,
  ai_usb_ms_audio_nav_next,
  ai_usb_ms_audio_nav_previous,
  ai_usb_ms_audio_nav_eof_occur,
  ai_usb_ms_audio_nav_nb,
  ai_usb_ms_audio_nav_getpos,
  ai_usb_ms_audio_nav_setpos,
  ai_usb_ms_audio_nav_repeat_get,
  ai_usb_ms_audio_nav_repeat_set,
  ai_usb_ms_audio_nav_expmode_get,
  ai_usb_ms_audio_nav_expmode_set,
  ai_usb_ms_audio_nav_shuffle_get,
  ai_usb_ms_audio_nav_shuffle_set,
  ai_usb_ms_audio_nav_getname,
  ai_usb_ms_audio_nav_file_info_type,
  ai_usb_ms_audio_nav_file_info_version,
  ai_usb_ms_audio_nav_file_info_title,
  ai_usb_ms_audio_nav_file_info_artist,
  ai_usb_ms_audio_nav_file_info_album,
  ai_usb_ms_audio_nav_file_info_year,
  ai_usb_ms_audio_nav_file_info_track,
  ai_usb_ms_audio_nav_file_info_genre,
  ai_usb_ms_audio_nav_file_info_duration,
  ai_usb_ms_audio_nav_file_info_image,
  ai_usb_ms_audio_ctrl_stop,
  ai_usb_ms_audio_ctrl_resume,
  ai_usb_ms_audio_ctrl_pause,
  ai_usb_ms_audio_ctrl_time,
  ai_usb_ms_audio_ctrl_status,
  ai_usb_ms_audio_ctrl_ffw,
  ai_usb_ms_audio_ctrl_frw,
  ai_usb_ms_audio_ctrl_start_ffw,
  ai_usb_ms_audio_ctrl_start_frw,
  ai_usb_ms_audio_ctrl_stop_ffw_frw,
  ai_specific_async_cmd_task_null
};
