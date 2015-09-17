
/**
* Copyright (c) 2015 Atmel Corporation and 2012 – 2013, Thingsquare, http://www.thingsquare.com/. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of Atmel nor the name of Thingsquare nor the names of its contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
* Atmel microcontroller or Atmel wireless product.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*
*/

/**
 * \mainpage
 * \section preface Preface
 * Proximetry example application uses Proximetry agent modules to communicate to Atmel Cloud via
 * a border router.
 * This example,
 * 1.	Will publish data to the MQTT host broker configured with configured topic. 
 * 2.	Uses ADC, to get Temperature ,Supply Voltage and Analog Input Voltage. 
 * 3.   Also uses LED0 for ON , OFF and BLINK configurations.
 * 4.   Uses SW0 for Key state and Diagnostics     
 * 3.	Uses Border router to connect outside the 6LoWPAN network.
 * 	
 *      

 */

#include <string.h>
#include "thsq.h"
#include "netstack-aes.h"
#include "dev/button-sensor.h"
#include "rpl/rpl-private.h"
#include "port.h"
#include "asf.h"
#include "adc.h"

#include "common.h"
#include "prox_agent.h"
#include "misc.h"

#include "conf_agent_app.h"
#include "settings.h"
#include "diagnostic_button.h"

static struct etimer reconnect_timer;

static process_event_t reconnect_event;
static uint8_t reconnecting = 0;


/*---------------------------------------------------------------------------*/
/*  end node thread															 */
/*---------------------------------------------------------------------------*/

PROCESS(end_node_process, "End_Node");

PROCESS_THREAD(end_node_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;
  etimer_set(&et, CLOCK_SECOND);

  printf("Using NodeId: %X\n\r", get_nodeid());

  /* Turn off AES */
  netstack_aes_set_active(1);

  /* Allocate events */
  reconnect_event = process_alloc_event();

  /* Reconnect from here */

  while(1) {
      if(simple_rpl_parent() == NULL) {

      printf("\r\n Connecting to a Wireless Network...\r\n");
      etimer_set(&et, CLOCK_SECOND / 1);
      while(simple_rpl_parent() == NULL) {
		PROCESS_YIELD_UNTIL(etimer_expired(&et));
        etimer_reset(&et);
      }
      printf("\r\n Connected to Wireless network\r\n");
    }

	process_start(&prox_agent_process, NULL);

    /* Reset reconnecting flag */
    reconnecting = 0;

    /* Main loop */
    while(1) {

      PROCESS_WAIT_EVENT();
      if(ev == reconnect_event) {
        reconnecting = 1;
        etimer_set(&reconnect_timer, CLOCK_SECOND*10);
      }
      if(reconnecting &&
         etimer_expired(&reconnect_timer)) {
        break;
      }
    }
  }
  PROCESS_END();
}



/*---------------------------------------------------------------------------*/
/*  Led thread                                                               */
/*---------------------------------------------------------------------------*/

static struct timer led_timer;

/**
 * @brief Sets the desired LED behaviour
 */
static void set_led_state(int state)
{
	port_pin_set_output_level(LED_0_PIN, state == LED_ON ? LED_0_ACTIVE : LED_0_INACTIVE);
}


static void led_toggle(void)
{
	port_pin_toggle_output_level(LED_0_PIN);
}

PROCESS(led_process, "led");



/**
 * @brief Sets the desired LED behaviour
 *
 *  Led thread handle two functionalities:
 *  - the led state,
 *  - diagnostic button.
 *  The led state service is driving from Proximetry agent. The led can be set to one of three state ON,OFF,BLINKING.
 *  The diagnostic button service allows to determine the end node connection state without console inspection.
 *  The diagnostic button distinguish three states:
 *	- end node disconnected from BR							- led blinking with 5Hz frequency for 5s,
 *	- end node connected with BR only						- led blinking with 2Hz frequency for 5s ,
 *	- end node agent connected to Cloud Serve	 			- led is On for 5s.
 */
PROCESS_THREAD(led_process, ev, data)
{

	UNUSED(data);
	UNUSED(ev);
	PROCESS_BEGIN();

	timer_set(&led_timer, LED_TOGGLE_INTERVAL);

	while(1)
	{
		diagnostic_button_service();

		if (diagnostic_button_service_is_active())
		{
			diagnostic_button_led_service();
		}
		else
		{
			if (!timer_expired(&led_timer))
				goto end;

			timer_restart(&led_timer);
			switch (get_settings()->led0_state)
			{
				case LED_OFF:
					set_led_state(LED_OFF);
					break;
				case LED_ON:
					set_led_state(LED_ON);
					break;
				case LED_BLINK:
					led_toggle();
					break;
				default:
					;
			}
		}
end:
		PROCESS_PAUSE();
	}

	PROCESS_END();
}

AUTOSTART_PROCESSES(&end_node_process,&led_process);

/*---------------------------------------------------------------------------*/
