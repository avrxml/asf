/*****************************************************************************
 *
 * \file
 *
 * \brief Accelerometer sensor header file.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
