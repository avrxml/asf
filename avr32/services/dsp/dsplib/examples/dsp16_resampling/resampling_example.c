/**
 * \file
 *
 * \mainpage
 * \section title 16-bit signal resampling program example.
 *
 * \section file File(s)
 * - \ref resampling_example.c
 *
 * This file is a complete example showing how to use the resampling function
 * from the DSPLib.
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

#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "pm.h"
#include "cycle_counter.h"

//! Sampling rate of the original signal
#define F_INPUT           22050
//! Sampling rate of the output signal
#define F_OUTPUT          48510
//! Number of point to process
#define N                 200
#define NB_CUTS           40
//! Filter order
#define FILTER_ORDER      6

A_ALIGNED dsp16_t s[N] = {
    DSP16_Q(0.00000000000),
    DSP16_Q(0.28111111333),
    DSP16_Q(0.53955074319),
    DSP16_Q(0.75447585092),
    DSP16_Q(0.90855282432),
    DSP16_Q(0.98935542552),
    DSP16_Q(0.99036696149),
    DSP16_Q(0.91150585231),
    DSP16_Q(0.75913221057),
    DSP16_Q(0.54553490121),
    DSP16_Q(0.28794045010),
    DSP16_Q(0.00712373261),
    DSP16_Q(-0.27426751067),
    DSP16_Q(-0.53353920393),
    DSP16_Q(-0.74978120297),
    DSP16_Q(-0.90555368889),
    DSP16_Q(-0.98829368154),
    DSP16_Q(-0.99132823811),
    DSP16_Q(-0.91441262302),
    DSP16_Q(-0.76375004563),
    DSP16_Q(-0.55149137432),
    DSP16_Q(-0.29475517441),
    DSP16_Q(-0.01424710371),
    DSP16_Q(0.26740998943),
    DSP16_Q(0.52750058851),
    DSP16_Q(0.74504850496),
    DSP16_Q(0.90250859824),
    DSP16_Q(0.98718178341),
    DSP16_Q(0.99223920660),
    DSP16_Q(0.91727298892),
    DSP16_Q(0.76832912173),
    DSP16_Q(0.55741986022),
    DSP16_Q(0.30155494042),
    DSP16_Q(0.02136975179),
    DSP16_Q(-0.26053889761),
    DSP16_Q(-0.52143520338),
    DSP16_Q(-0.74027799708),
    DSP16_Q(-0.89941770689),
    DSP16_Q(-0.98601978758),
    DSP16_Q(-0.99309982072),
    DSP16_Q(-0.92008680485),
    DSP16_Q(-0.77286920651),
    DSP16_Q(-0.56332005806),
    DSP16_Q(-0.30833940306),
    DSP16_Q(-0.02849131539),
    DSP16_Q(0.25365458391),
    DSP16_Q(0.51534335635),
    DSP16_Q(0.73546992141),
    DSP16_Q(0.89628117170),
    DSP16_Q(0.98480775301),
    DSP16_Q(0.99391003681),
    DSP16_Q(0.92285392803),
    DSP16_Q(0.77737006956),
    DSP16_Q(0.56919166842),
    DSP16_Q(0.31510821802),
    DSP16_Q(0.03561143311),
    DSP16_Q(-0.24675739769),
    DSP16_Q(-0.50922535656),
    DSP16_Q(-0.73062452196),
    DSP16_Q(-0.89309915185),
    DSP16_Q(-0.98354574121),
    DSP16_Q(-0.99466981373),
    DSP16_Q(-0.92557421803),
    DSP16_Q(-0.78183148247),
    DSP16_Q(-0.57503439332),
    DSP16_Q(-0.32186104181),
    DSP16_Q(-0.04272974361),
    DSP16_Q(0.23984768897),
    DSP16_Q(0.50308151450),
    DSP16_Q(0.72574204462),
    DSP16_Q(0.88987180881),
    DSP16_Q(0.98223381622),
    DSP16_Q(0.99537911295),
    DSP16_Q(0.92824753679),
    DSP16_Q(0.78625321883),
    DSP16_Q(0.58084793626),
    DSP16_Q(0.32859753172),
    DSP16_Q(0.04984588566),
    DSP16_Q(-0.23292580842),
    DSP16_Q(-0.49691214195),
    DSP16_Q(-0.72082273717),
    DSP16_Q(-0.88659930637),
    DSP16_Q(-0.98087204463),
    DSP16_Q(-0.99603789846),
    DSP16_Q(-0.93087374864),
    DSP16_Q(-0.79063505425),
    DSP16_Q(-0.58663200220),
    DSP16_Q(-0.33531734590),
    DSP16_Q(-0.05695949812),
    DSP16_Q(0.22599210729),
    DSP16_Q(0.49071755200),
    DSP16_Q(0.71586684926),
    DSP16_Q(0.88328181061),
    DSP16_Q(0.97946049553),
    DSP16_Q(0.99664613682),
    DSP16_Q(0.93345272033),
    DSP16_Q(0.79497676635),
    DSP16_Q(0.59238629762),
    DSP16_Q(0.34202014333),
    DSP16_Q(0.06407021998),
    DSP16_Q(-0.21904693747),
    DSP16_Q(-0.48449805902),
    DSP16_Q(-0.71087463239),
    DSP16_Q(-0.87991948987),
    DSP16_Q(-0.97799924057),
    DSP16_Q(-0.99720379718),
    DSP16_Q(-0.93598432095),
    DSP16_Q(-0.79927813480),
    DSP16_Q(-0.59811053049),
    DSP16_Q(-0.34870558384),
    DSP16_Q(-0.07117769040),
    DSP16_Q(0.21209065142),
    DSP16_Q(0.47825397862),
    DSP16_Q(0.70584633990),
    DSP16_Q(0.87651251479),
    DSP16_Q(0.97648835389),
    DSP16_Q(0.99771085123),
    DSP16_Q(0.93846842205),
    DSP16_Q(0.80353894132),
    DSP16_Q(0.60380441033),
    DSP16_Q(0.35537332817),
    DSP16_Q(0.07828154867),
    DSP16_Q(-0.20512360214),
    DSP16_Q(-0.47198562769),
    DSP16_Q(-0.70078222697),
    DSP16_Q(-0.87306105827),
    DSP16_Q(-0.97492791218),
    DSP16_Q(-0.99816727324),
    DSP16_Q(-0.94090489755),
    DSP16_Q(-0.80775896968),
    DSP16_Q(-0.60946764817),
    DSP16_Q(-0.36202303794),
    DSP16_Q(-0.08538143430),
    DSP16_Q(0.19814614320),
    DSP16_Q(0.46569332434),
    DSP16_Q(0.69568255060),
    DSP16_Q(0.86956529547),
    DSP16_Q(0.97331799462),
    DSP16_Q(0.99857304005),
    DSP16_Q(0.94329362382),
    DSP16_Q(0.81193800572),
    DSP16_Q(0.61509995662),
    DSP16_Q(0.36865437568),
    DSP16_Q(0.09247698697),
    DSP16_Q(-0.19115862870),
    DSP16_Q(-0.45937738788),
    DSP16_Q(-0.69054756959),
    DSP16_Q(-0.86602540378),
    DSP16_Q(-0.97165868292),
    DSP16_Q(-0.99892813106),
    DSP16_Q(-0.94563447962),
    DSP16_Q(-0.81607583735),
    DSP16_Q(-0.62070104984),
    DSP16_Q(-0.37526700488),
    DSP16_Q(-0.09956784660),
    DSP16_Q(0.18416141325),
    DSP16_Q(0.45303813884),
    DSP16_Q(0.68537754452),
    DSP16_Q(0.86244156286),
    DSP16_Q(0.96995006128),
    DSP16_Q(0.99923252826),
    DSP16_Q(0.94792734617),
    DSP16_Q(0.82017225460),
    DSP16_Q(0.62627064360),
    DSP16_Q(0.38186058995),
    DSP16_Q(0.10665365334),
    DSP16_Q(-0.17715485193),
    DSP16_Q(-0.44667589893),
    DSP16_Q(-0.68017273777),
    DSP16_Q(-0.85881395457),
    DSP16_Q(-0.96819221641),
    DSP16_Q(-0.99948621620),
    DSP16_Q(-0.95017210710),
    DSP16_Q(-0.82422704957),
    DSP16_Q(-0.63180845525),
    DSP16_Q(-0.38843479627),
    DSP16_Q(-0.11373404759),
    DSP16_Q(0.17013930032),
    DSP16_Q(0.44029099101),
    DSP16_Q(0.67493341347),
    DSP16_Q(0.85514276301),
    DSP16_Q(0.96638523751),
    DSP16_Q(0.99968918200),
    DSP16_Q(0.95236864849),
    DSP16_Q(0.82824001649),
    DSP16_Q(0.63731420374),
    DSP16_Q(0.39498929023),
    DSP16_Q(0.12080867005),
    DSP16_Q(-0.16311511444),
    DSP16_Q(-0.43388373912),
    DSP16_Q(-0.66965983751),
    DSP16_Q(-0.85142817448),
    DSP16_Q(-0.96452921630),
    DSP16_Q(-0.99984141536),
    DSP16_Q(-0.95451685888),
    DSP16_Q(-0.83221095172),
    DSP16_Q(-0.64278760969),
    DSP16_Q(-0.40152373919),
    DSP16_Q(-0.12787716168),
    DSP16_Q(0.15608265077)
};

//! The main function
int main(int argc, char *argv[])
{
  A_ALIGNED static dsp16_t s_input[N/NB_CUTS];
  int f;
  dsp_resampling_t *resampling;
  dsp16_t *output;
  U32 temp;
  dsp16_resampling_options_t options;

  // Initialize options
  memset(&options, 0, sizeof(options));
  options.coefficients_generation = DSP16_RESAMPLING_OPTIONS_USE_DYNAMIC;
  options.dynamic.coefficients_normalization = true;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  dsp16_debug_printf("16-bit fixed point signal resampling\n");
  dsp16_debug_printf("Input Fs: %iHz\n", F_INPUT);
  dsp16_debug_printf("Output Fs: %iHz\n", F_OUTPUT);
  dsp16_debug_printf("%i samples to process cut into %i buffers\n", N, NB_CUTS);
  dsp16_debug_printf("Filter order used: %i\n", FILTER_ORDER);

  if (!(resampling = dsp16_resampling_setup(F_INPUT, F_OUTPUT, N/NB_CUTS, FILTER_ORDER, 1, (malloc_fct_t) malloc, &options)))
  {
    dsp16_debug_printf("Unable to allocate enough memory\n");
    return -1;
  }

  if (!(output = (dsp16_t *) malloc(dsp16_resampling_get_output_max_buffer_size(resampling)*sizeof(dsp16_t) + 4)))
  {
    dsp16_debug_printf("Unable to allocate enough memory for the output buffer\n");
    return -1;
  }

  for(f=0; f<NB_CUTS; f++)
  {
    memcpy(s_input, &s[N/NB_CUTS*f], (N/NB_CUTS)*sizeof(dsp16_t));
    temp = Get_sys_count();
    dsp16_resampling_compute(resampling, output, s_input, 0);
    temp = Get_sys_count() - temp;
    dsp16_debug_print_vect(output, dsp16_resampling_get_output_current_buffer_size(resampling));
  }
  dsp16_debug_printf(
    "Cycle count per iteration: %i\n            in total (to compute %i samples): %i\n",
    temp,
    dsp16_resampling_get_output_current_buffer_size(resampling)*NB_CUTS,
    temp*NB_CUTS);

  dsp16_resampling_free(resampling, (free_fct_t) free);

  while(1);
}

