/*****************************************************************************
 *
 * \file
 *
 * \brief AEC
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


#include <stdio.h>
#include <string.h>

#define SQ(x) ((x)*(x))
#define ROUND_DIV(a,b) (((a) + ((b)/2))/(b))

#define NUM_TAPS 256
#define BLOCK_SIZE 64

void echo_cancel(const short *out_buf, const short *in, size_t block_size, int *filter_q16, size_t num_taps, short *echo_cancelled_in)
{
    const short *cur_out = &out_buf[block_size-1];
    const short *cur_in = &in[block_size-1];
    short *cur_in_ec = &echo_cancelled_in[block_size-1];

    const short *out_p;
    int out_norm_sq_p16; /* The value is * 2^16 */

    /* Calculate initial norm squared of output vector */
    out_norm_sq_p16 = 0;
    for (out_p = &out_buf[block_size+num_taps-1]; out_p != &out_buf[block_size-1]; --out_p)
        out_norm_sq_p16 += SQ(ROUND_DIV((int) *out_p, 1 << 8));

    do
    {
        int *filter_q16_p;
        int echo_est_q16;
        short echo_est;

        /* Calculate echo estimate for this sample using output data  */
        echo_est_q16 = 0;
        for (out_p = &cur_out[num_taps-1], filter_q16_p = &filter_q16[num_taps-1]; out_p != cur_out; --out_p, --filter_q16_p)
            echo_est_q16 += (int) (*out_p)*(*filter_q16_p);

        echo_est = ROUND_DIV(echo_est_q16, 1 << 16);

        /* Echo cancelled input is simply input minus echo estimate
                       * Round echo_est_q16 to nearest int when converting to short
                       * This can also be interpreted as the error term, which
                       * is used for the NLMS correction below
                       */
        *cur_in_ec = *cur_in - echo_est;

        /* Update norm squared of output vector */
        out_norm_sq_p16 += SQ(ROUND_DIV((int) cur_out[0], 1<<8)) - SQ(ROUND_DIV((int) cur_out[num_taps], 1<<8));

        /* Update filter tap weights using NLMS correction */
        if (out_norm_sq_p16 != 0)
        {
            for (out_p = &cur_out[num_taps-1], filter_q16_p = &filter_q16[num_taps-1]; out_p != cur_out; --out_p, --filter_q16_p)
                *filter_q16_p += ROUND_DIV((int) *cur_in_ec * (int) *out_p, out_norm_sq_p16);
        }
    } while (cur_out--, cur_in_ec--, cur_in-- != in);
}


int main(int argc, char *_argv[])
{
  short out_buf[BLOCK_SIZE+NUM_TAPS];
  int filter_q16[NUM_TAPS];

  memset(out_buf, 0, sizeof (out_buf));
  memset(filter_q16, 0, sizeof (filter_q16)); // The filter tap weights must
                                              // initially be zero!!

  while (!kbhit())
  {
    short in[BLOCK_SIZE];
    short echo_cancelled_in[BLOCK_SIZE];

    // Output BLOCK_SIZE samples to the speaker from outBuf and input
    // BLOCK_SIZE samples into inBuf from the microphone.
    play_record_local(outBuf, in, BLOCK_SIZE);

    // Cancel the echo of out present on in using the outBuf buffered
    // samples.  Adaptively updates the filter tap weights as it goes.
    echo_cancel(out_buf, in, BLOCK_SIZE, filter_q16, NUM_TAPS, echo_cancelled_in);

    // Shift samples in preparation to get more data...
    memmove (&amp; out_buf[BLOCK_SIZE], &out_buf[0], sizeof (out_buf) - sizeof (in));

    // Send the echo cancelled input and receive to/from remote source
    play_record_remote(echo_cancelled_in, out, BLOCK_SIZE);
  }
}
