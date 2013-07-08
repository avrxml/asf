/**************************************************************************
 *
 * \file
 *
 * \brief Low-level driver for AVR32 USBB.
 *
 * This file contains the USBB low-level driver routines.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


//_____ I N C L U D E S ____________________________________________________

#include "usb_drv.h"


//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

//! Pointers to the FIFO data registers of pipes/endpoints
//! Use aggregated pointers to have several alignments available for a same address
UnionVPtr pep_fifo[MAX_PEP_NB];

//! ---------------------------------------------------------
//! ------------------ DEVICE -------------------------------
//! ---------------------------------------------------------

#if USB_DEVICE_FEATURE == true

#include "usb_descriptors.h"

//! usb_init_device
//!
//!  This function initializes the USB device controller and
//!  configures the Default Control Endpoint.
//!
//! @return Status
//!
Status_bool_t usb_init_device(void)
{
  return Is_usb_id_device() && !Is_usb_endpoint_enabled(EP_CONTROL) &&
         Usb_configure_endpoint(EP_CONTROL,
                                TYPE_CONTROL,
                                DIRECTION_OUT,
                                EP_CONTROL_LENGTH,
                                SINGLE_BANK);
}

//! usb_set_ep_txpacket
//!
//!  This function fills the selected endpoint FIFO with a constant byte, using
//!  as few accesses as possible.
//!
//! @param ep           Number of the addressed endpoint
//! @param txbyte       Byte to fill the endpoint with
//! @param data_length  Number of bytes to write
//!
//! @return             Number of non-written bytes
//!
//! @note The selected endpoint FIFO may be filled in several steps by calling
//! usb_set_ep_txpacket several times.
//!
//! @warning Invoke Usb_reset_endpoint_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be filled in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 usb_set_ep_txpacket(U8 ep, U8 txbyte, U32 data_length)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionVPtr   ep_fifo_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCVPtr ep_fifo_end;
  Union64     txval;
#else
  UnionCVPtr  ep_fifo_end;
  union
  {
    U8 u8[1];
  } txval;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for write loops and limit the number of bytes to write
  ep_fifo_cur.u8ptr = pep_fifo[ep].u8ptr;
  ep_fifo_end.u8ptr = ep_fifo_cur.u8ptr +
                      min(data_length, Usb_get_endpoint_size(ep) - Usb_byte_count(ep));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  ep_fifo_end.u16ptr = (U16 *)Align_down((U32)ep_fifo_end.u8ptr, sizeof(U16));
  ep_fifo_end.u32ptr = (U32 *)Align_down((U32)ep_fifo_end.u16ptr, sizeof(U32));
  ep_fifo_end.u64ptr = (U64 *)Align_down((U32)ep_fifo_end.u32ptr, sizeof(U64));
#endif  // !__OPTIMIZE_SIZE__
  txval.u8[0] = txbyte;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  txval.u8[1] = txval.u8[0];
  txval.u16[1] = txval.u16[0];
  txval.u32[1] = txval.u32[0];

  // If pointer to FIFO data register is not 16-bit aligned
  if (!Test_align((U32)ep_fifo_cur.u8ptr, sizeof(U16)))
  {
    // Write 8-bit data to reach 16-bit alignment
    if (ep_fifo_cur.u8ptr < ep_fifo_end.u8ptr)
    {
      *ep_fifo_cur.u8ptr++ = txval.u8[0];
    }
  }

  // If pointer to FIFO data register is not 32-bit aligned
  if (!Test_align((U32)ep_fifo_cur.u16ptr, sizeof(U32)))
  {
    // Write 16-bit data to reach 32-bit alignment
    if (ep_fifo_cur.u16ptr < ep_fifo_end.u16ptr)
    {
      *ep_fifo_cur.u16ptr++ = txval.u16[0];
    }
  }

  // If pointer to FIFO data register is not 64-bit aligned
  if (!Test_align((U32)ep_fifo_cur.u32ptr, sizeof(U64)))
  {
    // Write 32-bit data to reach 64-bit alignment
    if (ep_fifo_cur.u32ptr < ep_fifo_end.u32ptr)
    {
      *ep_fifo_cur.u32ptr++ = txval.u32[0];
    }
  }

  // Write 64-bit-aligned data
  while (ep_fifo_cur.u64ptr < ep_fifo_end.u64ptr)
  {
    *ep_fifo_cur.u64ptr++ = txval.u64;
  }

  // Write remaining 32-bit data if some
  if (ep_fifo_cur.u32ptr < ep_fifo_end.u32ptr)
  {
    *ep_fifo_cur.u32ptr++ = txval.u32[0];
  }

  // Write remaining 16-bit data if some
  if (ep_fifo_cur.u16ptr < ep_fifo_end.u16ptr)
  {
    *ep_fifo_cur.u16ptr++ = txval.u16[0];
  }

  // Write remaining 8-bit data if some
  if (ep_fifo_cur.u8ptr < ep_fifo_end.u8ptr)
  {
    *ep_fifo_cur.u8ptr++ = txval.u8[0];
  }

#else

  // Write remaining 8-bit data if some
  while (ep_fifo_cur.u8ptr < ep_fifo_end.u8ptr)
  {
    *ep_fifo_cur.u8ptr++ = txval.u8[0];
  }

#endif  // !__OPTIMIZE_SIZE__

  // Compute the number of non-written bytes
  data_length -= ep_fifo_cur.u8ptr - pep_fifo[ep].u8ptr;

  // Save current position in FIFO data register
  pep_fifo[ep].u8ptr = ep_fifo_cur.u8ptr;

  // Return the number of non-written bytes
  return data_length;
}

//! usb_write_ep_txpacket
//!
//!  This function writes the buffer pointed to by txbuf to the selected
//!  endpoint FIFO, using as few accesses as possible.
//!
//! @param ep           Number of the addressed endpoint
//! @param txbuf        Address of buffer to read
//! @param data_length  Number of bytes to write
//! @param ptxbuf       NULL or pointer to the buffer address to update
//!
//! @return             Number of non-written bytes
//!
//! @note The selected endpoint FIFO may be written in several steps by calling
//! usb_write_ep_txpacket several times.
//!
//! @warning Invoke Usb_reset_endpoint_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be written in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 usb_write_ep_txpacket(U8 ep, const void *txbuf, U32 data_length, const void **ptxbuf)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionVPtr   ep_fifo;
  UnionCPtr   txbuf_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCPtr  txbuf_end;
#else
  UnionCPtr   txbuf_end;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for copy loops and limit the number of bytes to copy
  ep_fifo.u8ptr = pep_fifo[ep].u8ptr;
  txbuf_cur.u8ptr = txbuf;
  txbuf_end.u8ptr = txbuf_cur.u8ptr +
                    min(data_length, Usb_get_endpoint_size(ep) - Usb_byte_count(ep));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  txbuf_end.u16ptr = (U16 *)Align_down((U32)txbuf_end.u8ptr, sizeof(U16));
  txbuf_end.u32ptr = (U32 *)Align_down((U32)txbuf_end.u16ptr, sizeof(U32));
  txbuf_end.u64ptr = (U64 *)Align_down((U32)txbuf_end.u32ptr, sizeof(U64));

  // If all addresses are aligned the same way with respect to 16-bit boundaries
  if (Get_align((U32)txbuf_cur.u8ptr, sizeof(U16)) == Get_align((U32)ep_fifo.u8ptr, sizeof(U16)))
  {
    // If pointer to transmission buffer is not 16-bit aligned
    if (!Test_align((U32)txbuf_cur.u8ptr, sizeof(U16)))
    {
      // Copy 8-bit data to reach 16-bit alignment
      if (txbuf_cur.u8ptr < txbuf_end.u8ptr)
      {
        // 8-bit accesses to FIFO data registers do require pointer post-increment
        *ep_fifo.u8ptr++ = *txbuf_cur.u8ptr++;
      }
    }

    // If all addresses are aligned the same way with respect to 32-bit boundaries
    if (Get_align((U32)txbuf_cur.u16ptr, sizeof(U32)) == Get_align((U32)ep_fifo.u16ptr, sizeof(U32)))
    {
      // If pointer to transmission buffer is not 32-bit aligned
      if (!Test_align((U32)txbuf_cur.u16ptr, sizeof(U32)))
      {
        // Copy 16-bit data to reach 32-bit alignment
        if (txbuf_cur.u16ptr < txbuf_end.u16ptr)
        {
          // 16-bit accesses to FIFO data registers do require pointer post-increment
          *ep_fifo.u16ptr++ = *txbuf_cur.u16ptr++;
        }
      }

      // If pointer to transmission buffer is not 64-bit aligned
      if (!Test_align((U32)txbuf_cur.u32ptr, sizeof(U64)))
      {
        // Copy 32-bit data to reach 64-bit alignment
        if (txbuf_cur.u32ptr < txbuf_end.u32ptr)
        {
          // 32-bit accesses to FIFO data registers do not require pointer post-increment
          *ep_fifo.u32ptr = *txbuf_cur.u32ptr++;
        }
      }

      // Copy 64-bit-aligned data
      while (txbuf_cur.u64ptr < txbuf_end.u64ptr)
      {
        // 64-bit accesses to FIFO data registers do not require pointer post-increment
        *ep_fifo.u64ptr = *txbuf_cur.u64ptr++;
      }

      // Copy 32-bit-aligned data
      if (txbuf_cur.u32ptr < txbuf_end.u32ptr)
      {
        // 32-bit accesses to FIFO data registers do not require pointer post-increment
        *ep_fifo.u32ptr = *txbuf_cur.u32ptr++;
      }
    }

    // Copy remaining 16-bit data if some
    while (txbuf_cur.u16ptr < txbuf_end.u16ptr)
    {
      // 16-bit accesses to FIFO data registers do require pointer post-increment
      *ep_fifo.u16ptr++ = *txbuf_cur.u16ptr++;
    }
  }

#endif  // !__OPTIMIZE_SIZE__

  // Copy remaining 8-bit data if some
  while (txbuf_cur.u8ptr < txbuf_end.u8ptr)
  {
    // 8-bit accesses to FIFO data registers do require pointer post-increment
    *ep_fifo.u8ptr++ = *txbuf_cur.u8ptr++;
  }

  // Save current position in FIFO data register
  pep_fifo[ep].u8ptr = ep_fifo.u8ptr;

  // Return the updated buffer address and the number of non-copied bytes
  if (ptxbuf) *ptxbuf = txbuf_cur.u8ptr;
  return data_length - (txbuf_cur.u8ptr - (U8 *)txbuf);
}

//! usb_read_ep_rxpacket
//!
//!  This function reads the selected endpoint FIFO to the buffer pointed to by
//!  rxbuf, using as few accesses as possible.
//!
//! @param ep           Number of the addressed endpoint
//! @param rxbuf        Address of buffer to write
//! @param data_length  Number of bytes to read
//! @param prxbuf       NULL or pointer to the buffer address to update
//!
//! @return             Number of non-read bytes
//!
//! @note The selected endpoint FIFO may be read in several steps by calling
//! usb_read_ep_rxpacket several times.
//!
//! @warning Invoke Usb_reset_endpoint_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be read in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 usb_read_ep_rxpacket(U8 ep, void *rxbuf, U32 data_length, void **prxbuf)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionCVPtr  ep_fifo;
  UnionPtr    rxbuf_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCPtr  rxbuf_end;
#else
  StructCPtr  rxbuf_end;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for copy loops and limit the number of bytes to copy
  ep_fifo.u8ptr = pep_fifo[ep].u8ptr;
  rxbuf_cur.u8ptr = rxbuf;
  rxbuf_end.u8ptr = rxbuf_cur.u8ptr + min(data_length, Usb_byte_count(ep));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  rxbuf_end.u16ptr = (U16 *)Align_down((U32)rxbuf_end.u8ptr, sizeof(U16));
  rxbuf_end.u32ptr = (U32 *)Align_down((U32)rxbuf_end.u16ptr, sizeof(U32));
  rxbuf_end.u64ptr = (U64 *)Align_down((U32)rxbuf_end.u32ptr, sizeof(U64));

  // If all addresses are aligned the same way with respect to 16-bit boundaries
  if (Get_align((U32)rxbuf_cur.u8ptr, sizeof(U16)) == Get_align((U32)ep_fifo.u8ptr, sizeof(U16)))
  {
    // If pointer to reception buffer is not 16-bit aligned
    if (!Test_align((U32)rxbuf_cur.u8ptr, sizeof(U16)))
    {
      // Copy 8-bit data to reach 16-bit alignment
      if (rxbuf_cur.u8ptr < rxbuf_end.u8ptr)
      {
        // 8-bit accesses to FIFO data registers do require pointer post-increment
        *rxbuf_cur.u8ptr++ = *ep_fifo.u8ptr++;
      }
    }

    // If all addresses are aligned the same way with respect to 32-bit boundaries
    if (Get_align((U32)rxbuf_cur.u16ptr, sizeof(U32)) == Get_align((U32)ep_fifo.u16ptr, sizeof(U32)))
    {
      // If pointer to reception buffer is not 32-bit aligned
      if (!Test_align((U32)rxbuf_cur.u16ptr, sizeof(U32)))
      {
        // Copy 16-bit data to reach 32-bit alignment
        if (rxbuf_cur.u16ptr < rxbuf_end.u16ptr)
        {
          // 16-bit accesses to FIFO data registers do require pointer post-increment
          *rxbuf_cur.u16ptr++ = *ep_fifo.u16ptr++;
        }
      }

      // If pointer to reception buffer is not 64-bit aligned
      if (!Test_align((U32)rxbuf_cur.u32ptr, sizeof(U64)))
      {
        // Copy 32-bit data to reach 64-bit alignment
        if (rxbuf_cur.u32ptr < rxbuf_end.u32ptr)
        {
          // 32-bit accesses to FIFO data registers do not require pointer post-increment
          *rxbuf_cur.u32ptr++ = *ep_fifo.u32ptr;
        }
      }

      // Copy 64-bit-aligned data
      while (rxbuf_cur.u64ptr < rxbuf_end.u64ptr)
      {
        // 64-bit accesses to FIFO data registers do not require pointer post-increment
        *rxbuf_cur.u64ptr++ = *ep_fifo.u64ptr;
      }

      // Copy 32-bit-aligned data
      if (rxbuf_cur.u32ptr < rxbuf_end.u32ptr)
      {
        // 32-bit accesses to FIFO data registers do not require pointer post-increment
        *rxbuf_cur.u32ptr++ = *ep_fifo.u32ptr;
      }
    }

    // Copy remaining 16-bit data if some
    while (rxbuf_cur.u16ptr < rxbuf_end.u16ptr)
    {
      // 16-bit accesses to FIFO data registers do require pointer post-increment
      *rxbuf_cur.u16ptr++ = *ep_fifo.u16ptr++;
    }
  }

#endif  // !__OPTIMIZE_SIZE__

  // Copy remaining 8-bit data if some
  while (rxbuf_cur.u8ptr < rxbuf_end.u8ptr)
  {
    // 8-bit accesses to FIFO data registers do require pointer post-increment
    *rxbuf_cur.u8ptr++ = *ep_fifo.u8ptr++;
  }

  // Save current position in FIFO data register
  pep_fifo[ep].u8ptr = (volatile U8 *)ep_fifo.u8ptr;

  // Return the updated buffer address and the number of non-copied bytes
  if (prxbuf) *prxbuf = rxbuf_cur.u8ptr;
  return data_length - (rxbuf_cur.u8ptr - (U8 *)rxbuf);
}

#endif  // USB_DEVICE_FEATURE == true

//! ---------------------------------------------------------
//! ------------------ HOST ---------------------------------
//! ---------------------------------------------------------

#if USB_HOST_FEATURE == true

//! host_disable_all_pipes
//!
//!  This function disables all pipes for the host controller.
//!  Useful to execute upon disconnection.
//!
//! @return Void
//!
void host_disable_all_pipes(void)
{
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  bool sav_glob_int_en;
#endif
  U8 p;

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  // Disable global interrupts
  if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
#endif
  for (p = 0; p < MAX_PEP_NB; p++)
  { // Disable the pipe <p> (disable interrupt, free memory, reset pipe, ...)
    Host_disable_pipe_interrupt(p);
    Host_reset_pipe(p);
    Host_unallocate_memory(p);
    Host_disable_pipe(p);
  }
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  (void)Is_host_pipe_enabled(MAX_PEP_NB - 1);
  // Restore the global interrupts to the initial state
  if (sav_glob_int_en) cpu_irq_enable();
#endif
}

//! host_set_p_txpacket
//!
//!  This function fills the selected pipe FIFO with a constant byte, using
//!  as few accesses as possible.
//!
//! @param p            Number of the addressed pipe
//! @param txbyte       Byte to fill the pipe with
//! @param data_length  Number of bytes to write
//!
//! @return             Number of non-written bytes
//!
//! @note The selected pipe FIFO may be filled in several steps by calling
//! host_set_p_txpacket several times.
//!
//! @warning Invoke Host_reset_pipe_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be filled in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 host_set_p_txpacket(U8 p, U8 txbyte, U32 data_length)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionVPtr   p_fifo_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCVPtr p_fifo_end;
  Union64     txval;
#else
  UnionCVPtr  p_fifo_end;
  union
  {
    U8 u8[1];
  } txval;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for write loops and limit the number of bytes to write
  p_fifo_cur.u8ptr = pep_fifo[p].u8ptr;
  p_fifo_end.u8ptr = p_fifo_cur.u8ptr +
                     min(data_length, Host_get_pipe_size(p) - Host_byte_count(p));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  p_fifo_end.u16ptr = (U16 *)Align_down((U32)p_fifo_end.u8ptr, sizeof(U16));
  p_fifo_end.u32ptr = (U32 *)Align_down((U32)p_fifo_end.u16ptr, sizeof(U32));
  p_fifo_end.u64ptr = (U64 *)Align_down((U32)p_fifo_end.u32ptr, sizeof(U64));
#endif  // !__OPTIMIZE_SIZE__
  txval.u8[0] = txbyte;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  txval.u8[1] = txval.u8[0];
  txval.u16[1] = txval.u16[0];
  txval.u32[1] = txval.u32[0];

  // If pointer to FIFO data register is not 16-bit aligned
  if (!Test_align((U32)p_fifo_cur.u8ptr, sizeof(U16)))
  {
    // Write 8-bit data to reach 16-bit alignment
    if (p_fifo_cur.u8ptr < p_fifo_end.u8ptr)
    {
      *p_fifo_cur.u8ptr++ = txval.u8[0];
    }
  }

  // If pointer to FIFO data register is not 32-bit aligned
  if (!Test_align((U32)p_fifo_cur.u16ptr, sizeof(U32)))
  {
    // Write 16-bit data to reach 32-bit alignment
    if (p_fifo_cur.u16ptr < p_fifo_end.u16ptr)
    {
      *p_fifo_cur.u16ptr++ = txval.u16[0];
    }
  }

  // If pointer to FIFO data register is not 64-bit aligned
  if (!Test_align((U32)p_fifo_cur.u32ptr, sizeof(U64)))
  {
    // Write 32-bit data to reach 64-bit alignment
    if (p_fifo_cur.u32ptr < p_fifo_end.u32ptr)
    {
      *p_fifo_cur.u32ptr++ = txval.u32[0];
    }
  }

  // Write 64-bit-aligned data
  while (p_fifo_cur.u64ptr < p_fifo_end.u64ptr)
  {
    *p_fifo_cur.u64ptr++ = txval.u64;
  }

  // Write remaining 32-bit data if some
  if (p_fifo_cur.u32ptr < p_fifo_end.u32ptr)
  {
    *p_fifo_cur.u32ptr++ = txval.u32[0];
  }

  // Write remaining 16-bit data if some
  if (p_fifo_cur.u16ptr < p_fifo_end.u16ptr)
  {
    *p_fifo_cur.u16ptr++ = txval.u16[0];
  }

  // Write remaining 8-bit data if some
  if (p_fifo_cur.u8ptr < p_fifo_end.u8ptr)
  {
    *p_fifo_cur.u8ptr++ = txval.u8[0];
  }

#else

  // Write remaining 8-bit data if some
  while (p_fifo_cur.u8ptr < p_fifo_end.u8ptr)
  {
    *p_fifo_cur.u8ptr++ = txval.u8[0];
  }

#endif  // !__OPTIMIZE_SIZE__

  // Compute the number of non-written bytes
  data_length -= p_fifo_cur.u8ptr - pep_fifo[p].u8ptr;

  // Save current position in FIFO data register
  pep_fifo[p].u8ptr = p_fifo_cur.u8ptr;

  // Return the number of non-written bytes
  return data_length;
}

//! host_write_p_txpacket
//!
//!  This function writes the buffer pointed to by txbuf to the selected
//!  pipe FIFO, using as few accesses as possible.
//!
//! @param p            Number of the addressed pipe
//! @param txbuf        Address of buffer to read
//! @param data_length  Number of bytes to write
//! @param ptxbuf       NULL or pointer to the buffer address to update
//!
//! @return             Number of non-written bytes
//!
//! @note The selected pipe FIFO may be written in several steps by calling
//! host_write_p_txpacket several times.
//!
//! @warning Invoke Host_reset_pipe_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be written in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 host_write_p_txpacket(U8 p, const void *txbuf, U32 data_length, const void **ptxbuf)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionVPtr   p_fifo;
  UnionCPtr   txbuf_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCPtr  txbuf_end;
#else
  UnionCPtr   txbuf_end;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for copy loops and limit the number of bytes to copy
  p_fifo.u8ptr = pep_fifo[p].u8ptr;
  txbuf_cur.u8ptr = txbuf;
  txbuf_end.u8ptr = txbuf_cur.u8ptr +
                    min(data_length, Host_get_pipe_size(p) - Host_byte_count(p));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  txbuf_end.u16ptr = (U16 *)Align_down((U32)txbuf_end.u8ptr, sizeof(U16));
  txbuf_end.u32ptr = (U32 *)Align_down((U32)txbuf_end.u16ptr, sizeof(U32));
  txbuf_end.u64ptr = (U64 *)Align_down((U32)txbuf_end.u32ptr, sizeof(U64));

  // If all addresses are aligned the same way with respect to 16-bit boundaries
  if (Get_align((U32)txbuf_cur.u8ptr, sizeof(U16)) == Get_align((U32)p_fifo.u8ptr, sizeof(U16)))
  {
    // If pointer to transmission buffer is not 16-bit aligned
    if (!Test_align((U32)txbuf_cur.u8ptr, sizeof(U16)))
    {
      // Copy 8-bit data to reach 16-bit alignment
      if (txbuf_cur.u8ptr < txbuf_end.u8ptr)
      {
        // 8-bit accesses to FIFO data registers do require pointer post-increment
        *p_fifo.u8ptr++ = *txbuf_cur.u8ptr++;
      }
    }

    // If all addresses are aligned the same way with respect to 32-bit boundaries
    if (Get_align((U32)txbuf_cur.u16ptr, sizeof(U32)) == Get_align((U32)p_fifo.u16ptr, sizeof(U32)))
    {
      // If pointer to transmission buffer is not 32-bit aligned
      if (!Test_align((U32)txbuf_cur.u16ptr, sizeof(U32)))
      {
        // Copy 16-bit data to reach 32-bit alignment
        if (txbuf_cur.u16ptr < txbuf_end.u16ptr)
        {
          // 16-bit accesses to FIFO data registers do require pointer post-increment
          *p_fifo.u16ptr++ = *txbuf_cur.u16ptr++;
        }
      }

      // If pointer to transmission buffer is not 64-bit aligned
      if (!Test_align((U32)txbuf_cur.u32ptr, sizeof(U64)))
      {
        // Copy 32-bit data to reach 64-bit alignment
        if (txbuf_cur.u32ptr < txbuf_end.u32ptr)
        {
          // 32-bit accesses to FIFO data registers do not require pointer post-increment
          *p_fifo.u32ptr = *txbuf_cur.u32ptr++;
        }
      }

      // Copy 64-bit-aligned data
      while (txbuf_cur.u64ptr < txbuf_end.u64ptr)
      {
        // 64-bit accesses to FIFO data registers do not require pointer post-increment
        *p_fifo.u64ptr = *txbuf_cur.u64ptr++;
      }

      // Copy 32-bit-aligned data
      if (txbuf_cur.u32ptr < txbuf_end.u32ptr)
      {
        // 32-bit accesses to FIFO data registers do not require pointer post-increment
        *p_fifo.u32ptr = *txbuf_cur.u32ptr++;
      }
    }

    // Copy remaining 16-bit data if some
    while (txbuf_cur.u16ptr < txbuf_end.u16ptr)
    {
      // 16-bit accesses to FIFO data registers do require pointer post-increment
      *p_fifo.u16ptr++ = *txbuf_cur.u16ptr++;
    }
  }

#endif  // !__OPTIMIZE_SIZE__

  // Copy remaining 8-bit data if some
  while (txbuf_cur.u8ptr < txbuf_end.u8ptr)
  {
    // 8-bit accesses to FIFO data registers do require pointer post-increment
    *p_fifo.u8ptr++ = *txbuf_cur.u8ptr++;
  }

  // Save current position in FIFO data register
  pep_fifo[p].u8ptr = p_fifo.u8ptr;

  // Return the updated buffer address and the number of non-copied bytes
  if (ptxbuf) *ptxbuf = txbuf_cur.u8ptr;
  return data_length - (txbuf_cur.u8ptr - (U8 *)txbuf);
}

//! host_read_p_rxpacket
//!
//!  This function reads the selected pipe FIFO to the buffer pointed to by
//!  rxbuf, using as few accesses as possible.
//!
//! @param p            Number of the addressed pipe
//! @param rxbuf        Address of buffer to write
//! @param data_length  Number of bytes to read
//! @param prxbuf       NULL or pointer to the buffer address to update
//!
//! @return             Number of non-read bytes
//!
//! @note The selected pipe FIFO may be read in several steps by calling
//! host_read_p_rxpacket several times.
//!
//! @warning Invoke Host_reset_pipe_fifo_access before this function when at
//! FIFO beginning whether or not the FIFO is to be read in several steps.
//!
//! @warning Do not mix calls to this function with calls to indexed macros.
//!
U32 host_read_p_rxpacket(U8 p, void *rxbuf, U32 data_length, void **prxbuf)
{
  // Use aggregated pointers to have several alignments available for a same address
  UnionCVPtr  p_fifo;
  UnionPtr    rxbuf_cur;
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  StructCPtr  rxbuf_end;
#else
  UnionCPtr   rxbuf_end;
#endif  // !__OPTIMIZE_SIZE__

  // Initialize pointers for copy loops and limit the number of bytes to copy
  p_fifo.u8ptr = pep_fifo[p].u8ptr;
  rxbuf_cur.u8ptr = rxbuf;
  rxbuf_end.u8ptr = rxbuf_cur.u8ptr + min(data_length, Host_byte_count(p));
#if (!defined __OPTIMIZE_SIZE__) || !__OPTIMIZE_SIZE__  // Auto-generated when GCC's -Os command option is used
  rxbuf_end.u16ptr = (U16 *)Align_down((U32)rxbuf_end.u8ptr, sizeof(U16));
  rxbuf_end.u32ptr = (U32 *)Align_down((U32)rxbuf_end.u16ptr, sizeof(U32));
  rxbuf_end.u64ptr = (U64 *)Align_down((U32)rxbuf_end.u32ptr, sizeof(U64));

  // If all addresses are aligned the same way with respect to 16-bit boundaries
  if (Get_align((U32)rxbuf_cur.u8ptr, sizeof(U16)) == Get_align((U32)p_fifo.u8ptr, sizeof(U16)))
  {
    // If pointer to reception buffer is not 16-bit aligned
    if (!Test_align((U32)rxbuf_cur.u8ptr, sizeof(U16)))
    {
      // Copy 8-bit data to reach 16-bit alignment
      if (rxbuf_cur.u8ptr < rxbuf_end.u8ptr)
      {
        // 8-bit accesses to FIFO data registers do require pointer post-increment
        *rxbuf_cur.u8ptr++ = *p_fifo.u8ptr++;
      }
    }

    // If all addresses are aligned the same way with respect to 32-bit boundaries
    if (Get_align((U32)rxbuf_cur.u16ptr, sizeof(U32)) == Get_align((U32)p_fifo.u16ptr, sizeof(U32)))
    {
      // If pointer to reception buffer is not 32-bit aligned
      if (!Test_align((U32)rxbuf_cur.u16ptr, sizeof(U32)))
      {
        // Copy 16-bit data to reach 32-bit alignment
        if (rxbuf_cur.u16ptr < rxbuf_end.u16ptr)
        {
          // 16-bit accesses to FIFO data registers do require pointer post-increment
          *rxbuf_cur.u16ptr++ = *p_fifo.u16ptr++;
        }
      }

      // If pointer to reception buffer is not 64-bit aligned
      if (!Test_align((U32)rxbuf_cur.u32ptr, sizeof(U64)))
      {
        // Copy 32-bit data to reach 64-bit alignment
        if (rxbuf_cur.u32ptr < rxbuf_end.u32ptr)
        {
          // 32-bit accesses to FIFO data registers do not require pointer post-increment
          *rxbuf_cur.u32ptr++ = *p_fifo.u32ptr;
        }
      }

      // Copy 64-bit-aligned data
      while (rxbuf_cur.u64ptr < rxbuf_end.u64ptr)
      {
        // 64-bit accesses to FIFO data registers do not require pointer post-increment
        *rxbuf_cur.u64ptr++ = *p_fifo.u64ptr;
      }

      // Copy 32-bit-aligned data
      if (rxbuf_cur.u32ptr < rxbuf_end.u32ptr)
      {
        // 32-bit accesses to FIFO data registers do not require pointer post-increment
        *rxbuf_cur.u32ptr++ = *p_fifo.u32ptr;
      }
    }

    // Copy remaining 16-bit data if some
    while (rxbuf_cur.u16ptr < rxbuf_end.u16ptr)
    {
      // 16-bit accesses to FIFO data registers do require pointer post-increment
      *rxbuf_cur.u16ptr++ = *p_fifo.u16ptr++;
    }
  }

#endif  // !__OPTIMIZE_SIZE__

  // Copy remaining 8-bit data if some
  while (rxbuf_cur.u8ptr < rxbuf_end.u8ptr)
  {
    // 8-bit accesses to FIFO data registers do require pointer post-increment
    *rxbuf_cur.u8ptr++ = *p_fifo.u8ptr++;
  }

  // Save current position in FIFO data register
  pep_fifo[p].u8ptr = (volatile U8 *)p_fifo.u8ptr;

  // Return the updated buffer address and the number of non-copied bytes
  if (prxbuf) *prxbuf = rxbuf_cur.u8ptr;
  return data_length - (rxbuf_cur.u8ptr - (U8 *)rxbuf);
}

#endif  // USB_HOST_FEATURE == true
