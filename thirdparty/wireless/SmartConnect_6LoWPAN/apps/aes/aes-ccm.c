/*
 * Copyright (c) 2012, Thingsquare, www.thingsquare.com.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Fredrik Osterlind <fredrik@thingsquare.com>
 */

#include "contiki.h"
#include "net/ip/uip.h"

#include "aes-ccm.h"
#include "aes.h"

#include "dev/watchdog.h"

#include <stdio.h>
#include <string.h>

/*
 * CCM over AES-128 (RFC3610, http://www.ietf.org/rfc/rfc3610.txt).
 */

#define BLOCK_SIZE 16
#define L_SIZELEN 2 /* Size field length: 2 to 8 bytes  */
#define NONCE_LEN (15-L_SIZELEN) /* Nonce length: fixed to 13 bytes */

/*#define PRINTF(...) printf(__VA_ARGS__)*/
#define PRINTF(...)

/* CBC-MAC */
#define PRINT_CBCMAC_BLOCKS 0
#if PRINT_CBCMAC_BLOCKS
#define CBCMAC_PRINTF(...) PRINTF(__VA_ARGS__)
static int cbcmac_blockcount;
#else /* PRINT_CBCMAC_BLOCKS */
#define CBCMAC_PRINTF(...)
#endif /* PRINT_CBCMAC_BLOCKS */

static unsigned char cbcmac_xor[BLOCK_SIZE];

/* CTR: payload and MIC */
#define PRINT_CTR_BLOCKS 0
#if PRINT_CTR_BLOCKS
#define CTR_PRINTF(...) PRINTF(__VA_ARGS__)
#else /* PRINT_CTR_BLOCKS */
#define CTR_PRINTF(...)
#endif /* PRINT_CTR_BLOCKS */

#define PRINT_CTR_MIC_BLOCKS 0
#if PRINT_CTR_MIC_BLOCKS
#define CTR_MIC_PRINTF(...) PRINTF(__VA_ARGS__)
#else /* PRINT_CTR_MIC_BLOCKS */
#define CTR_MIC_PRINTF(...)
#endif /* PRINT_CTR_MIC_BLOCKS */

void aes_ccm_memcpy(const unsigned char *dest, const unsigned char *from,
                    int len);
#ifndef aes_ccm_memcpy
#define aes_ccm_memcpy memcpy
#endif /* aes_ccm_memcpy */

/*---------------------------------------------------------------------------*/
static void
cbcmac_clear(void)
{
#if PRINT_CBCMAC_BLOCKS
  cbcmac_blockcount = 0;
#endif /* PRINT_CBCMAC_BLOCKS */
  /* clear last cipher block */
  memset(cbcmac_xor, 0, sizeof(cbcmac_xor));
}
/*---------------------------------------------------------------------------*/
static void
cbcmac_append(const unsigned char *key, unsigned char *block)
{
  int i;

#if PRINT_CBCMAC_BLOCKS
  CBCMAC_PRINTF("B_%d: ", cbcmac_blockcount);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CBCMAC_PRINTF("%02x", block[i]);
  }
  CBCMAC_PRINTF("\n");
#endif /* PRINT_CBCMAC_BLOCKS */

  /* xor with last cipher block */
  for(i = 0; i < BLOCK_SIZE; i++) {
    block[i] ^= cbcmac_xor[i];
  }

#if PRINT_CBCMAC_BLOCKS
  CBCMAC_PRINTF("^_%d: ", cbcmac_blockcount);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CBCMAC_PRINTF("%02x", block[i]);
  }
  CBCMAC_PRINTF("\n");
#endif /* PRINT_CBCMAC_BLOCKS */

  /* encrypt */
  aes_encrypt(block, key);
  memcpy(cbcmac_xor, block, BLOCK_SIZE);

#if PRINT_CBCMAC_BLOCKS
  CBCMAC_PRINTF("X_%d: ", cbcmac_blockcount);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CBCMAC_PRINTF("%02x", block[i]);
  }
  CBCMAC_PRINTF("\n");
  cbcmac_blockcount++;
