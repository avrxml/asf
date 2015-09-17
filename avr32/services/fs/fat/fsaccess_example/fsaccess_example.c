/*****************************************************************************
 *
 * \file
 *
 * \brief File System access example application.
 *
 * This file gives an example of using the FS access, through POSIX commands.
 *
 * The example is written for UC3 and EVK110x.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software. It also gives an example of the usage of the
 * FAT filesystem through the dataflash memory AT45DBX & SPI driver.
 *
 * The example lets you issue the following commands on RS232 using POSIX interfaces (open, read, write, close):
 * - read a file from the memory files system;
 * - concatenate a file to an existing one;
 * - copy the content of a file to a new one.
 *
 * Note: This example assumes that the dataflash memory contains a file in the
 *       root directory (and of course that the memory filesystem is a
 *       FAT12/16/32).
 *
 * \section files Main Files
 * - file.c : functions for file accesses
 * - file.h : headers for file accesses
 * - fat.c : functions for FAT accesses
 * - fat.h : headers for FAT accesses
 * - fat_unusual.c : add-ons functions for FAT accesses
 * - fs_com.h : structures and defines for file system accesses
 * - navigation.c : functions for navigators accesses
 * - navigation.h : headers for navigators accesses
 * - fsaccess.c : functions for POSIX interfaces
 * - fsaccess.h : headers for POSIX interfaces
 * - ctrl_access.c : functions for memory control access interfaces
 * - ctrl_access.h : headers for memory control access interfaces
 * - fsaccess_example.c : FAT example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with an SPI module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1100, EVK1101 or EVK1105 evaluation kit
 *
 * \section setupinfo Setup Information
 * <BR>CPU speed: <i> 12 MHz </i>
 * - Connect USART1 (on EVK1100 or EVK1101) to a PC serial port via a standard
 *   RS232 DB9 cable, or USART0 (on EVK1105) abstracted with a USB CDC
 *   connection to a PC.
 * - Set the following settings in your terminal of choice: 57600 8N1
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <string.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "spi.h"
#include "conf_at45dbx.h"
#include "fsaccess.h"


// display a welcome message
#define MSG_WELCOME "\r\n ---------- Welcome to FSACCESS Example ---------- \r\n"
#define MSG_FILENAME "Enter the filename : "
#define MSG_MODE     "\r\nWhat do you want to do ?\r\n\t1 - Read file\r\n\t2 - Concatenate a file to an existing one\r\n\t3 - Copy the content of a file to a new one\r\nenter your choice : "


// display test result
#define TEST_SUCCESS "\t[PASS]\r\n"
#define TEST_FAIL    "\t[FAIL]\r\n"

#define CR                    '\r'
#define LF                    '\n'
#define CRLF                  "\r\n"
#define CTRL_Q                0x11
#define CTRL_C                0x03
#define BKSPACE_CHAR          '\b'
#define ABORT_CHAR            CTRL_C
#define QUIT_APPEND           CTRL_Q


#define NB_SECTOR_TO_SEND    4


int fsaccess_example_get_mode (void);
void fsaccess_example_get_filename (char * buf);
int fsaccess_example_read(char * path);
int fsaccess_example_write(char * source, char * destination, int flags);

#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz

#if UC3L
/*! \name Parameters to pcl_configure_clocks().
 */
//! @{
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  undef  EXAMPLE_TARGET_PBACLK_FREQ_HZ
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
static pcl_freq_param_t pcl_dfll_freq_param =
{
  .main_clk_src = PCL_MC_DFLL0,
  .cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
  .pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
  .pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
  .dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
  .pextra_params = &gc_dfllif_ref_opt
};
//! @}
#endif


/*! \brief Initialize dataflash memory AT45DBX resources: GPIO, SPI and AT45DBX
 *
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

  // Initialize data flash with SPI clock at EXAMPLE_TARGET_PBACLK_FREQ_HZ.
  at45dbx_init(spiOptions, EXAMPLE_TARGET_PBACLK_FREQ_HZ);
}


/*! \brief Main function, execution starts here.
 *  RS232 is used to input/output information.
 *  The example lets you issue the following commands on RS232 using POSIX interfaces (open, read, write, close):
 *  - read a file from the memory files system;
 *  - concatenate a file to an existing one;
 *  - copy the content of a file to a new one.
 *
 * Connect USART_1 to your serial port via a standard RS-232 D-SUB9 cable
 * Set the following settings in your terminal of choice: 57600 8N1
 *
 */
