/*****************************************************************************
 *
 * \file
 *
 * \brief NAND FLASH low-level macros, definition, types and information in
 *        regard to manufacturer IDs.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _NF_H_
#define _NF_H_

/**
 * \defgroup group_avr32_components_memory_nand_flash_nand_flash_ebi MEMORY - NAND Flash on EBI
 *
 * Driver for parallel NAND flash connected to the External Bus Interface (EBI).
 * Provides functions for initializing and accessing a NAND flash.
 *
 * \{
 */

//_____ I N C L U D E S ____________________________________________________

#include "conf_nf.h"
#include "board.h"
#include "gpio.h"
#include "ecchrs.h"
#include "nf.h"

#undef _GLOBEXT_
#if (defined _NF_DRV_C_)
#  define _GLOBEXT_
#else
#  define _GLOBEXT_ extern
#endif


//_____ M A C R O S ________________________________________________________

#define AVR32_EBI_NAND_FLASH_ADDRESS  AVR32_EBI_CS3_ADDRESS

/*! \brief Read a data byte from the Nand Flash. */
__always_inline static U8 nf_rd_data( void )
{
  volatile unsigned char *p_data  = ((volatile unsigned char *)AVR32_EBI_NAND_FLASH_ADDRESS);
  return *p_data;
}

/*! \brief Write a data byte to the Nand Flash. */
__always_inline static void nf_wr_data( U8 data )
{
  volatile unsigned char *p_data  = ((volatile unsigned char *)AVR32_EBI_NAND_FLASH_ADDRESS);
  *p_data=data;
}

/*! \brief Ensure that both CLE and ALE are not asserted. */
__always_inline static void nf_send_nop( void )
{
  //volatile unsigned char *p_nop  =  (volatile unsigned char *)AVR32_EBI_NAND_FLASH_ADDRESS;
  volatile unsigned char *p_nop  =  (volatile unsigned char *)AVR32_EBI_CS0_ADDRESS;
  *p_nop = 0;
}

/*! \brief Send an address cycle on the last selected Nand Flash.
 *
 * \param addr address
 */
__always_inline static void nf_wr_addr(U8 addr)
{
  volatile int A;
  volatile unsigned char *p_ale  = ((volatile unsigned char *)AVR32_EBI_NAND_FLASH_ADDRESS + 0x00400000);
  A=0;            // If a access at AVR32_EBI_CS0_ADDRESS is done just BEFORE ALE address then the ALE is low in same time of WE rise (ALE hold time ~5ns)
  *p_ale = addr;
  A=0;            // If a access at AVR32_EBI_CS0_ADDRESS is done just AFTER ALE address then the ALE is low in same time of WE rise (ALE hold time ~5ns)
  nf_send_nop();
}

/*! \brief Send a command cycle on the last selected Nand Flash.
 *
 * \param cmd command.
 */
__always_inline static void nf_wr_cmd(U8 cmd)
{
  volatile int A;
  volatile unsigned char *p_cle  = ((volatile unsigned char *)AVR32_EBI_NAND_FLASH_ADDRESS + 0x00200000);
  A=0;            // If an access at AVR32_EBI_CS0_ADDRESS is done just BEFORE CLE address then the CLE is low in same time of WE rise (CLE hold time ~5ns)
  *p_cle = cmd;
  A=0;            // If an access at AVR32_EBI_CS0_ADDRESS is done just AFTER CLE address then the CLE is low in same time of WE rise (CLE hold time ~5ns)
  nf_send_nop();
}

/*! \brief Send a command cycle on the last selected Nand Flash.
 *
 * \param dev Device number.
 */
__always_inline static void nf_select(U8 dev)
{
  if( dev==0 )
  {
#if (NF_N_DEVICES==2) || (NF_N_DEVICES==4)
    // Release all other CS
    gpio_set_gpio_pin(NF_DEV_1_CS_PIN);
# if (NF_N_DEVICES==4)
    gpio_set_gpio_pin(NF_DEV_2_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_3_CS_PIN);
# endif
#endif
    // Select device
    gpio_clr_gpio_pin(NF_DEV_0_CS_PIN);
  }
#if (NF_N_DEVICES==2) || (NF_N_DEVICES==4)
  else if( dev==1 )
  {
    // Release all other CS
    gpio_set_gpio_pin(NF_DEV_0_CS_PIN);
# if (NF_N_DEVICES==4)
    gpio_set_gpio_pin(NF_DEV_2_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_3_CS_PIN);
# endif
    // Select device
    gpio_clr_gpio_pin(NF_DEV_1_CS_PIN);
  }
# if (NF_N_DEVICES==4)
  else if( dev==2 )
  {
    // Release all other CS
    gpio_set_gpio_pin(NF_DEV_0_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_1_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_3_CS_PIN);

    // Select device
    gpio_clr_gpio_pin(NF_DEV_2_CS_PIN);
  }
  else if( dev==3 )
  {
    // Release all other CS
    gpio_set_gpio_pin(NF_DEV_0_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_1_CS_PIN);
    gpio_set_gpio_pin(NF_DEV_2_CS_PIN);

    // Select device
    gpio_clr_gpio_pin(NF_DEV_3_CS_PIN);
  }
# endif
#endif
}

