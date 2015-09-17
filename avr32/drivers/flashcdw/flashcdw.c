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


#include <avr32/io.h>
#include <stddef.h>
#include "compiler.h"
#include "flashcdw.h"


/*! \name FLASHCDW Writable Bit-Field Registers
 */
//! @{

typedef union
{
	unsigned long                   fcr;
	avr32_flashcdw_fcr_t            FCR;
} u_avr32_flashcdw_fcr_t;

typedef union
{
	unsigned long                   fcmd;
	avr32_flashcdw_fcmd_t           FCMD;
} u_avr32_flashcdw_fcmd_t;

//! @}


/*! \name Flash Properties
 */
//! @{


unsigned int flashcdw_get_flash_size(void)
{
	const uint16_t FLASH_SIZE[1 << AVR32_FLASHCDW_FPR_FSZ_SIZE] = {
		4,
		8,
		16,
		32,
		48,
		64,
		96,
		128,
		192,
		256,
		384,
		512,
		768,
		1024,
		2048,
	};
	return ((unsigned int)FLASH_SIZE[(AVR32_FLASHCDW.fpr & AVR32_FLASHCDW_FPR_FSZ_MASK
			) >> AVR32_FLASHCDW_FPR_FSZ_OFFSET])<<10;
}


unsigned int flashcdw_get_page_count(void)
{
	return flashcdw_get_flash_size() / AVR32_FLASHCDW_PAGE_SIZE;
}


unsigned int flashcdw_get_page_count_per_region(void)
{
	return flashcdw_get_page_count() / AVR32_FLASHCDW_REGIONS;
}


unsigned int flashcdw_get_page_region(int page_number)
{
	return ((page_number >= 0) ? page_number
			: flashcdw_get_page_number()) / flashcdw_get_page_count_per_region();
}


unsigned int flashcdw_get_region_first_page_number(unsigned int region)
{
	return region * flashcdw_get_page_count_per_region();
}


//! @}


/*! \name FLASHC Control
 */
//! @{


unsigned int flashcdw_get_wait_state(void)
{
	return (AVR32_FLASHCDW.fcr & AVR32_FLASHCDW_FCR_FWS_MASK) >> AVR32_FLASHCDW_FCR_FWS_OFFSET;
}


void flashcdw_set_wait_state(unsigned int wait_state)
{
	u_avr32_flashcdw_fcr_t u_avr32_flashcdw_fcr = {AVR32_FLASHCDW.fcr};
	u_avr32_flashcdw_fcr.FCR.fws = wait_state;
	AVR32_FLASHCDW.fcr = u_avr32_flashcdw_fcr.fcr;
}


void flashcdw_set_flash_waitstate_and_readmode(unsigned long cpu_f_hz)
{
	if (cpu_f_hz > AVR32_FLASHCDW_FWS_0_MAX_FREQ) { // > 15MHz
		if (cpu_f_hz <= AVR32_FLASHCDW_FWS_1_MAX_FREQ) { // <= 30MHz
			// Set a wait-state, disable the high-speed read mode.
			flashcdw_set_wait_state(1);
			flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_HSDIS, -1);
		} else {
			// Set a wait-state, enable the high-speed read mode.
			flashcdw_set_wait_state(1);
			flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_HSEN, -1);
		}
	} else { // <= 15MHz
		// No wait-state, disable the high-speed read mode
		flashcdw_set_wait_state(0);
		flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_HSDIS, -1);
	}
}


bool flashcdw_is_ready_int_enabled(void)
{
	return ((AVR32_FLASHCDW.fcr & AVR32_FLASHCDW_FCR_FRDY_MASK) != 0);
}


void flashcdw_enable_ready_int(bool enable)
{
	u_avr32_flashcdw_fcr_t u_avr32_flashcdw_fcr = {AVR32_FLASHCDW.fcr};
	u_avr32_flashcdw_fcr.FCR.frdy = (enable != false);
	AVR32_FLASHCDW.fcr = u_avr32_flashcdw_fcr.fcr;
}


bool flashcdw_is_lock_error_int_enabled(void)
{
	return ((AVR32_FLASHCDW.fcr & AVR32_FLASHCDW_FCR_LOCKE_MASK) != 0);
}


