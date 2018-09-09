/*****************************************************************************
 *
 * \file
 *
 * \brief Software Driver for the LIS3L06AL Accelerometer for AVR UC3 A & B devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
