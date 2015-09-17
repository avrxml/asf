/**
 * \file
 *
 * \brief Example application for the RF4CE and QMatrix integration for ATMega128RFA1 on AVR477 board.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   3. The name of ATMEL may not be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 *   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 *   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 *   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * AVR477 is a Unique reference design with true integration of RF, Touch & MEMS

 * \section Connection Connection Aspects
 * \image html FlowDiagram.jpg

 * \section Sections Sections
 * - AVR477 Remote : This page provides firmware details of the AVR477 Remote.
 * - AVR477 Target : This page provides firmware details of the AVR477 Target.
 *                   The Target application can be run on the STB-RCB128RFA1  
 *                   board or the RZ600-RF231 board.     
 *
 * \section deviceinfo Device Info
 * This application is targeted for the ATmega128RFA1 MCU on the AVR477 Touch 
 * Remote Control board.\n
 * For the Demo scenario,a Target application that sends QDebug Data,received over air,
 * to QTouch Studio is used.\nThis Target Application can be run on the ATUC3A3256S MCU 
 * RZ600 board or on the ATmega128RFA1 MCU on the Sensor Terminal Board,when used along
 * with the QT600.
 *  
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC,IAR for AVR,IAR for AVR32.\n
 * Supported Versions : AVR Studio 4,AVR Studio 5,IAR for AVR 6.10,IAR for AVR32 4.10 or higher.\n 
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/**
 * \page Rem AVR477 Remote
 
 * \section intro Introduction
 * AVR477 Remote Application showcases the capabilities of the AVR477 Touch Remote Control.

 * \section Arch Software Architecture
 * \image html SWArchitecture.jpg
 * \section HW Hardware
 *  AVR477 Remote Control Board
 * \image html Remote.jpg
 
 * \section desc Description 
 * The Remote Application transfers measured touch data over air as RF4CE packets.\n 
 * The AVR477 Remote control has periodic sleep functionality and wake-up based on Proximity Sensing implemented.\n
 * Back-lit LEDs and a Buzzer are available for indications.\n
 * A 3D Accelerometer is available for 3D position sensing.\n
 * Warm Reset functionality is implemented,which allows for retaining RF4CE Network Related 
 * parameters(Network Information Base) on the occurence of power failures.\n
 * Over-the-Air Firmware upgrade feature has been implemented.
 *
 * \defgroup RApp_API Remote Application related API's
 * \defgroup QTouch QTouch related API's
 * ZRC profile related
 * \defgroup apiRF4CONTROL_ZRC_CONSTANTS RF4CE ZRC constants
 * \defgroup apiRF4Control_ZRC_API RF4CE ZRC APIs
 * \defgroup apiRF4Control_ZRC_CMD_DISC_API RF4CE ZRC command discovery APIs
 * RF4CE Network related
 * \defgroup apiRF4CONTROL_NWK_CONSTANTS RF4CE Network constants
 * \defgroup apiRF4CONTROL_NWK_API RF4CE Network API
 * \defgroup apiRF4CONTROL_GENERIC_API RF4CE Generic API

 * QMatrix APIs
 * \defgroup QMatrix QMatrix related Configuration
 */

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
/* RF4CE */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pal.h"
#include "app_config.h"
#include "rf4ce.h"
#include "zrc.h"
#include "pb_pairing.h"
#include "vendor_data.h"
#include "compiler.h"

#if defined(__ICCAVR__)
#include "ioavr.h"              
#include "inavr.h"
#elif defined(__GNUC__)
#include "avr/io.h"    
#include <avr/interrupt.h>
#endif

#include "avrtypes.h"

#include "touch_api.h"

#ifdef _DEBUG_INTERFACE_
#include "qdebug.h"
#include "qdebug_transport.h"
#endif/* _DEBUG_INTERFACE_*/

#ifdef _PROX_
#include "t10_prox.h"
#endif /*Proximity Sensor*/

#ifdef _ACCELERO_
#include "kxtf9_driver.h"
#endif /*_ACCELERO*/

#include "memorymap.h"

/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
type definitions
----------------------------------------------------------------------------*/
/* RF4CE node states */
typedef enum node_status_tag
{
    IDLE,
    WARM_START,
    COLD_START,
    TRANSMITTING,
    RESET,

#ifdef _ENABLE_SLEEP_
    TRX_OFF_START,
    TRX_OFF_CONFIRM,
    TRX_ON_START
#endif /* _ENABLE_SLEEP_ */

} SHORTENUM node_status_t;

/*----------------------------------------------------------------------------
macros
----------------------------------------------------------------------------*/
#define QT_MEASUREMENT_PERIOD_MSEC   15u 

#ifdef _ENABLE_SLEEP_
#define AVR477_SLEEP_DURATION_IN_US ( AVR477_SLEEP_DURATION_SEC * 1000000u )
#define AVR477_ACTIVE_DURATION_IN_QT_MEASURE_PERIOD  (AVR477_ACTIVE_DURATION_SEC * 1000 / QT_MEASUREMENT_PERIOD_MSEC)
#endif /* _ENABLE_SLEEP_ */

#ifdef _ENABLE_WARM_RESET_
#define AVR477_WARM_RESET_WAIT_DURATION_IN_US ( AVR477_WARM_RESET_WAIT_TIME_SEC * 1000000u )
#endif /* _ENABLE_WARM_RESET_ */

#define ACCELERO_SIG_REF_DEFAULT (30u)

#define __delay_cycles(n)     __builtin_avr_delay_cycles(n)
#define __enable_interrupt()  sei()

