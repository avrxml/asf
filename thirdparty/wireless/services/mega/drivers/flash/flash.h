/**
 * \file flash.h
 *
 * \brief MEGA FLASH DRIVER
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

/* Prevent double inclusion */
#ifndef FLASH_H
#define FLASH_H

/* Flash Commands*/

#if defined(__ICCAVR__)

#define PGM_READ_BYTE_FAR(x) __extended_load_program_memory((const unsigned char \
		__farflash *)x)

#define SPM_BUSY_WAIT() do {} \
	while ((SPMCSR & (uint8_t)(1 << (SPMEN))))
#define ENABLE_RWW_SECTION() __DataToR0ByteToSPMCR_SPM(0x00, 0x11)
#define EEPROM_BUSY_WAIT() do {} \
	while (EECR & (1 << EEPE))

/* Flash write abstraction */
#define FLASH_ERASE(addr)  _SPM_24_ERASE(addr)
#define FLASH_PAGE_FILL(addr, data)  _SPM_24_FILLTEMP(addr, data)
#define FLASH_PAGE_WRITE(addr)  _SPM_24_PAGEWRITE(addr)

#endif /* #if defined(__ICCAVR__) */

#if defined(__GNUC__)

#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define PGM_READ_BYTE_FAR(x) pgm_read_byte_far(x)

#define SPM_BUSY_WAIT() boot_spm_busy_wait()
#define ENABLE_RWW_SECTION() boot_rww_enable()
#define EEPROM_BUSY_WAIT() eeprom_busy_wait()
/* Flash write abstraction */
#define FLASH_ERASE(addr)  boot_page_erase(addr)
#define FLASH_PAGE_FILL(addr, data)  boot_page_fill(addr, data)
#define FLASH_PAGE_WRITE(addr)  boot_page_write(addr)

#endif /* #if defined(__GNUC__) */

/**
 * @brief Writes data to flash location : Performs all activities like filling
 * the page buffer,erasing the
 * flash page and pushing the data to the appropriate page address
 *
 * @param flash_addr    Flash address to be written
 * @param length        Number of bytes to write
 * @param data          Pointer to data location
 *
 */
void flash_write(uint32_t, uint32_t, uint8_t *);

/**
 *  @brief Reads data from the flash memory
 * @param read_addr Address in the Flash to be read
 * @param len           Number of bytes to be read
 * @param ret_buf	Pointer to the  location of the read data
 */
void flash_read(uint32_t, uint32_t, uint8_t *);

/**
 * @brief Erases a page
 * @param page_number The page that has to be erased
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_erase_page(uint32_t);
#endif
#ifdef __GNUC__
void flash_erase_page(uint32_t )
__attribute__((section(".bootloader"), noinline));
#endif

/**
 * @brief Fills the temporary page buffer
 *
 * This function fills the temporay flash page buffer.
 *
 * @param flash_addr    address of flash to be written
 * @param length        Length of provide data buffer
 * @param data          Pointer to data buffer
 *
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_fill_page_buffer(uint32_t, uint16_t, uint8_t * );
#endif
#ifdef __GNUC__
void flash_fill_page_buffer(uint32_t, uint16_t, uint8_t *)
__attribute__((section(".bootloader"), noinline));
#endif

/**
 * @brief Erases and Writes the actual flash page
 *
 * This function writes the data from the temporary flash page buffer to the
 * actual flash page.
 *
 * @param page_start_addr  Start address of the flash page
 *
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_program_page(uint32_t );
#endif
#ifdef __GNUC__
void flash_program_page(uint32_t )
__attribute__((section(".bootloader"), noinline));
#endif

/** Page size */
#if     AVR8_PART_IS_DEFINED(ATmega128RFA1) || \
	AVR8_PART_IS_DEFINED(ATmega256RFR2) || \
	AVR8_PART_IS_DEFINED(ATmega2564RFR2)

#ifdef __ICCAVR__
# ifndef SPM_PAGESIZE
#  define SPM_PAGESIZE  256
# endif
#endif
# ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE      (SPM_PAGESIZE)
#endif
#define FLASH_SIZE                       (FLASHEND + 1)

#endif

#endif  /* #ifndef FLASH_H */
