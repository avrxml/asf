/**************************************************************************
 *
 * \file
 *
 * \brief Management of the UART device firmware upgrade.
 *
 * This file manages the UART device firmware upgrade.
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

#include <avr32/io.h>
#include <stddef.h>
#include <string.h>

#include "compiler.h"
#include "board.h"
#include "conf_uart.h"
#include "conf_clocks.h"
#include "gpio.h"
#include "intc.h"
#include "usart.h"
#include "power_clocks_lib.h"
#include "flash_api.h"
#include "uart_dfu.h"
#include "boot.h"
#include "conf_isp.h"
#include "isp.h"


//_____ M A C R O S ________________________________________________________


#define DFU_STATE_IDLE                    0     // Idle state
#define DFU_STATE_PROGRAM                 1     // After a CMD_PROGRAM START and during CMD_PROGRAM_DATA.


#define DFU_CMD_STATUS_OK_NORETCHAR       0x0
#define DFU_CMD_STATUS_OK                 0x1
#define DFU_CMD_STATUS_errSECURITY_SET    0x2   // Read command is forbidden because the Security bit is set
#define DFU_CMD_STATUS_errUNKNOWN         0x4   // Unsupported command
#define DFU_CMD_STATUS_errADDRESS         0x8   // Invalid start or end address
#define DFU_CMD_STATUS_errWRITE           0x10  // Write attempt on a read-only memory
#define DFU_CMD_STATUS_errCHECKSUM        0x20  // Wrong checksum
#define DFU_CMD_STATUS_errUNEXPECTED      0x40  // Unexpected command (state-machine violation)
#define DFU_CMD_STATUS_errBLANKCHECK      0x80  // Blank check error




//! Answer codes
#define DFU_ANSWER_UNKNOWN                'K'   //! Unknown command
#define DFU_ANSWER_CHKSUM_ERR             'X'   //! Checksum error
#define DFU_ANSWER_RW_ERROR_SECURITY_SET  'L'   //! Read/Write error because the security bit is set
#define DFU_ANSWER_WRITE_ERROR            'P'   //! Write error (read-only memory)
#define DFU_ANSWER_OK                     '.'   //! Command successfully executed
#define DFU_ANSWER_EQUAL                  '='   //! For the read mem command

#define DFU_ANSWER_END_CHAR1              '\r'  //! CR
#define DFU_ANSWER_END_CHAR2              '\n'  //! LF

// Command groups
#define CMD_GRP_DATA                      0x00
#define CMD_GRP_DNLOAD                    0x01
#define CMD_GRP_UPLOAD                    0x03
#define CMD_GRP_EXEC                      0x04
#define CMD_GRP_SELECT                    0x06

// CMD_GRP_DNLOAD commands
#define CMD_PROGRAM_START                 0x00

// CMD_GRP_UPLOAD commands
#define CMD_READ_MEMORY                   0x00
#define CMD_BLANK_CHECK                   0x01

// CMD_GRP_EXEC commands
#define CMD_ERASE                         0x00
#define CMD_START_APPLI                   0x03

// CMD_GRP_SELECT commands
#define CMD_SELECT_MEMORY                 0x03

// CMD_ERASE arguments
#define CMD_ERASE_ARG_CHIP                0xFF

// CMD_START_APPLI arguments
#define CMD_START_APPLI_ARG_RESET         0x00
#define CMD_START_APPLI_ARG_NO_RESET      0x01

// CMD_SELECT_MEMORY arguments
#define CMD_SELECT_MEMORY_ARG_UNIT        0x00
#define CMD_SELECT_MEMORY_ARG_PAGE        0x01

// Memory units (CMD_SELECT_MEMORY_ARG_UNIT arguments)
#define MEM_FLASH                         0x00
#define MEM_EEPROM                        0x01
#define MEM_SECURITY                      0x02
#define MEM_CONFIGURATION                 0x03
#define MEM_BOOTLOADER                    0x04
#define MEM_SIGNATURE                     0x05
#define MEM_USER                          0x06
#define MEM_INTRAM                        0x07

// Number of memory units
#define MEM_COUNT                         (MEM_INTRAM + 1)

// Product information addresses
#define PRODUCT_MANUFACTURER_ID_ADDRESS   0x00
#define PRODUCT_PART_NUMBER_MSB_ADDRESS   0x01
#define PRODUCT_PART_NUMBER_LSB_ADDRESS   0x02
#define PRODUCT_REVISION_ADDRESS          0x03

// Number of product information items
#define PRODUCT_INF_ITEM_COUNT            (PRODUCT_REVISION_ADDRESS + 1)

// ISP information addresses
#define ISP_VERSION_ADDRESS               0x00
#define ISP_ID0_ADDRESS                   0x01
#define ISP_ID1_ADDRESS                   0x02

// Number of ISP information items
#define ISP_INF_ITEM_COUNT                (ISP_ID1_ADDRESS + 1)

// Synchronizer character (sent by batchisp).
#define DFU_SYNCHRO_CHAR                  'U'

// Packet start character (sent by batchisp).
#define DFU_PACKET_START_CHAR             ':'

// Max data length of an intel-hex frame
#define INTELHEX_MAX_DATA_LENGTH         0xFFu


//_____ D E C L A R A T I O N S ____________________________________________

static void mem_flash_read(void *dst, U32 src, size_t nbytes);
static int mem_flash_write(U32 dst, const void *src, size_t nbytes);
static void mem_security_read(void *dst, U32 src, size_t nbytes);
static int mem_security_write(U32 dst, const void *src, size_t nbytes);
static void mem_configuration_read(void *dst, U32 src, size_t nbytes);
static int mem_configuration_write(U32 dst, const void *src, size_t nbytes);
static void mem_bootloader_read(void *dst, U32 src, size_t nbytes);
static void mem_signature_read(void *dst, U32 src, size_t nbytes);
static void mem_user_read(void *dst, U32 src, size_t nbytes);
static int mem_user_write(U32 dst, const void *src, size_t nbytes);
static void mem_intram_read(void *dst, U32 src, size_t nbytes);

#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
extern void _unhandled_interrupt(void);


//_____ D E F I N I T I O N S ______________________________________________

static U8 PRODUCT_INF[PRODUCT_INF_ITEM_COUNT];

static const U8 ISP_INF[ISP_INF_ITEM_COUNT] =
{
  ISP_VERSION,
  ISP_ID0,
  ISP_ID1
};


static address_t MEMORY_END_ADDRESS[MEM_COUNT] =
{
  // MEM_FLASH
  {FLASH_API_FLASH_SIZE - 1},  // overwritten in uart_dfu_start()

  // MEM_EEPROM
  {0},

  // MEM_SECURITY
  {0},

  // MEM_CONFIGURATION
  {AVR32_FLASHC_GPF_NUM - 1},

  // MEM_BOOTLOADER
  {sizeof(ISP_INF) - 1},

  // MEM_SIGNATURE
  {sizeof(PRODUCT_INF) - 1},

  // MEM_USER
  {FLASH_API_USER_PAGE_SIZE - 1},

  // MEM_INTRAM
  {AVR32_SRAM_SIZE - 1}
};

static const struct
{
  void (*read)(void *dst, U32 src, size_t nbytes);
  int (*write)(U32 dst, const void *src, size_t nbytes);
} MEMORY_ACCESS[MEM_COUNT] =
{
  // MEM_FLASH
  {
    mem_flash_read,
    mem_flash_write
  },

  // MEM_EEPROM
  {
    NULL,
    NULL
  },

  // MEM_SECURITY
  {
    mem_security_read,
    mem_security_write
  },

  // MEM_CONFIGURATION
  {
    mem_configuration_read,
    mem_configuration_write
  },

  // MEM_BOOTLOADER
  {
    mem_bootloader_read,
    NULL
  },

  // MEM_SIGNATURE
  {
    mem_signature_read,
    NULL
  },

  // MEM_USER
  {
    mem_user_read,
    mem_user_write
  },

  // MEM_INTRAM
  {
    mem_intram_read,
    NULL
  }
};

static const U8 MEMORY_ERASE_VALUE[MEM_COUNT] =
{
  // MEM_FLASH
  0xFF,

  // MEM_EEPROM
  0xFF,

  // MEM_SECURITY
  0x00,

  // MEM_CONFIGURATION
  0x01, // 1 because at the bit-level

  // MEM_BOOTLOADER
  0x00,

  // MEM_SIGNATURE
  0x00,

  // MEM_USER
  0xFF,

  // MEM_INTRAM
  0x00
};

static const gpio_map_t USART_RX_GPIO_MAP =
{
  {DFU_USART_CANDIDATE0_RX_PIN, DFU_USART_CANDIDATE0_RX_FUNCTION},
  {DFU_USART_CANDIDATE1_RX_PIN, DFU_USART_CANDIDATE1_RX_FUNCTION},
  {DFU_USART_CANDIDATE2_RX_PIN, DFU_USART_CANDIDATE2_RX_FUNCTION},
  {DFU_USART_CANDIDATE3_RX_PIN, DFU_USART_CANDIDATE3_RX_FUNCTION},
};

static const gpio_map_t USART_TX_GPIO_MAP =
{
  {DFU_USART_CANDIDATE0_TX_PIN, DFU_USART_CANDIDATE0_TX_FUNCTION},
  {DFU_USART_CANDIDATE1_TX_PIN, DFU_USART_CANDIDATE1_TX_FUNCTION},
  {DFU_USART_CANDIDATE2_TX_PIN, DFU_USART_CANDIDATE2_TX_FUNCTION},
  {DFU_USART_CANDIDATE3_TX_PIN, DFU_USART_CANDIDATE3_TX_FUNCTION},
};

static volatile avr32_usart_t* DFU_USART_CANDIDATES[] =
{
  DFU_USART_CANDIDATE0,
  DFU_USART_CANDIDATE1,
  DFU_USART_CANDIDATE2,
  DFU_USART_CANDIDATE3
};

static int DFU_USART_IRQS[] =
{
  DFU_USART_CANDIDATE0_IRQ,
  DFU_USART_CANDIDATE1_IRQ,
  DFU_USART_CANDIDATE2_IRQ,
  DFU_USART_CANDIDATE3_IRQ
};

static volatile avr32_usart_t* DFU_USART;
static bool       security_active = true;
static U8         memory = MEM_FLASH;
static address_t  start_address;
static address_t  end_address;
static U32        address_range_nb_bytes;
static U8         dfu_buffer[DFU_BUFFER_LENGTH];
static U32        dfu_buffer_nb_Bytes; // Counter of valid Bytes in the DFU buffer.
static U8         *ptr_dfu_buffer;
static U8         dfu_status = DFU_CMD_STATUS_OK;
static U8         dfu_state = DFU_STATE_IDLE;

static U16        current_page = 0;  // Implicitly initialized to zero by default

#define UART_RX_BUFFER_SIZE   32
// UART rx buffer
static volatile U8   rx_buffer[UART_RX_BUFFER_SIZE];
static volatile U32  rx_write_idx;  // Implicitly initialized to zero by default
static volatile U32  rx_read_idx;   // Implicitly initialized to zero by default

static int flag_checksum_read;


/**
*   Calculates the CRC-8-CCITT.
*
*   CRC-8-CCITT is defined to be x^8 + x^2 + x + 1
*
*   To use this function use the following template:
*
*       crc = Crc8( crc, data );
*/
#define POLYNOMIAL    (ISP_CFG1_CRC8_POLYNOMIAL << 7)
static U8 Crc8( U8 inCrc, U8 inData )
{
  int i;
  U16 data;

  data = inCrc ^ inData;
  data <<= 8;

  for ( i = 0; i < 8; i++ )
  {
    if (( data & 0x8000 ) != 0 )
    {
      data = data ^ POLYNOMIAL;
    }
    data = data << 1;
  }
  return (unsigned char)( data >> 8 );
} // Crc8

