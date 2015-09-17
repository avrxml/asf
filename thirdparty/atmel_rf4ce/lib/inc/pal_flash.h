/**
 * @file pal_flash.h
 *
 * @brief Flash writting support functionality
 *
 * $Id: pal_flash.h 26812 2011-05-23 08:27:09Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

#if ((defined FLASH_SUPPORT) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef PAL_FLASH_H
#define PAL_FLASH_H


/**
 * @brief Writes data to flash location
 *
 * @param start_offset  Offset of start address
 * @param length        Number of bytes to write
 * @param data          Pointer to data location
 *
 * @ingroup apiPalApi
 */
#ifdef STORE_NIB_TO_FLASH
void pal_flash_write(uint32_t start_addr, uint32_t length, uint8_t *data);
#endif

#ifndef BOOT_FLASH

#ifdef NVM_MULTI_WRITE
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_erase_page(uint32_t page_start_addr);
#endif
#ifdef __GNUC__
void flash_erase_page(uint32_t page_start_addr)
    __attribute__((section(".bootloader"),noinline));
#endif
#endif  /* #ifdef _NVM_MULTI_WRITE */


#ifdef NVM_MULTI_WRITE
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_write_page(uint32_t page_start_addr);
#endif
#ifdef __GNUC__
void flash_write_page(uint32_t page_start_addr)
    __attribute__((section(".bootloader"),noinline));
#endif
#endif  /* #ifdef _NVM_MULTI_WRITE */


/**
 * @brief Fills the temporary page buffer
 *
 * This function fills the temporay flash page buffer.
 *
 * @param start_offset  Offset of start address
 * @param length        Length of provide data buffer
 * @param data          Pointer to data buffer
 *
 * @ingroup apiPalApi
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_fill_page_buffer(uint16_t start_offset, uint16_t length, uint8_t *data);
#endif
#ifdef __GNUC__
void flash_fill_page_buffer(uint16_t start_offset, uint16_t length, uint8_t *data)
    __attribute__((section(".bootloader"),noinline));
#endif


/**
 * @brief Writes the actual flash page
 *
 * This function writes the data from the temporary flash page buffer to the
 * actual flash page.
 *
 * @param page_start_addr  Start address of the flash page
 *
 * @ingroup apiPalApi
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_program_page(uint32_t page_start_addr);
#endif
#ifdef __GNUC__
void flash_program_page(uint32_t page_start_addr)
    __attribute__((section(".bootloader"),noinline));
#endif


/**
 * @brief Swaps the firmware image
 *
 * This function swapes the image with the current application program.
 *
 * @param image_start_addr  Start address of the image within the flash
 * @param image_size        Size of the firmware image
 *
 * @ingroup apiPalApi
 */
#ifdef __ICCAVR__
#pragma location = "bootloader"
void flash_swap(uint32_t image_start_addr, uint32_t image_size);
#endif
#ifdef __GNUC__
void flash_swap(uint32_t image_start_addr, uint32_t image_size)
    __attribute__((section(".bootloader"),noinline));
#endif


#endif /* BOOT_FLASH */

#endif  /* #ifndef PAL_FLASH_H */
#endif /* #if ((defined FLASH_SUPPORT) || (defined DOXYGEN)) */


