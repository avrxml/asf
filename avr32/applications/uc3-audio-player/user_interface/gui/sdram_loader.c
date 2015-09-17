/**************************************************************************
 *
 * \file
 *
 * \brief SDRAM loader.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stddef.h>
#include "board.h"
#include "sdramc.h"
#include "navigation.h"
#include "file.h"
#include "spi.h"
#include "at45dbx.h"
#include "conf_at45dbx.h"
#include "conf_audio_player.h"
#include "gpio.h"
#include "et024006dhu.h"


#include "sdram_loader.h"

mspace sdram_mspace;

#define DATAFLASH_NAV_ID 0
// probably LUN_ID1 from conf_access
#define DATAFLASH_DISK_ID 1

ram_file_t ram_files[NUMBER_OF_IMAGES] = {
	{ .name = L"/AVR32_start_320x240_RGB565.bmp"},
	{ .name = L"/disk_nav_320x240_RGB565.bmp"},
	{ .name = L"/audio_player.bmp"},
	{ .name = L"/Gjallarhorn_100x100_RGB565.bmp"},
	{ .name = L"/in_progress_128x16_RGB565.bmp"},
	{ .name = L"/loading_100x29_RGB565.bmp"},
	{ .name = L"/not_supported_176x29_RGB565.bmp"}
};


/*! \brief File header of a bitmap file. 14 bytes long, little endian representation and packed.
 * See bitmap specification for more details.
 */
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
bm_file_header_s {
  /*! \brief Magic number used to identify the bitmap file.
   * Typical values for these 2 bytes are 0x42 0x4D (ASCII code points for B and M)
   */
  unsigned short bfType;
  /*! \brief Size of the bitmap file */
  unsigned int bfSize;
  /*! \brief Reserved. May be used by some software */
  unsigned int bfReserved;
  /*! \brief Offset in bytes where the bitmap data can be found */
  unsigned int bfOffBits;
}
#if __ICCAVR32__
#pragma pack()
#endif
bm_file_header_t ;

/**
 * \brief Bitmap file information header. Starts after the file header and is 40 bytes long, in little endian and packed.
 */
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
bm_info_header_s {
  /*! \brief Size of the information header */
  unsigned int biSize;
  /*! \brief Width of the bitmap in pixels */
  int biWidth;
  /*! \brief Height of the bitmap in pixels
   *   If this value is positive -> "bottom-up"-bitmap (picture data starts with the bottom
   * line and ends with the top line)
   * If this value is negative ->"top-down"-Bitmap (picture data starts with the top line
   * and ends with the bottom line)
   */
  int biHeight;
  /*! \brief Number of color planes being used. Not often used. */
  unsigned short   biPlanes;
  /*! \brief Number of bits per pixel, which is the color depth of the image.
   *  Typical values are 1, 4, 8, 16, 24 and 32.
   */
  unsigned short   biBitCount;
  /*! \brief Define the compression method being used
   *  0 (BI_RGB): No compression
   *  1 (BI_RLE8):
   *  2 (BI_RLE4):
   *  3 (BI_BITFIELDS):
   */
  unsigned int   biCompression;

  /*! \brief Image size. This is the size of the raw bitmap data. */
  unsigned int   biSizeImage;
  /*! \brief Horizontal resolution */
  int   biXPelsPerMeter;
  /*! \brief Vertical resolution */
  int   biYPelsPerMeter;
  unsigned int   biClrUsed;
  unsigned int   biClrImportant;
}
#if __ICCAVR32__
#pragma pack()
#endif
bm_info_header_t;

/**
 * \brief Swaps a block of data to the other endian representation.
 *
 * \param pblock Pointer to the first byte of the data block
 * \param size Size of the data bock in bytes
 */
void swap_endian_blk(unsigned char *pblock, int size)
{
        unsigned char *pend = &pblock[size - 1];
        unsigned char temp;
        while(pblock < pend)
        {
                temp = *pblock;
                *pblock = *pend;
                *pend = temp;
                pblock++;
                pend--;
        }
}

void bm_get_info(bm_file_header_t *bf, bm_info_header_t *bi)
{
	// read header
	file_read_buf((void *) bf, sizeof(bm_file_header_t));
	file_read_buf((void *) bi, sizeof(bm_info_header_t));

  /* correct endianess for the stuff we need
   * only raw image data size and offset in file */
  //swap_endian_blk((unsigned char *) &bm_file_header.bfType, 2);
  //swap_endian_blk((unsigned char *) &bm_file_header.bfSize, 4);

  swap_endian_blk((unsigned char *) bf + offsetof(bm_file_header_t, bfOffBits), 4);
  swap_endian_blk((unsigned char *) bi + offsetof(bm_info_header_t, biWidth), 4);
  swap_endian_blk((unsigned char *) bi + offsetof(bm_info_header_t, biHeight), 4);
  swap_endian_blk((unsigned char *) bi + offsetof(bm_info_header_t, biBitCount), 2);
  swap_endian_blk((unsigned char *) bi + offsetof(bm_info_header_t, biSizeImage), 4);
  //swap_endian_blk((unsigned char *) &bm_info_header.biCompression, 4);


}
/*! \brief Initializes AT45DBX resources: GPIO, SPI and AT45DBX.
 */
