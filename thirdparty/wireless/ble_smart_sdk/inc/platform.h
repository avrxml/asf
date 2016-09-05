/**************************************************************************//**
  \file platform.h
 
  \brief Includes datatypes and signatures for platform
 
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


#ifndef PLATFORM_DRV_H_INCLUDED
#define PLATFORM_DRV_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** callback function type for handling BLE FW stack messages */
typedef void (*platform_interface_callback) (uint8_t *, uint32_t );

/** callback function type for handling resume from sleep */
typedef void (*resume_callback) (void);
	
#define MSG_PREVENT_SLEEP	(0x4000)
#define APP_PREVENT_SLEEP	(0x8000)

/** enumerated status values for this platform driver */
typedef enum PLATFORM_STATUS
{
	/** Platform Initialized and ready */
	STATUS_SUCCESS = 0x00,
	/** Platform Initialized and ready */
	STATUS_ALREADY_INITIALIZED,
	/** Platform Initialized and ready */
	STATUS_RECEIVED_PLF_EVENT_MSG,
	/** Platform Initialized and ready */
	STATUS_RECEIVED_BLE_MSG,
	/** Failed to initialize the platform */
	STATUS_INITIALIZATION_FAILED,
	/** Failed to initialize the platform */
	STATUS_NOT_INITIALIZED,
	/** Timeout failure */
	STATUS_TIMEOUT = 0xD0,
	/** Status Failure */
	STATUS_FAILURE = 7,
	/** Status Busy */
	STATUS_RESOURCE_BUSY,
	/** Invalid arugment */
	STATUS_INVALID_ARGUMENT,
}plf_drv_status;
	
/**
@defgroup platform-drv platform Driver API

@{
*/
	
/** \name Configuration and initialization
 * @{
 */

/**
 * \brief Initializes a SAMB11 Platform for driver usage.
 *
 * Initializes the common data structures used by other peripheral drivers.
 * \note Call this API before using anyother API from other driver modules.
 *
 * \return Status of platform driver initialization.
 * \retval STATUS_SUCCESS								Platform driver initialized.
 * \retval STATUS_ALREADY_INITIALIZED		Platform driver already initialized.
 * \retval STATUS_NOT_INITIALIZED				Platform driver not initialized.
 */
plf_drv_status platform_driver_init(void);

/**
 * \brief Platform event wait function.
 *
 * Function to wait on a platform event. Platform event may be any peripheral
 * interrupt event callback message, or timer event callback message or message
 * from BLE FW stack. This is blocking API which will wait on the application
 * message queue.
 *
 * \note	BLE stack library internally calls this API to wait on any messages
 * 	from BLE Firmware. Application that uses both BLE library and driver library,
 * 	can call either the at_ble_event_get API to get either BLE or platform event or
 * 	call \ref platform_event_get API to get platform event.
 *
 * \param[in] timeout		Timeout value, currently not used.
 *
 * \return Status of platform event wait function.
 * \retval STATUS_RECEIVED_BLE_MSG				Recieved a message from BLE FW.
 * \retval STATUS_RECEIVED_PLF_EVENT_MSG	Recieved a Platform specific message.
 * \retval STATUS_FAILURE									Failed to receive any valid message.
 */
plf_drv_status platform_event_wait(uint32_t timeout);

/**
 * \brief API to send message to BLE FW stack.
 *
 * An API to send KE_MSG to BLE FW stack. Any message format will not
 * be understandable by the BLE FW stack. Please use BLE library API
 * to use BLE feature.
 * \note This API will be called from BLE API library.
 *
 * \param[in] data 		Buffer pointer for any event specific data.
 * \param[in] len 		unsigned short which contains the length of the data buffer.
 */
void platform_interface_send(uint8_t* data, uint32_t len);