#endif /* PRINT_CBCMAC_BLOCKS */
}
/*---------------------------------------------------------------------------*/
static void
ctr_next_ctr_block(const unsigned char *key, const unsigned char *nonce,
                  uint16_t counter, unsigned char *outbuf)
{
  uint8_t flags;
  uint16_t tmp;

  /* Prepare CTR block */
  memset(outbuf, 0, BLOCK_SIZE);

  /* CTR block: Flags field */
  flags = 1 * (L_SIZELEN - 1); /* size. length */
  memcpy(&outbuf[0], &flags, 1);

  /* CTR block: Nonce */
  memcpy(&outbuf[1], nonce, NONCE_LEN);

  /* CTR block: Counter */
  tmp = UIP_HTONS(counter);
  memcpy(&outbuf[BLOCK_SIZE - L_SIZELEN], &tmp, L_SIZELEN); /* MSB. */

#if PRINT_CTR_BLOCKS
  CTR_PRINTF("A_%d: ", counter);
  int i;
  for(i = 0; i < BLOCK_SIZE; i++) {
    CTR_PRINTF("%02x", outbuf[i]);
  }
  CTR_PRINTF("\n");
#endif /* PRINT_CTR_BLOCKS */

  /* Encrypt CTR block */
  aes_encrypt(outbuf, key);

  CTR_PRINTF("CTR: Counter block encrypted\n");
#if PRINT_CTR_BLOCKS
  CTR_PRINTF("S_%d: ", counter);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CTR_PRINTF("%02x", outbuf[i]);
  }
  CTR_PRINTF("\n");
