/**
* \file  demo_output.c
*
* \brief Demo output Implementation.
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#include "system.h"
#include "miwi_config.h"
#include "miwi_config_p2p.h"
#include "miwi_api.h"
#include "delay.h"
#include "demo_output.h"
#include "sio2host.h"
#include "asf.h"


/*************************************************************************/
// the following two variable arrays are the data to be transmitted
// in this demo. They are bit map of English word "MiWi" and "DE"
// respectively.
/*************************************************************************/
const uint8_t MiWi[6][21] =
{
    {0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
    
};   

const uint8_t DE[6][11] =
{
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
}; 


#define DEBOUNCE_TIME 0x00003FFF

#if defined (CONF_BOARD_JOYSTICK)
#define IS_JOYSTICK_PUSH_ACTION(x) (x > 25 && x < 35)
#define IS_JOYSTICK_UP_ACTION(x) (x > 85 && x < 95)
#define IS_JOYSTICK_LEFT_ACTION(x) (x > 120 && x < 130)
#define IS_JOYSTICK_DOWN_ACTION(x) (x > 220 && x < 230)
#define IS_JOYSTICK_RIGHT_ACTION(x) (x > 200 && x < 215)

static struct adc_module adc_instance;
#endif

bool PUSH_BUTTON_pressed;
MIWI_TICK PUSH_BUTTON_press_time;

void DemoOutput_Greeting(void)
{    
#if defined (ENABLE_LCD)
     #if defined(PHY_AT86RF233)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo on \n SAMR21 Node", 0, true);
        #endif
        #if defined(PROTOCOL_STAR)
            LCDDisplay((char *)"Simple STAR on \n SAMR21 Node", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi Demo on \n SAMR21 Node ", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO on \n SAMR21 Node ", 0, true);
        #endif
    #elif defined(PHY_AT86RF212B)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo on \n SAMR30 Node ", 0, true);
        #endif
        #if defined(PROTOCOL_STAR)
            LCDDisplay((char *)"Simple STAR on \n SAMR30 Node", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi Demo on \n SAMR30 Node ", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO on \n SAMR30 Node ", 0, true);
        #endif
    #endif
#endif
    #if defined (ENABLE_CONSOLE)
        #if defined(PROTOCOL_P2P)
            printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) P2P Stack ...");  
        #endif
        #if defined(PROTOCOL_STAR)
            printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) STAR Stack ...");  
        #endif
        #if defined(PROTOCOL_MIWI)
            printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) Stack ...");  
        #endif 
        #if defined(PROTOCOL_MIWI_PRO)
            printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) PRO Stack ...");  
        #endif 
    #endif
    #if defined(ENABLE_CONSOLE)   
        #if defined(PHY_AT86RF233)
        printf("\r\n     RF Transceiver: AT86RF233");
        printf("\r\n   Demo Instruction:");
        printf("\r\n                     Power on the board until LED 1 lights up");
        printf("\r\n                     to indicate connecting with peer. ");
        printf("\r\n                     Push SW Button to broadcast message. ");
#if defined (CONF_BOARD_JOYSTICK)		
        printf("\r\n                     Press Joystick CENTER Button to unicast encrypted message.");
#endif		
        printf("\r\n                     LED 1 will be toggled upon receiving messages. ");
        printf("\r\n\r\n");		
        #elif defined(PHY_AT86RF212B)
        printf("\r\n     RF Transceiver: AT86RF212B");
        printf("\r\n   Demo Instruction:");
        printf("\r\n                     Power on the board until LED 1 lights up");
        printf("\r\n                     to indicate connecting with peer.");
        printf("\r\n                     Push Button 1 to broadcast message.");
		printf("\r\n                     LED 1 will be toggled upon receiving messages. ");
        printf("\r\n\r\n");		
        #endif
 
    #endif 
}        

void demo_output_freezer_options(void)
{ 
#if defined (ENABLE_LCD)
	#if defined(PHY_AT86RF233)
	LCDDisplay((char *)"SW: Use Nwk Freezer \nPress in 5 sec", 0, false);
	#elif defined(PHY_AT86RF212B)
	LCDDisplay((char *)"SW: Use Nwk Freezer \nPress in 5 sec", 0, false);
	#endif
	delay_ms(1000);
#endif
}
void DemoOutput_Channel(uint8_t channel, uint8_t Step)
{
    if( Step == 0 )
    {
#if defined (ENABLE_LCD)       
        LCDDisplay((char *)"Connecting Peer on \n Channel ", channel, true);
#endif
#if defined (ENABLE_CONSOLE)
        #if !defined(MIWIKIT)
        printf("\r\nConnecting Peer on Channel ");
        printf("%d",channel);
        printf("\r\n");
        #endif
#endif
    }
    else
    { 
#if defined (ENABLE_LCD)
        LCDDisplay((char *)"Connected Peer on \n Channel ", channel, true);
#endif
#if defined (ENABLE_CONSOLE)
        #if !defined(MIWIKIT)
        printf("\r\nConnected Peer on Channel ");
        printf("%d",channel);
        printf("\r\n");
        #endif
#endif
    }
}    