void flashcdw_enable_lock_error_int(bool enable)
{
	u_avr32_flashcdw_fcr_t u_avr32_flashcdw_fcr = {AVR32_FLASHCDW.fcr};
	u_avr32_flashcdw_fcr.FCR.locke = (enable != false);
	AVR32_FLASHCDW.fcr = u_avr32_flashcdw_fcr.fcr;
}


bool flashcdw_is_prog_error_int_enabled(void)
{
	return ((AVR32_FLASHCDW.fcr & AVR32_FLASHCDW_FCR_PROGE_MASK) != 0);
}


void flashcdw_enable_prog_error_int(bool enable)
{
	u_avr32_flashcdw_fcr_t u_avr32_flashcdw_fcr = {AVR32_FLASHCDW.fcr};
	u_avr32_flashcdw_fcr.FCR.proge = (enable != false);
	AVR32_FLASHCDW.fcr = u_avr32_flashcdw_fcr.fcr;
}


//! @}


/*! \name FLASHC Status
 */
//! @{


bool flashcdw_is_ready(void)
{
	return ((AVR32_FLASHCDW.fsr & AVR32_FLASHCDW_FSR_FRDY_MASK) != 0);
}


void flashcdw_default_wait_until_ready(void)
{
	while (!flashcdw_is_ready());
}


void (*volatile flashcdw_wait_until_ready)(void) = flashcdw_default_wait_until_ready;


/*! \brief Gets the error status of the FLASHCDW.
 *
 * \return The error status of the FLASHCDW built up from
 *         \c AVR32_FLASHCDW_FSR_LOCKE_MASK and \c AVR32_FLASHCDW_FSR_PROGE_MASK.
 *
 * \warning This hardware error status is cleared by all functions reading the
 *          Flash Status Register (FSR). This function is therefore not part of
 *          the driver's API which instead presents \ref flashcdw_is_lock_error
 *          and \ref flashcdw_is_programming_error.
 */
static unsigned int flashcdw_get_error_status(void)
{
	return AVR32_FLASHCDW.fsr & (AVR32_FLASHCDW_FSR_LOCKE_MASK |
			AVR32_FLASHCDW_FSR_PROGE_MASK);
}


//! Sticky error status of the FLASHCDW.
//! This variable is updated by functions that issue FLASHCDW commands. It
//! contains the cumulated FLASHCDW error status of all the FLASHCDW commands issued
//! by a function.
static unsigned int flashcdw_error_status = 0;


bool flashcdw_is_lock_error(void)
{
	return ((flashcdw_error_status & AVR32_FLASHCDW_FSR_LOCKE_MASK) != 0);
}


bool flashcdw_is_programming_error(void)
{
	return ((flashcdw_error_status & AVR32_FLASHCDW_FSR_PROGE_MASK) != 0);
}


bool flashcdw_is_high_speed_enabled(void)
{
	return ((AVR32_FLASHCDW.fsr & AVR32_FLASHCDW_FSR_HSMODE_MASK) != 0);
}


//! @}


/*! \name FLASHCDW Command Control
 */
//! @{


unsigned int flashcdw_get_command(void)
{
	return (AVR32_FLASHCDW.fcmd & AVR32_FLASHCDW_FCMD_CMD_MASK) >> AVR32_FLASHCDW_FCMD_CMD_OFFSET;
}


unsigned int flashcdw_get_page_number(void)
{
	return (AVR32_FLASHCDW.fcmd & AVR32_FLASHCDW_FCMD_PAGEN_MASK) >> AVR32_FLASHCDW_FCMD_PAGEN_OFFSET;
}


void flashcdw_issue_command(unsigned int command, int page_number)
{
	u_avr32_flashcdw_fcmd_t u_avr32_flashcdw_fcmd;

	flashcdw_wait_until_ready();
	u_avr32_flashcdw_fcmd.fcmd = AVR32_FLASHCDW.fcmd;
	u_avr32_flashcdw_fcmd.FCMD.cmd = command;
	if (page_number >= 0) {
		u_avr32_flashcdw_fcmd.FCMD.pagen = page_number;
	}
	u_avr32_flashcdw_fcmd.FCMD.key = AVR32_FLASHCDW_FCMD_KEY_KEY;
	AVR32_FLASHCDW.fcmd = u_avr32_flashcdw_fcmd.fcmd;
	flashcdw_error_status = flashcdw_get_error_status();
	flashcdw_wait_until_ready();
}


