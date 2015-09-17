/**************************************************************************
 *
 * \file
 *
 * \brief uShell command line interpreter.
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


//_____  I N C L U D E S ___________________________________________________

#if (defined __GNUC__)
  #include "nlao_usart.h"
#endif
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "usart.h"
#ifdef FREERTOS_USED
  #include "FreeRTOS.h"
  #include "task.h"
#endif
#include "ctrl_access.h"
#include "fat.h"
#include "file.h"
#include "navigation.h"
#include "conf_usb.h"
#include "usb_drv.h"
#include "cycle_counter.h"
#if USB_HOST_FEATURE == true
  #include "usb_host_enum.h"
  #include "usb_host_task.h"
  #include "host_mass_storage_task.h"
#endif
#include "ushell_task.h"


//_____ M A C R O S ________________________________________________________

/*! \name Shell USART Configuration
 */
//! @{
#if BOARD == EVK1100
#  define SHL_USART               (&AVR32_USART0)
#  define SHL_USART_RX_PIN        AVR32_USART0_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART0_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART0_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1101
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1103
#  define SHL_USART               (&AVR32_USART2)
#  define SHL_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1104
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1105
#  define SHL_USART               (&AVR32_USART0)
#  define SHL_USART_RX_PIN        AVR32_USART0_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART0_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART0_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == UC3C_EK
#  define SHL_USART               (&AVR32_USART2)
#  define SHL_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#else
#  error Unsupported board
#endif
//! @}

#define USHELL_HISTORY        10 // Must be 1 or more
#define USHELL_NB_LINE        20
#define USHELL_NB_COL         80

#define USHELL_SIZE_CMD_LINE  70
#define USHELL_MAX_NB_ARG     2

//_____ D E C L A R A T I O N S ____________________________________________


// Manage task
static bool     g_b_ushell_task_run = false;
static uint32_t g_u32_ushell_pba_hz;

// To manage command line
static uint8_t  g_u8_escape_sequence=0;
static uint8_t  g_u8_cmd_size=0;
static uint8_t  g_u8_history_pos=0;
static uint8_t  g_u8_history_pos_search=0;
static char     g_s_cmd_his[USHELL_HISTORY][USHELL_SIZE_CMD_LINE];
static char     g_s_cmd[USHELL_SIZE_CMD_LINE];
static char     g_s_arg[USHELL_MAX_NB_ARG][USHELL_SIZE_CMD_LINE];

// To manage a file system shortcut
static Fs_index g_mark_index;


// Internal sub routines
bool    ushell_cmd_scan         ( void );
uint8_t ushell_cmd_decode       ( void );
void    ushell_clean_cmd_line   ( void );
void    ushell_history_up       ( void );
void    ushell_history_down     ( void );
void    ushell_history_display  ( void );
bool    ushell_more_wait        ( void );
// Internal sub routines for file system commands
void  ushell_cmd_nb_drive     ( void );
void  ushell_cmd_free_space   ( void );
void  ushell_cmd_format       ( void );
void  ushell_cmd_mount        ( void );
void  ushell_cmd_space        ( void );
void  ushell_cmd_ls           ( bool b_more );
void  ushell_cmd_cd           ( void );
void  ushell_cmd_gotoparent   ( void );
void  ushell_cmd_cat          ( bool b_more);
void  ushell_cmd_help         ( void );
void  ushell_cmd_mkdir        ( void );
void  ushell_cmd_touch        ( void );
void  ushell_cmd_rm           ( void );
void  ushell_cmd_append_file  ( void );
void  ushell_cmd_copy         ( void );
void  ushell_cmd_rename       ( void );
bool  ushell_cmd_sync         ( void );
void  ushell_cmd_perform      ( void );
void  ushell_path_valid_syntac( char *path );
// Internal sub routines for USB commands
void  ushell_cmdusb_ls        ( void );
void  ushell_cmdusb_suspend   ( void );
void  ushell_cmdusb_resume    ( void );