int main(void)
{
char filename1[90];
char filename2[90];

#if BOARD == UC3L_EK
  // Note: on the AT32UC3L-EK board, there is no crystal/external clock connected
  // to the OSC0 pinout XIN0/XOUT0. We shall then program the DFLL and switch the
  // main clock source to the DFLL.
  pcl_configure_clocks(&pcl_dfll_freq_param);
  // Note: since it is dynamically computing the appropriate field values of the
  // configuration registers from the parameters structure, this function is not
  // optimal in terms of code size. For a code size optimal solution, it is better
  // to create a new function from pcl_configure_clocks_dfll0() and modify it
  // to use preprocessor computation from pre-defined target frequencies.
#else
  // Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
  // frequency FOSC0) with an appropriate startup time then switch the main clock
  // source to Osc0.
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif


  // initialize RS232 debug text output
  init_dbg_rs232(EXAMPLE_TARGET_PBACLK_FREQ_HZ);

  // initialize AT45DBX resources: GPIO, SPI and AT45DBX
  at45dbx_resources_init();

  // initialize FSACCESS mutex and navigators
  b_fsaccess_init();

  // Try to init data flash
  if (at45dbx_mem_check())
  {
    // display welcome message
    print_dbg(MSG_WELCOME);
  }
  else
  {
    // display error message
    print_dbg("Initialization failed\r\n");
    return (-1);
  }

  while (true)
  {
    // check the user command
    switch (fsaccess_example_get_mode())
    {
    case '1':
      // Wait for a filename
      fsaccess_example_get_filename(filename1);
      // Read it and display the content
      fsaccess_example_read(filename1);
    break;
    case '2':
      // Wait for source filename
      print_dbg("Source : ");
      fsaccess_example_get_filename(filename1);
      // Wait for destination filename
      print_dbg("Destination : ");
      fsaccess_example_get_filename(filename2);
      // Write from source to destination (append to the existing file)
      fsaccess_example_write(filename1, filename2, O_APPEND);
    break;
    case '3':
      // Wait for source filename
      print_dbg("Source : ");
      fsaccess_example_get_filename(filename1);
      // Wait for destination filename
      print_dbg("Destination : ");
      fsaccess_example_get_filename(filename2);
      // Write from source to destination (append to the nonexisting file)
      fsaccess_example_write(filename1, filename2, (O_CREAT | O_APPEND));
    break;
    default:
      // Display error message
      print_dbg("Wrong choice\r\n");
    break;
    }
  }
}

/*! \brief Wait for user to select the mode he wants to test
 */
int fsaccess_example_get_mode (void)
{
int c;

  // Display asking message
  print_dbg(MSG_MODE);
  while (true)
  {
    // If something new in the USART :
    usart_reset_status(DBG_USART);
    if (usart_read_char(DBG_USART, &c) == USART_SUCCESS)
    {
      // Echo char
      print_dbg_char(c);
      // Add CRLF
      print_dbg(CRLF);
      return (c);
    }
  }
}

/*! \brief Wait for user to enter a filename
 */
void fsaccess_example_get_filename (char * buf)
{
unsigned long i_str = 3;
unsigned short file;
int c;

  // Display asking message
  print_dbg(MSG_FILENAME);
  file = false;
  buf[0] = 'A';
  buf[1] = ':';
  buf[2] = '/';

  while (file == false)
  {
    // If something new in the USART :
    usart_reset_status(DBG_USART);
    if (usart_read_char(DBG_USART, &c) == USART_SUCCESS)
    {
      // Regarding the received char :
      switch (c)
      {
      // This is a carriage return
      case CR:
        // Add CRLF
        print_dbg(CRLF);
        // Add NUL char
        buf[i_str] = '\0';
        // End the loop
        file = true;
        // Reset string length
        i_str = 0;
      break;
      // This is a CTRL+C
      case ABORT_CHAR:
        i_str = 0;
        // Add CRLF
        print_dbg(CRLF);
        print_dbg(MSG_FILENAME);
      break;
      // This is a backspace
      case BKSPACE_CHAR:
        if (i_str > 0)
        {
          // Echo the backspace
          print_dbg_char(c);
          // Echo a space
          print_dbg_char(' ');
          // Echo the backspace
          print_dbg_char(c);
          // Decrease string length
          i_str--;
        }
      break;
      // This is a char
      default:
        // Echo it
        print_dbg_char(c);
        // Append to cmd line and increase string length
        buf[i_str++] = c;
      break;
      }
    }
  }
}

/*! \brief Read a file and display it to USART
 */