static bool is_isp_forced(void)
{
  return((ISP_CFG1&ISP_CFG1_FORCE_MASK)>>ISP_CFG1_FORCE_OFFSET);
}

static void force_isp(bool force)
{
  U32 u32_tempo;
  U8  crc8 = 0;
  int i;


  // 1) Read the config word1 and set the ISP_FORCE bit to force.
  u32_tempo = (ISP_CFG1 & ~ISP_CFG1_FORCE_MASK) | ((force<<ISP_CFG1_FORCE_OFFSET) &  ISP_CFG1_FORCE_MASK);

  // 2) Compute the CRC8 and update the config word1
  for(i=24; i; i-=8)  // Compute the CRC8 on the 3 upper Bytes of the word.
    crc8 =  Crc8(crc8, u32_tempo>>i);
  u32_tempo = (u32_tempo & ~ISP_CFG1_CRC8_MASK) | ((crc8<<ISP_CFG1_CRC8_OFFSET) &  ISP_CFG1_CRC8_MASK);

  // 3) Write the config word1
  mem_user_write(ISP_CFG1_OFFSET, &u32_tempo, 4);
}



//! \brief Read n Bytes from the internal Flash memory at address src and copy at address dst
static void mem_flash_read(void *dst, U32 src, size_t nbytes)
{
  memcpy(dst, AVR32_FLASH + src, nbytes);
}

//! \brief Write n Bytes from src to the internal Flash memory address dst.
static int mem_flash_write(U32 dst, const void *src, size_t nbytes)
{
  flash_api_memcpy(AVR32_FLASH + dst, src, nbytes, false);
  if(flash_api_is_lock_error() || flash_api_is_programming_error())
    return(FAIL);
  else
    return(PASS);
}

