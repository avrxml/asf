/**
 * \file
 *
 * \brief FLASHCDW driver for AVR32 UC3.
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


#ifndef _FLASHCDW_H_
#define _FLASHCDW_H_

/**
 * \defgroup group_avr32_drivers_flashcdw FLASHCDW - FLASH Controller Double-Word
 *
 * FLASHCDW interfaces a flash block with the 32-bit internal HSB bus.
 *
 * \{
 */

#include <avr32/io.h>
#include <stddef.h>
#include "compiler.h"


#ifdef AVR32_FLASHCDW_101_H_INCLUDED

#define AVR32_FLASHCDW_FGPFRLO_UPROT                                    22
#define AVR32_FLASHCDW_FGPFRLO_UPROT_MASK                       0x00400000
#define AVR32_FLASHCDW_FGPFRLO_UPROT_OFFSET                             22
#define AVR32_FLASHCDW_FGPFRLO_UPROT_SIZE                                1

#define AVR32_FLASHCDW_FGPFRLO_SECURE                                   20
#define AVR32_FLASHCDW_FGPFRLO_SECURE_MASK                      0x00300000
#define AVR32_FLASHCDW_FGPFRLO_SECURE_OFFSET                            20
#define AVR32_FLASHCDW_FGPFRLO_SECURE_SIZE                               2

//! Secure bits value when Secure State is enabled and Secure State Debug is disabled
#define AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDDIS                         2

//! Secure bits value when Secure State is enabled and Secure State Debug is enabled
#define AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDEN                          1

// These defines are missing from or wrong in the toolchain header file flashcdw_101.h
// #9267
#endif


//! Number of flash regions defined by the FLASHCDW.
#define AVR32_FLASHCDW_REGIONS  (AVR32_FLASHCDW_FLASH_SIZE /\
		(AVR32_FLASHCDW_PAGES_PR_REGION * AVR32_FLASHCDW_PAGE_SIZE))


/*! \name Flash Properties
 */
//! @{

/*! \brief Gets the size of the whole flash array.
 *
 * \return The size of the whole flash array in bytes.
 */
extern unsigned int flashcdw_get_flash_size(void);

/*! \brief Gets the total number of pages in the flash array.
 *
 * \return The total number of pages in the flash array.
 */
extern unsigned int flashcdw_get_page_count(void);

/*! \brief Gets the number of pages in each flash region.
 *
 * \return The number of pages in each flash region.
 */
extern unsigned int flashcdw_get_page_count_per_region(void);

/*! \brief Gets the region number of a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return The region number of the specified page.
 */
extern unsigned int flashcdw_get_page_region(int page_number);

/*! \brief Gets the number of the first page of a region.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHCDW_REGIONS - 1)</tt>.
 *
 * \return The number of the first page of the specified region.
 */
extern unsigned int flashcdw_get_region_first_page_number(unsigned int region);

//! @}


/*! \name FLASHCDW Control
 */
//! @{

/*! \brief Gets the number of wait states of flash read accesses.
 *
 * \return The number of wait states of flash read accesses.
 */
extern unsigned int flashcdw_get_wait_state(void);

/*! \brief Sets the number of wait states of flash read accesses.
 *
 * \param wait_state The number of wait states of flash read accesses: \c 0 to
 *                   \c 1.
 */
extern void flashcdw_set_wait_state(unsigned int wait_state);

/*! \brief Depending on the CPU frequency, set the wait states of flash read
 *         accesses and enable or disable the High speed read mode.
 *
 * \param cpu_f_hz The CPU frequency
 */
extern void flashcdw_set_flash_waitstate_and_readmode(unsigned long cpu_f_hz);

/*! \brief Alias on the flashcdw_set_flash_waitstate_and_readmode() function.
 *
 * \param cpu_f_hz The CPU frequency
 */
#define flash_set_bus_freq(cpu_f_hz) flashcdw_set_flash_waitstate_and_readmode(cpu_f_hz)

/*! \brief Tells whether the Flash Ready interrupt is enabled.
 *
 * \return Whether the Flash Ready interrupt is enabled.
 */
extern bool flashcdw_is_ready_int_enabled(void);