/*----------------------------------------------------------------------------
prototypes
----------------------------------------------------------------------------*/
/* initialise host app, pins, watchdog, etc */
static void init_system( void );
/* configure timer ISR to fire regularly */
static void init_timer_isr( void );
/* Assign the parameters values to global configuration parameter structure */
static void qt_set_parameters( void );
/* Configure the Burst Length*/
static void burst_len_config( void );

/* Configure the buzzer */
static void buzzer_init(void);

/* LED Control */
static inline void led0_on(void);
static inline void led1_on(void);
static inline void led0_off(void);
static inline void led1_off(void);
static inline void led0_toggle(void);
static inline void led1_toggle(void);


/* qt filter callback function */ 
static void qt_avr477_filter_cb(void);

static void app_task(void);
static void extended_delay_ms(uint16_t delay_ms);
static void indicate_fault_behavior(void);

#ifdef _ENABLE_WARM_RESET_
static void start_warm_reset_cb(void *callback_parameter);
#endif /* _ENABLE_WARM_RESET_ */

#ifdef _ENABLE_SLEEP_
static void start_awake_timer_cb(void *callback_parameter);
static void sleep_led_control(void);
#endif /* _ENABLE_SLEEP_ */

#ifdef ZRC_CMD_DISCOVERY
static void start_cmd_disc_cb(void *callback_parameter);
#endif

static void buzzer_control(void);
static void config_sensors(void);




/* === GLOBALS ============================================================= */

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;

static node_status_t node_status;
static uint8_t pairing_ref = 0xFF;

#ifdef _ENABLE_SLEEP_
extern uint8_t allow_to_sleep;
static uint16_t enable_sleep_counter = 0;
#endif /* _ENABLE_SLEEP_ */

extern uint16_t  delivery;
extern uint16_t  qgSubsOnce;
extern uint16_t  qgSubsChange;
extern uint16_t  qgSubsAllways;

/* filter for channel signals */
static uint16_t filter[QT_NUM_CHANNELS][8];

/* Timer period in msec. */
uint16_t qt_measurement_period_msec = QT_MEASUREMENT_PERIOD_MSEC;

static uint16_t status_flag = 0u;
static uint16_t burst_flag = 0u;

/* flag set by timer ISR when it's time to measure touch */
static volatile uint8_t time_to_measure_touch = 0u;
/* current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

/*----------------------------------------------------------------------------
global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
extern variables
----------------------------------------------------------------------------*/
/* This configuration data structure parameters if needs to be changed will be
changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;

#ifdef _QMATRIX_
//extern y_line_info_t y_line_info[NUM_Y_LINES];
//extern x_line_info_t x_line_info[NUM_X_LINES];

extern y_line_info_t ya_line_info[NUM_Y_LINES];
extern y_line_info_t yb_line_info[NUM_Y_LINES];
extern x_line_info_t x_line_info[NUM_X_LINES];

/* Fill out the X-Line masks on the X- Port selected.
* The order of X - Line numbering follows from the way the
* X-Lines are filled as below.
* Here, X0,X1,X2,X3,X4,X5,X6,X7 are on port-pin specified.
* 1 - if to specify if X line is on PORT_X_1,pin on the same selected port.
* 2 - if to specify if X line is on PORT_X_2,pin on the same selected port.
* 3 - if to specify if X line is on PORT_X_3,pin on the same selected port.
*
* Note: 1. The Number entries should be based on NUM_X_LINES
*          4 entries when NUM_X_LINES =4 and
*          8 entries when NUM_X_LINES=8
*/

x_line_info_t x_line_info[NUM_X_LINES]= {
FILL_OUT_X_LINE_INFO(1,6),
FILL_OUT_X_LINE_INFO(2,4),
FILL_OUT_X_LINE_INFO(2,5),
FILL_OUT_X_LINE_INFO(2,6),
FILL_OUT_X_LINE_INFO(2,7)
};

/* Fill out the Y-Line masks on the Y- Line port selected
 * The order of Y - Line numbering follows from the way the
 * Y-Lines are filled as below
 * Here, Y0,Y1,Y2,Y3 on 0,1,2,3
 * Note: 1. The Number entries should be based on NUM_X_LINES
 *          2 entries when NUM_Y_LINES=2
 *          4 entries when NUM_Y_LINES=4
 *          8 entries when NUM_Y_LINES=8
*/
y_line_info_t ya_line_info[NUM_Y_LINES]= {
    FILL_OUT_YA_LINE_INFO(  0u ),
    FILL_OUT_YA_LINE_INFO(  1u ),
    FILL_OUT_YA_LINE_INFO(  2u ),
    FILL_OUT_YA_LINE_INFO(  3u ),
};
y_line_info_t yb_line_info[NUM_Y_LINES]= {
    FILL_OUT_YB_LINE_INFO(  0u ),
    FILL_OUT_YB_LINE_INFO(  1u ),
    FILL_OUT_YB_LINE_INFO(  2u ),
    FILL_OUT_YB_LINE_INFO(  3u ),
};
#endif/*_QMATRIX_*/

#ifdef _ENABLE_SLEEP_
/**
 * @brief This function controls the LED in a definitive fashion before going to sleep.
 * @ingroup RApp_API
 */

static void sleep_led_control(void)
{
    for (uint8_t l = 0; l < 3; l++)
    {
        led0_on();
        led1_off();
        extended_delay_ms(100);
        led0_off();
        led1_on();
        extended_delay_ms(100);         
    }
    led0_off();
    led1_off();
    extended_delay_ms(200);
}
#endif /* _ENABLE_SLEEP_ */

