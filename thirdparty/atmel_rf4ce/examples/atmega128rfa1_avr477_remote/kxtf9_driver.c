#if defined(__ICCAVR__)
#include "ioavr.h"              
#include "inavr.h"
#elif defined(__GNUC__)
#include "avr/io.h"    
#include <avr/interrupt.h>
#endif

#include "twi_master.h"
#include "kxtf9_driver.h"
#include "memorymap.h"
#include "pal.h"

uint8_t  acclero_int_flag;

/**
 * @brief Initialization of accelero
 * @ingroup  RApp_API
 * Should be done before main
 *
 */
void accelero_init( void )
{
   PORTD &= ~ (0x03);
   DDRD  &= ~ (0x03);
   DDRE  |= (1<<DDE3)|(1<<DDE4);
 
 accelero_on();
   pal_timer_delay(5000);

    PORTE &= ~(1<<PE1);
    pal_timer_delay(50000);
   
   twi_master_initialise();

   accelero_write (CTRL_REG3_SRST | CTRL_REG3_OTP_6_3HZ | CTRL_REG3_OTDT_400HZ | CTRL_REG3_OWUF_50HZ, KXTF9_CTRL_REG3);

   pal_timer_delay(1000);
   accelero_write (CTRL_REG1_RES | CTRL_REG1_WUFE, KXTF9_CTRL_REG1);
   accelero_write (CTRL_REG2_FUM | CTRL_REG2_FDM | CTRL_REG2_UPM | CTRL_REG2_DOM |CTRL_REG2_RIM |CTRL_REG2_LEM, KXTF9_CTRL_REG2);

   accelero_write (0x05u, KXTF9_WUF_THRESH);
   accelero_write (DATA_CTRL_ODR_200HZ, KXTF9_DATA_CTRL_REG);
   accelero_write (TILT_TIMER_INIT, KXTF9_TILT_TIMER);
   accelero_write (WUF_TIMER_INIT, KXTF9_WUF_TIMER);
   accelero_write (CTRL_REG1_PC1 | CTRL_REG1_RES | CTRL_REG1_TPE, KXTF9_CTRL_REG1);
 
    pal_timer_delay(1000);
    pal_timer_delay(1000);
    pal_timer_delay(1000);
    pal_timer_delay(1000);
    pal_timer_delay(1000);

   accelero_read (KXTF9_INT_REL, &memory_map.body.accelero_status);
   accelero_write (0x55, KXTF9_WHO_AM_I);
   accelero_read (KXTF9_WHO_AM_I, &memory_map.body.accelero_status);
   if (memory_map.body.accelero_status != 0x55u)
   {
      while(1);
   }
   accelero_read (KXTF9_CTRL_REG3, &memory_map.body.accelero_status);
   accelero_read (KXTF9_CTRL_REG1, &memory_map.body.accelero_status);
   accelero_read (KXTF9_CTRL_REG2, &memory_map.body.accelero_status);
   accelero_read (KXTF9_INT_CTRL_REG1, &memory_map.body.accelero_status);
   accelero_read (KXTF9_WUF_THRESH, &memory_map.body.accelero_status);
   accelero_read (KXTF9_DATA_CTRL_REG, &memory_map.body.accelero_status);
   accelero_read (KXTF9_WUF_TIMER, &memory_map.body.accelero_status);
   accelero_read (KXTF9_TILT_TIMER, &memory_map.body.accelero_status);

 asm("nop");
}

/**
 * @brief read the accelero
 * @ingroup  RApp_API
 * @param address of read location
 * @param address of variable to store the read data.
 */
void accelero_read( unsigned char address, unsigned char *addr_ptr)
{
    twi_start_transceiver_with_data ( &address, 0, FALSE, address );
   while (twi_transceiver_busy() ); 
   // Check if the last operation was successful
   if ( TWI_statusReg.lastTransOK )
   {
      twi_start_transceiver_with_data ( &address, 1, TRUE, address );              
   }
   while (twi_transceiver_busy()); 
   if ( TWI_statusReg.lastTransOK )
   {
      twi_get_data_from_transceiver( addr_ptr, 1 );
   } 
   
}

/**
 * @brief read the accelero
 * @ingroup  RApp_API
 * @param data Yhe data to be written
 * @param address of register to write the data
 */
void accelero_write ( unsigned char data, unsigned char address)
{
   twi_start_transceiver_with_data ( &data, 1, FALSE, address );
} 

/**
 * @brief Accelero ISR
 * @ingroup  RApp_API
 */
ISR(INT3_vect)
{
  acclero_int_flag = 1;
}

/**
 * @brief Enables the accelero
 * @ingroup  RApp_API
 */
void accelero_on(void)
{
DDRE|=(1<<PE1);
PORTE &= ~(1<<PE1);
}

/**
 * @brief Disable the accelero
 * @ingroup  RApp_API
 */
void accelero_off(void)
{
DDRE |= (1<<PE1);
PORTE |= (1<<PE1);
}