//! @}


/*! \name FLASHCDW Global Commands
 */
//! @{


void flashcdw_no_operation(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_NOP, -1);
}


void flashcdw_erase_all(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_EA, -1);
}


//! @}


/*! \name FLASHCDW Protection Mechanisms
 */
//! @{


bool flashcdw_is_security_bit_active(void)
{
	return ((AVR32_FLASHCDW.fsr & AVR32_FLASHCDW_FSR_SECURITY_MASK) != 0);
}


void flashcdw_activate_security_bit(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_SSB, -1);
}


unsigned int flashcdw_get_bootloader_protected_size(void)
{
	unsigned int bootprot = (1 << AVR32_FLASHCDW_FGPFRLO_BOOTPROT_SIZE) - 1 -
			flashcdw_read_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_BOOTPROT_OFFSET,
			AVR32_FLASHCDW_FGPFRLO_BOOTPROT_SIZE);
	return (bootprot) ? AVR32_FLASHCDW_PAGE_SIZE << bootprot : 0;
}


unsigned int flashcdw_set_bootloader_protected_size(unsigned int bootprot_size)
{
	flashcdw_set_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_BOOTPROT_OFFSET,
			AVR32_FLASHCDW_FGPFRLO_BOOTPROT_SIZE,
			(1 << AVR32_FLASHCDW_FGPFRLO_BOOTPROT_SIZE) - 1 -
			((bootprot_size) ?
			32 - clz((((min(max(bootprot_size, AVR32_FLASHCDW_PAGE_SIZE << 1),
			AVR32_FLASHCDW_PAGE_SIZE <<
			((1 << AVR32_FLASHCDW_FGPFRLO_BOOTPROT_SIZE) - 1)) +
			AVR32_FLASHCDW_PAGE_SIZE - 1) /
			AVR32_FLASHCDW_PAGE_SIZE) << 1) - 1) - 1 :
			0));
	return flashcdw_get_bootloader_protected_size();
}


bool flashcdw_is_external_privileged_fetch_locked(void)
{
	return (!flashcdw_read_gp_fuse_bit(AVR32_FLASHCDW_FGPFRLO_EPFL_OFFSET));
}


void flashcdw_lock_external_privileged_fetch(bool lock)
{
	flashcdw_set_gp_fuse_bit(AVR32_FLASHCDW_FGPFRLO_EPFL_OFFSET, !lock);
}

bool flashcdw_is_jtag_user_protection_enabled(void)
{
	return (!flashcdw_read_gp_fuse_bit(AVR32_FLASHCDW_FGPFRLO_UPROT_OFFSET));
}

void flashcdw_enable_jtag_user_protection(void)
{
	flashcdw_set_gp_fuse_bit(AVR32_FLASHCDW_FGPFRLO_UPROT_OFFSET, false);
}

void flashcdw_disable_jtag_user_protection(void)
{
	flashcdw_set_gp_fuse_bit(AVR32_FLASHCDW_FGPFRLO_UPROT_OFFSET, true);
}

bool flashcdw_is_secure_state_enabled(void)
{
	uint64_t temp = flashcdw_read_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_SECURE_OFFSET,
		AVR32_FLASHCDW_FGPFRLO_SECURE_SIZE);

	if ((AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDDIS == temp)
		|| (AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDEN == temp)) {
		return true;
	} else {
		return false;
	}
}

bool flashcdw_is_secure_state_debug_enabled(void)
{
	return( AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDEN
			== flashcdw_read_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_SECURE_OFFSET,
			AVR32_FLASHCDW_FGPFRLO_SECURE_SIZE) );
}

void flashcdw_enable_secure_state_no_debug(void)
{
	flashcdw_write_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_SECURE_OFFSET,
			AVR32_FLASHCDW_FGPFRLO_SECURE_SIZE,
			AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDDIS);
}

