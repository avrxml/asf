/**
 * \file
 *
 * \brief USB Device driver
 * Compliance with common driver UDD
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb.h"
#include "sysclk.h"
#include "sleep.h"
#include "udd.h"
#include "usbc_otg.h"
#include "usbc_device.h"
#include <string.h>

#ifndef UDD_NO_SLEEP_MGR
#   include "sleepmgr.h"
#endif

#ifndef UDD_USB_INT_LEVEL
#   define UDD_USB_INT_LEVEL 0 // By default USB interrupt have low priority
#endif

//  These defines are missing from or wrong in the toolchain header file
#ifndef AVR32_PM_AWEN_USBCWEN_MASK
// Optional #undef AVR32_PM_AWEN_USBCWEN_MASK if the define value is wrong.
#   if UC3L3_L4
#      define AVR32_PM_AWEN_USBCWEN_MASK  0x80
#   elif UC3C || UC3D
#      define AVR32_PM_AWEN_USBCWEN_MASK  0x01
#   else
#      error 'Missing definition for AVR32_PM_AWEN_USBCWEN_MASK mask value'
#  endif
#endif

/**
 * \ingroup udd_group
 * \defgroup udd_usbc_group USBC Device Driver
 *
 * \section USBC_CONF USBC Custom configuration
 * The following USBC driver configuration must be included in the conf_usb.h
 * file of the application.
 *
 * UDD_USB_INT_LEVEL<br>
 * Option to change the interrupt priority (0 to 3) by default 0 (recommended).
 *
 * \section Callbacks management
 * The USB driver is fully managed by interrupt and does not request periodic
 * task. Thereby, the USB events use callbacks to transfer the information.
 * The callbacks are declared in static during compilation or in variable during
 * code execution.
 *
 * Static declarations defined in conf_usb.h:
 * - UDC_VBUS_EVENT(bool b_present)<br>
 *   To signal Vbus level change
 * - UDC_SUSPEND_EVENT()<br>
 *   Called when USB bus enter in suspend mode
 * - UDC_RESUME_EVENT()<br>
 *   Called when USB bus is wakeup
 * - UDC_SOF_EVENT()<br>
 *   Called for each received SOF, Note: Each 1ms in HS/FS mode only.
 *
 * Dynamic callbacks, called "endpoint job" , are registered
 * in udd_ep_job_t structure via the following functions:
 * - udd_ep_run()<br>
 *   To call it when a transfer is finish
 * - udd_ep_wait_stall_clear()<br>
 *   To call it when a endpoint halt is disabled
 *
 * \section Power mode management
 * The Sleep modes authorized :
 * - in USB IDLE state, the USBC needs of USB clock and authorizes up to IDLE mode
 * - in USB SUSPEND state, the USBC no needs USB clock but requests a minimum
 *   clock restart timing. Thus, it authorizes up to STATIC or STANDBY mode.
 * - Vbus monitoring used in USB Self-Power mode authorizes up to STOP mode
 *
 * The USBC_SLEEP_MODE_USB_IDLE equals SLEEPMGR_IDLE.
 *
 * The USBC_SLEEP_MODE_USB_SUSPEND depends on USB Power mode,
 * USB clock startup timing and USB Speed mode:
 * | Power Mode | Speed mode | Clock Startup | Sleep mode authorized |
 * | X          | LS, FS     | >10ms         | SLEEPMGR_STDBY        |
 * | X          | HS         | >3ms          | SLEEPMGR_STDBY        |
 * | Self-Power | LS, FS     | <=10ms        | SLEEPMGR_STOP         |
 * | Self-Power | HS         | <=3ms         | SLEEPMGR_STOP         |
 * | Bus-Power  | LS, FS     | <=10ms        | SLEEPMGR_STATIC       |
 * | Bus-Power  | HS         | <=3ms         | SLEEPMGR_STATIC       |
 *
 * @{
 */


// Check USB Device configuration
#ifndef USB_DEVICE_EP_CTRL_SIZE
#   error USB_DEVICE_EP_CTRL_SIZE not defined
#endif
#ifndef USB_DEVICE_MAX_EP
#   error USB_DEVICE_MAX_EP not defined
#endif
#if USB_DEVICE_MAX_EP >= AVR32_USBC_EPT_NUM
#   error USB_DEVICE_MAX_EP is too high and not supported by this part
#endif
#if (UC3C)
#   ifdef USB_DEVICE_HS_SUPPORT
#      error The High speed mode is not supported on this part, please remove USB_DEVICE_HS_SUPPORT in conf_usb.h
#   endif
#endif


/**
 * \name Power management routine.
 */
//@{

#ifndef UDD_NO_SLEEP_MGR

//! Definition of sleep levels
#  if ((defined USB_DEVICE_HS_SUPPORT) && (USBCLK_STARTUP_TIMEOUT>3000)) \
		|| ((!defined USB_DEVICE_HS_SUPPORT) && (USBCLK_STARTUP_TIMEOUT>10000))
#    define USBC_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_STDBY
#else
#  if (USB_DEVICE_ATTR & USB_CONFIG_ATTR_SELF_POWERED)
#    define USBC_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_STOP
#  else
#    define USBC_SLEEP_MODE_USB_SUSPEND  SLEEPMGR_STATIC
#  endif
#endif
#define USBC_SLEEP_MODE_USB_IDLE  SLEEPMGR_IDLE

//! State of USB line
static bool udd_b_idle;


/*! \brief Authorize or not the CPU powerdown mode
 *
 * \param b_enable   true to authorize powerdown mode
 */
static void udd_sleep_mode(bool b_idle)
{
	if (!b_idle && udd_b_idle) {
		sleepmgr_unlock_mode(USBC_SLEEP_MODE_USB_IDLE);
	}
	if (b_idle && !udd_b_idle) {
		sleepmgr_lock_mode(USBC_SLEEP_MODE_USB_IDLE);
	}
	udd_b_idle = b_idle;
}
#else

static void udd_sleep_mode(bool b_idle)
{
	UNUSED(b_idle);
}
#endif  // UDD_NO_SLEEP_MGR

//@}

/**
 * @brief USB SRAM data about endpoint descriptor table
 * The content of the USB SRAM can be :
 * - modified by USB hardware by interface to signal endpoint status.
 *   Thereby, it is read by software.
 * - modified by USB software to control endpoint.
 *   Thereby, it is read by hardware.
 * This data section is volatile.
 *
 * @{
 */
