
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
 * MQTT example application uses MQTT and MDNS modules to communicate to MQTT host configured 
 * and establish a MQTT connection.
 * This example,
 * 1.	Will publish data to the MQTT host broker configured with configured topic. 
 * 2.	Uses IO1_xpro board, to get Temperature and Light sensor details of the current 
 *      environment and send them to MQTT broker in JSON format.
 * 3.	Uses Border router to connect outside the 6LoWPAN network.
 * 4.	Uses MDNS module, to get IP address of MQTT host broker configured. 
 *      MDNS server used here, is Google DNS server (::ffff:8.8.8.8).

 */
#include <string.h>
#include "thsq.h"
#include "netstack-aes.h"
#include "dev/button-sensor.h"
#include "rpl/rpl-private.h"
#include "port.h"
#include "asf.h"
#include "mqtt.h"
#include "battery-sensor.h"
#include "sam0_sensors.h"
#define SEND_INTERVAL		(60 * CLOCK_SECOND)
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static struct mqtt_connection conn;
static struct mqtt_message* msg_ptr = 0;
static struct etimer sensor_timer;
static struct etimer reconnect_timer;
//static uint16_t random_topic;
static char clientid[16];
static char str_topic_state[30];
static char str_topic_sensor[30];
static char str_topic_led[30];
static char app_buffer[128];


static char batt_str[8];
static const struct sensors_sensor *battery = NULL;
static char temp_str[8];
static char mac_adr_str[18];
static uint16_t nodeid;

char str[48];
volatile float temp_C;
float temp_F;

volatile double temp_res;

static uint16_t button_sensor_value=0;
static process_event_t led_updates_changed_event;
static process_event_t reconnect_event;
static uint8_t reconnecting = 0;
static uip_ipaddr_t google_ipv4_dns_server = {
    .u8 = {
      /* Google's IPv4 DNS in mapped in an IPv6 address (::ffff:8.8.8.8) */
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xff, 0xff,
      0x08, 0x08, 0x08, 0x08,
    }
};


/* MQTT Configuration details */
#define HOST          "m2m.eclipse.org"
#define VERSION	      "v1"
#define PRIORITY      "p0"
#define UUID	        "atmeld"




/*---------------------------------------------------------------------------*/

PROCESS(mqtt_example_process, "MQTT Example");
AUTOSTART_PROCESSES(&mqtt_example_process);