#warning Update for full NF support.
#define nf_force_CE()
#define nf_unforce_CE()



//! Resets the configuration state machine of the NFC.
//#define Nfc_reset_config()    ( ACC = NFCFG )

// ooo ooo                            o
//  o   o
//  o   o
//  o   o   oooo   ooo oo  oo oo    ooo    oo oo    oooooo
//  o o o       o    oo  o  oo  o     o     oo  o  o    o
//  o o o   ooooo    o      o   o     o     o   o  o    o
//  o o o  o    o    o      o   o     o     o   o  o    o
//   o o   o    o    o      o   o     o     o   o   ooooo
//   o o    oooo o ooooo   ooo ooo  ooooo  ooo ooo      o
//                                                      o
//                                                  oooo
//
// Please note that auto-detect mode (NF_AUTO_DETECT_2KB=true or
// NF_AUTO_DETECT_512B=true) may give different performances
// than static compilation (#define NF_TYPE_...)
//

//! Nand Flash Maker definition
#define  M_ID_MICRON       0x2C
#define  M_ID_HYNIX        0xAD
#define  M_ID_SANDISK      0x98
#define  M_ID_ST           0x20
#define  M_ID_TOSHIBA      0x98
#define  M_ID_TOSHIBA96    0x96
#define  M_ID_SAMSUNG      0xEC
#define  M_ID_INFINEON     0xC1

// Structs used in generic mode

// Device Indentifiant
typedef struct {
   U8  manuf;
   U8  dev;
   U8  conf;
} St_nf_id;

// Table to link Device Indentifiant and Number of block
typedef struct {
   U8  dev_id;
   U8  nb_zones;     // zone = 1024 blocks
}St_nf_link_id_block;

// Device Configuration
typedef struct {
   U8  copy_back_cont   :4;
   U8  copy_back_discont:4;
   U8  cache_program    :1;
   U8  ce_low           :1;
   U8  timing_read      :1;
   U8  dfc_nfc_clock    :5;
} St_nf_conf;


//-- Pre Check configuration
//
#define NF_BAD_CONFIG 	         false
#define NF_GENERIC_DRIVER        false // true configuration is not possible
#define NF_SHIFT_SECTOR_BYTE     9     // (1<<n) size of sector, unit in bytes (other value not supported)

#if (!defined NF_AUTO_DETECT_2KB)
#  define   NF_AUTO_DETECT_2KB   false
#endif
#if (!defined NF_AUTO_DETECT_512B)
#  define   NF_AUTO_DETECT_512B  false
#endif
#if (NF_AUTO_DETECT_2KB==true) && (NF_AUTO_DETECT_512B==true)
#  undef  NF_BAD_CONFIG
#  define NF_BAD_CONFIG (true)
#  error NF_AUTO_DETECT_2KB and NF_AUTO_DETECT_512B can not be activated at the same time.
#endif

#include "nf_512B.h"
#include "nf_2KB.h"

//-- Final Check configuration
//
#if (NF_AUTO_DETECT_2KB==true) && (NF_TYPE_512B_KNOWN==true)
#  undef  NF_BAD_CONFIG
#  define NF_BAD_CONFIG (true)
#  error NF_AUTO_DETECT_2KB and NF_TYPE_x 512B can not be activated at the same time.
#endif
#if (NF_AUTO_DETECT_512B==true) && (NF_TYPE_2KB_KNOWN==true)
#  undef  NF_BAD_CONFIG
#  define NF_BAD_CONFIG (true)
#  error NF_AUTO_DETECT_512B and NF_TYPE_x 2KB can not be activated at the same time.
#endif
#if (NF_AUTO_DETECT_2KB==false) && (NF_AUTO_DETECT_512B==false)
#  define NF_DETECTION_ID  (DISABLE)
#  if (NF_TYPE_2KB_KNOWN ==false) && (NF_TYPE_512B_KNOWN ==false)
#     undef  NF_BAD_CONFIG
#     define NF_BAD_CONFIG (true)
#     error No NandFlash configuration found (NF_AUTO_DETECT_2KB, NF_AUTO_DETECT_512B, NF_TYPE_x)
#  endif
#else
#  define NF_DETECTION_ID  (ENABLE)
#endif




