#include <asf.h>
#include "button.h"
#include "platform.h"

extern uint32_t 	*wakeup_event_pending;
bool button_debounce = true; 

void button_init(void)
{
	struct gpio_config config_gpio_pin;

	gpio_get_config_defaults(&config_gpio_pin);

	config_gpio_pin.direction  = GPIO_PIN_DIR_INPUT;
	config_gpio_pin.input_pull = GPIO_PIN_PULL_NONE;

	gpio_pin_set_config(BUTTON_0_PIN, &config_gpio_pin);
}

void aon_gpio0_cb_handler(void)
{
if(button_debounce)
{
	*wakeup_event_pending &= ~0xFF;
	*wakeup_event_pending |= 0x1;
	button_debounce = false;
}
}

void aon_gpio1_cb_handler(void)
{
	*wakeup_event_pending &= ~(0xFF << 8);
	*wakeup_event_pending |= (0x1 << 8);
}

void aon_gpio2_cb_handler(void)
{
	*wakeup_event_pending &= ~(0xFF << 16);
	*wakeup_event_pending |= (0x1 << 16);
}

void button_register_callback(button_callback_t callback)
{
	#ifdef __SAMB11G18A__
	enum gpio_callback cb_type = GPIO_CALLBACK_FALLING;
	#else
	enum gpio_callback cb_type = GPIO_CALLBACK_RISING;
	#endif
	
	
	if(callback != NULL)
	{
		#if (BUTTON_0_PIN == PIN_AO_GPIO_0)
		{
			wakeup_int_register_callback(PORT_WAKEUP_SOURCE_AON_GPIO_0, callback);
			gpio_register_callback(BUTTON_0_PIN, aon_gpio0_cb_handler, GPIO_CALLBACK_RISING);
		}
		#elif (BUTTON_0_PIN == PIN_AO_GPIO_1)
		{
			wakeup_int_register_callback(PORT_WAKEUP_SOURCE_AON_GPIO_1, callback);
			gpio_register_callback(BUTTON_0_PIN, aon_gpio1_cb_handler, GPIO_CALLBACK_RISING);
		}
		#elif (BUTTON_0_PIN == PIN_AO_GPIO_2)
		{
			wakeup_int_register_callback(PORT_WAKEUP_SOURCE_AON_GPIO_2, callback);
			gpio_register_callback(BUTTON_0_PIN, aon_gpio2_cb_handler, GPIO_CALLBACK_RISING);
		}
		#else
		{			
			gpio_register_callback(BUTTON_0_PIN, callback, cb_type);
		}
		#endif
		
		gpio_enable_callback(BUTTON_0_PIN);		
	}
	else
	{
		gpio_disable_callback(BUTTON_0_PIN);
	}
	
	cb_type = cb_type; //For Compiler warnings
}

uint8_t button_0_input_level(void)
{
	return gpio_pin_get_input_level(BUTTON_0_PIN);
}
