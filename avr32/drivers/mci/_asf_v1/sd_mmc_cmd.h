/**
 * \file
 *
 * \brief SD/MMC card command list.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _SD_MMC_CMD_H_
#define _SD_MMC_CMD_H_

/**
 * \defgroup group_avr32_drivers_mci_sd_mmc_cmd SD MMC Commands
 * \ingroup group_avr32_drivers_mci
 *
 * \{
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "compiler.h"
#include "mci.h"

//-- Class 0 & 1 commands: Basic commands and Read Stream commands
#define SD_MMC_GO_IDLE_STATE_CMD                 (0 | MCI_TRCMD_NO | MCI_SPCMD_NONE )
#define SD_MMC_INIT_STATE_CMD                    (0 | MCI_TRCMD_NO | MCI_SPCMD_INIT )
#define SD_MMC_MMC_GO_IDLE_STATE_CMD             (0 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_OPDCMD)
#define SD_MMC_MMC_SEND_OP_COND_CMD              (1 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_OPDCMD)
#define SD_MMC_ALL_SEND_CID_CMD                  (2 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_RSPTYP_136 )
#define SD_MMC_MMC_ALL_SEND_CID_CMD              (2 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_RSPTYP_136| MCI_OPDCMD)
#define SD_MMC_SET_RELATIVE_ADDR_CMD             (3 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_MAXLAT )
#define SD_MMC_MMC_SET_RELATIVE_ADDR_CMD         (3 | MCI_TRCMD_NO | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_MAXLAT | MCI_OPDCMD)

#define MCI_SET_DSR_CMD                          (4 | MCI_TRCMD_NO | SPCMD_NONE | MCI_RSPTYP_NO | MCI_MAXLAT )

#define SD_MMC_SEL_DESEL_CARD_CMD                (7 | MCI_TRCMD_NO     | MCI_SPCMD_NONE | MCI_RSPTYP_48B  | MCI_MAXLAT )
#define SD_MMC_SEND_EXT_CSD_CMD                  (8 | MCI_TRCMD_START  | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRDIR | MCI_MAXLAT )
#define SD_MMC_SEND_CSD_CMD                      (9 | MCI_TRCMD_NO     | MCI_SPCMD_NONE | MCI_RSPTYP_136 | MCI_MAXLAT )
#define SD_MMC_SEND_CID_CMD                      (10| MCI_TRCMD_NO     | MCI_SPCMD_NONE | MCI_RSPTYP_136 | MCI_MAXLAT )
#define SD_MMC_MMC_READ_DAT_UNTIL_STOP_CMD       (11| MCI_TRTYP_STREAM | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRDIR | MCI_TRCMD_START | MCI_MAXLAT )

#define SD_MMC_STOP_READ_TRANSMISSION_CMD        (12| MCI_TRCMD_STOP | MCI_SPCMD_NONE  | MCI_RSPTYP_48 | MCI_MAXLAT )
#define SD_MMC_STOP_WRITE_TRANSMISSION_CMD       (12| MCI_TRCMD_STOP | MCI_SPCMD_NONE  | MCI_RSPTYP_48B | MCI_MAXLAT )

#define SD_MMC_STOP_TRANSMISSION_SYNC_CMD        (12| MCI_TRCMD_STOP | MCI_SPCMD_SYNC  | MCI_RSPTYP_48 | MCI_MAXLAT )
#define SD_MMC_SEND_STATUS_CMD                   (13| MCI_TRCMD_NO   | MCI_SPCMD_NONE  | MCI_RSPTYP_48 | MCI_MAXLAT )
#define SD_MMC_GO_INACTIVE_STATE_CMD             (15| MCI_RSPTYP_NO )

//*------------------------------------------------
//* Class 2 commands: Block oriented Read commands
//*------------------------------------------------

#define SD_MMC_SET_BLOCKLEN_CMD                  (16 | MCI_TRCMD_NO   | MCI_SPCMD_NONE | MCI_RSPTYP_48   | MCI_MAXLAT )
#define SD_MMC_READ_SINGLE_BLOCK_CMD             (17 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_START | MCI_TRTYP_BLOCK    | MCI_TRDIR | MCI_MAXLAT)
#define SD_MMC_READ_MULTIPLE_BLOCK_CMD           (18 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_START | MCI_TRTYP_MULTIPLE | MCI_TRDIR | MCI_MAXLAT)

//*--------------------------------------------
//* Class 3 commands: Sequential write commands
//*--------------------------------------------

#define SD_MMC_MMC_WRITE_DAT_UNTIL_STOP_CMD      (20 | MCI_TRTYP_STREAM | MCI_SPCMD_NONE | MCI_RSPTYP_48 & ~(MCI_TRDIR) | MCI_TRCMD_START | MCI_MAXLAT )	// MMC

//*------------------------------------------------
//* Class 4 commands: Block oriented write commands
//*------------------------------------------------

#define SD_MMC_WRITE_BLOCK_CMD                   (24 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_START | (MCI_TRTYP_BLOCK    &  ~(MCI_TRDIR)) | MCI_MAXLAT)
#define SD_MMC_WRITE_MULTIPLE_BLOCK_CMD          (25 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_START | (MCI_TRTYP_MULTIPLE &  ~(MCI_TRDIR)) | MCI_MAXLAT)
#define SD_MMC_PROGRAM_CSD_CMD                   (27 | MCI_RSPTYP_48 )


//*----------------------------------------
//* Class 6 commands: Group Write protect
//*----------------------------------------

#define SD_MMC_SET_WRITE_PROT_CMD                (28 | MCI_RSPTYP_48)
#define SD_MMC_CLR_WRITE_PROT_CMD                (29 | MCI_RSPTYP_48)
#define SD_MMC_SEND_WRITE_PROT_CMD               (30 | MCI_RSPTYP_48)


//*----------------------------------------
//* Class 5 commands: Erase commands
//*----------------------------------------

#define SD_MMC_TAG_SECTOR_START_CMD              (32 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_TAG_SECTOR_END_CMD                (33 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_MMC_UNTAG_SECTOR_CMD              (34 | MCI_RSPTYP_48 )
#define SD_MMC_MMC_TAG_ERASE_GROUP_START_CMD     (35 | MCI_RSPTYP_48 )
#define SD_MMC_MMC_TAG_ERASE_GROUP_END_CMD       (36 | MCI_RSPTYP_48 )
#define SD_MMC_MMC_UNTAG_ERASE_GROUP_CMD         (37 | MCI_RSPTYP_48 )
#define SD_MMC_ERASE_CMD                         (38 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_NO | MCI_MAXLAT )

//*----------------------------------------
//* Class 7 commands: Lock commands
//*----------------------------------------

#define SD_MMC_LOCK_UNLOCK                       (42 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_NO | MCI_MAXLAT)  // no tested

//*-----------------------------------------------
// Class 8 commands: Application specific commands
//*-----------------------------------------------
#define SD_MMC_SD_SEND_IF_COND_CMD               (8 | MCI_TRCMD_NO     | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_MAXLAT )

#define SD_MMC_APP_CMD                           (55 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_GEN_CMD                           (56 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT) // no tested
#define MMC_SWITCH_CMD                           (6  | MCI_SPCMD_NONE | MCI_RSPTYP_48B | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_SWITCH_FUNC                           (6  | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_START | MCI_TRTYP_BLOCK | MCI_TRDIR | MCI_MAXLAT)
#define SD_MMC_SDCARD_SET_BUS_WIDTH_CMD          (6  | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_SDCARD_STATUS_CMD                 (13 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_SDCARD_SEND_NUM_WR_BLOCKS_CMD     (22 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_SDCARD_SET_WR_BLK_ERASE_COUNT_CMD (23 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_SDCARD_APP_OP_COND_CMD            (41 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO )
#define SD_MMC_SDCARD_SET_CLR_CARD_DETECT_CMD    (42 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)
#define SD_MMC_SDCARD_SEND_SCR_CMD               (51 | MCI_SPCMD_NONE | MCI_RSPTYP_48  | MCI_TRCMD_NO | MCI_MAXLAT)

#define SD_MMC_SDCARD_APP_ALL_CMD               ( MCI_SDCARD_SET_BUS_WIDTH_CMD +\
                                                  MCI_SDCARD_STATUS_CMD +\
                                                  MCI_SDCARD_SEND_NUM_WR_BLOCKS_CMD +\
                                                  MCI_SDCARD_SET_WR_BLK_ERASE_COUNT_CMD +\
                                                  MCI_SDCARD_APP_OP_COND_CMD +\
                                                  MCI_SDCARD_SET_CLR_CARD_DETECT_CMD +\
                                                  MCI_SDCARD_SEND_SCR_CMD)

//*----------------------------------------
//* Class 9 commands: IO Mode commands
//*----------------------------------------

#define SD_MMC_FAST_IO_CMD                       (39 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_MAXLAT)
#define SD_MMC_GO_IRQ_STATE_CMD                  (40 | MCI_SPCMD_NONE | MCI_RSPTYP_48 | MCI_TRCMD_NO | MCI_MAXLAT)

/**
 * \}
 */

#endif  // _SD_MMC_CMD_H_
