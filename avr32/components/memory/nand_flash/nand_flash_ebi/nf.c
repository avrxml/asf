/*****************************************************************************
 *
 * \file
 *
 * \brief Nand Flash driver for AVR32 UC3.
 *
 * This file contains basic functions to interface a nandflash through the SMC.
 *
 *                       interface can be used.
 *
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"

//#define _TRACE_        (ENABLE)

//_____  I N C L U D E S ___________________________________________________
#define _NF_C_

#include "conf_nf.h"
#include "nf.h"
#include "gpio.h"
#include "smc.h"

#if( NF_BAD_CONFIG==(false) )


//_____ D E C L A R A T I O N ______________________________________________


#if (NF_AUTO_DETECT_2KB==false) && (NF_AUTO_DETECT_512B==false)
//! \brief Tests the Nand Flash configuration
//!
//! The function verifies that the connected NF is
//! properly declared in conf_nf.h.
//!
//! \param nb_dev number of device
//!
//! \return The number of device connected and corresponding
//! to NF identifiers.
//!
U8 nf_check_type( U8 nb_dev )
{
   U8 i_dev;

   //nf_init(        nb_dev, 0 );
#warning Update for full support.
   nf_reset_nands( nb_dev ); // Reset all the NF devices

   // Test NF configuration
   //
   for( i_dev=0 ; i_dev<nb_dev ; i_dev++ )
   {
      nf_select( i_dev );
      nf_wait_busy();
      nf_force_CE();
      nf_wr_cmd(NF_READ_ID_CMD);
      nf_wr_addr( 0 );
      if(( nf_rd_data()!=G_DEV_MAKER  )
      || ( nf_rd_data()!=G_DEV_ID     ))
      {
         Assert( false );
         return i_dev;
      }
      if( G_CE_LOW )
      {
         // Activate CE Low if needed. This config is static
         // and we supposed that it is no more deactivated in firmware.
         nf_force_CE();
      }
   }
   return nb_dev;
}
#endif


//! \brief Reset all the NF devices.
//!
//! \param nb_dev Number of device
//!
void nf_reset_nands( U8 nb_dev )
{
   U8 i_dev;
   for( i_dev=0 ; i_dev<nb_dev ; i_dev++ )
   {
      nf_select( i_dev );
      // The wait is mandatory here since the function is used to wait any
      // pending internal programming (Cache Program cmd).
      nf_wait_busy();
      nf_wr_cmd(NF_RESET_CMD);
      nf_wait_busy();
   }
}



//! \brief Initializes the Nand Flash Controller and low level driver.
//!
//! \param hsb_f_hz HSB frequency in Hertz
//!
void nf_init( U32 hsb_f_hz /*U8 nb_dev, U16 lpb*/ )
{
  static const gpio_map_t SMC_NF_EBI_GPIO_MAP =
  {
    {ATPASTE2(EBI_NANDOE,_PIN),ATPASTE2(EBI_NANDOE,_FUNCTION)},
    {ATPASTE2(EBI_NANDWE,_PIN),ATPASTE2(EBI_NANDWE,_FUNCTION)},
 };

  gpio_enable_module(SMC_NF_EBI_GPIO_MAP, sizeof(SMC_NF_EBI_GPIO_MAP) / sizeof(SMC_NF_EBI_GPIO_MAP[0]));
  smc_init(hsb_f_hz);

  // Enable Nand Flash Chip Select
  AVR32_HMATRIX.sfr[AVR32_EBI_HMATRIX_NR] |= (1 << AVR32_EBI_NAND_CS);

/*
   Nfc_disable();           // Reset the macro
   Nfc_enable();
   Nfc_ecc_disable();
   Nfc_spzone_disable();
#if (defined NF_TIMING_READ)
   Nfc_set_read_timing( NF_TIMING_READ );
#endif
*/
}



//! \brief returns the frequency supported by the selected NF.
//!
//! \return the frequency
//!
U8 nf_get_freq( void )
{
   #warning Update for full support.
   return 0;
   //return G_CLK_DFC_NFC;
}