void flashcdw_enable_secure_state_with_debug(void)
{
	flashcdw_write_gp_fuse_bitfield(AVR32_FLASHCDW_FGPFRLO_SECURE_OFFSET,
			AVR32_FLASHCDW_FGPFRLO_SECURE_SIZE,
			AVR32_FLASHCDW_FGPFRLO_SECURE_SSEN_SSDEN);
}

bool flashcdw_is_page_region_locked(int page_number)
{
	return flashcdw_is_region_locked(flashcdw_get_page_region(page_number));
}


bool flashcdw_is_region_locked(unsigned int region)
{
	return ((AVR32_FLASHCDW.fsr & AVR32_FLASHCDW_FSR_LOCK0_MASK
			<< (region & (AVR32_FLASHCDW_REGIONS - 1))) != 0);
}


void flashcdw_lock_page_region(int page_number, bool lock)
{
	flashcdw_issue_command(
			(lock) ? AVR32_FLASHCDW_FCMD_CMD_LP:AVR32_FLASHCDW_FCMD_CMD_UP,
			page_number);
}


void flashcdw_lock_region(unsigned int region, bool lock)
{
	flashcdw_lock_page_region(flashcdw_get_region_first_page_number(region), lock);
}


void flashcdw_lock_all_regions(bool lock)
{
	unsigned int error_status = 0;
	unsigned int region = AVR32_FLASHCDW_REGIONS;

	while (region) {
		flashcdw_lock_region(--region, lock);
		error_status |= flashcdw_error_status;
	}
	flashcdw_error_status = error_status;
}


//! @}


/*! \name Access to General-Purpose Fuses
 */
//! @{


bool flashcdw_read_gp_fuse_bit(unsigned int gp_fuse_bit)
{
	return ((flashcdw_read_all_gp_fuses() & 1ULL << (gp_fuse_bit & 0x3F)) != 0);
}


uint64_t flashcdw_read_gp_fuse_bitfield(unsigned int pos, unsigned int width)
{
	return flashcdw_read_all_gp_fuses() >> (pos & 0x3F) & ((1ULL << min(width, 64)) - 1);
}


uint8_t flashcdw_read_gp_fuse_byte(unsigned int gp_fuse_byte)
{
	return flashcdw_read_all_gp_fuses() >> ((gp_fuse_byte & 0x07) << 3);
}


uint64_t flashcdw_read_all_gp_fuses(void)
{
	return AVR32_FLASHCDW.fgpfrlo | (uint64_t)AVR32_FLASHCDW.fgpfrhi << 32;
}


bool flashcdw_erase_gp_fuse_bit(unsigned int gp_fuse_bit, bool check)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_EGPB, gp_fuse_bit & 0x3F);
	return (check) ? flashcdw_read_gp_fuse_bit(gp_fuse_bit) : true;
}


bool flashcdw_erase_gp_fuse_bitfield(unsigned int pos, unsigned int width, bool check)
{
	unsigned int error_status = 0;
	unsigned int gp_fuse_bit;

	pos &= 0x3F;
	width = min(width, 64);

	for (gp_fuse_bit = pos; gp_fuse_bit < pos + width; gp_fuse_bit++) {
		flashcdw_erase_gp_fuse_bit(gp_fuse_bit, false);
		error_status |= flashcdw_error_status;
	}
	flashcdw_error_status = error_status;
	return (check) ? (flashcdw_read_gp_fuse_bitfield(pos, width) == (1ULL << width) - 1) : true;
}


bool flashcdw_erase_gp_fuse_byte(unsigned int gp_fuse_byte, bool check)
{
	unsigned int error_status;
	unsigned int current_gp_fuse_byte;
	uint64_t value = flashcdw_read_all_gp_fuses();

	flashcdw_erase_all_gp_fuses(false);
	error_status = flashcdw_error_status;
	for (current_gp_fuse_byte = 0; current_gp_fuse_byte < 8; current_gp_fuse_byte++, value >>= 8) {
		if (current_gp_fuse_byte != gp_fuse_byte) {
			flashcdw_write_gp_fuse_byte(current_gp_fuse_byte, value);
			error_status |= flashcdw_error_status;
		}
	}
	flashcdw_error_status = error_status;
	return (check) ? (flashcdw_read_gp_fuse_byte(gp_fuse_byte) == 0xFF) : true;
}


