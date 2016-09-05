#ifndef __BUTTON_H__
#define __BUTTON_H__

typedef void (*button_callback_t)(void);

void button_init(void);
void button_register_callback(button_callback_t callback);

#endif /* __BUTTON_H__ */