//! \brief Check the status of the selected device.
//!
//! \return a status:
//!           PASS if the status is PASS;
//!           FAIL if the status is FAIL
//!
bool nf_check_status( void )
{
  U8 data;
  nf_wait_busy(); // Send a status command and wait the completion of the last command

  ecchrs_freeze(&AVR32_ECCHRS);
  data = nf_rd_data();
  ecchrs_unfreeze(&AVR32_ECCHRS);
  if ( (data&NF_MASK_STATUS_FAIL)==0 ) { return PASS; } // I/O 0   Pass:0  Fail:1
  else                                 { return FAIL; }
}

//! \brief Opens a page for read. The function will adapt the commands according to the
//! type of flash memory. The busy is polled at the end of the function.
//!
//! \param page_addr          absolute page address of the block
//! \param byte_addr          relative byte address inside the page.
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The NF device should have been selected before with <code>nf_select( id )</code>.
//!
void nf_open_page_read( U32 page_addr, U16 byte_addr)
{
   nf_wait_busy();
   Nf_open_page_read( page_addr, byte_addr);
}

//! \brief Opens a page for write. The function will adapt the commands according to the
//! type of flash memory.
//!
//! \param page_addr          absolute page address of the block
//! \param byte_addr          relative byte address inside the page.
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The NF device should have been selected before with <code>nf_select( id )</code>.
//!
void nf_open_page_write( U32 page_addr, U16 byte_addr)
{
   Nf_open_page_write( page_addr, byte_addr);
}

//! \brief Mark a block as 'invalid' by clearing it entirely.
//!
//! \param page_addr          absolute page address of the block
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The device which holds this bad block should have been selected
//! before with <code>nf_select( id )</code>.
//!
void nf_mark_bad_block(U32 page_addr)
{
   U8  n_bytes;
   U8  i_byte;
   U8  i_page;


   n_bytes= ( Is_nf_512() )
   ?  16  // 512B page access
   :  64  // 2KB  page access
   ;
   // Erasing the block is mandatory to prevent partial programming
   // (some 512B NF does support partial prog, but not after a copy back command).
   nf_erase_block( page_addr, true );
   for ( i_page=(U8)1<<G_SHIFT_BLOCK_PAGE ; i_page!=0 ; i_page--, page_addr++ )
   {
      nf_open_page_write( page_addr, NF_SPARE_POS-8 );
      nf_wr_data('A'); nf_wr_data('t');
      nf_wr_data('m'); nf_wr_data('e');
      nf_wr_data('l'); nf_wr_data(' ');
      nf_wr_data(' '); nf_wr_data(' ');
      for ( i_byte=n_bytes ; i_byte!=0 ; i_byte-=4 )
      {
         nf_wr_data(0);
         nf_wr_data(0);
         nf_wr_data(0);
         nf_wr_data(0);
      }
      nf_wr_cmd(NF_PAGE_PROGRAM_CMD); // Confirm programming
   }
}

//! \brief Erases a block.
//!
//! The erase will be done only if the block is not bad
//!
//! \param page_addr          absolute page address of the block
//! \param force_erase        true forces erasing, false erases the block (if not bad)
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The device which holds the block to delete should have been selected
//! before with <code>nf_select( id )</code>.
//!
void nf_erase_block( U32 page_addr, U8 force_erase )
{
   if( false==force_erase )
   {
      nf_open_page_read( page_addr, NF_SPARE_POS + G_OFST_BLK_STATUS );
      if( (nf_rd_data()!=0xFF) ) return; // The block is bad. We can not erase it
   }
   nf_wait_busy();
   nf_wr_cmd(NF_BLOCK_ERASE_CMD);          // Auto Block Erase Setup
   nf_wr_addr( LSB0(page_addr) );
   nf_wr_addr( LSB1(page_addr) );
   if ( 3==G_N_ROW_CYCLES )
   {
      nf_wr_addr( MSB1(page_addr) );
   }
   nf_wr_cmd(NF_BLOCK_ERASE_CONFIRM_CMD);      // Erase command
}

