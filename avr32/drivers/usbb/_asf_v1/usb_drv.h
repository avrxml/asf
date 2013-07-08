/**************************************************************************
 *
 * \file
 *
 * \brief Low-level driver for AVR32 USBB.
 *
 * This file contains the USBB low-level driver definitions.
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/


#ifndef _USB_DRV_H_
#define _USB_DRV_H_


//_____ I N C L U D E S ____________________________________________________

#include "compiler.h"
#include "preprocessor.h"
#include "usbb.h"
#include "conf_usb.h"

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#if defined(AVR32_UC3A364_H_INCLUDED) || defined(AVR32_UC3A364S_H_INCLUDED) \
    || defined(AVR32_UC3A3128S_H_INCLUDED) || defined(AVR32_UC3A3128_H_INCLUDED) \
    || defined(AVR32_UC3A3256S_H_INCLUDED) || defined(AVR32_UC3A3256_H_INCLUDED)
#undef  AVR32_USBB_EPT_NUM // define value is wrong.
#define AVR32_USBB_EPT_NUM       8
#endif

//! @defgroup USBB_low_level_driver USBB low-level driver module
//! USBB low-level driver module
//! @warning Bit-masks are used instead of bit-fields because PB registers
//! require 32-bit write accesses while AVR32-GCC 4.0.2 builds 8-bit
//! accesses even when volatile unsigned int bit-fields are specified.
//! @{

//_____ M A C R O S ________________________________________________________

//! Maximal number of USBB pipes/endpoints.
//! This is the same value as the one produced by Usb_get_pipe_endpoint_max_nbr().
//! As it is constant and known for a given target, there is no need to decrease
//! performance and to complexify program structure by using a value in memory.
//! The use of MAX_PEP_NB is hence preferred here to the use of a variable
//! initialized from Usb_get_pipe_endpoint_max_nbr().
//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#define MAX_PEP_NB            AVR32_USBB_EPT_NUM

//! @defgroup USBB_endpoints USBB endpoints
//! Identifiers of USBB endpoints (USBB device operating mode).
//! @{
#define EP_CONTROL            0
#define EP_0                  0
#define EP_1                  1
#define EP_2                  2
#define EP_3                  3
#define EP_4                  4
#define EP_5                  5
#define EP_6                  6
//! @}

//! @defgroup USBB_pipes USBB pipes
//! Identifiers of USBB pipes (USBB host operating mode).
//! @{
#define P_CONTROL             0
#define P_0                   0
#define P_1                   1
#define P_2                   2
#define P_3                   3
#define P_4                   4
#define P_5                   5
#define P_6                   6
//! @}

//! @defgroup USBB_types USBB standard types
//! List of the standard types used in USBB.
//! @{
#define IP_NAME_PART_1           1
#define IP_NAME_PART_2           2

#define DMA_BUFFER_SIZE_16_BITS  AVR32_USBB_UFEATURES_DMA_BUFFER_SIZE_16_BITS
#define DMA_BUFFER_SIZE_24_BITS  AVR32_USBB_UFEATURES_DMA_BUFFER_SIZE_24_BITS

#define AWAITVRISE_TIMER         AVR32_USBB_USBCON_TIMPAGE_A_WAIT_VRISE
  #define AWAITVRISE_TMOUT_20_MS   AVR32_USBB_USBCON_TIMVALUE_A_WAIT_VRISE_20_MS
  #define AWAITVRISE_TMOUT_50_MS   AVR32_USBB_USBCON_TIMVALUE_A_WAIT_VRISE_50_MS
  #define AWAITVRISE_TMOUT_70_MS   AVR32_USBB_USBCON_TIMVALUE_A_WAIT_VRISE_70_MS
  #define AWAITVRISE_TMOUT_100_MS  AVR32_USBB_USBCON_TIMVALUE_A_WAIT_VRISE_100_MS
#define VBBUSPULSING_TIMER       AVR32_USBB_USBCON_TIMPAGE_VB_BUS_PULSING
  #define VBBUSPULSING_TMOUT_15_MS AVR32_USBB_USBCON_TIMVALUE_VB_BUS_PULSING_15_MS
  #define VBBUSPULSING_TMOUT_23_MS AVR32_USBB_USBCON_TIMVALUE_VB_BUS_PULSING_23_MS
  #define VBBUSPULSING_TMOUT_31_MS AVR32_USBB_USBCON_TIMVALUE_VB_BUS_PULSING_31_MS
  #define VBBUSPULSING_TMOUT_40_MS AVR32_USBB_USBCON_TIMVALUE_VB_BUS_PULSING_40_MS
#define PDTMOUTCNT_TIMER         AVR32_USBB_USBCON_TIMPAGE_PD_TMOUT_CNT
  #define PDTMOUTCNT_TMOUT_93_MS   AVR32_USBB_USBCON_TIMVALUE_PD_TMOUT_CNT_93_MS
  #define PDTMOUTCNT_TMOUT_105_MS  AVR32_USBB_USBCON_TIMVALUE_PD_TMOUT_CNT_105_MS
  #define PDTMOUTCNT_TMOUT_118_MS  AVR32_USBB_USBCON_TIMVALUE_PD_TMOUT_CNT_118_MS
  #define PDTMOUTCNT_TMOUT_131_MS  AVR32_USBB_USBCON_TIMVALUE_PD_TMOUT_CNT_131_MS
#define SRPDETTMOUT_TIMER        AVR32_USBB_USBCON_TIMPAGE_SRP_DET_TMOUT
  #define SRPDETTMOUT_TMOUT_10_US  AVR32_USBB_USBCON_TIMVALUE_SRP_DET_TMOUT_10_US
  #define SRPDETTMOUT_TMOUT_100_US AVR32_USBB_USBCON_TIMVALUE_SRP_DET_TMOUT_100_US
  #define SRPDETTMOUT_TMOUT_1_MS   AVR32_USBB_USBCON_TIMVALUE_SRP_DET_TMOUT_1_MS
  #define SRPDETTMOUT_TMOUT_11_MS  AVR32_USBB_USBCON_TIMVALUE_SRP_DET_TMOUT_11_MS

#define TYPE_CONTROL             AVR32_USBB_UECFGX_EPTYPE_CONTROL
#define TYPE_ISOCHRONOUS         AVR32_USBB_UECFGX_EPTYPE_ISOCHRONOUS
#define TYPE_BULK                AVR32_USBB_UECFGX_EPTYPE_BULK
#define TYPE_INTERRUPT           AVR32_USBB_UECFGX_EPTYPE_INTERRUPT

#define TRANSFER_TYPE_MASK          0x03
#define SYNCHRONIZATION_TYPE_MASK   0x0c
#define USAGE_TYPE_MASK             0x30

#define DIRECTION_OUT            AVR32_USBB_UECFGX_EPDIR_OUT
#define DIRECTION_IN             AVR32_USBB_UECFGX_EPDIR_IN

#define TOKEN_SETUP              AVR32_USBB_UPCFGX_PTOKEN_SETUP
#define TOKEN_IN                 AVR32_USBB_UPCFGX_PTOKEN_IN
#define TOKEN_OUT                AVR32_USBB_UPCFGX_PTOKEN_OUT

#define SINGLE_BANK              AVR32_USBB_UECFGX_EPBK_SINGLE
#define DOUBLE_BANK              AVR32_USBB_UECFGX_EPBK_DOUBLE
#define TRIPLE_BANK              AVR32_USBB_UECFGX_EPBK_TRIPLE

#define BANK_PID_DATA0           0
#define BANK_PID_DATA1           1
//! @}


//! Post-increment operations associated with 64-, 32-, 16- and 8-bit accesses to
//! the FIFO data registers of pipes/endpoints
//! @note 64- and 32-bit accesses to FIFO data registers do not require pointer
//! post-increment while 16- and 8-bit ones do.
//! @note Only for internal use.
//! @{
#define Pep_fifo_access_64_post_inc()
#define Pep_fifo_access_32_post_inc()
#define Pep_fifo_access_16_post_inc()   ++
#define Pep_fifo_access_8_post_inc()    ++
//! @}


//! @defgroup USBB_properties USBB IP properties
//! These macros give access to IP properties
//! @{
  //! Get IP name part 1 or 2
#define Usb_get_ip_name(part)           (AVR32_USBB_unamex(part))
  //! Get IP version
#define Usb_get_ip_version()            (Rd_bitfield(AVR32_USBB_uvers, AVR32_USBB_UVERS_VERSION_NUM_MASK))
  //! Get number of metal fixes
#define Usb_get_metal_fix_nbr()         (Rd_bitfield(AVR32_USBB_uvers, AVR32_USBB_UVERS_METAL_FIX_NUM_MASK))
  //! Get maximal number of pipes/endpoints (number of hardware-implemented pipes/endpoints)
#define Usb_get_pipe_endpoint_max_nbr() (((Rd_bitfield(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_EPT_NBR_MAX_MASK) - 1) & ((1 << AVR32_USBB_UFEATURES_EPT_NBR_MAX_SIZE) - 1)) + 1)
  //! Get number of hardware-implemented DMA channels
#define Usb_get_dma_channel_nbr()       (Rd_bitfield(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_DMA_CHANNEL_NBR_MASK))
  //! Get DMA buffer size
#define Usb_get_dma_buffer_size()       (Rd_bitfield(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_DMA_BUFFER_SIZE_MASK))
  //! Get DMA FIFO depth in words
#define Usb_get_dma_fifo_word_depth()   (((Rd_bitfield(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_DMA_FIFO_WORD_DEPTH_MASK) - 1) & ((1 << AVR32_USBB_UFEATURES_DMA_FIFO_WORD_DEPTH_SIZE) - 1)) + 1)
  //! Get DPRAM size (FIFO maximal size) in bytes
#define Usb_get_dpram_size()            (128 << Rd_bitfield(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_FIFO_MAX_SIZE_MASK))
  //! Test if DPRAM is natively byte write capable
#define Is_usb_dpram_byte_write_capable() (Tst_bits(AVR32_USBB_ufeatures, AVR32_USBB_UFEATURES_BYTE_WRITE_DPRAM_MASK))
  //! Get size of USBB PB address space
#define Usb_get_ip_paddress_size()      (AVR32_USBB_uaddrsize)
//! @}


//! @defgroup USBB_general USBB common management drivers
//! These macros manage the USBB controller
//! @{
  //! Configure time-out of specified OTG timer
#define Usb_configure_timeout(timer, timeout) (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UNLOCK_MASK),\
                                               Wr_bitfield(AVR32_USBB_usbcon, AVR32_USBB_USBCON_TIMPAGE_MASK, timer),\
                                               Wr_bitfield(AVR32_USBB_usbcon, AVR32_USBB_USBCON_TIMVALUE_MASK, timeout),\
                                               Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UNLOCK_MASK))
  //! Get configured time-out of specified OTG timer
#define Usb_get_timeout(timer)                (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UNLOCK_MASK),\
                                               Wr_bitfield(AVR32_USBB_usbcon, AVR32_USBB_USBCON_TIMPAGE_MASK, timer),\
                                               Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UNLOCK_MASK),\
                                               Rd_bitfield(AVR32_USBB_usbcon, AVR32_USBB_USBCON_TIMVALUE_MASK))

#if USB_DEVICE_FEATURE == true && USB_HOST_FEATURE == true
  //! Check that multiplexed pin used for USB_ID is defined
  #ifndef USB_ID
    #error YOU MUST define in your board header file the multiplexed pin used for USB_ID as AVR32_USBB_USB_ID_x_x
  #endif
  //! Pin and function for USB_ID according to configuration from USB_ID
#define USB_ID_PIN              ATPASTE2(USB_ID, _PIN)
#define USB_ID_FUNCTION         ATPASTE2(USB_ID, _FUNCTION)
  //! Input USB_ID from its pin
#define Usb_input_id_pin() \
{\
  (Tst_bits(USB_ID_FUNCTION, 0x01)) ?\
    (AVR32_GPIO.port[USB_ID_PIN >> 5].pmr0s = 1 << (USB_ID_PIN & 0x1F)) :\
    (AVR32_GPIO.port[USB_ID_PIN >> 5].pmr0c = 1 << (USB_ID_PIN & 0x1F)); \
  (Tst_bits(USB_ID_FUNCTION, 0x02)) ?\
    (AVR32_GPIO.port[USB_ID_PIN >> 5].pmr1s = 1 << (USB_ID_PIN & 0x1F)) :\
    (AVR32_GPIO.port[USB_ID_PIN >> 5].pmr1c = 1 << (USB_ID_PIN & 0x1F)); \
  AVR32_GPIO.port[USB_ID_PIN >> 5].gperc = 1 << (USB_ID_PIN & 0x1F);\
  AVR32_GPIO.port[USB_ID_PIN >> 5].puers = 1 << (USB_ID_PIN & 0x1F);\
}
  //! Test if USB_ID is input from its pin
#define Is_usb_id_pin_input() \
          (!Tst_bits(AVR32_GPIO.port[USB_ID_PIN >> 5].gper, 1 << (USB_ID_PIN & 0x1F)) &&\
            Tst_bits(AVR32_GPIO.port[USB_ID_PIN >> 5].pmr0, 1 << (USB_ID_PIN & 0x1F)) == Tst_bits(USB_ID_FUNCTION, 0x01) &&\
            Tst_bits(AVR32_GPIO.port[USB_ID_PIN >> 5].pmr1, 1 << (USB_ID_PIN & 0x1F)) == Tst_bits(USB_ID_FUNCTION, 0x02))
#endif  // USB_DEVICE_FEATURE == true && USB_HOST_FEATURE == true
  //! Enable external USB_ID pin (listened to by USB)
#define Usb_enable_id_pin()             (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIDE_MASK))
  //! Disable external USB_ID pin (ignored by USB)
#define Usb_disable_id_pin()            (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIDE_MASK))
  //! Test if external USB_ID pin enabled (listened to by USB)
#define Is_usb_id_pin_enabled()         (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIDE_MASK))
  //! Disable external USB_ID pin and force device mode
#define Usb_force_device_mode()         (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIMOD_MASK), Usb_disable_id_pin())
  //! Test if device mode is forced
#define Is_usb_device_mode_forced()     (!Is_usb_id_pin_enabled() && Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIMOD_MASK))
  //! Disable external USB_ID pin and force host mode
#define Usb_force_host_mode()           (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIMOD_MASK), Usb_disable_id_pin())
  //! Test if host mode is forced
#define Is_usb_host_mode_forced()       (!Is_usb_id_pin_enabled() && !Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_UIMOD_MASK))

#if USB_HOST_FEATURE == true
  //! Check that multiplexed pin used for USB_VBOF is defined
  #ifndef USB_VBOF
    #error YOU MUST define in your board header file the multiplexed pin used for USB_VBOF as AVR32_USBB_USB_VBOF_x_x
  #endif
  //! Pin and function for USB_VBOF according to configuration from USB_VBOF
#define USB_VBOF_PIN            ATPASTE2(USB_VBOF, _PIN)
#define USB_VBOF_FUNCTION       ATPASTE2(USB_VBOF, _FUNCTION)
  //! Output USB_VBOF onto its pin
#define Usb_output_vbof_pin() \
{\
  (Tst_bits(USB_VBOF_FUNCTION, 0x01)) ?\
    (AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0s = 1 << (USB_VBOF_PIN & 0x1F)) :\
    (AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0c = 1 << (USB_VBOF_PIN & 0x1F)); \
  (Tst_bits(USB_VBOF_FUNCTION, 0x02)) ?\
    (AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1s = 1 << (USB_VBOF_PIN & 0x1F)) :\
    (AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1c = 1 << (USB_VBOF_PIN & 0x1F)); \
  AVR32_GPIO.port[USB_VBOF_PIN >> 5].gperc  = 1 << (USB_VBOF_PIN & 0x1F);\
  AVR32_GPIO.port[USB_VBOF_PIN >> 5].odmerc = 1 << (USB_VBOF_PIN & 0x1F);\
  AVR32_GPIO.port[USB_VBOF_PIN >> 5].puerc  = 1 << (USB_VBOF_PIN & 0x1F);\
}
  //! Test if USB_VBOF is output onto its pin
#define Is_usb_vbof_pin_output() \
          (!Tst_bits(AVR32_GPIO.port[USB_VBOF_PIN >> 5].gper, 1 << (USB_VBOF_PIN & 0x1F)) &&\
            Tst_bits(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr0, 1 << (USB_VBOF_PIN & 0x1F)) == Tst_bits(USB_VBOF_FUNCTION, 0x01) &&\
            Tst_bits(AVR32_GPIO.port[USB_VBOF_PIN >> 5].pmr1, 1 << (USB_VBOF_PIN & 0x1F)) == Tst_bits(USB_VBOF_FUNCTION, 0x02))
#endif  // USB_HOST_FEATURE == true
  //! Set USB_VBOF output pin polarity
#define Usb_set_vbof_active_high()      (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSPO_MASK))
#define Usb_set_vbof_active_low()       (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSPO_MASK))
  //! Get USB_VBOF output pin polarity
#define Is_usb_vbof_active_high()       (!Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSPO_MASK))
#define Is_usb_vbof_active_low()        (!Is_usb_vbof_active_high())

  //! Use device full speed mode (default)
#define Usb_use_full_speed_mode()       (Clr_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_LS_MASK))
  //! Test if device full speed mode is used
#define Is_usb_full_speed_mode_used()   (!Is_usb_low_speed_mode_forced())
#ifdef AVR32_USBB_UDCON_SPDCONF
  //! Force device full speed mode (i.e. disable high speed)
#define Usb_force_full_speed_mode()	(Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 3))
  //! Enable dual speed mode (full speed and high speed; default)
#define Usb_use_dual_speed_mode()	(Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_SPDCONF_MASK, 0))
#else
#define Usb_force_full_speed_mode()	do { } while (0)
#define Usb_use_dual_speed_mode()	do { } while (0)
#endif
  //! Force device low-speed mode
#define Usb_force_low_speed_mode()      (Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_LS_MASK))
  //! Test if device low-speed mode is forced
#define Is_usb_low_speed_mode_forced()  (Tst_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_LS_MASK))
  //! Test if controller is in full speed mode
#define Is_usb_full_speed_mode()        (Rd_bitfield(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_SPEED_MASK) == AVR32_USBB_USBSTA_SPEED_FULL)
  //! Test if controller is in low-speed mode
#define Is_usb_low_speed_mode()         (Rd_bitfield(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_SPEED_MASK) == AVR32_USBB_USBSTA_SPEED_LOW)

  //! Enable USB macro
#define Usb_enable()                  (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_USBE_MASK))
  //! Disable USB macro
#define Usb_disable()                 (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_USBE_MASK))
#define Is_usb_enabled()              (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_USBE_MASK))

  //! Enable OTG pad
#define Usb_enable_otg_pad()          (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_OTGPADE_MASK))
  //! Disable OTG pad
#define Usb_disable_otg_pad()         (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_OTGPADE_MASK))
#define Is_usb_otg_pad_enabled()      (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_OTGPADE_MASK))

  //! Stop (freeze) internal USB clock
#define Usb_freeze_clock()            (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_FRZCLK_MASK))
#define Usb_unfreeze_clock()          (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_FRZCLK_MASK))
#define Is_usb_clock_frozen()         (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_FRZCLK_MASK))

  //! Get the dual-role device state of the internal USB finite state machine of the USBB controller
#define Usb_get_fsm_drd_state()       (Rd_bitfield(AVR32_USBB_usbfsm, AVR32_USBB_USBFSM_DRDSTATE_MASK))

#define Usb_enable_id_interrupt()     (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_IDTE_MASK))
#define Usb_disable_id_interrupt()    (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_IDTE_MASK))
#define Is_usb_id_interrupt_enabled() (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_IDTE_MASK))
#define Is_usb_id_device()            (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_ID_MASK))
#define Usb_ack_id_transition()       (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_IDTIC_MASK)
#define Usb_raise_id_transition()     (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_IDTIS_MASK)
#define Is_usb_id_transition()        (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_IDTI_MASK))

#define Usb_enable_vbus_interrupt()   (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSTE_MASK))
#define Usb_disable_vbus_interrupt()  (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSTE_MASK))
#define Is_usb_vbus_interrupt_enabled() (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSTE_MASK))
#define Is_usb_vbus_high()            (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_VBUS_MASK))
#define Is_usb_vbus_low()             (!Is_usb_vbus_high())
#define Usb_ack_vbus_transition()     (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_VBUSTIC_MASK)
#define Usb_raise_vbus_transition()   (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_VBUSTIS_MASK)
#define Is_usb_vbus_transition()      (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_VBUSTI_MASK))

  //! enables hardware control over the USB_VBOF output pin
#define Usb_enable_vbus_hw_control()  (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSHWC_MASK))
  //! disables hardware control over the USB_VBOF output pin
#define Usb_disable_vbus_hw_control() (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSHWC_MASK))
#define Is_usb_vbus_hw_control_enabled() (!Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBUSHWC_MASK))
  //! requests VBus activation
#define Usb_enable_vbus()             (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_VBUSRQS_MASK)
  //! requests VBus deactivation
#define Usb_disable_vbus()            (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_VBUSRQC_MASK)
  //! tests if VBus activation has been requested
#define Is_usb_vbus_enabled()         (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_VBUSRQ_MASK))

  //! initiates a Host negotiation Protocol
#define Usb_device_initiate_hnp()     (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPREQ_MASK))
  //! accepts a Host negotiation Protocol
#define Usb_host_accept_hnp()         (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPREQ_MASK))
  //! rejects a Host negotiation Protocol
#define Usb_host_reject_hnp()         (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPREQ_MASK))
  //! initiates a Session Request Protocol
#define Usb_device_initiate_srp()     (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPREQ_MASK))
  //! selects VBus as SRP method
#define Usb_select_vbus_srp_method()  (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPSEL_MASK))
#define Is_usb_vbus_srp_method_selected() (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPSEL_MASK))
  //! selects data line as SRP method
#define Usb_select_data_srp_method()  (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPSEL_MASK))
#define Is_usb_data_srp_method_selected() (!Is_usb_vbus_srp_method_selected())
  //! tests if a HNP occurs
#define Is_usb_hnp()                  (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPREQ_MASK))
  //! tests if a SRP from device occurs
#define Is_usb_device_srp()           (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPREQ_MASK))

  //! enables suspend time out interrupt
#define Usb_enable_suspend_time_out_interrupt()   (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_STOE_MASK))
  //! disables suspend time out interrupt
#define Usb_disable_suspend_time_out_interrupt()  (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_STOE_MASK))
#define Is_usb_suspend_time_out_interrupt_enabled() (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_STOE_MASK))
  //! acks suspend time out interrupt
#define Usb_ack_suspend_time_out_interrupt()      (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_STOIC_MASK)
  //! raises suspend time out interrupt
#define Usb_raise_suspend_time_out_interrupt()    (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_STOIS_MASK)
  //! tests if a suspend time out occurs
#define Is_usb_suspend_time_out_interrupt()       (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_STOI_MASK))

  //! enables HNP error interrupt
#define Usb_enable_hnp_error_interrupt()          (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPERRE_MASK))
  //! disables HNP error interrupt
#define Usb_disable_hnp_error_interrupt()         (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPERRE_MASK))
#define Is_usb_hnp_error_interrupt_enabled()      (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_HNPERRE_MASK))
  //! acks HNP error interrupt
#define Usb_ack_hnp_error_interrupt()             (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_HNPERRIC_MASK)
  //! raises HNP error interrupt
#define Usb_raise_hnp_error_interrupt()           (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_HNPERRIS_MASK)
  //! tests if a HNP error occurs
#define Is_usb_hnp_error_interrupt()              (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_HNPERRI_MASK))

  //! enables role exchange interrupt
#define Usb_enable_role_exchange_interrupt()      (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_ROLEEXE_MASK))
  //! disables role exchange interrupt
#define Usb_disable_role_exchange_interrupt()     (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_ROLEEXE_MASK))
#define Is_usb_role_exchange_interrupt_enabled()  (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_ROLEEXE_MASK))
  //! acks role exchange interrupt
#define Usb_ack_role_exchange_interrupt()         (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_ROLEEXIC_MASK)
  //! raises role exchange interrupt
#define Usb_raise_role_exchange_interrupt()       (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_ROLEEXIS_MASK)
  //! tests if a role exchange occurs
#define Is_usb_role_exchange_interrupt()          (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_ROLEEXI_MASK))

  //! enables B-device connection error interrupt
#define Usb_enable_bconnection_error_interrupt()  (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_BCERRE_MASK))
  //! disables B-device connection error interrupt
#define Usb_disable_bconnection_error_interrupt() (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_BCERRE_MASK))
#define Is_usb_bconnection_error_interrupt_enabled() (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_BCERRE_MASK))
  //! acks B-device connection error interrupt
#define Usb_ack_bconnection_error_interrupt()     (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_BCERRIC_MASK)
  //! raises B-device connection error interrupt
#define Usb_raise_bconnection_error_interrupt()   (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_BCERRIS_MASK)
  //! tests if a B-device connection error occurs
#define Is_usb_bconnection_error_interrupt()      (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_BCERRI_MASK))

  //! enables VBus error interrupt
#define Usb_enable_vbus_error_interrupt()         (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBERRE_MASK))
  //! disables VBus error interrupt
#define Usb_disable_vbus_error_interrupt()        (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBERRE_MASK))
#define Is_usb_vbus_error_interrupt_enabled()     (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_VBERRE_MASK))
  //! acks VBus error interrupt
#define Usb_ack_vbus_error_interrupt()            (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_VBERRIC_MASK)
  //! raises VBus error interrupt
#define Usb_raise_vbus_error_interrupt()          (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_VBERRIS_MASK)
  //! tests if a VBus error occurs
#define Is_usb_vbus_error_interrupt()             (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_VBERRI_MASK))

  //! enables SRP interrupt
#define Usb_enable_srp_interrupt()                (Set_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPE_MASK))
  //! disables SRP interrupt
#define Usb_disable_srp_interrupt()               (Clr_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPE_MASK))
#define Is_usb_srp_interrupt_enabled()            (Tst_bits(AVR32_USBB_usbcon, AVR32_USBB_USBCON_SRPE_MASK))
  //! acks SRP interrupt
#define Usb_ack_srp_interrupt()                   (AVR32_USBB_usbstaclr = AVR32_USBB_USBSTACLR_SRPIC_MASK)
  //! raises SRP interrupt
#define Usb_raise_srp_interrupt()                 (AVR32_USBB_usbstaset = AVR32_USBB_USBSTASET_SRPIS_MASK)
  //! tests if a SRP occurs
#define Is_usb_srp_interrupt()                    (Tst_bits(AVR32_USBB_usbsta, AVR32_USBB_USBSTA_SRPI_MASK))
//! @}


//! @defgroup USBB_device_driver USBB device controller drivers
//! These macros manage the USBB Device controller.
//! @{
  //! initiates a remote wake-up
#define Usb_initiate_remote_wake_up()             (Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_RMWKUP_MASK))
  //! detaches from USB bus
#define Usb_detach()                              (Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_DETACH_MASK))
  //! attaches to USB bus
#define Usb_attach()                              (Clr_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_DETACH_MASK))
  //! test if remote wake-up still running
#define Is_usb_pending_remote_wake_up()           (Tst_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_RMWKUP_MASK))
  //! test if the device is detached
#define Is_usb_detached()                         (Tst_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_DETACH_MASK))

  //! enables remote wake-up interrupt
#define Usb_enable_remote_wake_up_interrupt()     (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_UPRSMES_MASK)
  //! disables remote wake-up interrupt
#define Usb_disable_remote_wake_up_interrupt()    (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_UPRSMEC_MASK)

#define Is_usb_remote_wake_up_interrupt_enabled() (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_UPRSME_MASK))
  //! acks remote wake-up
#define Usb_ack_remote_wake_up_start()            (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_UPRSMC_MASK)
  //! raises remote wake-up
#define Usb_raise_remote_wake_up_start()          (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_UPRSMS_MASK)
  //! tests if remote wake-up still running
#define Is_usb_remote_wake_up_start()             (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_UPRSM_MASK))

  //! enables resume interrupt
#define Usb_enable_resume_interrupt()             (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_EORSMES_MASK)
  //! disables resume interrupt
#define Usb_disable_resume_interrupt()            (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_EORSMEC_MASK)
#define Is_usb_resume_interrupt_enabled()         (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_EORSME_MASK))
  //! acks resume
#define Usb_ack_resume()                          (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_EORSMC_MASK)
  //! raises resume
#define Usb_raise_resume()                        (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_EORSMS_MASK)
  //! tests if resume occurs
#define Is_usb_resume()                           (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_EORSM_MASK))

  //! enables wake-up interrupt
#define Usb_enable_wake_up_interrupt()            (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_WAKEUPES_MASK)
  //! disables wake-up interrupt
#define Usb_disable_wake_up_interrupt()           (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_WAKEUPEC_MASK)
#define Is_usb_wake_up_interrupt_enabled()        (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_WAKEUPE_MASK))
  //! acks wake-up
#define Usb_ack_wake_up()                         (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_WAKEUPC_MASK)
  //! raises wake-up
#define Usb_raise_wake_up()                       (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_WAKEUPS_MASK)
  //! tests if wake-up occurs
#define Is_usb_wake_up()                          (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_WAKEUP_MASK))

  //! enables USB reset interrupt
#define Usb_enable_reset_interrupt()              (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_EORSTES_MASK)
  //! disables USB reset interrupt
#define Usb_disable_reset_interrupt()             (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_EORSTEC_MASK)
#define Is_usb_reset_interrupt_enabled()          (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_EORSTE_MASK))
  //! acks USB reset
#define Usb_ack_reset()                           (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_EORSTC_MASK)
  //! raises USB reset
#define Usb_raise_reset()                         (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_EORSTS_MASK)
  //! tests if USB reset occurs
#define Is_usb_reset()                            (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_EORST_MASK))

  //! enables Start-of-Frame Interrupt
#define Usb_enable_sof_interrupt()                (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_SOFES_MASK)
  //! disables Start-of-Frame Interrupt
#define Usb_disable_sof_interrupt()               (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_SOFEC_MASK)
#define Is_usb_sof_interrupt_enabled()            (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_SOFE_MASK))
  //! acks Start-of-Frame
#define Usb_ack_sof()                             (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_SOFC_MASK)
  //! raises Start-of-Frame
#define Usb_raise_sof()                           (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_SOFS_MASK)
  //! tests if Start-of-Frame occurs
#define Is_usb_sof()                              (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_SOF_MASK))

  //! enables suspend state interrupt
#define Usb_enable_suspend_interrupt()            (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_SUSPES_MASK)
  //! disables suspend state interrupt
#define Usb_disable_suspend_interrupt()           (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_SUSPEC_MASK)
#define Is_usb_suspend_interrupt_enabled()        (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_SUSPE_MASK))
  //! acks Suspend
#define Usb_ack_suspend()                         (AVR32_USBB_udintclr = AVR32_USBB_UDINTCLR_SUSPC_MASK)
  //! raises Suspend
#define Usb_raise_suspend()                       (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_SUSPS_MASK)
  //! tests if Suspend state detected
#define Is_usb_suspend()                          (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_SUSP_MASK))

  //! enables USB device address
#define Usb_enable_address()                      (Set_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_ADDEN_MASK))
  //! disables USB device address
#define Usb_disable_address()                     (Clr_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_ADDEN_MASK))
#define Is_usb_address_enabled()                  (Tst_bits(AVR32_USBB_udcon, AVR32_USBB_UDCON_ADDEN_MASK))
  //! configures the USB device address
#define Usb_configure_address(addr)               (Wr_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_UADD_MASK, addr))
  //! gets the currently configured USB device address
#define Usb_get_configured_address()              (Rd_bitfield(AVR32_USBB_udcon, AVR32_USBB_UDCON_UADD_MASK))

  //! returns the current frame number
#define Usb_frame_number()                        (Rd_bitfield(AVR32_USBB_udfnum, AVR32_USBB_UDFNUM_FNUM_MASK))
  //! tests if a crc error occurs in frame number
#define Is_usb_frame_number_crc_error()           (Tst_bits(AVR32_USBB_udfnum, AVR32_USBB_UDFNUM_FNCERR_MASK))
//! @}


//! @defgroup USBB_general_endpoint USBB endpoint drivers
//! These macros manage the common features of the endpoints.
//! @{
  //! resets the selected endpoint
#define Usb_reset_endpoint(ep)                    (Set_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPRST0_MASK << (ep)),\
                                                   Clr_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPRST0_MASK << (ep)))
  //! tests if the selected endpoint is being reset
#define Is_usb_resetting_endpoint(ep)             (Tst_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPRST0_MASK << (ep)))

  //! enables the selected endpoint
#define Usb_enable_endpoint(ep)                   (Set_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPEN0_MASK << (ep)))
  //! enables the STALL handshake
#define Usb_enable_stall_handshake(ep)            (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_STALLRQS_MASK)
  //! Sends a STALL handshake for the next host request. A STALL handshake will
  //! be sent for each following request until a SETUP or a Clear Halt Feature
  //! occurs for this endpoint.
#define Usb_halt_endpoint(ep)                     (Usb_enable_stall_handshake(ep))
  //! resets the data toggle sequence
#define Usb_reset_data_toggle(ep)                 (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_RSTDTS_MASK)
  //! disables the selected endpoint
#define Usb_disable_endpoint(ep)                  (Clr_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPEN0_MASK << (ep)))
  //! disables the STALL handshake
#define Usb_disable_stall_handshake(ep)           (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_STALLRQC_MASK)
  //! tests if the selected endpoint is enabled
#define Is_usb_endpoint_enabled(ep)               (Tst_bits(AVR32_USBB_uerst, AVR32_USBB_UERST_EPEN0_MASK << (ep)))
  //! tests if STALL handshake request is running
#define Is_usb_endpoint_stall_requested(ep)       (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_STALLRQ_MASK))
  //! tests if the data toggle sequence is being reset
#define Is_usb_data_toggle_reset(ep)              (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_RSTDT_MASK))

  //! tests if an interrupt is triggered by the selected endpoint
#define Is_usb_endpoint_interrupt(ep)             (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_EP0INT_MASK << (ep)))
  //! enables the selected endpoint interrupt
#define Usb_enable_endpoint_interrupt(ep)         (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_EP0INTES_MASK << (ep))
  //! disables the selected endpoint interrupt
#define Usb_disable_endpoint_interrupt(ep)        (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_EP0INTEC_MASK << (ep))
  //! tests if the selected endpoint interrupt is enabled
#define Is_usb_endpoint_interrupt_enabled(ep)     (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_EP0INTE_MASK << (ep)))
  //! returns the lowest endpoint number generating an endpoint interrupt or MAX_PEP_NB if none
#define Usb_get_interrupt_endpoint_number()       (ctz(((AVR32_USBB_udint >> AVR32_USBB_UDINT_EP0INT_OFFSET) &\
                                                        (AVR32_USBB_udinte >> AVR32_USBB_UDINTE_EP0INTE_OFFSET)) |\
                                                       (1 << MAX_PEP_NB)))

  //! configures the selected endpoint type
#define Usb_configure_endpoint_type(ep, type)     (Wr_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPTYPE_MASK, type))
  //! gets the configured selected endpoint type
#define Usb_get_endpoint_type(ep)                 (Rd_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPTYPE_MASK))
  //! enables the bank autoswitch for the selected endpoint
#define Usb_enable_endpoint_bank_autoswitch(ep)   (Set_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_AUTOSW_MASK))
  //! disables the bank autoswitch for the selected endpoint
#define Usb_disable_endpoint_bank_autoswitch(ep)   (Clr_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_AUTOSW_MASK))
#define Is_usb_endpoint_bank_autoswitch_enabled(ep) (Tst_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_AUTOSW_MASK))
  //! configures the selected endpoint direction
#define Usb_configure_endpoint_direction(ep, dir) (Wr_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPDIR_MASK, dir))
  //! gets the configured selected endpoint direction
#define Usb_get_endpoint_direction(ep)            (Rd_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPDIR_MASK))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of USBB controller
  //! for endpoint size bit-field.
#define Usb_format_endpoint_size(size)            (32 - clz(((U32)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! configures the selected endpoint size
#define Usb_configure_endpoint_size(ep, size)     (Wr_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPSIZE_MASK, Usb_format_endpoint_size(size)))
  //! gets the configured selected endpoint size
#define Usb_get_endpoint_size(ep)                 (8 << Rd_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPSIZE_MASK))
  //! configures the selected endpoint number of banks
#define Usb_configure_endpoint_bank(ep, bank)     (Wr_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPBK_MASK, bank))
  //! gets the configured selected endpoint number of banks
#define Usb_get_endpoint_bank(ep)                 (Rd_bitfield(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPBK_MASK))
  //! allocates the configuration x in DPRAM memory
#define Usb_allocate_memory(ep)                   (Set_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_ALLOC_MASK))
  //! un-allocates the configuration x in DPRAM memory
#define Usb_unallocate_memory(ep)                 (Clr_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_ALLOC_MASK))
#define Is_usb_memory_allocated(ep)               (Tst_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_ALLOC_MASK))

  //! configures selected endpoint in one step
#define Usb_configure_endpoint(ep, type, dir, size, bank) \
(\
  Usb_enable_endpoint(ep),\
  Wr_bits(AVR32_USBB_uecfgx(ep), AVR32_USBB_UECFGX_EPTYPE_MASK |\
                                 AVR32_USBB_UECFGX_EPDIR_MASK  |\
                                 AVR32_USBB_UECFGX_EPSIZE_MASK |\
                                 AVR32_USBB_UECFGX_EPBK_MASK,   \
          (((U32)(type) << AVR32_USBB_UECFGX_EPTYPE_OFFSET) & AVR32_USBB_UECFGX_EPTYPE_MASK) |\
          (((U32)(dir ) << AVR32_USBB_UECFGX_EPDIR_OFFSET ) & AVR32_USBB_UECFGX_EPDIR_MASK ) |\
          ( (U32)Usb_format_endpoint_size(size) << AVR32_USBB_UECFGX_EPSIZE_OFFSET         ) |\
          (((U32)(bank) << AVR32_USBB_UECFGX_EPBK_OFFSET  ) & AVR32_USBB_UECFGX_EPBK_MASK  )),\
  Usb_allocate_memory(ep),\
\
  Is_usb_endpoint_configured(ep)\
)

  //! acks endpoint overflow interrupt
#define Usb_ack_overflow_interrupt(ep)            (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_OVERFIC_MASK)
  //! raises endpoint overflow interrupt
#define Usb_raise_overflow_interrupt(ep)          (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_OVERFIS_MASK)
  //! acks endpoint underflow interrupt
#define Usb_ack_underflow_interrupt(ep)           (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_RXSTPIC_MASK)
  //! raises endpoint underflow interrupt
#define Usb_raise_underflow_interrupt(ep)         (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_RXSTPIS_MASK)
  //! returns data toggle
#define Usb_data_toggle(ep)                       (Rd_bitfield(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_DTSEQ_MASK))
  //! returns the number of busy banks
#define Usb_nb_busy_bank(ep)                      (Rd_bitfield(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_NBUSYBK_MASK))
  //! tests if current endpoint is configured
#define Is_usb_endpoint_configured(ep)            (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_CFGOK_MASK))
  //! tests if an overflow occurs
#define Is_usb_overflow(ep)                       (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_OVERFI_MASK))
  //! tests if an underflow occurs
#define Is_usb_underflow(ep)                      (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_RXSTPI_MASK))

  //! returns the byte count
#define Usb_byte_count(ep)                        (Rd_bitfield(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_BYCT_MASK))
  //! returns the control direction
#define Usb_control_direction()                   (Rd_bitfield(AVR32_USBB_uestax(EP_CONTROL), AVR32_USBB_UESTAX_CTRLDIR_MASK))
  //! returns the number of the current bank
#define Usb_current_bank(ep)                      (Rd_bitfield(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_CURRBK_MASK))

  //! kills last bank
#define Usb_kill_last_in_bank(ep)                 (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_KILLBKS_MASK)
  //! acks SHORT PACKET received
#define Usb_ack_short_packet(ep)                  (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_SHORTPACKETIC_MASK)
  //! raises SHORT PACKET received
#define Usb_raise_short_packet(ep)                (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_SHORTPACKETIS_MASK)
  //! acks STALL sent
#define Usb_ack_stall(ep)                         (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_STALLEDIC_MASK)
  //! raises STALL sent
#define Usb_raise_stall(ep)                       (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_STALLEDIS_MASK)
  //! acks CRC ERROR ISO OUT detected
#define Usb_ack_crc_error(ep)                     (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_STALLEDIC_MASK)
  //! raises CRC ERROR ISO OUT detected
#define Usb_raise_crc_error(ep)                   (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_STALLEDIS_MASK)
  //! acks NAK IN received
#define Usb_ack_nak_in(ep)                        (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_NAKINIC_MASK)
  //! raises NAK IN received
#define Usb_raise_nak_in(ep)                      (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_NAKINIS_MASK)
  //! acks NAK OUT received
#define Usb_ack_nak_out(ep)                       (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_NAKOUTIC_MASK)
  //! raises NAK OUT received
#define Usb_raise_nak_out(ep)                     (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_NAKOUTIS_MASK)

  //! tests if last bank killed
#define Is_usb_last_in_bank_killed(ep)            (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_KILLBK_MASK))
  //! tests if endpoint read allowed
#define Is_usb_read_enabled(ep)                   (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_RWALL_MASK))
  //! tests if endpoint write allowed
#define Is_usb_write_enabled(ep)                  (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_RWALL_MASK))
  //! tests if SHORT PACKET received
#define Is_usb_short_packet(ep)                   (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_SHORTPACKETI_MASK))
  //! tests if STALL sent
#define Is_usb_stall(ep)                          (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_STALLEDI_MASK))
  //! tests if CRC ERROR ISO OUT detected
#define Is_usb_crc_error(ep)                      (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_STALLEDI_MASK))
  //! tests if NAK IN received
#define Is_usb_nak_in(ep)                         (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_NAKINI_MASK))
  //! tests if NAK OUT received
#define Is_usb_nak_out(ep)                        (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_NAKOUTI_MASK))

  //! clears FIFOCON bit
#define Usb_ack_fifocon(ep)                       (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_FIFOCONC_MASK)

  //! acks SETUP received
#define Usb_ack_setup_received_free()             (AVR32_USBB_uestaxclr(EP_CONTROL) = AVR32_USBB_UESTAXCLR_RXSTPIC_MASK)
  //! raises SETUP received
#define Usb_raise_setup_received()                (AVR32_USBB_uestaxset(EP_CONTROL) = AVR32_USBB_UESTAXSET_RXSTPIS_MASK)
  //! acks OUT received
#define Usb_ack_out_received(ep)                  (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_RXOUTIC_MASK)
  //! raises OUT received
#define Usb_raise_out_received(ep)                (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_RXOUTIS_MASK)
  //! frees current bank for OUT endpoint
#define Usb_free_out(ep)                          (Usb_ack_fifocon(ep))
  //! acks OUT received and frees current bank
#define Usb_ack_out_received_free(ep)             (Usb_ack_out_received(ep), Usb_free_out(ep))
  //! acks OUT received on control endpoint and frees current bank
#define Usb_ack_control_out_received_free()       (AVR32_USBB_uestaxclr(EP_CONTROL) = AVR32_USBB_UESTAXCLR_RXOUTIC_MASK)
  //! raises OUT received on control endpoint
#define Usb_raise_control_out_received()          (AVR32_USBB_uestaxset(EP_CONTROL) = AVR32_USBB_UESTAXSET_RXOUTIS_MASK)

  //! acks IN ready
#define Usb_ack_in_ready(ep)                      (AVR32_USBB_uestaxclr(ep) = AVR32_USBB_UESTAXCLR_TXINIC_MASK)
  //! raises IN ready
#define Usb_raise_in_ready(ep)                    (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_TXINIS_MASK)
  //! sends current bank for IN endpoint
#define Usb_send_in(ep)                           (Usb_ack_fifocon(ep))
  //! acks IN ready and sends current bank
#define Usb_ack_in_ready_send(ep)                 (Usb_ack_in_ready(ep), Usb_send_in(ep))
  //! acks IN ready on control endpoint and sends current bank
#define Usb_ack_control_in_ready_send()           (AVR32_USBB_uestaxclr(EP_CONTROL) = AVR32_USBB_UESTAXCLR_TXINIC_MASK)
  //! raises IN ready on control endpoint
#define Usb_raise_control_in_ready()              (AVR32_USBB_uestaxset(EP_CONTROL) = AVR32_USBB_UESTAXSET_TXINIS_MASK)

  //! tests if FIFOCON bit set
#define Is_usb_fifocon(ep)                        (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_FIFOCON_MASK))

  //! tests if SETUP received
#define Is_usb_setup_received()                   (Tst_bits(AVR32_USBB_uestax(EP_CONTROL), AVR32_USBB_UESTAX_RXSTPI_MASK))
  //! tests if OUT received
#define Is_usb_out_received(ep)                   (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_RXOUTI_MASK))
  //! tests if current bank filled for OUT endpoint
#define Is_usb_out_filled(ep)                     (Is_usb_fifocon(ep))
  //! tests if OUT received on control endpoint
#define Is_usb_control_out_received()             (Tst_bits(AVR32_USBB_uestax(EP_CONTROL), AVR32_USBB_UESTAX_RXOUTI_MASK))

  //! tests if IN ready
#define Is_usb_in_ready(ep)                       (Tst_bits(AVR32_USBB_uestax(ep), AVR32_USBB_UESTAX_TXINI_MASK))
  //! tests if current bank sent for IN endpoint
#define Is_usb_in_sent(ep)                        (Is_usb_fifocon(ep))
  //! tests if IN ready on control endpoint
#define Is_usb_control_in_ready()                 (Tst_bits(AVR32_USBB_uestax(EP_CONTROL), AVR32_USBB_UESTAX_TXINI_MASK))

  //! forces all banks full (OUT) or free (IN) interrupt
#define Usb_force_bank_interrupt(ep)              (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_NBUSYBKS_MASK)
  //! unforces all banks full (OUT) or free (IN) interrupt
#define Usb_unforce_bank_interrupt(ep)            (AVR32_USBB_uestaxset(ep) = AVR32_USBB_UESTAXSET_NBUSYBKS_MASK)
  //! enables all banks full (OUT) or free (IN) interrupt
#define Usb_enable_bank_interrupt(ep)             (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_NBUSYBKES_MASK)
  //! enables SHORT PACKET received interrupt
#define Usb_enable_short_packet_interrupt(ep)     (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_SHORTPACKETES_MASK)
  //! enables STALL sent interrupt
#define Usb_enable_stall_interrupt(ep)            (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_STALLEDES_MASK)
  //! enables CRC ERROR ISO OUT detected interrupt
#define Usb_enable_crc_error_interrupt(ep)        (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_STALLEDES_MASK)
  //! enables overflow interrupt
#define Usb_enable_overflow_interrupt(ep)         (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_OVERFES_MASK)
  //! enables NAK IN interrupt
#define Usb_enable_nak_in_interrupt(ep)           (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_NAKINES_MASK)
  //! enables NAK OUT interrupt
#define Usb_enable_nak_out_interrupt(ep)          (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_NAKOUTES_MASK)
  //! enables SETUP received interrupt
#define Usb_enable_setup_received_interrupt()     (AVR32_USBB_ueconxset(EP_CONTROL) = AVR32_USBB_UECONXSET_RXSTPES_MASK)
  //! enables underflow interrupt
#define Usb_enable_underflow_interrupt(ep)        (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_RXSTPES_MASK)
  //! enables OUT received interrupt
#define Usb_enable_out_received_interrupt(ep)     (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_RXOUTES_MASK)
  //! enables OUT received on control endpoint interrupt
#define Usb_enable_control_out_received_interrupt() (AVR32_USBB_ueconxset(EP_CONTROL) = AVR32_USBB_UECONXSET_RXOUTES_MASK)
  //! enables IN ready interrupt
#define Usb_enable_in_ready_interrupt(ep)         (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_TXINES_MASK)
  //! enables IN ready on control endpoint interrupt
#define Usb_enable_control_in_ready_interrupt()   (AVR32_USBB_ueconxset(EP_CONTROL) = AVR32_USBB_UECONXSET_TXINES_MASK)
  //! disables all banks full (OUT) or free (IN) interrupt
#define Usb_disable_bank_interrupt(ep)            (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_NBUSYBKEC_MASK)
  //! disables SHORT PACKET received interrupt
#define Usb_disable_short_packet_interrupt(ep)    (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_SHORTPACKETEC_MASK)
  //! disables STALL sent interrupt
#define Usb_disable_stall_interrupt(ep)           (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_STALLEDEC_MASK)
  //! disables CRC ERROR ISO OUT detected interrupt
#define Usb_disable_crc_error_interrupt(ep)       (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_STALLEDEC_MASK)
  //! disables overflow interrupt
#define Usb_disable_overflow_interrupt(ep)        (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_OVERFEC_MASK)
  //! disables NAK IN interrupt
#define Usb_disable_nak_in_interrupt(ep)          (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_NAKINEC_MASK)
  //! disables NAK OUT interrupt
#define Usb_disable_nak_out_interrupt(ep)         (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_NAKOUTEC_MASK)
  //! disables SETUP received interrupt
#define Usb_disable_setup_received_interrupt()    (AVR32_USBB_ueconxclr(EP_CONTROL) = AVR32_USBB_UECONXCLR_RXSTPEC_MASK)
  //! disables underflow interrupt
#define Usb_disable_underflow_interrupt(ep)       (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_RXSTPEC_MASK)
  //! disables OUT received interrupt
#define Usb_disable_out_received_interrupt(ep)    (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_RXOUTEC_MASK)
  //! disables OUT received on control endpoint interrupt
#define Usb_disable_control_out_received_interrupt() (AVR32_USBB_ueconxclr(EP_CONTROL) = AVR32_USBB_UECONXCLR_RXOUTEC_MASK)
  //! disables IN ready interrupt
#define Usb_disable_in_ready_interrupt(ep)        (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_TXINEC_MASK)
  //! disables IN ready on control endpoint interrupt
#define Usb_disable_control_in_ready_interrupt()  (AVR32_USBB_ueconxclr(EP_CONTROL) = AVR32_USBB_UECONXCLR_TXINEC_MASK)
  //! tests if all banks full (OUT) or free (IN) interrupt enabled
#define Is_usb_bank_interrupt_enabled(ep)         (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_NBUSYBKE_MASK))
  //! tests if SHORT PACKET received interrupt is enabled
#define Is_usb_short_packet_interrupt_enabled(ep) (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_SHORTPACKETE_MASK))
  //! tests if STALL sent interrupt is enabled
#define Is_usb_stall_interrupt_enabled(ep)        (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_STALLEDE_MASK))
  //! tests if CRC ERROR ISO OUT detected interrupt is enabled
#define Is_usb_crc_error_interrupt_enabled(ep)    (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_STALLEDE_MASK))
  //! tests if overflow interrupt is enabled
#define Is_usb_overflow_interrupt_enabled(ep)     (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_OVERFE_MASK))
  //! tests if NAK IN interrupt is enabled
#define Is_usb_nak_in_interrupt_enabled(ep)       (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_NAKINE_MASK))
  //! tests if NAK OUT interrupt is enabled
#define Is_usb_nak_out_interrupt_enabled(ep)      (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_NAKOUTE_MASK))
  //! tests if SETUP received interrupt is enabled
#define Is_usb_setup_received_interrupt_enabled() (Tst_bits(AVR32_USBB_ueconx(EP_CONTROL), AVR32_USBB_UECONX_RXSTPE_MASK))
  //! tests if underflow interrupt is enabled
#define Is_usb_underflow_interrupt_enabled(ep)    (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_RXSTPE_MASK))
  //! tests if OUT received interrupt is enabled
#define Is_usb_out_received_interrupt_enabled(ep) (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_RXOUTE_MASK))
  //! tests if OUT received on control endpoint interrupt is enabled
#define Is_usb_control_out_received_interrupt_enabled() (Tst_bits(AVR32_USBB_ueconx(EP_CONTROL), AVR32_USBB_UECONX_RXOUTE_MASK))
  //! tests if IN ready interrupt is enabled
#define Is_usb_in_ready_interrupt_enabled(ep)     (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_TXINE_MASK))
  //! tests if IN ready on control endpoint interrupt is enabled
#define Is_usb_control_in_ready_interrupt_enabled() (Tst_bits(AVR32_USBB_ueconx(EP_CONTROL), AVR32_USBB_UECONX_TXINE_MASK))

  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define Usb_get_endpoint_fifo_access(ep, scale) \
          (AVR32_USBB_FIFOX_DATA(ep, scale))

  //! Reset known position inside FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to reset known position
  //! @warning Always call this macro before any read/write macro/function
  //! when at FIFO beginning.
#define Usb_reset_endpoint_fifo_access(ep) \
          (pep_fifo[(ep)].u64ptr = Usb_get_endpoint_fifo_access(ep, 64))

  //! Read 64-, 32-, 16- or 8-bit data from FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       64-, 32-, 16- or 8-bit data read
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @note This macro assures that used HSB addresses are identical to the
  //! DPRAM internal pointer modulo 32 bits.
  //! @warning Always call Usb_reset_endpoint_fifo_access before this macro when
  //! at FIFO beginning.
  //! @warning Do not mix calls to this macro with calls to indexed macros below.
#define Usb_read_endpoint_data(ep, scale) \
          (*pep_fifo[(ep)].TPASTE3(u, scale, ptr)\
           TPASTE3(Pep_fifo_access_, scale, _post_inc)())

  //! Write 64-, 32-, 16- or 8-bit data to FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param data   64-, 32-, 16- or 8-bit data to write
  //! @return       64-, 32-, 16- or 8-bit data written
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @note This macro assures that used HSB addresses are identical to the
  //! DPRAM internal pointer modulo 32 bits.
  //! @warning Always call Usb_reset_endpoint_fifo_access before this macro when
  //! at FIFO beginning.
  //! @warning Do not mix calls to this macro with calls to indexed macros below.
#define Usb_write_endpoint_data(ep, scale, data) \
          (*pep_fifo[(ep)].TPASTE3(u, scale, ptr)\
           TPASTE3(Pep_fifo_access_, scale, _post_inc)() = (data))

  //! Read 64-, 32-, 16- or 8-bit indexed data from FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param index  Index of scaled data array to access
  //! @return       64-, 32-, 16- or 8-bit data read
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
  //! @warning Do not mix calls to this macro with calls to non-indexed macros above.
#define Usb_read_endpoint_indexed_data(ep, scale, index) \
          (AVR32_USBB_FIFOX_DATA(ep, scale)[(index)])

  //! Write 64-, 32-, 16- or 8-bit indexed data to FIFO data register of selected endpoint.
  //! @param ep     Endpoint of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param index  Index of scaled data array to access
  //! @param data   64-, 32-, 16- or 8-bit data to write
  //! @return       64-, 32-, 16- or 8-bit data written
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
  //! @warning Do not mix calls to this macro with calls to non-indexed macros above.
#define Usb_write_endpoint_indexed_data(ep, scale, index, data) \
          (AVR32_USBB_FIFOX_DATA(ep, scale)[(index)] = (data))
//! @}


//! @defgroup USBB_general_endpoint_dma USBB endpoint DMA drivers
//! These macros manage the common features of the endpoint DMA channels.
//! @{
  //! enables the disabling of HDMA requests by endpoint interrupts
#define Usb_enable_endpoint_int_dis_hdma_req(ep)      (AVR32_USBB_ueconxset(ep) = AVR32_USBB_UECONXSET_EPDISHDMAS_MASK)
  //! disables the disabling of HDMA requests by endpoint interrupts
#define Usb_disable_endpoint_int_dis_hdma_req(ep)     (AVR32_USBB_ueconxclr(ep) = AVR32_USBB_UECONXCLR_EPDISHDMAC_MASK)
  //! tests if the disabling of HDMA requests by endpoint interrupts is enabled
#define Is_usb_endpoint_int_dis_hdma_req_enabled(ep)  (Tst_bits(AVR32_USBB_ueconx(ep), AVR32_USBB_UECONX_EPDISHDMA_MASK))

  //! raises the selected endpoint DMA channel interrupt
#define Usb_raise_endpoint_dma_interrupt(epdma)       (AVR32_USBB_udintset = AVR32_USBB_UDINTSET_DMA1INTS_MASK << ((epdma) - 1))
  //! tests if an interrupt is triggered by the selected endpoint DMA channel
#define Is_usb_endpoint_dma_interrupt(epdma)          (Tst_bits(AVR32_USBB_udint, AVR32_USBB_UDINT_DMA1INT_MASK << ((epdma) - 1)))
  //! enables the selected endpoint DMA channel interrupt
#define Usb_enable_endpoint_dma_interrupt(epdma)      (AVR32_USBB_udinteset = AVR32_USBB_UDINTESET_DMA1INTES_MASK << ((epdma) - 1))
  //! disables the selected endpoint DMA channel interrupt
#define Usb_disable_endpoint_dma_interrupt(epdma)     (AVR32_USBB_udinteclr = AVR32_USBB_UDINTECLR_DMA1INTEC_MASK << ((epdma) - 1))
  //! tests if the selected endpoint DMA channel interrupt is enabled
#define Is_usb_endpoint_dma_interrupt_enabled(epdma)  (Tst_bits(AVR32_USBB_udinte, AVR32_USBB_UDINTE_DMA1INTE_MASK << ((epdma) - 1)))
//! @todo Implement macros for endpoint DMA registers and descriptors
#if 0
#define Usb_set_endpoint_dma_nxt_desc_addr(epdma, nxt_desc_addr) (AVR32_USBB_UDDMAX_NEXTDESC(epdma).nxt_desc_addr = (U32)(nxt_desc_addr))
#define Usb_get_endpoint_dma_nxt_desc_addr(epdma)                ((avr32_usbb_uxdmax_t *)AVR32_USBB_UDDMAX_NEXTDESC(epdma).nxt_desc_addr)
#define (epdma) (AVR32_USBB_UDDMAX_addr(epdma))
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).ch_byte_length)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).burst_lock_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).desc_ld_irq_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).eobuff_irq_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).eot_irq_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).dmaend_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).buff_close_in_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).ld_nxt_ch_desc_en)
#define (epdma) (AVR32_USBB_UDDMAX_CONTROL(epdma).ch_en)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).ch_byte_cnt)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).desc_ld_sta)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).eoch_buff_sta)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).eot_sta)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).ch_active)
#define (epdma) (AVR32_USBB_UDDMAX_STATUS(epdma).ch_en)
#endif
//! @}


//! @defgroup USBB_host_driver USBB host controller drivers
//! These macros manage the USBB Host controller.
//! @{
  //! enables SOF generation
#define Host_enable_sof()                      (Set_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_SOFE_MASK))
  //! disables SOF generation
#define Host_disable_sof()                     (Clr_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_SOFE_MASK))
  //! tests if SOF generation enabled
#define Is_host_sof_enabled()                  (Tst_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_SOFE_MASK))
  //! sends a USB Reset to the device
#define Host_send_reset()                      (Set_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_RESET_MASK))
  //! stops sending a USB Reset to the device
#define Host_stop_sending_reset()              (Clr_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_RESET_MASK))
  //! tests if USB Reset running
#define Is_host_sending_reset()                (Tst_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_RESET_MASK))
  //! sends a USB Resume to the device
#define Host_send_resume()                     (Set_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_RESUME_MASK))
  //! tests if USB Resume running
#define Is_host_sending_resume()               (Tst_bits(AVR32_USBB_uhcon, AVR32_USBB_UHCON_RESUME_MASK))

#ifdef AVR32_USBB_UHCON_SPDCONF
  //! Force device full speed mode (i.e. disable high speed)
#define Host_force_full_speed_mode()	        (Wr_bitfield(AVR32_USBB_uhcon, AVR32_USBB_UHCON_SPDCONF_MASK, 3))
  //! Enable high speed mode
#define Host_enable_high_speed_mode()	        (Wr_bitfield(AVR32_USBB_uhcon, AVR32_USBB_UHCON_SPDCONF_MASK, 0))
#endif

  //! enables host Start-of-Frame interrupt
#define Host_enable_sof_interrupt()            (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_HSOFIES_MASK)
  //! enables host Start-of-Frame interrupt
#define Host_disable_sof_interrupt()           (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_HSOFIEC_MASK)
#define Is_host_sof_interrupt_enabled()        (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_HSOFIE_MASK))
  //! acks SOF detection
#define Host_ack_sof()                         (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_HSOFIC_MASK)
  //! raises SOF detection
#define Host_raise_sof()                       (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_HSOFIS_MASK)
  //! tests if SOF detected
#define Is_host_sof()                          (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_HSOFI_MASK))

  //! enables host wake-up interrupt detection
#define Host_enable_hwup_interrupt()            (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_HWUPIES_MASK)
  //! disables host wake-up interrupt detection
#define Host_disable_hwup_interrupt()           (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_HWUPIEC_MASK)
#define Is_host_hwup_interrupt_enabled()        (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_HWUPIE_MASK))
  //! acks host wake-up detection
#define Host_ack_hwup()                         (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_HWUPIC_MASK)
  //! raises host wake-up detection
#define Host_raise_hwup()                       (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_HWUPIS_MASK)
  //! tests if host wake-up detected
#define Is_host_hwup()                          (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_HWUPI_MASK))

  //! enables host down stream resume sent interrupt detection
#define Host_enable_down_stream_resume_interrupt()            (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_RSMEDIES_MASK)
  //! disables host down stream resume sent interrupt detection
#define Host_disable_down_stream_resume_interrupt()           (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_RSMEDIEC_MASK)
#define Is_host_down_stream_resume_interrupt_enabled()        (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_RSMEDIE_MASK))
  //! acks host down stream resume sent
#define Host_ack_down_stream_resume()                         (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_RSMEDIC_MASK)
  //! raises host down stream resume sent
#define Host_raise_down_stream_resume()                       (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_RSMEDIS_MASK)
#define Is_host_down_stream_resume()                          (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_RSMEDI_MASK))

  //! enables host remote wake-up interrupt detection
#define Host_enable_remote_wakeup_interrupt()         (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_RXRSMIES_MASK)
  //! disables host remote wake-up interrupt detection
#define Host_disable_remote_wakeup_interrupt()        (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_RXRSMIEC_MASK)
#define Is_host_remote_wakeup_interrupt_enabled()     (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_RXRSMIE_MASK))
  //! acks host remote wake-up detection
#define Host_ack_remote_wakeup()                      (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_RXRSMIC_MASK)
  //! raises host remote wake-up detection
#define Host_raise_remote_wakeup()                    (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_RXRSMIS_MASK)
  //! tests if host remote wake-up detected
#define Is_host_remote_wakeup()                       (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_RXRSMI_MASK))

  //! enables host device connection interrupt
#define Host_enable_device_connection_interrupt()     (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_DCONNIES_MASK)
  //! disables USB device connection interrupt
#define Host_disable_device_connection_interrupt()    (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_DCONNIEC_MASK)
#define Is_host_device_connection_interrupt_enabled() (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_DCONNIE_MASK))
  //! acks device connection
#define Host_ack_device_connection()           (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_DCONNIC_MASK)
  //! raises device connection
#define Host_raise_device_connection()         (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_DCONNIS_MASK)
  //! tests if a USB device has been detected
#define Is_host_device_connection()            (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_DCONNI_MASK))

  //! enables host device disconnection interrupt
#define Host_enable_device_disconnection_interrupt()     (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_DDISCIES_MASK)
  //! disables USB device connection interrupt
#define Host_disable_device_disconnection_interrupt()    (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_DDISCIEC_MASK)
#define Is_host_device_disconnection_interrupt_enabled() (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_DDISCIE_MASK))
  //! acks device disconnection
#define Host_ack_device_disconnection()        (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_DDISCIC_MASK)
  //! raises device disconnection
#define Host_raise_device_disconnection()      (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_DDISCIS_MASK)
  //! tests if a USB device has been removed
#define Is_host_device_disconnection()         (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_DDISCI_MASK))

  //! enables host USB reset sent interrupt
#define Host_enable_reset_sent_interrupt()     (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_RSTIES_MASK)
  //! disables host USB reset sent interrupt
#define Host_disable_reset_sent_interrupt()    (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_RSTIEC_MASK)
#define Is_host_reset_sent_interrupt_enabled() (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_RSTIE_MASK))
  //! acks host USB reset sent
#define Host_ack_reset_sent()                  (AVR32_USBB_uhintclr = AVR32_USBB_UHINTCLR_RSTIC_MASK)
  //! raises host USB reset sent
#define Host_raise_reset_sent()                (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_RSTIS_MASK)
  //! tests if host USB reset sent
#define Is_host_reset_sent()                   (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_RSTI_MASK))

  //! sets the current frame number
#define Host_set_frame_number(fnum)            (Wr_bitfield(AVR32_USBB_uhfnum, AVR32_USBB_UHFNUM_FNUM_MASK, fnum))
  //! returns the current frame number
#define Host_frame_number()                    (Rd_bitfield(AVR32_USBB_uhfnum, AVR32_USBB_UHFNUM_FNUM_MASK))
  //! returns the current frame length
#define Host_frame_length()                    (Rd_bitfield(AVR32_USBB_uhfnum, AVR32_USBB_UHFNUM_FLENHIGH_MASK))

  //! configures the USB device address associated with the selected pipe
#define Host_configure_address(p, addr)        (Wr_bitfield(AVR32_USBB_uhaddrx(1 + ((p) >> 2)), AVR32_USBB_UHADDR1_UHADDR_P0_MASK << (((p) & 0x03) << 3), addr))
  //! gets the currently configured USB device address associated with the selected pipe
#define Host_get_configured_address(p)         (Rd_bitfield(AVR32_USBB_uhaddrx(1 + ((p) >> 2)), AVR32_USBB_UHADDR1_UHADDR_P0_MASK << (((p) & 0x03) << 3)))
//! @}


//! @defgroup USBB_general_pipe USBB pipe drivers
//! These macros manage the common features of the pipes.
//! @{
  //! enables the selected pipe
#define Host_enable_pipe(p)                    (Set_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))
  //! disables the selected pipe
#define Host_disable_pipe(p)                   (Clr_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))
  //! tests if the selected pipe is enabled
#define Is_host_pipe_enabled(p)                (Tst_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PEN0_MASK << (p)))

  //! tests if an interrupt is triggered by the selected pipe
#define Is_host_pipe_interrupt(p)              (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_P0INT_MASK << (p)))
  //! enables the selected pipe interrupt
#define Host_enable_pipe_interrupt(p)          (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_P0INTES_MASK << (p))
  //! disables the selected pipe interrupt
#define Host_disable_pipe_interrupt(p)         (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_P0INTEC_MASK << (p))
  //! tests if the selected pipe interrupt is enabled
#define Is_host_pipe_interrupt_enabled(p)      (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_P0INTE_MASK << (p)))
  //! returns the lowest pipe number generating a pipe interrupt or MAX_PEP_NB if none
#define Host_get_interrupt_pipe_number()       (ctz(((AVR32_USBB_uhint >> AVR32_USBB_UHINT_P0INT_OFFSET) &\
                                                     (AVR32_USBB_uhinte >> AVR32_USBB_UHINTE_P0INTE_OFFSET)) |\
                                                    (1 << MAX_PEP_NB)))

  //! configures the interrupt pipe request frequency (period in ms) for the selected pipe
#define Host_configure_pipe_int_req_freq(p, freq) (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_INTFRQ_MASK, freq))
  //! gets the configured interrupt pipe request frequency (period in ms) for the selected pipe
#define Host_get_pipe_int_req_freq(p)          (Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_INTFRQ_MASK))
  //! configures the selected pipe endpoint number
#define Host_configure_pipe_endpoint_number(p, ep_num) (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PEPNUM_MASK, ep_num))
  //! gets the configured selected pipe endpoint number
#define Host_get_pipe_endpoint_number(p)       (Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PEPNUM_MASK))
  //! configures the selected pipe type
#define Host_configure_pipe_type(p, type)      (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PTYPE_MASK, type))
  //! gets the configured selected pipe type
#define Host_get_pipe_type(p)                  (Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PTYPE_MASK))
  //! enables the bank autoswitch for the selected pipe
#define Host_enable_pipe_bank_autoswitch(p)    (Set_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_AUTOSW_MASK))
  //! disables the bank autoswitch for the selected pipe
#define Host_disable_pipe_bank_autoswitch(p)   (Clr_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_AUTOSW_MASK))
#define Is_host_pipe_bank_autoswitch_enabled(p) (Tst_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_AUTOSW_MASK))
  //! configures the selected pipe token
#define Host_configure_pipe_token(p, token)    (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PTOKEN_MASK, token))
  //! gets the configured selected pipe token
#define Host_get_pipe_token(p)                 (Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PTOKEN_MASK))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of USBB controller
  //! for pipe size bit-field.
#define Host_format_pipe_size(size)            (32 - clz(((U32)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! configures the selected pipe size
#define Host_configure_pipe_size(p, size)      (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PSIZE_MASK, Host_format_pipe_size(size)))
  //! gets the configured selected pipe size
#define Host_get_pipe_size(p)                  (8 << Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PSIZE_MASK))
  //! configures the selected pipe number of banks
#define Host_configure_pipe_bank(p, bank)      (Wr_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PBK_MASK, bank))
  //! gets the configured selected pipe number of banks
#define Host_get_pipe_bank(p)                  (Rd_bitfield(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PBK_MASK))
  //! allocates the configuration x in DPRAM memory
#define Host_allocate_memory(p)                (Set_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_ALLOC_MASK))
  //! un-allocates the configuration x in DPRAM memory
#define Host_unallocate_memory(p)              (Clr_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_ALLOC_MASK))
#define Is_host_memory_allocated(p)            (Tst_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_ALLOC_MASK))
  //! Enable PING management for the endpoint p
#define Host_enable_ping(p)                    (Set_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_PINGEN_MASK))

  //! configures selected pipe in one step
#define Host_configure_pipe(p, freq, ep_num, type, token, size, bank) \
(\
  Host_enable_pipe(p),\
  Wr_bits(AVR32_USBB_upcfgx(p), AVR32_USBB_UPCFGX_INTFRQ_MASK |\
                                AVR32_USBB_UPCFGX_PEPNUM_MASK |\
                                AVR32_USBB_UPCFGX_PTYPE_MASK  |\
                                AVR32_USBB_UPCFGX_PTOKEN_MASK |\
                                AVR32_USBB_UPCFGX_PSIZE_MASK  |\
                                AVR32_USBB_UPCFGX_PBK_MASK,    \
          (((U32)(freq  ) << AVR32_USBB_UPCFGX_INTFRQ_OFFSET) & AVR32_USBB_UPCFGX_INTFRQ_MASK) |\
          (((U32)(ep_num) << AVR32_USBB_UPCFGX_PEPNUM_OFFSET) & AVR32_USBB_UPCFGX_PEPNUM_MASK) |\
          (((U32)(type  ) << AVR32_USBB_UPCFGX_PTYPE_OFFSET ) & AVR32_USBB_UPCFGX_PTYPE_MASK ) |\
          (((U32)(token ) << AVR32_USBB_UPCFGX_PTOKEN_OFFSET) & AVR32_USBB_UPCFGX_PTOKEN_MASK) |\
          ( (U32)Host_format_pipe_size(size) << AVR32_USBB_UPCFGX_PSIZE_OFFSET               ) |\
          (((U32)(bank  ) << AVR32_USBB_UPCFGX_PBK_OFFSET   ) & AVR32_USBB_UPCFGX_PBK_MASK   )),\
  Host_allocate_memory(p),\
\
  Is_host_pipe_configured(p)\
)

  //! resets the selected pipe
#define Host_reset_pipe(p)                     (Set_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PRST0_MASK << (p)),\
                                                Clr_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PRST0_MASK << (p)))
  //! tests if the selected pipe is being reset
#define Is_host_resetting_pipe(p)              (Tst_bits(AVR32_USBB_uprst, AVR32_USBB_UPRST_PRST0_MASK << (p)))

  //! freezes the pipe
#define Host_freeze_pipe(p)                    (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_PFREEZES_MASK)
  //! unfreezees the pipe
#define Host_unfreeze_pipe(p)                  (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_PFREEZEC_MASK)
  //! tests if the current pipe is frozen
#define Is_host_pipe_frozen(p)                 (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_PFREEZE_MASK))

  //! resets the data toggle sequence
#define Host_reset_data_toggle(p)              (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_RSTDTS_MASK)
  //! tests if the data toggle sequence is being reset
#define Is_host_data_toggle_reset(p)           (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_RSTDT_MASK))

  //! acks pipe overflow interrupt
#define Host_ack_overflow_interrupt(p)         (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_OVERFIC_MASK)
  //! raises pipe overflow interrupt
#define Host_raise_overflow_interrupt(p)       (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_OVERFIS_MASK)
  //! acks pipe underflow interrupt
#define Host_ack_underflow_interrupt(p)        (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_TXSTPIC_MASK)
  //! raises pipe underflow interrupt
#define Host_raise_underflow_interrupt(p)      (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_TXSTPIS_MASK)
  //! tests if an overflow occurs
#define Is_host_overflow(p)                    (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_OVERFI_MASK))
  //! tests if an underflow occurs
#define Is_host_underflow(p)                   (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_TXSTPI_MASK))

  //! returns data toggle
#define Host_data_toggle(p)                    (Rd_bitfield(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_DTSEQ_MASK))
  //! returns the number of busy banks
#define Host_nb_busy_bank(p)                   (Rd_bitfield(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_NBUSYBK_MASK))
  //! returns the number of the current bank
#define Host_current_bank(p)                   (Rd_bitfield(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_CURRBK_MASK))
  //! tests if current pipe is configured
#define Is_host_pipe_configured(p)             (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_CFGOK_MASK))
  //! returns the byte count
#define Host_byte_count(p)                     (Rd_bitfield(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_PBYCT_MASK))

  //! tests if a STALL has been received
#define Is_host_stall(p)                       (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_RXSTALLDI_MASK))
  //! tests if CRC ERROR ISO IN detected
#define Is_host_crc_error(p)                   (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_RXSTALLDI_MASK))
  //! tests if an error occurs on current pipe
#define Is_host_pipe_error(p)                  (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_PERRI_MASK))
  //! tests if SHORT PACKET received
#define Is_host_short_packet(p)                (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_SHORTPACKETI_MASK))

  //! clears FIFOCON bit
#define Host_ack_fifocon(p)                    (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_FIFOCONC_MASK)

  //! acks setup
#define Host_ack_setup_ready()                 (AVR32_USBB_upstaxclr(P_CONTROL) = AVR32_USBB_UPSTAXCLR_TXSTPIC_MASK)
  //! raises setup
#define Host_raise_setup_ready()               (AVR32_USBB_upstaxset(P_CONTROL) = AVR32_USBB_UPSTAXSET_TXSTPIS_MASK)
  //! sends current bank for SETUP pipe
#define Host_send_setup()                      (Host_ack_fifocon(P_CONTROL))
  //! acks setup and sends current bank
#define Host_ack_setup_ready_send()            (Host_ack_setup_ready(), Host_send_setup())
  //! acks OUT sent
#define Host_ack_out_ready(p)                  (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_TXOUTIC_MASK)
  //! raises OUT sent
#define Host_raise_out_ready(p)                (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_TXOUTIS_MASK)
  //! sends current bank for OUT pipe
#define Host_send_out(p)                       (Host_ack_fifocon(p))
  //! acks OUT sent and sends current bank
#define Host_ack_out_ready_send(p)             (Host_ack_out_ready(p), Host_send_out(p))
  //! acks control OUT
#define Host_ack_control_out_ready()           (AVR32_USBB_upstaxclr(P_CONTROL) = AVR32_USBB_UPSTAXCLR_TXOUTIC_MASK)
  //! raises control OUT
#define Host_raise_control_out_ready()         (AVR32_USBB_upstaxset(P_CONTROL) = AVR32_USBB_UPSTAXSET_TXOUTIS_MASK)
  //! sends current bank for OUT control pipe
#define Host_send_control_out()                (Host_ack_fifocon(P_CONTROL))
  //! acks control OUT and sends current bank
#define Host_ack_control_out_ready_send()      (Host_ack_control_out_ready(), Host_send_control_out())

  //! acks IN reception
#define Host_ack_in_received(p)                (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_RXINIC_MASK)
  //! raises IN reception
#define Host_raise_in_received(p)              (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_RXINIS_MASK)
  //! frees current bank for IN pipe
#define Host_free_in(p)                        (Host_ack_fifocon(p))
  //! acks IN reception and frees current bank
#define Host_ack_in_received_free(p)           (Host_ack_in_received(p), Host_free_in(p))
  //! acks control IN
#define Host_ack_control_in_received()         (AVR32_USBB_upstaxclr(P_CONTROL) = AVR32_USBB_UPSTAXCLR_RXINIC_MASK)
  //! raises control IN
#define Host_raise_control_in_received()       (AVR32_USBB_upstaxset(P_CONTROL) = AVR32_USBB_UPSTAXSET_RXINIS_MASK)
  //! frees current bank for IN control pipe
#define Host_free_control_in()                 (Host_ack_fifocon(P_CONTROL))
  //! acks control IN and frees current bank
#define Host_ack_control_in_received_free()    (Host_ack_control_in_received(), Host_free_control_in())

  //! tests if FIFOCON bit set
#define Is_host_fifocon(p)                     (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_FIFOCON_MASK))

  //! tests if SETUP has been sent
#define Is_host_setup_ready()                  (Tst_bits(AVR32_USBB_upstax(P_CONTROL), AVR32_USBB_UPSTAX_TXSTPI_MASK))
  //! tests if current bank sent for SETUP pipe
#define Is_host_setup_sent()                   (Is_host_fifocon(P_CONTROL))
  //! tests if OUT has been sent
#define Is_host_out_ready(p)                   (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_TXOUTI_MASK))
  //! tests if current bank sent for OUT pipe
#define Is_host_out_sent(p)                    (Is_host_fifocon(p))
  //! tests if control OUT has been sent
#define Is_host_control_out_ready()            (Tst_bits(AVR32_USBB_upstax(P_CONTROL), AVR32_USBB_UPSTAX_TXOUTI_MASK))
  //! tests if current bank sent for OUT control pipe
#define Is_host_control_out_sent()             (Is_host_fifocon(P_CONTROL))

  //! tests if IN received
#define Is_host_in_received(p)                 (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_RXINI_MASK))
  //! tests if IN received in current bank
#define Is_host_in_filled(p)                   (Is_host_fifocon(p))
  //! tests if control IN has been received
#define Is_host_control_in_received()          (Tst_bits(AVR32_USBB_upstax(P_CONTROL), AVR32_USBB_UPSTAX_RXINI_MASK))
  //! tests if IN received in current bank for control pipe
#define Is_host_control_in_filled()            (Is_host_fifocon(P_CONTROL))

  //! acks STALL reception
#define Host_ack_stall(p)                      (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_RXSTALLDIC_MASK)
  //! raises STALL reception
#define Host_raise_stall(p)                    (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_RXSTALLDIS_MASK)
  //! acks CRC ERROR ISO IN detected
#define Host_ack_crc_error(p)                  (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_RXSTALLDIC_MASK)
  //! raises CRC ERROR ISO IN detected
#define Host_raise_crc_error(p)                (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_RXSTALLDIS_MASK)
  //! acks pipe error
#define Host_ack_pipe_error(p)                 (AVR32_USBB_upstaxset(p) = (Is_host_pipe_error(p)) ? AVR32_USBB_UPSTAXSET_PERRIS_MASK : 0)
  //! raises pipe error
#define Host_raise_pipe_error(p)               (AVR32_USBB_upstaxset(p) = (Is_host_pipe_error(p)) ? 0 : AVR32_USBB_UPSTAXSET_PERRIS_MASK)
  //! acks SHORT PACKET received
#define Host_ack_short_packet(p)               (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_SHORTPACKETIC_MASK)
  //! raises SHORT PACKET received
#define Host_raise_short_packet(p)             (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_SHORTPACKETIS_MASK)

  //! tests if NAK handshake has been received
#define Is_host_nak_received(p)                (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_NAKEDI_MASK))
  //! acks NAK received
#define Host_ack_nak_received(p)               (AVR32_USBB_upstaxclr(p) = AVR32_USBB_UPSTAXCLR_NAKEDIC_MASK)
  //! raises NAK received
#define Host_raise_nak_received(p)             (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_NAKEDIS_MASK)

  //! tests if pipe read allowed
#define Is_host_read_enabled(p)                (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_RWALL_MASK))
  //! tests if pipe write allowed
#define Is_host_write_enabled(p)               (Tst_bits(AVR32_USBB_upstax(p), AVR32_USBB_UPSTAX_RWALL_MASK))

  //! enables continuous IN mode
#define Host_enable_continuous_in_mode(p)      (Set_bits(AVR32_USBB_upinrqx(p), AVR32_USBB_UPINRQX_INMODE_MASK))
  //! disables continuous IN mode
#define Host_disable_continuous_in_mode(p)     (Clr_bits(AVR32_USBB_upinrqx(p), AVR32_USBB_UPINRQX_INMODE_MASK))
  //! tests if continuous IN mode is enabled
#define Is_host_continuous_in_mode_enabled(p)  (Tst_bits(AVR32_USBB_upinrqx(p), AVR32_USBB_UPINRQX_INMODE_MASK))

  //! sets number of IN requests to perform before freeze
#define Host_in_request_number(p, in_num)      (Wr_bitfield(AVR32_USBB_upinrqx(p), AVR32_USBB_UPINRQX_INRQ_MASK, (in_num) - 1))
  //! returns number of remaining IN requests
#define Host_get_in_request_number(p)          (Rd_bitfield(AVR32_USBB_upinrqx(p), AVR32_USBB_UPINRQX_INRQ_MASK) + 1)

  //! acks all pipe error
#define Host_ack_all_errors(p)                 (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATATGL_MASK |\
                                                                               AVR32_USBB_UPERRX_DATAPID_MASK |\
                                                                               AVR32_USBB_UPERRX_PID_MASK     |\
                                                                               AVR32_USBB_UPERRX_TIMEOUT_MASK |\
                                                                               AVR32_USBB_UPERRX_CRC16_MASK   |\
                                                                               AVR32_USBB_UPERRX_COUNTER_MASK))

  //! tests if error occurs on pipe
#define Host_error_status(p)                   (Rd_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATATGL_MASK |\
                                                                              AVR32_USBB_UPERRX_DATAPID_MASK |\
                                                                              AVR32_USBB_UPERRX_PID_MASK     |\
                                                                              AVR32_USBB_UPERRX_TIMEOUT_MASK |\
                                                                              AVR32_USBB_UPERRX_CRC16_MASK))

  //! acks bad data toggle
#define Host_ack_bad_data_toggle(p)            (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATATGL_MASK))
#define Is_host_bad_data_toggle(p)             (Tst_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATATGL_MASK))
  //! acks data PID error
#define Host_ack_data_pid_error(p)             (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATAPID_MASK))
#define Is_host_data_pid_error(p)              (Tst_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_DATAPID_MASK))
  //! acks PID error
#define Host_ack_pid_error(p)                  (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_PID_MASK))
#define Is_host_pid_error(p)                   (Tst_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_PID_MASK))
  //! acks time-out error
#define Host_ack_timeout_error(p)              (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_TIMEOUT_MASK))
#define Is_host_timeout_error(p)               (Tst_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_TIMEOUT_MASK))
  //! acks CRC16 error
#define Host_ack_crc16_error(p)                (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_CRC16_MASK))
#define Is_host_crc16_error(p)                 (Tst_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_CRC16_MASK))
  //! clears the error counter
#define Host_clear_error_counter(p)            (Clr_bits(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_COUNTER_MASK))
#define Host_get_error_counter(p)              (Rd_bitfield(AVR32_USBB_uperrx(p), AVR32_USBB_UPERRX_COUNTER_MASK))

  //! enables overflow interrupt
#define Host_enable_overflow_interrupt(p)      (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_OVERFIES_MASK)
  //! disables overflow interrupt
#define Host_disable_overflow_interrupt(p)     (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_OVERFIEC_MASK)
  //! tests if overflow interrupt is enabled
#define Is_host_overflow_interrupt_enabled(p)  (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_OVERFIE_MASK))

  //! enables underflow interrupt
#define Host_enable_underflow_interrupt(p)     (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_TXSTPES_MASK)
  //! disables underflow interrupt
#define Host_disable_underflow_interrupt(p)    (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_TXSTPEC_MASK)
  //! tests if underflow interrupt is enabled
#define Is_host_underflow_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_TXSTPE_MASK))

  //! forces all banks full (OUT) or free (IN) interrupt
#define Host_force_bank_interrupt(p)           (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_NBUSYBKS_MASK)
  //! unforces all banks full (OUT) or free (IN) interrupt
#define Host_unforce_bank_interrupt(p)         (AVR32_USBB_upstaxset(p) = AVR32_USBB_UPSTAXSET_NBUSYBKS_MASK)
  //! enables all banks full (IN) or free (OUT) interrupt
#define Host_enable_bank_interrupt(p)          (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_NBUSYBKES_MASK)
  //! disables all banks full (IN) or free (OUT) interrupt
#define Host_disable_bank_interrupt(p)         (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_NBUSYBKEC_MASK)
  //! tests if all banks full (IN) or free (OUT) interrupt is enabled
#define Is_host_bank_interrupt_enabled(p)      (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_NBUSYBKE_MASK))

  //! enables SHORT PACKET received interrupt
#define Host_enable_short_packet_interrupt(p)  (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_SHORTPACKETIES_MASK)
  //! disables SHORT PACKET received interrupt
#define Host_disable_short_packet_interrupt(p) (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_SHORTPACKETIEC_MASK)
  //! tests if SHORT PACKET received interrupt is enabled
#define Is_host_short_packet_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_SHORTPACKETIE_MASK))

  //! enables STALL received interrupt
#define Host_enable_stall_interrupt(p)         (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_RXSTALLDES_MASK)
  //! disables STALL received interrupt
#define Host_disable_stall_interrupt(p)        (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_RXSTALLDEC_MASK)
  //! tests if STALL received interrupt is enabled
#define Is_host_stall_interrupt_enabled(p)     (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_RXSTALLDE_MASK))

  //! enables CRC ERROR ISO IN detected interrupt
#define Host_enable_crc_error_interrupt(p)     (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_RXSTALLDES_MASK)
  //! disables CRC ERROR ISO IN detected interrupt
#define Host_disable_crc_error_interrupt(p)    (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_RXSTALLDEC_MASK)
  //! tests if CRC ERROR ISO IN detected interrupt is enabled
#define Is_host_crc_error_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_RXSTALLDE_MASK))

  //! enables NAK received interrupt
#define Host_enable_nak_received_interrupt(p)  (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_NAKEDES_MASK)
  //! disables NAK received interrupt
#define Host_disable_nak_received_interrupt(p) (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_NAKEDEC_MASK)
  //! tests if NAK received interrupt is enabled
#define Is_host_nak_received_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_NAKEDE_MASK))

  //! enables pipe error interrupt
#define Host_enable_pipe_error_interrupt(p)    (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_PERRES_MASK)
  //! disables pipe error interrupt
#define Host_disable_pipe_error_interrupt(p)   (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_PERREC_MASK)
  //! tests if pipe error interrupt is enabled
#define Is_host_pipe_error_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_PERRE_MASK))

  //! enables SETUP pipe ready interrupt
#define Host_enable_setup_ready_interrupt()    (AVR32_USBB_upconxset(P_CONTROL) = AVR32_USBB_UPCONXSET_TXSTPES_MASK)
  //! disables SETUP pipe ready interrupt
#define Host_disable_setup_ready_interrupt()   (AVR32_USBB_upconxclr(P_CONTROL) = AVR32_USBB_UPCONXCLR_TXSTPEC_MASK)
  //! tests if SETUP pipe ready interrupt is enabled
#define Is_host_setup_ready_interrupt_enabled() (Tst_bits(AVR32_USBB_upconx(P_CONTROL), AVR32_USBB_UPCONX_TXSTPE_MASK))

  //! enables OUT pipe ready interrupt
#define Host_enable_out_ready_interrupt(p)     (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_TXOUTES_MASK)
  //! disables OUT pipe ready interrupt
#define Host_disable_out_ready_interrupt(p)    (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_TXOUTEC_MASK)
  //! tests if OUT pipe ready interrupt is enabled
#define Is_host_out_ready_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_TXOUTE_MASK))

  //! enables OUT control pipe ready interrupt
#define Host_enable_control_out_ready_interrupt() (AVR32_USBB_upconxset(P_CONTROL) = AVR32_USBB_UPCONXSET_TXOUTES_MASK)
  //! disables OUT control pipe ready interrupt
#define Host_disable_control_out_ready_interrupt() (AVR32_USBB_upconxclr(P_CONTROL) = AVR32_USBB_UPCONXCLR_TXOUTEC_MASK)
  //! tests if OUT control pipe ready interrupt is enabled
#define Is_host_control_out_ready_interrupt_enabled() (Tst_bits(AVR32_USBB_upconx(P_CONTROL), AVR32_USBB_UPCONX_TXOUTE_MASK))

  //! enables IN pipe reception interrupt
#define Host_enable_in_received_interrupt(p)   (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_RXINES_MASK)
  //! disables IN pipe reception interrupt
#define Host_disable_in_received_interrupt(p)  (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_RXINEC_MASK)
  //! tests if IN pipe reception interrupt is enabled
#define Is_host_in_received_interrupt_enabled(p) (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_RXINE_MASK))

  //! enables IN control pipe reception interrupt
#define Host_enable_control_in_received_interrupt() (AVR32_USBB_upconxset(P_CONTROL) = AVR32_USBB_UPCONXSET_RXINES_MASK)
  //! disables IN control pipe reception interrupt
#define Host_disable_control_in_received_interrupt() (AVR32_USBB_upconxclr(P_CONTROL) = AVR32_USBB_UPCONXCLR_RXINEC_MASK)
  //! tests if IN control pipe reception interrupt is enabled
#define Is_host_control_in_received_interrupt_enabled() (Tst_bits(AVR32_USBB_upconx(P_CONTROL), AVR32_USBB_UPCONX_RXINE_MASK))

  //! Get 64-, 32-, 16- or 8-bit access to FIFO data register of selected pipe.
  //! @param p      Pipe of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       Volatile 64-, 32-, 16- or 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define Host_get_pipe_fifo_access(p, scale) \
          (AVR32_USBB_FIFOX_DATA(p, scale))

  //! Reset known position inside FIFO data register of selected pipe.
  //! @param p      Pipe of which to reset known position
  //! @warning Always call this macro before any read/write macro/function
  //! when at FIFO beginning.
#define Host_reset_pipe_fifo_access(p) \
          (pep_fifo[(p)].u64ptr = Host_get_pipe_fifo_access(p, 64))

  //! Read 64-, 32-, 16- or 8-bit data from FIFO data register of selected pipe.
  //! @param p      Pipe of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @return       64-, 32-, 16- or 8-bit data read
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @note This macro assures that used HSB addresses are identical to the
  //! DPRAM internal pointer modulo 32 bits.
  //! @warning Always call Host_reset_pipe_fifo_access before this macro when
  //! at FIFO beginning.
  //! @warning Do not mix calls to this macro with calls to indexed macros below.
#define Host_read_pipe_data(p, scale) \
          (*pep_fifo[(p)].TPASTE3(u, scale, ptr)\
           TPASTE3(Pep_fifo_access_, scale, _post_inc)())

  //! Write 64-, 32-, 16- or 8-bit data to FIFO data register of selected pipe.
  //! @param p      Pipe of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param data   64-, 32-, 16- or 8-bit data to write
  //! @return       64-, 32-, 16- or 8-bit data written
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @note This macro assures that used HSB addresses are identical to the
  //! DPRAM internal pointer modulo 32 bits.
  //! @warning Always call Host_reset_pipe_fifo_access before this macro when
  //! at FIFO beginning.
  //! @warning Do not mix calls to this macro with calls to indexed macros below.
#define Host_write_pipe_data(p, scale, data) \
          (*pep_fifo[(p)].TPASTE3(u, scale, ptr)\
           TPASTE3(Pep_fifo_access_, scale, _post_inc)() = (data))

  //! Read 64-, 32-, 16- or 8-bit indexed data from FIFO data register of selected pipe.
  //! @param p      Pipe of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param index  Index of scaled data array to access
  //! @return       64-, 32-, 16- or 8-bit data read
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
  //! @warning Do not mix calls to this macro with calls to non-indexed macros above.
#define Host_read_pipe_indexed_data(p, scale, index) \
          (AVR32_USBB_FIFOX_DATA(p, scale)[(index)])

  //! Write 64-, 32-, 16- or 8-bit indexed data to FIFO data register of selected pipe.
  //! @param p      Pipe of which to access FIFO data register
  //! @param scale  Data scale in bits: 64, 32, 16 or 8
  //! @param index  Index of scaled data array to access
  //! @param data   64-, 32-, 16- or 8-bit data to write
  //! @return       64-, 32-, 16- or 8-bit data written
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries except 64-bit accesses which
  //! require only 32-bit alignment.
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
  //! @warning Do not mix calls to this macro with calls to non-indexed macros above.
#define Host_write_pipe_indexed_data(p, scale, index, data) \
          (AVR32_USBB_FIFOX_DATA(p, scale)[(index)] = (data))
//! @}


//! @defgroup USBB_general_pipe_dma USBB pipe DMA drivers
//! These macros manage the common features of the pipe DMA channels.
//! @{
  //! enables the disabling of HDMA requests by pipe interrupts
#define Host_enable_pipe_int_dis_hdma_req(p)      (AVR32_USBB_upconxset(p) = AVR32_USBB_UPCONXSET_PDISHDMAS_MASK)
  //! disables the disabling of HDMA requests by pipe interrupts
#define Host_disable_pipe_int_dis_hdma_req(p)     (AVR32_USBB_upconxclr(p) = AVR32_USBB_UPCONXCLR_PDISHDMAC_MASK)
  //! tests if the disabling of HDMA requests by pipe interrupts si enabled
#define Is_host_pipe_int_dis_hdma_req_enabled(p)  (Tst_bits(AVR32_USBB_upconx(p), AVR32_USBB_UPCONX_PDISHDMA_MASK))

  //! raises the selected pipe DMA channel interrupt
#define Host_raise_pipe_dma_interrupt(pdma)       (AVR32_USBB_uhintset = AVR32_USBB_UHINTSET_DMA1INTS_MASK << ((pdma) - 1))
  //! tests if an interrupt is triggered by the selected pipe DMA channel
#define Is_host_pipe_dma_interrupt(pdma)          (Tst_bits(AVR32_USBB_uhint, AVR32_USBB_UHINT_DMA1INT_MASK << ((pdma) - 1)))
  //! enables the selected pipe DMA channel interrupt
#define Host_enable_pipe_dma_interrupt(pdma)      (AVR32_USBB_uhinteset = AVR32_USBB_UHINTESET_DMA1INTES_MASK << ((pdma) - 1))
  //! disables the selected pipe DMA channel interrupt
#define Host_disable_pipe_dma_interrupt(pdma)     (AVR32_USBB_uhinteclr = AVR32_USBB_UHINTECLR_DMA1INTEC_MASK << ((pdma) - 1))
  //! tests if the selected pipe DMA channel interrupt is enabled
#define Is_host_pipe_dma_interrupt_enabled(pdma)  (Tst_bits(AVR32_USBB_uhinte, AVR32_USBB_UHINTE_DMA1INTE_MASK << ((pdma) - 1)))
//! @todo Implement macros for pipe DMA registers and descriptors
//! @}

//! @}


//_____ D E C L A R A T I O N S ____________________________________________

extern  UnionVPtr       pep_fifo[MAX_PEP_NB];

#if USB_DEVICE_FEATURE == true
extern  Status_bool_t   usb_init_device         (                             void   );
extern  U32             usb_set_ep_txpacket     (U8,         U8  , U32               );
extern  U32             usb_write_ep_txpacket   (U8, const void *, U32, const void **);
extern  U32             usb_read_ep_rxpacket    (U8,       void *, U32,       void **);
#endif

#if USB_HOST_FEATURE == true
extern  void            host_disable_all_pipes  (                             void   );
extern  U32             host_set_p_txpacket     (U8,         U8  , U32               );
extern  U32             host_write_p_txpacket   (U8, const void *, U32, const void **);
extern  U32             host_read_p_rxpacket    (U8,       void *, U32,       void **);
#endif


#endif  // _USB_DRV_H_
