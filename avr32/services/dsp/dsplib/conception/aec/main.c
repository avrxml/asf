/*****************************************************************************
 *
 * \file
 *
 * \brief AEC
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
