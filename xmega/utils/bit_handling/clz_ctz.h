/**
 * \file
 *
 * \brief CLZ/CTZ C implementation.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef CLZ_CTH_H
#define CLZ_CTH_H

/**
 * \brief Count leading zeros in unsigned integer
 *
 * This macro takes unsigned integers of any size, and evaluates to a call to
 * the clz-function for its size. These functions count the number of zeros,
 * starting with the MSB, before a one occurs in the integer.
 *
 * \param x Unsigned integer to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
#define clz(x)    compiler_demux_size(sizeof(x), clz, (x))

/**
 * \internal
 * \brief Count leading zeros in unsigned, 8-bit integer
 *
 * \param x Unsigned byte to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz8(uint8_t x)
{
	uint8_t bit = 0;

	if (x & 0xf0) {
		x >>= 4;
	} else {
		bit += 4;
	}

	if (x & 0x0c) {
		x >>= 2;
	} else {
		bit += 2;
	}

	if (!(x & 0x02)) {
		bit++;
	}

	return bit;

}

/**
 * \internal
 * \brief Count leading zeros in unsigned, 16-bit integer
 *
 * \param x Unsigned word to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz16(uint16_t x)
{
	uint8_t bit = 0;

	if (x & 0xff00) {
		x >>= 8;
	} else {
		bit += 8;
	}

	return bit + clz8(x);
}

/**
 * \internal
 * \brief Count leading zeros in unsigned, 32-bit integer
 *
 * \param x Unsigned double word to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz32(uint32_t x)
{
	uint8_t bit = 0;

	if (x & 0xffff0000) {
		x >>= 16;
	} else {
		bit += 16;
	}

	return bit + clz16(x);
}

/**
 * \brief Count trailing zeros in unsigned integer
 *
 * This macro takes unsigned integers of any size, and evaluates to a call to
 * the ctz-function for its size. These functions count the number of zeros,
 * starting with the LSB, before a one occurs in the integer.
 *
 * \param x Unsigned integer to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
#define ctz(x)    compiler_demux_size(sizeof(x), ctz, (x))

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 8-bit integer
 *
 * \param x Unsigned byte to count the trailing zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t ctz8(uint8_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x0f)) {
		bit += 4;
		x >>= 4;
	}
	if (!(x & 0x03)) {
		bit += 2;
		x >>= 2;
	}
	if (!(x & 0x01))
		bit++;

	return bit;
}

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 16-bit integer
 *
 * \param x Unsigned word to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
__always_inline static uint8_t ctz16(uint16_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x00ff)) {
		bit += 8;
		x >>= 8;
	}

	return bit + ctz8(x);
}

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 32-bit integer
 *
 * \param x Unsigned double word to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
__always_inline static uint8_t ctz32(uint32_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x0000ffff)) {
		bit += 16;
		x >>= 16;
	}

	return bit + ctz16(x);
}

#endif /* CLZ_CTZ_H */