// NFCON management
//
#if 0

//! \brief Set the read timing off the NFC.
//!
//! \param 0   Timing is [1,5 ; 0.5],
//!        1   Timing is [1 ; 1].
#define Nfc_set_read_timing(x)          (NFCON = (NFCON & ~MSK_NFCON_TRS) | ((x<<4)&MSK_NFCON_TRS))

#define Nfc_spzone_enable()             (NFCON |=  MSK_NFCON_SPZEN) //!< Enables the spare zone management
#define Nfc_spzone_disable()            (NFCON &= ~MSK_NFCON_SPZEN) //!< Disables the spare zone management

#define Nfc_ecc_enable()                (NFCON |=  MSK_NFCON_ECCEN) //!< Enables the ECC management
#define Nfc_ecc_disable()               (NFCON &= ~MSK_NFCON_ECCEN) //!< Disables the ECC management

#define Nfc_enable()                    (NFCON |=  MSK_NFCON_EN)    //!< Enables the NFC
#define Nfc_disable()                   (NFCON &= ~MSK_NFCON_EN)    //!< Disables the NFC

// NFERR management
//
#define Nfc_get_err()                   (NFERR)          //!< Returns the error descriptor
#define SHERRID_OK              0
#define SHERRID_CORRECTABLE     4
#define SHERRID_UNCORRECTABLE   8
#define SHERRID_BAD_CRC       0xC
#define Nfc_sherr(x)            ((x)&0x0C)    // Second half group error id
#define Nfc_shfb(x)             (((x)>>3) &7) // Second half group fail bit

#define FHERRID_OK              0
#define FHERRID_CORRECTABLE     1
#define FHERRID_UNCORRECTABLE   2
#define FHERRID_BAD_CRC         3
#define Nfc_fherr(x)            ((x)&0x03)  // First  half group error id
#define Nfc_fhfb(x)             ((x)&7   )  // First  half group fail bit

//! @defgroup nfc_actions NFC Actions
//! List of the actions that can be performed by the NFC.
//! @{
#define NFC_ACT_NOP        0 //!< No Operation.
#define NFC_ACT_DEV_SELECT 1 //!< Device selection. The number is given by the extension.
#define NFC_ACT_READ       2 //!< Read flow through DFC.
#define NFC_ACT_WRITE      3 //!< Write flow through DFC.
#define NFC_ACT_ASSERT_CE  4 //!< Assert the CE pad of the last selected device.
#   define NFC_EXT_CELOW     1
#   define NFC_EXT_NOP       0
#define NFC_ACT_STOP       5 //!< Stop the NFC, or the DFC flow.
#define NFC_ACT_ADC_EXT    6 //!< Column address extension. The extension gives A9:8.
//! @}

//! Sends an "action" to the NFC.
#define Nfc_action(act, ext)      \
   ( NFACT =                      \
      ((ext)<<3) & MSK_NFACT_EXT  \
   |  (act)      & MSK_NFACT_ACT )


// NFDAT, NFDATF, NFUDAT management
//
#define Nfc_wr_data(data)                 (NFDAT = (data) ) //!< Writes a data
#define Nfc_rd_data()                     (NFDAT          ) //!< Reads a data
#define Nfc_rd_data_fetch_next()          (NFDATF         ) //!< Reads a data and fetch the next one
#define Nfc_rd_status()                   (NFADC          ) //!< Reads a data and fetch the next one. BP and ECC does not change.



// NFSTA management
//
#define Is_nfc_running()                  ( NFC_RUN   ) //!< returns true if the NFC is running
#define Is_nfc_smc_locked()               ( NFC_SMLCK ) //!< returns true if the SMC is locked
#define Is_nfc_smc_detected()             ( NFC_SMCD  ) //!< returns true if the SMC is detected
#define Is_nfc_end_of_page()              ( NFC_EOP   ) //!< Returns true if the NFC reaches the end of the page


