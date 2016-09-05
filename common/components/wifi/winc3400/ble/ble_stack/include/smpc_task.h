/**
 * \file
 *
 * \brief SMPC.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
 */

#ifndef SMPC_TASK_H_INCLUDED
#define SMPC_TASK_H_INCLUDED

#include "nmi_types.h"
#include "gapm_task.h"
#include "cmn_defs.h"

#define SMPC_IDX_MAX           BLE_CONNECTION_MAX

///Maximum Encryption Key size
#define SMPC_MAX_ENC_SIZE_LEN                 0x10

/// SMPC Task Messages
enum
{
    ///Host -> SMPC request to start pairing
    SMPC_PAIRING_REQ = 0x1C00,
    ///SMPC -> Host request for pairing information
    SMPC_PAIRING_IND,
    ///Host -> SMPC response with pairing information
    SMPC_PAIRING_RSP,

    ///Host -> SMPC request to encrypt link
    SMPC_START_ENC_REQ,
    ///SMPC -> Host security procedure over
    SMPC_SEC_STARTED_IND,

    //SMPC-> Host requests & responses for keys
    SMPC_TK_REQ_IND,
    SMPC_TK_REQ_RSP,
    SMPC_LTK_REQ_IND,
    SMPC_LTK_REQ_RSP,
    SMPC_IRK_REQ_IND,
    SMPC_IRK_REQ_RSP,
    SMPC_CSRK_REQ_IND,
    SMPC_CSRK_REQ_RSP,

    ///SMPC->Host key that was received
    SMPC_KEY_IND,

    /// -> SMPC : Timer Expiration indication
    SMPC_RTX_IND,

    //SMPC -> SMPM (-> LLM)
    ///Encryption Request using AES-128 HW implemented
    SMPC_E_REQ,
    ///Random Number request from HW
    SMPC_RAND_REQ,

    //(LLM -> ) SMPM -> SMPC
    ///Encrypted data response
    SMPC_E_CFM,
    ///Random number value response
    SMPC_RAND_CFM,

    //Message signing (GAP or GATT may be the source)
    ///HL  -> SMPC signature calculation request (whether for TX or check RX)
    SMPC_SIGN_REQ,
    ///SMPC-> HL Response with sign counter + calculated signature
    SMPC_GEN_SIGN_RSP,
    ///SMPC-> HL Response with status of signature check
    SMPC_CHK_SIGN_RSP,

    //Random address generation
    ///GAP  -> SMPC random address generation request
    SMPC_GEN_RAND_ADDR_REQ,
    ///SMPC -> GAP response with generated random address
    SMPC_GEN_RAND_ADDR_RSP,

    //Random address resolution
    ///GAP-> SMPC random address resolution request
    SMPC_SOLVE_ADDR_REQ,
    ///SMPC-> GAP given random address resolution conclusion
    SMPC_SOLVE_ADDR_RSP,

    ///Ask APP to check if peer with BD address is Unauth/Authen/Authen+Authoriz
    SMPC_CHK_BD_ADDR_REQ_IND,
    SMPC_CHK_BD_ADDR_REQ_RSP,

    SMPC_DBG_TRC,
    /// protocol timeout event, sent to source of procedure start
    SMPC_TIMEOUT_EVT,
    ///Repeated attempts Timeout
    SMPC_ATTEMPTS_IND
};


/// IO Capability Values
enum
{
    ///Display Only
    SMP_IO_CAP_DISPLAY_ONLY = 0x00,
    ///Display Yes No
    SMP_IO_CAP_DISPLAY_YES_NO,
    ///Keyboard Only
    SMP_IO_CAP_KB_ONLY,
    ///No Input No Output
    SMP_IO_CAP_NO_INPUT_NO_OUTPUT,
    ///Keyboard Display
    SMP_IO_CAP_KB_DISPLAY,
    SMP_IO_CAP_LAST
};

/// OOB Data Present Flag Values
enum
{
    ///OOB Data not present
    SMP_OOB_AUTH_DATA_NOT_PRESENT = 0x00,
    ///OOB data from remote device present
    SMP_OOB_AUTH_DATA_FROM_REMOTE_DEV_PRESENT,
    SMP_OOB_AUTH_DATA_LAST
};

/// Authentication Requirements
enum
{
    ///No MITM No Bonding
    SMP_AUTH_REQ_NO_MITM_NO_BOND = 0x00,
    ///No MITM Bonding
    SMP_AUTH_REQ_NO_MITM_BOND,
    ///MITM No Bonding
    SMP_AUTH_REQ_MITM_NO_BOND = 0x04,
    ///MITM and Bonding
    SMP_AUTH_REQ_MITM_BOND,
    SMP_AUTH_REQ_LAST
};

#endif /* SMPC_TASK_H_INCLUDED */