/*! \brief Enables or disables the Flash Ready interrupt.
 *
 * \param enable Whether to enable the Flash Ready interrupt: \c true or
 *               \c false.
 */
extern void flashcdw_enable_ready_int(bool enable);

/*! \brief Tells whether the Lock Error interrupt is enabled.
 *
 * \return Whether the Lock Error interrupt is enabled.
 */
extern bool flashcdw_is_lock_error_int_enabled(void);

/*! \brief Enables or disables the Lock Error interrupt.
 *
 * \param enable Whether to enable the Lock Error interrupt: \c true or
 *               \c false.
 */
extern void flashcdw_enable_lock_error_int(bool enable);

/*! \brief Tells whether the Programming Error interrupt is enabled.
 *
 * \return Whether the Programming Error interrupt is enabled.
 */
extern bool flashcdw_is_prog_error_int_enabled(void);

/*! \brief Enables or disables the Programming Error interrupt.
 *
 * \param enable Whether to enable the Programming Error interrupt: \c true or
 *               \c false.
 */
extern void flashcdw_enable_prog_error_int(bool enable);

//! @}


/*! \name FLASHCDW Status
 */
//! @{

/*! \brief Tells whether the FLASHCDW is ready to run a new command.
 *
 * \return Whether the FLASHCDW is ready to run a new command.
 */
extern bool flashcdw_is_ready(void);

/*! \brief Waits actively until the FLASHCDW is ready to run a new command.
 *
 * This is the default function assigned to \ref flashcdw_wait_until_ready.
 */
extern void flashcdw_default_wait_until_ready(void);

//! Pointer to the function used by the driver when it needs to wait until the
//! FLASHCDW is ready to run a new command.
//! The default function is \ref flashcdw_default_wait_until_ready.
//! The user may change this pointer to use another implementation.
extern void (*volatile flashcdw_wait_until_ready)(void);

/*! \brief Tells whether a Lock Error has occurred during the last function
 *         called that issued one or more FLASHCDW commands.
 *
 * \return Whether a Lock Error has occurred during the last function called
 *         that issued one or more FLASHCDW commands.
 */
extern bool flashcdw_is_lock_error(void);

/*! \brief Tells whether a Programming Error has occurred during the last
 *         function called that issued one or more FLASHCDW commands.
 *
 * \return Whether a Programming Error has occurred during the last function
 *         called that issued one or more FLASHCDW commands.
 */
extern bool flashcdw_is_programming_error(void);

/*! \brief Tells whether the High-speed read mode is enabled.
 *
 * \return 1 if the high-speed read mode is enabled, else 0.
 */
extern bool flashcdw_is_high_speed_enabled(void);

//! @}


/*! \name FLASHCDW Command Control
 */
//! @{

/*! \brief Gets the last issued FLASHCDW command.
 *
 * \return The last issued FLASHCDW command.
 */
extern unsigned int flashcdw_get_command(void);

/*! \brief Gets the current FLASHCDW page number.
 *
 * \return The current FLASHCDW page number.
 */
extern unsigned int flashcdw_get_page_number(void);

/*! \brief Issues a FLASHCDW command.
 *
 * \param command The command: \c AVR32_FLASHCDW_FCMD_CMD_x.
 * \param page_number The page number to apply the command to:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: use this to apply the command to the current page number
 *        or if the command does not apply to any page number;
 *   \arg this argument may have other meanings according to the command. See
 *        the FLASHCDW chapter of the MCU datasheet.
 *
 * \warning A Lock Error is issued if the command violates the protection
 *          mechanism.
 *
 * \warning A Programming Error is issued if the command is invalid.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_issue_command(unsigned int command, int page_number);

//! @}


/*! \name FLASHCDW Global Commands
 */
//! @{

/*! \brief Issues a No Operation command to the FLASHCDW.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_no_operation(void);

/*! \brief Issues an Erase All command to the FLASHCDW.
 *
 * This command erases all bits in the flash array, the general-purpose fuse
 * bits and the Security bit. The User page is not erased.
 *
 * This command also ensures that all volatile memories, such as register file
 * and RAMs, are erased before the Security bit is erased, i.e. deactivated.
 *
 * \warning A Lock Error is issued if at least one region is locked or the
 *          bootloader protection is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern void flashcdw_erase_all(void);

//! @}


/*! \name FLASHCDW Protection Mechanisms
 */