//! \brief Write the value 1 to *dst if the security bit is set, else write 0.
static void mem_security_read(void *dst, U32 src, size_t nbytes)
{
  if (nbytes)
    *(U8 *)dst = flash_api_is_security_bit_active();
}

//! \brief Set the security bit.
static int mem_security_write(U32 dst, const void *src, size_t nbytes)
{
  if (nbytes && *(U8 *)src)
  {
    security_active = true;
    flash_api_activate_security_bit();
    if(flash_api_is_lock_error() || flash_api_is_programming_error())
      return(FAIL);
    else
      return(PASS);
  }
  else
    return(FAIL);
}

//! \brief Read nbytes GP fuse bit, copy each bit to a Byte
//
// \param dst     pointer on the bit value
// \param src     the GP fuse bit to read
// \param nbytes  number of GP fuse bits to read
static void mem_configuration_read(void *dst, U32 src, size_t nbytes)
{
  U8 *dest = dst;
  while (nbytes--)
    *dest++ = flash_api_read_gp_fuse_bit(src++);
}

//! \brief Write nbytes GP fuse bit (one Byte per bit)
//
// \param dst     the GP fuse bit to set
// \param src     pointer on the bit value
// \param nbytes  number of GP fuse bits to set
static int mem_configuration_write(U32 dst, const void *src, size_t nbytes)
{
  const U8 *source = src;
  while (nbytes--)
    flash_api_set_gp_fuse_bit(dst++, *source++);
  if(flash_api_is_lock_error() || flash_api_is_programming_error())
    return(FAIL);
  else
    return(PASS);
}

//! \brief Read the bootloader memory (i.e. the ISP version info)
static void mem_bootloader_read(void *dst, U32 src, size_t nbytes)
{
  memcpy(dst, ISP_INF + src, nbytes);
}

//! \brief Read the signature memory (i.e. the UC3 device info)
static void mem_signature_read(void *dst, U32 src, size_t nbytes)
{
  memcpy(dst, PRODUCT_INF + src, nbytes);
}

//! \brief Read n Bytes from the user page address src and copy to the address dst
static void mem_user_read(void *dst, U32 src, size_t nbytes)
{
  memcpy(dst, (U8 *)FLASH_API_USER_PAGE + src, nbytes);
}

//! \brief Write n Bytes from src to the user page address dst.
static int mem_user_write(U32 dst, const void *src, size_t nbytes)
{
  flash_api_memcpy(FLASH_API_USER_PAGE + dst, src, nbytes, true);
  if(flash_api_is_lock_error() || flash_api_is_programming_error())
    return(FAIL);
  else
    return(PASS);
}

//! \brief Read n Bytes from the internal page address src and copy to the address dst
static void mem_intram_read(void *dst, U32 src, size_t nbytes)
{
  memcpy(dst, AVR32_SRAM + src, nbytes);
}


/*
 * USART interrupt service routines.
 */
#if defined(__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void vUSART_ISR( void )
{
  rx_buffer[rx_write_idx++] = DFU_USART->rhr;
  rx_write_idx &= ~UART_RX_BUFFER_SIZE; // Keep the buffer index within the buffer range.
#ifdef DEBUG_ON
  // Check for buffer overflow
  if(rx_read_idx == rx_write_idx)
    gpio_clr_gpio_pin(LED0_GPIO);
#endif
  // Note: The USART Rx interrupt flag is cleared by side effect when reading
  // the received character.
}


// USART Interrupt service routine used as long as the chosen USART has not been identified.
#if defined(__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void vUSART_ISR_init( void )
{
  int i,j;
  U8  rxchar;

  for(i=0; i<sizeof(DFU_USART_IRQS)/sizeof(DFU_USART_IRQS[0]);i++)
  {
    if((AVR32_INTC.icr[(~AVR32_INTC_INT3)&(AVR32_INTC_NUM_INT_LEVELS-1)]&AVR32_INTC_CAUSE_MASK) == (DFU_USART_IRQS[i]>>5))
    {
      rxchar = DFU_USART_CANDIDATES[i]->rhr;
      if(rxchar != DFU_SYNCHRO_CHAR)
        continue;
      else
      {
        // The USART is chosen.
        DFU_USART = DFU_USART_CANDIDATES[i];
        rx_buffer[rx_write_idx++] = rxchar;
        rx_write_idx &= ~UART_RX_BUFFER_SIZE; // Keep the buffer index within the buffer range.
        // Register the regular USART interrupt handler
        INTC_register_interrupt((__int_handler)&vUSART_ISR, DFU_USART_IRQS[i], AVR32_INTC_INT3);
        // Disable the other USARTs
        for(j=0; j<sizeof(DFU_USART_CANDIDATES)/sizeof(DFU_USART_CANDIDATES[0]);j++)
        {
          if(i!=j)
          {
            // Reset unused USART.
            usart_reset(DFU_USART_CANDIDATES[j]);
            // Enable the default GPIO feature on the corresponding USART RX pin.
            gpio_enable_gpio_pin(USART_RX_GPIO_MAP[j].pin);
            // Undo the uart rx interrupt handler registration
            INTC_register_interrupt((__int_handler)&_unhandled_interrupt, DFU_USART_IRQS[j], AVR32_INTC_INT3);
          }
        }
        // Enable the chosen USART TX pin
        gpio_enable_module_pin(USART_TX_GPIO_MAP[i].pin, USART_TX_GPIO_MAP[i].function);
        break;
      }
    }
  }
  // Note: The USART Rx interrupt flag is cleared by side effect when reading
  // the received character.
}



/*! \brief  function used to configure the 4 potential USART candidates with the current PBA frequency.
 */