bool flashcdw_erase_all_gp_fuses(bool check)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_EAGPF, -1);
	return (check) ? (flashcdw_read_all_gp_fuses() == 0xFFFFFFFFFFFFFFFFULL) : true;
}


void flashcdw_write_gp_fuse_bit(unsigned int gp_fuse_bit, bool value)
{
	if (!value) {
		flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_WGPB, gp_fuse_bit & 0x3F);
	}
}


void flashcdw_write_gp_fuse_bitfield(unsigned int pos, unsigned int width, uint64_t value)
{
	unsigned int error_status = 0;
	unsigned int gp_fuse_bit;

	pos &= 0x3F;
	width = min(width, 64);

	for (gp_fuse_bit = pos; gp_fuse_bit < pos + width; gp_fuse_bit++, value >>= 1) {
		flashcdw_write_gp_fuse_bit(gp_fuse_bit, value & 0x01);
		error_status |= flashcdw_error_status;
	}
	flashcdw_error_status = error_status;
}


void flashcdw_write_gp_fuse_byte(unsigned int gp_fuse_byte, uint8_t value)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_PGPFB, (gp_fuse_byte & 0x07) | value << 3);
}


void flashcdw_write_all_gp_fuses(uint64_t value)
{
	unsigned int error_status = 0;
	unsigned int gp_fuse_byte;

	for (gp_fuse_byte = 0; gp_fuse_byte < 8; gp_fuse_byte++, value >>= 8) {
		flashcdw_write_gp_fuse_byte(gp_fuse_byte, value);
		error_status |= flashcdw_error_status;
	}
	flashcdw_error_status = error_status;
}


void flashcdw_set_gp_fuse_bit(unsigned int gp_fuse_bit, bool value)
{
	if (value) {
		flashcdw_erase_gp_fuse_bit(gp_fuse_bit, false);
	} else {
		flashcdw_write_gp_fuse_bit(gp_fuse_bit, false);
	}
}


void flashcdw_set_gp_fuse_bitfield(unsigned int pos, unsigned int width, uint64_t value)
{
	unsigned int error_status = 0;
	unsigned int gp_fuse_bit;

	pos &= 0x3F;
	width = min(width, 64);

	for (gp_fuse_bit = pos; gp_fuse_bit < pos + width; gp_fuse_bit++, value >>= 1) {
		flashcdw_set_gp_fuse_bit(gp_fuse_bit, value & 0x01);
		error_status |= flashcdw_error_status;
	}
	flashcdw_error_status = error_status;
}


void flashcdw_set_gp_fuse_byte(unsigned int gp_fuse_byte, uint8_t value)
{
	unsigned int error_status;

	switch (value)
	{
	case 0xFF:
		flashcdw_erase_gp_fuse_byte(gp_fuse_byte, false);
		break;

	case 0x00:
		flashcdw_write_gp_fuse_byte(gp_fuse_byte, 0x00);
		break;

	default:
		flashcdw_erase_gp_fuse_byte(gp_fuse_byte, false);
		error_status = flashcdw_error_status;
		flashcdw_write_gp_fuse_byte(gp_fuse_byte, value);
		flashcdw_error_status |= error_status;
		break;
	}
}


void flashcdw_set_all_gp_fuses(uint64_t value)
{
	unsigned int error_status;

	switch (value)
	{
	case 0xFFFFFFFFFFFFFFFFULL:
		flashcdw_erase_all_gp_fuses(false);
		break;

	case 0x0000000000000000ULL:
		flashcdw_write_all_gp_fuses(0x0000000000000000ULL);
		break;

	default:
		flashcdw_erase_all_gp_fuses(false);
		error_status = flashcdw_error_status;
		flashcdw_write_all_gp_fuses(value);
		flashcdw_error_status |= error_status;
		break;
	}
}


//! @}


/*! \name Access to Flash Pages
 */
//! @{


void flashcdw_clear_page_buffer(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_CPB, -1);
}


bool flashcdw_is_page_erased(void)
{
	return ((AVR32_FLASHCDW.fsr & AVR32_FLASHCDW_FSR_QPRR_MASK) != 0);
}


bool flashcdw_quick_page_read(int page_number)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_QPR, page_number);
	return flashcdw_is_page_erased();
}


