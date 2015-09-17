/**************************************************************************
 *
 * \file
 *
 * \brief Accelerometer sensor driver.
 *
 * This file provides an example for the ADC + Accelerometer on AVR32 UC3 devices
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

# include <stdio.h>

# include "s_accelerometer.h"
# include "board.h"
# include "gpio.h"
# include "pm.h"
# include "adc.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


xyz_t g_angle;
volatile avr32_adc_t * adc= (volatile avr32_adc_t *) &AVR32_ADC;

unsigned char adc_channel_x = 1;
unsigned char adc_channel_y = 2;
unsigned char adc_channel_z = 3;


struct S_acc_conv_t {
   U16 sin;
   U8  angle;
};

struct S_acc_conv_t acc_table []=
{  {  .sin=(U16)(SIN90 * ACC_1G), .angle=90}
,  {  .sin=(U16)(SIN85 * ACC_1G), .angle=85}
,  {  .sin=(U16)(SIN80 * ACC_1G), .angle=80}
,  {  .sin=(U16)(SIN75 * ACC_1G), .angle=75}
,  {  .sin=(U16)(SIN70 * ACC_1G), .angle=70}
,  {  .sin=(U16)(SIN65 * ACC_1G), .angle=65}
,  {  .sin=(U16)(SIN60 * ACC_1G), .angle=60}
,  {  .sin=(U16)(SIN55 * ACC_1G), .angle=55}
,  {  .sin=(U16)(SIN50 * ACC_1G), .angle=50}
,  {  .sin=(U16)(SIN45 * ACC_1G), .angle=45}
,  {  .sin=(U16)(SIN40 * ACC_1G), .angle=40}
,  {  .sin=(U16)(SIN35 * ACC_1G), .angle=35}
,  {  .sin=(U16)(SIN30 * ACC_1G), .angle=30}
,  {  .sin=(U16)(SIN25 * ACC_1G), .angle=25}
,  {  .sin=(U16)(SIN20 * ACC_1G), .angle=20}
,  {  .sin=(U16)(SIN15 * ACC_1G), .angle=15}
,  {  .sin=(U16)(SIN10 * ACC_1G), .angle=10}
,  {  .sin=(U16)(SIN5  * ACC_1G), .angle=5 }
,  {  .sin=(U16)(SIN0  * ACC_1G), .angle=0 }
};

Avr32_acc_t acc = {
   // adc measure
   .m = { .x=0 , .y=0 , .z=0 } ,

   // angles
   .left = 0 ,
   .right = 0 ,
   .up = 0 ,
   .down = 0 ,
   .topdown = 0 ,

   // required to eval quiet or move
   .k = { .x=ACC_ZERO_X , .y=ACC_ZERO_Y , .z=ACC_ZERO_Z } ,  // constant ( 0 , 0 , 0 )
   .ak = { .x=0 , .y=0 , .z=0 } ,
   .ak2 = 0 ,

   // optional (for move)
   .g = { .x=ACC_ZERO_X , .y=ACC_ZERO_Y , .z=ACC_MIN } ,  // update ( 0 , 0 , 1g )
   .ag = { .x=0 , .y=0 , .z=0 } ,
   .ag2 = 0 ,
   .s = { .x=0 , .y=0 , .z=0 } ,
   .s2 = 0
} ;


//_____ D E C L A R A T I O N S ____________________________________________

static void xyz_diff (xyz_t p, xyz_t q, xyz_t* r)
{
  r->x = p.x - q.x ;
  r->y = p.y - q.y ;
  r->z = p.z - q.z ;
}

static int xyz_sumsq (xyz_t p)
{
  return ( (p.x*p.x) + (p.y*p.y) + (p.z*p.z) ) ;
}

// 3x ADC conversion
static void acc_get_value ( volatile avr32_adc_t * adc, unsigned char adc_channel_x, unsigned char adc_channel_y, unsigned char adc_channel_z, xyz_t* val)
{

  adc_enable( adc,adc_channel_x);
  adc_start(  adc) ;
  val->x = ( adc_get_value(adc, adc_channel_x) >> ACC_SHIFT ) ;
  adc_disable( adc,adc_channel_x);

  adc_enable( adc,adc_channel_y);
  adc_start(  adc) ;
  val->y = ( adc_get_value(adc, adc_channel_y) >> ACC_SHIFT ) ;
  adc_disable( adc,adc_channel_y);

  adc_enable( adc,adc_channel_z);
  adc_start(  adc) ;
  val->z = ( adc_get_value(adc, adc_channel_z) >> ACC_SHIFT ) ;
  adc_disable( adc,adc_channel_z);

  //printf("x=%d y=%d z=%d \n\r", val->x, val->y, val->z);
}


// acceleration about 1g +-10%
// gravity is always present !
// this is used to decide slow / fast behavior
static char acc_slow()
{
  return ( (acc.ak2 > ACC_QUIET_LO) && (acc.ak2 < ACC_QUIET_HI) ) ;
}


static void accelerometer_action_x( char* buf )
{
  if (acc_slow())
  {
      U32 i;
      for( i=0 ; i<sizeof(acc_table)/sizeof(struct S_acc_conv_t) ; i++)
      {
         if( abs(acc.ak.x) >= acc_table[i].sin )
         {
            if( acc.ak.z<0 )
               if( acc.ak.x>0 )
               {
                  g_angle.x =  acc_table[i].angle ;
                  //printf("1. g_angle.x = %d\n\r", g_angle.x);
               }
               else
               {
                  g_angle.x = 270 + 90-acc_table[i].angle ;
                  if( g_angle.x==360 )
                     g_angle.x=0;
                  //printf("2. g_angle.x = %d\n\r", g_angle.x);
               }
            else
               if( acc.ak.x>0 )
               {
                  g_angle.x =  90 + 90-acc_table[i].angle ;
                  //printf("3. g_angle.x = %d\n\r", g_angle.x);
               }
               else
               {
                  g_angle.x = 180 + acc_table[i].angle ;
                  //printf("4. g_angle.x = %d\n\r", g_angle.x);
               }

            //printf("g_angle.x = %d\n\r", g_angle.x);
            break;
         }
      }
   }
}

static void accelerometer_action_y( char* buf )
{
  if (acc_slow())
  {
      U32 i;

      for( i=0 ; i<sizeof(acc_table)/sizeof(struct S_acc_conv_t) ; i++)
      {
         if( abs(acc.ak.y) >= acc_table[i].sin )
         {
            if( acc.ak.z<0 )
               if( acc.ak.y>0 )
               {
                  g_angle.y =  acc_table[i].angle ;
                  //printf("1. g_angle.y = %d\n\r", g_angle.y);
               }
               else
               {
                  g_angle.y = 270 + 90-acc_table[i].angle ;
                  if( g_angle.y==360 )
                     g_angle.y=0;
                  //printf("2. g_angle.y = %d\n\r", g_angle.y);
               }
            else
               if( acc.ak.y>0 )
               {
                  g_angle.y =  90 + 90-acc_table[i].angle ;
                  //printf("3. g_angle.y = %d\n\r", g_angle.y);
               }
               else
               {
                  g_angle.y = 180 + acc_table[i].angle ;
                  //printf("4. g_angle.y = %d\n\r", g_angle.y);
               }
            //printf("g_angle.y = %d\n\r", g_angle.y);
            break;
         }
      }
   }
}



/*!
 *  \brief Init the accelerometer driver
 */