// NFECC management
//
#define Nfc_get_ecc()                     (NFECC  ) //!< Returns the ECC descriptor
#define Nfc_init_ecc()                    (NFECC=0) //!< Initializes the ECC descriptor and engine
#define Nfc_get_n_ecc()                   ((NFSTA & MSK_NFSTA_NECC)>>1  ) //!< Returns the number of ECC descriptor
#define Nfc_get_bpos()                    (((U16)NFBPH<<8) | NFBPL)           //!< Returns the byte position



// Interrupt management
//
#define Is_nfc_it_stop()                  (NFINT & MSK_NFINT_STOPI  )   //!< Returns true if the NFC did stop
#define Is_nfc_it_ecc_err()               (NFINT & MSK_NFINT_ECCERRI)   //!< Returns true if an ECC error is triggered
#define Is_nfc_it_ecc_rdy()               (NFINT & MSK_NFINT_ECCRDYI)   //!< Returns true if the ECC is ready
#define Is_nfc_it_illegal()               (NFINT & MSK_NFINT_ILGLI  )   //!< Returns true if an illegal operation is triggered
#define Is_nfc_it_smc_transtion()         (NFINT & MSK_NFINT_SMCTI  )   //!< Returns true if the SMC state did change (insert/remove)

// ANL is fully supported by the NFC
#define Nfc_clear_it_all()                (NFINT  = 0                 ) //!< Clears all the interrupts
#define Nfc_set_it_stop()                 (NFINT |=  MSK_NFINT_STOPI  ) //!< Set the Stop interrupt
#define Nfc_clear_it_stop()               (NFINT &= ~MSK_NFINT_STOPI  ) //!< Clears the Stop interrupt
#define Nfc_clear_it_ecc_err()            (NFINT &= ~MSK_NFINT_ECCERRI) //!< Clears the ECC error interrupt
#define Nfc_clear_it_ecc_rdy()            (NFINT &= ~MSK_NFINT_ECCRDYI) //!< Clears the ECC ready interrupt
#define Nfc_clear_it_illegal()            (NFINT &= ~MSK_NFINT_ILGLI  ) //!< Clears the Illegal interrupt
#define Nfc_clear_it_smc_transtion()      (NFINT &= ~MSK_NFINT_SMCTI  ) //!< Clears the SMC transition interrupt

// ORL on a register that is composed of control bits is fully supported !
#define Nfc_enable_it_stop()              (NFIEN |=  MSK_NFIEN_STOPE  ) //!< Enables the Stop interrupt
#define Nfc_enable_it_ecc_err()           (NFIEN |=  MSK_NFIEN_ECCERRE) //!< Enables the ECC error interrupt
#define Nfc_enable_it_ecc_rdy()           (NFIEN |=  MSK_NFIEN_ECCRDYE) //!< Enables the ECC ready interrupt
#define Nfc_enable_it_illegal()           (NFIEN |=  MSK_NFIEN_ILGLE  ) //!< Enables the Illegal interrupt
#define Nfc_enable_it_smc_transtion()     (NFIEN |=  MSK_NFIEN_SMCTE  ) //!< Enables the SMC transition interrupt

// ANL on a register that is composed of control bits is fully supported !
#define Nfc_disable_it_all()              (NFIEN  = 0                 ) //!< Disables the interrupts
#define Nfc_disable_it_stop()             (NFIEN &= ~MSK_NFIEN_STOPE  ) //!< Disables the Stop interrupt
#define Nfc_disable_it_ecc_err()          (NFIEN &= ~MSK_NFIEN_ECCERRE) //!< Disables the ECC error interrupt
#define Nfc_disable_it_ecc_rdy()          (NFIEN &= ~MSK_NFIEN_ECCRDYE) //!< Disables the ECC ready interrupt
#define Nfc_disable_it_illegal()          (NFIEN &= ~MSK_NFIEN_ILGLE  ) //!< Disables the Illegal interrupt
#define Nfc_disable_it_smc_transtion()    (NFIEN &= ~MSK_NFIEN_SMCTE  ) //!< Disables the SMC transition interrupt



//! In this mode, the spare zone is not managed by the NFC.
//!
#  define Nfc_mode_manual()          ( Nfc_spzone_disable(), Nfc_ecc_disable(), Nfc_disable_it_ecc_rdy() )

//! In this mode, the spare zone is entirely managed by the NFC.
//!
#  define Nfc_mode_auto()            ( Nfc_spzone_enable() , Nfc_ecc_enable() , Nfc_disable_it_ecc_rdy() )