UDC_BSS(32)
static volatile usb_desc_table_t udd_g_ep_table[2 * (USB_DEVICE_MAX_EP + 1)];

/**
 * \name Control endpoint low level management routine.
 *
 * This function performs control endpoint management.
 * It handle the SETUP/DATA/HANDSHAKE phases of a control transaction.
 */
//@{

//! Global variable to give and record information about setup request management
COMPILER_WORD_ALIGNED udd_ctrl_request_t udd_g_ctrlreq;

//! Bit definitions about endpoint control state machine for udd_ep_control_state
typedef enum {
	UDD_EPCTRL_SETUP                  = 0, //!< Wait a SETUP packet
	UDD_EPCTRL_DATA_OUT               = 1, //!< Wait a OUT data packet
	UDD_EPCTRL_DATA_IN                = 2, //!< Wait a IN data packet
	UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP  = 3, //!< Wait a IN ZLP packet
	UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP = 4, //!< Wait a OUT ZLP packet
	UDD_EPCTRL_STALL_REQ              = 5, //!< STALL enabled on IN & OUT packet
} udd_ctrl_ep_state_t;

//! State of the endpoint control management
static udd_ctrl_ep_state_t udd_ep_control_state;

//! Total number of data received/sent during data packet phase with previous payload buffers
static uint16_t udd_ctrl_prev_payload_nb_trans;

//! Number of data received/sent to/from udd_g_ctrlreq.payload buffer
static uint16_t udd_ctrl_payload_nb_trans;

/**
 * \brief Buffer to store the data received on control endpoint (SETUP/OUT endpoint 0)
 *
 * Used to avoid a RAM buffer overflow in case of the payload buffer
 * is smaller than control endpoint size
 */
UDC_BSS(4) uint8_t udd_ctrl_buffer[USB_DEVICE_EP_CTRL_SIZE];

/**
 * \brief Reset control endpoint
 *
 * Called after a USB line reset or when UDD is enabled
 */
static void udd_reset_ep_ctrl(void);

/**
 * \brief Reset control endpoint management
 *
 * Called after a USB line reset or at the end of SETUP request (after ZLP)
 */
static void udd_ctrl_init(void);

//! \brief Managed reception of SETUP packet on control endpoint
static void udd_ctrl_setup_received(void);

//! \brief Managed reception of IN packet on control endpoint
static void udd_ctrl_in_sent(void);

//! \brief Managed reception of OUT packet on control endpoint
static void udd_ctrl_out_received(void);

//! \brief Managed underflow event of IN packet on control endpoint
static void udd_ctrl_underflow(void);

//! \brief Managed overflow event of OUT packet on control endpoint
static void udd_ctrl_overflow(void);

//! \brief Managed stall event of IN/OUT packet on control endpoint
static void udd_ctrl_stall_data(void);

//! \brief Send a ZLP IN on control endpoint
static void udd_ctrl_send_zlp_in(void);

//! \brief Send a ZLP OUT on control endpoint
static void udd_ctrl_send_zlp_out(void);

//! \brief Call callback associated to setup request
static void udd_ctrl_endofrequest(void);


/**
 * \brief Main interrupt routine for control endpoint
 *
 * This switches control endpoint events to correct sub function.
 *
 * \return \c 1 if an event about control endpoint is occurred, otherwise \c 0.
 */
static bool udd_ctrl_interrupt(void);

//@}


/**
 * \name Management of bulk/interrupt/isochronous endpoints
 *
 * The UDD manages the data transfer on endpoints:
 * - Start data transfer on endpoint with USB Device DMA
 * - Send a ZLP packet if requested
 * - Call callback registered to signal end of transfer
 * The transfer abort and stall feature are supported.
 */
//@{
#if (0!=USB_DEVICE_MAX_EP)

//! Structure definition about job registered on an endpoint
typedef struct {
	//! A job is registered on this endpoint
	uint8_t busy:1;

	//! A short packet is requested for this job on endpoint IN
	uint8_t b_shortpacket:1;

	//! The cache buffer is currently used on endpoint OUT
	uint8_t b_use_out_cache_buffer:1;

	//! Buffer located in internal RAM to send or fill during job
	uint8_t *buf;

	//! Size of buffer to send or fill
	iram_size_t buf_size;

	//! Total number of data transfered on endpoint
	iram_size_t nb_trans;

	union {
		//! Callback to call at the end of transfer
		udd_callback_trans_t call_trans;

		//! Callback to call when the endpoint halt is cleared
		udd_callback_halt_cleared_t call_nohalt;
	};
} udd_ep_job_t;


//! Array to register a job on bulk/interrupt/isochronous endpoint
static udd_ep_job_t udd_ep_job[USB_DEVICE_MAX_EP];

/**
 * \brief Buffer to store the data received on bulk/interrupt endpoints
 *
 * Used to avoid a RAM buffer overflow in case of the user buffer
 * is smaller than endpoint size
 *
 * \warning The isochronous endpoint is not protected by this system
 * and the user must always use a buffer corresponding at endpoint size
 */
#if (defined USB_DEVICE_LOW_SPEED)
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][8];
#elif (defined USB_DEVICE_HS_SUPPORT)
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][512];
#else
UDC_BSS(4) uint8_t udd_ep_out_cache_buffer[USB_DEVICE_MAX_EP][64];
#endif

/**
 * \brief Call the callback associated to the job which is finished
 *
 * \param ep         endpoint number of job to abort
 */
static void udd_ep_trans_done(udd_ep_id_t ep);

/**
 * \brief Main interrupt routine for bulk/interrupt/isochronous endpoints
 *
 * This switches endpoint events to correct sub function.
 *
 * \return \c 1 if an event about bulk/interrupt/isochronous endpoints has occurred, otherwise \c 0.
 */
static bool udd_ep_interrupt(void);

#endif // (0!=USB_DEVICE_MAX_EP)
//@}


/**
 * \brief Function called by USBC interrupt to manage USB device interrupts
 *
 * USB Device interrupt events are split in three parts:
 * - USB line events (SOF, reset, suspend, resume, wakeup)
 * - control endpoint events (setup reception, end of data transfer, underflow, overflow, stall)
 * - bulk/interrupt/isochronous endpoints events (end of data transfer)
 *
 * Note:
 * Here, the global interrupt mask is not clear when an USB interrupt is enabled
 * because this one can not be occurred during the USB ISR (=during INTX is masked).
 * See Technical reference $3.8.3 Masking interrupt requests in peripheral modules.
 */
