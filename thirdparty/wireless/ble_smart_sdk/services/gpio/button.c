#include <asf.h>
#include "button.h"

button_callback_t button_cb_func = NULL;

static void button_callback(void)
{
	gpio_disable_callback(BUTTON_0_PIN);

	if(button_cb_func)
	{
		button_cb_func();
	}

	gpio_enable_callback(BUTTON_0_PIN);
}

void button_init(void)
{
	struct gpio_config config_gpio_pin;

	gpio_get_config_defaults(&config_gpio_pin);

	config_gpio_pin.direction  = GPIO_PIN_DIR_INPUT;
	config_gpio_pin.input_pull = GPIO_PIN_PULL_NONE;

	gpio_pin_set_config(BUTTON_0_PIN, &config_gpio_pin);
}

void button_register_callback(button_callback_t callback)
{
	button_cb_func = callback;
	
	if(button_cb_func == NULL)
	{
		gpio_disable_callback(BUTTON_0_PIN);
	}
	else
	{
		gpio_register_callback(BUTTON_0_PIN, button_callback, GPIO_CALLBACK_RISING);
		gpio_enable_callback(BUTTON_0_PIN);
	}
}
