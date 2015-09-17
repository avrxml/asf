/**
 * \file
 *
 * \brief XMEGA-A1 Xplained demo application
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage
 *
 * This application emulates the Production test which all XMEGA-A1 Xplained
 * boards are delivered with from factory.
 * Further, it shows how to use some of the functionality in the AVR Software
 * Framework found at http://asf.atmel.no.
 *
 * This application is a Memory game, in which the user must press the board
 * buttons in the same sequence in which their corresponding LEDs have lit up.
 * The game plays back samples or tones, depending on the game configuration,
 * via DAC and onboard speaker when buttons are pressed or LED sequences are
 * played.
 *
 * A sequence of button presses must be recorded before the game can start, and
 * the board will show running LEDs while waiting for the user to enter such a
 * sequence. The running LEDs indicate the timeout before the button press
 * sequence is saved to EEPROM and the game is started. If the user does not
 * press any buttons before the timeout, the application will simply restart the
 * process.
 *
 * The game can be configured at startup by means of two jumpers:
 * - by connecting \ref GAMEMODE_SENSE_PIN to \ref GAMEMODE_GROUND_PIN, the
 * board will try to load a button press sequence from EEPROM and start the game
 * of Memory, assuming valid game data could be found.
 * - by connecting \ref SOUNDMODE_SENSE_PIN and \ref SOUNDMODE_GROUND_PIN, the
 * application is configured to play notes from the chromatic scale, starting at
 * an A of 441Hz, instead of the sound samples.
 *
 * For more information on the various drivers included in ASF, see the website,
 * where documentation and code examples detailing each driver and service can
 * be found.
 *
 * \section Configuration
 * Some board-specific configurations and definitions are required for this
 * application. These are contained in \ref conf_example.h.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <board.h>
#include <compiler.h>
#include <gpio.h>
#include <nvm.h>
#include <sysclk.h>
#include <tc.h>
#include <conf_example.h>

#include "sound.h"

#define F_CPU sysclk_get_cpu_hz()

#ifdef __GNUC__
#  include <util/delay.h>
#else
static inline void _delay_ms(uint16_t t)
{
	uint16_t i;

	for (i = 0; i < t; ++i) {
		__delay_cycles(F_CPU / 6000);
	}
}
#endif

//! \name Board-specific application configuration
//@{
/**
 * \def GAMEMODE_SENSE_PIN
 * \brief Game mode configuration jumper sense pin ID.
 */

/**
 * \def GAMEMODE_GROUND_PIN
 * \brief Game mode configuration jumper ground pin ID.
 */

/**
 * \def SOUNDMODE_SENSE_PIN
 * \brief Sound mode configuration jumper sense pin ID.
 */

/**
 * \def SOUNDMODE_GROUND_PIN
 * \brief Sound mode configuration jumper ground pin ID.
 */

/**
 * \def DEBOUNCE_TIMER_MODULE
 * \brief Timer module to use for debouncing of push buttons.
 */

/**
 * \def TIMEOUT_TIMER_MODULE
 * \brief Timer module to use for detecting timeout.
 */

/**
 * \def SOUND_TIMER_MODULE
 * \brief Timer module to use for triggering DAC conversions.
 */

/**
 * \def SOUND_TIMER_EVENT
 * \brief Timer overflow event source for event system channel MUX.
 *
 * \note This must correspond to \ref SOUND_TIMER_MODULE.
 */

/**
 * \fn get_board_button_mask
 * \brief Get bitmask with current board button status
 *
 * \note The buttons should be mapped to the bits in a way which matches their
 * location with the LEDs.
 *
 * \return Status of buttons:
 * - 0 : button is pressed
 * - 1 : button is not pressed
 */

/**
 * \fn write_board_led_mask
 * \brief Set status of all board LEDs
 *
 * \note The LEDs should be mapped to the bits in a way which matches their
 * location with the buttons.
 *
 * \param mask Mask of LEDs to turn ON and OFF:
 * - 0 : LED will be turned OFF
 * - 1 : LED will be turned ON
 */
//@}

//! Length of header for saving button presses to EEPROM.
#define SEQUENCE_HEADER_LENGTH      2

//! Maximum length of buffer for button press sequence.
#define SEQUENCE_BUFFER_MAX_LENGTH  255

//! Enumeration of game modes.
enum game_mode {
	//! Record button presses for the game.
	GAME_RECORD_MODE,
	//! Play game with recorded button sequence.
	GAME_PLAY_MODE,
};

