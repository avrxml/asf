/**
 * \file
 *
 * \brief USBC Host Driver header file.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _USBC_HOST_H_
#define _USBC_HOST_H_

#include "compiler.h"
#include "preprocessor.h"
#include "usbc_otg.h"

// These defines are missing from the toolchain header file
#ifndef AVR32_USBC_UPSTA0CLR_PERRIC
#  define AVR32_USBC_UPSTA0CLR_PERRIC                                  3
#  define AVR32_USBC_UPSTA0CLR_PERRIC_MASK                    0x00000008
#  define AVR32_USBC_UPSTA0CLR_PERRIC_OFFSET                           3
#  define AVR32_USBC_UPSTA0CLR_PERRIC_SIZE                             1
#endif

//! \ingroup uhd_group
//! \defgroup uhd_usbc_group USBC Host Driver
//! USBC low-level driver for USB host mode
//!
//! @warning Bit-masks are used instead of bit-fields because PB registers
//! require 32-bit write accesses while AVR32-GCC 4.0.2 builds 8-bit
//! accesses even when volatile unsigned int bit-fields are specified.
//! @{


//! @name USBC Host IP properties
//! These macros give access to IP properties
//! @{
//! Get maximal number of endpoints
#define uhd_get_pipe_max_nbr() \
	(((USBC_RD_BITFIELD(UFEATURES_EPT_NBR,MAX) - 1) \
	& ((1 << AVR32_USBC_UFEATURES_EPT_NBR_MAX_SIZE) - 1)) + 1)
//! @}

//! @name Host Vbus line control
//!
//! VBOF is an optional output pin which allows to enable or disable
//! the external VBus generator.
//!
//! @{
//! Enables hardware control of USB_VBOF output pin when a Vbus error occur
#define uhd_enable_vbus_error_hw_control()    USBC_CLR_BITS(USBCON,VBUSHWC)
//! Disables hardware control of USB_VBOF output pin when a Vbus error occur
#define uhd_disable_vbus_error_hw_control()   USBC_SET_BITS(USBCON,VBUSHWC)

//! Pin and function for USB_VBOF according to configuration from USB_VBOF
#define USB_VBOF_PIN            ATPASTE2(USB_VBOF, _PIN)
#define USB_VBOF_FUNCTION       ATPASTE2(USB_VBOF, _FUNCTION)
//! Output USB_VBOF onto its pin
#define uhd_output_vbof_pin() {\
	(Tst_bits(USB_VBOF_FUNCTION, 0x01)) ?\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0s = 1 << (USB_VBOF_PIN & 0x1F)) :\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0c = 1 << (USB_VBOF_PIN & 0x1F)); \
	(Tst_bits(USB_VBOF_FUNCTION, 0x02)) ?\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1s = 1 << (USB_VBOF_PIN & 0x1F)) :\
	(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1c = 1 << (USB_VBOF_PIN & 0x1F)); \
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].gperc  = 1 << (USB_VBOF_PIN & 0x1F);\
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].odmerc = 1 << (USB_VBOF_PIN & 0x1F);\
	AVR32_GPIO.port[USB_VBOF_PIN >> 5].puerc  = 1 << (USB_VBOF_PIN & 0x1F); }
//! Set USB_VBOF output pin polarity
#define uhd_set_vbof_active_high()            USBC_CLR_BITS(USBCON,VBUSPO)
#define uhd_set_vbof_active_low()             USBC_SET_BITS(USBCON,VBUSPO)
//! Requests VBus activation
#define uhd_enable_vbus()                     USBC_REG_SET(USBSTA,VBUSRQ)
//! Requests VBus deactivation
#define uhd_disable_vbus()                    USBC_REG_CLR(USBSTA,VBUSRQ)
//! Tests if VBus activation has been requested
#define Is_uhd_vbus_enabled()                 USBC_TST_BITS(USBSTA,VBUSRQ)
//! @}

//! @name Host Vbus line monitoring
//!
//! The VBus level is always checked by USBC hardware.
//!
//! @{
#define uhd_enable_vbus_error_interrupt()     USBC_SET_BITS(USBCON,VBERRE)
#define uhd_disable_vbus_error_interrupt()    USBC_CLR_BITS(USBCON,VBERRE)
#define Is_uhd_vbus_error_interrupt_enabled() USBC_TST_BITS(USBCON,VBERRE)
#define uhd_ack_vbus_error_interrupt()        USBC_REG_CLR(USBSTA,VBERRI)
#define Is_uhd_vbus_error_interrupt()         USBC_TST_BITS(USBSTA,VBERRI)
//! @}

//! @name USB device connection/disconnection monitoring
//! @{
#define uhd_enable_connection_int()           USBC_REG_SET(UHINTE,DCONNIE)
#define uhd_disable_connection_int()          USBC_REG_CLR(UHINTE,DCONNIE)
#define Is_uhd_connection_int_enabled()       USBC_TST_BITS(UHINTE,DCONNIE)
#define uhd_ack_connection()                  USBC_REG_CLR(UHINT,DCONNI)
#define Is_uhd_connection()                   USBC_TST_BITS(UHINT,DCONNI)

#define uhd_enable_disconnection_int()        USBC_REG_SET(UHINTE,DDISCIE)
#define uhd_disable_disconnection_int()       USBC_REG_CLR(UHINTE,DDISCIE)
#define Is_uhd_disconnection_int_enabled()    USBC_TST_BITS(UHINTE,DDISCIE)
#define uhd_ack_disconnection()               USBC_REG_CLR(UHINT,DDISCI)
#define Is_uhd_disconnection()                USBC_TST_BITS(UHINT,DDISCI)
//! @}

//! @name USB device speed control
//! @{
#define uhd_get_speed_mode()                  USBC_RD_BITFIELD(USBSTA,SPEED)
#define Is_uhd_low_speed_mode() \
	(USBC_RD_BITFIELD(USBSTA,SPEED) == AVR32_USBC_USBSTA_SPEED_LOW)
#define Is_uhd_full_speed_mode() \
	(USBC_RD_BITFIELD(USBSTA,SPEED) == AVR32_USBC_USBSTA_SPEED_FULL)
#define Is_uhd_high_speed_mode()            false
//! @}

//! @name Bus events control
//! These macros manage the bus events: reset, SOF, resume, wakeup.
//! @{

//! Initiates a reset event
//! @{
#define uhd_start_reset()                            USBC_SET_BITS(UHCON,RESET)
#define Is_uhd_starting_reset()                      USBC_TST_BITS(UHCON,RESET)
#define uhd_stop_reset()                             USBC_CLR_BITS(UHCON,RESET)

#define uhd_enable_reset_sent_interrupt()            USBC_REG_SET(UHINTE,RSTIE)
#define uhd_disable_reset_sent_interrupt()           USBC_REG_CLR(UHINTE,RSTIE)
#define Is_uhd_reset_sent_interrupt_enabled()        USBC_TST_BITS(UHINTE,RSTIE)
#define uhd_ack_reset_sent()                         USBC_REG_CLR(UHINT,RSTI)
#define Is_uhd_reset_sent()                          USBC_TST_BITS(UHINT,RSTI)
//! @}

//! Initiates a SOF events
//! @{
#define uhd_enable_sof()                             USBC_SET_BITS(UHCON,SOFE)
#define uhd_disable_sof()                            USBC_CLR_BITS(UHCON,SOFE)
#define Is_uhd_sof_enabled()                         USBC_TST_BITS(UHCON,SOFE)
#define uhd_get_sof_number()                         USBC_RD_BITFIELD(UHFNUM,FNUM)
#define uhd_get_microsof_number() \
		(Rd_bitfield(AVR32_USBC.uhfnum, \
		AVR32_USBC_UHFNUM_FNUM_MASK|AVR32_USBC_UHFNUM_MFNUM_MASK))
#define uhd_get_frame_position()                     USBC_RD_BITFIELD(UHFNUM,FLENHIGH)
#define uhd_enable_sof_interrupt()                   USBC_REG_SET(UHINTE,HSOFIE)
#define uhd_disable_sof_interrupt()                  USBC_REG_CLR(UHINTE,HSOFIE)
#define Is_uhd_sof_interrupt_enabled()               USBC_TST_BITS(UHINTE,HSOFIE)
#define uhd_ack_sof()                                USBC_REG_CLR(UHINT,HSOFI)
#define Is_uhd_sof()                                 USBC_TST_BITS(UHINT,HSOFI)
//! @}

//! Initiates a resume event
//! It is called downstream resume event.
//! @{
#define uhd_send_resume()                            USBC_SET_BITS(UHCON,RESUME)
#define Is_uhd_sending_resume()                      USBC_TST_BITS(UHCON,RESUME)

#define uhd_enable_downstream_resume_interrupt()     USBC_REG_SET(UHINTE,RSMEDIE)
#define uhd_disable_downstream_resume_interrupt()    USBC_REG_CLR(UHINTE,RSMEDIE)
#define Is_uhd_downstream_resume_interrupt_enabled() USBC_TST_BITS(UHINTE,RSMEDIE)
#define uhd_ack_downstream_resume()                  USBC_REG_CLR(UHINT,RSMEDI)
#define Is_uhd_downstream_resume()                   USBC_TST_BITS(UHINT,RSMEDI)
//! @}

//! Detection of a wake-up event
//! A wake-up event is received when the host controller is in the suspend mode:
//! - and an upstream resume from the peripheral is detected.
//! - and a peripheral disconnection is detected.
//! @{
#define uhd_enable_wakeup_interrupt()                USBC_REG_SET(UHINTE,HWUPIE)
#define uhd_disable_wakeup_interrupt()               USBC_REG_CLR(UHINTE,HWUPIE)
#define Is_uhd_wakeup_interrupt_enabled()            USBC_TST_BITS(UHINTE,HWUPIE)
#define uhd_ack_wakeup()                             USBC_REG_CLR(UHINT,HWUPI)
#define Is_uhd_wakeup()                              USBC_TST_BITS(UHINT,HWUPI)

#define uhd_enable_upstream_resume_interrupt()       USBC_REG_SET(UHINTE,RXRSMIE)
#define uhd_disable_upstream_resume_interrupt()      USBC_REG_CLR(UHINTE,RXRSMIE)
#define Is_uhd_upstream_resume_interrupt_enabled()   USBC_TST_BITS(UHINTE,RXRSMIE)
#define uhd_ack_upstream_resume()                    USBC_REG_CLR(UHINT,RXRSMI)
#define Is_uhd_upstream_resume()                     USBC_TST_BITS(UHINT,RXRSMI)
//! @}
//! @}


//! @name Pipes management
//! @{

//! Generic macros for USBC pipe registers that can be arrayed
//! @{
#define USBC_ARRAY(reg,index)   (((volatile unsigned long*)(&AVR32_USBC.reg))[index])
#define USBC_P_CLR_BITS(reg, bit, pipe) \
		(Clr_bits(USBC_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBC_,reg,0_,bit,_MASK)))
#define USBC_P_SET_BITS(reg, bit, pipe) \
		(Set_bits(USBC_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBC_,reg,0_,bit,_MASK)))
#define USBC_P_TST_BITS(reg, bit, pipe) \
		(Tst_bits(USBC_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBC_,reg,0_,bit,_MASK)))
#define USBC_P_RD_BITFIELD(reg, bit, pipe) \
		(Rd_bitfield(USBC_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBC_,reg,0_,bit,_MASK)))
#define USBC_P_WR_BITFIELD(reg, bit, pipe, value) \
		(Wr_bitfield(USBC_ARRAY(TPASTE2(reg,0),pipe),\
		TPASTE5(AVR32_USBC_,reg,0_,bit,_MASK), value))
#define USBC_P_REG_CLR(reg, bit, pipe) \
		(USBC_ARRAY(TPASTE2(reg,0CLR),pipe) \
		 = TPASTE5(AVR32_USBC_,reg,0CLR_,bit,C_MASK))
#define USBC_P_REG_SET(reg, bit, pipe) \
		(USBC_ARRAY(TPASTE2(reg,0SET),pipe) \
		 = TPASTE5(AVR32_USBC_,reg,0SET_,bit,S_MASK))
//! @}

//! USB address of pipes
//! @{
#define uhd_udesc_set_uhaddr(pipe,addr) \
   uhd_g_pipe_table[pipe].CTR_STA.uhaddr = addr
#define uhd_udesc_set_uhaddr_bk1(pipe,addr) \
   uhd_g_pipe_table[pipe].CTR_STA1.uhaddr = addr
#define uhd_udesc_get_uhaddr(pipe)             \
   uhd_g_pipe_table[pipe].CTR_STA.uhaddr
//! @}

//! Pipe enable
//! Enable, disable, reset, freeze
//! @{
#define uhd_enable_pipe(p) \
		(Set_bits(AVR32_USBC.uprst, AVR32_USBC_UPRST_PEN0_MASK << (p)))
#define uhd_disable_pipe(p) \
		(Clr_bits(AVR32_USBC.uprst, AVR32_USBC_UPRST_PEN0_MASK << (p)))
#define Is_uhd_pipe_enabled(p) \
		(Tst_bits(AVR32_USBC.uprst, AVR32_USBC_UPRST_PEN0_MASK << (p)))

#define uhd_freeze_pipe(p)                       USBC_P_REG_SET(UPCON,PFREEZE,p)
#define uhd_unfreeze_pipe(p)                     USBC_P_REG_CLR(UPCON,PFREEZE,p)
#define Is_uhd_pipe_frozen(p)                    USBC_P_TST_BITS(UPCON,PFREEZE,p)

#define uhd_reset_data_toggle(p)                 USBC_P_REG_CLR(UPCON,INITDTGL,p)
#define uhd_set_data_toggle(p)                   USBC_P_REG_SET(UPCON,INITDTGL,p)
#define uhd_set_bank0(p)                         USBC_P_REG_CLR(UPCON,INITBK,p)
#define uhd_set_bank1(p)                         USBC_P_REG_SET(UPCON,INITBK,p)

//! @}

//! Pipe configuration
//! @{
#define uhd_configure_pipe_binterval(p,freq)     USBC_P_WR_BITFIELD(UPCFG,BINTERVAL,p,freq)
#define uhd_get_pipe_binterval(p)                USBC_P_RD_BITFIELD(UPCFG,BINTERVAL,p)

#define uhd_udesc_set_epnum(pipe,ep) \
   uhd_g_pipe_table[pipe].CTR_STA.ep_num = ep
#define uhd_udesc_get_epnum(pipe) \
   uhd_g_pipe_table[pipe].CTR_STA.ep_num

#define uhd_configure_pipe_type(p, type)         USBC_P_WR_BITFIELD(UPCFG,PTYPE,p, type)
#define uhd_get_pipe_type(p)                     USBC_P_RD_BITFIELD(UPCFG,PTYPE,p)

#define uhd_configure_pipe_token(p, token)       USBC_P_WR_BITFIELD(UPCFG,PTOKEN,p, token)
#define uhd_get_pipe_token(p)                    USBC_P_RD_BITFIELD(UPCFG,PTOKEN,p)
#define uhd_is_pipe_in(p)                        (AVR32_USBC_UPCFG0_PTOKEN_IN==uhd_get_pipe_token(p))
#define uhd_is_pipe_out(p)                       (AVR32_USBC_UPCFG0_PTOKEN_OUT==uhd_get_pipe_token(p))

#define uhd_get_pipe_endpoint_address(p) \
		(uhd_udesc_get_epnum(p) \
		| (uhd_is_pipe_in(p)? USB_EP_DIR_IN : USB_EP_DIR_OUT))

//! Bounds given integer size to allowed range and rounds it up to the nearest
//! available greater size, then applies register format of USBC controller
//! for pipe size bit-field.
#define uhd_format_pipe_size(size) \
		(32 - clz(((uint32_t)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
#define uhd_configure_pipe_size(p,size) \
		USBC_P_WR_BITFIELD(UPCFG,PSIZE,p, uhd_format_pipe_size(size))
#define uhd_get_pipe_size(p)                     (8 << USBC_P_RD_BITFIELD(UPCFG,PSIZE,p))
#define uhd_configure_pipe_bank(p,bank)          USBC_P_WR_BITFIELD(UPCFG,PBK,p,bank)
#define uhd_get_pipe_bank(p)                     USBC_P_RD_BITFIELD(UPCFG,PBK,p)

#define uhd_udesc_set_errormax(pipe,val)             \
   uhd_g_pipe_table[pipe].CTR_STA.error_number_max = val

#define uhd_configure_pipe(p, bInterval, ep_num, type, token, size, bank) { \
	USBC_ARRAY(upcfg0,p) = \
	(((uint32_t)(bInterval) << AVR32_USBC_UPCFG0_BINTERVAL_OFFSET) & AVR32_USBC_UPCFG0_BINTERVAL_MASK) |\
	(((uint32_t)(type  ) << AVR32_USBC_UPCFG0_PTYPE_OFFSET ) & AVR32_USBC_UPCFG0_PTYPE_MASK ) |\
	(((uint32_t)(token ) << AVR32_USBC_UPCFG0_PTOKEN_OFFSET) & AVR32_USBC_UPCFG0_PTOKEN_MASK) |\
	((uint32_t)uhd_format_pipe_size(size) << AVR32_USBC_UPCFG0_PSIZE_OFFSET               ) |\
	(((uint32_t)(bank  ) << AVR32_USBC_UPCFG0_PBK_OFFSET   ) & AVR32_USBC_UPCFG0_PBK_MASK   ); \
	uhd_udesc_set_epnum(p,ep_num); \
	uhd_udesc_set_errormax(p,3); \
}
//! @}

//! Pipe main interrupts management
//! @{
#define uhd_enable_pipe_interrupt(p) \
		(AVR32_USBC.uhinteset = AVR32_USBC_UHINTESET_P0INTES_MASK << (p))
#define uhd_disable_pipe_interrupt(p) \
		(AVR32_USBC.uhinteclr = AVR32_USBC_UHINTECLR_P0INTEC_MASK << (p))
#define Is_uhd_pipe_interrupt_enabled(p) \
		(Tst_bits(AVR32_USBC.uhinte, AVR32_USBC_UHINTE_P0INTE_MASK << (p)))
#define Is_uhd_pipe_interrupt(p) \
		(Tst_bits(AVR32_USBC.uhint, AVR32_USBC_UHINT_P0INT_MASK << (p)))
//! returns the lowest pipe number generating a pipe interrupt or AVR32_USBC_EPT_NUM if none
#define uhd_get_interrupt_pipe_number() \
	(ctz(((AVR32_USBC.uhint >> AVR32_USBC_UHINT_P0INT_OFFSET) & \
	(AVR32_USBC.uhinte >> AVR32_USBC_UHINTE_P0INTE_OFFSET)) | (1 << AVR32_USBC_EPT_NUM)))
//! @}

//! Pipe overflow and underflow for isochronous and interrupt endpoints
//! @{
#define uhd_enable_errorflow_interrupt(p)         USBC_P_REG_SET(UPCON,ERRORFIE,p)
#define uhd_disable_errorflow_interrupt(p)        USBC_P_REG_CLR(UPCON,ERRORFIE,p)
#define Is_uhd_errorflow_interrupt_enabled(p)     USBC_P_TST_BITS(UPCON,ERRORFIE,p)
#define uhd_ack_errorflow_interrupt(p)            USBC_P_REG_CLR(UPSTA,ERRORFI,p)
#define Is_uhd_errorflow(p)                       USBC_P_TST_BITS(UPSTA,ERRORFI,p)
//! @}

//! USB packet errors management
//! @{
#define uhd_enable_stall_interrupt(p)            USBC_P_REG_SET(UPCON,RXSTALLDE,p)
#define uhd_disable_stall_interrupt(p)           USBC_P_REG_CLR(UPCON,RXSTALLDE,p)
#define Is_uhd_stall_interrupt_enabled(p)        USBC_P_TST_BITS(UPCON,RXSTALLDE,p)
#define uhd_ack_stall(p)                         USBC_P_REG_CLR(UPSTA,RXSTALLDI,p)
#define Is_uhd_stall(p)                          USBC_P_TST_BITS(UPSTA,RXSTALLDI,p)

#define uhd_enable_crc_error_interrupt(p)        USBC_P_REG_SET(UPCON,RXSTALLDE,p)
#define uhd_disable_crc_error_interrupt(p)       USBC_P_REG_CLR(UPCON,RXSTALLDE,p)
#define Is_uhd_crc_error_interrupt_enabled(p)    USBC_P_TST_BITS(UPCON,RXSTALLDE,p)
#define uhd_ack_crc_error(p)                     USBC_P_REG_CLR(UPSTA,RXSTALLDI,p)
#define Is_uhd_crc_error(p)                      USBC_P_TST_BITS(UPSTA,RXSTALLDI,p)

#define uhd_enable_pipe_error_interrupt(p)       USBC_P_REG_SET(UPCON,PERRE,p)
#define uhd_disable_pipe_error_interrupt(p)      USBC_P_REG_CLR(UPCON,PERRE,p)
#define Is_uhd_pipe_error_interrupt_enabled(p)   USBC_P_TST_BITS(UPCON,PERRE,p)
#define uhd_ack_pipe_error(p)                    USBC_P_REG_CLR(UPSTA,PERRI,p)
#define Is_uhd_pipe_error(p)                     USBC_P_TST_BITS(UPSTA,PERRI,p)
#define uhd_raise_pipe_error(p)                  USBC_P_REG_SET(UPSTA,PERRI,p)

#define USBC_UPERR_UPERR0_DATATGL_MASK    (1<<0)
#define USBC_UPERR_UPERR0_DATAPID_MASK    (1<<1)
#define USBC_UPERR_UPERR0_PID_MASK        (1<<2)
#define USBC_UPERR_UPERR0_TIMEOUT_MASK    (1<<3)
#define USBC_UPERR_UPERR0_CRC16_MASK      (1<<4)

#define uhd_udesc_get_error_status(pipe) \
   uhd_g_pipe_table[pipe].CTR_STA.error_status
#define uhd_udesc_ack_error_status(pipe) \
   uhd_g_pipe_table[pipe].CTR_STA.error_status = 0
//! @}

//! Pipe data management
//! @{
#define uhd_data_toggle(p)                       USBC_P_RD_BITFIELD(UPSTA,DTSEQ,p)

#define uhd_enable_bank_interrupt(p)             USBC_P_REG_SET(UPCON,NBUSYBKE,p)
#define uhd_disable_bank_interrupt(p)            USBC_P_REG_CLR(UPCON,NBUSYBKE,p)
#define Is_uhd_bank_interrupt_enabled(p)         USBC_P_TST_BITS(UPCON,NBUSYBKE,p)
#define uhd_nb_busy_bank(p)                      USBC_P_RD_BITFIELD(UPSTA,NBUSYBK,p)
#define uhd_current_bank(p)                      USBC_P_RD_BITFIELD(UPSTA,CURRBK,p)


#define uhd_udesc_set_buf0_addr(pipe,buf)                \
   uhd_g_pipe_table[pipe].pipe_add_bank0 = buf
#define uhd_udesc_rst_buf0_size(pipe)                    \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.multi_packet_size = 0
#define uhd_udesc_get_buf0_size(pipe)                    \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.multi_packet_size
#define uhd_udesc_set_buf0_size(pipe,size)               \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.multi_packet_size = size
#define uhd_udesc_rst_buf0_ctn(pipe)                     \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.byte_count = 0
#define uhd_udesc_get_buf0_ctn(pipe)                     \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.byte_count
#define uhd_udesc_set_buf0_ctn(pipe,size)                \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.byte_count = size
#define uhd_udesc_set_buf0_autozlp(pipe,val)             \
   uhd_g_pipe_table[pipe].PCKSIZE_BK0.auto_zlp = val


// Maximum size of a transfer in multipacket mode
#define UHD_ENDPOINT_MAX_TRANS ((32*1024)-1)

#define uhd_enable_ram_eccess_error_interrupt(p) USBC_P_REG_SET(UPCON,RAMACERIE,p)
#define uhd_disable_ram_eccess_error_interrupt(p) USBC_P_REG_CLR(UPCON,RAMACERIE,p)
#define Is_uhd_ram_eccess_error_interrupt_enabled(p) USBC_P_TST_BITS(UPCON,RAMACERIE,p)
#define uhd_ack_ram_eccess_error(p)              USBC_P_REG_CLR(UPSTA,RAMACERI,p)
#define Is_uhd_ram_eccess_error(p)               USBC_P_TST_BITS(UPSTA,RAMACERI,p)

#define Is_uhd_fifocon(p)                        USBC_P_TST_BITS(UPCON,FIFOCON,p)
#define uhd_ack_fifocon(p)                       USBC_P_REG_CLR(UPCON,FIFOCON,p)

#define uhd_enable_setup_ready_interrupt(p)      USBC_P_REG_SET(UPCON,TXSTPE,p)
#define uhd_disable_setup_ready_interrupt(p)     USBC_P_REG_CLR(UPCON,TXSTPE,p)
#define Is_uhd_setup_ready_interrupt_enabled(p)  USBC_P_TST_BITS(UPCON,TXSTPE,p)
#define uhd_ack_setup_ready(p)                   USBC_P_REG_CLR(UPSTA,TXSTPI,p)
#define Is_uhd_setup_ready(p)                    USBC_P_TST_BITS(UPSTA,TXSTPI,p)

#define uhd_enable_in_received_interrupt(p)      USBC_P_REG_SET(UPCON,RXINE,p)
#define uhd_disable_in_received_interrupt(p)     USBC_P_REG_CLR(UPCON,RXINE,p)
#define Is_uhd_in_received_interrupt_enabled(p)  USBC_P_TST_BITS(UPCON,RXINE,p)
#define uhd_ack_in_received(p)                   USBC_P_REG_CLR(UPSTA,RXINI,p)
#define Is_uhd_in_received(p)                    USBC_P_TST_BITS(UPSTA,RXINI,p)

#define uhd_enable_out_ready_interrupt(p)        USBC_P_REG_SET(UPCON,TXOUTE,p)
#define uhd_disable_out_ready_interrupt(p)       USBC_P_REG_CLR(UPCON,TXOUTE,p)
#define Is_uhd_out_ready_interrupt_enabled(p)    USBC_P_TST_BITS(UPCON,TXOUTE,p)
#define uhd_ack_out_ready(p)                     USBC_P_REG_CLR(UPSTA,TXOUTI,p)
#define Is_uhd_out_ready(p)                      USBC_P_TST_BITS(UPSTA,TXOUTI,p)
#define uhd_raise_out_ready(p)                   USBC_P_REG_SET(UPSTA,TXOUTI,p)

#define uhd_enable_nak_received_interrupt(p)     USBC_P_REG_SET(UPCON,NAKEDE,p)
#define uhd_disable_nak_received_interrupt(p)    USBC_P_REG_CLR(UPCON,NAKEDE,p)
#define Is_uhd_nak_received_interrupt_enabled(p) USBC_P_TST_BITS(UPCON,NAKEDE,p)
#define uhd_ack_nak_received(p)                  USBC_P_REG_CLR(UPSTA,NAKEDI,p)
#define Is_uhd_nak_received(p)                   USBC_P_TST_BITS(UPSTA,NAKEDI,p)

#define uhd_enable_continuous_in_mode(p)         USBC_P_SET_BITS(UPINRQ,INMODE,p)
#define uhd_disable_continuous_in_mode(p)        USBC_P_CLR_BITS(UPINRQ,INMODE,p)
#define Is_uhd_continuous_in_mode_enabled(p)     USBC_P_TST_BITS(UPINRQ,INMODE,p)

#define uhd_in_request_number(p,in_num)          USBC_P_WR_BITFIELD(UPINRQ,INRQ,p,in_num)
#define uhd_get_in_request_number(p)             USBC_P_RD_BITFIELD(UPINRQ,INRQ,p)

  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected pipe.
  //! @param p      Target Pipe number
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define uhd_get_pipe_fifo_access(p, scale) \
          (((volatile TPASTE3(uint, scale, _t) (*)[0x10000 / ((scale) / 8)])AVR32_USBC_SLAVE)[(p)])
//! @}

//! @name USB host pipe descriptor table management
//! @{
COMPILER_PACK_SET(1)

struct avr32_usbc_pcksize_bk_t {
	unsigned int auto_zlp:1;
	unsigned int multi_packet_size:15;
	unsigned int res:1;
	unsigned int byte_count:15;
};

struct avr32_usbc_ctr_sta_bk_t {
	unsigned int res:13;
	unsigned int underfi:1;
	unsigned int overfi:1;
	unsigned int crc:1;
	unsigned int res2:16;
};

struct avr32_usbc_ctr_sta_t {
	unsigned int res:8;
	unsigned int error_status:8;
	unsigned int error_number_max:4;
	unsigned int ep_num:4;
	unsigned int res2:1;
	unsigned int uhaddr:7;
};

typedef struct {
	uint32_t *pipe_add_bank0;
	union {
		unsigned long pcksize_bk0;
		struct avr32_usbc_pcksize_bk_t PCKSIZE_BK0;
	};
	union {
		unsigned long ctr_sta_bk0;
		struct avr32_usbc_ctr_sta_bk_t CTR_STA_BK0;
	};
	union {
		unsigned long ctr_sta;
		struct avr32_usbc_ctr_sta_t CTR_STA;
	};
	uint32_t *pipe_add_bank1;
	union {
		unsigned long pcksize_bk1;
		struct avr32_usbc_pcksize_bk_t PCKSIZE_BK1;
	};
	union {
		unsigned long ctr_sta_bk1;
		struct avr32_usbc_ctr_sta_bk_t CTR_STA_BK1;
	};
	union {
		unsigned long reserved;
		unsigned long ctr_sta1;
		struct avr32_usbc_ctr_sta_t CTR_STA1;
	};
} uhd_desc_table_t;

COMPILER_PACK_RESET()
//! @}

//! @}
//! @}

#endif // _USBC_HOST_H_