//! In this mode, the spare zone is not automatically managed by the NFC.
//! However, an interrupt is triggered when the ECC FIFO is full, so after
//! each 512 bytes processed. The user must program/verify the spare zone.
//!
#  define Nfc_mode_ecc_manual()      ( Nfc_spzone_disable(), Nfc_ecc_enable() , Nfc_enable_it_ecc_rdy()  )

#endif



//! \brief  Opens a page for read. The macro will adapt the commands according to the
//! type of flash memory. The busy is polled at the end of the function.
//!
//! \param page_addr          absolute page address of the block
//! \param byte_addr          relative byte address inside the page.
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The NF device should have been selected before with <code>Nfc_action(NFC_ACT_DEV_SELECT, id)</code>.
//!
#define Nf_open_page_read( page_addr, byte_addr)           \
   if ( Is_nf_512() ) /* 512B pages */                     \
   {                                                       \
      if ( (byte_addr)<256 )                               \
      {                                                    \
         nf_wr_cmd(NF_READ_A_AREA_CMD);                    \
         nf_wr_addr( LSB( byte_addr) );                    \
         nf_wr_addr( LSB0(page_addr) );                    \
         nf_force_CE();                                    \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
         nf_wait_busy();                                   \
         if(G_CE_LOW)                                      \
         {  nf_unforce_CE();}                              \
      }                                                    \
      else if ( (byte_addr)<512 )                          \
      {                                                    \
         nf_wr_cmd(NF_READ_B_AREA_CMD);                    \
         nf_wr_addr( LSB( byte_addr) ); /* col */          \
         nf_wr_addr( LSB0(page_addr) ); /* row */          \
         nf_force_CE();                                    \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
         nf_wait_busy();                                   \
         if(G_CE_LOW)                                      \
         {  nf_unforce_CE();}                              \
      }                                                    \
      else                                                 \
      {                                                    \
         nf_wr_cmd(NF_READ_C_AREA_CMD);                    \
         nf_wr_addr( LSB( byte_addr) ); /* col */          \
         nf_wr_addr( LSB0(page_addr) ); /* row */          \
         nf_force_CE();                                    \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
         nf_wait_busy();                                   \
         if(G_CE_LOW)                                      \
         {  nf_unforce_CE();}                              \
      }                                                    \
   }                                                       \
   if ( Is_nf_2k() ) /* 2KB pages */                       \
   {                                                       \
      nf_wr_cmd(NF_READ_CMD);                              \
      nf_wr_addr( LSB( byte_addr) );                       \
      nf_wr_addr( MSB( byte_addr) );                       \
      nf_wr_addr( LSB0(page_addr) );                       \
      nf_wr_addr( LSB1(page_addr) );                       \
      if ( 3==G_N_ROW_CYCLES )                             \
      {                                                    \
         nf_wr_addr( MSB1(page_addr) );                    \
      }                                                    \
      nf_wr_cmd(NF_READ_CMD2);                             \
      nf_wait_busy();                                      \
   }                                                       \
   nf_wr_cmd(NF_READ_CMD); /* Back to the read mode */



//! \brief Opens a page for write. The macro will adapt the commands according to the
//! type of flash memory.
//!
//! \param page_addr          absolute page address of the block
//! \param byte_addr          relative byte address inside the page.
//!
//! \pre <code>nf_init()</code> should have been called before.
//! The NF device should have been selected before with <code>Nfc_action(NFC_ACT_DEV_SELECT, id)</code>.
//!
#define Nf_open_page_write( page_addr, byte_addr)          \
   Nf_wait_cache_busy();                                   \
                                                           \
   if ( Is_nf_512() ) /* 512B pages */                     \
   {                                                       \
      if ( (byte_addr)<256 )                               \
      {                                                    \
         nf_wr_cmd(NF_READ_A_AREA_CMD);                    \
         nf_wr_cmd(NF_SEQUENTIAL_DATA_INPUT_CMD);          \
         nf_wr_addr( LSB( byte_addr) );                    \
         nf_wr_addr( LSB0(page_addr) );                    \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
      }                                                    \
      else if ( (byte_addr)<512 )                          \
      {                                                    \
         nf_wr_cmd(NF_READ_B_AREA_CMD);                    \
         nf_wr_cmd(NF_SEQUENTIAL_DATA_INPUT_CMD);          \
         nf_wr_addr( LSB( byte_addr) ); /* col */          \
         nf_wr_addr( LSB0(page_addr) ); /* row */          \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
      }                                                    \
      else                                                 \
      {                                                    \
         nf_wr_cmd(NF_READ_C_AREA_CMD);                    \
         nf_wr_cmd(NF_SEQUENTIAL_DATA_INPUT_CMD);          \
         nf_wr_addr( LSB( byte_addr) ); /* col */          \
         nf_wr_addr( LSB0(page_addr) ); /* row */          \
         nf_wr_addr( LSB1(page_addr) );                    \
         if ( 3==G_N_ROW_CYCLES )                          \
         {                                                 \
            nf_wr_addr( MSB1(page_addr) );                 \
         }                                                 \
      }                                                    \
   }                                                       \
   if ( Is_nf_2k() ) /* 2KB pages */                       \
   {                                                       \
      nf_wr_cmd(NF_SEQUENTIAL_DATA_INPUT_CMD);             \
      nf_wr_addr( LSB( byte_addr) );                       \
      nf_wr_addr( MSB( byte_addr) );                       \
      nf_wr_addr( LSB0(page_addr) );                       \
      nf_wr_addr( LSB1(page_addr) );                       \
      if ( 3==G_N_ROW_CYCLES )                             \
      {                                                    \
         nf_wr_addr( MSB1(page_addr) );                    \
      }                                                    \
   }