/*---------------------------------------------------------------------------*/
static void
mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void* data)
{
  uint16_t i;

  switch(event) {
    case MQTT_EVENT_CONNECTED: {
      printf("\r\n APP - Application has a MQTT connection\n");
      break;
    }
    case MQTT_EVENT_DISCONNECTED: {
      printf("\r\n APP - Disconnected from MQTT broker\n");
      break;
      }
    case MQTT_EVENT_PUBLISH: {
      msg_ptr = data;
      /* New led value */
      if(strcmp(msg_ptr->topic, str_topic_led) == 0) {
        msg_ptr->payload_chunk[msg_ptr->payload_length] = 0;

        if(strcmp((char *)(msg_ptr->payload_chunk), "on") == 0) {
		  printf("\r\n LED ON \n");
         //leds_on(LEDS_RED);
		 port_pin_set_output_level(PIN_PA22, true);
		 port_pin_set_output_level(PIN_PA23, true);
		 port_pin_set_output_level(PIN_PA13, true);
		 port_pin_set_output_level(PIN_PA19,false);
        }
        if(strcmp((char *)(msg_ptr->payload_chunk), "off") == 0) {
		  printf("\r\n LED OFF \n");
		  port_pin_set_output_level(PIN_PA22, false);
		  port_pin_set_output_level(PIN_PA23, false);
		  port_pin_set_output_level(PIN_PA13, false);
		  port_pin_set_output_level(PIN_PA19,true);
          //leds_off(LEDS_RED);
        }
      }

      /* Implement first_flag in publish message? */
      if(msg_ptr->first_chunk) {
        msg_ptr->first_chunk = 0;
        printf("\r\n APP - Application received a publish on topic '%s'. Payload "
               "size is %i bytes. Content:\n\n", msg_ptr->topic, msg_ptr->payload_length);
      }

      for(i = 0; i < msg_ptr->payload_chunk_length; i++) {
        printf("%c", msg_ptr->payload_chunk[i]);
      }
      if(msg_ptr->payload_left == 0) {
        printf("\n");
        printf("\n");
        printf("\r\n APP - Application received publish content succefully.\n");
      }
      break;
    }
    case MQTT_EVENT_SUBACK: {
      printf("\r\n APP - Application is subscribed to topic successfully\n");
      break;
    }
    case MQTT_EVENT_UNSUBACK: {
      printf("\r\n APP - Application is unsubscribed to topic successfully\n");
      break;
    }
    case MQTT_EVENT_PUBACK: {
      printf("\r\n APP - Publishing complete.\n");
      break;
    }
    default:
      printf("\r\n APP - Application got a unhandled MQTT event: %i\n", event);
      break;
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mqtt_example_process, ev, data)
{
    PROCESS_BEGIN();
	static unsigned long  timestmp;
	volatile uip_ds6_addr_t *lladdr;
	volatile uint16_t battery_v;
	volatile sensor_result_t temp;





  timestmp = clock_time();

  

  lladdr = uip_ds6_get_link_local(-1);
  sprintf(mac_adr_str,"%02x%02x%02x%02x%02x%02x%02x%02x",lladdr ->ipaddr.u8[8],lladdr->ipaddr.u8[9],lladdr->ipaddr.u8[10],lladdr->ipaddr.u8[11],lladdr->ipaddr.u8[12],lladdr->ipaddr.u8[13],lladdr->ipaddr.u8[14],lladdr->ipaddr.u8[15]);

  nodeid = lladdr->ipaddr.u8[14];
  nodeid <<= 8;
  nodeid |= lladdr->ipaddr.u8[15];
  printf("Using NodeId: %X\n\r", nodeid);
  
  sprintf(str_topic_state, "/%s/%s/%s/%x%s",VERSION,PRIORITY,UUID,nodeid,"/status");
  sprintf(str_topic_sensor, "/%s/%s/%s/%x%s",VERSION,PRIORITY,UUID,nodeid,"/sensor");
  printf("\r\n%s\n",str_topic_sensor);
  sprintf(str_topic_led, "/%s/%s/%s/%x%s",VERSION,PRIORITY,UUID,nodeid,"/led");
  
  mdns_init();
  mdns_conf(&google_ipv4_dns_server);

  /* Turn off AES */
  netstack_aes_set_active(1);

  /* Allocate events */
  led_updates_changed_event = process_alloc_event();
  reconnect_event = process_alloc_event();
  /* At the moment it is up to the user to provide the underlying input and
   * output buffer.
   */
  etimer_set(&sensor_timer, CLOCK_SECOND*5);
  sprintf(clientid, "/%s/%x",UUID,nodeid);
  printf("\r\nMQTT Client ID : %s",clientid);
  mqtt_register(&conn, &mqtt_example_process, clientid, mqtt_event);

  mqtt_set_last_will(&conn, str_topic_state, "offline", MQTT_QOS_LEVEL_0);
  

  static struct etimer et;
  etimer_set(&et, CLOCK_SECOND);
  
 
	battery = sensors_find(BATTERY_SENSOR);
	battery_sensor_init();
	
	
  
  /* Reconnect from here */

  while(1) {
      if(simple_rpl_parent() == NULL) {

      printf("\r\n Connecting to a Wireless Network...\r\n");
      etimer_set(&et, CLOCK_SECOND / 1);
      while(simple_rpl_parent() == NULL) {
        PROCESS_WAIT_UNTIL(etimer_expired(&et));
        etimer_reset(&et);
        //leds_toggle(LEDS_RED);
		//leds_on(LEDS_RED);
      }
      //leds_off(LEDS_RED);
      printf("\r\n Connected to Wireless network\r\n");
    }

    /* Reset reconnecting flag */
    reconnecting = 0;

    /* Connect to MQTT server */
    conn.auto_reconnect = 1;
    mqtt_connect(&conn, HOST, 1883, 20);
    if (!mqtt_connected(&conn)) {
      printf("\r\n Not connected to the MQTT broker yet. Retrying ...");
      etimer_set(&et, CLOCK_SECOND / 1);
      while (!mqtt_connected(&conn)) {
        mqtt_connect(&conn, HOST, 1883, 20);
        PROCESS_WAIT_UNTIL(etimer_expired(&et));
        etimer_reset(&et);
      }      
    }    
      // PROCESS_WAIT_UNTIL(mqtt_connected(&conn));
    conn.auto_reconnect = 0;
    printf("\r\n Connected to the MQTT broker now");
    
	
	/* Publish to the online topic that we are online. */
    PROCESS_WAIT_UNTIL(mqtt_ready(&conn));
    mqtt_publish(&conn,
               NULL,
               str_topic_state,
               (uint8_t *)"online",
               strlen("online"),
               MQTT_QOS_LEVEL_0,
               MQTT_RETAIN_ON);
    //printf("comes here 2**************************\n");
    /* Subscribe to the led topic */
    PROCESS_WAIT_UNTIL(mqtt_ready(&conn));
    mqtt_subscribe(&conn,
                   NULL,
                   str_topic_led,
                   MQTT_QOS_LEVEL_0);
    //printf("comes here 3***************************\n");
    /* Main loop */
    while(1) {
      PROCESS_WAIT_EVENT();
      if(ev == reconnect_event) {
        mqtt_disconnect(&conn);
        reconnecting = 1;
        etimer_set(&reconnect_timer, CLOCK_SECOND*10);
      }
      if(reconnecting &&
         etimer_expired(&reconnect_timer)) {
        break;
      }


      if(etimer_expired(&sensor_timer) && mqtt_ready(&conn)) {
	  button_sensor_value++;
        /* Send sensor data and blink led */
		//leds_toggle(LEDS_RED);

		// get timestamp
		timestmp = clock_time();

		if(battery != NULL)
		battery_v = battery->value(0);
		get_sensor_value(TEMPERATURE,(sensor_result_t *) &temp);
		sprintf (batt_str, "%d", battery_v);
		temp.sensor_value*=100;
		sprintf(temp_str, "%d.%d", ((int)temp.sensor_value/100), ((int)temp.sensor_value%100));
		
		/* 
		* \" is escape sequence for Double Quotes.
		*/	
		sprintf(app_buffer,"%s%lu%s%s%s%s%s%s%s","{\"timestamp\": \"",timestmp,
			"\",\"BATTERY\": \"",
			batt_str,"\",\"TEMP\": \"", temp_str, "\",\"sender_id\": \"",
		mac_adr_str,"\"}");
		printf("\r\nAPP - Sending Battery sensor value %s Temp Sensor value %s  app buffer size %d\n",batt_str, temp_str, strlen(app_buffer));
	
		
        mqtt_publish(&conn,
               NULL,
               str_topic_sensor,
               (uint8_t *)app_buffer,
               strlen(app_buffer),
               MQTT_QOS_LEVEL_0,
               MQTT_RETAIN_OFF);
        etimer_restart(&sensor_timer);
		
      }
    }
  }
  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