#endif /* PRINT_CTR_BLOCKS */
}
/*---------------------------------------------------------------------------*/
static int
cbcmac_calc(const unsigned char *key, const unsigned char *nonce,
                       const unsigned char *adata, unsigned long adata_len,
                       const unsigned char *payload, unsigned long payload_len,
                       int mic_len, unsigned char *outbuf)
{
  /*
   * Return value:
   * Number of encrypted bytes at success, negative value at failure.
   */

  unsigned char BUF[BLOCK_SIZE];
  uint16_t tmp, tmp2;
  uint8_t flags;

  cbcmac_clear();

  /* Block B_0 */
  memset(BUF, 0, sizeof(BUF));

  /* B_0: Flags field */
  flags = 0;
  flags += 64 * (adata_len > 0 ? 1 : 0); /* contains associated data */
  flags += 8 * ((mic_len - 2) / 2); /* auth. length */
  flags += 1 * (L_SIZELEN - 1); /* size. length */
  memcpy(&BUF[0], &flags, 1);

  /* B_0: Nonce */
  memcpy(&BUF[1], nonce, NONCE_LEN);

  /* B_0: Size field */
  tmp2 = (uint16_t) payload_len; /* XXX Max size supported is 0xFFFF */
  tmp = UIP_HTONS(tmp2);
  memcpy(&BUF[BLOCK_SIZE - L_SIZELEN], &tmp, L_SIZELEN); /* MSB */
  CBCMAC_PRINTF("CBC-MAC: First block prepared\n");
  cbcmac_append(key, BUF);

  /* B_1..n: auth. blocks */
  if(adata_len > 0) {
    signed long left, idx;
    int len;

    CBCMAC_PRINTF("CBC-MAC: Have auth. data\n");
    memset(BUF, 0, sizeof(BUF));
    if(adata_len < 65536) {
      /* 2 bytes data length in first auth. block */
      tmp2 = (uint16_t) adata_len;
      tmp = UIP_HTONS(tmp2);
      memcpy(&BUF[0], &tmp, 2);
      len = 2;
    } else {
      uint8_t sizefield[6];
      sizefield[0] = 0xff;
      sizefield[1] = 0xfe;
      sizefield[2] = (adata_len >> 24) & 0xff;
      sizefield[3] = (adata_len >> 16) & 0xff;
      sizefield[4] = (adata_len >> 8) & 0xff;
      sizefield[5] = (adata_len) & 0xff;
      memcpy(&BUF[0], &sizefield, sizeof(sizefield));
      len = sizeof(sizefield);
    }

    /* 16 - len bytes data in first auth. block */
    left = adata_len;
    idx = 0;
    memcpy(&BUF[len], &adata[idx],
           (left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left));
    idx += (left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left);
    left -= (left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left);

    /* 16 bytes data in subsequent auth. blocks */
    while(left > 0) {
      CBCMAC_PRINTF("CBC-MAC: Auth. block prepared\n");
      cbcmac_append(key, BUF);

      /* Auth data + padding with zeroes */
      memset(BUF, 0, sizeof(BUF));
      memcpy(&BUF[0], &adata[idx], (left > BLOCK_SIZE ? BLOCK_SIZE : left));
      idx += (left > BLOCK_SIZE ? BLOCK_SIZE : left);
      left -= (left > BLOCK_SIZE ? BLOCK_SIZE : left);

    }

    CBCMAC_PRINTF("CBC-MAC: Auth. block prepared (last)\n");
    cbcmac_append(key, BUF);
  }

  /* B_n..m: message blocks */
  if(payload_len > 0) {
    unsigned long left, idx;

    CBCMAC_PRINTF("CBC-MAC: Have payload data\n");
    memset(BUF, 0, sizeof(BUF));

    left = payload_len;
    idx = 0;
    while(left > 0) {

      /* Auth data + padding with zeroes */
      memset(BUF, 0, sizeof(BUF));
      memcpy(&BUF[0], &payload[idx], (left > BLOCK_SIZE ? BLOCK_SIZE : left));

      idx += (left > BLOCK_SIZE ? BLOCK_SIZE : left);
      left -= (left > BLOCK_SIZE ? BLOCK_SIZE : left);

      CBCMAC_PRINTF("CBC-MAC: Payload block prepared\n");
      cbcmac_append(key, BUF);
    }
  }

  /* Copy mic_len bytes of CBC-MAC to outbuf */
  memcpy(outbuf, BUF, mic_len);
  return mic_len;
}
/*---------------------------------------------------------------------------*/
static int
cbcmac_verify(const unsigned char *key, const unsigned char *nonce,
                         const unsigned char *adata, unsigned long adata_len,
                         const unsigned char *cipher, unsigned long cipher_len,
                         int mic_len, unsigned char *outbuf)
{
  /*
   * Return value:
   * Number of encrypted bytes at success, negative value at failure.
   */

  unsigned char BUF[BLOCK_SIZE];
  uint16_t tmp, tmp2;
  uint8_t flags;

  cbcmac_clear();

  /* Block B_0 */
  memset(BUF, 0, sizeof(BUF));

  /* B_0: Flags field */
  flags = 0;
  flags += 64 * (adata_len > 0 ? 1 : 0); /* contains associated data */
  flags += 8 * ((mic_len - 2) / 2); /* auth. length */
  flags += 1 * (L_SIZELEN - 1); /* size. length */
  memcpy(&BUF[0], &flags, 1);

  /* B_0: Nonce */
  memcpy(&BUF[1], nonce, NONCE_LEN);

  /* B_0: Size field */
  tmp2 = (uint16_t) cipher_len; /* Note: limits cipher length to 65536 bytes */
  tmp = UIP_HTONS(tmp2);
  memcpy(&BUF[BLOCK_SIZE - L_SIZELEN], &tmp, L_SIZELEN); /* MSB */
  CBCMAC_PRINTF("CBC-MAC: First block prepared\n");
  cbcmac_append(key, BUF);

  /* B_1..n: auth. blocks */
  if(adata_len > 0) {
    unsigned long left, idx;
    int len;

    CBCMAC_PRINTF("CBC-MAC: Have auth. data\n");
    memset(BUF, 0, sizeof(BUF));
    if(adata_len < 65536) {
      /* 2 bytes data length in first auth. block */
      tmp2 = (uint16_t) adata_len;
      tmp = UIP_HTONS(tmp2);
      memcpy(&BUF[0], &tmp, 2);
      len = 2;
    } else {
      uint8_t sizefield[6];
      sizefield[0] = 0xff;
      sizefield[1] = 0xfe;
      sizefield[2] = (adata_len >> 24) & 0xff;
      sizefield[3] = (adata_len >> 16) & 0xff;
      sizefield[4] = (adata_len >> 8) & 0xff;
      sizefield[5] = (adata_len) & 0xff;
      memcpy(&BUF[0], &sizefield, sizeof(sizefield));
      len = sizeof(sizefield);
    }

    /* 16 - len bytes data in first auth. block */
    left = adata_len;
    idx = 0;
    memcpy(&BUF[len], &adata[idx],
           ((unsigned long)left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left));
    idx += ((unsigned long)left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left);
    left -= ((unsigned long)left > (BLOCK_SIZE - len) ? (BLOCK_SIZE - len) : left);

    /* 16 bytes data in subsequent auth. blocks */
    while(left > 0) {
      CBCMAC_PRINTF("CBC-MAC: Auth. block prepared\n");
      cbcmac_append(key, BUF);

      //watchdog_periodic();
	  wdt_reset_count();
      /* Auth data + padding with zeroes */
      memset(BUF, 0, sizeof(BUF));
      memcpy(&BUF[0], &adata[idx], (left > BLOCK_SIZE ? BLOCK_SIZE : left));
      idx += (left > BLOCK_SIZE ? BLOCK_SIZE : left);
      left -= (left > BLOCK_SIZE ? BLOCK_SIZE : left);
    }

    CBCMAC_PRINTF("CBC-MAC: Auth. block prepared (last)\n");
    cbcmac_append(key, BUF);
  }

  /* B_n..m: message blocks */
  if(cipher_len > 0) {
    unsigned long left, idx, counter, i;

    CBCMAC_PRINTF("CBC-MAC: Have cipher data\n");
    memset(BUF, 0, sizeof(BUF));

    left = cipher_len;
    idx = 0;
    counter = 1; /* S_1 .. S_n */
    while(left > 0) {

      /* Decrypt block right now */
      ctr_next_ctr_block(key, nonce, counter, BUF);
      /* XOR with payload block */
      for(i = 0; i < BLOCK_SIZE; i++) {
        if(idx + i >= cipher_len) {
          BUF[i] = 0;
        } else {
          BUF[i] ^= cipher[idx + i];
        }
      }
      counter++;

      idx += (left > BLOCK_SIZE ? BLOCK_SIZE : left);
      left -= (left > BLOCK_SIZE ? BLOCK_SIZE : left);

      CBCMAC_PRINTF("CBC-MAC: Payload block prepared\n");
      cbcmac_append(key, BUF);
    }
  }

  /* Copy mic_len bytes of CBC-MAC to outbuf */
  memcpy(outbuf, BUF, mic_len);
  return mic_len;
}
/*---------------------------------------------------------------------------*/
static unsigned long
ctr_payload(const unsigned char *key, const unsigned char *nonce,
            const unsigned char *payload, unsigned long payload_len,
            unsigned char *outbuf)
{
  /*
   * Return value:
   * Number of encrypted bytes at success, negative value at failure.
   */

  unsigned char BUF[BLOCK_SIZE];
  unsigned long left, idx;
  int i;
  uint16_t counter;

  /* Encrypt payload */
  left = payload_len;
  idx = 0;
  counter = 1; /* S_1 .. S_n */
  while(left > 0) {
    ctr_next_ctr_block(key, nonce, counter, BUF);

    /* XOR with payload block */
    for(i = 0; i < BLOCK_SIZE; i++) {
      if(idx + i >= payload_len) {
        break;
      }
      BUF[i] ^= payload[idx + i];
    }
    CTR_PRINTF("CTR: Payload XORed with counter block\n");
#if PRINT_CTR_BLOCKS
    CTR_PRINTF("X_%d: ", counter);
    for(i = 0; i < BLOCK_SIZE; i++) {
      if(idx + i >= payload_len) {
        break;
      }
      CTR_PRINTF("%02x", BUF[i]);
    }
    CTR_PRINTF("\n");
#endif /* PRINT_CTR_BLOCKS */

    aes_ccm_memcpy(&outbuf[idx], BUF, (left > BLOCK_SIZE ? BLOCK_SIZE : left));

    idx += (left > BLOCK_SIZE ? BLOCK_SIZE : left);
    left -= (left > BLOCK_SIZE ? BLOCK_SIZE : left);

    counter++;
  }

  return idx;
}
/*---------------------------------------------------------------------------*/
static int
ctr_mic(const unsigned char *key, const unsigned char *nonce,
        const unsigned char *cbcmac, int mic_len, unsigned char *outbuf)
{
  /*
   * Return value:
   * Number of encrypted bytes at success, negative value at failure.
   */

  unsigned char BUF[BLOCK_SIZE];
  int i;
  uint16_t tmp;
  uint8_t flags;

  /* Prepare CTR block */
  memset(BUF, 0, sizeof(BUF));

  /* CTR block: Flags field */
  flags = 1 * (L_SIZELEN - 1); /* size. length */
  memcpy(&BUF[0], &flags, 1);

  /* CTR block: Nonce */
  memcpy(&BUF[1], nonce, NONCE_LEN);

  /* CTR block: Counter */
  tmp = UIP_HTONS(0); /* S_0: counter is 0 */
  memcpy(&BUF[BLOCK_SIZE - L_SIZELEN], &tmp, L_SIZELEN); /* MSB. */

#if PRINT_CTR_MIC_BLOCKS
  CTR_MIC_PRINTF("A_%d: ", 0);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CTR_MIC_PRINTF("%02x", BUF[i]);
  }
  CTR_MIC_PRINTF("\n");