//! Enumeration of sound playback modes.
enum sound_mode {
	//! One-shot playback of samples.
	SOUND_SAMPLES,
	//! Looped playback of sinewaves.
	SOUND_SINEWAVE,
};

//! Buffer to contain the button press sequence.
uint8_t sequence_buffer[SEQUENCE_BUFFER_MAX_LENGTH];
//! Length of currently recorded button sequence in buffer.
uint8_t sequence_buffer_length;

//! Counter for timeout-period before saving button sequence.
static volatile uint8_t save_timeout;

//! \name Internal application functions
//@{
static void start_debounce_timer(void);
static void init_save_timer(void);
static void start_save_timer(void);
static void play_game(uint8_t buttons, enum sound_mode soundmode,
		enum game_mode *gamemode);
static void record_buttons(uint8_t buttons, enum sound_mode soundmode);
static void save_gamedata(void);
static void load_gamedata(void);
//@}

void timeout_callback(void);

/**
 * \brief Main function.
 *
 * Initializes the board, checks the jumper configuration and sets up the game
 * accordingly before the main loop is entered. The main loop is where the state
 * of the board buttons is read out and passed on to the game functions
 * \ref record_buttons or \ref play_game, depending on the state of the game.
 */
int main(void)
{
	enum sound_mode soundmode;
	enum game_mode  gamemode;

	sysclk_init();
	board_init();

	sound_init();

	// Configure the pins for selection of game mode.
	gpio_configure_pin(SOUNDMODE_SENSE_PIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	gpio_configure_pin(SOUNDMODE_GROUND_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

	gpio_configure_pin(GAMEMODE_SENSE_PIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	gpio_configure_pin(GAMEMODE_GROUND_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

	start_debounce_timer();
	init_save_timer();
	pmic_enable_level(PMIC_LVL_LOW);
	cpu_irq_enable();

	// Load current game data, if there is any.
	load_gamedata();

	// Select initial game state based on presence of config jumper.
	if (gpio_pin_is_low(GAMEMODE_SENSE_PIN) && (sequence_buffer[0] == 0xCC)) {
		gamemode = GAME_PLAY_MODE;
		sequence_buffer_length = sequence_buffer[1];
	} else {
		gamemode = GAME_RECORD_MODE;
		sequence_buffer_length = 0;
		save_timeout = 0x0F;
		start_save_timer();
	}

	// Select sinewave or sample playback based on presence of config jumper.
	soundmode = gpio_pin_is_low(SOUNDMODE_SENSE_PIN) ?
			SOUND_SINEWAVE : SOUND_SAMPLES;

	while (1) {
		uint8_t buttons;

		buttons = get_board_button_mask();

		switch (gamemode) {
		case GAME_RECORD_MODE:
			record_buttons(buttons, soundmode);

			// Show buttons if any were pressed..
			if (buttons) {
				write_board_led_mask(buttons);
				save_timeout = 0x0F;
			} else {
			// ..otherwise, display the timeout.
				uint8_t _save_timeout = save_timeout;

				write_board_led_mask((_save_timeout & 0x0F)
						| ((_save_timeout & 0x0F) << 4));
			}

			if (!save_timeout || (sequence_buffer_length ==
					(SEQUENCE_BUFFER_MAX_LENGTH
					 - SEQUENCE_HEADER_LENGTH))) {
				if (sequence_buffer_length == 0) {
					save_timeout = 0x0F;
					start_save_timer();
					continue;
				}

				save_gamedata();

				gamemode = GAME_PLAY_MODE;
			}
			break;

		case GAME_PLAY_MODE:
			write_board_led_mask(buttons);
			play_game(buttons, soundmode, &gamemode);
			break;
		}
	}
}


/**
 * \brief Set up and start timer for debouncing of the push buttons.
 *
 * The timer/counter is set up in normal, up-counting mode with a period of
 * approximately 10 ms.
 */
void start_debounce_timer(void)
{
	uint32_t resolution;
	uint16_t period;

	tc_enable(&DEBOUNCE_TIMER_MODULE);
	tc_set_wgm(&DEBOUNCE_TIMER_MODULE, TC_WG_NORMAL);
	tc_set_resolution(&DEBOUNCE_TIMER_MODULE, 1000);

	resolution = tc_get_resolution(&DEBOUNCE_TIMER_MODULE);
	period = (resolution * 10) / 1000;
	tc_write_period(&DEBOUNCE_TIMER_MODULE, period);
}

/**
 * \brief Start timer for measuring inactivity before saving.
 *
 * The timer/counter should be initialized with \ref init_save_timer before this
 * function is called.
 */
void start_save_timer(void)
{
	tc_write_clock_source(&TIMEOUT_TIMER_MODULE, TC_CLKSEL_DIV1024_gc);
}

/**
 * \brief Set up timer for measuring inactivity before saving.
 *
 * The timer/counter is set up in normal, up-counting mode with a period of
 * approximately 1.6 seconds.
 */
void init_save_timer(void)
{
	uint32_t resolution;
	uint16_t period;

	tc_enable(&TIMEOUT_TIMER_MODULE);
	tc_set_overflow_interrupt_callback(&TIMEOUT_TIMER_MODULE, &timeout_callback);
	tc_set_overflow_interrupt_level(&TIMEOUT_TIMER_MODULE, PMIC_LVL_LOW);

	resolution = sysclk_get_per_hz() / 1024;
	period = (resolution * 16) / 10;
	tc_write_period(&TIMEOUT_TIMER_MODULE, period);
}

/**
 * \brief Record button presses and play sound.
 *
 * \note This modifies the global variables \ref sequence_buffer and
 * \ref sequence_buffer_length.
 *
 * \param buttons Mask with pressed buttons.
 * \param soundmode Indicates whether to play a sound sample or sinewave.
 */
void record_buttons(uint8_t buttons, enum sound_mode soundmode)
{
	static uint8_t last_buttons = 0;
	static uint8_t flt_buttons = 0;
	static uint8_t flt_last_buttons = 0;
	static uint8_t pressed_buttons = 0;
	uint8_t        dn_buttons;
	uint8_t        up_buttons;

	// Perform debouncing.
	if (tc_is_overflow(&DEBOUNCE_TIMER_MODULE)) {
		tc_clear_overflow(&DEBOUNCE_TIMER_MODULE);

		if (buttons == last_buttons) {
			flt_buttons = buttons;
		}
		last_buttons = buttons;
	}

	// Detect changed buttons based on previously filtered states.
	dn_buttons = (flt_buttons ^ flt_last_buttons) & flt_buttons;
	up_buttons = (flt_buttons ^ flt_last_buttons) & flt_last_buttons;

	// Add new button presses to the mask to save.
	pressed_buttons |= dn_buttons;

	switch(soundmode) {
	case SOUND_SAMPLES:
		// Play single sample for new button presses.
		sound_play_sample(dn_buttons);
		break;

	case SOUND_SINEWAVE:
		// Play notes for all pressed buttons.
		sound_play_note(buttons);
		break;
	}

	// Save the button press mask when all buttons have been released.
	if (up_buttons && !flt_buttons) {
		sequence_buffer[SEQUENCE_HEADER_LENGTH + sequence_buffer_length++] = pressed_buttons;
		pressed_buttons = 0x00;
	}

	flt_last_buttons = flt_buttons;
}

/**
 * \brief Compares button presses with stored game data.
 *
 * Uses a timer to debounce buttons. The method used requires
 * the function to be called often, but not necessarily regularly.
 *
 * \param buttons Mask with pressed buttons.
 * \param soundmode Indicates whether to play a sound sample or sinewave.
 * \param gamemode Game mode pointer -- the mode is changed if the game is won.
 */
void play_game(uint8_t buttons, enum sound_mode soundmode,
		enum game_mode *gamemode)
{
	static bool    newgame  = true;
	static uint8_t game_top = 1;
	static uint8_t game_seq = 0;
	static uint8_t last_buttons = 0;
	static uint8_t flt_buttons = 0;
	static uint8_t flt_last_buttons = 0;
	static uint8_t pressed_buttons = 0;
	uint8_t        dn_buttons;
	uint8_t        up_buttons;

	// Perform debouncing.
	if (tc_is_overflow(&DEBOUNCE_TIMER_MODULE)) {
		tc_clear_overflow(&DEBOUNCE_TIMER_MODULE);

		if (buttons == last_buttons) {
			flt_buttons = buttons;
		}
		last_buttons = buttons;
	}

	// Detect changed buttons based on previously filtered states.
	dn_buttons = (flt_buttons ^ flt_last_buttons) & flt_buttons;
	up_buttons = (flt_buttons ^ flt_last_buttons) & flt_last_buttons;

	// Add new button presses to the mask to play with.
	pressed_buttons |= dn_buttons;

	if (soundmode == SOUND_SAMPLES) {
		// Make sure sound is played only on first press.
		sound_play_sample(dn_buttons);
		while (sound_is_playing());
	}

	/* Process the game state if the game was just started or all pressed
	 * buttons have been released.
	 */
	if (newgame || (up_buttons && !flt_buttons)) {

		// Correct buttons pressed?
		if (pressed_buttons == sequence_buffer[SEQUENCE_HEADER_LENGTH
				+ game_seq]) {
			// Advance the game -- light up next LED in sequence.
			game_seq++;

			// Round finished? Make the next round longer.
			if (game_seq == game_top) {
				game_seq = 0;
				game_top++;
			}
		} else {
		// No.
			// Restart the game, play a sound and flash the LEDs.
			game_top = 1;
			game_seq = 0;

			sound_play_sample(PIN4_bm);

			write_board_led_mask(0xff);
			_delay_ms(2000);
			write_board_led_mask(0x00);
			_delay_ms(1000);
		}

		// Was this the last round?
		if (game_top > sequence_buffer_length) {
			// Play fanfare and flash some LEDs.
			sound_play_sample(PIN0_bm | PIN2_bm | PIN7_bm);

			write_board_led_mask(0x55);
			_delay_ms(1500);
			write_board_led_mask(0xaa);
			_delay_ms(1500);

			while (sound_is_playing());

			write_board_led_mask(0x55);
			_delay_ms(1500);
			write_board_led_mask(0xaa);
			_delay_ms(1500);

			// Play "you win!" voice cut and flash LEDs again.
			sound_play_sample(PIN0_bm | PIN1_bm | PIN7_bm);

			write_board_led_mask(0x55);
			_delay_ms(1500);
			write_board_led_mask(0xaa);
			_delay_ms(1500);

			while (sound_is_playing());

			// Reinitialize the game.
			game_top = 1;
			game_seq = 0;
			newgame = true;
			pressed_buttons = 0x00;

			// ..and go to record mode.
			sequence_buffer_length = 0;
			save_timeout = 0x0F;
			start_save_timer();
			*gamemode = GAME_RECORD_MODE;

			return;
		}

		// Light up the LEDs in sequence.
		if (game_seq == 0) {
			uint8_t i;
			uint8_t seq_mask;

			for(i = 0; i < game_top; ++i) {
				seq_mask = sequence_buffer[
					SEQUENCE_HEADER_LENGTH + i];

				sound_stop_timer();
				write_board_led_mask(0x00);
				_delay_ms(400);
				write_board_led_mask(seq_mask);

				switch (soundmode) {
				case SOUND_SAMPLES:
					sound_play_sample(seq_mask);
					while (sound_is_playing());
					break;

				case SOUND_SINEWAVE:
					sound_play_note(seq_mask);
					_delay_ms(400);
					break;
				}
			}
		}

		// Reset button press status and flag game as no longer new.
		pressed_buttons = 0x00;
		newgame = false;
	} else {
	// Some buttons are still pressed.
		switch (soundmode) {
		case SOUND_SAMPLES:
		// Samples play only once.
			break;

		case SOUND_SINEWAVE:
		// Sound notes for currently pressed buttons.
			sound_play_note(buttons);
			break;
		}
	}

	flt_last_buttons = flt_buttons;
}

//! Save current game data to EEPROM.
void save_gamedata(void)
{
	uint16_t addr;

	// Update the header of the save buffer.
	sequence_buffer[0] = 0xCC;
	sequence_buffer[1] = sequence_buffer_length;

	for (addr = 0; addr < SEQUENCE_BUFFER_MAX_LENGTH; addr += 32) {
		nvm_eeprom_flush_buffer();
		nvm_eeprom_load_page_to_buffer(&sequence_buffer[addr]);
		nvm_eeprom_atomic_write_page(addr / 32);
	}
}

//! Load game data from EEPROM.
void load_gamedata(void)
{
	uint16_t i;

	for (i = 0; i < SEQUENCE_BUFFER_MAX_LENGTH; ++i) {
		sequence_buffer[i] = nvm_eeprom_read_byte(i);
	}
}

/**
 * \brief Timer/counter overflow interrupt callback function.
 *
 * This updates the variable for the save timeout and stops its timer/counter
 * once timeout has occurred.
 *
 * \note Modifies global variable save_timeout.
 */
void timeout_callback(void)
{
	save_timeout = save_timeout >> 1;
	if (!save_timeout) {
		tc_write_clock_source(&TIMEOUT_TIMER_MODULE, TC_CLKSEL_OFF_gc);
	}
}
