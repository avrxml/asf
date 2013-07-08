/*
   Optimized function for speeding up synthesis filter
   in MPEG Audio Decoding.
   Copyright 2003-2006 Atmel Corporation.

   Written by Ronny Pedersen and Lars Even Almås, Atmel Norway

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */


/* *****************
   Defining macros
   ***************** */


	.macro	ldalign		REG1, REG2, offset
	ld.uh	\REG1, \REG2[\offset + 2]
	ldins.h	\REG1:t, \REG2[\offset]
	.endm

	.macro	window_1	f, ptr, acc, ptr_offset, mul, tmp1_lo, tmp1_hi, tmp2_lo, tmp2_hi, tmp3_lo, tmp3_hi
	ld.d		\tmp1_lo, \f[0*4]			/* tmp1 = { f[0], f[1] } */
/*	ld.w		\tmp2_lo, \ptr[0*2+\ptr_offset*2]*/	/* tmp2_lo = { ptr[0], ptr[1] }*/
	ldalign		\tmp2_lo, \ptr, 0*2+\ptr_offset*2

	ld.d		\tmp3_lo, \f[6*4]			/* tmp3 = { f[6], f[7] } */
/*	ld.w		\tmp2_hi, \ptr[6*2+\ptr_offset*2]*/	/* tmp2_hi = { ptr[6], ptr[7] }*/
	ldalign		\tmp2_hi, \ptr, 6*2+\ptr_offset*2	/* tmp2_hi = { ptr[6], ptr[7] }*/

	.if \mul
	mulwh.d		\acc, \tmp1_hi, \tmp2_lo:t		/* f[0] * ptr[0]*/
	.else
	macwh.d		\acc, \tmp1_hi, \tmp2_lo:t		/* f[0] * ptr[0]*/
	.endif
	macwh.d		\acc, \tmp3_lo, \tmp2_lo:b		/* f[7] * ptr[1]*/
/*	ld.w		\tmp2_lo, \ptr[2*2+\ptr_offset*2]*/	/* tmp2_lo = { ptr[2], ptr[3] }*/
	ldalign		\tmp2_lo, \ptr, 2*2+\ptr_offset*2	/* tmp2_lo = { ptr[2], ptr[3] }*/

	macwh.d		\acc, \tmp1_lo, \tmp2_hi:b		/* f[1] * ptr[7]*/
	ld.d		\tmp1_lo, \f[2*4]			/* tmp1 = { f[2], f[3] } */

	macwh.d		\acc, \tmp3_hi, \tmp2_lo:t		/* f[6] * ptr[2]*/
	macwh.d		\acc, \tmp1_hi, \tmp2_hi:t		/* f[2] * ptr[6]*/
	ld.d		\tmp3_lo, \f[4*4]			/* tmp3 = { f[4], f[5] } */
/*	ld.w		\tmp2_hi, \ptr[4*2+\ptr_offset*2]*/	/* tmp2_hi = { ptr[4], ptr[5] }*/
	ldalign		\tmp2_hi, \ptr, 4*2+\ptr_offset*2	/* tmp2_hi = { ptr[4], ptr[5] }*/

	macwh.d		\acc, \tmp3_lo, \tmp2_lo:b		/* f[5] * ptr[3]*/

	macwh.d		\acc, \tmp1_lo, \tmp2_hi:b		/* f[3] * ptr[5]*/
	macwh.d		\acc, \tmp3_hi, \tmp2_hi:t		/* f[4] * ptr[4]*/
	.endm

	.macro	window_2	f, ptr, acc, ptr_offset, mul, tmp1_lo, tmp1_hi, tmp2_lo, tmp2_hi, tmp3_lo, tmp3_hi
	ld.d		\tmp1_lo, \f[0*4]			/* tmp1 = { f[0], f[1] } */
/*	ld.w		\tmp2_lo, \ptr[7*2+\ptr_offset*2]*/	/* tmp2_lo = { ptr[7], ptr[8] }*/
	ldalign		\tmp2_lo, \ptr, 7*2+\ptr_offset*2	/* tmp2_lo = { ptr[7], ptr[8] }*/

	ld.d		\tmp3_lo, \f[2*4]			/* tmp3 = { f[2], f[3] } */
/*	ld.w		\tmp2_hi, \ptr[9*2+\ptr_offset*2]*/	/* tmp2_hi = { ptr[9], ptr[10] }*/
	ldalign		\tmp2_hi, \ptr, 9*2+\ptr_offset*2	/* tmp2_hi = { ptr[9], ptr[10] }*/
	.if \mul
	mulwh.d		\acc, \tmp1_hi, \tmp2_lo:t		/* f[0] * ptr[7]*/
	.else
	macwh.d		\acc, \tmp1_hi, \tmp2_lo:t		/* f[0] * ptr[7]*/
	.endif
	macwh.d		\acc, \tmp1_lo, \tmp2_lo:b		/* f[1] * ptr[8]*/

	ld.d		\tmp1_lo, \f[4*4]			/* tmp1 = { f[4], f[5] } */