//! @{

/*! \brief Tells whether the Security bit is active.
 *
 * \return Whether the Security bit is active.
 */
extern bool flashcdw_is_security_bit_active(void);

/*! \brief Activates the Security bit.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_activate_security_bit(void);

/*! \brief Gets the bootloader protected size.
 *
 * \return The bootloader protected size in bytes.
 */
extern unsigned int flashcdw_get_bootloader_protected_size(void);

/*! \brief Sets the bootloader protected size.
 *
 * \param bootprot_size The wanted bootloader protected size in bytes. If this
 *                      size is not supported, the actual size will be the
 *                      nearest greater available size or the maximal possible
 *                      size if the requested size is too large.
 *
 * \return The actual bootloader protected size in bytes.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern unsigned int flashcdw_set_bootloader_protected_size(unsigned int bootprot_size);

/*! \brief Tells whether external privileged fetch is locked.
 *
 * \return Whether external privileged fetch is locked.
 */
extern bool flashcdw_is_external_privileged_fetch_locked(void);

/*! \brief Locks or unlocks external privileged fetch.
 *
 * \param lock Whether to lock external privileged fetch: \c true or \c false.
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_lock_external_privileged_fetch(bool lock);

/*! \brief Tells whether the jtag user protection is enabled.
 *
 * \return Whether the jtag user protection is enabled.
 */
extern bool flashcdw_is_jtag_user_protection_enabled(void);

/*! \brief Enable the jtag user protection feature (i.e. program (set to "0") the UPROT bit)
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_enable_jtag_user_protection(void);

/*! \brief Disable the jtag user protection feature (i.e. erase (set to "1") the UPROT bit)
 *
 * \warning A Lock Error is issued if the Security bit is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_disable_jtag_user_protection(void);

/*! \brief Tells whether the Secure State is enabled.
 *
 * \return Whether the Secure State is enabled.
 */
extern bool flashcdw_is_secure_state_enabled(void);

/*! \brief Tells whether the Secure State debug is enabled.
 *
 * \return Whether the Secure State debug is enabled.
 */
extern bool flashcdw_is_secure_state_debug_enabled(void);

/*! \brief Enable the Secure State with the Secure State debug disabled
 *
 * \warning A Lock Error is issued if the SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_enable_secure_state_no_debug(void);

/*! \brief Enable the Secure State with the Secure State debug enabled
 *
 * \warning A Lock Error is issued if the SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_enable_secure_state_with_debug(void);

/*! \brief Tells whether the region of a page is locked.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return Whether the region of the specified page is locked.
 */
extern bool flashcdw_is_page_region_locked(int page_number);

/*! \brief Tells whether a region is locked.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHCDW_REGIONS - 1)</tt>.
 *
 * \return Whether the specified region is locked.
 */
extern bool flashcdw_is_region_locked(unsigned int region);

/*! \brief Locks or unlocks the region of a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 * \param lock Whether to lock the region of the specified page: \c true or
 *             \c false.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_lock_page_region(int page_number, bool lock);

/*! \brief Locks or unlocks a region.
 *
 * \param region The region number: \c 0 to <tt>(AVR32_FLASHCDW_REGIONS - 1)</tt>.
 * \param lock Whether to lock the specified region: \c true or \c false.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_lock_region(unsigned int region, bool lock);

/*! \brief Locks or unlocks all regions.
 *
 * \param lock Whether to lock the regions: \c true or \c false.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_lock_all_regions(bool lock);

//! @}


/*! \name Access to General-Purpose Fuses
 */
//! @{

