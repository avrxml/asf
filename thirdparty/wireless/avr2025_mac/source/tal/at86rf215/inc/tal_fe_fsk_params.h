/**
 * @file tal_fe_fsk_params.h
 *
 * @brief Parameter mapping of function fsk_rfcfg. T
 *        his file was created by mk_fsk_params.pl. Do not edit!
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
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef TAL_FE_FSK_PARAMS_H
#define TAL_FE_FSK_PARAMS_H

#ifdef RF215v1
/* columns: {
 *txcutc,txdfe,pac04,pac09,pac24,rxbwc09,rxdfe09,rxbwc24,rxdfe24,agcc,agcs } */
/* cow index: srate_midx = srate << 3 + midx */
#define FSK_PARAMS \
	{ \
		{ 192, 10, 127, 116, 116, 0, 10, 0, 10, 129, 119    },                                 /*
		                                                                                        *srate=0,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 192, 10, 127, 116, 116, 0, 10, 0, 10, 129, 119    },                                 /*
		                                                                                        *srate=0,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 192, 10, 127, 120, 117, 0, 10, 0, 42, 129, 119    },                                 /*
		                                                                                        *srate=0,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 192, 42, 127, 127, 118, 0, 42, 1, 42, 129, 119    },                                 /*
		                                                                                        *srate=0,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 193, 42, 127, 127, 118, 1, 74, 18, 74, 129, 119    },                                /*
		                                                                                        *srate=0,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 194, 74, 127, 127, 118, 18, 74, 3, 106, 129, 119    },                               /*
		                                                                                        *srate=0,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 195, 74, 127, 127, 118, 3, 106, 3, 106, 129, 119    },                               /*
		                                                                                        *srate=0,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 195, 106, 127, 127, 118, 3, 106, 4, 106, 129, 119    },                              /*
		                                                                                        *srate=0,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 129, 5, 127, 116, 116, 0, 5, 1, 5, 129, 119    },                                    /*
		                                                                                        *srate=1,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 129, 5, 127, 116, 116, 1, 5, 1, 5, 129, 119    },                                    /*
		                                                                                        *srate=1,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 130, 5, 127, 120, 117, 18, 5, 3, 37, 129, 119    },                                  /*
		                                                                                        *srate=1,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 131, 37, 127, 127, 118, 3, 37, 4, 37, 129, 119    },                                 /*
		                                                                                        *srate=1,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 132, 37, 127, 127, 118, 3, 37, 21, 69, 129, 119    },                                /*
		                                                                                        *srate=1,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 133, 69, 127, 127, 118, 4, 69, 6, 101, 129, 119    },                                /*
		                                                                                        *srate=1,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 134, 69, 127, 127, 118, 21, 69, 6, 101, 129, 119    },                               /*
		                                                                                        *srate=1,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 134, 101, 127, 127, 118, 21, 69, 6, 101, 129, 119    },                              /*
		                                                                                        *srate=1,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 131, 2, 127, 116, 116, 18, 4, 3, 4, 129, 119    },                                   /*
		                                                                                        *srate=2,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 131, 2, 127, 116, 116, 3, 4, 3, 4, 129, 119    },                                    /*
		                                                                                        *srate=2,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 132, 2, 127, 120, 117, 3, 36, 4, 36, 129, 119    },                                  /*
		                                                                                        *srate=2,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 133, 2, 127, 127, 118, 4, 36, 6, 68, 129, 119    },                                  /*
		                                                                                        *srate=2,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 134, 2, 127, 127, 118, 21, 36, 6, 68, 129, 119    },                                 /*
		                                                                                        *srate=2,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 135, 34, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                                /*
		                                                                                        *srate=2,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 135, 34, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                                /*
		                                                                                        *srate=2,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 136, 34, 127, 127, 118, 7, 100, 24, 100, 129, 119    },                              /*
		                                                                                        *srate=2,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 132, 2, 127, 116, 116, 3, 36, 4, 36, 129, 119    },                                  /*
		                                                                                        *srate=3,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 132, 2, 127, 116, 116, 3, 36, 4, 36, 129, 119    },                                  /*
		                                                                                        *srate=3,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 133, 2, 127, 120, 117, 4, 36, 6, 68, 129, 119    },                                  /*
		                                                                                        *srate=3,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 134, 2, 127, 127, 118, 21, 68, 6, 100, 129, 119    },                                /*
		                                                                                        *srate=3,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 135, 34, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                                /*
		                                                                                        *srate=3,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 136, 34, 127, 127, 118, 6, 100, 7, 100, 129, 119    },                               /*
		                                                                                        *srate=3,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 137, 66, 127, 127, 118, 7, 100, 24, 132, 129, 119    },                              /*
		                                                                                        *srate=3,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 137, 66, 127, 127, 118, 7, 100, 24, 132, 129, 119    },                              /*
		                                                                                        *srate=3,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 70, 1, 127, 116, 116, 21, 2, 6, 2, 129, 119    },                                    /*
		                                                                                        *srate=4,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 70, 1, 127, 116, 116, 21, 2, 6, 2, 129, 119    },                                    /*
		                                                                                        *srate=4,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 71, 1, 127, 120, 117, 21, 2, 7, 34, 129, 119    },                                   /*
		                                                                                        *srate=4,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 72, 1, 127, 127, 118, 6, 34, 7, 34, 129, 119    },                                   /*
		                                                                                        *srate=4,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 73, 1, 127, 127, 118, 7, 34, 24, 66, 129, 119    },                                  /*
		                                                                                        *srate=4,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 74, 33, 127, 127, 118, 24, 66, 9, 66, 129, 119    },                                 /*
		                                                                                        *srate=4,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 74, 33, 127, 127, 118, 9, 66, 9, 98, 129, 119    },                                  /*
		                                                                                        *srate=4,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 75, 33, 127, 127, 118, 9, 66, 10, 98, 129, 119    },                                 /*
		                                                                                        *srate=4,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 71, 1, 127, 116, 116, 6, 2, 7, 34, 129, 119    },                                    /*
		                                                                                        *srate=5,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 71, 1, 127, 116, 116, 6, 2, 7, 34, 129, 119    },                                    /*
		                                                                                        *srate=5,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 72, 1, 127, 120, 117, 6, 34, 7, 34, 129, 119    },                                   /*
		                                                                                        *srate=5,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 73, 1, 127, 127, 118, 24, 34, 24, 66, 129, 119    },                                 /*
		                                                                                        *srate=5,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 74, 33, 127, 127, 118, 24, 66, 9, 66, 129, 119    },                                 /*
		                                                                                        *srate=5,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 75, 33, 127, 127, 118, 9, 66, 10, 98, 129, 119    },                                 /*
		                                                                                        *srate=5,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 75, 65, 127, 127, 118, 10, 98, 10, 98, 129, 119    },                                /*
		                                                                                        *srate=5,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 75, 65, 127, 127, 118, 10, 98, 27, 98, 129, 119    }                                 /*
		                                                                                        *srate=5,
		                                                                                        *midx=7
		                                                                                        **/ \
	}