/**
 * @brief This function is called every time the qt measuremement is done.
 * This is the pointer to callback function called before processing the signal 
 * @ingroup QTouch
 *
 */
static void qt_avr477_filter_cb(void)
{    
    uint8_t i;

    /* filter channel signals - averaging 6 samples */
    for( i = 0u; i < QT_NUM_CHANNELS; i++ )
   {
    filter[i][0] = filter[i][1];
    filter[i][1] = filter[i][2];
    filter[i][2] = filter[i][3];
    filter[i][3] = filter[i][4];
    filter[i][4] = filter[i][5];
    filter[i][5] = qt_measure_data.channel_signals[i];
    qt_measure_data.channel_signals[i] = ( ( filter[i][0] +
                                             filter[i][1] +
                                             filter[i][2] +
                                             filter[i][3] +
                                             filter[i][4] +
                                             filter[i][5] ) / 6u );
    }

#ifdef _ACCELERO_

    /* Set references for accelero (dummy channels) */
    qt_measure_data.channel_references[4]  = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_references[20] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_references[24] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_references[25] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_references[26] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_references[27] = ACCELERO_SIG_REF_DEFAULT;

    /* Set signals for accelero (dummy channels) */
    qt_measure_data.channel_signals[4]  = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_signals[20] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_signals[24] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_signals[25] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_signals[26] = ACCELERO_SIG_REF_DEFAULT;
    qt_measure_data.channel_signals[27] = ACCELERO_SIG_REF_DEFAULT;
    
    switch (memory_map.body.accelero_status)
    {
        case CTRL_REG2_FUM: /* Face up state mask */
        {
            qt_measure_data.channel_signals[4] = 10;
            break;
        }
        case CTRL_REG2_FDM: /* Face down state mask */
        {
            qt_measure_data.channel_signals[20] = 10;
            break;
        }
        case CTRL_REG2_UPM: /* Up state mask */
        {
            qt_measure_data.channel_signals[24] = 10;
            break;
        }
        case CTRL_REG2_DOM: /* Down state mask */
        {
            qt_measure_data.channel_signals[25] = 10;
            break;
        }
        case CTRL_REG2_RIM: /* Right state mask */
        {
            qt_measure_data.channel_signals[26] = 10;
            break;
        }
        case CTRL_REG2_LEM: /* Left state mask */
        {
            qt_measure_data.channel_signals[27] = 10;
            break;
        }
    }
#else
    {
      return;
    }
#endif
}

/**
 * @brief Main function - Initialization of RF4CE and QTouch are done. 
 * Then app_task() and nwk_task() is called in a loop.
 * @ingroup RApp_API
 */
int main( void )
{

    if (nwk_init() != NWK_SUCCESS)
    {
        /* something went wrong during initialization */
        while (1)
        {
            indicate_fault_behavior();
        }
    }

    /*
     * The stack is initialized above,
     * hence the global interrupts are enabled here.
     */    
    pal_global_irq_enable();

    init_timer_isr();
    
    /* initialise host app, pins, watchdog, etc */
    init_system();
    
    /*Configure Burst Length*/
    burst_len_config();
    config_sensors();

    /* Initialise and set touch params */
    qt_init_sensing();
    qt_set_parameters();
    buzzer_init();

    /*  Address to pass address of user functions   */
    /*  This function is called after the library has made capacitive measurements,
    *   but before it has processed them. The user can use this hook to apply filter
    *   functions to the measured signal values.(Possibly to fix sensor layout faults)    */
    /* This function is also used to send signal values to simulate Accelero meter,
       Just for demo purpose */
    qt_filter_callback = qt_avr477_filter_cb; 

    /* Initialize debug protocol */
#ifdef _DEBUG_INTERFACE_
    QDebug_Init();
#endif/* _DEBUG_INTERFACE_*/

#ifdef _PROX_
prox_init();
#endif/* _PROX_ */

 #ifdef _ACCELERO_
 accelero_init();
#endif /*_ACCELERO_ */

    /* Node status is set to Reset to start pairing or warm reset */
    node_status = RESET;

#ifdef _ENABLE_WARM_RESET_
    /* A Timer is started. If within this timeout cold-reset is not done,
       then warm start will be performed */
    pal_timer_start(T_LED_TIMER,
                    AVR477_WARM_RESET_WAIT_DURATION_IN_US,
                    TIMEOUT_RELATIVE,
                    (FUNC_PTR)start_warm_reset_cb,
                    NULL);
#endif /* _ENABLE_WARM_RESET_ */

    while (1)
    {
        app_task(); /* Application task */
        nwk_task(); /* RF4CE network layer task */
    }
}

/**
 * @brief This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters . This is
 * used to initialize the configuration data for processing.
 * @ingroup QTouch
 */
static void qt_set_parameters( void )
{
   qt_config_data.qt_di              = DEF_QT_DI;
   qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
   qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
   qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
   qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
   qt_config_data.qt_recal_threshold = RECAL_100;  //DEF_QT_RECAL_THRESHOLD ??;
   qt_config_data.qt_pos_recal_delay = 10u;        //DEF_QT_POS_RECAL_DELAY ??;
}
/**
 * @brief configure the individual channel burst length (Based on tuning)
 *
 */
