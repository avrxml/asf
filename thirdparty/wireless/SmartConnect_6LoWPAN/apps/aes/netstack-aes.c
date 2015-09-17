/*
 * Copyright (c) 2012, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/rime/rimestats.h"

#include "netstack-aes.h"
#include "aes-ccm.h"

#include <string.h>
#include <stdio.h>

extern const struct network_driver NETSTACK_AES_NETWORK;
extern const struct mac_driver NETSTACK_AES_MAC;

#define HEADER_LENGTH 16 /* This is actually protocol dependent, but
                           this setting works for IPv6 with 6lowpan
                           adaptation over 802.15.4. */
#define NONCE_LEN 13     /* We use the 802.15.4 header as a nonce,
                           fixed at 13 bytes. */
#define AES_MICLEN NETSTACK_AES_MICLEN     /* Control level of authentication
                           security. Less than 4 is considered
                           insecure. */

static uint8_t key[16] =  { 0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0 };

#if NETSTACK_AES_CONF_MASTER_KEY
static uint8_t master_key[16] = { 0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0 };
#endif /* NETSTACK_AES_CONF_MASTER_KEY */

static uint8_t key_set;
static uint8_t active = 1;
static uint8_t payload_tmp[PACKETBUF_SIZE + AES_MICLEN];
/*---------------------------------------------------------------------------*/
void
netstack_aes_set_active(int a)
{
  active = a;
}
/*---------------------------------------------------------------------------*/
/* This function copies data into the data portion of the
   packetbuf. The function must retain and restore the packet
   attributes, since they are removed by packetbuf_copyfrom().
*/
static void
copy_packet(const uint8_t *data, int datalen)
{
  struct packetbuf_attr attrs[PACKETBUF_NUM_ATTRS];
  struct packetbuf_addr addrs[PACKETBUF_NUM_ADDRS];
  packetbuf_attr_copyto(attrs, addrs);
  packetbuf_copyfrom(data, datalen);
  packetbuf_attr_copyfrom(attrs, addrs);
}
/*---------------------------------------------------------------------------*/
int
netstack_aes_encrypt(void)
{
  /* This function is called by the network layer to send packets. We
     encrypt the packet from the packetbuf, put it into the temporary
     buffer payload_tmp, then copy it into the packetbuf. */

  uint8_t *payload;
  uint16_t payload_len;
  uint8_t i;
  long ret;
  uint8_t nonce[NONCE_LEN];

  if(!active) {
    return packetbuf_totlen();
  }

  /* Sanity check the key: signal an error if no key has been set. */
  if(!key_set) {
    printf("Error: encryption failed, no key set\n");
    packetbuf_clear();
    return 0;
  }

  /* XXX TODO: check if packet becomes too long with MIC, return
     failure. */
  packetbuf_compact();
  payload = packetbuf_hdrptr();
  payload_len = packetbuf_totlen();

  /* Generate unique nonce (per key). We assume that the header
     contains the appropriate level of uniqueness. This nonce must be
     recreated by the receiver, by only reading the cleartext
     header.

     We could potentially generate a hash of the header instead, as
     the full header may not fit in the short nonce, but since the
     header contains a sequence number in the first 13 bytes, this
     will do for now. */

  i = 0;
  while(i < sizeof(nonce)) {
    int c =
      (sizeof(nonce) - i < HEADER_LENGTH) ? sizeof(nonce) - i :
      HEADER_LENGTH;
    memcpy(&nonce[i], payload, c);
    i += c;
  }

  ret = aes_ccm_encrypt(key, nonce, &((unsigned char*)payload)[0],
                        HEADER_LENGTH,
                        &((unsigned char*)payload)[HEADER_LENGTH],
                        payload_len - HEADER_LENGTH, AES_MICLEN,
                        payload_tmp);
  ret = payload_len + AES_MICLEN;

  if(ret < 0) {
    printf("Error: encryption failed, aborting send %li\n", ret);
  } else {
    copy_packet(payload_tmp, (int)ret);
  }
  return (int)ret;
}
/*---------------------------------------------------------------------------*/
int
netstack_aes_decrypt(void)
{
  uint16_t payload_len;
  uint8_t *payload_encrypted;
  uint8_t nonce[NONCE_LEN];
  int i;
  long ret;

  /* This is called from the RDC layer. We decrypt the incoming packet. */

  if(!active) {
    return packetbuf_totlen();
  }

  if(!key_set) {
    printf("Error: decryption failed, no key set\n");
    packetbuf_clear();
    return 0;
  }

  payload_encrypted = packetbuf_dataptr();
  payload_len = packetbuf_totlen();

  if(payload_len > HEADER_LENGTH + AES_MICLEN) {
    /* Recreate nonce using cleartext header. */
    i = 0;
    while(i < sizeof(nonce)) {
      int c =
          (sizeof(nonce) - i < HEADER_LENGTH) ? sizeof(nonce) - i :
              HEADER_LENGTH;
      memcpy(&nonce[i], payload_encrypted, c);
      i += c;
    }

    ret = aes_ccm_decrypt(key, nonce, payload_encrypted, HEADER_LENGTH,
                          &payload_encrypted[HEADER_LENGTH],
                          payload_len - HEADER_LENGTH, AES_MICLEN,
                          &((unsigned char*)payload_tmp)[HEADER_LENGTH]);
#if NETSTACK_AES_CONF_MASTER_KEY
    if(ret < 0) {
      ret = aes_ccm_decrypt(master_key, nonce, payload_encrypted, HEADER_LENGTH,
                            &payload_encrypted[HEADER_LENGTH],
                            payload_len - HEADER_LENGTH, AES_MICLEN,
                            &((unsigned char*)payload_tmp)[HEADER_LENGTH]);
    }
#endif /* NETSTACK_AES_CONF_MASTER_KEY */
    memcpy(payload_tmp, payload_encrypted, HEADER_LENGTH);

    if(ret < 0) {
      /*      printf("Error: decryption failed, dropping packet %li\n", ret);*/
      RIMESTATS_ADD(decryptiondrops);
      payload_len = 0;
    } else {
      payload_len = HEADER_LENGTH + (int)ret;
    }
    copy_packet(payload_tmp, payload_len);
  }
  return (int)payload_len;
}
/*---------------------------------------------------------------------------*/
int
netstack_aes_verify(void)
{
  uint16_t payload_len;
  uint8_t *payload_encrypted;
  uint8_t nonce[NONCE_LEN];
  int i, ret;

  /* This is called from the RDC layer. We integrity check the incoming,
   * and return a positive number at success, or 0 at failure. */

  if(!key_set) {
    printf("Error: verify failed, no key set\n");
    return 0;
  }

  payload_encrypted = packetbuf_dataptr();
  payload_len = packetbuf_totlen();

  if(payload_len > HEADER_LENGTH) {
    /* Recreate nonce using cleartext header. */
    i = 0;
    while(i < sizeof(nonce)) {
      int c =
          (sizeof(nonce) - i < HEADER_LENGTH) ? sizeof(nonce) - i :
              HEADER_LENGTH;
      memcpy(&nonce[i], payload_encrypted, c);
      i += c;
    }

    ret = aes_ccm_verify(key, nonce, payload_encrypted, HEADER_LENGTH,
                         &payload_encrypted[HEADER_LENGTH],
                         payload_len - HEADER_LENGTH, AES_MICLEN);

    if(ret <= 0) {
      printf("Error: verify failed %i\n", ret);
      return 0;
    } else {
      return 1;
    }
  }
  return 2; /* Success: nothing to verify */
}
/*---------------------------------------------------------------------------*/
void
netstack_aes_init(void)
{
  printf("Netstack AES link layer encryption enabled\n");
  key_set = 0;
  active = 1;
}
/*---------------------------------------------------------------------------*/
void
netstack_aes_set_key(const uint8_t *aes_key)
{
  memcpy(key, aes_key, sizeof(key));
  key_set = 1;
}
/*---------------------------------------------------------------------------*/
void
netstack_aes_set_master_key(const uint8_t *aes_key)
{
#if NETSTACK_AES_CONF_MASTER_KEY
  memcpy(master_key, aes_key, sizeof(key));
#endif /* NETSTACK_AES_CONF_MASTER_KEY */
}
/*---------------------------------------------------------------------------*/
