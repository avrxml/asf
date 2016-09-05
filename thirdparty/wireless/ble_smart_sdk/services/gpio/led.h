/*
 * led.h
 *
 * Created: 2015-11-04 오후 2:38:17
 *  Author: mksong
 */

void led_init(void);

#ifdef OTAU_FLASH_TEST
void debug_pin_init(void);

void debug_pin_set_high(void);

void debug_pin_set_low(void);

void debug_pin1_set_high(void);

void debug_pin1_set_low(void);

#endif /* OTAU_FLASH_TEST */