/*! \brief Reads a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 *
 * \return The value of the specified general-purpose fuse bit.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern bool flashcdw_read_gp_fuse_bit(unsigned int gp_fuse_bit);

/*! \brief Reads a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 *
 * \return The value of the specified general-purpose fuse bit-field.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern uint64_t flashcdw_read_gp_fuse_bitfield(unsigned int pos, unsigned int width);

/*! \brief Reads a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 *
 * \return The value of the specified general-purpose fuse byte.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern uint8_t flashcdw_read_gp_fuse_byte(unsigned int gp_fuse_byte);

/*! \brief Reads all general-purpose fuses.
 *
 * \return The value of all general-purpose fuses as a word.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern uint64_t flashcdw_read_all_gp_fuses(void);

/*! \brief Erases a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern bool flashcdw_erase_gp_fuse_bit(unsigned int gp_fuse_bit, bool check);

/*! \brief Erases a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern bool flashcdw_erase_gp_fuse_bitfield(unsigned int pos, unsigned int width, bool check);

/*! \brief Erases a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active and the SECURE bits are in the target fuse byte.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern bool flashcdw_erase_gp_fuse_byte(unsigned int gp_fuse_byte, bool check);

/*! \brief Erases all general-purpose fuses.
 *
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern bool flashcdw_erase_all_gp_fuses(bool check);

/*! \brief Writes a general-purpose fuse bit.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param value The value of the specified general-purpose fuse bit.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_write_gp_fuse_bit(unsigned int gp_fuse_bit, bool value);

/*! \brief Writes a general-purpose fuse bit-field.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param value The value of the specified general-purpose fuse bit-field.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_write_gp_fuse_bitfield(unsigned int pos, unsigned int width, uint64_t value);

/*! \brief Writes a general-purpose fuse byte.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param value The value of the specified general-purpose fuse byte.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active and the SECURE bits are in the target fuse byte.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_write_gp_fuse_byte(unsigned int gp_fuse_byte, uint8_t value);

/*! \brief Writes all general-purpose fuses.
 *
 * \param value The value of all general-purpose fuses as a word.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_write_all_gp_fuses(uint64_t value);

/*! \brief Sets a general-purpose fuse bit with the appropriate erase and write
 *         operations.
 *
 * \param gp_fuse_bit The general-purpose fuse bit: \c 0 to \c 63.
 * \param value The value of the specified general-purpose fuse bit.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_set_gp_fuse_bit(unsigned int gp_fuse_bit, bool value);

/*! \brief Sets a general-purpose fuse bit-field with the appropriate erase and
 *         write operations.
 *
 * \param pos The bit-position of the general-purpose fuse bit-field: \c 0 to
 *            \c 63.
 * \param width The bit-width of the general-purpose fuse bit-field: \c 0 to
 *              \c 64.
 * \param value The value of the specified general-purpose fuse bit-field.
 *
 * \warning A Lock Error is issued:
 *          - if the Security bit is active and the command is applied to BOOTPROT or EPFL or UPROT fuses,
 *          - if the SECURE mode is active and the command is applied to the SECURE bits.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_set_gp_fuse_bitfield(unsigned int pos, unsigned int width, uint64_t value);

/*! \brief Sets a general-purpose fuse byte with the appropriate erase and write
 *         operations.
 *
 * \param gp_fuse_byte The general-purpose fuse byte: \c 0 to \c 7.
 * \param value The value of the specified general-purpose fuse byte.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active and the SECURE bits are in the target fuse byte.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_set_gp_fuse_byte(unsigned int gp_fuse_byte, uint8_t value);

/*! \brief Sets all general-purpose fuses with the appropriate erase and write
 *         operations.
 *
 * \param value The value of all general-purpose fuses as a word.
 *
 * \warning A Lock Error is issued if the Security bit is active or if the
 *          SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note The actual number of general-purpose fuse bits implemented by hardware
 *       is given by \c AVR32_FLASHCDW_GPF_NUM. The other bits among the 64 are
 *       fixed at 1 by hardware.
 */
extern void flashcdw_set_all_gp_fuses(uint64_t value);

//! @}


/*! \name Access to Flash Pages
 */
//! @{

/*! \brief Clears the page buffer.
 *
 * This command resets all bits in the page buffer to one. Write accesses to the
 * page buffer can only change page buffer bits from one to zero.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern void flashcdw_clear_page_buffer(void);

/*! \brief Tells whether the page to which the last Quick Page Read or Quick
 *         Page Read User Page command was applied was erased.
 *
 * \return Whether the page to which the last Quick Page Read or Quick Page Read
 *         User Page command was applied was erased.
 */