bool flashcdw_erase_page(int page_number, bool check)
{
	bool page_erased = true;

	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_EP, page_number);

	if (check) {
		unsigned int error_status = flashcdw_error_status;
		page_erased = flashcdw_quick_page_read(-1);
		flashcdw_error_status |= error_status;
	}
	return page_erased;
}


bool flashcdw_erase_all_pages(bool check)
{
	bool all_pages_erased = true;
	unsigned int error_status = 0;
	unsigned int page_number = flashcdw_get_page_count();

	while (page_number) {
		all_pages_erased &= flashcdw_erase_page(--page_number, check);
		error_status |= flashcdw_error_status;
	}

	flashcdw_error_status = error_status;
	return all_pages_erased;
}


void flashcdw_write_page(int page_number)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_WP, page_number);
}


bool flashcdw_quick_user_page_read(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_QPRUP, -1);
	return flashcdw_is_page_erased();
}


bool flashcdw_erase_user_page(bool check)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_EUP, -1);
	return (check) ? flashcdw_quick_user_page_read() : true;
}


void flashcdw_write_user_page(void)
{
	flashcdw_issue_command(AVR32_FLASHCDW_FCMD_CMD_WUP, -1);
}


volatile void *flashcdw_memset8(volatile void *dst, uint8_t src, size_t nbytes, bool erase)
{
	return flashcdw_memset16(dst, src | (uint16_t)src << 8, nbytes, erase);
}


volatile void *flashcdw_memset16(volatile void *dst, uint16_t src, size_t nbytes, bool erase)
{
	return flashcdw_memset32(dst, src | (uint32_t)src << 16, nbytes, erase);
}


volatile void *flashcdw_memset32(volatile void *dst, uint32_t src, size_t nbytes, bool erase)
{
	return flashcdw_memset64(dst, src | (uint64_t)src << 32, nbytes, erase);
}


