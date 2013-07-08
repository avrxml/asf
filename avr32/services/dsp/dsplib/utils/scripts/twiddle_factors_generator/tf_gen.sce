// Twiddle factor table generation for the AVR32 DSP Lib

// Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. The name of ATMEL may not be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
// SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Parameter N_TWIDDLE_FACTORS
// Used for a N_TWIDDLE_FACTORS-Point FFT maximum
N_TWIDDLE_FACTORS = 4096;
// Parameter BITS
// The number of bits of the twiddle factors coefficients
BITS = 32;




PI = 3.14159265358979323846;
QA = 1;
QB = (BITS-QA);
resolution = int(abs(log10(1/(2^QB)))+2);

  printf("/*\n");
  printf(" * Twiddle factors for a %d-point FFT\n", N_TWIDDLE_FACTORS);
  printf(" * Minimized twiddle factors table: %d*(%d/4 + 1)*2 = %d bytes\n", BITS/8, N_TWIDDLE_FACTORS, (N_TWIDDLE_FACTORS/4+1)*2*BITS/8);
  printf(" * Full twiddle factors table: %d*(%d/4)*6 + %d*2 = %d bytes\n", BITS/8, N_TWIDDLE_FACTORS, BITS/8, (N_TWIDDLE_FACTORS/4)*6*BITS/8 + BITS/8*2);
  printf(" */\n");
  printf("#if DSP%d_N_TWIDDLE_FACTORS == %d\n\n", BITS, N_TWIDDLE_FACTORS);

  printf("  A_ALIGNED TWIDDLE_FACTORS_PREFIX_TAB dsp%d_t dsp%d_twiddle_factors[DSP%d_N_TWIDDLE_FACTORS/2+2] = {\n", BITS, BITS, BITS);
  printf("  /*Re(w)   Im(w)  */\n");

  for i=0:4:N_TWIDDLE_FACTORS,
    k = i/(N_TWIDDLE_FACTORS*4);
    w = exp(-2*PI*%i*k);

    str_format = sprintf("    DSP%d_Q_CONVERT(%%.%df), DSP%d_Q_CONVERT(%%.%df)", BITS, resolution, BITS, resolution);
    printf(str_format, w, -w*%i);
    if i < N_TWIDDLE_FACTORS then
      printf(",\n");
    else
      printf("\n");
    end
  end;

  printf("  };\n\n");

  printf("  // If we need to speed up the code\n");
  printf("#  if !(DSP_OPTIMIZATION & DSP_OPTI_SIZE)\n");
  printf("    A_ALIGNED TWIDDLE_FACTORS_PREFIX_TAB dsp%d_t dsp%d_twiddle_factors2[DSP%d_N_TWIDDLE_FACTORS] = {\n", BITS, BITS, BITS);
  printf("    /*Re(w2)   Im(w2)   Re(w3)   Im(w3)  */\n");

  for i=0:4:N_TWIDDLE_FACTORS-4,
    k = i/(N_TWIDDLE_FACTORS*4);
    w2 = exp(-2*PI*%i*k*2);
    w3 = exp(-2*PI*%i*k*3);

    str_format = sprintf("    DSP%d_Q_CONVERT(%%.%df), DSP%d_Q_CONVERT(%%.%df),", BITS, resolution, BITS, resolution);
    printf(str_format, w2, -w2*%i);
    printf("\n");
    str_format = sprintf("    DSP%d_Q_CONVERT(%%.%df), DSP%d_Q_CONVERT(%%.%df)", BITS, resolution, BITS, resolution);
    printf(str_format, w3, -w3*%i);
    if i < N_TWIDDLE_FACTORS-4 then
      printf(",\n");
    else
      printf("\n");
    end
  end;

  printf("    };\n");
  printf("#  endif\n\n");

  printf("#endif\n");