//_____ D E C L A R A T I O N ______________________________________________
//

#define  NF_MAX_DEVICES    4

//! Define the maximum timeout of the Nand Flash
// max time reset (10us)
// max time for Data Transfer from Cell to Register (MLC=50us)
// Convert the timing (us) to a number of loop test for the nfc_nf_is_ready function
// formula = coef security * (maximun timeout / timing of one loop at 30MHz x2)
//#define  NF_MAX_RB_TIMEOUT    (2 * (50/1))    // 240us with 12MHz in mode X2


//! @defgroup nfc_flashes_commands Flash-memories commands
//! Set of commands of the FLASH memories (NF, SMC, ...)
//! @{
#define NF_READ_A_AREA_CMD            0x00 //!< Read Command of zone A (512B)
#define NF_READ_B_AREA_CMD            0x01 //!< Read Command of zone B (512B)
#define NF_READ_C_AREA_CMD            0x50 //!< Read Command of zone C (spare) (512B)

#define NF_READ_CMD                   0x00 //!< Read Command (2KB)
#define NF_READ_CMD2                  0x30 //!< Confirm read Command (2KB)
#define NF_RANDOM_READ_CMD_C1         0x05 //!< Random read Command (2KB)
#define NF_RANDOM_READ_CMD_C2         0xE0 //!< Confirm random read Command (2KB)

#define NF_READ_ID_CMD                0x90 //!< Read ID Command
#define NF_READ_ID2_CMD               0x91 //!< Read ID 2 Command

#define NF_RESET_CMD                  0xff //!< Reset command

#define NF_SEQUENTIAL_DATA_INPUT_CMD  0x80 //!< Sequential data input command
#define NF_PAGE_PROGRAM_CMD           0x10 //!< Page Program command

#define NF_RANDOM_DATA_INPUT_CMD      0x85 //!< Random data input command (2KB)
#define NF_COPY_BACK_CMD              0x35 //!< Copy-back command (2KB)
#define NF_CACHE_PROGRAM_CMD          0x15 //!< Cache program (fast) command (2KB)

#define NF_BLOCK_ERASE_CMD            0x60 //!< Erase command
#define NF_BLOCK_ERASE_CONFIRM_CMD    0xD0 //!< Confirm erase command

#define NF_READ_STATUS_CMD            0x70 //!< Read Status command
//! @}



//! @defgroup nfc_mask_status Flash-memories read-status bits
//! Masks of the status returned by the NF_READ_STATUS_CMD command.
//! @{
#define NF_MASK_STATUS_FAIL               (1<<0) //!< Fail
#define NF_MASK_STATUS_READY              (1<<6) //!< Ready
#define NF_MASK_STATUS_T_RDY_2KB          (1<<5) //!< True Ready
#define NF_MASK_STATUS_T_RDY_512B         (1<<6) //!< True Ready
//! @}



//! Macro that tests the cache busy.
//!
#define Nf_wait_cache_busy()                                         \
   {                                                                 \
      ecchrs_freeze(&AVR32_ECCHRS);                                  \
      nf_wr_cmd(NF_READ_STATUS_CMD);                                 \
      while( (nf_rd_data() & NF_MASK_STATUS_READY)==0 /*busy*/ );    \
      while( (nf_rd_data() & NF_MASK_STATUS_READY)==0 /*busy*/ );    \
      ecchrs_unfreeze(&AVR32_ECCHRS);                                \
   }