#define DEFAULT_BURST_LENGTH    20u
#define LED_KEY_BURST_LENGTH_1  32u
#define LED_KEY_BURST_LENGTH_2  32u
#define ZERO_BURST_LENGTH       0u
#define ROTOR_BURST_LENGTH_8    40u
#define ROTOR_BURST_LENGTH_9    36u
#define ROTOR_BURST_LENGTH_10   25u
#define ROTOR_BURST_LENGTH_11   32u
static void burst_len_config( void )
{
   /* Used channels */
   qt_burst_lengths[0] = LED_KEY_BURST_LENGTH_1;
   qt_burst_lengths[1] = DEFAULT_BURST_LENGTH;
   qt_burst_lengths[2] = DEFAULT_BURST_LENGTH;
   qt_burst_lengths[3] = LED_KEY_BURST_LENGTH_2;
   
   qt_burst_lengths[8] = ROTOR_BURST_LENGTH_8;
   qt_burst_lengths[9] = ROTOR_BURST_LENGTH_9;
   qt_burst_lengths[10] = ROTOR_BURST_LENGTH_10;
   qt_burst_lengths[11] = ROTOR_BURST_LENGTH_11;
   qt_burst_lengths[12] = DEFAULT_BURST_LENGTH;
   
   qt_burst_lengths[16] = DEFAULT_BURST_LENGTH;
   qt_burst_lengths[17] = DEFAULT_BURST_LENGTH;
   qt_burst_lengths[18] = DEFAULT_BURST_LENGTH;
   qt_burst_lengths[19] = DEFAULT_BURST_LENGTH;
   
   /* Unused channels */
   qt_burst_lengths[4] = ZERO_BURST_LENGTH;
   qt_burst_lengths[5] = ZERO_BURST_LENGTH;
   qt_burst_lengths[6] = ZERO_BURST_LENGTH;
   qt_burst_lengths[7] = ZERO_BURST_LENGTH;
   qt_burst_lengths[13] = ZERO_BURST_LENGTH;
   qt_burst_lengths[14] = ZERO_BURST_LENGTH;
   qt_burst_lengths[15] = ZERO_BURST_LENGTH;
   qt_burst_lengths[20] = ZERO_BURST_LENGTH;
   qt_burst_lengths[21] = ZERO_BURST_LENGTH;
   qt_burst_lengths[22] = ZERO_BURST_LENGTH;
   qt_burst_lengths[23] = ZERO_BURST_LENGTH;
   qt_burst_lengths[24] = ZERO_BURST_LENGTH;
   qt_burst_lengths[25] = ZERO_BURST_LENGTH;
   qt_burst_lengths[26] = ZERO_BURST_LENGTH;
   qt_burst_lengths[27] = ZERO_BURST_LENGTH;
   qt_burst_lengths[28] = ZERO_BURST_LENGTH; 
   qt_burst_lengths[29] = ZERO_BURST_LENGTH; 
   qt_burst_lengths[30] = ZERO_BURST_LENGTH; 
   qt_burst_lengths[31] = ZERO_BURST_LENGTH; 
}

/**
 * @brief configure timer ISR to fire regularly
 * @ingroup QTouch
 *
 */
static void init_timer_isr( void )
{
   /*  set timer compare value (how often timer ISR will fire) */
   OCR0A = ( TICKS_PER_MS * qt_measurement_period_msec);
   /*  enable timer ISR on compare A */
   TIMSK0 = 0x02u; // Caution
   /*  timer prescaler = system clock / 1024  */
   TCCR0B = 0x05u; // Caution
   /*  timer mode = CTC (count up to compare value, then reset)    */
   TCCR0A = 0x02u; // Caution
}

/**
 * @brief initialise host app, pins, watchdog, etc
 * @ingroup RApp_API
 *
 */
static void init_system( void )
{
   /* run at 8MHz */
   /*
   asm("ldi r16,0x80");
   asm("sts 0x61,r16");
   asm("ldi r16,0x00");
   asm("sts 0x61,r16");
    */
    /* disable pull-ups */
    MCUCR |= (1u << PUD);
    
    /*Configure GPIO for LED Output and buzzer */
    DDRE  |=  (1<<DDE3)|(1<<DDE4)|(1<<DDE6);
}

/**
 * @brief timer 0 compare ISR
 * @ingroup RApp_API
 *
 */