void DemoOutput_Instruction(void)
{
#if defined (ENABLE_LCD)
#if BOARD == SAMR21ZLL_EK	
	  LCDDisplay((char *)"SW FUNC : Broadcast \nJOYSTKCenter:Unicast", 0, false);
#elif ((BOARD == SAMR21_XPLAINED_PRO) || (BOARD == SAMR30_XPLAINED_PRO)) && (defined EXT_BOARD_OLED1_XPLAINED_PRO)
	  LCDDisplay((char *)"SW     : Broadcast \nBUTTON1: Unicast", 0, false);
#elif ((BOARD == SAMR21_XPLAINED_PRO) || (BOARD == SAMR30_XPLAINED_PRO))
	  LCDDisplay((char *)"SW: Broadcast", 0, false);
#endif
#endif
}


void DemoOutput_HandleMessage(void)
{
    uint8_t i;

    if( rxMessage.flags.bits.secEn )
    {
        sio2host_tx((uint8_t *)"Secured ", sizeof("Secured "));
    }

    if( rxMessage.flags.bits.broadcast )
    {
        sio2host_tx((uint8_t *)"Broadcast Packet with RSSI ", sizeof("Broadcast Packet with RSSI "));
    }
    else
    {
        sio2host_tx((uint8_t *)"Unicast Packet with RSSI ",sizeof("Unicast Packet with RSSI "));
    }
    printf("%02x", rxMessage.PacketRSSI);
    if( rxMessage.flags.bits.srcPrsnt )
    {
        sio2host_tx((uint8_t *)" from ", sizeof(" from "));
        if( rxMessage.flags.bits.altSrcAddr )
        {
            printf( "%x", rxMessage.SourceAddress[1]);
            printf( "%x", rxMessage.SourceAddress[0]);
        }
        else
        {    
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                printf("%x", rxMessage.SourceAddress[MY_ADDRESS_LENGTH-1-i]);
            }    
        }
    }
    sio2host_tx((uint8_t *)": ",sizeof(": "));
    
    for(i = 0; i < rxMessage.PayloadSize; i++)
    {
        sio2host_putchar(rxMessage.Payload[i]);
    }       
} 

void DemoOutput_UpdateTxRx(uint8_t TxNum, uint8_t RxNum)
{
#if defined (ENABLE_LCD)
    LCDTRXCount(TxNum, RxNum);  
#endif
}

void DemoOutput_ChannelError(uint8_t channel)
{
    #if defined (ENABLE_CONSOLE)
        printf("\r\nSelection of channel ");
        printf("%d", channel);
        printf(" is not supported in current configuration.\r\n");
    #endif
}

void DemoOutput_UnicastFail(void)
{
    #if defined (ENABLE_CONSOLE)
        printf("\r\nUnicast Failed\r\n");
    #endif
#if defined (ENABLE_LCD)
    LCDDisplay((char *)" Unicast Failed", 0, true);
#endif
}    

#if defined(PROTOCOL_STAR)
    void Source_END_DEVICE_INFO(uint8_t *SrAddr)
    {
#if defined (ENABLE_LCD)
        LCD_Erase();
		snprintf(LCDText, sizeof(LCDText), "Data Packet from\n Address:%02x%02x%02x", SrAddr[0],SrAddr[1],SrAddr[2]);
		LCD_Update();
#endif
        delay_ms(1200);
    }
    
void STAR_DEMO_OPTIONS_MESSAGE(bool NetworkRole)
{
#if defined (ENABLE_LCD)
    if (NetworkRole)
    {
		#if defined(PHY_AT86RF233)
	        LCDDisplay((char *)"PC :SW FUNC \n   to Broadcast", 0, false);
	    #elif defined(PHY_AT86RF212B)
	        LCDDisplay((char *)"PC :SW FUNC \n   to Broadcast", 0, false);
	    #endif
    }
    else
    {
		#if defined(PHY_AT86RF233)
			LCDDisplay((char *)"SW FUNC : Unicast \nJOYSTICK: Next Node", 0, false);
	    #elif defined(PHY_AT86RF212B)
	        LCDDisplay((char *)"SW FUNC : Unicast \nJOYSTICK: Next Node", 0, false);
	    #endif
    }
#endif    
}
#endif
    

