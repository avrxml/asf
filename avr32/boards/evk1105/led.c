/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3A EVK1105 board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the EVK1105 board.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include <avr32/io.h>
#include "preprocessor.h"
#include "compiler.h"
#include "evk1105.h"
#include "led.h"


//! Structure describing LED hardware connections.
typedef const struct
{
  struct
  {
    uint32_t PORT;     //!< LED GPIO port.
    uint32_t PIN_MASK; //!< Bit-mask of LED pin in GPIO port.
  } GPIO; //!< LED GPIO descriptor.
  struct
  {
    int32_t CHANNEL;  //!< LED PWM channel (< 0 if N/A).
    int32_t FUNCTION; //!< LED pin PWM function (< 0 if N/A).
  } PWM;  //!< LED PWM descriptor.
} tLED_DESCRIPTOR;


//! Hardware descriptors of all LEDs.
static tLED_DESCRIPTOR LED_DESCRIPTOR[LED_COUNT] =
{
#define INSERT_LED_DESCRIPTOR(LED_NO, unused)                 \
  {                                                           \
    {LED##LED_NO##_GPIO / 32, 1 << (LED##LED_NO##_GPIO % 32)},\
    {LED##LED_NO##_PWM,       LED##LED_NO##_PWM_FUNCTION    } \
  },
  MREPEAT(LED_COUNT, INSERT_LED_DESCRIPTOR, ~)
#undef INSERT_LED_DESCRIPTOR
};


//! Saved state of all LEDs.
static volatile uint32_t LED_State = (1 << LED_COUNT) - 1;


uint32_t LED_Read_Display(void)
{
  return LED_State;
}


void LED_Display(uint32_t leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor;
  volatile avr32_gpio_port_t *led_gpio_port;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  LED_State = leds;

  // For all LEDs...
  for (led_descriptor = &LED_DESCRIPTOR[0];
       led_descriptor < LED_DESCRIPTOR + LED_COUNT;
       led_descriptor++)
  {
    // Set the LED to the requested state.
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    if (leds & 1)
    {
      led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= 1;
  }
}


uint32_t LED_Read_Display_Mask(uint32_t mask)
{
  return Rd_bits(LED_State, mask);
}


void LED_Display_Mask(uint32_t mask, uint32_t leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  uint8_t led_shift;

  // Make sure only existing LEDs are specified.
  mask &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Wr_bits(LED_State, mask, leds);

  // While there are specified LEDs left to manage...
  while (mask)
  {
    // Select the next specified LED and set it to the requested state.
    led_shift = 1 + ctz(mask);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    leds >>= led_shift - 1;
    if (leds & 1)
    {
      led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= 1;
    mask >>= led_shift;
  }
}


bool LED_Test(uint32_t leds)
{
  return Tst_bits(LED_State, leds);
}


void LED_Off(uint32_t leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  uint8_t led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Clr_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and turn it off.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrs  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


void LED_On(uint32_t leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  uint8_t led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Set_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and turn it on.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrc  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


void LED_Toggle(uint32_t leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  uint8_t led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Tgl_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and toggle it.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrt  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK;
    leds >>= led_shift;
  }
}


uint32_t LED_Read_Display_Field(uint32_t field)
{
  return Rd_bitfield(LED_State, field);
}


void LED_Display_Field(uint32_t field, uint32_t leds)
{
  // Move the bit-field to the appropriate position for the bit-mask.
  LED_Display_Mask(field, leds << ctz(field));
}


uint8_t LED_Get_Intensity(uint32_t led)
{
  tLED_DESCRIPTOR *led_descriptor;

  // Check that the argument value is valid.
  led = ctz(led);
  led_descriptor = &LED_DESCRIPTOR[led];
  if (led >= LED_COUNT || led_descriptor->PWM.CHANNEL < 0) return 0;

  // Return the duty cycle value if the LED PWM channel is enabled, else 0.
  return (AVR32_PWM.sr & (1 << led_descriptor->PWM.CHANNEL)) ?
           AVR32_PWM.channel[led_descriptor->PWM.CHANNEL].cdty : 0;
}


void LED_Set_Intensity(uint32_t leds, uint8_t intensity)
{
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_pwm_channel_t *led_pwm_channel;
  volatile avr32_gpio_port_t *led_gpio_port;
  uint8_t led_shift;

  // For each specified LED...
  for (leds &= (1 << LED_COUNT) - 1; leds; leds >>= led_shift)
  {
    // Select the next specified LED and check that it has a PWM channel.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    if (led_descriptor->PWM.CHANNEL < 0) continue;

    // Initialize or update the LED PWM channel.
    led_pwm_channel = &AVR32_PWM.channel[led_descriptor->PWM.CHANNEL];
    if (!(AVR32_PWM.sr & (1 << led_descriptor->PWM.CHANNEL)))
    {
      led_pwm_channel->cmr = (AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) &
                             ~(AVR32_PWM_CALG_MASK |
                               AVR32_PWM_CPOL_MASK |
                               AVR32_PWM_CPD_MASK);
      led_pwm_channel->cprd = 0x000000FF;
      led_pwm_channel->cdty = intensity;
      AVR32_PWM.ena = 1 << led_descriptor->PWM.CHANNEL;
    }
    else
    {
      AVR32_PWM.isr;
      while (!(AVR32_PWM.isr & (1 << led_descriptor->PWM.CHANNEL)));
      led_pwm_channel->cupd = intensity;
    }

    // Switch the LED pin to its PWM function.
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    if (led_descriptor->PWM.FUNCTION & 0x1)
    {
      led_gpio_port->pmr0s = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->pmr0c = led_descriptor->GPIO.PIN_MASK;
    }
    if (led_descriptor->PWM.FUNCTION & 0x2)
    {
      led_gpio_port->pmr1s = led_descriptor->GPIO.PIN_MASK;
    }
    else
    {
      led_gpio_port->pmr1c = led_descriptor->GPIO.PIN_MASK;
    }
    led_gpio_port->gperc = led_descriptor->GPIO.PIN_MASK;
  }
}