ISR(TIMER0_COMPA_vect)
{
   /* set flag: it's time to measure touch */
   time_to_measure_touch = 1u;
   /* update the current time */
   current_time_ms_touch += qt_measurement_period_msec;
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-RF4CE-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

 /**
 * @brief  The NLME-RESET.confirm primitive allows the NLME to notify the application of
 * the status of its request to reset the NWK layer.
 *
 * @param Status status of the nwk reset
 */

void nlme_reset_confirm(nwk_enum_t Status)
{

  uint8_t IEEE_Address[8] = {0xbc, 0xbe};
    nlme_set_request(nwkPrivateIeeeAddr, 0, &IEEE_Address[0]);

    if (Status != NWK_SUCCESS)
    {
        while (1)
        {
            // endless while loop!
            indicate_fault_behavior();
        }
    }

    if (node_status == COLD_START)
    {
        pairing_ref = 0xFF;
        nlme_start_request();
    }
    else    // warm start
    {
        pairing_ref = 0;
        node_status = IDLE;
        /* Set power save mode */
        //nlme_rx_enable_request(nwkcMinActivePeriod);
    }
}


/**
 * @brief  The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 *
 * @param Status status of the nwk start
 */
void nlme_start_confirm(nwk_enum_t Status)
{
    if (Status != NWK_SUCCESS)
    {
        while (1)
        {
            indicate_fault_behavior();
        }
    }

    dev_type_t OrgDevTypeList[1];;
    profile_id_t OrgProfileIdList[1];
    profile_id_t DiscProfileIdList[1];

    OrgDevTypeList[0] = DEV_TYPE_REMOTE_CONTROL;
    OrgProfileIdList[0] = PROFILE_ID_ZRC;
    DiscProfileIdList[0] = PROFILE_ID_ZRC;

    pbp_org_pair_request(APP_CAPABILITIES, OrgDevTypeList, OrgProfileIdList,
                         DEV_TYPE_WILDCARD, NUM_SUPPORTED_PROFILES, DiscProfileIdList);
}


/**
 * @brief Push button pairing confirm; target and controller use
 *
 * The push button pairing confirm is a callback that provides the status of the
 * push pairing request.
 *
 * @param Status        Status of the last pairing transaction
 * @param PairingRef    If pairing was successful, PairingRef contains assigned
 *                      pairing reference.
 */
 void pbp_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
    if (Status != NWK_SUCCESS)
    {
        while(1)
        {
            indicate_fault_behavior();
        }
    }

    pairing_ref = PairingRef;

#ifdef ZRC_CMD_DISCOVERY
    /* Start timer to send the cmd discovery request */
    pal_timer_start(T_LED_TIMER,
                    aplcMinTargetBlackoutPeriod_us,
                    TIMEOUT_RELATIVE,
                    (FUNC_PTR)start_cmd_disc_cb,
                    NULL);
#else
    /* Set power save mode */
    //nlme_rx_enable_request(nwkcMinActivePeriod);
#endif
}

 /**
 * @brief Callback funtion indication that the timer expired for sending the command
 * discovery request.
 * @param callback_parameter    Callback parameter (not used)
 */
#ifdef ZRC_CMD_DISCOVERY
static void start_cmd_disc_cb(void *callback_parameter)
{
    zrc_cmd_disc_request(pairing_ref);

    /* Keep compiler happy */
    UNUSED(callback_parameter);
}
#endif

 /**
 * @brief The command discovery confirm callback provides information about the command
 * discovery reqest.
 *
 * @param Status        Status of the cmd discovery.
 * @param PairingRef    The reference number to paired device in pairing table.
 * @param SupportedCmd  Commands supported on Target.
 */

 
 /**
 * @brief The command discovery confirm callback provides information about the command
 * discovery reqest.
 *
 * @param Status        Status of the cmd discovery.
 * @param PairingRef    The reference number to paired device in pairing table.
 * @param SupportedCmd  Commands supported on Target.
 */
#ifdef ZRC_CMD_DISCOVERY
void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef, uint8_t *SupportedCmd)
{
    /* Enable transceiver Power Save Mode */
    //nlme_rx_enable_request(nwkcMinActivePeriod);

    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
    UNUSED(SupportedCmd);

/* Process commands from PC */
#ifdef _DEBUG_INTERFACE_
    // QDebug_ProcessCommands();
#endif/* _DEBUG_INTERFACE_*/

#if 0   
    // uint8_t sreg = SREG;                
    // cli();                              
    // CLKPR = 0x80;   /* set CLKPCE */    
    // CLKPR = 0;                          
    // SREG = sreg; 
    // asm("nop");
    // asm("nop");  
    // asm("nop");
    // asm("nop");
     // __enable_interrupt();
#endif
                
    node_status = IDLE;
}
#endif


/**
 * @brief The command discovery indication callback indicates that a command discovery
 * request command was receiced.
 *
 * @param PairingRef    The reference number to paired device in pairing table.
 */

#ifdef ZRC_CMD_DISCOVERY
void zrc_cmd_disc_indication(uint8_t PairingRef)
{
    /* Send back the response */
    uint8_t cec_cmds[32];
    PGM_READ_BLOCK(cec_cmds, supported_cec_cmds, 32);
    zrc_cmd_disc_response(PairingRef, cec_cmds);
}
#endif

 /**
 * @brief The NLME-RX-ENABLE.confirm primitive reports the results of the attempt to
 * enable or disable the receiver.
 *
 * @param Status    Status of the receiver switching.
 */ 
void nlme_rx_enable_confirm(nwk_enum_t Status)
{
    if (Status != NWK_SUCCESS)
    {
        while(1)
        {
            indicate_fault_behavior();
        }
    }

    if (node_status == COLD_START)
    {
        node_status = IDLE;

    }
    else if (node_status == WARM_START)
    {
        node_status = IDLE;

    }

#ifdef _ENABLE_SLEEP_
    if (node_status == TRX_OFF_START)
    {
        node_status = TRX_OFF_CONFIRM; 
    }
    if(node_status == TRX_ON_START)
    {
        node_status = IDLE;
    }
#endif /* _ENABLE_SLEEP_ */    
}

 /**
 * @brief Buzzer initialization
 * @ingroup RApp_API
 */ 
 static void buzzer_init(void)
 {
    DDRE|=(1<<DDE6);
    PORTE &= ~(1<<PORTE6);
 }
 
 /**
 * @brief Buzzer Control. Delay is introduced to make it audible 
 * @ingroup RApp_API
 */ 
static void buzzer_control(void)
{
    uint8_t i =  150u;

       do{
        PORTE  ^= (1<<PORTE6);
        for(int j =0;j<255;j++) asm("nop");
           i--;
       }while (i>0);
    PORTE &= ~(1<<PORTE6);
}


/**
 * @brief Application task and state machine
 * @ingroup RApp_API
 */