//! \brief Reads the number spare bytes specified and stores them in a array.
//!
//! \param p_byte             pointer on the array in which are stored the spare bytes.
//! \param n_byte             number of spare bytes to read.
//! \param page_addr          absolute page address of the block.
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The NF device should have been selected before with <code>nf_select( id )</code>.
//!
void nfc_read_spare_byte(
   U8 _MEM_TYPE_SLOW_ * p_byte
,  U8  n_byte
,  U32 page_addr)
{
   U8  i;

   nf_open_page_read( page_addr, NF_SPARE_POS);

   for ( i=0 ; i!=n_byte ; i++ )
      p_byte[i] = nf_rd_data();
}

//! \brief Tests the true busy. Note that we test twice the ready, since there is
//! an hardware minimum requirement between the end of the busy and the first
//! read cycle. Since the busy is not wired, the ready is tested twice.
//!
void nf_wait_busy( void )
{
   ecchrs_freeze(&AVR32_ECCHRS);
   nf_wr_cmd(NF_READ_STATUS_CMD);
   if( Is_nf_2k() )
   {
      if( G_CACHE_PROG )
      {
         while( (nf_rd_data() & NF_MASK_STATUS_T_RDY_2KB )==0 );
         while( (nf_rd_data() & NF_MASK_STATUS_T_RDY_2KB )==0 );
      }
      else
      {
         while( (nf_rd_data() & NF_MASK_STATUS_READY     )==0 );
         while( (nf_rd_data() & NF_MASK_STATUS_READY     )==0 );
      }
   }
   if( Is_nf_512() )
   {
      while( (nf_rd_data() & NF_MASK_STATUS_T_RDY_512B )==0 );
      while( (nf_rd_data() & NF_MASK_STATUS_T_RDY_512B )==0 );
   }
   ecchrs_unfreeze(&AVR32_ECCHRS);
}



//! \brief Read the ID of the Nand-Flash
//!
//! \param read_id_cmd  Read_id command (NF_READ_ID_CMD, NF_READ_ID2_CMD)
//! \param nf_num       Nand Flash number
//!
//! \return :
//!   MSB0(ret) (MSB) is the Maker Code,
//!   MSB1(ret) is the Device Id,
//!   MSB2(ret) is 3rd byte returned,
//!   MSB3(ret) (LSB) is 4th byte returned.
//!
//! \pre <code>nf_init()</code> should have been called before.
//!
U32 nf_read_id( U8 read_id_cmd, U8 nf_num )
{
   U32 ret;

   nf_select( nf_num );
   nf_wait_busy();
   //nf_force_CE();
   nf_wr_cmd (read_id_cmd);
   nf_wr_addr( 0 );

   MSB0(ret)= nf_rd_data(); // Maker Code
   MSB1(ret)= nf_rd_data(); // Device Id
   MSB2(ret)= nf_rd_data(); // extra
   MSB3(ret)= nf_rd_data(); // extra (Multi Plane Support)

   //Nfc_action( NFC_ACT_ASSERT_CE, NFC_EXT_NOP);
   return ret;
}


#if (NF_DETECTION_ID==ENABLE) || (NF_AUTO_DETECT_2KB==true) || (NF_AUTO_DETECT_512B==true)
//! \brief Check the status Ready/Busy of the Nand Flash
//! \return bool
//!    true   -> The Nand Flash is ready and connected
//!    false  -> The Nand Flash must be no connected (timeout)
//!
static bool nfc_nf_is_ready( void )
{
   U8 u8_timeout;
   U8 dummy;
   nf_wr_cmd( NF_READ_STATUS_CMD );  // send status for each read, because the NF must be in reset sequence
   dummy = Nfc_rd_status();     // active first read

   for (u8_timeout=NF_MAX_RB_TIMEOUT ; u8_timeout!=0 ; u8_timeout--)
   {
      if(( (Nfc_rd_status() & NF_MASK_STATUS_READY) !=0 )    // the busy pin is not tested, and the bit ready may be wrong penddind the rise of busy pin
      && ( (Nfc_rd_status() & NF_MASK_STATUS_READY) !=0 ) )  // To not read a wrong status, we check the status after 6 cycles (300ns)
      {
            return true;  // NF READY
      }
   }
   return false;          // TIMEOUT
}