void accelerometer_init(void)
{
  // enable GPIO pins for ADC
  gpio_enable_module_pin(AVR32_ADC_AD_1_PIN,   AVR32_ADC_AD_1_FUNCTION);   // ADC channel 1
  gpio_enable_module_pin(AVR32_ADC_AD_2_PIN,   AVR32_ADC_AD_2_FUNCTION);   // ADC channel 2
  gpio_enable_module_pin(AVR32_ADC_AD_3_PIN,   AVR32_ADC_AD_3_FUNCTION);   // ADC channel 3


  // configure ADC
  adc_configure(adc);
}



/*!
 *  \brief Measures the accelerometer values and converts them to X and Y angles.
 *
 *  \param angle Boolean (0 for X-angle, 1 for Y-angle)
 *  \param buf char buffer in which the light sensor value is stored.
 */
void accelerometer_measure(U32 angle, char* buf)
{
  // start + get from adc
  acc_get_value(adc, adc_channel_x, adc_channel_y, adc_channel_z, &acc.m ) ;

  // ak  = acceleration = m - k
  xyz_diff( acc.m , acc.k, &acc.ak ) ;
  //printf("x=%d y=%d z=%d \n\r", acc.ak.x, acc.ak.y, acc.ak.z);
  acc.ak2 = xyz_sumsq( acc.ak ) ;

  if(!angle)
  {
     accelerometer_action_x( buf );
     sprintf( buf, "%3d\r\n", g_angle.x);
  } else {
     accelerometer_action_y( buf );
     sprintf( buf, "%3d\r\n", g_angle.y);
  }

  // update g for next move
  acc.g.x = acc.m.x ;
  acc.g.y = acc.m.y ;
  acc.g.z = acc.m.z ;
}