static void app_task(void)
{
    uint8_t temp_status1;
    uint8_t temp_status2;

#ifdef _PROX_
    static uint16_t count = 0x1ffu;
#endif /*_PROX_*/
   
    /* Port B is used for X lines. But RF4CE is overwriting whole
                     DDRB, instead of bits it used */
    DDRB |= 0xf0; 

    switch (node_status)
    {
        case IDLE:
        {
            /* If any data to Transmit, send it first. 
               Data to be send will be updated in the delivery variable by the 
               QDebug_SendData() function in QDebug.c 
               Bit-fields of Delivery is checked to see the type of data to be
               Transmitted.            
            */
            if (delivery & (1u<<SUBS_SIGN_ON))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_SIGN_ON);
                Transmit_Sign_On();
                return;
            }
        
            if (delivery & (1u<<SUBS_GLOBAL_CONFIG))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_GLOBAL_CONFIG);
                Transmit_Global_Config();
                return;
            }
        
            if (delivery & (1u<<SUBS_SENSOR_CONFIG))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_SENSOR_CONFIG);
                Transmit_Sensor_Config();
                return;
            }
#ifdef _GENERIC_QDEBUG_            
            if (delivery & (1u<<SUBS_SIGNALS))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_SIGNALS);
                Transmit_Signals();
                return;
            }
            
            if (delivery & (1u<<SUBS_REF))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_REF);
                Transmit_Ref();
                return;
            }
            
            if (delivery & (1u<<SUBS_DELTA))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_DELTA);
                Transmit_Delta();
                return;
            }
            
            if (delivery & (1u<<SUBS_STATES))
            {
                node_status = TRANSMITTING;
                delivery &= ~(1u << SUBS_STATES);
                Transmit_State();
                return;
            }
#endif  /* _GENERIC_QDEBUG_ */              
   
            /* After sending the data, do Touch Measurements */
            if(( time_to_measure_touch ) || (burst_flag))         
            {
                time_to_measure_touch = 0u;
                
                /* Read Tilt position from accelero meter */
#ifdef _ACCELERO_
                accelero_read (KXTF9_INT_SRC_REG2, &memory_map.body.accelero_status);
                accelero_read (KXTF9_INT_REL, &memory_map.body.accelero_status);
                accelero_read (KXTF9_TILT_POS_CUR, &memory_map.body.accelero_status);
#endif /*_ACCELERO_ */

                /* Measure touch once */
                status_flag = qt_measure_sensors(current_time_ms_touch );
                burst_flag = status_flag & QTLIB_BURST_AGAIN;

                /* check whether the sensor status is changed from previous status */
                temp_status1 = memory_map.body.sensor_status[0] ^ qt_measure_data.qt_touch_status.sensor_states[0];
                temp_status2 = memory_map.body.sensor_status[1] ^ qt_measure_data.qt_touch_status.sensor_states[1];
                temp_status2 &= 0x03; 
                
                /* If Sensor status is changed, do not sleep, but buzz  */
                if(temp_status1|temp_status2)
                {
#ifdef _ENABLE_SLEEP_
                    enable_sleep_counter=0;
                    buzzer_control();
#endif /* _ENABLE_SLEEP_ */                    
                    

                    /* Store back the current sensor status */ 
                    temp_status1 = 0;
                    temp_status2 = 0;
                    memory_map.body.sensor_status[0] = qt_measure_data.qt_touch_status.sensor_states[0];
                    memory_map.body.sensor_status[1] = qt_measure_data.qt_touch_status.sensor_states[1];
                    memory_map.body.wheel_position = qt_measure_data.qt_touch_status.rotor_slider_values[0];
                
#ifdef _PROX_
                    if((PIND & (1 << PIND2)) && prox_int_flag) count--;
                    if(count == 0 )
                    {
                        count = 0x10u;
                        PORTE &= ~((1<<PE3)|(1<<PE4));
                        prox_int_flag = 0;
                    }
#endif  /* _PROX_ */
                } 

#ifdef _ENABLE_SLEEP_
                else  if (allow_to_sleep)
                {
                    enable_sleep_counter++;
                    if(enable_sleep_counter > AVR477_ACTIVE_DURATION_IN_QT_MEASURE_PERIOD) 
                    {
                        node_status = TRX_OFF_START;
                        nlme_rx_enable_request(RX_DURATION_OFF);
                        return;
                    }
                }
                else
                {
                    enable_sleep_counter = 0;
                }
#endif /* _ENABLE_SLEEP_ */                
                

                /* send debug data */ 
                #ifdef _DEBUG_INTERFACE_
                    QDebug_SendData(status_flag);
                #endif/* _DEBUG_INTERFACE_*/

                /* Time-critical host application code goes here */
            } 
        /* Non time-critical application code goes here */
        break;
        }

#ifdef _ENABLE_SLEEP_
        
        case TRX_OFF_CONFIRM:
        {
            if (nwk_stack_idle())
            {
                prox_on(); 
                enable_sleep_counter = 0;
                                    
                pal_timer_start(T_LED_TIMER,
                                AVR477_SLEEP_DURATION_IN_US,
                                TIMEOUT_RELATIVE,
                                (FUNC_PTR)start_awake_timer_cb,
                                NULL);
                delivery = 0;
                accelero_off();     
                sleep_led_control();
  
#if defined( __GNUC__ ) 
                pal_sleep_mode(SLEEP_MODE_PWR_SAVE);
#elif defined ( __ICCAVR__ )
                /* Clear interrupt flag */
                EIFR = (1 << INTF5);
                /* Enable external interrupt */
                EIMSK |= (1 << INT5);
                WDT_DISABLE();
                SET_SLEEP_MODE(SLEEP_MODE_PWR_SAVE);
                SLEEP_MODE();
                WDT_RESET();
                WDT_SYSTEM_RESET_MODE(WDT_TO_8S);
#endif
                accelero_init();
                prox_off();
                node_status = TRX_ON_START;
                nlme_rx_enable_request(RX_DURATION_INFINITY);
            }
            break;
        }
