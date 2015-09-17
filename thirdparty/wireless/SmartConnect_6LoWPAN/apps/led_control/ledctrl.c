

#include "contiki-conf.h"
#include "port.h"
#include "delay.h"
#include "ledctrl.h"


#define LED_UDP_BLINK_COUNT  10
#define LED_UDP_BLINK__HALF_PERIOD_MS  500

struct simple_udp_connection led_udp_conn;
void ledctrl_receiver(struct simple_udp_connection *c,
                      const uip_ipaddr_t *sender_addr,
                      uint16_t sender_port,
                      const uip_ipaddr_t *receiver_addr,
                      uint16_t receiver_port,
                      const uint8_t *data,
                      uint16_t datalen);

/* Prototypes */
PROCESS(ledctrl_process, "LED Control process");

void
ledctrl_receiver(struct simple_udp_connection *c,
          const uip_ipaddr_t *sender_addr,
          uint16_t sender_port,
          const uip_ipaddr_t *receiver_addr,
          uint16_t receiver_port,
          const uint8_t *data,
          uint16_t datalen)
{
  printf("\r\n Inside led control event handler");
  uint8_t ledevt =  *data;
  // printf("Data received on port %d from port %d with length %d,Payload %lu\n\r",receiver_port, sender_port, datalen, *(uint32_t *)data);
  printf("\r\n UDP LED event received is, %d \n", ledevt);
  
  switch (ledevt) {
    
    case LED_UDP_ON: 
      printf("\r\n Led On ");
      LED_On(LED0);
      break;
      
    case  LED_UDP_OFF:
      printf("\r\n Led Off ");
      LED_Off(LED0);
      break;
      
    case LED_UDP_BLINK:
      printf("\r\n Led Blink ");
      LED_Off(LED0);
      for(int i=0;i< LED_UDP_BLINK_COUNT; i++)
      {
        LED_On(LED0);
        delay_ms(LED_UDP_BLINK__HALF_PERIOD_MS);
        LED_Off(LED0);
        delay_ms(LED_UDP_BLINK__HALF_PERIOD_MS);
      }
      break;
      
  }    
  
}


/*---------------------------------------------------------------------------*/

PROCESS_THREAD(ledctrl_process, ev, data)
{
  int status;
  
  PROCESS_BEGIN();
  status = simple_udp_register(&led_udp_conn,LED_UDP_PORT,NULL,LED_UDP_PORT,ledctrl_receiver);
  printf("Registered led listen callback for UDP port %d. Success? %d  ", LED_UDP_PORT, status);

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void ledctrl_init()
{
  process_start(&ledctrl_process,NULL);
}