#define SIZE_PAGE_BYTE                  ((U16)1<<G_SHIFT_PAGE_BYTE)
#define SIZE_SECTOR_BYTE                ((U16)1<<S_SHIFT_SECTOR_BYTE)
#define SIZE_BLOCK_PAGE                 ((U8)1<<G_SHIFT_BLOCK_PAGE)
#define SIZE_PAGE_SECTOR                ((U8)1<<(G_SHIFT_PAGE_BYTE - S_SHIFT_SECTOR_BYTE))

// Spare zone offsets definition
//
//#define NF_SPARE_SIZE                16  // in bytes

#define NFC_SPARE_OFST_1_BLK_ID         1
#  define NFC_BLK_ID_SYSTEM       0x39    // System block (Font, screen, firmware, ...)
#  define NFC_BLK_ID_SUBLUT       0xE8    // sub-LUT block
#  define NFC_BLK_ID_RCV          0x72    // Recovery block
#  define NFC_BLK_ID_FBB          0xB4    // Free-blocks block
#  define NFC_BLK_ID_DATA         0xFF    // Data block (mass storage). Also the default value after an erase
#  define NFC_BLK_ID_QUARANTINE   0x8D    // Block which encountered an ECC error not recoverable. Not yet bad...

#define NFC_SPARE_OFST_2_BYTE_2         2
#define NFC_SPARE_OFST_3_BYTE_3         3 // NFC_BLK_ID_DATA: used as 'data valid'
#  define NFC_OFST_3_DATA_SRC                0x00    // Identify a source block (recovery)
#  define NFC_OFST_3_DATA_DST                0xFF    // Identify a recipient block (recovery)

#define NFC_SPARE_OFST_4_BYTE_4         4
#  define NFC_OFST_4_FBB_DRIVER_RELEASE      0x01    // Current NF release. When a new firmware implies that LUT and FBB must be rebuild, just increment this number.
#  define NFC_SPARE_DATA_VALID    0xFF
#  define NFC_SPARE_DATA_INVALID     0
#define NFC_SPARE_OFST_EXPORT          11

#define NFC_SPARE_OFST_6_LBA            6 // and 7
#  define NFC_OFST_6_FBB_VALID    0xFF
#  define NFC_OFST_6_FBB_INVALID     0

#define NFC_SPARE_OFST_ECC2             8 // and 9, 10
#define NFC_SPARE_OFST_ECC1            13 // and 14, 15

#if( NF_BAD_CONFIG==(false) )
#  if (NF_GENERIC_DRIVER==true) || (NF_AUTO_DETECT_2KB==true) || (NF_AUTO_DETECT_512B==true)
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_n_zones          ; // number of zones (=1024 blocks) per device
      _GLOBEXT_ _MEM_TYPE_SLOW_ U16  g_n_blocks         ; // number of blocks per device
      _GLOBEXT_ _MEM_TYPE_FAST_ U8   g_n_row_cycles     ; // number of row cycles to access a page of the NF memory
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_copy_back_cont   ; // 0 = copy back not supported, N = number of    CONTINUE subdivision contraint on copyback
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_copy_back_discont; // 0 = copy back not supported, N = number of DISCONTINUE subdivision contraint on copyback
      _GLOBEXT_                 bool g_cache_program    ; // 1 = cache program supported, 0 = not supported
      _GLOBEXT_                 bool g_ce_low           ; // tell if CE must be low during read cycle or not
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_clock_dfc_nfc    ; // Clock of dfc and nfc
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_dev_maker        ; // Device maker
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_dev_id           ; // Device ID
#     define G_N_ZONES                 g_n_zones
#     define G_N_BLOCKS                g_n_blocks
#     define G_N_ROW_CYCLES            g_n_row_cycles
#     define G_COPY_BACK_CONT          g_copy_back_cont
#     define G_COPY_BACK_DISCONT       g_copy_back_discont
#     define G_CACHE_PROG              g_cache_program
#     define G_CE_LOW                  g_ce_low
#     define G_CLK_DFC_NFC             g_clock_dfc_nfc
#     define G_DEV_MAKER               g_dev_maker
#     define G_DEV_ID                  g_dev_id
#  else
#     define G_N_ZONES                 (NF_N_ZONES)
#     define G_N_BLOCKS                (NF_N_BLOCKS)
#     define G_N_ROW_CYCLES            (NF_N_ROW_CYCLES)
#     define G_COPY_BACK_CONT          (NF_COPYBACK_CONT)
#     define G_COPY_BACK_DISCONT       (NF_COPYBACK_DISCONT)
#     define G_CACHE_PROG              (NF_CACHE_PROGRAM)
#     define G_CE_LOW                  (NF_CE_LOW)
#     define G_CLK_DFC_NFC             (CLK_DFC_NFC)
#     define G_DEV_MAKER               (NF_DEV_MAKER)
#     define G_DEV_ID                  (NF_DEV_ID)
#  endif