#endif /* PRINT_CTR_MIC_BLOCKS */

  /* Encrypt CTR block */
  aes_encrypt(BUF, key);
  CTR_MIC_PRINTF("CTRMIC: Counter block encrypted\n");
#if PRINT_CTR_MIC_BLOCKS
  CTR_MIC_PRINTF("S_%d: ", 0);
  for(i = 0; i < BLOCK_SIZE; i++) {
    CTR_MIC_PRINTF("%02x", BUF[i]);
  }
  CTR_MIC_PRINTF("\n");
#endif /* PRINT_CTR_MIC_BLOCKS */

  /* XOR with CBC-MAC */
  for(i = 0; i < mic_len; i++) {
    BUF[i] ^= cbcmac[i];
  }
  CTR_MIC_PRINTF("CTRMIC: CBC-MAC XORed with counter block\n");
#if PRINT_CTR_MIC_BLOCKS
  CTR_MIC_PRINTF("X_%d: ", 0);
  for(i = 0; i < mic_len; i++) {
    CTR_MIC_PRINTF("%02x", BUF[i]);
  }
  CTR_MIC_PRINTF("\n");
#endif /* PRINT_CTR_MIC_BLOCKS */

  memcpy(outbuf, BUF, mic_len);

  return mic_len;
}
/*---------------------------------------------------------------------------*/
long
aes_ccm_encrypt(const unsigned char *key, const unsigned char *nonce,
                const unsigned char *adata, unsigned long adata_len,
                const unsigned char *payload, unsigned long payload_len,
                int mic_len, unsigned char *outbuf)
{
  /*
   * Return value:
   * Number of encrypted bytes at success, negative value at failure.
   */

  int i, cbcmac_len;
  long ctr_len;

  /* MIC length: 4, 6, 8, 10, 12, 14, or 16 bytes */
  unsigned char cbcmac[16];

  /* Copy adata (header) */
  memcpy(&outbuf[0], adata, adata_len);


  /* Authentication: calculate CBC-MAC (MIC) over header and payload */
  cbcmac_len = cbcmac_calc(key, nonce, adata, adata_len, payload, payload_len,
                           mic_len, cbcmac);
  if(cbcmac_len < 0 || cbcmac_len != mic_len) {
    return -1;
  }
  PRINTF("CBCMAC %d bytes:\n", cbcmac_len);
  for(i = 0; i < cbcmac_len; i++) {
    PRINTF("%02x", cbcmac[i]);
  }
  PRINTF("\n");


  /* Encryption: encrypt payload using CTR */
  ctr_len = ctr_payload(key, nonce, payload, payload_len, &outbuf[adata_len]);
  if(ctr_len < 0) {
    return -2;
  }
  PRINTF("CTR payload cipher %d bytes:\n", ctr_len);
  for(i = 0; i < ctr_len; i++) {
    PRINTF("%02x", outbuf[adata_len+i]);
  }
  PRINTF("\n");


  /* Encryption: encrypt MIC */
  mic_len = ctr_mic(key, nonce, cbcmac, mic_len, &outbuf[adata_len + payload_len]);
  if(mic_len < 0) {
    return -3;
  }
  PRINTF("CTR MIC %d bytes:\n", mic_len);
  for(i = 0; i < mic_len; i++) {
    PRINTF("%02x", outbuf[adata_len+payload_len+i]);
  }
  PRINTF("\n");

  return (long)(adata_len + payload_len + mic_len);
}
/*---------------------------------------------------------------------------*/
long
aes_ccm_decrypt(const unsigned char *key, const unsigned char *nonce,
                    const unsigned char *adata, unsigned long adata_len,
                    const unsigned char *ciphermic, signed long ciphermic_len,
                    int mic_len, unsigned char *outbuf)
{
  int cbcmac_len, cbcmac_len2;
  unsigned long i; 
  unsigned long ctr_len;

  /* MIC length: 4, 6, 8, 10, 12, 14, or 16 bytes */
  unsigned char cbcmac[16];
  unsigned char cbcmac2[16]; /* recomputed MIC */

  /*
   * Return value:
   * Number of plaintext bytes at success, negative value at failure.
   */

  /* Santity-check: Cipher + MIC must be equal to or longer than the MIC itself */
  if(ciphermic_len < mic_len) {
    return -6;
  }

  /* Decryption: decrypt MIC */
  cbcmac_len = ctr_mic(key, nonce, &ciphermic[ciphermic_len - mic_len], mic_len,
                       cbcmac);
  if(cbcmac_len < 0 || cbcmac_len != mic_len) {
    return -1;
  }
  PRINTF("CBCMAC %d bytes:\n", mic_len);
  for(i = 0; i < mic_len; i++) {
    PRINTF("%02x", cbcmac[i]);
  }
  PRINTF("\n");


  /* Decryption: decrypt payload using CTR */
  ctr_len = ctr_payload(key, nonce, ciphermic, ciphermic_len - mic_len,
                        &outbuf[0]);
  if(ctr_len == 0) {
    return -2;
  }
  PRINTF("CTR payload plaintext %d bytes:\n", ctr_len);
  for(i = 0; i < ctr_len; i++) {
    PRINTF("%02x", outbuf[i]);
  }
  PRINTF("\n");


  /* Authentication: re-calculate CBC-MAC (MIC) over header and payload */
  cbcmac_len2 = cbcmac_calc(key, nonce, adata, adata_len, outbuf, ctr_len,
                            mic_len, cbcmac2);
  if(cbcmac_len2 < 0 || cbcmac_len2 != mic_len) {
    return -3;
  }
  PRINTF("CBCMAC %d bytes:\n", cbcmac_len2);
  for(i = 0; i < cbcmac_len2; i++) {
    PRINTF("%02x", cbcmac2[i]);
  }
  PRINTF("\n");

  /* Verify that MICs match */
  if(cbcmac_len != cbcmac_len2) {
    return -4;
  }
  for(i = 0; i < cbcmac_len; i++) {
    if(cbcmac[i] != cbcmac2[i]) {
      return -5;
    }
  }

  return (long)ctr_len;
}
/*---------------------------------------------------------------------------*/
int
aes_ccm_verify(const unsigned char *key, const unsigned char *nonce,
                   const unsigned char *adata, unsigned long adata_len,
                   const unsigned char *ciphermic, unsigned long ciphermic_len,
                   int mic_len)
{
  int i;
  int cbcmac_len, cbcmac_len2;

  /* MIC length: 4, 6, 8, 10, 12, 14, or 16 bytes */
  unsigned char cbcmac[16];
  unsigned char cbcmac2[16]; /* recomputed MIC */

  /*
   * Return value:
   * 1 if buffer integrity was verified, 0 otherwise.
   */

  /* Decryption: decrypt MIC */
  cbcmac_len = ctr_mic(key, nonce, &ciphermic[ciphermic_len - mic_len], mic_len,
                       cbcmac);
  if(cbcmac_len < 0 || cbcmac_len != mic_len) {
    return 0;
  }
  PRINTF("CBCMAC %d bytes:\n", mic_len);
  for(i = 0; i < mic_len; i++) {
    PRINTF("%02x", cbcmac[i]);
  }
  PRINTF("\n");

  /* Integrity check: both decrypt and calculate CBC-MAC at the same time.
   * We now decrypt the cipher on-the-fly, without storing the cleartext. */
  cbcmac_len2 = cbcmac_verify(key, nonce, adata, adata_len, ciphermic,
                              ciphermic_len - mic_len, mic_len, cbcmac2);
  if(cbcmac_len2 < 0 || cbcmac_len2 != mic_len) {
    return 0;
  }
  PRINTF("CBCMAC %d bytes:\n", cbcmac_len2);
  for(i = 0; i < cbcmac_len2; i++) {
    PRINTF("%02x", cbcmac2[i]);
  }
  PRINTF("\n");

  /* Verify that MICs match */
  if(cbcmac_len != cbcmac_len2) {
    return 0;
  }
  for(i = 0; i < cbcmac_len; i++) {
    if(cbcmac[i] != cbcmac2[i]) {
      return 0;
    }
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