//! \brief Read the ID of the Nand-Flash and update the global variable
//!
//! \return :
//!   nf index of listing "nf_listing"
//!   otherwise : NO_NF_CONNECTED or NF_UNKNOW
//!
U8  nfc_detect( void )
{
   U32   u32_nf_ids;
   U8    u8_i, u8_conf;

   // Init the Nand Flash Controller
   nf_init(        NF_MAX_DEVICES, 0 );
   nf_reset_nands( NF_MAX_DEVICES ); // Reset all the NF devices

   // Check the presence of device 0
   if ( false == nfc_nf_is_ready() )
      return NO_NF_CONNECTED;

   // Read the Nand Flash IDs of device 0
   u32_nf_ids = nf_read_id( NF_READ_ID_CMD, 0 );

   // Identify the Nand Flash (device 0)
   for( u8_i=0 ; u8_i < (sizeof(nf_list_id)/sizeof(St_nf_id)) ; u8_i++)
   {
      if((nf_list_id[u8_i].manuf == MSB0(u32_nf_ids))
      && (nf_list_id[u8_i].dev   == MSB1(u32_nf_ids)))
         break; // here, ID is know
   }
   if( u8_i == (sizeof(nf_list_id)/sizeof(St_nf_id)) )
      return NF_UNKNOW;

   // Set NF configuration parameters for initialisation and access
#if (NF_GENERIC_DRIVER==true)
#  error Test me...
   g_shift_page_byte    =;
   g_shift_block_page   =;
#endif

#if (NF_GENERIC_DRIVER==true) || (NF_AUTO_DETECT_2KB==true) ||(NF_AUTO_DETECT_512B==true)

   // Record info
   u8_conf     =  nf_list_id[u8_i].conf;
   g_dev_maker =  MSB0(u32_nf_ids); // Device maker
   g_dev_id    =  MSB1(u32_nf_ids); // Device ID

   // Search the number of block of device
   for( u8_i=0 ; u8_i < (sizeof(nf_list_link_id_block)/sizeof(St_nf_link_id_block)) ; u8_i++)
   {
      if( nf_list_link_id_block[u8_i].dev_id == g_dev_id )
         break; // ID found
   }
   if( u8_i == (sizeof(nf_list_link_id_block)/sizeof(St_nf_link_id_block)) )
      while(1);   // Error in NF definition

   g_n_zones            =  nf_list_link_id_block[u8_i].nb_zones;
#if (NF_AUTO_DETECT_2KB==true)
   if( 1 == g_n_zones )
      g_n_row_cycles    =  2;
   else
      g_n_row_cycles    =  3;
#endif
#if (NF_AUTO_DETECT_512B==true)
   if( 2 >= g_n_zones )
      g_n_row_cycles    =  2;
   else
      g_n_row_cycles    =  3;
#endif
   g_n_blocks           =  g_n_zones*1024L;

   g_copy_back_cont     = nf_list_conf[u8_conf].copy_back_cont   ;
   g_copy_back_discont  = nf_list_conf[u8_conf].copy_back_discont;
   g_cache_program      = nf_list_conf[u8_conf].cache_program    ;
   g_ce_low             = nf_list_conf[u8_conf].ce_low;
   g_clock_dfc_nfc      = (nf_list_conf[u8_conf].dfc_nfc_clock<<5) & MSK_DNFCKS;

   Nfc_set_read_timing((U8)nf_list_conf[u8_conf].timing_read );
   if( g_ce_low )
   {
      nf_force_CE();
   }
#endif
   return u8_i;
}

#endif

//! \brief Prepare a copy-back session
//!
//! \param page_addr          absolute source page address of the block
//!
//! \pre <code>nf_init()</code> should have been called before.
//!
void nf_copy_back_init( U32 page_addr )
{
   nf_wait_busy();
   nf_wr_cmd(NF_READ_CMD);
   nf_wr_addr( 0 );
   nf_wr_addr( 0 );
   nf_wr_addr( LSB0(page_addr) );
   nf_wr_addr( LSB1(page_addr) );
   if ( 3==G_N_ROW_CYCLES )
   {
      nf_wr_addr( MSB1(page_addr) );
   }
   nf_wr_cmd(NF_COPY_BACK_CMD);
   nf_wait_busy();
}