static void at45dbx_resources_init(void)
{
  static const gpio_map_t AT45DBX_SPI_GPIO_MAP =
  {
    {AT45DBX_SPI_SCK_PIN,          AT45DBX_SPI_SCK_FUNCTION         },  // SPI Clock.
    {AT45DBX_SPI_MISO_PIN,         AT45DBX_SPI_MISO_FUNCTION        },  // MISO.
    {AT45DBX_SPI_MOSI_PIN,         AT45DBX_SPI_MOSI_FUNCTION        },  // MOSI.
#define AT45DBX_ENABLE_NPCS_PIN(NPCS, unused) \
    {AT45DBX_SPI_NPCS##NPCS##_PIN, AT45DBX_SPI_NPCS##NPCS##_FUNCTION},  // Chip Select NPCS.
    MREPEAT(AT45DBX_MEM_CNT, AT45DBX_ENABLE_NPCS_PIN, ~)
#undef AT45DBX_ENABLE_NPCS_PIN
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = AT45DBX_SPI_FIRST_NPCS,   // Defined in conf_at45dbx.h.
    .baudrate     = AT45DBX_SPI_MASTER_SPEED, // Defined in conf_at45dbx.h.
    .bits         = AT45DBX_SPI_BITS,         // Defined in conf_at45dbx.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
                     sizeof(AT45DBX_SPI_GPIO_MAP) / sizeof(AT45DBX_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(AT45DBX_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(AT45DBX_SPI, 0, 0, 0);

  // Enable SPI.
  spi_enable(AT45DBX_SPI);

  // Initialize data flash with .
  at45dbx_init(spiOptions, FPBA_HZ);
}

/* strlen is ascii string length (srcstr length)
 */
void ascii2unicode(char *deststr, const char *srcstr, size_t strlen)
{
	for(;strlen > 0;strlen--)
	{
		*deststr = '\0';
		deststr++;
		*deststr = *srcstr;
		deststr++;
		srcstr++;
	}
}

void load_sdram_data(int hsb_hz)
{
	uint8_t *ramptr;
	uint16_t i = 0;
	volatile uint16_t disks_cnt;
	bm_file_header_t bf;
	bm_info_header_t bi;
	int line;
	uint8_t tmp;
	bool warning_display=false;

	sdramc_init(hsb_hz);

	/* Create a new memory space from the SDRAM */
	sdram_mspace = create_mspace_with_base((void*) SDRAM_START_ADDRESS, MEM_SPACE_SIZE, 0);

	at45dbx_resources_init();

	nav_reset();
	if(nav_select(DATAFLASH_NAV_ID) == false)
		;

	disks_cnt = nav_drive_nb();

	if(nav_drive_set(DATAFLASH_NAV_ID) == false)
		;//error

	if(nav_partition_mount() == false)
	{
		// error
	}

	for(i = 0; i < (sizeof(ram_files)/sizeof(ram_file_t)); i++)
	{
		if( !ram_files[i].name )
			continue;
		if(nav_setcwd((FS_STRING) ram_files[i].name, false, false) == false)
		{
			warning_display=true;
			continue;
		}

		if( !file_open(FOPEN_MODE_R))
			continue;

		// get bitmap header information
		bm_get_info(&bf, &bi);

		// goto start of raw image data start
		file_seek(bf.bfOffBits, FS_SEEK_SET);

		// allocate space for the image from the SDRAM memory space
		if( (ram_files[i].start_addr = mspace_malloc(sdram_mspace, bi.biSizeImage)) == NULL)
		{
			file_close();
			continue;
		}

		// read image data to the SDRAM
		// We start with the last line since the picture is upside down
		for(line = bi.biHeight -1; line >= 0; line--)
		{
			file_read_buf((void *)((uint8_t *)ram_files[i].start_addr + (line * bi.biWidth * 2)), bi.biWidth * 2);
		}

		// we need to swap pixel data from LE to BE
		ramptr = ram_files[i].start_addr;
		for(line = 0; line < bi.biSizeImage; line++)
		{
			tmp = *ramptr;
			*ramptr = *(ramptr + 1);
			ramptr++;
			*ramptr = tmp;
			ramptr++;
		}

		file_close();
	}
	nav_reset();
	// TODO Disable the dataflash or make the USB LUN the default first
	// one in the audio player since we do not want to browse the data
	// flash for files.
	// For now we just disable the chip select pin
	gpio_enable_gpio_pin(AT45DBX_SPI_NPCS0_PIN);
	if( warning_display ) {
		et024006_PrintConsole("Unable to open some pictures", RED, -1);
		et024006_PrintConsole("on the dataflash disk", RED, -1);
		cpu_delay_ms(5000, FCPU_HZ);
	}
}

