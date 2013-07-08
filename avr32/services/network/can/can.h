/*****************************************************************************
 *
 * \file
 *
 * \brief CAN Software Stack for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2009-2011 Atmel Corporation. All rights reserved.
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

#ifndef _CAN_H_
#define _CAN_H_

/**
 * \defgroup group_avr32_services_network_can CAN software stack
 *
 * CAN software stack for 32-bit AVR that supports initializing the CAN bus and sending/receiving data.
 *
 * \{
 */

//_____ I N C L U D E S ________________________________________________________
#include "conf_can.h"
#include "canif.h"
#include "compiler.h"
#include "preprocessor.h"
//_____ D E F I N I T I O N S __________________________________________________

// ----------
//! This constant is used as return value for "can_cmd" function.
#define CAN_CMD_REFUSED          0xFF
// ----------
//!This constant is used as return value for "can_cmd" function.
#define CAN_CMD_ACCEPTED         0x00
// ----------
//! This constant is used as return value for "can_get_status" function.
#define CAN_STATUS_COMPLETED     0x00
// ----------
//! This constant is used as return value for "can_get_status" function.
#define CAN_STATUS_NOT_COMPLETED 0x01
// ----------
//! This constant is used as return value for "can_get_status" function.
#define CAN_STATUS_ERROR         0x02
// ----------
//! This constant is used as return value for "can_get_status" function.
#define CAN_STATUS_WAKEUP        0x03
// ----------
//! This constant is used as return value for "can_get_status" function.
#define CAN_STATUS_BUSOFF        0x04
// ----------
//! This constant is used for MOB Allocation status
#define CAN_MOB_NOT_ALLOCATED    0xFF
// ----------
//! This constant is used for MOB Initialization request
#define CAN_DATA_FRAME              0
//! This constant is used for MOB Initialization request
#define CAN_REMOTE_FRAME           1

typedef struct{
  U8 handle;
  can_msg_t *can_msg;
  U8 dlc;
  U8 req_type;
  U8 status;
}can_mob_t;
//_____ D E C L A R A T I O N S ________________________________________________


/*! \brief Initialize CAN channel
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param can_msg_ram_add Address of the location of MOB Ram Buffer
 *  \param operating_mode  Selection between CAN_CHANNEL_MODE_NORMAL or CAN_CHANNEL_MODE_LISTENING or CAN_CHANNEL_MODE_LOOPBACK
 *  \param can_msg_callback  Callback for hardware interrupt
 *  \return U8 CAN_CMD_REFUSED or CAN_CMD_ACCEPTED
 */
extern U8 can_init( U8 ch,
                    U32 can_msg_ram_add,
                    U8 operating_mode,
                    void (*can_msg_callback) (U8,U8));

/*! \brief Allocate one MOB in a specific CAN Channel
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \return U8 CAN_CMD_REFUSED or the number of the MOB selected
 */
extern U8 can_mob_alloc(U8 ch);

/*! \brief Free one specific MOB in a specific CAN Channel
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \return U8 CAN_CMD_REFUSED or CAN_CMD_ACCEPTED
 */
extern U8 can_mob_free(U8 ch,U8 handle);

/*! \brief Start Transmission
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \param dlc Datalength
 *  \param req_type CAN_DATA_FRAME or CAN_REMOTE_FRAME
 *  \param can_msg CAN Message
 *  \return U8 CAN_CMD_REFUSED or CAN_CMD_ACCEPTED
 */
extern U8 can_tx( U8 ch,
                  U8 handle,
                  U8 dlc,
                  U8 req_type,
                  const can_msg_t *can_msg);

/*! \brief Start Reception
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \param req_type CAN_DATA_FRAME or CAN_REMOTE_FRAME
 *  \param can_msg CAN Message
 *  \return U8 CAN_CMD_REFUSED or CAN_CMD_ACCEPTED
 */
extern U8 can_rx( U8 ch,
                  U8 handle,
                  U8 req_type,
                  const can_msg_t *can_msg);


/*! \brief Get Mob data from a selected MOB
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \return Union64 See compiler.h file for type return definition
 */
extern Union64 can_get_mob_data( U8 ch ,
                                 U8 handle);

/*! \brief Get Mob dlc from a selected MOB
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \return U8 Return the DLC
 */
extern U8 can_get_mob_dlc( U8 ch ,
                           U8 handle);

/*! \brief Get Mob ID from a selected MOB
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \return U32 Return the ID
 */
extern U32 can_get_mob_id( U8 ch ,
                           U8 handle);

/*! \brief Get Mob status from a selected MOB
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param handle CAN MOB number
 *  \return U8  CAN_CMD_REFUSED or CAN_STATUS_ERROR or CAN_STATUS_COMPLETED or CAN_STATUS_NOT_COMPLETED
 */
extern U8 can_mob_get_status( U8 ch,
                              U8 handle);

/*! \brief Enable interrupt on CAN Channel
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \return U8 CAN_CMD_REFUSED or CAN_CMD_ACCEPTED
 */
extern U8 can_enable_interrupt( U8 ch);

/*! \brief Clear All Status on CAN Channel for a selected MOB
 *  \param ch CAN channel selected 0 (CAN Channel 0) 1 (CAN Channel 1)
 *  \param mob CAN MOB number
 */
extern void can_clear_status (U8 ch, U8 mob);
//______________________________________________________________________________

/**
 * \}
 */

#endif // _CAN_H_
