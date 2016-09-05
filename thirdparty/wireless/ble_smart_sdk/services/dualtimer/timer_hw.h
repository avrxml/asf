#ifndef __TIMER_HW_H__
#define __TIMER_HW_H__

typedef void (*hw_timer_callback_t)(void);

void hw_timer_init(void);
void hw_timer_register_callback(hw_timer_callback_t cb_ptr);
void hw_timer_start(uint32_t delay);
void hw_timer_stop(void);

void dualtimer_callback2(void);

#define PWR_WAKEUP_DOMAIN_ARM   (1)
#define USER_TIMER_CALLBACK             0x41
#define AON_SLEEP_TIMER_EXPIRY_CALLBACK 0x42

/** Enum for the possible callback types for the timer module. */
enum timer_callback_type {
	/** Callback for timer expiry*/
	TIMER_EXPIRED_CALLBACK_TYPE_DETECT = 1,
};

#endif /* __TIMER_HW_H__ */
