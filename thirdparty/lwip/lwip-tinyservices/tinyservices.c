/**
 *
 * \file
 *
 * \brief Tiny DNS Server.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "conf_tinyservices.h"
#include "tinyservices.h"
#include "wifi_private.h"
#include <string.h>

/**
 * Copy application supplied data into a pbuf.
 * This function can only be used to copy the equivalent of buf->tot_len data.
 *
 * @param buf pbuf to fill with data
 * @param offset logical offset into pbuf to start writing data into
 * @param dataptr application supplied data buffer
 * @param len length of the application supplied data buffer
 *
 * @return ERR_OK if successful, ERR_MEM if the pbuf is not big enough
 */
err_t pbuf_take_partial(struct pbuf *pbuf, uint32_t offset, const void *dataptr, u16_t len)
{
  struct pbuf *p;
  u16_t left;
  u16_t buf_copy_len;
  u16_t copied_total = 0;

  Assert(pbuf != NULL);
  Assert(dataptr != NULL);

  left = 0;

  if ((pbuf == NULL) || (dataptr == NULL)) {
    return ERR_ARG;
  }

  /* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
  for(p = pbuf; len != 0 && p != NULL; p = p->next) {
      if ((offset != 0) && (offset >= p->len)) {
          /* don't copy to this buffer --> on to the next */
          offset -= p->len;
      } else {
          buf_copy_len = p->len - offset;
          if (buf_copy_len > len) {
              buf_copy_len = len;
          }

          /* copy the necessary parts of the buffer */
          memcpy(&((char *)p->payload)[offset], &((char *)dataptr)[left], buf_copy_len);
          copied_total += buf_copy_len;
          left += buf_copy_len;
          len -= buf_copy_len;
          offset = 0;
      }
  }
  Assert(len == 0);

  return ERR_OK;
}

/**
 * Read a byte from a pbuf. If the read is beyond the end of the
 * pbuf chain's payload, then return an error instead.
 *
 * @param buf    pbuf to read data from.
 * @param offset logical offset into pbuf to start reading from
 * @param pval   memory to place the read byte into. 
 *
 * @return       0 if successful, 1 if the read offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_read8(struct pbuf * pbuf, uint32_t offset, uint8_t * pval)
{
    uint32_t err = 1;

    if(pbuf->tot_len > offset) {
        *pval = pbuf_get_at(pbuf, offset);
        err = 0;
    }

    return err;
}

/**
 * Write a byte to a pbuf. If the requested write is beyond the 
 * end of the pbuf chain's payload, then this function does not
 * perform the write.  It returns an error instead.
 *
 * @param buf    pbuf to write data to.
 * @param offset logical offset into pbuf to start writing to.
 * @param val    the data to write to the pbuf. 
 *
 * @return       0 if successful, 1 if the write offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_write8(struct pbuf * pbuf, uint32_t offset, uint8_t val)
{
    uint32_t err = 1;
    err_t perr = ERR_OK;
    uint8_t buf[1];

    buf[0] = val;

    if(pbuf->tot_len >= (offset+1)) {
        perr = pbuf_take_partial(pbuf, offset, buf, 1);
        if(perr == ERR_OK) {
            err = 0;
        }
    }

    return err;
}

/**
 * Write two bytes (big-endian) to a pbuf. If the requested write is 
 * beyond the * end of the pbuf chain's payload, then this function does not
 * perform the write.  It returns an error instead.
 *
 * @param buf    pbuf to write data to.
 * @param offset logical offset into pbuf to start writing to.
 * @param val    the data to write to the pbuf. 
 *
 * @return       0 if successful, 1 if the write offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_write16be(struct pbuf * pbuf, uint32_t offset, uint16_t val)
{
    uint32_t err = 1;
    err_t perr = ERR_OK;
    uint8_t buf[2];

    _wifi_put16_be(buf, val);

    if(pbuf->tot_len >= (offset+2)) {
        perr = pbuf_take_partial(pbuf, offset, buf, 2);
        if(perr == ERR_OK) {
            err = 0;
        }
    }

    return err;
}

/**
 * Read two bytes (big-endian) from a pbuf. If the read is beyond the end 
 * of the pbuf chain's payload, then return an error instead.
 *
 * @param buf    pbuf to read data from.
 * @param offset logical offset into pbuf to start reading from
 * @param pval   memory to place the read bytes into. 
 *
 * @return       0 if successful, 1 if the read offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_read16be(struct pbuf * pbuf, uint32_t offset, uint16_t * pval)
{
    uint32_t err = 1;
    uint16_t num_bytes;
    uint8_t buffer[2];

    if(pbuf->tot_len >= (offset+2)) {
      num_bytes = pbuf_copy_partial(pbuf, buffer, 2, offset);
      if(num_bytes == 2) {
        *pval = _wifi_get16_be(buffer);
        err = 0;
      }
    }
    return err;
}

/**
 * Read four bytes (little-endian) from a pbuf. If the requested read is 
 * beyond the * end of the pbuf chain's payload, then this function does not
 * perform the read.  It returns an error instead.
 *
 * @param buf    pbuf to read data from.
 * @param offset logical offset into pbuf to start reading from.
 * @param val    the data to read from the pbuf. 
 *
 * @return       0 if successful, 1 if the read offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_read32le(struct pbuf * pbuf, uint32_t offset, uint32_t * pval)
{
    uint32_t err = 1;
    uint16_t num_bytes;
    uint8_t buf[4];

    if(pbuf->tot_len >= (offset+4)) {
        num_bytes = pbuf_copy_partial(pbuf, buf, 4, offset);
        if(num_bytes == 4) {
           *pval = _wifi_get32_le(buf);
           err = 0;
        }
    }

    return err;
}

/**
 * Write four bytes (little-endian) to a pbuf. If the requested write is 
 * beyond the * end of the pbuf chain's payload, then this function does not
 * perform the write.  It returns an error instead.
 *
 * @param buf    pbuf to write data to.
 * @param offset logical offset into pbuf to start writing to.
 * @param val    the data to write to the pbuf. 
 *
 * @return       0 if successful, 1 if the write offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_write32le(struct pbuf * pbuf, uint32_t offset, uint32_t val)
{
    uint32_t err = 1;
    err_t perr = ERR_OK;
    uint8_t buf[4];

    _wifi_put32_le(buf, val);

    if(pbuf->tot_len >= (offset+4)) {
        perr = pbuf_take_partial(pbuf, offset, buf, 4);
        if(perr == ERR_OK) {
            err = 0;
        }
    }

    return err;
}

/**
 * Write four bytes (big-endian) to a pbuf. If the requested write is 
 * beyond the * end of the pbuf chain's payload, then this function does not
 * perform the write.  It returns an error instead.
 *
 * @param buf    pbuf to write data to.
 * @param offset logical offset into pbuf to start writing to.
 * @param val    the data to write to the pbuf. 
 *
 * @return       0 if successful, 1 if the write offset is beyond the
 *               end of the pbuf chain's total length.
 */
uint32_t pbuf_write32be(struct pbuf * pbuf, uint32_t offset, uint32_t val)
{
    uint32_t err = 1;
    err_t perr = ERR_OK;
    uint8_t buf[4];

    _wifi_put32_be(buf, val);

    if(pbuf->tot_len >= (offset+4)) {
        perr = pbuf_take_partial(pbuf, offset, buf, 4);
        if(perr == ERR_OK) {
            err = 0;
        }
    }

    return err;
}