volatile void *flashcdw_memset64(volatile void *dst, uint64_t src, size_t nbytes, bool erase)
{
	// Use aggregated pointers to have several alignments available for a same address.
	UnionCVPtr flash_array_end;
	UnionVPtr dest;
	Union64 source = {0};
	StructCVPtr dest_end;
	UnionCVPtr flash_page_source_end;
	bool incomplete_flash_page_end;
	Union64 flash_dword;
	UnionVPtr tmp;
	unsigned int error_status = 0;
	unsigned int i;

	// Reformat arguments.
	flash_array_end.u8ptr = AVR32_FLASH + flashcdw_get_flash_size();
	dest.u8ptr = dst;
	for (i = (Get_align((uint32_t)dest.u8ptr, sizeof(uint64_t)) - 1) & (sizeof(uint64_t) - 1);
			src; i = (i - 1) & (sizeof(uint64_t) - 1)) {
		source.u8[i] = src;
		src >>= 8;
	}
	dest_end.u8ptr = dest.u8ptr + nbytes;

	// If destination is outside flash, go to next flash page if any.
	if (dest.u8ptr < AVR32_FLASH) {
		dest.u8ptr = AVR32_FLASH;
	} else if (flash_array_end.u8ptr <= dest.u8ptr && dest.u8ptr < AVR32_FLASHCDW_USER_PAGE) {
		dest.u8ptr = AVR32_FLASHCDW_USER_PAGE;
	}

	// If end of destination is outside flash, move it to the end of the previous flash page if any.
	if (dest_end.u8ptr > AVR32_FLASHCDW_USER_PAGE + AVR32_FLASHCDW_USER_PAGE_SIZE) {
		dest_end.u8ptr = AVR32_FLASHCDW_USER_PAGE + AVR32_FLASHCDW_USER_PAGE_SIZE;
	} else if (AVR32_FLASHCDW_USER_PAGE >= dest_end.u8ptr && dest_end.u8ptr > flash_array_end.u8ptr) {
		dest_end.u8ptr = flash_array_end.u8ptr;
	}

	// Align each end of destination pointer with its natural boundary.
	dest_end.u16ptr = (uint16_t *)Align_down((uint32_t)dest_end.u8ptr, sizeof(uint16_t));
	dest_end.u32ptr = (uint32_t *)Align_down((uint32_t)dest_end.u16ptr, sizeof(uint32_t));
	dest_end.u64ptr = (uint64_t *)Align_down((uint32_t)dest_end.u32ptr, sizeof(uint64_t));

	// While end of destination is not reached...
	while (dest.u8ptr < dest_end.u8ptr) {
		// Clear the page buffer in order to prepare data for a flash page write.
		flashcdw_clear_page_buffer();
		error_status |= flashcdw_error_status;

		// Determine where the source data will end in the current flash page.
		flash_page_source_end.u64ptr = (uint64_t *)min((uint32_t)dest_end.u64ptr,
				Align_down((uint32_t)dest.u8ptr, AVR32_FLASHCDW_PAGE_SIZE) + AVR32_FLASHCDW_PAGE_SIZE);

		// Determine if the current destination page has an incomplete end.
		incomplete_flash_page_end = (Align_down((uint32_t)dest.u8ptr, AVR32_FLASHCDW_PAGE_SIZE) >=
				Align_down((uint32_t)dest_end.u8ptr, AVR32_FLASHCDW_PAGE_SIZE));

		// Use a flash double-word buffer to manage unaligned accesses.
		flash_dword.u64 = source.u64;

		// If destination does not point to the beginning of the current flash page...
		if (!Test_align((uint32_t)dest.u8ptr, AVR32_FLASHCDW_PAGE_SIZE)) {
			// Fill the beginning of the page buffer with the current flash page data.
			// This is required by the hardware, even if page erase is not requested,
			// in order to be able to write successfully to erased parts of flash
			// pages that have already been written to.
			for (tmp.u8ptr = (uint8_t *)Align_down((uint32_t)dest.u8ptr, AVR32_FLASHCDW_PAGE_SIZE);
					tmp.u64ptr < (uint64_t *)Align_down((uint32_t)dest.u8ptr, sizeof(uint64_t));
					tmp.u64ptr++) {
				*tmp.u32ptr = *tmp.u32ptr;
				*(tmp.u32ptr+1) = *(tmp.u32ptr+1);
			}

			// If destination is not 64-bit aligned...
			if (!Test_align((uint32_t)dest.u8ptr, sizeof(uint64_t))) {
				// Fill the beginning of the flash double-word buffer with the current
				// flash page data.
				// This is required by the hardware, even if page erase is not
				// requested, in order to be able to write successfully to erased parts
				// of flash pages that have already been written to.
				for (i = 0; i < Get_align((uint32_t)dest.u8ptr, sizeof(uint64_t)); i++) {
					flash_dword.u8[i] = *tmp.u8ptr++;
				}

				// Align the destination pointer with its 64-bit boundary.
				dest.u64ptr = (uint64_t *)Align_down((uint32_t)dest.u8ptr, sizeof(uint64_t));

				// If the current destination double-word is not the last one...
				if (dest.u64ptr < dest_end.u64ptr) {
					// Write the flash double-word buffer to the page buffer and reinitialize it.
					*dest.u32ptr++ = flash_dword.u32[0];
					*dest.u32ptr++ = flash_dword.u32[1];
					flash_dword.u64 = source.u64;
				}
			}
		}

		// Write the source data to the page buffer with 64-bit alignment.
		for (i = flash_page_source_end.u64ptr - dest.u64ptr; i; i--) {
			*dest.u32ptr++ = source.u32[0];
			*dest.u32ptr++ = source.u32[1];
		}

		// If the current destination page has an incomplete end...
		if (incomplete_flash_page_end) {
			// This is required by the hardware, even if page erase is not requested,
			// in order to be able to write successfully to erased parts of flash
			// pages that have already been written to.
			{
				tmp.u8ptr = (volatile uint8_t *)dest_end.u8ptr;

				// If end of destination is not 64-bit aligned...
				if (!Test_align((uint32_t)dest_end.u8ptr, sizeof(uint64_t))) {
					// Fill the end of the flash double-word buffer with the current flash page data.
					for (i = Get_align((uint32_t)dest_end.u8ptr, sizeof(uint64_t)); i < sizeof(uint64_t); i++) {
						flash_dword.u8[i] = *tmp.u8ptr++;
					}

					// Write the flash double-word buffer to the page buffer.
					*dest.u32ptr++ = flash_dword.u32[0];
					*dest.u32ptr++ = flash_dword.u32[1];
				}

				// Fill the end of the page buffer with the current flash page data.
				for (; !Test_align((uint32_t)tmp.u64ptr, AVR32_FLASHCDW_PAGE_SIZE); tmp.u64ptr++) {
					*tmp.u32ptr = *tmp.u32ptr;
					*(tmp.u32ptr+1) = *(tmp.u32ptr+1);
				}
			}
		}

		// If the current flash page is in the flash array...
		if (dest.u8ptr <= AVR32_FLASHCDW_USER_PAGE) {
			// Erase the current page if requested and write it from the page buffer.
			if (erase) {
				flashcdw_erase_page(-1, false);
				error_status |= flashcdw_error_status;
			}
			flashcdw_write_page(-1);
			error_status |= flashcdw_error_status;

			// If the end of the flash array is reached, go to the User page.
			if (dest.u8ptr >= flash_array_end.u8ptr) {
				dest.u8ptr = AVR32_FLASHCDW_USER_PAGE;
			}
		} else {
			// Erase the User page if requested and write it from the page buffer.
			if (erase) {
				flashcdw_erase_user_page(false);
				error_status |= flashcdw_error_status;
			}
			flashcdw_write_user_page();
			error_status |= flashcdw_error_status;
		}
	}

	// Update the FLASHC error status.
	flashcdw_error_status = error_status;

	// Return the initial destination pointer as the standard memset function does.
	return dst;
}