#if (0)
//! \brief Launch a copy-back session
//!
//! \param page_addr          absolute recipient page address of the block
//!
//! \pre <code>nf_init()</code> should have been called before.
//!
void nfc_copy_back_conf( U32 page_addr )
{
   nf_wait_busy();
   nf_wr_cmd(NF_RANDOM_DATA_INPUT_CMD);
   nf_wr_addr( 0 );
   nf_wr_addr( 0 );
   nf_wr_addr( LSB0(page_addr) );
   nf_wr_addr( LSB1(page_addr) );
   if ( 3==G_N_ROW_CYCLES )
   {
      nf_wr_addr( MSB1(page_addr) );
   }
   nf_wr_cmd(NF_PAGE_PROGRAM_CMD);
}
#endif

//! \brief unprotect all the flashes
//!
void nf_unprotect( void )
{
  gpio_set_gpio_pin(NF_WP_PIN);
}

//! \brief Protect all the flashes
//!
void nf_protect( void )
{
  U32 i_dev;

  // Wait until the end of any pending internal programming (Cache Program cmd).
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    nf_select( i_dev );
    nf_wait_busy();
  }
  gpio_clr_gpio_pin(NF_WP_PIN);
}

//! \briefDisplay the 2 first pages of a block.
//!
//! \param block_addr          Physical block address
//! \param dev_id              Device number
//!
//! \pre <code>nf_init()</code> should have been called before.
//!
#if 0
void nfc_print_block(U16 block_addr, U8 dev_id)
{
   _MEM_TYPE_SLOW_ U32 page_addr=(U32)block_addr*((U8)1<<G_SHIFT_BLOCK_PAGE);
   _MEM_TYPE_SLOW_ U16 n_bytes;
   _MEM_TYPE_SLOW_ U16 i_byte;
   _MEM_TYPE_SLOW_ U8  i_page;

   trace("\n\rDisplay block 0x");
   trace_hex( MSB(block_addr) );
   trace_hex( LSB(block_addr) );

   n_bytes= ( Is_nf_512() )
   ?  512   // 512B page access
   :  2048  // 2KB  page access
   ;
   nf_select( dev_id );
   //for ( i_page=(U8)1<<G_SHIFT_BLOCK_PAGE ; i_page!=0 ; i_page--, page_addr++ )
   for( i_page=0 ; i_page<64 ; i_page++, page_addr++ )
   {
      trace("\n\rOpening page 0x");
      trace_hex( MSB0(page_addr) );
      trace_hex( MSB1(page_addr) );
      trace_hex( MSB2(page_addr) );
      trace_hex( MSB3(page_addr) );
      #if 0
      nf_open_page_read( page_addr, 0 );
      for( i_byte=0 ; i_byte<n_bytes ; )
      {
         if( !(i_byte%32) )
         {
            trace("\n\r0x");
            trace_hex( MSB(i_byte) );
            trace_hex( LSB(i_byte) );
            trace(" 0x");
         }
         else if( !(i_byte%16) ) trace("   ");
         else if( !(i_byte% 8) ) trace(" ");
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
         i_byte+=4;
      }
      #else
      nf_open_page_read( page_addr, n_bytes );
      #endif
      trace("\n\rSpare zone: 0x");
      for( i_byte=4*4 ; i_byte!=0 ; i_byte-- )
      { // discard spare zone
         if( i_byte%4==0 ) trace_nl();
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
         trace_hex( nf_rd_data() );
      }
      trace("\n\r");
   }
   trace("\n\rOther way to access spare zone: 0x");
   page_addr=(U32)block_addr*((U8)1<<G_SHIFT_BLOCK_PAGE);
   nf_open_page_read( page_addr, NF_SPARE_POS );
   i_byte=Nfc_rd_data_fetch_next();
   {
      for ( i_byte=4*4 ; i_byte!=0 ; i_byte-- )
      { // discard spare zone
         trace_hex( Nfc_rd_data_fetch_next() );
         trace_hex( Nfc_rd_data_fetch_next() );
         trace_hex( Nfc_rd_data_fetch_next() );
         trace_hex( Nfc_rd_data_fetch_next() );
      }
      trace("\n\r");
   }
}

#endif // NF_BAD_CONFIG

#endif // _NF_C_
