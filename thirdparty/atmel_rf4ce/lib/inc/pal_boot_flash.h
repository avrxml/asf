/**
 * @file pal_boot_flash.h
 *
 * @brief  Declaration Boot flash functions.
 *
 * $Id: pal_boot_flash.h 26812 2011-05-23 08:27:09Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

 #if ((defined BOOT_FLASH) || (defined DOXYGEN))

#ifndef _PALBOOTFLASH_H
#define _PALBOOTFLASH_H

/******************************************************************************
                   Types section
******************************************************************************/
/* The necessary list of functions ID for rf4ce */
typedef enum
{
  FILL_PAGE_BUFFER,
  PROGRAM_PAGE,
  SWAP_IMAGE,
  ERASE_PAGE,
  WRITE_PAGE,
  MAX_FUNCTION_NUMBER
} rf4ce_func_identity_t;

/* Parameters for filling page buffer */
typedef struct
{
  uint16_t start_offset;
  uint16_t length;
  uint8_t *data;
} fill_page_buffer_req_t;

/* Parameters for programming page */
typedef struct
{
  uint32_t page_start_addr;
} program_page_req_t;

/* Parameters for swapping image */
typedef struct
{
  uint32_t image_start_addr;
  uint32_t image_size;
} swap_images_req_t;

/* Parameters for erasing page */
typedef struct
{
  uint32_t page_start_addr;
} erase_page_req_t;

/* Parameters for writing page */
typedef struct
{
  uint32_t page_start_addr;
} write_page_req_t;

/* Common memory for all requests */
typedef union
{
  fill_page_buffer_req_t fill_page_buffer_req;
  program_page_req_t     program_page_req;
  swap_images_req_t      swap_images_req;
  erase_page_req_t       erase_page_req;
  write_page_req_t       write_page_req;
} common_boot_req_t;

/* Request structure for boot functionality */
typedef struct
{
  rf4ce_func_identity_t request_type;
  common_boot_req_t     common_boot_req;
} rf4ce_boot_req_t;

/* Prototype of the entry point to bootloader. */
typedef void (* boot_entry_point_t)(void);

/* === Prototypes =========================================================== */

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
void flash_fill_page_buffer(uint16_t start_offset, uint16_t length, uint8_t *data);

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
void flash_program_page(uint32_t page_start_addr);

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
void flash_swap(uint32_t image_start_addr, uint32_t image_size);
/**
 * @brief Erase the flash page
 *
 * This function erase the data from the flash page.
 *
 * @param page_start_addr  Start address of the flash page
 *
 * @ingroup apiPalApi
 */
void flash_erase_page(uint32_t page_start_addr);

/**
 * @brief Write the flash page
 *
 * This function writes the data to the flash page.
 *
 * @param page_start_addr  Start address of the flash page
 *
 * @ingroup apiPalApi
 */
void flash_write_page(uint32_t page_start_addr);

#endif /* _RF4CEFLASH_H */
#endif /* #if ((defined FLASH_SUPPORT) || (defined DOXYGEN)) */
