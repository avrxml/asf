#include "gpio.h"
#include "gpio_from_sdk.h"

#include "common.h"

/* from port.h */
/** Type definition for an GPIO/PORT pin interrupt module callback function. */
typedef void (*portint_callback_t)(void);

enum port_status_code {
	PORT_STATUS_OK = 0,
	PORT_STATUS_RESOURCE_NOT_AVAILABLE,
	PORT_STATUS_ERR_NOT_INITIALIZED,
	PORT_STATUS_ERR_INVALID_ARG,
};

enum port_wakeup_source {
	/** External Wakeup source AON_GPIO_0 */
	PORT_WAKEUP_SOURCE_AON_GPIO_0    = 0,
	/** External Wakeup source AON_GPIO_1 */
	PORT_WAKEUP_SOURCE_AON_GPIO_1,
	/** External Wakeup source AON_GPIO_2 */
	PORT_WAKEUP_SOURCE_AON_GPIO_2,
	PORT_WAKEUP_SOURCE_MAX_VAL
};
/* from port.h */

portint_callback_t  wakeup_source_callback[PORT_WAKEUP_SOURCE_MAX_VAL];

enum port_status_code wakeup_int_unregister_callback(enum port_wakeup_source wakeup_source);
void wakeup_active_event_callback(uint32_t wakeup_source);

void gpio1_combined_isr_handler(void)
{
	uint8_t index = 0;
	/* portint_callback_t callback; */
	uint8_t port_gpio1 = LPGPIO_16;
	uint32_t intstatus = GPIO1->INTSTATUSCLEAR.reg; /* jeffy */
	GPIO1->INTTYPESET.reg |= (1 << 15); /* jeffy */

	do {
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
		index = 0;
		if (intstatus != 0) {
			port_gpio1 = (intstatus & ~(intstatus - 1));
			while (!(port_gpio1 == 1)) {
				port_gpio1 = port_gpio1 >> 1;
				index++;
			}

			/* chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called */
			/* if(port_int_callback[index+LPGPIO_16] != NULL) { */
			/* callback = port_int_callback[index+LPGPIO_16]; */
			/* callback(); */
			/* } */
			/* CMSDK_GPIO1->INTCLEAR = (1 << index); */
			/* /REG_GPIO1_INTSTATUSCLEAR = (1 << index); */
			GPIO1->INTSTATUSCLEAR.reg = (1 << index); /* / jeffy */
		} else {
			/* CMSDK_GPIO1->INTCLEAR = CMSDK_GPIO1->INTSTATUS; */
			GPIO1->INTSTATUSCLEAR.reg |= (1 << 15); /* jeffy */
			/* REG_GPIO1_INTSTATUSCLEAR = REG_GPIO1_INTENSET; */
		}

		intstatus = GPIO1->INTSTATUSCLEAR.reg;
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
	} while (intstatus != 0);

	NVIC_ClearPendingIRQ(8);
}

void gpio0_combined_isr_handler(void)
{
	uint8_t index = 0;
	/* portint_callback_t callback; */
	uint8_t port_gpio0 = LPGPIO_0;
	uint32_t intstatus = GPIO0->INTSTATUSCLEAR.reg; /* jeffy */
	GPIO0->INTTYPESET.reg |= (1 << 15); /* jeffy */

	do {
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
		index = 0;
		if (intstatus != 0) {
			port_gpio0 = (intstatus & ~(intstatus - 1));
			while (!(port_gpio0 == 1)) {
				port_gpio0 = port_gpio0 >> 1;
				index++;
			}

			/* chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called */
			/* if(port_int_callback[index+LPGPIO_16] != NULL) { */
			/* callback = port_int_callback[index+LPGPIO_16]; */
			/* callback(); */
			/* } */
			/* CMSDK_GPIO1->INTCLEAR = (1 << index); */
			/* /REG_GPIO1_INTSTATUSCLEAR = (1 << index); */
			GPIO0->INTSTATUSCLEAR.reg = (1 << index); /* / jeffy */
		} else {
			/* CMSDK_GPIO1->INTCLEAR = CMSDK_GPIO1->INTSTATUS; */
			GPIO0->INTSTATUSCLEAR.reg |= (1 << 15); /* jeffy */
			/* REG_GPIO1_INTSTATUSCLEAR = REG_GPIO1_INTENSET; */
		}

		intstatus = GPIO0->INTSTATUSCLEAR.reg;
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
	} while (intstatus != 0);

	NVIC_ClearPendingIRQ(7);
}

#ifdef CHIPVERSION_B0
void PORT1_COMB_Handler(void)
{
	/* if(CMSDK_GPIO1->INTSTATUS & ((1<<15) | (1<<14) | (1<<13))) { */
	if (GPIO1->INTSTATUSCLEAR.reg & ((1 << 15) | (1 << 14) | (1 << 13))) {
		handle_ext_wakeup_isr();

		/* clear specific int pin status that caused the Interrupt */
		/* CMSDK_GPIO1->INTCLEAR |= CMSDK_GPIO1->INTSTATUS & ((1<<15) | (1<<14) | (1<<13)); */
		GPIO1->INTSTATUSCLEAR.reg |= GPIO1->INTSTATUSCLEAR.reg & ((1 << 15) | (1 << 14) | (1 << 13));
		/* NVIC_ClearPendingIRQ(PORT1_COMB_IRQn); */
		NVIC_ClearPendingIRQ(8);
	} else {
		gpio1_combined_isr_handler();
	}
}

#endif  /* CHIPVERSION_B0 */

enum port_status_code wakeup_int_unregister_callback(enum port_wakeup_source wakeup_source)
{
	enum port_status_code status = PORT_STATUS_OK;
	if(	(wakeup_source == PORT_WAKEUP_SOURCE_AON_GPIO_0) || 
		(wakeup_source == PORT_WAKEUP_SOURCE_AON_GPIO_1) || 
		(wakeup_source == PORT_WAKEUP_SOURCE_AON_GPIO_2) ) 
	{
		wakeup_source_callback[wakeup_source] = 0;
	}
	else 
	{
		status = PORT_STATUS_ERR_INVALID_ARG;
	}
	return status;
}

void wakeup_active_event_callback(uint32_t wakeup_source)
{
	portint_callback_t callback;
	uint32_t *pu32WakeSource = (uint32_t *)wakeup_source;
	if((*pu32WakeSource & 0xFF) == 1)
	{
		if(wakeup_source_callback[0] != NULL) {
			callback = wakeup_source_callback[0];
			callback();
		}
		*pu32WakeSource &= ~(0xFF);
	}
	if(((*pu32WakeSource >> 8) & 0xFF) == 1)
	{
		if(wakeup_source_callback[1] != NULL) {
			callback = wakeup_source_callback[1];
			callback();
		}
		*pu32WakeSource &= ~(0xFF << 8);
	}
	if(((*pu32WakeSource >> 16) & 0xFF) == 1)
	{
		if(wakeup_source_callback[2] != NULL) {
			callback = wakeup_source_callback[2];
			callback();
		}
		*pu32WakeSource &= ~(0xFF << 16);
	}
}