int fsaccess_example_read(char * path)
{
char * ptrFile;
int fd, i;
long size;

  // Try to open the file
  if ((fd = open(path, O_RDONLY)) < 0)
  {
    // Display error message
    print_dbg(path);
    print_dbg(" : Open failed\r\n");
    return (-1);
  }

  // Get file size
  size = fsaccess_file_get_size(fd);
  // Display file size
  print_dbg("File size: ");
  print_dbg_ulong(size);
  // Add CRLF
  print_dbg(CRLF);

  // Allocate a buffer
  ptrFile = malloc((NB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR) + 1);

  // Allocation fails
  if (ptrFile == NULL)
  {
    // Display error message
    print_dbg("Malloc failed\r\n");
  }
  else
  {
    // Try to perform a single access
    if ( size < (NB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR) )
    {
      if( read(fd, ptrFile, size) != size)
      {
         // Display error message
         print_dbg("Reading entire file failed\r\n");
      }
       else
       {
         // Add a null terminating char
         ptrFile[size] = '\0';
         // Display the buffer to user
         print_dbg(ptrFile);
       }
    }
    else
    {
      // Try to send the biggest frame contained in the file
      for (i = NB_SECTOR_TO_SEND ; i > 0 ; i--)
      {
        // Get sectors of maximum size
        while(size > i * FS_SIZE_OF_SECTOR)
        {
          // Get the data from file
          if( read(fd, ptrFile, i * FS_SIZE_OF_SECTOR) !=  i * FS_SIZE_OF_SECTOR)
          {
            // Display error message
            print_dbg("Reading file block failed\r\n");
            // Close file
            close(fd);
            return (-1);
          }
          // Add a null terminating character
          ptrFile[i * FS_SIZE_OF_SECTOR] = '\0';
          // Display buffer content to user
          print_dbg(ptrFile);
          // Decrease remaining size
          size -= (i * FS_SIZE_OF_SECTOR);
        }
      }
      // Finish with the few data remaining (less than 1 sector)
      if ( size > 0 )
      {
        // Get the data from filesystem
        if( read(fd, ptrFile, size) != size)
        {
          // Display error message
          print_dbg("Reading file end failed\r\n");
          // Close file
          close(fd);
          return (-1);
        }
        else
        {
          // Add a null terminating char
          ptrFile[size] = '\0';
          // Display the buffer to user
          print_dbg(ptrFile);
        }
      }
    }
    // Free the buffer
    free(ptrFile);
  }
  // Close file
  close(fd);
  return (0);
}

/*! \brief Copy a file to another one
 */
int fsaccess_example_write(char * source, char * destination, int flags)
{
char * ptrFile;
int fd1, fd2, i;
long size;
int ErrorCode = -1;

  if ((fd1 = open(source, O_RDONLY)) < 0)
  {
    // Display error message
    print_dbg(source);
    print_dbg(": Open failed\r\n");
    return (ErrorCode);
  }
  if ((fd2 = open(destination, flags)) < 0)
  {
    // Display error message
    print_dbg(destination);
    print_dbg(": Open failed\r\n");
    return (ErrorCode);
  }

  // Get file size
  size = fsaccess_file_get_size(fd1);
  // Display file size
  print_dbg("Copying ");
  print_dbg_ulong(size);
  print_dbg(" Bytes\r\n");

  // Allocate a buffer
  ptrFile = malloc(NB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR);

  // Allocation fails
  if (ptrFile == NULL)
  {
    // Display error message
    print_dbg("Malloc failed\r\n");
    return (ErrorCode);
  }
  else
  {
    if ( size <= (NB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR) )
    {
      if ( read(fd1, ptrFile, size) != size )
      {
        // Display error message
        print_dbg("Reading entire file failed\r\n");
        // Escape
        goto close_end;
      }
      if ( write(fd2, ptrFile, size) != size )
      {
        // Display error message
        print_dbg("Writing entire file failed\r\n");
        // Escape
        goto close_end;
      }
    }
    else
    {
      // Try to send the biggest frame contained in the file
      for (i = NB_SECTOR_TO_SEND ; i > 0 ; i--)
      {
        // Get sectors of maximum size
        while(size > i * FS_SIZE_OF_SECTOR)
        {
          // Clear previous buffer
          memset(ptrFile, 0, NB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR);
          // Read the data from source file
          if( read(fd1, ptrFile, i * FS_SIZE_OF_SECTOR) !=  i * FS_SIZE_OF_SECTOR)
          {
            print_dbg("Reading file block failed\r\n");
            // Escape
            goto close_end;
          }
          // Write the data to destination file
          if ( write(fd2, ptrFile, i * FS_SIZE_OF_SECTOR) != i * FS_SIZE_OF_SECTOR )
          {
            print_dbg("Writing file block failed\r\n");
            // Escape
            goto close_end;
          }
          // Decrease remaining size
          size -= (i * FS_SIZE_OF_SECTOR);
        }
      }
      // Finish with the few data remaining (less than 1 sector)
      if ( size > 0 )
      {
      	// Get the data from filesystem
        if( read(fd1, ptrFile, size) !=  size)
        {
          print_dbg("Reading file end failed\r\n");
          // Escape
          goto close_end;
        }
        if ( write(fd2, ptrFile, size) != size )
        {
          print_dbg("Writing file end failed\r\n");
          // Escape
          goto close_end;
        }
      }
    }
    // No error occurs here
    ErrorCode = 0;
  }

close_end:
  // Free the buffer
  free(ptrFile);
  // Close files
  close(fd1);
  close(fd2);
  return (ErrorCode);
}