#  if (NF_GENERIC_DRIVER==true)
      _GLOBEXT_ _MEM_TYPE_FAST_ U8   g_shift_page_byte  ; // (1<<n) size of page,   unit in bytes
      _GLOBEXT_ _MEM_TYPE_FAST_ U8   g_shift_block_page ; // (1<<n) size of physical block,  unit in pages
      _GLOBEXT_ _MEM_TYPE_SLOW_ U8   g_ofst_blk_status  ; // Offset of Block Status information in spare zone
#     define Is_nf_2k()      (11==g_shift_page_byte )
#     define Is_not_nf_2k()  (11!=g_shift_page_byte )
#     define Is_nf_512()     ( 9==g_shift_page_byte )
#     define Is_not_nf_512() ( 9!=g_shift_page_byte )
#     define G_SHIFT_BLOCK_PAGE        g_shift_block_page
#     define G_SHIFT_PAGE_BYTE         g_shift_page_byte
#     define G_OFST_BLK_STATUS         g_ofst_blk_status
#     define NF_SPARE_POS              ( Is_nf_2k() ? 2048 : 512 )
#     define NF_N_GOOD_STATIC_BLOCK    ( Is_nf_2k() ?   32 : 256 )
#else
#     if (NF_SHIFT_PAGE_BYTE==11) // 2KB pages
#        define Is_nf_2k()      ( true  )
#        define Is_not_nf_2k()  ( false )
#        define Is_nf_512()     ( false )
#        define Is_not_nf_512() ( true  )
#        define NF_SPARE_POS         2048  // Spare zone starts at this byte offset in the page
#        define NF_N_GOOD_STATIC_BLOCK 32  // Number of the *valid* blocks in the static area
#        define G_OFST_BLK_STATUS       0  // Offset of the Bad Block information in spare zone
#     endif
#     if (NF_SHIFT_PAGE_BYTE==9) // 512B pages
#        define Is_nf_2k()        ( false )
#        define Is_not_nf_2k()    ( true  )
#        define Is_nf_512()       ( true  )
#        define Is_not_nf_512()   ( false )
#        define NF_SPARE_POS           512  // Spare zone starts at this byte offset in the page
#        define NF_N_GOOD_STATIC_BLOCK 256  // Number of the *valid* blocks in the static area
#        define G_OFST_BLK_STATUS        5  // Offset of the Bad Block information in spare zone
#     endif
#     define G_SHIFT_BLOCK_PAGE        (NF_SHIFT_BLOCK_PAGE)
#     define G_SHIFT_PAGE_BYTE         (NF_SHIFT_PAGE_BYTE)
#  endif

#define G_PROTECT_DEV             (NF_PROTECTED_DEVICES)



#endif // NF_BAD_CONFIG

//_____ F U N C T I O N S __________________________________________________

// Values used by "nfc_detect" fonction
//#define     NO_NF_CONNECTED   0xFF
//#define     NF_UNKNOW         0xFE


void        nf_init(            U32 hsb_f_hz );
U8          nf_get_freq(        void );
bool        nf_check_status(    void );
U8          nf_check_type(      U8 nb_dev );
void        nf_reset_nands(     U8 nb_dev );
void        nf_open_page_read(  U32 page_addr, U16 byte_addr);
void        nf_open_page_write( U32 page_addr, U16 byte_addr);
void        nf_mark_bad_block(  U32 page_addr );
void        nf_erase_block(     U32 page_addr, U8 force_erase );
void        nf_copy_back_init(  U32 page_addr );
void        nf_copy_back_conf(  U32 page_addr );
U32         nf_read_id(         U8 read_id_cmd, U8 nf_num );
U8          nf_detect( void );
void        nf_wait_busy(       void );
void        nf_unprotect(       void );
void        nf_protect(         void );

// Possible enhancement: possibility to protect areas of the NF.
// void        nf_protect_area( TBD );


void        nf_read_spare_byte(
   U8 _MEM_TYPE_SLOW_ * p_byte
,  U8  n_byte
,  U32 page_addr
);

void        nf_print_block(      U16 block_addr, U8 dev_id);

/**
 * \}
 */

#endif  // _NF_H_