#ifdef UHD_ENABLE
void udd_interrupt(void); // To avoid GCC warning
void udd_interrupt(void)
#else
#   ifdef FREERTOS_USED
#      include "FreeRTOS.h"
#      include "task.h"
ISR_FREERTOS(udd_interrupt, AVR32_USBC_IRQ_GROUP, UDD_USB_INT_LEVEL)
#   else
ISR(udd_interrupt, AVR32_USBC_IRQ_GROUP, UDD_USB_INT_LEVEL)
#   endif
#endif
{
	if (Is_udd_sof()) {
		udd_ack_sof();
		udc_sof_notify();
#ifdef UDC_SOF_EVENT
		UDC_SOF_EVENT();
#endif
		goto udd_interrupt_end;
	}

	if (udd_ctrl_interrupt()) {
		// Interrupt acked by control endpoint managed
		goto udd_interrupt_end;
	}

#if (0!=USB_DEVICE_MAX_EP)
	if (udd_ep_interrupt()) {
		// Interrupt acked by bulk/interrupt/isochronous endpoint managed
		goto udd_interrupt_end;
	}
#endif

	// USB bus reset detection
	if (Is_udd_reset()) {
		udd_ack_reset();
		// Abort all jobs on-going
#if (USB_DEVICE_MAX_EP != 0)
		// For each endpoint, kill job
		{
			uint8_t i;
			for (i = 1; i <= USB_DEVICE_MAX_EP; i++) {
				udd_ep_abort(i);
			}
		}
#endif
		// Reset USB Device Stack Core
		udc_reset();
		// Reset endpoint control
		udd_reset_ep_ctrl();
		// Reset endpoint control management
		udd_ctrl_init();
		goto udd_interrupt_end;
	}

	if (Is_udd_suspend_interrupt_enabled() && Is_udd_suspend()) {
		otg_unfreeze_clock();
		// The suspend interrupt is automatically acked when a wakeup occur
		udd_disable_suspend_interrupt();
		udd_enable_wake_up_interrupt();
		otg_freeze_clock(); // Mandatory to exit of sleep mode after a wakeup event
		udd_sleep_mode(false); // Enter in SUSPEND mode
#ifdef UDC_SUSPEND_EVENT
		UDC_SUSPEND_EVENT();
#endif
		goto udd_interrupt_end;
	}

	if (Is_udd_wake_up_interrupt_enabled() && Is_udd_wake_up()) {
		// Ack wakeup interrupt and enable suspend interrupt
		otg_unfreeze_clock();

		// Check USB clock ready after suspend and eventually sleep USB clock
		while( !Is_otg_clock_usable() );

		// The wakeup interrupt is automatically acked when a suspend occur
		udd_disable_wake_up_interrupt();
		udd_enable_suspend_interrupt();
		udd_sleep_mode(true); // Enter in IDLE mode
#ifdef UDC_RESUME_EVENT
		UDC_RESUME_EVENT();
#endif
		goto udd_interrupt_end;
	}

	if (Is_otg_vbus_transition()) {
		// Ack Vbus transition and send status to high level
		otg_unfreeze_clock();
		otg_ack_vbus_transition();
		otg_freeze_clock();
#ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
		if (Is_otg_vbus_high()) {
			udd_attach();
		} else {
			udd_detach();
		}
#endif		
#ifdef UDC_VBUS_EVENT
		UDC_VBUS_EVENT(Is_otg_vbus_high());
#endif
		goto udd_interrupt_end;
	}
udd_interrupt_end:
	otg_data_memory_barrier();
#if (defined FREERTOS_USED)
	// Since we do not know if the user callbacks have used or not FreeRTOS APIs, let's
	// consider that exiting from the USB interrupt will require a context switch.
	return pdTRUE;
#else
	return;
#endif
}


bool udd_include_vbus_monitoring(void)
{
#if ( UC3L3_L4 )
	return false;
#else
	return true;
#endif
}


void udd_enable(void)
{
	irqflags_t flags;

	flags = cpu_irq_save();

#ifdef UHD_ENABLE
	//* DUAL ROLE INITIALIZATION
	if (otg_dual_enable()) {
		// The current mode has been started by otg_dual_enable()
		cpu_irq_restore(flags);
		return;
	}
#else
	//* SINGLE DEVICE MODE INITIALIZATION
	sysclk_enable_usb();

	// Here, only the device mode is possible,
	// USBC interrupt is linked to UDD interrupt
	irq_register_handler(
#   ifdef FREERTOS_USED
			(__int_handler)
#   endif
			udd_interrupt, AVR32_USBC_IRQ, UDD_USB_INT_LEVEL);

	// Always authorize asynchronous USB interrupts to exit of sleep mode
	pm_asyn_wake_up_enable(AVR32_PM_AWEN_USBCWEN_MASK);
#endif

#if (defined USB_ID) && (defined UHD_ENABLE)
	// Check that the device mode is selected by ID pin
	if (!Is_otg_id_device()) {
		cpu_irq_restore(flags);
		return; // Device is not the current mode
	}
#else
	// ID pin not used then force device mode
	otg_disable_id_pin();
	otg_force_device_mode();
#endif

	// Enable USB hardware
	otg_enable_pad();
	otg_enable();
	otg_unfreeze_clock();
	(void)Is_otg_clock_frozen();

	memset((uint8_t *) udd_g_ep_table, 0, sizeof(udd_g_ep_table));
	otg_register_desc_tab(udd_g_ep_table);

	// Reset internal variables
#if (0!=USB_DEVICE_MAX_EP)
	{
		uint8_t i;
		for (i = 0; i < USB_DEVICE_MAX_EP; i++) {
			udd_ep_job[i].busy = false;
		}
	}
#endif

	// Set the USB speed requested by configuration file
#ifdef USB_DEVICE_LOW_SPEED
	udd_low_speed_enable();
#else
	udd_low_speed_disable();
#   ifdef USB_DEVICE_HS_SUPPORT
	udd_high_speed_enable();
#   else
	udd_high_speed_disable();
#   endif
#endif
	otg_ack_vbus_transition();
	// Force Vbus interrupt in case of Vbus always with a high level
	// This is possible with a short timing between a Host mode stop/start.
	if (Is_otg_vbus_high()) {
		otg_raise_vbus_transition();
	}
	otg_enable_vbus_interrupt();
	otg_freeze_clock();

#ifndef UDD_NO_SLEEP_MGR
	// Initialize the sleep mode authorized for the USB suspend mode
	udd_b_idle = false;
	sleepmgr_lock_mode(USBC_SLEEP_MODE_USB_SUSPEND);
#endif

#if UC3L3_L4
#  ifndef USB_DEVICE_ATTACH_AUTO_DISABLE
	udd_attach();
#  endif	
#endif	

	cpu_irq_restore(flags);
}


