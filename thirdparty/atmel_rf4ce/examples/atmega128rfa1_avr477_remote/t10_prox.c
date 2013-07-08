#if defined(__ICCAVR__)
#include "ioavr.h"              
#include "inavr.h"
#elif defined(__GNUC__)
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include "t10_prox.h"
#include "memorymap.h"
#include "pal.h"

uint8_t  int_flag;

uint8_t  prox_int_flag;
 

/**
 * @brief Initialization of Prox
 * @ingroup  RApp_API
 * Should be done before main
 *
 */
void prox_init( void )
{
  prox_off(); 
}

/**
 * @brief Prox ISR
 * @ingroup  RApp_API
 */
//#pragma vector=INT2_vect
//__interrupt void int2_isr(void)
ISR(INT2_vect)
{
    if(PIND & (1 << PIND2))
    {
        prox_int_flag = 0x01u;
        PORTE |=  ((1<<PORTE3) |(1<<PORTE4));
    }
    else
    {
        prox_int_flag = 0x00u;
        PORTE &= ~((1<<PORTE3) |(1<<PORTE4));
    }
}
/**
 * @brief Enable the prox
 * @ingroup  RApp_API
 */
void prox_on(void)
{
    DDRE |= (1<<PE0);
    PORTE &= ~(1<<PE0);
    /* Set External Interrupt Control Register (A) and External Interrupt Mask Register */
    EICRA |= (1 << ISC20); 
    EIMSK |= 1<<INT2;
}

/**
 * @brief Disable the prox
 * @ingroup  RApp_API
 */
void prox_off(void)
{
    DDRE |= (1<<PE0);
    PORTE |= (1<<PE0);
    /* Disable LEDs */
    PORTE&=~(1<<PE3);
    PORTE&=~(1<<PE4);
    /* Set External Interrupt Control Register (A) and External Interrupt Mask Register */
    EICRA &= ~(1 << ISC20); 
    EIMSK &= ~(1<<INT2);
}