static void uart_configure(void)
{
  int i;


  // USART options.
  static const usart_options_t USART_OPTIONS =
  {
    .baudrate     = DFU_UART_BAUD_RATE,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to all candidates USART RX
  gpio_enable_module(USART_RX_GPIO_MAP,
                     sizeof(USART_RX_GPIO_MAP) / sizeof(USART_RX_GPIO_MAP[0]));

  // Initialize all candidate USARTs in RS232 mode.
  for(i=0; i<sizeof(DFU_USART_CANDIDATES) / sizeof(DFU_USART_CANDIDATES[0]); i++)
    usart_init_rs232(DFU_USART_CANDIDATES[i], &USART_OPTIONS, TARGET_PBA_FREQ_HZ);

  AVR32_ENTER_CRITICAL_REGION();
  // Register the USART interrupt handler to the interrupt controller and
  // enable the USART interrupt.
  for(i=0; i<sizeof(DFU_USART_CANDIDATES) / sizeof(DFU_USART_CANDIDATES[0]); i++)
  {
    // We're not fully done yet: disable receiver and transmitter.
    DFU_USART_CANDIDATES[i]->cr |= AVR32_USART_CR_RXDIS_MASK | AVR32_USART_CR_TXDIS_MASK;

    INTC_register_interrupt((__int_handler)&vUSART_ISR_init, DFU_USART_IRQS[i], AVR32_INTC_INT3);

    // Enable USART interrupt sources (rx only)
    DFU_USART_CANDIDATES[i]->ier = AVR32_USART_IER_RXRDY_MASK;

    // Enable receiver and transmitter
    DFU_USART_CANDIDATES[i]->cr |= AVR32_USART_CR_RXEN_MASK | AVR32_USART_CR_TXEN_MASK;
  }
  AVR32_LEAVE_CRITICAL_REGION();

  // Enable all interrupts.
  Enable_global_interrupt();
}


/*! \brief  UART DFU read function (read + echo)
 * \param   data a pointer on an unsigned 8-bit data which will contain the data read.
 */
inline static void uart_dfu_read_char(U8 *data)
{
  do
  {
    if(rx_read_idx != rx_write_idx)
    {
      *data = rx_buffer[rx_read_idx++];
      rx_read_idx &= ~UART_RX_BUFFER_SIZE; // Keep the buffer index within the buffer range.
#ifdef UART_DFU_ECHO_ON
      usart_bw_write_char(DFU_USART, *data);  // Echo the character
#endif
      break;
    }
    else
    {
      // Go to FROZEN sleep mode. The CPU will wake up upon reception of a character.
      SLEEP(AVR32_PM_SMODE_FROZEN);
    }
  }while(1);
}

/*!
 * \brief Perform the UART autobaud process by using the synchronizer character sent by the external programmer.
 */
static int uart_autobaud_process(void)
{
  // Wait for the synchronizer character.
  while(!rx_write_idx)
  {
    // Go to FROZEN sleep mode. The CPU will wake up upon reception of a character.
    SLEEP(AVR32_PM_SMODE_FROZEN);
  }

  // Note: we expect to receive one predefined character. This is used as
  // a synchronizer and to perform the automatic detection of the UART.
  if(DFU_SYNCHRO_CHAR != rx_buffer[rx_read_idx++])
    return(FAIL);
  else
  {
    // Echo the synchronizer character back.
    usart_bw_write_char(DFU_USART, DFU_SYNCHRO_CHAR);
    return PASS;
  }
}



static void uart_dfu_stop(int forceisp)
{
  int i;
  force_isp(forceisp);

  //#
  //# Undo the UART settings performed by the bootloader
  //#
  Disable_global_interrupt();

  usart_reset(DFU_USART);

  // Disable receiver and transmitter
  DFU_USART->cr |= AVR32_USART_CR_RXDIS_MASK | AVR32_USART_CR_TXDIS_MASK;

  // Disable USART interrupt sources (rx only)
  DFU_USART->idr = AVR32_USART_IER_RXRDY_MASK;

  // Undo the UART feature of the chosen GPIO pins.
  gpio_enable_gpio(USART_RX_GPIO_MAP, sizeof(USART_RX_GPIO_MAP) / sizeof(USART_RX_GPIO_MAP[0]));
  gpio_enable_gpio(USART_TX_GPIO_MAP, sizeof(USART_TX_GPIO_MAP) / sizeof(USART_TX_GPIO_MAP[0]));

  // Undo the uart rx interrupt handler registration
  for(i=0; i<sizeof(DFU_USART_CANDIDATES)/sizeof(DFU_USART_CANDIDATES[0]);i++)
  {
    INTC_register_interrupt((__int_handler)&_unhandled_interrupt, DFU_USART_IRQS[i], AVR32_INTC_INT3);
  }
}


// Undo the uart settings performed by the bootloader. Set the FORCEISP
// config field to true/false(forceisp). Generate a WDT reset.
static void stop_dfu_and_reset(int forceisp)
{
  uart_dfu_stop(forceisp);
  // Generate a WDT reset
  Disable_global_interrupt();
  AVR32_WDT.ctrl = AVR32_WDT_CTRL_EN_MASK | AVR32_WDT_CTRL_CEN_MASK |
                   (10 << AVR32_WDT_CTRL_PSEL_OFFSET) |
                   (1 << AVR32_WDT_CTRL_DAR_OFFSET) | // Disable the WDT after reset
                   (AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET);
  AVR32_WDT.ctrl = AVR32_WDT_CTRL_EN_MASK | AVR32_WDT_CTRL_CEN_MASK |
                   (10 << AVR32_WDT_CTRL_PSEL_OFFSET) |
                   (1 << AVR32_WDT_CTRL_DAR_OFFSET) | // Disable the WDT after reset
                   ((~AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET) & AVR32_WDT_CTRL_KEY_MASK);
  while(1);
}


void uart_dfu_start(void)
{
  uint32_t did_reg = Get_debug_register(AVR32_DID);

  PRODUCT_INF[PRODUCT_MANUFACTURER_ID_ADDRESS] = Rd_bitfield(did_reg, AVR32_DID_MID_MASK);
  PRODUCT_INF[PRODUCT_PART_NUMBER_MSB_ADDRESS] = Rd_bitfield(did_reg, AVR32_DID_PN_MASK)>>8;
  PRODUCT_INF[PRODUCT_PART_NUMBER_LSB_ADDRESS] = Rd_bitfield(did_reg, AVR32_DID_PN_MASK);
  PRODUCT_INF[PRODUCT_REVISION_ADDRESS] = Rd_bitfield(did_reg, AVR32_DID_RN_MASK);
  MEMORY_END_ADDRESS[MEM_FLASH].long_address = flash_api_get_flash_size() - 1;
  security_active = flash_api_is_security_bit_active();

  // Configure USART
  uart_configure();

  // Autobaud & Synchronization
  if(uart_autobaud_process())
  {
    // The synchro character was not received or the UART configuration failed:
    // Stop the DFU, set the FORCEISP config field to true, and reset.
    stop_dfu_and_reset(true);
    // Not reachable
  }
}


/*! \brief  convert an ascii 2-byte data into a 1-byte hexdecimal value.
 * \param   ascii a pointer on the 2-byte data to convert.
 * \return  It returns the value of the ascii data in hexacimal.
 */
static U8 conv_ascii_to_hex(U8 *ascii)
{
  U8 i, temp;

  i = ascii[0] - '0';
  if (i > 9)
    i -= ('A'-'0'-0xA);
  i <<= 4;

  temp = ascii[1] - '0';
  if (temp > 9)
    temp -= ('A'-'0'-0xA);
  i += temp;

  return i;
}


//
// \brief Read 2digits from the current Intel Hex packet and return its corresponding Byte value.
// \param checksum the current checksum value (in/out)
// \return the Byte read
//
static U8 uart_read_Byte(U8 *checksum)
{
  U8  data[2];
  U8  u8_return_val;

  // Read 2 digits
  uart_dfu_read_char(&data[0]);
  uart_dfu_read_char(&data[1]);
  // Convert ascii to hexadecimal value
  u8_return_val = conv_ascii_to_hex(data);
  // Update Checksum
  *checksum += u8_return_val;

  return(u8_return_val);
}


static U16 uart_read_16bits(U8 *checksum)
{
  U16 ret_val;
  ret_val = (U16)uart_read_Byte(checksum)<<8;
  ret_val |= (U16)uart_read_Byte(checksum);
  return(ret_val);
}

/*! \brief  Convert each Byte of a input buffer into a hexadecimal 2-ascii-character
 *  string and write each digit to the UART.
 *
 * \param   pdfu_buffer buffer of Bytes
 * \param   nb_bytes number of Bytes in dfu_buffer[]
 *
 */
static void uart_convertbyte2ascii_write(U8 *pdfu_buffer, int nb_bytes)
{
  S8 temp;
  U8 digit;
  S8 shift;

  while(nb_bytes)
  {
/*    BETTER CODE SIZE???
    // Convert each Byte into a hexadecimal 2-ascii-character string
    for(i=1; i>=0; i--)
    {   // Process a digit
      temp = (*dfu_buffer) & 0x0F;
      if (temp >= 0xA)
        temp += 'A' - '0' - 0xA;
      data[i] = '0' + temp;
      // Next digit
      (*dfu_buffer) >>= 4;
    }
    // Go to next Byte
    dfu_buffer++;
    nb_bytes--;
    // Write each digit to the UART
    usart_bw_write_char(DFU_USART, digit[0]);
    usart_bw_write_char(DFU_USART, digit[1]);
*/
    shift = 4;
    do
    {
      temp = ((*pdfu_buffer)>>shift) & 0x0F;
      if(temp >= 0xA)
        temp += 'A' - '0' - 0xA;
      digit = '0' + temp;
      // Write each digit to the UART
      usart_bw_write_char(DFU_USART, digit);
      shift -= 4;
    }while(shift >= 0);
    // Go to next Byte
    pdfu_buffer++;
    nb_bytes--;
  }
}

/*! \brief  Convert and write on the USART a 2-byte value into a 4 characters ascii string.
 * \param   u16_val the 2-byte value to process.
 */
static void uart_convert16bit2ascii_write(U16 u16_val)
{
  S8  temp;
  int i;
  U8  data[4];

  for(i=3; i>=0; i--)
  {
    temp = u16_val & 0x0F;
    if (temp >= 0xA)
      temp += 'A' - '0' - 0xA;
    data[i] = '0' + temp;
    u16_val >>= 4;
  }
  for(i=0; i<4; i++)
    usart_bw_write_char(DFU_USART, data[i]);
}


/*! \brief  Parse a DFU Intel hex string and store info into a structure.
 *          This parser parses only the first 9 bytes of this structure (including the ':' character).
 * \param   req_header            The structure where the formatted info will be stored.
 * \param   intel_hex_req_header  The raw Intel Hex header frame (a pointer on the first 9 bytes).
 * \param   checksum              A pointer on a 8-bit value used to contain the checksum of the header frame.
 */
static void parse_dfu_intel_hex_header(req_header_t *req_header, U8 *intel_hex_req_header, U8 *checksum)
{
  // Fill the structure
  req_header->length = conv_ascii_to_hex(&intel_hex_req_header[1]);
  req_header->type = conv_ascii_to_hex(&intel_hex_req_header[7]);
  req_header->address.page_offset = (U16) conv_ascii_to_hex(&intel_hex_req_header[5]) + ((U16) conv_ascii_to_hex(&intel_hex_req_header[3]) << 8);
  req_header->address.page = current_page;

  // Compute the checksum
  *checksum = req_header->length;
  *checksum += req_header->type;
  *checksum += ((U8) req_header->address.page_offset) & 0xFF;
  *checksum += ((U16) req_header->address.page_offset) >> 8;
}



U8 uart_get_dfu_command(req_header_t *req_header)
{
  U8 i, checksum = 0;
  U8 intel_hex_req_header[9];

  // Wait for the reception of the packet start character
  do
  {
    uart_dfu_read_char(&intel_hex_req_header[0]);
  }while(DFU_PACKET_START_CHAR != intel_hex_req_header[0]);

  // Get the next 8 digits (==4 Bytes) of the header (length and offset and type fields)
  for(i=1; i<9; i++)
    uart_dfu_read_char(&intel_hex_req_header[i]);

  // Convert intel hex format to a structure
  parse_dfu_intel_hex_header(req_header, intel_hex_req_header, &checksum);

  return checksum;
}


/*! \brief  Build and send the answer frame
 */
static void send_answer_frame(void)
{
  // Eventually send an error code
  if( (dfu_status & DFU_CMD_STATUS_errUNKNOWN) ||
      (dfu_status & DFU_CMD_STATUS_errUNEXPECTED) )
    usart_bw_write_char(DFU_USART, DFU_ANSWER_UNKNOWN);
  else if(dfu_status & DFU_CMD_STATUS_errCHECKSUM)
    usart_bw_write_char(DFU_USART, DFU_ANSWER_CHKSUM_ERR);
  else if(dfu_status & DFU_CMD_STATUS_errSECURITY_SET)
    usart_bw_write_char(DFU_USART, DFU_ANSWER_RW_ERROR_SECURITY_SET);
  else if( (dfu_status & DFU_CMD_STATUS_errWRITE) ||
           (dfu_status & DFU_CMD_STATUS_errADDRESS) )
    usart_bw_write_char(DFU_USART, DFU_ANSWER_WRITE_ERROR);
  else if(dfu_status & DFU_CMD_STATUS_errBLANKCHECK)
  {
    // Only send the offset in the current page (i.e. start_address.page_offset)
    // (Convert each Byte to 2 ascii digits and send through UART).
    uart_convert16bit2ascii_write(start_address.page_offset);
  }
  else if(dfu_status & DFU_CMD_STATUS_OK)
    usart_bw_write_char(DFU_USART, DFU_ANSWER_OK);

  // End of answer frame
  usart_bw_write_char(DFU_USART, DFU_ANSWER_END_CHAR1);
  usart_bw_write_char(DFU_USART, DFU_ANSWER_END_CHAR2);
}


/*! \brief  Validate the checksum with the one read on the USART.
 * \param   checksum A pointer on a 8-bit value that contains the actual checksum.
 * \return  0 if the checksum is OK
 */
static U8 validate_checksum(U8 *checksum)
{
  flag_checksum_read = true;
  // Read the checksum byte
  uart_read_Byte(checksum);
  return(*checksum);
}



/*! \brief  Read and check the start address and the end address. Validate the checksum.
 *
 * \param   req_header pointer on a structure containing the frame header information
 * \param   checksum A pointer on a 8-bit value that contains the actual checksum.
 *
 * \return  0 if the start and end addresses are correct and if the checksum is OK.
 *          Additionally if the start address and/or end address are/is wrong, the
 *          dfu_status global variable is set to DFU_CMD_STATUS_errADDRESS.
 */
static U8 get_and_check_mem_range_and_crc(req_header_t *req_header, U8 *checksum)
{
  // NOTE: the start_address.page and the end_address.page fields have previously
  // been set in a CMD_SELECT_MEMORY_PAGE command.

  // Read the start address
  start_address.page_offset = uart_read_16bits(checksum);
  start_address.page = current_page;
  // Read the end address
  end_address.page_offset = uart_read_16bits(checksum);
  end_address.page = current_page;

  // Decrement the data length because we've consumed 4 Bytes from the data section.
  req_header->length -= 4;

  // Check the start address and the end address
  if( (end_address.long_address < start_address.long_address) ||
      (end_address.long_address > MEMORY_END_ADDRESS[memory].long_address) )
  {
    dfu_status = DFU_CMD_STATUS_errADDRESS;
    return FAIL;  // We must return a non-zero value.
  }

  // The number of Bytes
  address_range_nb_bytes = end_address.long_address - start_address.long_address + 1;

  // Reset the data buffer ptr to the start of the buffer.
  ptr_dfu_buffer = &dfu_buffer[0];
  dfu_buffer_nb_Bytes = 0;

  return(validate_checksum(checksum));
}


//
// \brief Dump the remaining Bytes of the current Intel Hex packet.
//
// Dump the remaining Bytes of the current Intel Hex packet and return a status
// message to the external programmer.
//
// \param The number of remaining unread Bytes from the current Intel Hex packet
//
static void dump_dfu_packet(U8 data_nb_Bytes_left)
{
  U8 trash;

  // Dump the remaining data Bytes
  while(data_nb_Bytes_left)
  {
    trash = uart_read_Byte(&trash);
    --data_nb_Bytes_left;
  }
  // Dump the checksum if it has not already been read
  if(flag_checksum_read == false)
    trash = uart_read_Byte(&trash);

  // Send the return message to the external programmer
  send_answer_frame();
}


//
// \brief Dump the remaining Bytes of the current Intel Hex packet and reset the DFU state machine
//
// Dump the remaining Bytes of the current Intel Hex packet, return a status message
// to the external programmer and reset the DFU state machine.
//
// \param data_nb_Bytes_left The number of remaining unread Bytes from the current Intel Hex packet
//
static void dump_dfu_packet_and_reset_dfu_state(U8 data_nb_Bytes_left)
{
  // Dump the remaining Bytes of the current Intel Hex packet, return a status
  // message to the external programmer
  dump_dfu_packet(data_nb_Bytes_left);

  // Reset the DFU state machine
  dfu_state = DFU_STATE_IDLE;
}


//! \brief Read n Bytes from the currently chosen memory and verify that each
//         Byte is set to the pre-defined erase value of that memory.
//
static void erase_check_mem(void)
{
  U32 frame_nb_bytes;
  U8  *ptr;


  while(address_range_nb_bytes)
  {
    // Do not read more than DFU_BUFFER_LENGTH_WRITE characters.
    frame_nb_bytes = min(address_range_nb_bytes, DFU_BUFFER_LENGTH_READ);
    // Read
    MEMORY_ACCESS[memory].read(dfu_buffer, start_address.long_address, frame_nb_bytes);
    // Perform the blank check
    ptr = dfu_buffer;
    while (frame_nb_bytes--)
    {
      if (*ptr++ != MEMORY_ERASE_VALUE[memory])
      {
        dfu_status = DFU_CMD_STATUS_errBLANKCHECK;
        return;
      }
      start_address.long_address++;
      address_range_nb_bytes--;
    }
    // address_range_nb_bytes and start_address.long_address are up-to-date.
  }
}


//! \brief Read address_range_nb_bytes Bytes from the currently selected memory
//         and send to UART.
static void read_mem(void)
{
  U32 frame_nb_bytes;


  while(address_range_nb_bytes)
  {
    // Write the start address followed by '=' to UART
    // Only send the start address offset in the current page (i.e. start_address.page_offset)
    // (Convert each Byte to 2 ascii digits and send through UART).
    uart_convert16bit2ascii_write(start_address.page_offset);
    usart_bw_write_char(DFU_USART, DFU_ANSWER_EQUAL);
    // Do not send more than DFU_BUFFER_LENGTH_READ characters to the external
    // programmer.
    frame_nb_bytes = min(address_range_nb_bytes, DFU_BUFFER_LENGTH_READ);
    // Read
    MEMORY_ACCESS[memory].read(dfu_buffer, start_address.long_address, frame_nb_bytes);
    // Convert each Byte to 2 ascii digits and sent through UART.
    uart_convertbyte2ascii_write(dfu_buffer, frame_nb_bytes);
    // We've consumed n Bytes from the current address range
    address_range_nb_bytes -= frame_nb_bytes;
    // Point to the next Byte to read
    start_address.long_address += frame_nb_bytes;
    // End of frame
    // NOTE: for the last frame, the CRLF will be returned by send_answer_frame().
    if(address_range_nb_bytes)
    {
      usart_bw_write_char(DFU_USART, DFU_ANSWER_END_CHAR1);
      usart_bw_write_char(DFU_USART, DFU_ANSWER_END_CHAR2);
    }
  }
}


//! \brief Receive the data from one intel-Hex frame and program the data to the
//         currently selected memory.
static void write_mem(req_header_t *req_header, U8 *checksum)
{
  // As long as there are Bytes in the current Intel-Hex frame and the current
  // address range has not been fully processed, get a Byte.
  while((req_header->length)&&(address_range_nb_bytes))
  {
    // WARNING NOTE: We do not consider the offset field of the Intel-Hex header
    // because the data are sent in order by the programmer.
    *ptr_dfu_buffer++ = uart_read_Byte(checksum);
    dfu_buffer_nb_Bytes++;
    // We've read one Byte from the current Intel Hex frame
    req_header->length--;
    // We've consumed one Byte from the current address range
    address_range_nb_bytes--;
  }

  // We should always be in the case where req_header->length == 0
  // else the remaining unexpected Bytes will be dumped before leaving this fct.

  // Check the checksum
  if(validate_checksum(checksum))
  {
    dfu_status = DFU_CMD_STATUS_errCHECKSUM;
    // Return an error status to the external programmer, reset the DFU state
    // machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  if(!address_range_nb_bytes)
  {   // All data has been received
    // Change dfu_state if all data have been received (will be done by
    // dump_dfu_packet_and_reset_dfu_state()).
    // Write to the memory.
    if(FAIL == MEMORY_ACCESS[memory].write(start_address.long_address, dfu_buffer, dfu_buffer_nb_Bytes))
      dfu_status = DFU_CMD_STATUS_errWRITE;
    // Return the status to the external programmer, reset the DFU state machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
  }
  else if(DFU_BUFFER_LENGTH - dfu_buffer_nb_Bytes < INTELHEX_MAX_DATA_LENGTH)
  {
    // Time to write if the DFU buffer remaining number of free Bytes is lower
    // than a max intel-hex frame size.
    if(FAIL == MEMORY_ACCESS[memory].write(start_address.long_address, dfu_buffer, dfu_buffer_nb_Bytes))
    {
      dfu_status = DFU_CMD_STATUS_errWRITE;
      // Return the status to the external programmer, reset the DFU state machine.
      dump_dfu_packet_and_reset_dfu_state(req_header->length);
    }
    else
    {
      // Update the next address to write to.
      start_address.long_address += dfu_buffer_nb_Bytes;
      // Reset the data buffer ptr to the start of the buffer.
      ptr_dfu_buffer = &dfu_buffer[0];
      dfu_buffer_nb_Bytes = 0;
      // Return a status message to the external programmer
      dump_dfu_packet(req_header->length);
    }
  }
  else
  {   // Write to memory will be done later.
    // Return a status message to the external programmer
    dump_dfu_packet(req_header->length);
  }
  return;
}


//
//! \brief Process commands under the CMD_GRP_DOWNLOAD type.
//  Process commands under the CMD_GRP_DOWNLOAD type by parsing the data section
//  of the Intel Hex frame (and the checksum field). Sets dfu_status and dfu_state
//  upon error. dfu_state is updated to DFU_STATE_PROGRAM upon success.
//
//  \param req_header Structure containing the Intel Hex header info
//  \param checksum   Current checksum value
//
void process_cmd_grp_dnload(req_header_t *req_header, U8 *checksum);
void process_cmd_grp_dnload(req_header_t *req_header, U8 *checksum)
{
  U8 command;


  if( true == security_active )
  {   // The security bit is set => any command in this group is forbidden.
    dfu_status = DFU_CMD_STATUS_errSECURITY_SET;
  }
  else if( NULL == MEMORY_ACCESS[memory].write )
  {   // The currently selected memory is not writable
    dfu_status = DFU_CMD_STATUS_errWRITE;
  }
  if( DFU_CMD_STATUS_OK != dfu_status )
  {
    // Dump the rest of the Intel Hex packet, return a status to the external
    // programmer, reset the DFU state machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  // Read the command field
  command = uart_read_Byte(checksum);
  // Decrement the data length because we've consumed a Byte from the data section.
  req_header->length--;

  switch(command)
  {
  case CMD_PROGRAM_START:
    if(DFU_STATE_IDLE != dfu_state)
    {   // State machine violation
      dfu_status = DFU_CMD_STATUS_errUNEXPECTED;
      break;
    }
    // Do the command start address and end address fit in the currently selected memory?
    // If OK, is the checksum correct?
    if(get_and_check_mem_range_and_crc(req_header, checksum))
    {
      if(dfu_status != DFU_CMD_STATUS_errADDRESS)
        dfu_status = DFU_CMD_STATUS_errCHECKSUM;
      break;
    }
    // The command is valid, change the DFU state.
    dfu_state = DFU_STATE_PROGRAM;
    // Dump the rest of the Intel Hex packet and return a status to the external
    // programmer.
    dump_dfu_packet(req_header->length);
    return;

  default:
    // The received command is not supported.
    dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    break;
  }

  // Dump the rest of the Intel Hex packet, return a status to the external
  // programmer, reset the DFU state machine.
  dump_dfu_packet_and_reset_dfu_state(req_header->length);
}


//
//! \brief Process commands under the CMD_GRP_DATA type.
//  Process commands under the CMD_GRP_DATA type by parsing the data section
//  of the Intel Hex frame (and the checksum field). Sets dfu_status and change
//  dfu_state upon error. Change dfu_state when all data have been received.
//
//  \param req_header Structure containing the Intel Hex header info
//  \param checksum   Current checksum value
//
void process_cmd_grp_data(req_header_t *req_header, U8 *checksum);
void process_cmd_grp_data(req_header_t *req_header, U8 *checksum)
{
  // The DATA command is valid only if the dfu_state is DFU_STATE_PROGRAM.
  if(DFU_STATE_PROGRAM != dfu_state)
  {
    // The received command is unexpected: set the error code in dfu_status
    // and reset dfu_state to DFU_STATE_IDLE.
    dfu_status = DFU_CMD_STATUS_errUNEXPECTED;
    // Dump the rest of the Intel Hex packet, return the status to the external
    // programmer, reset the DFU state machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  write_mem(req_header, checksum);
}


//
//! \brief Process commands under the CMD_GRP_UPLOAD type.
//  Process commands under the CMD_GRP_UPLOAD type by parsing the data section
//  of the Intel Hex frame (and the checksum field). Sets dfu_status and change
//  dfu_state upon error.
//
//  \param req_header Structure containing the Intel Hex header info
//  \param checksum   Current checksum value
//
void process_cmd_grp_upload(req_header_t *req_header, U8 *checksum);
void process_cmd_grp_upload(req_header_t *req_header, U8 *checksum)
{
  U8 command;


  if( true == security_active )
  {   // The security bit is set => any command in this group is forbidden
      // except for the following memories: MEM_SECURITY, MEM_CONFIGURATION,
      // MEM_BOOTLOADER, MEM_SIGNATURE.
    if((memory == MEM_FLASH) || (memory == MEM_EEPROM) || (memory == MEM_USER)
      || (memory == MEM_INTRAM))
    {
      dfu_status = DFU_CMD_STATUS_errSECURITY_SET;
    }
  }
  else if( NULL == MEMORY_ACCESS[memory].read )
  {   // The currently selected memory is not readable
    dfu_status = DFU_CMD_STATUS_errUNKNOWN;
  }
  if( DFU_CMD_STATUS_OK != dfu_status )
  {
    // Dump the rest of the Intel Hex packet, return a status to the external
    // programmer, reset the DFU state machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  // Read the command field
  command = uart_read_Byte(checksum);
  // Decrement the data length because we've consumed a Byte from the data section.
  req_header->length--;

  // NOTE: no use to test the DFU state: the external program is responsible
  // for issuing the commands in the correct order.

  // Do the command start address and end address fit in the currently selected
  // memory?
  // If OK, is the checksum correct?
  if(get_and_check_mem_range_and_crc(req_header, checksum))
  {
    if(dfu_status != DFU_CMD_STATUS_errADDRESS)
      dfu_status = DFU_CMD_STATUS_errCHECKSUM;
    // Dump the rest of the Intel Hex packet, return a status to the external
    // programmer, reset the DFU state machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  switch(command)
  {
  case CMD_READ_MEMORY:
    dfu_status = DFU_CMD_STATUS_OK_NORETCHAR;
    // Read n Bytes from the chosen memory and send to UART.
    read_mem();
    break;

  case CMD_BLANK_CHECK:
    // Read n Bytes from the currently chosen memory and verify that each Byte
    // is set to the pre-defined erase value of that memory.
    erase_check_mem();
    break;
  default:
    // The received command is not supported.
    dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    break;
  }

  // Return a status to the external programmer, reset the DFU state machine.
  dump_dfu_packet_and_reset_dfu_state(req_header->length);
}


//
//! \brief Process commands under the CMD_GRP_EXEC type.
//  Process commands under the CMD_GRP_EXEC type by parsing the data section
//  of the Intel Hex frame (and the checksum field). Sets dfu_status and change
//  dfu_state upon error.
//
//  \param req_header Structure containing the Intel Hex header info
//  \param checksum   Current checksum value
//
void process_cmd_grp_exec(req_header_t *req_header, U8 *checksum);
void process_cmd_grp_exec(req_header_t *req_header, U8 *checksum)
{
  U8 command, arg;


  // Read the command and argument fields
  command = uart_read_Byte(checksum);
  arg = uart_read_Byte(checksum);
  // Decrement the data length because we've consumed two Bytes from the data
  // section.
  req_header->length -= 2;

  // Check the checksum
  if(validate_checksum(checksum))
  {
    dfu_status = DFU_CMD_STATUS_errCHECKSUM;
    // Return an error status to the external programmer, reset the DFU state
    // machine.
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    return;
  }

  // NOTE: no use to test the DFU state: the external program is responsible
  // for issuing the commands in the correct order.
  switch(command)
  {
  case CMD_ERASE:
    if(arg != CMD_ERASE_ARG_CHIP)
    {
      dfu_status = DFU_CMD_STATUS_errUNKNOWN;
      break;
    }
    memory = MEM_FLASH;
    end_address.long_address = start_address.long_address = 0;
    flash_api_lock_all_regions(false);
    flash_api_erase_all_pages(false);
    security_active = false;
    break;

  case CMD_START_APPLI:
    if(arg == CMD_START_APPLI_ARG_RESET)
    {
      // Undo the uart settings performed by the bootloader. Set the FORCEISP
      // config field to false. Generate a WDT reset.
      stop_dfu_and_reset(false);
      // Not reachable
    }
    else if(arg == CMD_START_APPLI_ARG_NO_RESET)
    {
      // Undo the uart settings performed by the bootloader. Set the FORCEISP
      // config field to false.
      uart_dfu_stop(false);
      // Undo the clock settings performed by the bootloader.
      sys_clk_gen_stop();
      wait_10_ms();
      boot_program();
      // Not reachable
    }
    else
    {
      dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    }
    break;
  }
  // Return a status to the external programmer, reset the DFU state machine.
  dump_dfu_packet_and_reset_dfu_state(req_header->length);
}


//
//! \brief Process commands under the CMD_GRP_SELECT type.
//  Process commands under the CMD_GRP_SELECT type by parsing the data section
//  of the Intel Hex frame (and the checksum field). Sets dfu_status and change
//  dfu_state upon error.
//
//  \param req_header Structure containing the Intel Hex header info
//  \param checksum   Current checksum value
//
void process_cmd_grp_select(req_header_t *req_header, U8 *checksum);
void process_cmd_grp_select(req_header_t *req_header, U8 *checksum)
{
  U8  command, arg1, mem_id;
  U16 page_nb;


  // Read the command and argument1 fields
  command = uart_read_Byte(checksum);
  arg1 = uart_read_Byte(checksum);
  // Decrement the data length because we've consumed two Bytes from the data
  // section.
  req_header->length -= 2;

  // NOTE: no use to test the DFU state: the external program is responsible
  // for issuing the commands in the correct order.
  switch(command)
  {
  case CMD_SELECT_MEMORY:
    if(arg1 == CMD_SELECT_MEMORY_ARG_UNIT)
    {
      // Read the memory id field
      mem_id = uart_read_Byte(checksum);
      // Decrement the data length because we've consumed one Byte from the data
      // section.
      req_header->length--;

      // Check the checksum
      if(validate_checksum(checksum))
      {
        dfu_status = DFU_CMD_STATUS_errCHECKSUM;
        break;
      }

      switch(mem_id)
      {
        case MEM_FLASH:
        case MEM_EEPROM:
        case MEM_SECURITY:
        case MEM_CONFIGURATION:
        case MEM_BOOTLOADER:
        case MEM_SIGNATURE:
        case MEM_USER:
        case MEM_INTRAM:
          memory = mem_id;
          end_address.long_address = start_address.long_address = 0;
          break;

        default:
          dfu_status = DFU_CMD_STATUS_errUNKNOWN;
          break;
      }
    }
    else if(arg1 == CMD_SELECT_MEMORY_ARG_PAGE)
    {
      // Read the page number field
      page_nb = uart_read_16bits(checksum);

      // Decrement the data length because we've consumed two Bytes from the data
      // section.
      req_header->length -= 2;

      // Check the checksum
      if(validate_checksum(checksum))
      {
        dfu_status = DFU_CMD_STATUS_errCHECKSUM;
        break;
      }

      // Check that the page number is in the correct range
      if(page_nb <= MEMORY_END_ADDRESS[memory].page)
      {
        current_page = end_address.page = start_address.page = page_nb;
      }
      else
      {
        dfu_status = STATUS_errADDRESS;
      }
    }
    else
    {   // Unknown sub-argument
      dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    }
    break;

  default:
    // Unknown argument
    dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    break;
  }
  // Return a status to the external programmer, reset the DFU state machine.
  dump_dfu_packet_and_reset_dfu_state(req_header->length);
}


void uart_execute_dfu_command(req_header_t *req_header, U8 *checksum)
{
  if (!is_isp_forced())
    force_isp(true);

  dfu_status = DFU_CMD_STATUS_OK;
  flag_checksum_read = false;

  // Execute the required command
  switch(req_header->type)
  {
  case CMD_GRP_DNLOAD:
    process_cmd_grp_dnload(req_header, checksum);
    break;
  case CMD_GRP_DATA:
    process_cmd_grp_data(req_header, checksum);
    break;
  case CMD_GRP_UPLOAD:
    process_cmd_grp_upload(req_header, checksum);
    break;
  case CMD_GRP_EXEC:
    process_cmd_grp_exec(req_header, checksum);
    break;
  case CMD_GRP_SELECT:
    process_cmd_grp_select(req_header, checksum);
    break;
  default:
    dfu_status = DFU_CMD_STATUS_errUNKNOWN;
    dump_dfu_packet_and_reset_dfu_state(req_header->length);
    break;
  }
}