void udd_disable(void)
{
	irqflags_t flags;

#ifdef UHD_ENABLE
#   ifdef USB_ID
	if (Is_otg_id_host()) {
		return; // Host mode running, ignore UDD disable
	}
#   else
	if (Is_otg_host_mode_forced()) {
		return; // Host mode running, ignore UDD disable
	}
#   endif
#endif

	flags = cpu_irq_save();
	otg_unfreeze_clock();
	udd_detach();
#ifndef UDD_NO_SLEEP_MGR
	sleepmgr_unlock_mode(USBC_SLEEP_MODE_USB_SUSPEND);
#endif

#ifndef UHD_ENABLE
	otg_disable();
	otg_disable_pad();
	sysclk_disable_usb();
	// Else the USB clock disable is done by UHC which manage USB dual role
#endif
	cpu_irq_restore(flags);
}


void udd_attach(void)
{
	irqflags_t flags;
	flags = cpu_irq_save();

	// At startup the USB bus state is unknown,
	// therefore the state is considered IDLE to not miss any USB event
	udd_sleep_mode(true);
	otg_unfreeze_clock();
	while (!Is_otg_clock_usable());

	// Authorize attach if Vbus is present
	udd_attach_device();

	// Enable USB line events
	udd_enable_reset_interrupt();
	udd_enable_suspend_interrupt();
	udd_enable_wake_up_interrupt();
	udd_enable_sof_interrupt();

	// Reset following interrupts flag
	udd_ack_reset();
	udd_ack_sof();

	// The first suspend interrupt must be forced
	udd_raise_suspend();
	udd_ack_wake_up();
	otg_freeze_clock();
	cpu_irq_restore(flags);
}


void udd_detach(void)
{
	otg_unfreeze_clock();

	// Detach device from the bus
	udd_detach_device();
	otg_freeze_clock();
	udd_sleep_mode(false);
}


bool udd_is_high_speed(void)
{
#ifdef USB_DEVICE_HS_SUPPORT
	return !Is_udd_full_speed_mode();
#else
	return false;
#endif
}


void udd_set_address(uint8_t address)
{
	udd_disable_address();
	udd_configure_address(address);
	udd_enable_address();
}


uint8_t udd_getaddress(void)
{
	return udd_get_configured_address();
}


uint16_t udd_get_frame_number(void)
{
	return udd_frame_number();
}

uint16_t udd_get_micro_frame_number(void)
{
#ifdef USB_DEVICE_HS_SUPPORT
	return udd_micro_frame_number();
#else
	return 0;
#endif
}


void udd_send_remotewakeup(void)
{
#ifndef UDD_NO_SLEEP_MGR
	if (!udd_b_idle)
#endif
	{
		udd_sleep_mode(true); // Enter in IDLE mode
		otg_unfreeze_clock();
		udd_initiate_remote_wake_up();
	}
}


void udd_set_setup_payload( uint8_t *payload, uint16_t payload_size )
{
	udd_g_ctrlreq.payload = payload;
	udd_g_ctrlreq.payload_size = payload_size;
}


#if (0!=USB_DEVICE_MAX_EP)
bool udd_ep_alloc(udd_ep_id_t ep, uint8_t bmAttributes,
		uint16_t MaxEndpointSize)
{
	uint8_t ep_addr = ep & USB_EP_ADDR_MASK;
	if (Is_udd_endpoint_enabled(ep_addr)) {
		return false;
	}

	// Check if endpoint size is 8,16,32,64,128,256,512 or 1023
	Assert(MaxEndpointSize < 1024);
	Assert((MaxEndpointSize == 1023) || !(MaxEndpointSize & (MaxEndpointSize - 1)));
	Assert(MaxEndpointSize >= 8);

	// Check endpoint type
	Assert(((bmAttributes & USB_EP_TYPE_MASK) == USB_EP_TYPE_ISOCHRONOUS)
			|| ((bmAttributes & USB_EP_TYPE_MASK) == USB_EP_TYPE_BULK)
			|| ((bmAttributes & USB_EP_TYPE_MASK) == USB_EP_TYPE_INTERRUPT));

	udd_configure_endpoint(ep_addr, bmAttributes,
			((ep & USB_EP_DIR_IN) ? 1 : 0),
			MaxEndpointSize, AVR32_USBC_UECFG0_EPBK_SINGLE);

	udd_enable_busy_bank0(ep_addr);
	udd_enable_endpoint(ep_addr);

#if (defined USB_DISABLE_NYET_FOR_OUT_ENDPOINT)
	// Disable the NYET feature for OUT endpoint. Using OUT multipacket, each
	// OUT packet are always NYET.
	if (!(ep & USB_EP_DIR_IN)) {
		udd_disable_nyet(ep_addr);
	}
#endif
	return true;
}


void udd_ep_free(udd_ep_id_t ep)
{
	udd_ep_abort(ep);
#if( defined UDC_RAM_ACCESS_ERROR_EVENT )
	if( Is_udd_ram_access_error(ep & 0x7F) ){
		UDC_RAM_ACCESS_ERROR_EVENT();
	}
#endif
	udd_disable_endpoint(ep & 0x7F);
}


bool udd_ep_is_halted(udd_ep_id_t ep)
{
	return Is_udd_endpoint_stall_requested(ep & USB_EP_ADDR_MASK);
}


bool udd_ep_set_halt(udd_ep_id_t ep)
{
	uint8_t ep_index = ep & USB_EP_ADDR_MASK;

	if (USB_DEVICE_MAX_EP < ep_index) {
		return false;
	}

	// Stall endpoint
	udd_enable_stall_handshake(ep_index);
	udd_ep_abort(ep);
	return true;
}