/**
 * 	\brief API to wait and receive a platform event.
 *
 * 	Function to wait on a platform event. Platform event may be any 
 *	peripheral interrupt event callback message, or timer event 
 *	callback message.This is blocking API which will wait until 
 *	receives any message from the platform.
 *
 * 	\note	BLE stack library internally calls this API to wait on 
 *	any messages from BLE Firmware. Application that uses both BLE 
 *	library and driver library,can call either the at_ble_event_get
 *	API to get either BLE or platform event or call this API to 
 *	recieve a platform event.
 *
 * 	\param[in,out] event_type Pointer to uint16_t type.
 * 	\param[in,out] data  			Pointer to a buffer to receive any event specific data.
 * 	\param[in,out] data_len 	Pointer to uint16_t type to inform about the size of buffer.
 *
 * 	\return Status of platform event get feature.
 * 	\retval STATUS_RECEIVED_BLE_MSG				Recieved a message from BLE FW.
 * 	\retval STATUS_RECEIVED_PLF_EVENT_MSG	Recieved a Platform specific message.
 * 	\retval STATUS_FAILURE									Failed to receive any valid message.
 *
 */
plf_drv_status platform_event_get(uint16_t* event_type, uint8_t* data, uint16_t *data_len);

/**
 * \brief API to register a callback function for handling BLE messages.
 *
 * This API registers a callback function for processing BLE messages that are
 * received from the BLE FW stack. Once registered any BLE messages received will
 * be forwarded to this callback function.
 * \note BLE API library will call this API to register a callback function for
 * receiving BLE specific messages. Before calling this API make sure you 
 * initialize the platform layer by calling \ref platform_driver_init API.
 *
 * \param[in] cb 	Callback function of type \ref platform_interface_callback.
 *
 * \return Status of registering a callback function.
 * \retval STATUS_SUCCESS						Successfully registered a callback function.
 * \retval STATUS_NOT_INITIALIZED		Failed to register because platform is not initialized.
 */
plf_drv_status platform_register_ble_msg_handler(platform_interface_callback cb);

/**
 * \brief API to send a message to application layer about any platform event.
 *
 * This API sends a message to application regarding any platform event. This API
 * is used by the callback functions which are being called from an interrupt 
 * context, to send a message to application thread regarding that callback
 * event.
 * \note Event that is received will be of type ((<callback_id << 8) |intr_index).
 *
 * \param[in] intr_index 	Interrupt index. This is specific value for each peripheral.
 * \param[in] callback_id callback_id is a specific callback event type.
 * \param[in] data 				Buffer pointer for any event specific data.
 * \param[in] data_len 		unsigned short which contains the length of the data buffer.
 */
 void send_plf_int_msg_ind(uint8_t intr_index, uint8_t callback_id, void *data, uint16_t data_len);

/**
 * \brief API to acquire sleep lock.
 *
 * This API acquires the sleep lock which prevent the system from entering into lower power
 * state (ULP). As long as this lock is held System is prevented from entering to any lower 
 * power state. 
 * \note Please use this lock judiciously. You may acquire this lock at app_entry and once 
 * the application has initialized and running release this lock using \ref release_sleep_lock
 * api to let the system go to ULP state to save power.
 *
 */
plf_drv_status acquire_sleep_lock(void);

/**
 * \brief API to release sleep lock.
 *
 * This API releases the sleep lock which allows the system to enter into lower power
 * state (ULP). As long as this lock is held System is prevented from entering to any lower 
 * power state. 
 * \note Once this application sleep lock is released, make sure the AON_GPIO_X are held low
 * in order for system to enter ULP state.
 *
 */
plf_drv_status release_sleep_lock(void);
plf_drv_status release_message_lock(void);

/**
 * \brief API to register a callback function when system resumes from sleep(ULP).
 *
 * This API registers a callback function that will be called when system exits ULP.
 * 
 * \note This callback can be used to reinitialize anything that might have cleared or
 * turned off when system entered ULP state. 
 *
 */
plf_drv_status register_resume_callback(resume_callback cb);

/**
 * \brief API to Initialize the ports available.
 *
 * This API initalizes the ports available on SAMB11.
 * 
 * \note Once system enters ULP(Ultra Low Power) state. All the configurations and 
 * settings of all GPIOs are reset to default. Once the system comes out of ULP, 
 * the application needs to call this API to reset the software structures used to 
 * represent these ports.
 *
 */
void init_port_list(void);

/** @ingroup platform-drv
  * @brief Reset BLE Chip.
  * @note This function will reset whole SoC, All running tasks including current will be terminated 
  * @warning This function should be called in BLE Context [event get loop] ONLY. 
  *          If you want to post is as separate event, use @ref at_ble_event_user_defined_post.
  */
void platform_chip_reset(void);
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif	//PLATFORM_DRV_H_INCLUDED