extern bool flashcdw_is_page_erased(void);

/*! \brief Applies the Quick Page Read command to a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \return Whether the specified page is erased.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern bool flashcdw_quick_page_read(int page_number);

/*! \brief Erases a page.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if the command is applied to a page belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern bool flashcdw_erase_page(int page_number, bool check);

/*! \brief Erases all pages within the flash array.
 *
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \warning A Lock Error is issued if at least one region is locked or the
 *          bootloader protection is active or the SECURE mode is active.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern bool flashcdw_erase_all_pages(bool check);

/*! \brief Writes a page from the page buffer.
 *
 * \param page_number The page number:
 *   \arg \c 0 to <tt>(flashcdw_get_page_count() - 1)</tt>: a page number within
 *        the flash array;
 *   \arg <tt>< 0</tt>: the current page number.
 *
 * \warning A Lock Error is issued if the command is applied to a page belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 */
extern void flashcdw_write_page(int page_number);

/*! \brief Issues a Quick Page Read User Page command to the FLASHCDW.
 *
 * \return Whether the User page is erased.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern bool flashcdw_quick_user_page_read(void);

/*! \brief Erases the User page.
 *
 * \param check Whether to check erase: \c true or \c false.
 *
 * \return Whether the erase succeeded or always \c true if erase check was not
 *         requested.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note An erase operation can only set bits.
 */
extern bool flashcdw_erase_user_page(bool check);

/*! \brief Writes the User page from the page buffer.
 *
 * \warning The page buffer is not automatically reset after a page write.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 *
 * \note A write operation can only clear bits; in other words, an erase operation
 *  must first be done if some bits need to be set to 1.
 */
extern void flashcdw_write_user_page(void);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src source byte.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source byte.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          cannot be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern volatile void *flashcdw_memset8(volatile void *dst, uint8_t src, size_t nbytes, bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source half-word.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source half-word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern volatile void *flashcdw_memset16(volatile void *dst, uint16_t src, size_t nbytes, bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source word.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern volatile void *flashcdw_memset32(volatile void *dst, uint32_t src, size_t nbytes, bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source double-word.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source double-word.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern volatile void *flashcdw_memset64(volatile void *dst, uint64_t src, size_t nbytes, bool erase);

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the repeated \a src big-endian source pattern.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Source double-word.
 * \param src_width \a src width in bits: 8, 16, 32 or 64.
 * \param nbytes Number of bytes to set.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
#define flashcdw_memset(dst, src, src_width, nbytes, erase) \
	TPASTE2(flashcdw_memset, src_width)((dst), (src), (nbytes), (erase))

/*! \brief Copies \a nbytes bytes to the flash destination pointed to by \a dst
 *         from the source pointed to by \a src.
 *
 * The destination areas that are not within the flash
 * array or the User page are caught by an Assert() operation.
 *
 * All pointer and size alignments are supported.
 *
 * \param dst Pointer to flash destination.
 * \param src Pointer to source data.
 * \param nbytes Number of bytes to copy.
 * \param erase Whether to erase before writing: \c true or \c false.
 *
 * \return The value of \a dst.
 *
 * \warning If copying takes place between areas that overlap, the behavior is
 *          undefined.
 *
 * \warning This function may be called with \a erase set to \c false only if
 *          the destination consists only of erased words, i.e. this function
 *          can not be used to write only one bit of a previously written word.
 *          E.g., if \c 0x00000001 then \c 0xFFFFFFFE are written to a word, the
 *          resulting value in flash may be different from \c 0x00000000.
 *
 * \warning A Lock Error is issued if the command is applied to pages belonging
 *          to a locked region or to the bootloader protected area or to a secure
 *          area requiring secure privileges.
 *
 * \note The FLASHCDW error status returned by \ref flashcdw_is_lock_error and
 *       \ref flashcdw_is_programming_error is updated.
 */
extern volatile void *flashcdw_memcpy(volatile void *dst, const void *src, size_t nbytes, bool erase);

//! @}

/**
 * \}
 */

#endif  // _FLASHCDW_H_