bool udd_ep_clear_halt(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;

	ep &= USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep) {
		return false;
	}
	ptr_job = &udd_ep_job[ep - 1];

	if (Is_udd_endpoint_stall_requested(ep)) {
		// Remove stall request
		udd_disable_stall_handshake(ep);
		if (Is_udd_stall(ep)) {
			udd_ack_stall(ep);
			// A packet has been stalled, then reset data toggle
			udd_reset_data_toggle(ep);
		}

		// If a job is register on clear halt action
		// then execute callback
		if (ptr_job->busy == true) {
			ptr_job->busy = false;
			ptr_job->call_nohalt();
		}
	}
	return true;
}


bool udd_ep_run(udd_ep_id_t ep, bool b_shortpacket,
		uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	udd_ep_id_t ep_num;
	udd_ep_job_t *ptr_job;
	irqflags_t flags;

	ep_num = ep & USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep_num) {
		return false;
	}
	if ((!Is_udd_endpoint_enabled(ep_num))
			|| Is_udd_endpoint_stall_requested(ep_num)) {
		return false; // Endpoint is halted
	}

	// Get job about endpoint
	ptr_job = &udd_ep_job[ep_num - 1];

	flags = cpu_irq_save();
	if (ptr_job->busy == true) {
		cpu_irq_restore(flags);
		return false; // Job already on going
	}
	ptr_job->busy = true;
	cpu_irq_restore(flags);

	// No job running. Let's setup a new one.
	//
	ptr_job->buf = buf;
	ptr_job->buf_size = buf_size;
	ptr_job->nb_trans = 0;
	ptr_job->call_trans = callback;
	ptr_job->b_shortpacket = b_shortpacket;
	ptr_job->b_use_out_cache_buffer = false;

	if ( (USB_EP_DIR_IN != (ep & USB_EP_DIR_IN))
			&& (AVR32_USBC_PTYPE_ISOCHRONOUS == udd_get_endpoint_type(ep_num))
			&& (0 != (buf_size % udd_get_endpoint_size(ep_num)))) {
		// The user must use a buffer size modulo endpoint size
		// for an isochronous IN endpoint
		ptr_job->busy = false;
		return false;
	}

	// Initialize value to simulate a empty transfer
	udd_udesc_rst_buf0_ctn(ep_num);
	udd_udesc_rst_buf0_size(ep_num);

	// Request next transfer
	udd_ep_trans_done(ep);
	return true;
}


void udd_ep_abort(udd_ep_id_t ep)
{
	irqflags_t flags;
	udd_ep_job_t *ptr_job;

	ep &= USB_EP_ADDR_MASK;

	// Disable interrupt of endpoint
	flags = cpu_irq_save();
	udd_disable_endpoint_interrupt(ep);
	cpu_irq_restore(flags);

	// Stop transfer
	udd_enable_busy_bank0(ep);

	// Job complete then call callback
	ptr_job = &udd_ep_job[ep - 1];
	if (!ptr_job->busy) {
		return;
	}
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		if (Is_udd_endpoint_in(ep)) {
			ep |= USB_EP_DIR_IN;
		}
		// It can be a Transfer or stall callback
		ptr_job->call_trans(UDD_EP_TRANSFER_ABORT, ptr_job->nb_trans, ep);
	}
}


bool udd_ep_wait_stall_clear(udd_ep_id_t ep,
		udd_callback_halt_cleared_t callback)
{
	udd_ep_job_t *ptr_job;

	ep &= USB_EP_ADDR_MASK;
	if (USB_DEVICE_MAX_EP < ep) {
		return false;
	}

	ptr_job = &udd_ep_job[ep - 1];

	if (!Is_udd_endpoint_enabled(ep)) {
		return false; // Endpoint not enabled
	}

	// Wait clear halt endpoint
	if (ptr_job->busy == true) {
		return false; // Job already on going
	}

	if (Is_udd_endpoint_stall_requested(ep)) {
		// Endpoint halted then registers the callback
		ptr_job->busy = true;
		ptr_job->call_nohalt = callback;
	} else {
		// endpoint not halted then call directly callback
		callback();
	}
	return true;
}
#endif // (0!=USB_DEVICE_MAX_EP)


#ifdef USB_DEVICE_HS_SUPPORT

void udd_test_mode_j(void)
{
	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_j();
}


void udd_test_mode_k(void)
{
	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_k();
}


void udd_test_mode_se0_nak(void)
{
	udd_enable_hs_test_mode();
}


void udd_test_mode_packet(void)
{
	irqflags_t flags;
	const uint8_t test_packet[] = {
		// 00000000 * 9
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		// 01010101 * 8
		0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
		// 01110111 * 8
		0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
		// 0, {111111S * 15}, 111111
		0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
				0xFF, 0xFF,
		// S, 111111S, {0111111S * 7}
		0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD,
		// 00111111, {S0111111 * 9}, S0
		0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0x7E
	};

	// Reconfigure control endpoint to bulk IN endpoint
	udd_disable_endpoint(0);
	udd_configure_endpoint(0, USB_EP_TYPE_BULK, 1,
			64, AVR32_USBC_UECFG0_EPBK_SINGLE);

	udd_enable_hs_test_mode();
	udd_enable_hs_test_mode_packet();

	// Send packet on endpoint 0
	udd_udesc_set_buf0_addr(0, (uint8_t *) test_packet);
	flags = cpu_irq_save();
	udd_enable_in_send_interrupt(0);
	cpu_irq_restore(flags);

	udd_ack_in_send(0);
}
#endif // USB_DEVICE_HS_SUPPORT



//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE CONTROL ENDPOINT

static void udd_reset_ep_ctrl(void)
{
	irqflags_t flags;

	// Reset USB address to 0
	udd_configure_address(0);
	udd_enable_address();

	// Alloc and configure control endpoint
	udd_configure_endpoint(0,
			USB_EP_TYPE_CONTROL,
			0,
			USB_DEVICE_EP_CTRL_SIZE, AVR32_USBC_UECFG0_EPBK_SINGLE);

	// Use internal buffer for endpoint control
	udd_udesc_set_buf0_addr(0, udd_ctrl_buffer);

	// don't use multipacket on endpoint control
	udd_udesc_rst_buf0_size(0);
	udd_enable_endpoint(0);
	udd_disable_busy_bank0(0);
	flags = cpu_irq_save();
	udd_enable_setup_received_interrupt(0);
	udd_enable_out_received_interrupt(0);
	udd_enable_endpoint_interrupt(0);
	cpu_irq_restore(flags);
}

