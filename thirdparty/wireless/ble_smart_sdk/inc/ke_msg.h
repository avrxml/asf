/**************************************************************************//**
  \file ke_msg.h
 
  \brief Includes datatypes and signatures for ke_msg
 
  Copyright (c) 2016, Atmel Corporation. All rights reserved.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Atmel Corporation: http://www.atmel.com
 
******************************************************************************/
/**************************************************************************//**
 * Adapted from RW
 * A kernel message has an ID, a receiver task ID and a source task ID.
 * In most cases, it also has parameters which are defined in
 * a structure dynamically embedded in the message structure,
 * so the whole message will be managed internally as one block.
 *
 *
 * In order to send a message, a function first have to allocate
 * the memory for this message. It can be done with the wrapper
 * macro KE_MSG_ALLOC() (which will call ke_msg_alloc()).

 * The message can then be sent with ke_msg_send(). The kernel
 * will take care of freeing the allocated memory.

 * If the message has no parameters, the ke_msg_send_basic() function
 * can be used.
 *
 * 
 ******************************************************************************/

#ifndef _KE_MSG_H_
#define _KE_MSG_H_

#include <stddef.h>          // standard definition
#include <stdint.h>          // standard integer
#include <stdbool.h>         // standard boolean
//#include "at_ble_api.h"

#define KE_MSG_HDR_LENGTH 8

/// Task Identifier. Composed by the task type and the task index.
typedef uint16_t ke_task_id_t;

/// Message Identifier. The number of messages is limited to 0xFFFF.
/// The message ID is divided in two parts:
/// - bits[15..10] : task index (no more than 64 tasks supported).
/// - bits[9..0] : message index (no more that 1024 messages per task).
typedef uint16_t ke_msg_id_t;

/// structure of a list element header
struct co_list_hdr{
	/// Pointer to next co_list_hdr
	struct co_list_hdr *next;
};
/// Message structure.
struct ke_msg
{
    struct co_list_hdr hdr;     ///< List header for chaining
#ifdef CHIPVERSION_A4
	uint8_t         hci_type;   ///< Type of HCI data (used by the HCI only)
    int8_t          hci_off;    ///< Offset of the HCI data in the message (used by the HCI only)
    uint16_t        hci_len;    ///< Length of the HCI traffic (used by the HCI only)
#endif	//CHIPVERSION_A4
    ke_msg_id_t     id;         ///< Message id.
    ke_task_id_t    dest_id;    ///< Destination kernel identifier.
    ke_task_id_t    src_id;     ///< Source kernel identifier.
    uint16_t        param_len;  ///< Parameter embedded struct length.
    uint32_t        param[1];   ///< Parameter embedded struct. Must be word-aligned.
};


///Structure for application system interface packet header
struct ke_msghdr
{
    ///Message id
    ke_msg_id_t  id;
    ///Destination task identifier for KE
    ke_task_id_t dest_id;
    ///Source task identifier for KE
    ke_task_id_t src_id;
    ///Message parameter length
    uint16_t     param_len;
};

/**
 ****************************************************************************************
 * @brief Convert a parameter pointer to a message pointer
 *
 * @param[in]  param_ptr Pointer to the parameter member of a ke_msg
 *                       Usually retrieved by a ke_msg_alloc()
 *
 * @return The pointer to the ke_msg
 ****************************************************************************************
 */
#if defined( __ICCARM__) //remove warning message on IAR
#pragma inline
static struct ke_msg *ke_param2msg(void const *param_ptr)
#else
static __inline struct ke_msg *ke_param2msg(void const *param_ptr)
#endif
{
	return (struct ke_msg *)((void *)(((uint8_t *)param_ptr) - offsetof(struct ke_msg, param)));
}

/**
 ****************************************************************************************
 * @brief Convert a message pointer to a parameter pointer
 *
 * @param[in]  msg Pointer to the ke_msg.
 *
 * @return The pointer to the param member
 ****************************************************************************************
 */
#if defined( __ICCARM__) //remove warning message on IAR
#pragma inline
static void *ke_msg2param(struct ke_msg const *msg)
#else
static __inline void *ke_msg2param(struct ke_msg const *msg)
#endif
{
    return (void*) (((uint8_t*) msg) + offsetof(struct ke_msg, param));
}

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc()
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define KE_MSG_ALLOC(id, dest, src, param_str) \
    (struct param_str*) ke_msg_alloc(id, dest, src, sizeof(struct param_str))

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_free()
 *
 * This macro calls ke_msg_free() with the appropriate msg pointer as parameter, according
 * to the message parameter pointer passed.
 *
 * @param[in] param_ptr parameter structure pointer
 ****************************************************************************************
 */
#define KE_MSG_FREE(param_ptr) ke_msg_free(ke_param2msg((param_ptr)))

/**
 ****************************************************************************************
 * @brief Convenient wrapper to ke_msg_alloc()
 *
 * This macro calls ke_msg_alloc() and cast the returned pointer to the
 * appropriate structure with a variable length. Can only be used if a parameter structure exists
 * for this message (otherwise, use ke_msg_send_basic()).Can only be used if the data array is
 * located at the end of the structure.
 *
 * @param[in] id        Message identifier
 * @param[in] dest      Destination Identifier
 * @param[in] src       Source Identifier
 * @param[in] param_str parameter structure tag
 * @param[in] length    length for the data
 *
 * @return Pointer to the parameter member of the ke_msg.
 ****************************************************************************************
 */
#define KE_MSG_ALLOC_DYN(id, dest, src, param_str,length)  (struct param_str*)ke_msg_alloc(id, dest, src, \
    (sizeof(struct param_str) + length));

void internal_host_send(uint8_t* data, uint32_t len);







#endif // _KE_MSG_H_