volatile void *flashcdw_memcpy(volatile void *dst, const void *src, size_t nbytes, bool erase)
{
	uint16_t page_pos;
	Union64 flash_dword;
	uint8_t i;
	bool b_user_page;
	unsigned int error_status = 0;
	uint8_t* flash_add;
	uint8_t* dest_add=(uint8_t*)dst;
	const uint8_t* src_buf=(const uint8_t*)src;

	// Copy area must be in flash array or flash user page
	Assert( (((uint8_t *)dst >= AVR32_FLASH)
			&& (((uint8_t *)dst + nbytes) <= (AVR32_FLASH + flashcdw_get_flash_size())))
			|| (((uint8_t *)dst >= AVR32_FLASHCDW_USER_PAGE)
			&& (((uint8_t *)dst + nbytes) <= (AVR32_FLASHCDW_USER_PAGE + AVR32_FLASHCDW_USER_PAGE_SIZE))) );

	b_user_page = (volatile uint8_t *)dst >= AVR32_FLASHCDW_USER_PAGE;

	flash_add = (uint8_t*)((uint32_t)dest_add - ((uint32_t)dest_add%AVR32_FLASHCDW_PAGE_SIZE));

	while ( nbytes ) {
		// Clear the page buffer in order to prepare data for a flash page write.
		flashcdw_clear_page_buffer();
		error_status |= flashcdw_error_status;

		// Loop in the page
		for (page_pos = 0; page_pos < AVR32_FLASHCDW_PAGE_SIZE; page_pos += sizeof(uint64_t)) {
			// Read the flash double-word buffer
			flash_dword.u64 = *(volatile uint64_t*)flash_add;

			// Update double-word if necessary
			for (i = 0; i < sizeof(uint64_t); i++) {
				if ( nbytes && (flash_add == dest_add)) {
					// Update page with data source
					flash_dword.u8[i] = *src_buf++;
					dest_add++;
					nbytes--;
				}
				flash_add++;
			}

			// Write the flash double-word buffer to the page buffer.
			*(volatile uint64_t*)((uint32_t)flash_add - sizeof(uint64_t))= flash_dword.u64;
		}

		// Erase the current page if requested and write it from the page buffer.
		if (erase) {
			(b_user_page)? flashcdw_erase_user_page(false) : flashcdw_erase_page(-1, false);
			error_status |= flashcdw_error_status;
		}

		// Write the page
		(b_user_page)? flashcdw_write_user_page() : flashcdw_write_page(-1);
		error_status |= flashcdw_error_status;
	}
	// Update the FLASHC error status.
	flashcdw_error_status = error_status;

	// Return the initial destination pointer as the standard memcpy function does.
	return dst;
}

//! @}
