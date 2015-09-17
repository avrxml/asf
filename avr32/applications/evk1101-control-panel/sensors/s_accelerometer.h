/*****************************************************************************
 *
 * \file
 *
 * \brief Accelerometer sensor header file.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


# ifndef _S_ACCELEROMETER_H
# define _S_ACCELEROMETER_H

#include "compiler.h"

//_____ M A C R O S ________________________________________________________



//_____ D E F I N I T I O N S ______________________________________________

// the 4 LSB's are unstable
// even with board on table
// on the other and , with shift 4 , needs different TRIG / QUIET settings
#define ACC_SHIFT 2

// nominal values out of accelerometer spec
#define ACC_ZERO  (512 >> ACC_SHIFT)
#define ACC_1G    (205 >> ACC_SHIFT)
#define ACC_MIN   ( ACC_ZERO - ACC_1G )
#define ACC_MAX   ( ACC_ZERO + ACC_1G )

#define SIN0  0
#define SIN5  0.087
#define SIN10 0.174
#define SIN15 0.259
#define SIN20 0.342
#define SIN25 0.423
#define SIN30 0.5
#define SIN35 0.574
#define SIN40 0.643
#define SIN45 0.707
#define SIN50 0.766
#define SIN55 0.819
#define SIN60 0.866
#define SIN65 0.906
#define SIN70 0.94
#define SIN75 0.966
#define SIN80 0.985
#define SIN85 0.996
#define SIN90 1

#define ACC_CALIB_X  0
#define ACC_CALIB_Y  0
#define ACC_CALIB_Z  0

#define ACC_ZERO_X ( ACC_ZERO + ACC_CALIB_X )
#define ACC_ZERO_Y ( ACC_ZERO + ACC_CALIB_Y )
#define ACC_ZERO_Z ( ACC_ZERO + ACC_CALIB_Z )

#define ACC_TRIG_X0 (int) ( ACC_ZERO_X - SIN15 * ACC_1G )
#define ACC_TRIG_X1 (int) ( ACC_ZERO_X + SIN15 * ACC_1G )
#define ACC_TRIG_Y0 (int) ( ACC_ZERO_Y - SIN15 * ACC_1G )
#define ACC_TRIG_Y1 (int) ( ACC_ZERO_Y + SIN15 * ACC_1G )
#define ACC_TRIG_Z_TOP (int) ( ACC_ZERO_Z - SIN75 * ACC_1G )
#define ACC_TRIG_Z_BOT (int) ( ACC_ZERO_Z + SIN60 * ACC_1G )

#define ACC_QUIET_LO (int) ( 0.6 * ACC_1G * ACC_1G )
#define ACC_QUIET_HI (int) ( 1.4 * ACC_1G * ACC_1G )

typedef struct { int x ; int y ; int z ; } xyz_t ;

typedef struct {
   // adc measure
   xyz_t m ;

   // angles
   char left ; char right ; char up ; char down ;
   char topdown ;

   // required to decide slow or fast
   xyz_t k ; xyz_t ak ; int ak2 ;

   // optional (for fast moves)
   xyz_t g ; xyz_t ag ; int ag2 ;
   xyz_t s ; int s2 ;

} Avr32_acc_t ;



//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Init the accelerometer driver
 */
void accelerometer_init();

/*!
 *  \brief Measures the accelerometer values and converts them to X and Y angles.
 *
 *  \param angle Boolean (0 for X-angle, 1 for Y-angle)
 *  \param buf char buffer in which the light sensor value is stored.
 */
void accelerometer_measure(U32 angle, char* buf);

#endif