/*	ld.w		\tmp2_lo, \ptr[11*2+\ptr_offset*2]*/	/* tmp2_lo = { ptr[11], ptr[12] }*/
	ldalign		\tmp2_lo, \ptr, 11*2+\ptr_offset*2	/* tmp2_lo = { ptr[11], ptr[12] }*/

	macwh.d		\acc, \tmp3_hi, \tmp2_hi:t		/* f[2] * ptr[9]*/
	macwh.d		\acc, \tmp3_lo, \tmp2_hi:b		/* f[3] * ptr[10]*/

	ld.d		\tmp3_lo, \f[6*4]			/* tmp3 = { f[6], f[7] } */
/*	ld.w		\tmp2_hi, \ptr[13*2+\ptr_offset*2]*/	/* tmp2_hi = { ptr[13], ptr[14] }*/
	ldalign		\tmp2_hi, \ptr, 13*2+\ptr_offset*2	/* tmp2_hi = { ptr[13], ptr[14] }*/

	macwh.d		\acc, \tmp1_hi, \tmp2_lo:t		/* f[4] * ptr[11]*/
	macwh.d		\acc, \tmp1_lo, \tmp2_lo:b		/* f[5] * ptr[12]*/
	macwh.d		\acc, \tmp3_hi, \tmp2_hi:t		/* f[6] * ptr[13]*/
	macwh.d		\acc, \tmp3_lo, \tmp2_hi:b		/* f[7] * ptr[14]*/
	.endm

	.macro	scale	res, d_lo, d_hi
	satrnds		\d_hi >> 11, 16
	.endm

/* **********************
   Starting main function
   ********************** */

  .section  .text.synth_avr32, "ax", @progbits

  .align  2

/* Function synth_avr32 is called from synth.c with arguments:
             phase, filter, *pcm1, &D[0]		*/

	.global	synth_avr32
synth_avr32:
	pushm		r0-r7, lr
	sub		sp, 8

	/* R12 = phase, R11 = filter, R10 = pcm1, r9 = D*/
	bld		r12, 0
	brcc		synth_even

	/* Filter for odd phases */

	/*	fe = &(*filter)[0][1][0];
		fx = &(*filter)[0][0][0];
		fo = &(*filter)[1][0][0]; */
	sub		lr /*fe*/, r11, -16*8*4
	sub		r8 /*fo*/, r11, -16*8*4*2

	/*	pe = phase >> 1; */
	lsr		r12, 1
	stdsp		sp[4], r12
	/*	ptr = (short const *)Dmod + pe; */
	add		r12, r9, r12 << 1

	/*	ML0(hi, lo, (*fx)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fx)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fx)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fx)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fx)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fx)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fx)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fx)[7], ptr[1 + 17]); */
	window_1	r11/*fx*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7

    /*		MLN(hi, lo); */
	neg		r0
	acr		r1
	neg		r1

    /*		MLA(hi, lo, (*fe)[0], ptr[0]);
		MLA(hi, lo, (*fe)[1], ptr[7]);
		MLA(hi, lo, (*fe)[2], ptr[6]);
                MLA(hi, lo, (*fe)[3], ptr[5]);
                MLA(hi, lo, (*fe)[4], ptr[4]);
                MLA(hi, lo, (*fe)[5], ptr[3]);
                MLA(hi, lo, (*fe)[6], ptr[2]);
                MLA(hi, lo, (*fe)[7], ptr[1]); */
	window_1	lr/*fe*/,r12/*ptr*/,r0/*acc*/,0/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7

    /*		*pcm1++ = SHIFT(MLZ(hi, lo));

		pcm2 = pcm1 + 31; */
	scale		r1, r0, r1
	st.h		r10/*pcm_1*/++, r1
	sub		r11/*pcm2*/, r10, -2*31

    /*		for (sb = 1; sb < 16; ++sb) { */
	mov 		r2, 15
	stdsp		sp[0], r2
