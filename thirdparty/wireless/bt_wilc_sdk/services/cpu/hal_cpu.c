// hal_cpu.h implementation
#include "hal_cpu.h"
#include "hal_led.h"

void hal_cpu_disable_irqs(void){
	//__disable_irq();
}

void hal_cpu_enable_irqs(void){
	// __enable_irq();
}

void hal_cpu_enable_irqs_and_sleep(void){
	hal_led_off();
	// __enable_irq();
	// __asm__("wfe");	// go to sleep if event flag isn't set. if set, just clear it. IRQs set event flag

	// note: hal_uart_needed_during_sleep can be used to disable peripheral clock if it's not needed for a timer
	hal_led_on();
}
