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


#ifndef _LIS3106AL_H_
#define _LIS3106AL_H_

/**
 * \defgroup group_avr32_components_accelerometer_lis3l06al MEMS Sensors - Accelerometer LIS3L06AL
 *
 * Software Driver for the Accelerometer LIS3L06AL. This driver relies on the ADC hardware module.
 * It provides functions to initialize the module, to read (x,y,z) coordinates and to get acceleration/orientation/angle information.
 *
 * \{
 */

//_____ D E F I N I T I O N S ______________________________________________

// the 4 LSB's are unstable
// even with board on table
// on the other and , with shift 4 , needs different TRIG / QUIET settings
#define ACC_SHIFT 2

/*! \name Nominal values out of lis3l06al accelerometer spec
 */
//! @{
#define ACC_ZERO  (512 >> ACC_SHIFT)
#define ACC_1G    (205 >> ACC_SHIFT)
#define ACC_MIN   ( ACC_ZERO - ACC_1G )
#define ACC_MAX   ( ACC_ZERO + ACC_1G )
//! @}

/*! \name Sinus table
 */
//! @{
#define SIN0  0
#define SIN15 0.26
#define SIN30 0.5
#define SIN45 0.71
#define SIN60 0.87
#define SIN75 0.97
#define SIN90 1
//! @}

/*! \name Axis calibration correction
 */
//! @{
#define ACC_CALIB_X  0
#define ACC_CALIB_Y  0
#define ACC_CALIB_Z  0
//! @}

/*! \name Axis zero point.
 */
//! @{
#define ACC_ZERO_X ( ACC_ZERO + ACC_CALIB_X )
#define ACC_ZERO_Y ( ACC_ZERO + ACC_CALIB_Y )
#define ACC_ZERO_Z ( ACC_ZERO + ACC_CALIB_Z )
//! @}

/*! \name Axis reference values for a set of predefined angles.
 */
//! @{
#define ACC_TRIG_X0 (int) ( ACC_ZERO_X - SIN15 * ACC_1G )
#define ACC_TRIG_X1 (int) ( ACC_ZERO_X + SIN15 * ACC_1G )
#define ACC_TRIG_Y0 (int) ( ACC_ZERO_Y - SIN15 * ACC_1G )
#define ACC_TRIG_Y1 (int) ( ACC_ZERO_Y + SIN15 * ACC_1G )
#define ACC_TRIG_Z_TOP (int) ( ACC_ZERO_Z - SIN75 * ACC_1G )
#define ACC_TRIG_Z_BOT (int) ( ACC_ZERO_Z + SIN60 * ACC_1G )
//! @}

/*! \name Sudden acceleration thresholds
 */
//! @{
#define ACC_QUIET_LO (int) ( 0.6 * ACC_1G * ACC_1G )
#define ACC_QUIET_HI (int) ( 1.4 * ACC_1G * ACC_1G )
//! @}

/*! \name Accelerometer ADC channel mapping
 */
//! @{
#if ( BOARD==EVK1101 )
#  define ADC_CHANNEL_X  ADC_ACC_X_CHANNEL
#  define ADC_CHANNEL_Y  ADC_ACC_Y_CHANNEL
#  define ADC_CHANNEL_Z  ADC_ACC_Z_CHANNEL
#else
#  define ADC_CHANNEL_X  0
#  define ADC_CHANNEL_Y  1
#  define ADC_CHANNEL_Z  2
#  warning 'ADC_CHANNEL_X,_Y,_Z should be customized for the application'
#endif
//! @}


//! All 3-axis values from ADC conversion.
typedef struct
{
  int x ; //!< x-axis value.
  int y ; //!< y-axis value.
  int z ; //!< z-axis value.
} xyz_t ;


//! Set of computed data.
typedef struct {

  xyz_t m ; //!< 3-axis adc measures

  struct {
     unsigned char left   :1 ;  //!< flip over to the left state
     unsigned char right  :1 ;  //!< flip over to the right state
     unsigned char up     :1 ;  //!< flip forward state
     unsigned char down   :1 ;  //!< flip backward state
     unsigned char topdown:1 ;  //!< top-down state
  } pos;  //!< Angle direction states.

  //! Required to decide slow or fast
  //! @{
  xyz_t k ;       //!< zero point   = ( 0 , 0 , 0 )
  xyz_t ak ;      //!< acceleration = m - k
  int   ak2 ;       //!< length       = sumsq(ak)
  //! @}

  //! optional (for fast moves)
  //! @{
  xyz_t g ;       //!< gravity      = ( 0 , 0 , 1g )
  xyz_t ag ;      //!< acceleration = m - g
  int ag2 ;       //!< length       = sumsq(ag)
  xyz_t s ;       //!< speed        = sum(a)
  int s2 ;        //!< length       = sumsq(s) [with sumsq = x*x + y*y + z*z]
  //! @}
} avr32_acc_t ;


/*! \brief Initialize Accelerometer driver. Mandatory to call.
 */
void acc_init(void);

/*! \brief Get accelerometer measure (3x) and process results.
 */
void acc_update(void);

/*! \brief Test acceleration about 1g +-10%
 *
 * Gravity is always present! This is used to decide slow / fast behavior
 *
 * \return bool      true if slow motion is detected,
 *                   false if fast  motion is detected.
 */
bool is_acc_slow(void);

/*! \brief Test if sensor have detected 'left' orientation.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if left is detected,
 *                   false if left is not detected.
 */
bool is_acc_left(void);

/*! \brief Test if sensor have detected 'right' orientation.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if right is detected,
 *                   false if right is not detected.
 */
bool is_acc_right(void);

/*! \brief Test if sensor have detected 'up' orientation.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if up is detected,
 *                   false if up is not detected.
 */
bool is_acc_up(void);

/*! \brief Test if sensor have detected 'down' orientation.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if down is detected,
 *                   false if down is not detected.
 */
bool is_acc_down(void);

/*! \brief Test if sensor have detected 'topdown' orientation.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if topdown is detected,
 *                   false if topdown is not detected.
 */
bool is_acc_topdown(void);

/*! \brief Detect a topdown to up transition.
 *
 * This function uses an software hysteresis.
 *
 * \return bool      true if topdown to up transition is detected,
 *                   false if topdown to up transition is not detected.
 */
bool is_acc_meuh(char stop);

/*! \brief Test if absolute value of X angle has reached the specified limit
 *
 * \return bool      1 if limit has been reached in one sens,
                     -1 if  limit has been reached in the opposite sens,
 *                   0 if limit has not been reached.
 */
signed long is_acc_abs_angle_x( unsigned long abs_ang );

/*! \brief Test if absolute value of Y angle has reached the specified limit
 *
 * \return bool      1 if limit has been reached in one sens,
 *                   -1 if  limit has been reached in the opposite sens,
 *                   0 if limit has not been reached.
 */
signed long is_acc_abs_angle_y( unsigned long abs_ang );

/*! \brief Return measured value of X sensor.
 *
 * \return measured value of X sensor.
 */
unsigned long acc_get_m_x( void );

/*! \brief Return measured value of Y sensor.
 *
 * \return measured value of Y sensor.
 */
unsigned long acc_get_m_y( void );

/*! \brief Return measured value of Z sensor.
 *
 * \return measured value of Z sensor.
 */
unsigned long acc_get_m_z( void );

/**
 * \}
 */

#endif
