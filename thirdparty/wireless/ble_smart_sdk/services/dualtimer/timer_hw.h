#ifndef __TIMER_HW_H__
#define __TIMER_HW_H__

typedef void (*hw_timer_callback_t)(void);

void hw_timer_init(void);
void hw_timer_register_callback(hw_timer_callback_t cb_ptr);
void hw_timer_start_ms(uint32_t delay_milliseconds);
void hw_timer_start(uint32_t delay);
void hw_timer_stop(void);

void dualtimer_callback2(void);

#endif /* __TIMER_HW_H__ */