/*********************************************************************
 * Function:        uint8_t ButtonPressed(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Return value to indicate which button has been pressed.
 *                  Return 0 if no button pressed.
 *
 * Side Effects:
 *
 * Overview:        This function check if any button has been pressed
 *
 * Note:
 ********************************************************************/

uint8_t ButtonPressed(void)
{
    MIWI_TICK tickDifference;
#if (BOARD == SAMR21_XPLAINED_PRO) || (BOARD == SAMR21ZLL_EK) || (BOARD == SAMR30_XPLAINED_PRO)
    if(!port_pin_get_input_level(BUTTON_0_PIN))
    {
        //if the button was previously not pressed
        if(PUSH_BUTTON_pressed == false)
        {
            PUSH_BUTTON_pressed = true;
            PUSH_BUTTON_press_time.Val = MiWi_TickGet();
            return 1;
        }
    }
#if ((BOARD == SAMR21_XPLAINED_PRO) && (defined EXT_BOARD_OLED1_XPLAINED_PRO)) || \
          ((BOARD == SAMR30_XPLAINED_PRO) && (defined EXT_BOARD_OLED1_XPLAINED_PRO))
    else if(!port_pin_get_input_level(WING_BUTTON_1))
    {
	    //if the button was previously not pressed
	    if(PUSH_BUTTON_pressed == false)
	    {
		    PUSH_BUTTON_pressed = true;
		    PUSH_BUTTON_press_time.Val = MiWi_TickGet();
		    return 2;
	    }
    }
#endif	
    else
    {
        //get the current time
        MIWI_TICK t;
		t.Val = MiWi_TickGet();

        //if the button has been released long enough
        tickDifference.Val = MiWi_TickGetDiff(t,PUSH_BUTTON_press_time);

        //then we can mark it as not pressed
        if(tickDifference.Val > DEBOUNCE_TIME)
        {
            PUSH_BUTTON_pressed = false;
        }
    }
#endif
    return 0;
}

#if (defined EXT_BOARD_OLED1_XPLAINED_PRO)
/**
 * \brief Configures buttons
 */
void Buttons_init(void)
{
	struct port_config conf;
	port_get_config_defaults(&conf);

	conf.direction = PORT_PIN_DIR_INPUT;
	conf.input_pull = PORT_PIN_PULL_UP;

	/* Configure all three buttons as inputs */
	port_pin_set_config(WING_BUTTON_1, &conf);
	port_pin_set_config(WING_BUTTON_2, &conf);
	port_pin_set_config(WING_BUTTON_3, &conf);
}
#endif

#if defined (CONF_BOARD_JOYSTICK)
/*********************************************************************
* Function: uint8_t Joystick_Init(void)
*
* Overview: Inits the Joystick
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void Joystick_Init(void)
{
	struct adc_config conf_adc;
	
	adc_get_config_defaults(&conf_adc);

	conf_adc.reference = ADC_REFERENCE_INTVCC0;
	conf_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	conf_adc.positive_input = ADC_POSITIVE_INPUT_PIN6;
	conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
	conf_adc.resolution = ADC_RESOLUTION_8BIT;
	
	adc_init(&adc_instance, ADC, &conf_adc);
	
	adc_enable(&adc_instance);
}


/*********************************************************************
* Function: uint8_t JoystickPressed(void)
*
* Overview: Reads the Joystick state
*
* PreCondition: None
*
* Input:  None
*
* Output: joystick State
*
********************************************************************/
uint8_t JoystickPressed(void)
{
   uint16_t read_value;

   adc_start_conversion(&adc_instance);
   
  do 
  {
	/* Wait for conversion to be done and read out result */
  } while (adc_read(&adc_instance, &read_value) == STATUS_BUSY);

  if (IS_JOYSTICK_UP_ACTION(read_value))
  {
	  return JOYSTICK_UP;
  }
  else if (IS_JOYSTICK_DOWN_ACTION(read_value))
  {
	  return JOYSTICK_DOWN;
  }
  else if (IS_JOYSTICK_LEFT_ACTION(read_value))
  {
	  return JOYSTICK_LEFT;
  }
  else if (IS_JOYSTICK_RIGHT_ACTION(read_value))
  {
	  return JOYSTICK_RIGHT;
  }
  else if (IS_JOYSTICK_PUSH_ACTION(read_value))
  {
	  return JOYSTICK_CENTER;
  }
  else
  {
	  return JOYSTICK_NONE;
  }
}

#endif