odd_loop:
    /*		++fe;
		ptr += 33; */
	sub		lr /*fe*/, -8*4
	sub		r12, -33*2

    /*		ML0(hi, lo, (*fo)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fo)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fo)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fo)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fo)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fo)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fo)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fo)[7], ptr[1 + 17]); */
	window_1	r8/*fo*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7
    /*		MLN(hi, lo); */

	neg		r0
	acr		r1
	neg		r1

    /*		MLA(hi, lo, (*fe)[7], ptr[1]);
		MLA(hi, lo, (*fe)[6], ptr[2]);
		MLA(hi, lo, (*fe)[5], ptr[3]);
		MLA(hi, lo, (*fe)[4], ptr[4]);
		MLA(hi, lo, (*fe)[3], ptr[5]);
		MLA(hi, lo, (*fe)[2], ptr[6]);
		MLA(hi, lo, (*fe)[1], ptr[7]);
		MLA(hi, lo, (*fe)[0], ptr[0]); */
	window_1	lr/*fe*/,r12/*ptr*/,r0/*acc*/,0/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7

    /*	ptr -= 2*pe; */
	lddsp		r2, sp[4]

    /*		*pcm1++ = SHIFT(MLZ(hi, lo)); */

	scale		r1, r0, r1
	st.h		r10/*pcm_1*/++, r1
	sub		r12/*ptr*/, r12, r2/*pe*/<< 2


    /*		ML0(hi, lo, (*fe)[0], ptr[7  + 17]);
		MLA(hi, lo, (*fe)[1], ptr[8  + 17]);
		MLA(hi, lo, (*fe)[2], ptr[9  + 17]);
		MLA(hi, lo, (*fe)[3], ptr[10 + 17]);
		MLA(hi, lo, (*fe)[4], ptr[11 + 17]);
		MLA(hi, lo, (*fe)[5], ptr[12 + 17]);
		MLA(hi, lo, (*fe)[6], ptr[13 + 17]);
		MLA(hi, lo, (*fe)[7], ptr[14 + 17]); */
	window_2	lr/*fe*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7
    /*		MLA(hi, lo, (*fo)[7], ptr[14]);
		MLA(hi, lo, (*fo)[6], ptr[13]);
		MLA(hi, lo, (*fo)[5], ptr[12]);
		MLA(hi, lo, (*fo)[4], ptr[11]);
		MLA(hi, lo, (*fo)[3], ptr[10]);
		MLA(hi, lo, (*fo)[2], ptr[9]);
		MLA(hi, lo, (*fo)[1], ptr[8]);
		MLA(hi, lo, (*fo)[0], ptr[7]); */
	window_2	r8/*fo*/,r12/*ptr*/,r0/*acc*/,0/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7


    /*		*pcm2-- = SHIFT(MLZ(hi, lo)); */
	lddsp		r3, sp[4]
	lddsp		r2, sp[0]
	scale		r1, r0, r1
	st.h		--r11/*pcm_2*/, r1

    /*		ptr += 2*pe; */
	add		r12/*ptr*/, r12, r3/*pe*/<< 2

    /*		++fo;
		} */
	sub		r8/*fo*/, -8*4

	sub		r2, 1
	stdsp		sp[0], r2
	brne		odd_loop

    /*		ptr += 33; */
	sub		r12/*ptr*/, -33*2

    /*		ML0(hi, lo, (*fo)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fo)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fo)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fo)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fo)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fo)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fo)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fo)[7], ptr[1 + 17]); */
	window_1	r8/*fo*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7

	rjmp		synth_end
synth_even:
	/* Filter for even phases */

	/*	fe = &(*filter)[0][0][0];
		fx = &(*filter)[0][1][0];
		fo = &(*filter)[1][1][0]; */
	sub		lr /*fx*/, r11, -16*8*4
	sub		r8 /*fo*/, r11, -(16*8*4*2 + 16*8*4)

	/*	po = ((phase - 1) & 0xF) >> 1; */
	sub		r12, 1
	andl		r12, 0xe, COH
	stdsp		sp[4], r12
	/*	ptr = (short const *)Dmod + po; */
	add		r12, r9, r12

	/*	ML0(hi, lo, (*fx)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fx)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fx)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fx)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fx)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fx)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fx)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fx)[7], ptr[1 + 17]); */
	window_1	lr/*fx*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7

    /*		MLN(hi, lo); */
	neg		r0
	acr		r1
	neg		r1

    /*		MLA(hi, lo, (*fe)[0], ptr[0 + 1]);
		MLA(hi, lo, (*fe)[1], ptr[7 + 1]);
		MLA(hi, lo, (*fe)[2], ptr[6 + 1]);
                MLA(hi, lo, (*fe)[3], ptr[5 + 1]);
                MLA(hi, lo, (*fe)[4], ptr[4 + 1]);
                MLA(hi, lo, (*fe)[5], ptr[3 + 1]);
                MLA(hi, lo, (*fe)[6], ptr[2 + 1]);
                MLA(hi, lo, (*fe)[7], ptr[1 + 1]); */
	window_1	r11/*fe*/,r12/*ptr*/,r0/*acc*/,1/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7

    /*		*pcm1++ = SHIFT(MLZ(hi, lo));

		pcm2 = pcm1 + 31; */
	scale		r1, r0, r1
	st.h		r10/*pcm_1*/++, r1
	sub		lr/*pcm2*/, r10, -2*31

    /*		for (sb = 1; sb < 16; ++sb) { */
	mov 		r2, 15
	stdsp		sp[0], r2