#endif /* _ENABLE_SLEEP_ */

        case RESET:
        {
#ifdef _ACCELERO_
            uint8_t accelero_tilt_status;
            accelero_read (KXTF9_TILT_POS_CUR, &accelero_tilt_status);
            /* If Accelero is in Vertical Position, then do the cold reset */
            if (accelero_tilt_status == CTRL_REG2_UPM || accelero_tilt_status == CTRL_REG2_DOM)
#endif /*_ACCELERO_ */
            {
                /* Cold start on power up without button press*/
                pal_timer_stop(T_LED_TIMER);
                node_status = COLD_START;
                nlme_reset_request(true);
                buzzer_control();
            }                  
        break; 
        }

        default:
            break;
    } 
} /* app_task*/



/**
 * @brief ZRC command confirm
 *
 * The ZRC comand confirm callback provides information about the status of the
 * last command request/transmission.
 *
 * @param Status        NWK_SUCCESS if transmission was successful
 * @param PairingRef    Pairing reference
 * @param RcCmd         Sent RC command
 */
void zrc_cmd_confirm(nwk_enum_t Status, uint8_t PairingRef, cec_code_t RcCmd)
{
    node_status = IDLE;
    TX_index = 4;
    
    if (Status != NWK_SUCCESS)
    {
        led1_toggle();
    }
    /* Keep compiler happy. */
    UNUSED(PairingRef);
    UNUSED(RcCmd);
}



/**
 * @brief Vendor data confirm.
 *
 * @param Status       Status of the vendor-data-request
 * @param PairingRef   The pairing reference
 */
#ifndef RF4CE_TARGET
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
    node_status = IDLE;
    TX_index = 4;
    
    if (Status != NWK_SUCCESS)
    {
        led1_toggle();
    }
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
}
#endif

/**
 * @brief The NLDE-DATA.confirm primitive is generated by the NWK layer entity in
 * response to an NLDE-DATA.request primitive.
 *
 * @param Status        NWK_SUCCESS if transmission was successful
 * @param PairingRef    Pairing reference
 * @param ProfileId     Profile Id
 */
void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t ProfileId)
{
    node_status = IDLE;

    if (Status != NWK_SUCCESS)
    {
        indicate_fault_behavior();
    }

    /* Keep compiler happy. */
    UNUSED(PairingRef);
    UNUSED(ProfileId);
}



/* --- Helper functions ---------------------------------------------------- */


/**
 * @brief Extended blocking delay
 * @ingroup RApp_API
 * @param delay_ms Delay value in ms
 */
static void extended_delay_ms(uint16_t delay_ms)
{
    uint16_t i;
    uint16_t timer_delay;

    timer_delay = delay_ms / 50;
    for (i = 0; i < timer_delay; i++)
    {
        pal_timer_delay(50000);
    }
}
 

/**
 * @brief Indicating malfunction
 * @ingroup RApp_API
 */
static void indicate_fault_behavior(void)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        led0_on();
        led1_on();
        extended_delay_ms(50);
        led0_off();
        led1_off();
        extended_delay_ms(50);
    }
    led1_off();
    led0_off();
}

/**
 * @brief led0 on
 * @ingroup RApp_API
 */
static inline void led0_on(void)
{
    PORTE|=(1<<PE3);
}

/**
 * @brief led1 on
 * @ingroup RApp_API
 */
static inline void led1_on(void)
{
    PORTE|=(1<<PE4);
}

/**
 * @brief  led0 off
 * @ingroup RApp_API
 */
static inline void led0_off(void)
{
    PORTE&=~(1<<PE3);
}

/**
 * @brief  led1 off
 * @ingroup RApp_API
 */
static inline void led1_off(void)
{
    PORTE&=~(1<<PE4);
}

/**
 * @brief led0 Toggle
 * @ingroup RApp_API
 */
static inline void led0_toggle(void)
{
    PORTE ^= (1<<PE3);
}

/**
 * @brief Led1 Toggle
 * @ingroup RApp_API
 */
static inline void led1_toggle(void)
{
    PORTE ^= (1<<PE4);
}

/**
 * @brief Vendor-spefic callback; handles reaction to incoming alive request
 */
void vendor_app_alive_req(void)
{
/* Variant to demonstrate FOTA featue */
#if 1

#else

#endif
}
/* --- Unused functions ---------------------------------------------------- */

/**
 * @brief The NLME-SET.confirm primitive allows the NLME to notify the application of the
 * status of its request to change the value of a NIB attribute.
 *
 * @param Status            NWK_SUCCESS if value is set correctly
 * @param NIBAttribute      Attribute Id
 * @param NIBAttributeIndex Attribute Index
 */
void nlme_set_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(NIBAttribute);
    UNUSED(NIBAttributeIndex);
}

/**
 * @brief  The NLME-GET.confirm primitive allows the NLME to notify the application of the
 * status of its request for the value of a NIB attribute.
 *
 * @param Status            NWK_SUCCESS if value is get correctly
 * @param NIBAttribute      Attribute Id
 * @param NIBAttributeValue Pointer where value of attribute is stored.
 * 
 */
