
#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include "net/ip/uip.h"
#include "net/ip/simple-udp.h"

#define LED_UDP_ON        0x01
#define LED_UDP_OFF       0x02
#define LED_UDP_BLINK     0x03


void ledctrl_init(void);


#endif /* LED_CONTROL_H_ */
