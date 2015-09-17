/**
 * \file
 *
 * \brief SSD1306 fonts.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

/*======= binar input =======*/
#define bits2bytes(b7,b6,b5,b4,b3,b2,b1,b0) ((uint8_t)((b7<<7)|(b6<<6)|(b5<<5)|(b4<<4)|(b3<<3)|(b2<<2)|(b1<<1)|(b0<<0)))

/*======= Character pointers table =======*/
extern uint8_t *font_table[95];

/*======= Characters data =======*/
extern uint8_t Font08px_32[3];
extern uint8_t Font08px_33[2];
extern uint8_t Font08px_34[4];
extern uint8_t Font08px_35[6];
extern uint8_t Font08px_36[6];
extern uint8_t Font08px_37[6];
extern uint8_t Font08px_38[6];
extern uint8_t Font08px_39[2];
extern uint8_t Font08px_40[4];
extern uint8_t Font08px_41[4];
extern uint8_t Font08px_42[4];
extern uint8_t Font08px_43[4];
extern uint8_t Font08px_44[2];
extern uint8_t Font08px_45[4];
extern uint8_t Font08px_46[2];
extern uint8_t Font08px_47[4];
extern uint8_t Font08px_N0[6];
extern uint8_t Font08px_N1[6];
extern uint8_t Font08px_N2[6];
extern uint8_t Font08px_N3[6];
extern uint8_t Font08px_N4[6];
extern uint8_t Font08px_N5[6];
extern uint8_t Font08px_N6[6];
extern uint8_t Font08px_N7[6];
extern uint8_t Font08px_N8[6];
extern uint8_t Font08px_N9[6];
extern uint8_t Font08px_58[2];
extern uint8_t Font08px_59[2];
extern uint8_t Font08px_60[5];
extern uint8_t Font08px_61[5];
extern uint8_t Font08px_62[5];
extern uint8_t Font08px_63[6];
extern uint8_t Font08px_64[9];
extern uint8_t Font08px_UA[6];
extern uint8_t Font08px_UB[5];
extern uint8_t Font08px_UC[5];
extern uint8_t Font08px_UD[5];
extern uint8_t Font08px_UE[5];
extern uint8_t Font08px_UF[5];
extern uint8_t Font08px_UG[6];
extern uint8_t Font08px_UH[5];
extern uint8_t Font08px_UI[4];
extern uint8_t Font08px_UJ[5];
extern uint8_t Font08px_UK[6];
extern uint8_t Font08px_UL[5];
extern uint8_t Font08px_UM[6];
extern uint8_t Font08px_UN[6];
extern uint8_t Font08px_UO[5];
extern uint8_t Font08px_UP[5];
extern uint8_t Font08px_UQ[6];
extern uint8_t Font08px_UR[5];
extern uint8_t Font08px_US[5];
extern uint8_t Font08px_UT[6];
extern uint8_t Font08px_UU[5];
extern uint8_t Font08px_UV[6];
extern uint8_t Font08px_UW[6];
extern uint8_t Font08px_UX[6];
extern uint8_t Font08px_UY[6];
extern uint8_t Font08px_UZ[6];
extern uint8_t Font08px_91[4];
extern uint8_t Font08px_92[4];
extern uint8_t Font08px_93[4];
extern uint8_t Font08px_94[4];
extern uint8_t Font08px_95[4];
extern uint8_t Font08px_96[3];
extern uint8_t Font08px_la[5];
extern uint8_t Font08px_lb[5];
extern uint8_t Font08px_lc[5];
extern uint8_t Font08px_ld[5];
extern uint8_t Font08px_le[5];
extern uint8_t Font08px_lf[4];
extern uint8_t Font08px_lg[5];
extern uint8_t Font08px_lh[5];
extern uint8_t Font08px_li[2];
extern uint8_t Font08px_lj[3];
extern uint8_t Font08px_lk[5];
extern uint8_t Font08px_ll[2];
extern uint8_t Font08px_lm[6];
extern uint8_t Font08px_ln[5];
extern uint8_t Font08px_lo[5];
extern uint8_t Font08px_lp[5];
extern uint8_t Font08px_lq[5];
extern uint8_t Font08px_lr[4];
extern uint8_t Font08px_ls[5];
extern uint8_t Font08px_lt[4];
extern uint8_t Font08px_lu[5];
extern uint8_t Font08px_lv[6];
extern uint8_t Font08px_lw[6];
extern uint8_t Font08px_lx[6];
extern uint8_t Font08px_ly[5];
extern uint8_t Font08px_lz[4];
extern uint8_t Font08px_123[4];
extern uint8_t Font08px_124[2];
extern uint8_t Font08px_125[4];
extern uint8_t Font08px_126[6];

#endif /* FONT_H_INCLUDED */