void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
                      uint8_t NIBAttributeIndex, void *NIBAttributeValue)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(NIBAttribute);
    UNUSED(NIBAttributeIndex);
    UNUSED(NIBAttributeValue);
}

/**
 * @brief The NLME-UNPAIR.indication primitive allows the NLME to notify the application
 * of the removal of a pairing link by another device.
 *
 * @param PairingRef  Pairing Reference of the device to be unpaired
 */
void nlme_unpair_indication(uint8_t PairingRef)
{
    /* Keep compiler happy */
    UNUSED(PairingRef);
}

/**
 * @brief The NLME-UNPAIR.confirm primitive allows the NLME to notify the application of
 * the status of its request to remove a pair with another device.
 *
 * @param Status      Staus of unpair confirm
 * @param PairingRef  Pairing Reference of the device to be unpaired
 */
void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
}

/**
 * @brief The NLME-UPDATE-KEY.confirm primitive allows the NLME to notify the
 * application of the status of its request to change the security link key of
 * a pairing table entry.
 *
 * @param Status      Staus of unpair confirm
 * @param PairingRef  Pairing Reference of the device
 */
void nlme_update_key_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
    /* Keep compiler happy */
    UNUSED(Status);
    UNUSED(PairingRef);
}

/**
 * @brief The callback function for the warm-reset timeout
 * @ingroup RApp_API
 * @param callback_parameter      Callback parameter (unused)
 */
#ifdef _ENABLE_WARM_RESET_
static void start_warm_reset_cb(void *callback_parameter)
{
    /* Warm start */
    node_status = WARM_START;
    pal_ps_get(INTERN_EEPROM, EE_QDEBUG_OFFSET, 2, &qgSubsOnce);
    pal_ps_get(INTERN_EEPROM, EE_QDEBUG_OFFSET + 2, 2, &qgSubsChange);
    pal_ps_get(INTERN_EEPROM, EE_QDEBUG_OFFSET + 4, 2, &qgSubsAllways);

#ifdef _ENABLE_SLEEP_   
    pal_ps_get(INTERN_EEPROM, EE_QDEBUG_OFFSET + 6, 2, &allow_to_sleep);
#endif /* _ENABLE_SLEEP_ */
    
    nlme_reset_request(false);

    buzzer_control();

    /* Keep compiler happy */
    UNUSED(callback_parameter);
}
#endif /* _ENABLE_WARM_RESET_ */


/**
 * @brief The callback function for the awake timer after the sleep period is over.
 * @ingroup RApp_API
 * @param callback_parameter      Callback parameter (unused)
 */
#ifdef _ENABLE_SLEEP_
static void start_awake_timer_cb(void *callback_parameter)
{
    /* Keep compiler happy */
    UNUSED(callback_parameter);
}
#endif /* _ENABLE_SLEEP_ */



/**
 * @brief  Configure the sensors, assign the channel number, aks_group, hysteresis.
 * @ingroup QTouch
 */
void config_sensors(void)
{
    qt_enable_key(CHANNEL_19, AKS_GROUP_1, 20u, HYST_6_25 );   /* Sensor 0 */
    qt_enable_key(CHANNEL_3, AKS_GROUP_1, 33u, HYST_6_25 );    /* Sensor 1 */
    qt_enable_key(CHANNEL_0, AKS_GROUP_1, 28u, HYST_6_25 );    /* Sensor 2 */
    qt_enable_key(CHANNEL_17, AKS_GROUP_1, 22u, HYST_6_25 );   /* Sensor 3 */
    qt_enable_key(CHANNEL_18, AKS_GROUP_1, 20u, HYST_6_25 );   /* Sensor 4 */
    qt_enable_key(CHANNEL_2, AKS_GROUP_1, 23u, HYST_6_25 );    /* Sensor 5 */
    qt_enable_key(CHANNEL_1, AKS_GROUP_1, 22u, HYST_6_25 );    /* Sensor 6 */
    qt_enable_key(CHANNEL_16, AKS_GROUP_1, 16u, HYST_6_25 );   /* Sensor 7 */
    qt_enable_rotor(CHANNEL_8, CHANNEL_11, AKS_GROUP_1, 55u, HYST_25, RES_8_BIT, 3u ); /* Sensor 8 */
    qt_enable_key(CHANNEL_12, AKS_GROUP_1, 20u, HYST_6_25 );   /* Sensor 9 */

#ifdef _ACCELERO_
#ifdef _DEBUG_INTERFACE_ 
   /* Enable 6 dummy keys to simulate Accelero meter, just for demo purpose */
    qt_enable_key(CHANNEL_4,  AKS_GROUP_2,10u,HYST_6_25);  /* Accelero FU */
    qt_enable_key(CHANNEL_20, AKS_GROUP_2,10u,HYST_6_25); /* Accelero FD */
    qt_enable_key(CHANNEL_24, AKS_GROUP_2,10u,HYST_6_25); /* Accelero UP */
    qt_enable_key(CHANNEL_25, AKS_GROUP_2,10u,HYST_6_25); /* Accelero DO */
    qt_enable_key(CHANNEL_26, AKS_GROUP_2,10u,HYST_6_25); /* Accelero RI */
    qt_enable_key(CHANNEL_27, AKS_GROUP_2,10u,HYST_6_25); /* Accelero LE */
#endif/* _DEBUG_INTERFACE_*/
#endif /*_ACCELERO_ */
}

/* EOF */