#else /* #ifdef RF215v1 */

/* columns: {
 *txcutc,txdfe,pac04,pac09,pac24,rxbwc09,rxdfe09,rxbwc24,rxdfe24,agcc,agcs } */
/* cow index: srate_midx = srate << 3 + midx */
#define FSK_PARAMS \
	{ \
		{ 192, 136, 127, 116, 116, 0, 10, 0, 10, 129, 119    },                                /*
		                                                                                        *srate=0,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 192, 136, 127, 116, 116, 0, 10, 0, 10, 129, 119    },                                /*
		                                                                                        *srate=0,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 192, 136, 127, 120, 117, 0, 10, 0, 42, 129, 119    },                                /*
		                                                                                        *srate=0,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 192, 136, 127, 127, 118, 0, 42, 1, 42, 129, 119    },                                /*
		                                                                                        *srate=0,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 193, 136, 127, 127, 118, 1, 74, 18, 74, 129, 119    },                               /*
		                                                                                        *srate=0,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 194, 136, 127, 127, 118, 18, 74, 3, 106, 129, 119    },                              /*
		                                                                                        *srate=0,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 195, 136, 127, 127, 118, 3, 106, 3, 106, 129, 119    },                              /*
		                                                                                        *srate=0,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 195, 136, 127, 127, 118, 3, 106, 4, 106, 129, 119    },                              /*
		                                                                                        *srate=0,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 129, 132, 127, 116, 116, 0, 5, 1, 5, 129, 119    },                                  /*
		                                                                                        *srate=1,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 129, 132, 127, 116, 116, 1, 5, 1, 5, 129, 119    },                                  /*
		                                                                                        *srate=1,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 130, 132, 127, 120, 117, 18, 5, 3, 37, 129, 119    },                                /*
		                                                                                        *srate=1,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 131, 132, 127, 127, 118, 3, 37, 4, 37, 129, 119    },                                /*
		                                                                                        *srate=1,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 132, 132, 127, 127, 118, 3, 37, 21, 69, 129, 119    },                               /*
		                                                                                        *srate=1,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 133, 132, 127, 127, 118, 4, 69, 6, 101, 129, 119    },                               /*
		                                                                                        *srate=1,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 134, 132, 127, 127, 118, 21, 69, 6, 101, 129, 119    },                              /*
		                                                                                        *srate=1,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 134, 132, 127, 127, 118, 21, 69, 6, 101, 129, 119    },                              /*
		                                                                                        *srate=1,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 131, 130, 127, 116, 116, 18, 4, 3, 4, 129, 119    },                                 /*
		                                                                                        *srate=2,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 131, 130, 127, 116, 116, 3, 4, 3, 4, 129, 119    },                                  /*
		                                                                                        *srate=2,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 132, 130, 127, 120, 117, 3, 36, 4, 36, 129, 119    },                                /*
		                                                                                        *srate=2,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 133, 130, 127, 127, 118, 4, 36, 6, 68, 129, 119    },                                /*
		                                                                                        *srate=2,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 134, 130, 127, 127, 118, 21, 36, 6, 68, 129, 119    },                               /*
		                                                                                        *srate=2,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 135, 130, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                               /*
		                                                                                        *srate=2,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 135, 130, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                               /*
		                                                                                        *srate=2,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 136, 130, 127, 127, 118, 7, 100, 24, 100, 129, 119    },                             /*
		                                                                                        *srate=2,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 132, 130, 127, 116, 116, 3, 36, 4, 36, 129, 119    },                                /*
		                                                                                        *srate=3,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 132, 130, 127, 116, 116, 3, 36, 4, 36, 129, 119    },                                /*
		                                                                                        *srate=3,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 133, 130, 127, 120, 117, 4, 36, 6, 68, 129, 119    },                                /*
		                                                                                        *srate=3,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 134, 130, 127, 127, 118, 21, 68, 6, 100, 129, 119    },                              /*
		                                                                                        *srate=3,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 135, 130, 127, 127, 118, 6, 68, 7, 100, 129, 119    },                               /*
		                                                                                        *srate=3,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 136, 130, 127, 127, 118, 6, 100, 7, 100, 129, 119    },                              /*
		                                                                                        *srate=3,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 137, 130, 127, 127, 118, 7, 100, 24, 132, 129, 119    },                             /*
		                                                                                        *srate=3,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 137, 130, 127, 127, 118, 7, 100, 24, 132, 129, 119    },                             /*
		                                                                                        *srate=3,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 70, 129, 127, 116, 116, 21, 2, 6, 2, 129, 119    },                                  /*
		                                                                                        *srate=4,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 70, 129, 127, 116, 116, 21, 2, 6, 2, 129, 119    },                                  /*
		                                                                                        *srate=4,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 71, 129, 127, 120, 117, 21, 2, 7, 34, 129, 119    },                                 /*
		                                                                                        *srate=4,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 72, 129, 127, 127, 118, 6, 34, 7, 34, 129, 119    },                                 /*
		                                                                                        *srate=4,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 73, 129, 127, 127, 118, 7, 34, 24, 66, 129, 119    },                                /*
		                                                                                        *srate=4,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 74, 129, 127, 127, 118, 24, 66, 9, 66, 129, 119    },                                /*
		                                                                                        *srate=4,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 74, 129, 127, 127, 118, 9, 66, 9, 98, 129, 119    },                                 /*
		                                                                                        *srate=4,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 75, 129, 127, 127, 118, 9, 66, 10, 98, 129, 119    },                                /*
		                                                                                        *srate=4,
		                                                                                        *midx=7
		                                                                                        **/ \
		{ 71, 129, 127, 116, 116, 6, 2, 7, 34, 129, 119    },                                  /*
		                                                                                        *srate=5,
		                                                                                        *midx=0
		                                                                                        **/ \
		{ 71, 129, 127, 116, 116, 6, 2, 7, 34, 129, 119    },                                  /*
		                                                                                        *srate=5,
		                                                                                        *midx=1
		                                                                                        **/ \
		{ 72, 129, 127, 120, 117, 6, 34, 7, 34, 129, 119    },                                 /*
		                                                                                        *srate=5,
		                                                                                        *midx=2
		                                                                                        **/ \
		{ 73, 129, 127, 127, 118, 24, 34, 24, 66, 129, 119    },                               /*
		                                                                                        *srate=5,
		                                                                                        *midx=3
		                                                                                        **/ \
		{ 74, 129, 127, 127, 118, 24, 66, 9, 66, 129, 119    },                                /*
		                                                                                        *srate=5,
		                                                                                        *midx=4
		                                                                                        **/ \
		{ 75, 129, 127, 127, 118, 9, 66, 10, 98, 129, 119    },                                /*
		                                                                                        *srate=5,
		                                                                                        *midx=5
		                                                                                        **/ \
		{ 75, 129, 127, 127, 118, 10, 98, 10, 98, 129, 119    },                               /*
		                                                                                        *srate=5,
		                                                                                        *midx=6
		                                                                                        **/ \
		{ 75, 129, 127, 127, 118, 10, 98, 27, 98, 129, 119    }                                /*
		                                                                                        *srate=5,
		                                                                                        *midx=7
		                                                                                        **/ \
	}

#endif /* #ifdef RF215v1 */

#if (defined RF215v3)
/* Pre-emphasis filter coefficients */
/* row: FSK data rate, columns: values for registers FSKPE0, FSKPE1, FSKPE2 */
#define FSK_PE_TABLE \
	{ \
		2, 3, 252, /* 50kHz */ \
		14, 15, 240, /* 100kHz */ \
		60, 63, 192, /* 150kHz */ \
		116, 127, 128, /* 200kHz */ \
		5, 60, 195, /* 300kHz */ \
		19, 41, 199 /* 400kHz */ \
	}
#endif

#endif /* TAL_FE_FSK_PARAMS_H */