//! @brief This function initializes the hardware/software resources required for ushell task.
//!
void ushell_task_init(uint32_t pba_hz)
{
  uint8_t u8_i;

  //** Initialize the USART used by uShell with the configured parameters
  static const gpio_map_t SHL_USART_GPIO_MAP =
  {
    {SHL_USART_RX_PIN, SHL_USART_RX_FUNCTION},
    {SHL_USART_TX_PIN, SHL_USART_TX_FUNCTION}
  };
#if (defined __GNUC__)
  set_usart_base((void *)SHL_USART);
  gpio_enable_module(SHL_USART_GPIO_MAP,
                     sizeof(SHL_USART_GPIO_MAP) / sizeof(SHL_USART_GPIO_MAP[0]));
  usart_init(SHL_USART_BAUDRATE);
#elif (defined __ICCAVR32__)
  static const usart_options_t SHL_USART_OPTIONS =
  {
    .baudrate = SHL_USART_BAUDRATE,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = SHL_USART;
  gpio_enable_module(SHL_USART_GPIO_MAP,
                     sizeof(SHL_USART_GPIO_MAP) / sizeof(SHL_USART_GPIO_MAP[0]));
  usart_init_rs232(SHL_USART, &SHL_USART_OPTIONS, pba_hz);
#endif


  //** Configure standard I/O streams as unbuffered.
#if (defined __GNUC__)
  setbuf(stdin, NULL);
#endif
  setbuf(stdout, NULL);

  // Set default state of ushell
  g_b_ushell_task_run = false;
  for( u8_i=0; u8_i<USHELL_HISTORY; u8_i++ ) {
     g_s_cmd_his[u8_i][0] = 0;  // Set end of line for all cmd line history
  }

  fputs(MSG_EXIT, stdout );

  g_u32_ushell_pba_hz = pba_hz;  // Save value to manage a time counter during perform command

#ifdef FREERTOS_USED
  xTaskCreate(ushell_task,
              configTSK_USHELL_NAME,
              configTSK_USHELL_STACK_SIZE,
              NULL,
              configTSK_USHELL_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


#ifdef FREERTOS_USED
/*! \brief Entry point of the explorer task management.
 *
 * This function performs uShell decoding to access file-system functions.
 *
 * \param pvParameters Unused.
 */
void ushell_task(void *pvParameters)
#else
/*! \brief Entry point of the explorer task management.
 *
 * This function performs uShell decoding to access file-system functions.
 */
void ushell_task(void)
#endif
{

#ifdef FREERTOS_USED
   //** Inifinite loop for RTOS because it is a RTOS task
   portTickType xLastWakeTime;

   xLastWakeTime = xTaskGetTickCount();
   while (true)
   {
      vTaskDelayUntil(&xLastWakeTime, configTSK_USHELL_PERIOD);
#else
   //** No loop with the basic scheduler
   {
#endif  // FREERTOS_USED


   //** Check the USB mode and authorize/unauthorize ushell
   if(!g_b_ushell_task_run)
   {
      if( Is_usb_id_device() )
#ifdef FREERTOS_USED
         continue;   // Continue in the RTOS task
#else
         return;     // Exit of the task scheduled
#endif
      g_b_ushell_task_run = true;
      // Display shell startup
      fputs(MSG_WELCOME, stdout);
      ushell_cmd_nb_drive();
      fputs(MSG_PROMPT, stdout);

      // Reset the embedded FS on ushell navigator and on first drive
      nav_reset();
      nav_select( FS_NAV_ID_USHELL_CMD );
      nav_drive_set( 0 );
   }else{
      if( Is_usb_id_device() )
      {
         g_b_ushell_task_run = false;
         fputs(MSG_EXIT, stdout );
         nav_exit();
#ifdef FREERTOS_USED
         continue;   // Continue in the RTOS task
#else
         return;     // Exit of the task scheduled
#endif
      }
   }

   //** Scan shell command
   if( !ushell_cmd_scan() )
#ifdef FREERTOS_USED
      continue;   // Continue in the RTOS task
#else
      return;     // Exit of the task scheduled
#endif

   //** Command ready then decode and execute this one
   switch( ushell_cmd_decode() )
   {
      // Displays number of  drives
      case CMD_NB_DRIVE:
      ushell_cmd_nb_drive();
      break;

      // Displays free space information for all connected drives
      case CMD_DF:
      ushell_cmd_free_space();
      break;

      // Formats disk
      case CMD_FORMAT:
      ushell_cmd_format();
      break;

      // Mounts a drive (e.g. "b:")
      case CMD_MOUNT:
      ushell_cmd_mount();
      break;

      // Displays the space information for current drive
      case CMD_SPACE:
      ushell_cmd_space();
      break;

      // Lists the files present in current directory (e.g. "ls")
      case CMD_LS:
      ushell_cmd_ls(false);
      break;
      case CMD_LS_MORE:
      ushell_cmd_ls(true);
      break;

      // Enters in a directory (e.g. "cd folder_toto")
      case CMD_CD:
      ushell_cmd_cd();
      break;

      // Enters in parent directory ("cd..")
      case CMD_UP:
      ushell_cmd_gotoparent();
      break;

      // Displays a text file
      case CMD_CAT:
      ushell_cmd_cat(false);
      break;
      case CMD_CAT_MORE:
      ushell_cmd_cat(true);
      break;

      // Displays the help
      case CMD_HELP:
      ushell_cmd_help();
      break;

      // Creates directory
      case CMD_MKDIR:
      ushell_cmd_mkdir();
      break;

      // Creates file
      case CMD_TOUCH:
      ushell_cmd_touch();
      break;

      // Deletes files or directories
      case CMD_RM:
      ushell_cmd_rm();
      break;

      // Appends char to selected file
      case CMD_APPEND:
      ushell_cmd_append_file();
      break;

      // Index routines (= specific shortcut from ATMEL FileSystem)
      case CMD_SET_ID:
      g_mark_index = nav_getindex();
      break;
      case CMD_GOTO_ID:
      nav_gotoindex( &g_mark_index );
      break;

      // Copies file to other location
      case CMD_CP:
      ushell_cmd_copy();
      break;

      // Renames file
      case CMD_MV:
      ushell_cmd_rename();
      break;

      // Synchronize folders
      case CMD_SYNC:
      ushell_cmd_sync();
      break;

      case CMD_PERFORM:
      ushell_cmd_perform();
      break;

      // USB commands
#if USB_HOST_FEATURE == true
      case CMD_LS_USB:
      ushell_cmdusb_ls();
      break;
      case CMD_USB_SUSPEND:
      ushell_cmdusb_suspend();
      break;
      case CMD_USB_RESUME:
      ushell_cmdusb_resume();
      break;
#endif

      case CMD_NONE:
      break;

      // Unknown command
      default:
      fputs(MSG_ER_CMD_NOT_FOUND, stdout);
      break;
   }

   fputs(MSG_PROMPT, stdout);

   }
}


//! @brief Get the full command line to be interpreted.
//!
//! @return true, if a command is ready
//!
bool ushell_cmd_scan(void)
{
   int c_key;

   // Something new of the UART ?
   if (usart_read_char(SHL_USART, &c_key) != USART_SUCCESS)
   {
      usart_reset_status(SHL_USART);
      return false;
   }

   if( 0 != g_u8_escape_sequence )
   {
      //** Decode escape sequence
      if( 1 == g_u8_escape_sequence )
      {
         if( 0x5B != c_key )
         {
            g_u8_escape_sequence=0;
            return false;  // Escape sequence cancel
         }
         g_u8_escape_sequence=2;
      }
      else
      {
         // Decode value of the sequence
         switch (c_key)
         {
/*
Note: OVERRUN error on USART with an RTOS and USART without interrupt management
If you want support "Escape sequence", then you have to implement USART interrupt management
            case 0x41:     // UP command
            ushell_clean_cmd_line();
            ushell_history_up();
            ushell_history_display();
            break;
            case 0x42:     // DOWN command
            ushell_clean_cmd_line();
            ushell_history_down();
            ushell_history_display();
            break;
*/
            default:       // Ignore other command
            break;
         }
         g_u8_escape_sequence=0; // End of Escape sequence
      }
      return false;
   }

   //** Normal sequence
   switch (c_key)
   {
      //** Command validation
      case ASCII_CR:
      putchar(ASCII_CR);         // Echo
      putchar(ASCII_LF);         // Add new line flag
      g_s_cmd_his[g_u8_history_pos][g_u8_cmd_size]=0;  // Add NULL terminator at the end of command line
      return true;

      //** Enter in escape sequence
      case ASCII_ESCAPE:
      g_u8_escape_sequence=1;
      break;

      //** backspace
      case ASCII_BKSPACE:
      if(g_u8_cmd_size>0)        // Beginning of line ?
      {
         // Remove the last character on terminal
         putchar(ASCII_BKSPACE); // Send a backspace to go in previous character
         putchar(' ');           // Send a space to erase previous character
         putchar(ASCII_BKSPACE); // Send a backspace to go in new end position (=previous character position)
         // Remove the last character on cmd line buffer
         g_u8_cmd_size--;
      }
      break;

      // History management
      case '!':
      ushell_clean_cmd_line();
      ushell_history_up();
      ushell_history_display();
      break;
      case '$':
      ushell_clean_cmd_line();
      ushell_history_down();
      ushell_history_display();
      break;

      //** Other char
      default:
      if( (0x1F<c_key) && (c_key<0x7F) && (USHELL_SIZE_CMD_LINE!=g_u8_cmd_size) )
      {
         // Accept char
         putchar(c_key);                                          // Echo
         g_s_cmd_his[g_u8_history_pos][g_u8_cmd_size++] = c_key;  // append to cmd line
      }
      break;
   }
   return false;
}


//! @brief decodes full command line into command type and arguments
//!
//!
//! @return the command type decoded
//!
//! @verbatim
//! The arguments are storage in g_s_arg global array
//! @endverbatim
//!
uint8_t ushell_cmd_decode( void )
{
   uint8_t cmd_type;
   uint8_t u8_i,u8_j,u8_k;
   bool b_arg_include_space;

   if(0==g_u8_cmd_size)
   {
      // Command line empty
      return CMD_NONE;
   }

   // Get command string and Change command to lower case
   for( u8_i=0; (g_s_cmd_his[g_u8_history_pos][u8_i]!=' ') && (u8_i<=g_u8_cmd_size); u8_i++)
   {
      g_s_cmd[u8_i] = g_s_cmd_his[g_u8_history_pos][u8_i];
      if( ('A'<=g_s_cmd[u8_i]) && (g_s_cmd[u8_i]<='Z') )
         g_s_cmd[u8_i] += ('a'-'A');
   }
   g_s_cmd[u8_i]=0;

   // Get arguments strings
   for( u8_j=0; u8_j<USHELL_MAX_NB_ARG; u8_j++ )
   {
      u8_i++;     // Jump space character
      // Check "
      b_arg_include_space = ( g_s_cmd_his[g_u8_history_pos][u8_i] == '"' );
      if( b_arg_include_space ) {
        u8_i++;
      }
      for( u8_k=0;
           (b_arg_include_space || (g_s_cmd_his[g_u8_history_pos][u8_i] != ' '))
           && ((!b_arg_include_space) || (g_s_cmd_his[g_u8_history_pos][u8_i] != '"'))
           && (u8_i<=g_u8_cmd_size);
           u8_i++, u8_k++ )
      {
         g_s_arg[u8_j][u8_k] = g_s_cmd_his[g_u8_history_pos][u8_i];
      }
      if( b_arg_include_space ) {
        u8_i++;   // Jump last "
      }
      g_s_arg[u8_j][u8_k] = 0;
   }

   // Reset command size and update history
   g_u8_cmd_size=0;
   g_u8_history_pos++;
   if( g_u8_history_pos == USHELL_HISTORY)
      g_u8_history_pos = 0;
   g_u8_history_pos_search = g_u8_history_pos;

   // Decode command type
   if (!strcmp(g_s_cmd, STR_DISK ))
   {  cmd_type=CMD_NB_DRIVE; }
   else if ( !strcmp(g_s_cmd, STR_DF))
   {  cmd_type=CMD_DF; }
   else if ( !strcmp(g_s_cmd, STR_FORMAT))
   {  cmd_type=CMD_FORMAT; }
   else if ( !strcmp(g_s_cmd, STR_MOUNT))
   {  cmd_type=CMD_MOUNT; }
   else if ( g_s_cmd[1]==':' )
   {  cmd_type=CMD_MOUNT; g_s_arg[0][0]=g_s_cmd[0];g_s_arg[0][1]='0'; }
   else if ( !strcmp(g_s_cmd, STR_SPACE))
   {  cmd_type=CMD_SPACE; }
   else if ( !strcmp(g_s_cmd, STR_LS))
   {  cmd_type=CMD_LS; }
   else if ( !strcmp(g_s_cmd, STR_LS_MORE))
   {  cmd_type=CMD_LS_MORE; }
   else if (!strcmp(g_s_cmd, STR_CD))
   {  cmd_type=CMD_CD; }
   else if ( !strcmp(g_s_cmd, STR_UP))
   {  cmd_type=CMD_UP; }
   else if ( !strcmp(g_s_cmd, STR_CAT))
   {  cmd_type=CMD_CAT; }
   else if ( !strcmp(g_s_cmd, STR_CAT_MORE))
   {  cmd_type=CMD_CAT_MORE; }
   else if ( !strcmp(g_s_cmd, STR_HELP))
   {  cmd_type=CMD_HELP; }
   else if ( !strcmp(g_s_cmd, STR_MKDIR))
   {  cmd_type=CMD_MKDIR; }
   else if ( !strcmp(g_s_cmd, STR_TOUCH))
   {  cmd_type=CMD_TOUCH; }
   else if ( !strcmp(g_s_cmd, STR_RM))
   {  cmd_type=CMD_RM; }
   else if ( !strcmp(g_s_cmd, STR_APPEND))
   {  cmd_type=CMD_APPEND; }
   else if ( !strcmp(g_s_cmd, STR_MARK))
   {  cmd_type=CMD_SET_ID; }
   else if ( !strcmp(g_s_cmd, STR_GOTO))
   {  cmd_type=CMD_GOTO_ID; }
   else if ( !strcmp(g_s_cmd, STR_CP))
   {  cmd_type=CMD_CP; }
   else if ( !strcmp(g_s_cmd, STR_MV))
   {  cmd_type=CMD_MV; }
   else if ( !strcmp(g_s_cmd, STR_SYNC))
   {  cmd_type=CMD_SYNC; }
   else if ( !strcmp(g_s_cmd, STR_PERFORM))
   {  cmd_type=CMD_PERFORM; }
#if USB_HOST_FEATURE == true
   else if ( !strcmp(g_s_cmd, STR_LS_USB))
   {  cmd_type=CMD_LS_USB; }
   else if ( !strcmp(g_s_cmd, STR_USB_SUSPEND))
   {  cmd_type=CMD_USB_SUSPEND; }
   else if ( !strcmp(g_s_cmd, STR_USB_RESUME))
   {  cmd_type=CMD_USB_RESUME; }
#endif
   else
   {
      fputs(MSG_ER_CMD_NOT_FOUND, stdout);
      return CMD_NONE;
   }
   return cmd_type;
}


//! @brief Cleans the command line on the display
//!
void ushell_clean_cmd_line( void )
{
   // Clean command line display
   while( 0 != g_u8_cmd_size )
   {
      // Remove the last character on cmd line buffer
      putchar(ASCII_BKSPACE); // Send a backspace to go in previous character
      putchar(' ');           // Send a space to erase previous character
      putchar(ASCII_BKSPACE); // Send a backspace to go in new end position (=previous character position)
      g_u8_cmd_size--;
   }
}


//! @brief Selects the previous command in history list
//!
void ushell_history_up( void )
{
   if( g_u8_history_pos_search == 0 )
   {
      if( (USHELL_HISTORY-1) == g_u8_history_pos )
         return;  // End of history list
      g_u8_history_pos_search = USHELL_HISTORY-1;
   }else{
      if( (g_u8_history_pos_search-1) == g_u8_history_pos )
         return;  // End of history list
      g_u8_history_pos_search--;
   }
   if( 0 == g_s_cmd_his[g_u8_history_pos_search][0] )
   {
      // History empty then go to previous selection
      ushell_history_down();
   }
}


//! @brief Selects the next command in history list
//!
void ushell_history_down( void )
{
   if( g_u8_history_pos_search == g_u8_history_pos )
      return;  // End of history list
   if( g_u8_history_pos == 0 )
   {
      if( (USHELL_HISTORY-1) == g_u8_history_pos_search )
         return;  // End of history list
      g_u8_history_pos_search++;
   }else{
      if( (g_u8_history_pos_search+1) == g_u8_history_pos )
         return;  // End of history list
   }
   g_u8_history_pos_search++;
   if( USHELL_HISTORY == g_u8_history_pos_search )
      g_u8_history_pos_search = 0;
}


//! @brief Displays the current history
//!
void ushell_history_display( void )
{
   g_u8_cmd_size=0;
   while( g_s_cmd_his[g_u8_history_pos_search][g_u8_cmd_size] != 0 )
   {
      putchar( g_s_cmd_his[g_u8_history_pos_search][g_u8_cmd_size] );
      g_s_cmd_his[g_u8_history_pos][g_u8_cmd_size] = g_s_cmd_his[g_u8_history_pos_search][g_u8_cmd_size];
      g_u8_cmd_size++;
   }
   g_s_cmd_his[g_u8_history_pos][g_u8_cmd_size] = 0;
}


//! @brief This function wait a key press
//!
//! @return true, if the action must be continue
//!
bool ushell_more_wait( void )
{
   int c_key;
   printf("\n\r-- space for more--");
   c_key=0;
   while( (c_key!='q') && (c_key!=' ') )
   {
      usart_reset_status(SHL_USART);
      while(usart_read_char(SHL_USART, &c_key) != USART_SUCCESS);
   }
   printf("\r                 \r");
   return (c_key==' ');
}


//! @brief This function display all drives present
//!
void ushell_cmd_nb_drive( void )
{
   uint8_t u8_tmp;

   printf("Memory interface available:\r\n");
   for( u8_tmp=0; u8_tmp<nav_drive_nb(); u8_tmp++ )
   {
      // Display drive letter name (a, b...)
      printf("%c: %s\r\n", 'a'+u8_tmp, mem_name(u8_tmp) );
   }
}


//! @brief This function displays the free space of each drive present
//!
void ushell_cmd_free_space( void )
{
   uint8_t u8_tmp;
   Fs_index sav_index = nav_getindex();      // Save current position
   for( u8_tmp=0; u8_tmp<nav_drive_nb(); u8_tmp++ )
   {
      nav_drive_set( u8_tmp );      // Select drive
      if( !nav_partition_mount() )  // Mount drive
         continue;

      // Display drive letter name (a, b...)
      printf("%c: %s\r\n", 'a'+u8_tmp, mem_name(u8_tmp) );
      if( g_s_arg[0][0]=='l' )        // Choose command option
      {
         // Long and exact function
         printf("Free space: %llu Bytes / %llu Bytes\n\r",
                   (uint64_t)(nav_partition_freespace() << FS_SHIFT_B_TO_SECTOR),
                   (uint64_t)(nav_partition_space() << FS_SHIFT_B_TO_SECTOR));
      }
      else
      {
         // Otherwise use fast command
         printf("Free space: %u %%\n\r", nav_partition_freespace_percent() );
      }
   }
   nav_gotoindex(&sav_index);       // Restore position
}


//! @brief This function formats a drive
//!
void ushell_cmd_format( void )
{
   if( g_s_arg[0][0] == 0 )
      return;

   // Select drive to format
   nav_drive_set( g_s_arg[0][0]-'a');
   if( !nav_drive_format(FS_FORMAT_DEFAULT) )
   {
      fputs(MSG_ER_FORMAT, stdout);
      return;
   }
}


//! @brief This function mount a drive
//!
void ushell_cmd_mount( void )
{
   uint8_t u8_drive_lun;
   Fs_index sav_index;

   if( g_s_arg[0][0] == 0 )
      return;

   // Compute the logical unit number of drive
   u8_drive_lun=g_s_arg[0][0]-'a';
   // Check lun number
   if( u8_drive_lun >= nav_drive_nb() )
   {
      fputs(MSG_ER_DRIVE, stdout);
      return;
   }

   // Mount drive
   sav_index = nav_getindex();      // Save previous position
   if( nav_drive_set(u8_drive_lun))
   {
      if( nav_partition_mount() )
         return;                    // Here, drive mounted
   }
   fputs(MSG_ER_MOUNT, stdout);
   nav_gotoindex(&sav_index);       // Restore previous position
}


//! @brief This function displays the disk space of current drive
//!
void ushell_cmd_space( void )
{
   uint32_t u32_space;
   // Display drive letter name (a, b...)
   fputs( mem_name(nav_drive_get()), stdout);
   putchar(' ');
   putchar( nav_drive_get()+'a');
   // Otherwise use fast command
   u32_space = nav_partition_space();
   if( 1024 >(u32_space % (2*1024)) )
   {
      u32_space = u32_space/(2*1024);
   }else{
      u32_space = (u32_space/(2*1024))+1;
   }
   printf(": space: %luMB \n\r", u32_space );
}


//! @brief This function manages the ls command
//!
//! @param  b_more  enable the '|more' management when true otherwise no '|more' management
//!
void ushell_cmd_ls( bool b_more )
{
   uint8_t str_char[MAX_FILE_PATH_LENGTH];
   uint16_t u16_i,u16_nb_file,u16_nb_dir,last_i;
   uint8_t ext_filter=false;

   //** Print drive name
   printf("%c: volume is %s\r\n", 'a'+nav_drive_get(), mem_name(nav_drive_get()) );
   printf("Drive uses ");
   switch (nav_partition_type())
   {
      case FS_TYPE_FAT_12:
      printf("FAT12\n\r");
      break;

      case FS_TYPE_FAT_16:
      printf("FAT16\n\r");
      break;

      case FS_TYPE_FAT_32:
      printf("FAT32\n\r");
      break;

      default:
      printf("an unknown partition type\r\n");
      return;
   }

   //** Print directory name
   if( !nav_dir_name( (FS_STRING)str_char, MAX_FILE_PATH_LENGTH ) )
      return;
   printf("Dir name is %s\n\r",str_char);

   //** Check extension filter in extra parameters
   if(g_s_arg[0][0]!=0)
   {
      if(g_s_arg[0][0] == '*' && g_s_arg[0][1]=='.')
      {
         ext_filter=true;
         for(u16_i=2; u16_i<USHELL_SIZE_CMD_LINE; u16_i++)
         {
            g_s_arg[0][u16_i-2]=g_s_arg[0][u16_i];
         }
      }
   }

   //** Print files list
   printf("          Size  Name\n\r");
   // Init loop at the beginning of directory
   nav_filelist_reset();
   u16_nb_file=0;
   u16_nb_dir=0;
   last_i=0;
   // For each file in list
   while( nav_filelist_set(0,FS_FIND_NEXT) )
   {
      if(!ext_filter)
      {
         // No extension filter
         if( nav_file_isdir() )
         {
            printf("Dir ");
            u16_nb_dir++;              // count the number of directory
         }else{
            printf("    ");
         }
      }
      else
      {
         // If extension filter then ignore directories
         if(nav_file_isdir())
            continue;
         // Check extension
         if(!nav_file_checkext((FS_STRING)g_s_arg[0]))
            continue;
      }
      u16_nb_file++;                   // count the total of files (directories and files)

      // Check 'more' step
      if( b_more && ((u16_nb_file%USHELL_NB_LINE)==0) && (u16_nb_file!=0) && (last_i != u16_nb_file) )
      {
         last_i=u16_nb_file;
         if( !ushell_more_wait() )
            return;  // Exit LS command
      }

      // Display file
      nav_file_name((FS_STRING)str_char, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
      printf("%10lu  %s\n\r", nav_file_lgt(), str_char);
   }
   // Display total number
   printf(" %4i Files\r\n", u16_nb_file-u16_nb_dir );
   printf(" %4i Dir\r\n", u16_nb_dir );
}


//! @brief This function enter in a directory
//!
void ushell_cmd_cd( void )
{
   if( g_s_arg[0][0] == 0 )
      return;

   // Add '\' at the end of path, else the nav_setcwd select the directory but don't enter into.
   ushell_path_valid_syntac( g_s_arg[0] );

   // Call file system routine
   if( nav_setcwd((FS_STRING)g_s_arg[0],true,false) == false )
   {
      fputs(MSG_ER_UNKNOWN_FILE, stdout);
   }
}


//! @brief This function go back to parent directory
//!
void ushell_cmd_gotoparent( void )
{
   nav_dir_gotoparent();
}


//! @brief Manage cat command
//!
//! @param b_more   enable the '|more' management
//!
//! @todo more management not fully functional with file without CR
//!
void ushell_cmd_cat(bool b_more)
{
   char c_file_character;
   uint8_t n_line=0;

   if( g_s_arg[0][0] == 0 )
      return;

   // Select file
   if( !nav_setcwd((FS_STRING)g_s_arg[0],true,false) )
   {
      fputs(MSG_ER_UNKNOWN_FILE, stdout);
      return;
   }

   // Open file
   file_open(FOPEN_MODE_R);
   while (file_eof()==false)
   {
      // Check 'b_more' option
      if( b_more && (n_line >= USHELL_NB_LINE))
      {
         n_line = 0;
         if( !ushell_more_wait() )
            break;   // Stop cat command
      }

      // Display a character
      c_file_character = file_getc();
      putchar( c_file_character );

      // Count the line number
      if (c_file_character==ASCII_LF)
         n_line++;
   }
   file_close();

   // Jump in a new line
   putchar(ASCII_CR);putchar(ASCII_LF);
}


//! @brief This function display the help
//!
void ushell_cmd_help( void )
{
   fputs(MSG_HELP, stdout);
}


//! @brief This function create a directory
//!
void ushell_cmd_mkdir( void )
{
   if( g_s_arg[0][0] == 0 )
      return;

   if( !nav_dir_make((FS_STRING)g_s_arg[0]) )
      fputs(MSG_KO, stdout);
}


//! @brief This function create a file
//!
void ushell_cmd_touch( void )
{
   if( g_s_arg[0][0] == 0 )
      return;

   nav_file_create((FS_STRING)g_s_arg[0]);
}


//! @brief This function delete a file or directory
//!
void ushell_cmd_rm( void )
{
   uint8_t u8_i = 0;
   Fs_index sav_index;

   if( g_s_arg[0][0] == 0 )
      return;

   // Save the position
   sav_index = nav_getindex();

   while( 1 )
   {
      // Restore the position
      nav_gotoindex(&sav_index);
      // Select file or directory
      if( !nav_setcwd( (FS_STRING)g_s_arg[0], true, false ) )
         break;
      // Delete file or directory
      if( !nav_file_del( false ) )
      {
         fputs(MSG_KO, stdout);
         break;
      }
      u8_i++;
   }
   printf( "%u file(s) deleted\n\r", u8_i );
}


//! @brief Minimalist file editor to append char to a file
//!
//! @verbatim
//! hit ^q to exit and save file
//! @endverbatim
//!
void ushell_cmd_append_file( void )
{
   int c_key;

   if( g_s_arg[0][0] == 0 )
      return;

   // Select file or directory
   if( !nav_setcwd( (FS_STRING)g_s_arg[0], true, false ) )
   {
      fputs(MSG_ER_UNKNOWN_FILE, stdout);
      return;
   }
   // Open file
   if( !file_open(FOPEN_MODE_APPEND) )
   {
      fputs(MSG_KO, stdout);
      return;
   }

   // Append file
   fputs(MSG_APPEND_WELCOME, stdout);
   while( 1 )
   {
      usart_reset_status(SHL_USART);
      while(usart_read_char(SHL_USART, &c_key) != USART_SUCCESS);

      if( c_key == ASCII_CTRL_Q )
         break;   // ^q to quit

      putchar( c_key );
      file_putc( c_key );
      if( c_key == ASCII_CR )
      {
         putchar(ASCII_LF);
         file_putc(ASCII_LF);
      }
   }

   // Close file
   file_close();
   putchar(ASCII_CR);putchar(ASCII_LF);
}


//! @brief This function copies a file to other location
//!
void ushell_cmd_copy( void )
{
   Fs_index sav_index;
   uint8_t u8_status_copy;

   if( g_s_arg[0][0] == 0 )
      return;

   // Save the position
   sav_index = nav_getindex();

   // Select source file
   if( !nav_setcwd( (FS_STRING)g_s_arg[0], true, false ) )
   {
      fputs(MSG_ER_UNKNOWN_FILE, stdout);
      return;
   }
   // Get name of source to be used as same destination name
   nav_file_name( (FS_STRING)g_s_arg[0], MAX_FILE_PATH_LENGTH, FS_NAME_GET, true );
   // Mark this selected file like source file
   if( !nav_file_copy())
   {
      fputs(MSG_KO, stdout);
      goto cp_end;
   }

   // Select destination
   if( g_s_arg[1][0]==0 )
   {
      // g_s_arg[1] is NULL, using mark
      if( !nav_gotoindex(&g_mark_index) )
         goto cp_end;
   }
   else
   {
      // g_s_arg[1] exists, then go to this destination
      if( !nav_setcwd( (FS_STRING)g_s_arg[1], true, false ) )
      {
         fputs(MSG_ER_UNKNOWN_FILE, stdout);
         goto cp_end;
      }
   }

   // Set the name destination and start paste
   if( !nav_file_paste_start((FS_STRING)g_s_arg[0]) )
   {
      fputs(MSG_ER_PASTE, stdout);
      goto cp_end;
   }

   // Performs copy
   do
   {
      u8_status_copy = nav_file_paste_state( false );
   }while( u8_status_copy == COPY_BUSY );

   // Check status of copy action
   if( u8_status_copy == COPY_FAIL )
   {
      fputs(MSG_ER_PASTE, stdout);
      goto cp_end;
   }

cp_end:
   // Restore the position
   nav_gotoindex(&sav_index);
}


//! @brief This function renames a file or a directory
//!
void ushell_cmd_rename( void )
{
   if( g_s_arg[0][0] == 0 )
      return;
   if( g_s_arg[1][0] == 0 )
      return;

   // Select source file
   if( !nav_setcwd( (FS_STRING)g_s_arg[0], true, false ) )
   {
      fputs(MSG_ER_UNKNOWN_FILE, stdout);
      return;
   }
   // Rename file or directory
   if( !nav_file_rename( (FS_STRING)g_s_arg[1] ) )
   {
      fputs(MSG_KO, stdout);
      return;
   }
}


//! @brief Synchronize a path with an other path
//!
//! @return true if success
//!
bool ushell_cmd_sync( void )
{
   Fs_index sav_index;
   uint8_t u8_folder_level = 0;

   if( g_s_arg[0][0] == 0 )
      return false;
   if( g_s_arg[1][0] == 0 )
      return false;
   // Add '\' at the end of path, else the nav_setcwd select the directory but don't enter into.
   ushell_path_valid_syntac( g_s_arg[0] );
   ushell_path_valid_syntac( g_s_arg[1] );

   printf("Synchronize folders:\n\r");
   sav_index = nav_getindex();   // Save the position

   // Select source directory in COPYFILE navigator handle
   nav_select( FS_NAV_ID_COPYFILE );
   printf("Select source directory\n\r");
   if( !nav_setcwd( (FS_STRING)g_s_arg[0], true, false ) )
      goto ushell_cmd_sync_error;
   nav_filelist_reset();

   // Select destination directory in USHELL navigator handle
   nav_select( FS_NAV_ID_USHELL_CMD );
   printf("Select destination directory\n\r");
   if( !nav_setcwd( (FS_STRING)g_s_arg[1], true, true ) )
      goto ushell_cmd_sync_error;
   nav_filelist_reset();

   // loop to scan and create ALL folders and files
   while(1)
   {
      while(1)
      {
         // Loop to Search files or directories
         // Reselect Source
         nav_select( FS_NAV_ID_COPYFILE );
         if( nav_filelist_set( 0 , FS_FIND_NEXT ) )
            break;   // a next file and directory is found

         // No other dir or file in current dir then go to parent dir on Source and Destination disk
         if( 0 == u8_folder_level )
         {
            // end of update folder
            //********* END OF COPY **************
            goto ushell_cmd_sync_finish;
         }

         printf("Go to parent\n\r");
         // Remark, nav_dir_gotoparent() routine go to in parent dir and select the children dir in list
         u8_folder_level--;
         if( !nav_dir_gotoparent() )
            goto ushell_cmd_sync_error;
         // Select Destination navigator and go to the same dir of Source
         nav_select( FS_NAV_ID_USHELL_CMD );
         if( !nav_dir_gotoparent() )
            goto ushell_cmd_sync_error;
      } // end of while (1)

      if( nav_file_isdir())
      {
         printf("Dir found - create dir: ");
         //** here, a new directory is found and is selected
         // Get name of current selection (= dir name on Source)
         if( !nav_file_name( (FS_STRING)g_s_arg[0], USHELL_SIZE_CMD_LINE, FS_NAME_GET, false ))
            goto ushell_cmd_sync_error;
         // Enter in dir (on Source)
         if( !nav_dir_cd())
            goto ushell_cmd_sync_error;
         u8_folder_level++;
         // Select Destination disk
         nav_select( FS_NAV_ID_USHELL_CMD );
         // Create folder in Destination disk
         printf((char*)g_s_arg[0]);
         printf("\n\r");
         if( !nav_dir_make( (FS_STRING )g_s_arg[0] ))
         {
            if( FS_ERR_FILE_EXIST != fs_g_status )
               goto ushell_cmd_sync_error;
            // here, error the name exist
         }
         // Here the navigator have selected the folder on Destination
         if( !nav_dir_cd())
         {
            if( FS_ERR_NO_DIR == fs_g_status )
            {
               // FYC -> Copy impossible, because a file have the same name of folder
            }
            goto ushell_cmd_sync_error;
         }
         // here, the folder is created and the navigator is entered in this dir
      }
      else
      {
         printf("File found - copy file: ");
         //** here, a new file is found and is selected
         // Get name of current selection (= file name on Source)
         if( !nav_file_name( (FS_STRING)g_s_arg[0], USHELL_SIZE_CMD_LINE, FS_NAME_GET, false ))
            goto ushell_cmd_sync_error;
         printf((char*)g_s_arg[0]);
         printf("\n\r");
         if( !nav_file_copy())
            goto ushell_cmd_sync_error;

         // Paste file in current dir of Destination disk
         nav_select( FS_NAV_ID_USHELL_CMD );
         while( !nav_file_paste_start( (FS_STRING)g_s_arg[0] ) )
         {
            // Error
            if( fs_g_status != FS_ERR_FILE_EXIST )
               goto ushell_cmd_sync_error;
            // File exists then deletes this one
            printf("File exists then deletes this one.\n\r");
            if( !nav_file_del( true ) )
               goto ushell_cmd_sync_error;
            // here, retry PASTE
         }
         // Copy running
         {
         uint8_t status;
         do{
            status = nav_file_paste_state(false);
         }while( COPY_BUSY == status );

         if( COPY_FINISH != status )
            goto ushell_cmd_sync_error;
         }
      } // if dir OR file
   } // end of first while(1)

ushell_cmd_sync_error:
   // Restore the position
   nav_select( FS_NAV_ID_USHELL_CMD );
   nav_gotoindex(&sav_index);
   printf("!!!Copy fail\n\r");
   return false;

ushell_cmd_sync_finish:
   // Restore the position
   nav_select( FS_NAV_ID_USHELL_CMD );
   nav_gotoindex(&sav_index);
   printf("End of copy\n\r");
   return true;
}

// File alloc space (unit sector 512B)
#define  FILE_ALLOC_SIZE      ((1024*1024L)/512L)      // 1MB

Fs_file_segment ushell_cmd_perform_alloc( uint8_t lun, uint16_t size_alloc )
{
   const FS_STRING file_tmp_name = "tmp.bin";
   Fs_file_segment g_recorder_seg;
   g_recorder_seg.u16_size = 0;

   if( !nav_drive_set(lun))
      return g_recorder_seg;

   if( !nav_partition_mount() )
      return g_recorder_seg;

   if( !nav_file_create((FS_STRING)file_tmp_name))
   {
      if( FS_ERR_FILE_EXIST != fs_g_status)
         return g_recorder_seg;
      nav_file_del(false);
      if( !nav_file_create((FS_STRING)file_tmp_name))
         return g_recorder_seg;
   }
   // Open file
   if( !file_open(FOPEN_MODE_W) )
   {
      nav_file_del(false);
      return g_recorder_seg;
   }
   // Define the size of segment to alloc (unit 512B)
   // Note: you can alloc more in case of you don't know total size
   g_recorder_seg.u16_size = size_alloc;
   // Alloc in FAT a cluster list equal or inferior at segment size
   if( !file_write( &g_recorder_seg ))
   {
      g_recorder_seg.u16_size = 0;
      file_close();
      nav_file_del(false);
   }
   return g_recorder_seg;   //** File open and FAT allocated
}

void ushell_cmd_perform_transfer( Fs_file_segment seg_src, Fs_file_segment seg_dest )
{
   uint8_t id_trans_memtomem;
   Ctrl_status status_stream;
   uint16_t u16_i, u16_trans_max;
   uint32_t u32_tmp, u32_time;

   u16_trans_max = ( seg_src.u16_size < seg_dest.u16_size )?  seg_src.u16_size : seg_dest.u16_size;
   for( u16_i=2; u16_i<=u16_trans_max; u16_i*=10 )
   {
      u32_time = Get_sys_count();
      id_trans_memtomem = stream_mem_to_mem( seg_src.u8_lun , seg_src.u32_addr , seg_dest.u8_lun , seg_dest.u32_addr , u16_i );
      if( ID_STREAM_ERR == id_trans_memtomem )
      {
         printf( "Transfer error\r\n");
         return;
      }
      while(1)
      {
         status_stream = stream_state( id_trans_memtomem );
         if( CTRL_BUSY == status_stream ) continue;
         if( CTRL_GOOD == status_stream ) break;
         if( CTRL_FAIL == status_stream ) {
            printf( "Transfer error\r\n");
            return;
         }
      }
      u32_time = cpu_cy_2_us(Get_sys_count()-u32_time, g_u32_ushell_pba_hz );
      u32_tmp = ((uint32_t)u16_i*(1000000/2))/u32_time;
      printf( "Transfer rate %4luKB/s - stream size %4iKB\r\n", u32_tmp, u16_i/2 );
      if( (8000000) < u32_time )
      {
         // The test time must be inferior at 8s
         break;
      }
   }
}

void ushell_cmd_perform_access( bool b_sens_write, Fs_file_segment seg )
{
   uint16_t u16_trans;
   uint32_t u32_tmp, u32_time;

   fat_cache_flush();
   fat_cache_reset();
   u32_time = Get_sys_count();
   for( u16_trans=0; u16_trans<seg.u16_size; u16_trans++ )
   {
      if( b_sens_write )
      {
         if( CTRL_GOOD != ram_2_memory( seg.u8_lun , seg.u32_addr , fs_g_sector )) {
            printf( "Transfer error\r\n");
            return;
         }
      }else{
         if( CTRL_GOOD != memory_2_ram( seg.u8_lun , seg.u32_addr , fs_g_sector )) {
            printf( "Transfer error\r\n");
            return;
         }
      }
      seg.u32_addr++;
      if( 8000000 < cpu_cy_2_us(Get_sys_count()-u32_time, g_u32_ushell_pba_hz) )
      {
         // Stop access after 8s
         break;
      }
   }
   u32_time = cpu_cy_2_us(Get_sys_count()-u32_time, g_u32_ushell_pba_hz);
   u32_tmp = ((uint32_t)u16_trans*(1000000/2))/u32_time;
   if( b_sens_write )
      printf( "Transfer rate - WRITE %4luKB/s\r\n", u32_tmp );
   else
      printf( "Transfer rate - READ %4luKB/s\r\n", u32_tmp );
}

#define SIZE_OF_EXT_BUFFER 8
static   uint8_t  u8_ext_buffer[512*SIZE_OF_EXT_BUFFER];

void ushell_cmd_perform_extaccess( bool b_sens_write, Fs_file_segment seg )
{
   uint16_t u16_trans;
   uint32_t u32_tmp, u32_time;
   uint8_t  u8_nb_trans_usb=0;

   fat_cache_flush();
   fat_cache_reset();
   u32_time = Get_sys_count();
   u16_trans=0;
   while( seg.u16_size!=0 )
   {
      if( 0 == (seg.u32_addr % SIZE_OF_EXT_BUFFER) )
      {
         u8_nb_trans_usb = SIZE_OF_EXT_BUFFER;
      }else{
         u8_nb_trans_usb = SIZE_OF_EXT_BUFFER - (seg.u32_addr % SIZE_OF_EXT_BUFFER);  // to align access with usual memory mapping
      }
      if (u8_nb_trans_usb > seg.u16_size)
        u8_nb_trans_usb = seg.u16_size;

      if( b_sens_write )
      {
         if( CTRL_GOOD != host_write_10_extram( seg.u32_addr , u8_ext_buffer, u8_nb_trans_usb )) {
            printf( "Transfer error\r\n");
            return;
         }
      }else{
         if( CTRL_GOOD != host_read_10_extram( seg.u32_addr , u8_ext_buffer, u8_nb_trans_usb )) {
            printf( "Transfer error\r\n");
            return;
         }
      }
      seg.u16_size -= u8_nb_trans_usb;
      u16_trans    += u8_nb_trans_usb;
      seg.u32_addr += u8_nb_trans_usb;
      if( 8000000 < cpu_cy_2_us(Get_sys_count()-u32_time, g_u32_ushell_pba_hz) )
      {
         // Stop access after 8s
         break;
      }
   }
   u32_time = cpu_cy_2_us(Get_sys_count()-u32_time, g_u32_ushell_pba_hz);
   u32_tmp = ((uint32_t)u16_trans*(1000000/2))/u32_time;
   if( b_sens_write )
      printf( "Transfer rate - WRITE %4luKB/s\r\n", u32_tmp);
   else
      printf( "Transfer rate - READ %4luKB/s\r\n", u32_tmp );
}


//! @brief Perform transfer between two devices
//!
void ushell_cmd_perform( void )
{
   Fs_index sav_index;
   Fs_file_segment seg1, seg2;

   if( g_s_arg[0][0] == 0 )
      return;

   sav_index = nav_getindex();   // Save the position

   // Alloc a file on each devices
   printf("Alloc a file on each devices\n\r");
   seg1 = ushell_cmd_perform_alloc( (g_s_arg[0][0]-'a') , FILE_ALLOC_SIZE );
   if( seg1.u16_size == 0 )
   {
      printf("!!!Error allocation on device 1\n\r");
      // Restore the position
      nav_gotoindex(&sav_index);
      return;
   }
   if( g_s_arg[1][0] != 0 )
   {
      nav_select( FS_NAV_ID_COPYFILE );
      seg2 = ushell_cmd_perform_alloc( (g_s_arg[1][0]-'a') , FILE_ALLOC_SIZE );
      if( seg2.u16_size == 0 )
      {
         nav_select( FS_NAV_ID_USHELL_CMD );
         file_close();
         nav_file_del(false);
         printf("!!!Error allocation on device 2\n\r");
         // Restore the position
         nav_gotoindex(&sav_index);
         return;
      }

      // Transfer data from device 1 to device 2
      printf("Transfer data from device 1 to device 2\r\n");
      ushell_cmd_perform_transfer(seg1,seg2);
      printf("Transfer data from device 2 to device 1\r\n");
      ushell_cmd_perform_transfer(seg2,seg1);
      // Delete files allocated
      nav_select( FS_NAV_ID_COPYFILE );
      file_close();
      nav_file_del(false);
      nav_select( FS_NAV_ID_USHELL_CMD );
   }
   else
   {
      ushell_cmd_perform_access( false, seg1 );
      ushell_cmd_perform_access( true, seg1 );
      if( LUN_ID_USB <= nav_drive_get() )
      {
         printf("Transfer large buffer on USB\r\n");
         ushell_cmd_perform_extaccess( false, seg1 );
         ushell_cmd_perform_extaccess( true, seg1 );
      }

   }

   file_close();
   nav_file_del(false);
   // Restore the position
   nav_gotoindex(&sav_index);
   printf("End of test\n\r");
   return;
}




//! @brief Appends the '\' char at the end of path
//!
void ushell_path_valid_syntac( char *path )
{
   uint8_t u8_tmp;

   // Compute size of substitute
   for( u8_tmp=0; u8_tmp<MAX_FILE_PATH_LENGTH; u8_tmp++ )
   {
      if( path[u8_tmp]==0)
         break;
   }
   // Append the '\' char for the nav_setcwd to enter the found directory
   if ( path[u8_tmp-1] != '\\')
   {
      path[u8_tmp]='\\';
      path[u8_tmp+1]=0;
   }
}


#if USB_HOST_FEATURE == true
//! @brief In host mode, display basic low level information about the device connected
//!
//! @note The device should be supported by the host (configured)
//!
void ushell_cmdusb_ls(void)
{
   uint8_t i,j;

   // Check USB host status
   if( (!Is_host_ready()) && (!Is_host_suspended()) )
   {
      fputs(MSG_NO_DEVICE, stdout);
      return;
   }
   if( Is_host_suspended() )
   {
      fputs(MSG_USB_SUSPENDED, stdout);
   }

   printf("VID:%04X, PID:%04X, ",Get_VID(),Get_PID());
   printf("MaxPower is %imA, ",2*Get_maxpower());
   if (Is_device_self_powered())
   {  fputs(MSG_SELF_POWERED, stdout);}
   else
   {  fputs(MSG_BUS_POWERED, stdout); }
   if (Is_usb_full_speed_mode())
   {  fputs(MSG_DEVICE_FULL_SPEED, stdout);}
   else
#if (USB_HIGH_SPEED_SUPPORT==false)
   {  fputs(MSG_DEVICE_LOW_SPEED, stdout); }
#else
   {  fputs(MSG_DEVICE_HIGH_SPEED, stdout); }
#endif
   if (Is_device_supports_remote_wakeup())
   {  fputs(MSG_REMOTE_WAKEUP_OK, stdout);}
   else
   {  fputs(MSG_REMOTE_WAKEUP_KO, stdout); }
   printf("Supported interface(s):%02i\n\r",Get_nb_supported_interface());
   for(i=0;i<Get_nb_supported_interface();i++)
   {
      printf("Interface nb:%02i, AltS nb:%02i, Class:%02i, SubClass:%02i, Protocol:%02i\n\r",\
         Get_interface_number(i), Get_altset_nb(i), Get_class(i), Get_subclass(i), Get_protocol(i));
      printf(" Endpoint(s) Addr:");
      if(Get_nb_ep(i))
      {
         for(j=0;j<Get_nb_ep(i);j++)
         {
            printf(" %02lX", Get_ep_nbr(i,j));
         }
      }
      else
      {
         printf("None");
      }
      putchar(ASCII_CR);putchar(ASCII_LF);
   }
}

//! @brief In host mode, set host in suspend mode
//!
void ushell_cmdusb_suspend(void)
{
   if( !Is_host_ready() )
   {
      fputs(MSG_NO_DEVICE, stdout);
   }
   Host_request_suspend();
}

//! @brief In host mode, resume host from suspend mode
//!
void ushell_cmdusb_resume(void)
{
   if( !Is_host_suspended() )
   {
      fputs(MSG_NO_DEVICE, stdout);
   }
   Host_request_resume();
}

#endif  // USB_HOST_FEATURE == true