static void udd_ctrl_init(void)
{
	// In case of abort of IN Data Phase:
	// No need to abort IN transfer (rise TXINI),
	// because it is automatically done by hardware when a Setup packet is received.
	// But the interrupt must be disabled to don't generate interrupt TXINI
	// after SETUP reception.
	udd_disable_in_send_interrupt(0);
	// In case of OUT ZLP event is no processed before Setup event occurs
	udd_ack_out_received(0);

	udd_g_ctrlreq.callback = NULL;
	udd_g_ctrlreq.over_under_run = NULL;
	udd_g_ctrlreq.payload_size = 0;
	udd_ep_control_state = UDD_EPCTRL_SETUP;
}


static void udd_ctrl_setup_received(void)
{
	irqflags_t flags;

	if (UDD_EPCTRL_SETUP != udd_ep_control_state) {
		// May be a hidden DATA or ZLP phase
		// or protocol abort
		udd_ctrl_endofrequest();

		// Reinitializes control endpoint management
		udd_ctrl_init();
	}
	// Fill setup request structure
	if (8 != udd_udesc_get_buf0_ctn(0)) {
		udd_ctrl_stall_data();
		udd_ack_setup_received(0);
		return; // Error data number doesn't correspond to SETUP packet
	}
	memcpy((uint8_t *) & udd_g_ctrlreq.req, udd_ctrl_buffer, 8);

	// Manage LSB/MSB to fit with CPU usage
	udd_g_ctrlreq.req.wValue = le16_to_cpu(udd_g_ctrlreq.req.wValue);
	udd_g_ctrlreq.req.wIndex = le16_to_cpu(udd_g_ctrlreq.req.wIndex);
	udd_g_ctrlreq.req.wLength = le16_to_cpu(udd_g_ctrlreq.req.wLength);

	// Decode setup request
	if (udc_process_setup() == false) {
		// Setup request unknown then stall it
		udd_ctrl_stall_data();
		udd_ack_setup_received(0);
		return;
	}
	udd_ack_setup_received(0);

	if (Udd_setup_is_in()) {
		// IN data phase requested
		udd_ctrl_prev_payload_nb_trans = 0;
		udd_ctrl_payload_nb_trans = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_IN;
		udd_ctrl_in_sent(); // Send first data transfer
	} else {
		if (0 == udd_g_ctrlreq.req.wLength) {
			// No data phase requested
			// Send IN ZLP to ACK setup request
			udd_ctrl_send_zlp_in();
			return;
		}

		// OUT data phase requested
		udd_ctrl_prev_payload_nb_trans = 0;
		udd_ctrl_payload_nb_trans = 0;
		udd_ep_control_state = UDD_EPCTRL_DATA_OUT;

		// To detect a protocol error, enable nak interrupt on data IN phase
		udd_ack_nak_in(0);
		flags = cpu_irq_save();
		udd_enable_nak_in_interrupt(0);
		cpu_irq_restore(flags);
	}
}


static void udd_ctrl_in_sent(void)
{
	static bool b_shortpacket = false;
	uint16_t nb_remain;
	irqflags_t flags;

	flags = cpu_irq_save();
	udd_disable_in_send_interrupt(0);
	cpu_irq_restore(flags);

	if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		// ZLP on IN is sent, then valid end of setup request
		udd_ctrl_endofrequest();
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	Assert(udd_ep_control_state == UDD_EPCTRL_DATA_IN);

	nb_remain = udd_g_ctrlreq.payload_size - udd_ctrl_payload_nb_trans;
	if (0 == nb_remain) {
		// All content of current buffer payload are sent
		// Update number of total data sending by previous payload buffer
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;
		if ((udd_g_ctrlreq.req.wLength == udd_ctrl_prev_payload_nb_trans)
				|| b_shortpacket) {
			// All data requested are transfered or a short packet has been sent
			// then it is the end of data phase.
			// Generate an OUT ZLP for handshake phase.
			udd_ctrl_send_zlp_out();
			return;
		}
		// Need of new buffer because the data phase is not complete
		if ((!udd_g_ctrlreq.over_under_run)
				|| (!udd_g_ctrlreq.over_under_run())) {
			// Underrun then send zlp on IN
			// Here nb_remain=0, this allows to send a IN ZLP
		} else {
			// A new payload buffer is given
			udd_ctrl_payload_nb_trans = 0;
			nb_remain = udd_g_ctrlreq.payload_size;
		}
	}
	// Continue transfer and send next data
	if (nb_remain >= USB_DEVICE_EP_CTRL_SIZE) {
		nb_remain = USB_DEVICE_EP_CTRL_SIZE;
		b_shortpacket = false;
	} else {
		b_shortpacket = true;
	}
	//** Critical section
	// Only in case of DATA IN phase abort without USB Reset signal after.
	// The IN data don't must be written in endpoint 0 DPRAM during
	// a next setup reception in same endpoint 0 DPRAM.
	// Thereby, an OUT ZLP reception must check before IN data write
	// and if no OUT ZLP is received the data must be written quickly (800us)
	// before an eventually ZLP OUT and SETUP reception
	flags = cpu_irq_save();
	if (Is_udd_out_received(0)) {
		// IN DATA phase aborted by OUT ZLP
		cpu_irq_restore(flags);
		udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;
		return; // Exit of IN DATA phase
	}
	// Write quickly the IN data
	memcpy(udd_ctrl_buffer,
			udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans,
			nb_remain);
	udd_ctrl_payload_nb_trans += nb_remain;
	udd_udesc_set_buf0_ctn(0, nb_remain);

	// Validate and send the data available in the control endpoint buffer
	udd_ack_in_send(0);
	udd_enable_in_send_interrupt(0);

	// In case of abort of DATA IN phase, no need to enable nak OUT interrupt
	// because OUT endpoint is already free and ZLP OUT accepted.
	cpu_irq_restore(flags);
}