even_loop:
    /*		++fe;
		ptr += 33; */
	sub		r11 /*fe*/, -8*4
	sub		r12, -33*2

    /*		ML0(hi, lo, (*fo)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fo)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fo)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fo)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fo)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fo)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fo)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fo)[7], ptr[1 + 17]); */
	window_1	r8/*fo*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7
    /*		MLN(hi, lo); */
	neg		r0
	acr		r1
	neg		r1

    /*		MLA(hi, lo, (*fe)[7], ptr[1 + 1]);
		MLA(hi, lo, (*fe)[6], ptr[2 + 1]);
		MLA(hi, lo, (*fe)[5], ptr[3 + 1]);
		MLA(hi, lo, (*fe)[4], ptr[4 + 1]);
		MLA(hi, lo, (*fe)[3], ptr[5 + 1]);
		MLA(hi, lo, (*fe)[2], ptr[6 + 1]);
		MLA(hi, lo, (*fe)[1], ptr[7 + 1]);
		MLA(hi, lo, (*fe)[0], ptr[0 + 1]); */
	window_1	r11/*fe*/,r12/*ptr*/,r0/*acc*/,1/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7

    /*		*pcm1++ = SHIFT(MLZ(hi, lo)); */
	lddsp		r2, sp[4]
	scale		r1, r0, r1
	st.h		r10/*pcm_1*/++, r1
    /*	ptr -= 2*po; */
	sub		r12/*ptr*/, r12, r2/*po*/<< 1


    /*		ML0(hi, lo, (*fe)[0], ptr[7  + 17 - 1]);
		MLA(hi, lo, (*fe)[1], ptr[8  + 17 - 1]);
		MLA(hi, lo, (*fe)[2], ptr[9  + 17 - 1]);
		MLA(hi, lo, (*fe)[3], ptr[10 + 17 - 1]);
		MLA(hi, lo, (*fe)[4], ptr[11 + 17 - 1]);
		MLA(hi, lo, (*fe)[5], ptr[12 + 17 - 1]);
		MLA(hi, lo, (*fe)[6], ptr[13 + 17 - 1]);
		MLA(hi, lo, (*fe)[7], ptr[14 + 17 - 1]); */
	window_2	r11/*fe*/,r12/*ptr*/,r0/*acc*/,16/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7
    /*		MLA(hi, lo, (*fo)[7], ptr[14]);
		MLA(hi, lo, (*fo)[6], ptr[13]);
		MLA(hi, lo, (*fo)[5], ptr[12]);
		MLA(hi, lo, (*fo)[4], ptr[11]);
		MLA(hi, lo, (*fo)[3], ptr[10]);
		MLA(hi, lo, (*fo)[2], ptr[9]);
		MLA(hi, lo, (*fo)[1], ptr[8]);
		MLA(hi, lo, (*fo)[0], ptr[7]); */
	window_2	r8/*fo*/,r12/*ptr*/,r0/*acc*/,0/*off*/,0/*mac*/,r2,r3,r4,r5,r6,r7


    /*		*pcm2-- = SHIFT(MLZ(hi, lo)); */
	lddsp		r3, sp[4]
	lddsp		r2, sp[0]
	scale		r1, r0, r1
	st.h		--lr/*pcm_2*/, r1

    /*		ptr += 2*po; */
	add		r12/*ptr*/, r12, r3/*po*/<< 1

    /*		++fo;
		} */
	sub		r8/*fo*/, -8*4

	sub		r2, 1
	stdsp		sp[0], r2
	brne		even_loop

    /*		ptr += 33; */
	sub		r12/*ptr*/, -33*2

    /*		ML0(hi, lo, (*fo)[0], ptr[0 + 17]);
		MLA(hi, lo, (*fo)[1], ptr[7 + 17]);
		MLA(hi, lo, (*fo)[2], ptr[6 + 17]);
		MLA(hi, lo, (*fo)[3], ptr[5 + 17]);
		MLA(hi, lo, (*fo)[4], ptr[4 + 17]);
		MLA(hi, lo, (*fo)[5], ptr[3 + 17]);
		MLA(hi, lo, (*fo)[6], ptr[2 + 17]);
		MLA(hi, lo, (*fo)[7], ptr[1 + 17]); */
	window_1	r8/*fo*/,r12/*ptr*/,r0/*acc*/,17/*off*/,1/*mul*/,r2,r3,r4,r5,r6,r7



synth_end:
   /*		*pcm1 = SHIFT(-MLZ(hi, lo)); */
	neg		r0
	acr		r1
	neg		r1
	scale		r1, r0, r1
	st.h		r10/*pcm_1*/, r1

	mov		r12, r10
	sub		sp, -8
	popm		r0-r7, pc
