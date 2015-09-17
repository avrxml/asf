/*****************************************************************************
 *
 * \file
 *
 * \brief Software Driver for the LIS3L06AL Accelerometer for AVR UC3 A & B devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "board.h"
#include "gpio.h"
#include "adc.h"
#include "lis3l06al.h"
#include "math.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

avr32_acc_t acc = {
  // adc measure
  .m = { .x=0 , .y=0 , .z=0 } ,

  // angles
  .pos.left    = 0,
  .pos.right   = 0,
  .pos.up      = 0,
  .pos.down    = 0,
  .pos.topdown = 0,

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


static void xyz_add (xyz_t p, xyz_t q, xyz_t* r )
{
  r->x = p.x + q.x ;
  r->y = p.y + q.y ;
  r->z = p.z + q.z ;
}

static void xyz_diff (xyz_t p, xyz_t q, xyz_t* r)
{
  r->x = p.x - q.x ;
  r->y = p.y - q.y ;
  r->z = p.z - q.z ;
}

static int round0 (int i, int ceil)
{
  return ( (abs(i) < abs(ceil)) ? 0 : i ) ;
}

static void xyz_round0 (xyz_t p ,int ceil, xyz_t* r )
{
  r->x = round0( p.x , ceil ) ;
  r->y = round0( p.y , ceil ) ;
  r->z = round0( p.z , ceil ) ;
}

static int xyz_sumsq (xyz_t p)
{
  return ( (p.x*p.x) + (p.y*p.y) + (p.z*p.z) ) ;
}

static void acc_get_value (
   volatile avr32_adc_t * adc
,  xyz_t* val ) {

  // get value for adc channel
  adc_enable(adc,  ADC_CHANNEL_X);
  adc_start(adc) ;
  val->x = ( adc_get_value(adc, ADC_CHANNEL_X) >> ACC_SHIFT ) ;
  adc_disable(adc, ADC_CHANNEL_X);

  adc_enable(adc,  ADC_CHANNEL_Y);
  adc_start(adc) ;
  val->y = ( adc_get_value(adc, ADC_CHANNEL_Y) >> ACC_SHIFT ) ;
  adc_disable(adc, ADC_CHANNEL_Y);

  adc_enable(adc,  ADC_CHANNEL_Z);
  adc_start(adc) ;
  val->z = ( adc_get_value(adc, ADC_CHANNEL_Z) >> ACC_SHIFT ) ;
  adc_disable(adc, ADC_CHANNEL_Z);
}



// acceleration about 1g +-10%
// gravity is always present !
// this is used to decide slow / fast behavior
bool is_acc_slow()
{
  return ( (acc.ak2 > ACC_QUIET_LO) && (acc.ak2 < ACC_QUIET_HI) ) ;
}


// left / right / up / down
// NO HYSTERESIS
// global = acc.

//char acc_left  () { return ( (acc.m.x > ACC_TRIG_X1) && (acc.m.z > ACC_TRIG_Z_TOP) ) ; }
//char acc_right () { return ( (acc.m.x < ACC_TRIG_X0) && (acc.m.z > ACC_TRIG_Z_TOP) ) ; }
//
//char acc_up    () { return ( (acc.m.y < ACC_TRIG_Y0) && (acc.m.z > ACC_TRIG_Z_TOP) ) ; }
//char acc_down  () { return ( (acc.m.y > ACC_TRIG_Y1) && (acc.m.z > ACC_TRIG_Z_TOP) ) ; }
//
//char acc_topdown () { return (acc.m.z > ACC_TRIG_Z_BOT) ; }


// left / right / up / down
// WITH HYSTERESIS
// global = acc.

bool is_acc_left()
{
  if      ( (acc.pos.left==0) && (acc.m.x > (int)(1.01*ACC_TRIG_X1)) ) acc.pos.left = 1 ;
  else if ( (acc.pos.left==1) && (acc.m.x < (int)(0.99*ACC_TRIG_X1)) ) acc.pos.left = 0 ;
  return acc.pos.left ;
}
bool is_acc_right()
{
  if      ( (acc.pos.right==0) && (acc.m.x < (int)(0.99*ACC_TRIG_X0)) ) acc.pos.right = 1 ;
  else if ( (acc.pos.right==1) && (acc.m.x > (int)(1.01*ACC_TRIG_X0)) ) acc.pos.right = 0 ;
  return acc.pos.right ;
}

bool is_acc_up()
{
  if      ( (acc.pos.up==0) && (acc.m.y < (int)(0.99*ACC_TRIG_Y0)) ) acc.pos.up = 1 ;
  else if ( (acc.pos.up==1) && (acc.m.y > (int)(1.01*ACC_TRIG_Y0)) ) acc.pos.up = 0 ;
  return acc.pos.up ;
}
bool is_acc_down()
{
  if      ( (acc.pos.down==0) && (acc.m.y > (int)(1.01*ACC_TRIG_Y1)) ) acc.pos.down = 1 ;
  else if ( (acc.pos.down==1) && (acc.m.y < (int)(0.99*ACC_TRIG_Y1)) ) acc.pos.down = 0 ;
  return acc.pos.down ;
}

bool is_acc_topdown()
{
  if      ( (acc.pos.topdown==0) && (acc.m.z > (int)(1.01*ACC_TRIG_Z_BOT)) ) acc.pos.topdown = 1 ;
  else if ( (acc.pos.topdown==1) && (acc.m.z < (int)(0.99*ACC_TRIG_Z_BOT)) ) acc.pos.topdown = 0 ;
  return acc.pos.topdown ;
}

bool is_acc_meuh(char stop)
{
  static char topdown = 0 ;
  static char meuh = 0 ;
  if ( stop )  meuh = 0 ;
  if      ( (topdown==0) && (acc.m.z > ACC_TRIG_Z_BOT) ) topdown = 1 ;
  else if ( (topdown==1) && (acc.m.z < ACC_TRIG_Z_TOP) ) { topdown = 0 ; meuh = 1 ; }
  return meuh ;
}

void acc_init(void)
{
  volatile avr32_adc_t *adc = &AVR32_ADC;
  static const gpio_map_t ADC_GPIO_MAP =
  {
    {AVR32_ADC_AD_3_PIN,  AVR32_ADC_AD_3_FUNCTION}, // ADC channel 3
    {AVR32_ADC_AD_1_PIN,  AVR32_ADC_AD_1_FUNCTION}, // ADC channel 1
    {AVR32_ADC_AD_2_PIN,  AVR32_ADC_AD_2_FUNCTION}  // ADC channel 2
  };

  // enable GPIO pins for ADC
  gpio_enable_module(ADC_GPIO_MAP,
                     sizeof(ADC_GPIO_MAP) / sizeof(ADC_GPIO_MAP[0]));

  // configure ADC
  adc_configure(adc);
}

void acc_update( void )
{
  volatile avr32_adc_t *adc = &AVR32_ADC;

  // start + get from adc
  acc_get_value(adc, &acc.m) ;

  // ak  = acceleration = m - k
  xyz_diff( acc.m , acc.k, &acc.ak ) ;
  acc.ak2 = xyz_sumsq( acc.ak ) ;

  if ( is_acc_slow() ) {
     // update g for next move
     acc.g.x = acc.m.x ;
     acc.g.y = acc.m.y ;
     acc.g.z = acc.m.z ;
  }
  else {
     xyz_diff( acc.m , acc.g, &acc.ag ) ;
     xyz_round0( acc.ag , 16, &acc.ag ) ;
     xyz_add( acc.s , acc.ag, &acc.s ) ;
     //xyz_round0( acc.s , 256, &acc.s ) ;
     //print_dbg("m = "); print_xyz( acc.m ) ;
     //print_dbg("k = "); print_xyz( acc.k ) ;
     //print_dbg("g = "); print_xyz( acc.g ) ;
     //print_dbg("ag = "); print_xyz( acc.ag ) ;
     //print_dbg("s = "); print_xyz( acc.s ) ;
     //print_dbg("\r\n");
     ////acc.s2 = xyz_sumsq( acc.s ) ;
     //print_dbg("s2 = 0x"); print_dbg_hex(acc.s2); print_dbg("\r\n");
  }
}

#define DEG_2_RAD(deg)  ((double)(deg)*3.14/180)
signed long is_acc_abs_angle_x( unsigned long abs_ang )
{
  if( abs(acc.ak.x) > (int)(sin(DEG_2_RAD(abs_ang)) * ACC_1G) )
  {
     if ( acc.ak.x > 0 ) return 1;
     else                return -1;
  }
  return 0;
}

signed long is_acc_abs_angle_y( unsigned long abs_ang )
{
  if( abs(acc.ak.y) > (int)(sin(DEG_2_RAD(abs_ang)) * ACC_1G) )
  {
     if ( acc.ak.y > 0 ) return 1;
     else                return -1;
  }
  return 0;
}

unsigned long acc_get_m_x( void )
{
   return acc.m.x;
}

unsigned long acc_get_m_y( void )
{
   return acc.m.y;
}

unsigned long acc_get_m_z( void )
{
   return acc.m.z;
}