static void udd_ctrl_out_received(void)
{
	irqflags_t flags;
	uint16_t nb_data;

	if (UDD_EPCTRL_DATA_OUT != udd_ep_control_state) {
		if ((UDD_EPCTRL_DATA_IN == udd_ep_control_state) ||
				(UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state)) {
			// End of SETUP request:
			// - Data IN Phase aborted,
			// - or last Data IN Phase hidden by ZLP OUT sending quickly,
			// - or ZLP OUT received normally.
			udd_ctrl_endofrequest();
		} else {
			// Protocol error during SETUP request
			udd_ctrl_stall_data();
		}
		// Reinitializes control endpoint management
		udd_ctrl_init();
		return;
	}
	// Read data received during OUT phase
	nb_data = udd_udesc_get_buf0_ctn(0);
	if (udd_g_ctrlreq.payload_size < (udd_ctrl_payload_nb_trans + nb_data)) {
		// Payload buffer too small
		nb_data = udd_g_ctrlreq.payload_size -
				udd_ctrl_payload_nb_trans;
	}

	memcpy((uint8_t *) (udd_g_ctrlreq.payload + udd_ctrl_payload_nb_trans),
			udd_ctrl_buffer, nb_data);
	udd_ctrl_payload_nb_trans += nb_data;

	if ((USB_DEVICE_EP_CTRL_SIZE != nb_data) ||
			(udd_g_ctrlreq.req.wLength <=
			(udd_ctrl_prev_payload_nb_trans + udd_ctrl_payload_nb_trans))) {
		// End of reception because it is a short packet
		// Before send ZLP, call intermediate callback
		// in case of data receive generate a stall
		udd_g_ctrlreq.payload_size = udd_ctrl_payload_nb_trans;
		if (NULL != udd_g_ctrlreq.over_under_run) {
			if (!udd_g_ctrlreq.over_under_run()) {
				// Stall ZLP
				udd_ctrl_stall_data();

				// Ack reception of OUT to replace NAK by a STALL
				udd_ack_out_received(0);
				return;
			}
		}
		// Send IN ZLP to ACK setup request
		udd_ack_out_received(0);
		udd_ctrl_send_zlp_in();
		return;
	}

	if (udd_g_ctrlreq.payload_size == udd_ctrl_payload_nb_trans) {
		// Overrun then request a new payload buffer
		if (!udd_g_ctrlreq.over_under_run) {
			// No callback available to request a new payload buffer
			udd_ctrl_stall_data();

			// Ack reception of OUT to replace NAK by a STALL
			udd_ack_out_received(0);
			return;
		}
		if (!udd_g_ctrlreq.over_under_run()) {
			// No new payload buffer delivered
			udd_ctrl_stall_data();

			// Ack reception of OUT to replace NAK by a STALL
			udd_ack_out_received(0);
			return;
		}
		// New payload buffer available
		// Update number of total data received
		udd_ctrl_prev_payload_nb_trans += udd_ctrl_payload_nb_trans;

		// Reinit reception on payload buffer
		udd_ctrl_payload_nb_trans = 0;
	}
	// Free buffer of control endpoint to authorize next reception
	udd_ack_out_received(0);

	// To detect a protocol error, enable nak interrupt on data IN phase
	udd_ack_nak_in(0);
	flags = cpu_irq_save();
	udd_enable_nak_in_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_underflow(void)
{
	if (Is_udd_out_received(0))
		return; // underflow ignored if OUT data is received

	if (UDD_EPCTRL_DATA_OUT == udd_ep_control_state) {
		// Host want to stop OUT transaction
		// then stop to wait OUT data phase and wait IN ZLP handshake
		udd_ctrl_send_zlp_in();

	} else if (UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP == udd_ep_control_state) {
		// A OUT handshake is waiting by device,
		// but host want extra IN data then stall extra IN data
		udd_enable_stall_handshake(0);
	}
}


static void udd_ctrl_overflow(void)
{
	if (Is_udd_in_send(0)) {
		return; // overflow ignored if IN data is received
	}

	// The case of UDD_EPCTRL_DATA_IN is not managed
	// because the OUT endpoint is already free and OUT ZLP accepted

	if (UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP == udd_ep_control_state) {
		// A IN handshake is waiting by device,
		// but host want extra OUT data then stall extra OUT data
		udd_enable_stall_handshake(0);
	}
}


static void udd_ctrl_stall_data(void)
{
	// Stall all packets on IN & OUT control endpoint
	udd_ep_control_state = UDD_EPCTRL_STALL_REQ;
	udd_enable_stall_handshake(0);
}


static void udd_ctrl_send_zlp_in(void)
{
	irqflags_t flags;

	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_IN_ZLP;

	// Validate and send empty IN packet on control endpoint
	udd_udesc_rst_buf0_ctn(0);

	flags = cpu_irq_save();

	// Send ZLP on IN endpoint
	udd_ack_in_send(0);
	udd_enable_in_send_interrupt(0);

	// To detect a protocol error, enable nak interrupt on data OUT phase
	udd_ack_nak_out(0);
	udd_enable_nak_out_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_send_zlp_out(void)
{
	irqflags_t flags;

	udd_ep_control_state = UDD_EPCTRL_HANDSHAKE_WAIT_OUT_ZLP;

	// To detect a protocol error, enable nak interrupt on data IN phase
	flags = cpu_irq_save();
	udd_ack_nak_in(0);
	udd_enable_nak_in_interrupt(0);
	cpu_irq_restore(flags);
}


static void udd_ctrl_endofrequest(void)
{
	// If a callback is registered then call it
	if (udd_g_ctrlreq.callback) {
		udd_g_ctrlreq.callback();
	}
}


static bool udd_ctrl_interrupt(void)
{

	if (!Is_udd_endpoint_interrupt(0)) {
		return false; // No interrupt events on control endpoint
	}

	// By default disable overflow and underflow interrupt
	udd_disable_nak_in_interrupt(0);
	udd_disable_nak_out_interrupt(0);

	// Search event on control endpoint
	if (Is_udd_setup_received(0)) {
		// SETUP packet received
		udd_ctrl_setup_received();
		return true;
	}
	if (Is_udd_out_received(0)) {
		// OUT packet received
		udd_ctrl_out_received();
		return true;
	}
	if (Is_udd_in_send(0) && Is_udd_in_send_interrupt_enabled(0)) {
		// IN packet sent
		udd_ctrl_in_sent();
		return true;
	}
	if (Is_udd_nak_out(0)) {
		// Overflow on OUT packet
		udd_ack_nak_out(0);
		udd_ctrl_overflow();
		return true;
	}
	if (Is_udd_nak_in(0)) {
		// Underflow on IN packet
		udd_ack_nak_in(0);
		udd_ctrl_underflow();
		return true;
	}
	return false;
}


//--------------------------------------------------------
//--- INTERNAL ROUTINES TO MANAGED THE BULK/INTERRUPT/ISOCHRONOUS ENDPOINTS

#if (0!=USB_DEVICE_MAX_EP)

static void udd_ep_trans_done(udd_ep_id_t ep)
{
	udd_ep_job_t *ptr_job;
	uint16_t ep_size, nb_trans;
	uint16_t next_trans;
	udd_ep_id_t ep_num;
	irqflags_t flags;

	ep_num = ep & USB_EP_ADDR_MASK;
	ep_size = udd_get_endpoint_size(ep_num);

	// Get job corresponding at endpoint
	ptr_job = &udd_ep_job[ep_num - 1];

	// Disable interrupt of endpoint
	flags = cpu_irq_save();
	udd_disable_endpoint_interrupt(ep_num);
	cpu_irq_restore(flags);

	if (!ptr_job->busy) {
		return; // No job is running, then ignore it (system error)
	}

	if (USB_EP_DIR_IN == (ep & USB_EP_DIR_IN)) {
		// Transfer complete on IN
		nb_trans = udd_udesc_get_buf0_size(ep_num);

		// Lock emission of new IN packet
		udd_enable_busy_bank0(ep_num);

		// Ack interrupt
		udd_ack_in_send(ep_num);

		if (0 == nb_trans) {
			if (0 == udd_nb_busy_bank(ep_num)) {
				// All byte are transfered than take nb byte requested
				nb_trans = udd_udesc_get_buf0_ctn(ep_num);
			}
		}
		// Update number of data transfered
		ptr_job->nb_trans += nb_trans;

		// Need to send other data
		if ((ptr_job->nb_trans != ptr_job->buf_size)
				|| ptr_job->b_shortpacket) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				// The USB hardware support a maximum
				// transfer size of UDD_ENDPOINT_MAX_TRANS Bytes
				next_trans = UDD_ENDPOINT_MAX_TRANS -
						(UDD_ENDPOINT_MAX_TRANS % ep_size);
				udd_udesc_set_buf0_autozlp(ep_num, false);
			} else {
				// Need ZLP, if requested and last packet is not a short packet
				udd_udesc_set_buf0_autozlp(ep_num, ptr_job->b_shortpacket);
				ptr_job->b_shortpacket = false; // No need to request another ZLP
			}

			udd_udesc_set_buf0_ctn(ep_num, next_trans);
			udd_udesc_rst_buf0_size(ep_num);

			// Link the user buffer directly on USB hardware DMA
			udd_udesc_set_buf0_addr(ep_num,
					&ptr_job->buf[ptr_job->nb_trans]);

			// Start transfer
			udd_ack_fifocon(ep_num);
			udd_disable_busy_bank0(ep_num);

			// Enable interrupt
			flags = cpu_irq_save();
			udd_enable_in_send_interrupt(ep_num);
			udd_enable_endpoint_interrupt(ep_num);
			cpu_irq_restore(flags);
			return;
		}
	} else {
		// Transfer complete on OUT
		nb_trans = udd_udesc_get_buf0_ctn(ep_num);

		// Lock reception of new OUT packet
		udd_enable_busy_bank0(ep_num);

		// Ack interrupt
		udd_ack_out_received(ep_num);
		udd_ack_fifocon(ep_num);

		// Can be necessary to copy data receive from cache buffer to user buffer
		if (ptr_job->b_use_out_cache_buffer) {
			memcpy(&ptr_job->buf[ptr_job->nb_trans],
					udd_ep_out_cache_buffer[ep_num - 1],
					ptr_job->buf_size % ep_size);
		}

		// Update number of data transfered
		ptr_job->nb_trans += nb_trans;
		if (ptr_job->nb_trans > ptr_job->buf_size) {
			ptr_job->nb_trans = ptr_job->buf_size;
		}

		// If all previous data requested are received and user buffer not full
		// then need to receive other data
		if ((nb_trans == udd_udesc_get_buf0_size(ep_num))
			&& (ptr_job->nb_trans != ptr_job->buf_size)) {
			next_trans = ptr_job->buf_size - ptr_job->nb_trans;
			if (UDD_ENDPOINT_MAX_TRANS < next_trans) {
				// The USB hardware support a maximum transfer size
				// of UDD_ENDPOINT_MAX_TRANS Bytes
				next_trans = UDD_ENDPOINT_MAX_TRANS
						- (UDD_ENDPOINT_MAX_TRANS % ep_size);
			} else {
				next_trans -= next_trans % ep_size;
			}

			udd_udesc_rst_buf0_ctn(ep_num);
			if (next_trans < ep_size) {
				// Use the cache buffer for Bulk or Interrupt size endpoint
				ptr_job->b_use_out_cache_buffer = true;
				udd_udesc_set_buf0_addr(ep_num,
						udd_ep_out_cache_buffer[ep_num-1]);
				udd_udesc_set_buf0_size(ep_num, ep_size);
			} else {
				// Link the user buffer directly on USB hardware DMA
				udd_udesc_set_buf0_addr(ep_num, &ptr_job->buf[ptr_job->nb_trans]);
				udd_udesc_set_buf0_size(ep_num, next_trans);
			}
			// Start transfer
			udd_disable_busy_bank0(ep_num);

			// Enable interrupt
			flags = cpu_irq_save();
			udd_enable_out_received_interrupt(ep_num);
			udd_enable_endpoint_interrupt(ep_num);
			cpu_irq_restore(flags);
			return;
		}
	}

	// Job complete then call callback
	ptr_job->busy = false;
	if (NULL != ptr_job->call_trans) {
		ptr_job->call_trans(UDD_EP_TRANSFER_OK, ptr_job->nb_trans, ep);
	}
	return;
}


static bool udd_ep_interrupt(void)
{
	udd_ep_id_t ep, ep_addr;

	// For each endpoint different of control endpoint (0)
	for (ep = 1; ep <= USB_DEVICE_MAX_EP; ep++) {
		if (!Is_udd_endpoint_interrupt_enabled(ep)
				|| !Is_udd_endpoint_interrupt(ep)) {
			continue;
		}
		ep_addr = Is_udd_endpoint_in(ep) ? (ep | USB_EP_DIR_IN) : ep;
		udd_ep_trans_done(ep_addr);
		return true;
	}
	return false;
}
#endif // (0!=USB_DEVICE_MAX_EP)

//@}
//@}
