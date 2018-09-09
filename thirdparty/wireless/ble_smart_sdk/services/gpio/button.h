#ifndef __BUTTON_H__
#define __BUTTON_H__

typedef void (*button_callback_t)(void);

void button_init(void);
void button_register_callback(button_callback_t callback);
uint8_t button_0_input_level(void);

void aon_gpio0_cb_handler(void);
void aon_gpio1_cb_handler(void);
void aon_gpio2_cb_handler(void);

#endif /* __BUTTON_H__ */