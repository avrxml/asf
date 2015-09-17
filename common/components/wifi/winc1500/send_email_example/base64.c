/**
 * \file
 *
 * \brief WINC1500 Send Email Example.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

static const char g_ccB64Tbl[64]
	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void ConvertToBase64(char *pcOutStr, const char *pccInStr, int iLen);

void ConvertToBase64(char *pcOutStr, const char *pccInStr, int iLen)
{
	const char *pccIn = (const char *)pccInStr;
	char *pcOut;
	int iCount;
	pcOut = pcOutStr;

	/* Loop in for Multiple of 24Bits and Convert to Base 64 */
	for (iCount = 0; iLen - iCount >= 3; iCount += 3, pccIn += 3) {
		*pcOut++ = g_ccB64Tbl[pccIn[0] >> 2];
		*pcOut++ = g_ccB64Tbl[((pccIn[0] & 0x03) << 4) | (pccIn[1] >> 4)];
		*pcOut++ = g_ccB64Tbl[((pccIn[1] & 0x0F) << 2) | (pccIn[2] >> 6)];
		*pcOut++ = g_ccB64Tbl[pccIn[2] & 0x3f];
	}

	/* Check if String is not multiple of 3 Bytes */
	if (iCount != iLen) {
		unsigned char ucLastByte;

		*pcOut++ = g_ccB64Tbl[pccIn[0] >> 2];
		ucLastByte = ((pccIn[0] & 0x03) << 4);

		if (iLen - iCount > 1) {
			/* If there are 2 Extra Bytes */
			ucLastByte |= (pccIn[1] >> 4);
			*pcOut++ = g_ccB64Tbl[ucLastByte];
			*pcOut++ = g_ccB64Tbl[((pccIn[1] & 0x0F) << 2)];
		} else {
			/* If there is only 1 Extra Byte */
			*pcOut++ = g_ccB64Tbl[ucLastByte];
			*pcOut++ = '=';
		}

		*pcOut++ = '=';
	}

	*pcOut  = '\0';
}
