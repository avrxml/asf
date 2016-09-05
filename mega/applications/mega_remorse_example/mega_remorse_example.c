/**
 * \file remorse.c
 *
 * \brief Morse Code Encoder/Decorder example
 *
 * Copyright (c) 2015-2016 Atmel Corporation. All rights reserved.
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
 * \mainpage MEGA Remorse Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 *
 * \page appdoc_preface Overview
 * This application remorse your input and output it through usart.
 */

/**
 * \page appdoc_main MEGA Remorse Example
 *
 * Overview:
 * - \ref appdoc_mega_remorse_app_intro
 * - \ref appdoc_mega_remorse_app_usage
 * - \ref appdoc_mega_remorse_app_compinfo
 * - \ref appdoc_mega_remorse_app_contactinfo
 *
 * \section appdoc_mega_remorse_app_intro Introduction
 * This application remorse your input and output it through usart.
 *
 * This application has been tested on following boards:
 * - ATmega328p Xplained Mini
 * - ATmega328pb Xplained Mini
 * - ATmega168pb Xplained Mini
 * - ATmega324pb Xplained Pro
 *
 * \section appdoc_mega_remorse_app_usage Usage
 * The application uses usart to receive input and output remorse result, 
 * once you input through usart, remorse result will output though usart,
 * if you press the button more than 4s, remorse will restart,
 * led will blink in remorse.
 *
 * \section appdoc_mega_remorse_app_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for MEGA.
 * Other compilers may or may not work.
 *
 * \section appdoc_mega_remorse_app_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <asf.h>
#include <conf_usart.h>


/* typedefs  */

/**
 * \brief Ring Buffer Structure.
 */
typedef struct {
	/** Offset from start of buffer where to write next. */
	int  p_to_buf;
	/** Offset from start of buffer where to read next. */
	int  p_from_buf;
	/** Size of ring in bytes. */
	int  buf_size;
	/** Pointer to start of buffer. */
	char *buf;
	} RING;

typedef RING *RING_ID;

/* defines */
#define DOT_TIME_IN_TICKS    (25)
#define DOT_TIME_IN_MS       (DOT_TIME_IN_TICKS * 10.0)
#define BAUD_PRESCALE        (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
/* the clock basis for polling and sequencing */
#define REMORSE_CLK_HZ       (100)
/* remorse will restart after this many idle ticks */
#define	REMORSE_TIMEOUT      (1000)
/* remorse will restart if button held this long */
#define	REMORSE_RESTART      (400)
/* ring buffer for decoded morse code */
#define MORSE_RX_BUF_SIZE    (32)

/* macros */
#define min(a,b)                \
   ({ __typeof__ (a) _a = (a);  \
      __typeof__ (b) _b = (b);  \
     _a < _b ? _a : _b; })


/* locals */

/* buffer for morse characters read */
static char                  morse_rx_buf[MORSE_RX_BUF_SIZE + 1]; 
/* morse receive buffer ring */
static RING                  morse_rx_ring;
/* morse receive buffer ring id */
static RING_ID               morse_rx_ring_id;
/* Instructions print out until first character received */
static bool                  user_input = false;
/* incremented on each clock tick */
static volatile uint32_t     remorse_tick_cnt;
/* reset to zero after each user input output */
static volatile uint32_t     remorse_wd_cnt;
/* set by watchdog to true to restart application */
static volatile bool         remorse_restart;

const uint8_t morse_ascii_tbl[][2] = {
	/* Morse Hex  A M    Morse Code      Binary  ASCII   Notes */
	{6, 0x14}, // ! !   – · – · – –     010100   33 33  Exclamation mark [!]
	{6, 0x2D}, // " "   · – · · – ·     101101   34 34  Quotation mark ["]
	{8, 0xFF}, // # ERR · · · · · · · · 11111111 35 ERR Hash mark [#]
	{7, 0x76}, // $ $   · · · – · · –   1110110  36 36  Dollar sign [$]
	{8, 0xFF}, // % ERR · · · · · · · · 11111111 37 ERR Percent [%]
	{5, 0x17}, // & &   · – · · ·       10111    38 38  Ampersand [&], Wait
	{6, 0x21}, // ' '   · – – – – ·     100001   39 39  Apostrophe [']
	{5, 0x09}, // ( (   – · – – ·       01001    40 40  Parenthesis open [(]
	{6, 0x12}, // ) )   – · – – · –     010010   41 41  Parenthesis close [)]
	{8, 0xFF}, // * ERR · · · · · · · · 11111111 42 69  Asterix [*]
	{5, 0x15}, // + +   · – · – ·       10101    43 43  Plus [+]
	{6, 0x0C}, // , ,   – – · · – –     001100   44 44  Comma [,]
	{6, 0x1E}, // - -   – · · · · –     011110   45 45  Hyphen, Minus [-]
	{6, 0x2A}, // . .   · – · – · –     101010   46 46  Period [.]
	{5, 0x0D}, // / /   – · · – ·       01101    47 47  Slash [/], Fraction bar
	{5, 0x00}, // 0 0   – – – – –       00000    48 48
	{5, 0x10}, // 1 1   · – – – –       10000    49 49
	{5, 0x18}, // 2 2   · · – – –       11000    50 50
	{5, 0x1C}, // 3 3   · · · – –       11100    51 51
	{5, 0x1E}, // 4 4   · · · · –       11110    52 52
	{5, 0x1F}, // 5 5   · · · · ·       11111    53 53
	{5, 0x0F}, // 6 6   – · · · ·       01111    54 54
	{5, 0x07}, // 7 7   – – · · ·       00111    55 55
	{5, 0x03}, // 8 8   – – – · ·       00011    56 56
	{5, 0x01}, // 9 9   – – – – ·       00001    57 57
	{6, 0x07}, // : :   – – – · · ·     000111   58 58  Colon [:]
	{6, 0x15}, // ; ;   – · – · – ·     010101   59 59  Semicolon [;]
	{8, 0xFF}, // < ERR · · · · · · · · 11111111 60 ERR Less Than [<]
	{5, 0x0E}, // = =   – · · · –       01110    61 61  Double dash [=]
	{8, 0xFF}, // > ERR · · · · · · · · 11111111 62 ERR Greather Than [<]
	{6, 0x33}, // ? ?   · · – – · ·     110011   63 63  Question mark [?]
	{6, 0x25}, // @ @   · – – · – ·     100101   64 64  At sign [@]
	{2, 0x02}, // A A   · –             10       65 65
	{4, 0x07}, // B B   – · · ·         0111     66 66
	{4, 0x05}, // C C   – · – ·         0101     67 67
	{3, 0x03}, // D D   – · ·           011      68 68
	{1, 0x01}, // E E   ·               1        69 69
	{4, 0x0D}, // F F   · · – ·         1101     70 70
	{3, 0x01}, // G G   – – ·           001      71 71
	{4, 0x0F}, // H H   · · · ·         1111     72 72
	{2, 0x03}, // I I   · ·             11       73 73
	{4, 0x08}, // J J   · – – –         1000     74 74
	{3, 0x02}, // K K   – · –           010      75 75
	{4, 0x0B}, // L L   · – · ·         1011     76 76
	{2, 0x00}, // M M   – –             00       77 77
	{2, 0x01}, // N N   – ·             01       78 78
	{3, 0x00}, // O O   – – –           000      79 79
	{4, 0x09}, // P P   · – – ·         1001     80 80
	{4, 0x02}, // Q Q   – – · –         0010     81 81
	{3, 0x05}, // R R   · – ·           101      82 82
	{3, 0x07}, // S S   · · ·           111      83 83
	{1, 0x00}, // T T   –               0        84 84 
	{3, 0x06}, // U U   · · –           110      85 85
	{4, 0x0E}, // V V   · · · –         1110     86 86
	{3, 0x04}, // W W   · – –           100      87 87
	{4, 0x06}, // X X   – · · –         0110     88 88
	{4, 0x04}, // Y Y   – · – –         0100     89 89
	{4, 0x03}, // Z Z   – – · ·         0011     90 90 
	{8, 0xFF}, // [ ERR · · · · · · · · 11111111 91 ERR Open Bracket [[]
	{8, 0xFF}, // \ ERR · · · · · · · · 11111111 92 ERR Backslash [\]
	{8, 0xFF}, // ] ERR · · · · · · · · 11111111 93 ERR Close Bracket []]
	{8, 0xFF}, // ^ ERR · · · · · · · · 11111111 94 ERR Circumflex [^]
	{6, 0x32}  // _ _   · · – – · –     110010   95 95  Underscore [_]
};

/* forward declarations */

static int              bsp_tty_putchar (char c, FILE *stream);
static int              bsp_tty_getchar (FILE *stream);
static int              bsp_morse_putchar (char c, FILE *stream);
static int              bsp_morse_getchar (FILE *stream);
static void             remorse_instructions(void);
static void             morse_fmtchar (char rc);
static void             morse_poll (void);
static RING_ID          rng_init (RING *p_ring, int nbytes, char *buffer);
static bool             rng_is_empty (RING_ID ring_id);
static bool             rng_is_full (RING_ID ring_id);
static int              rng_buf_get (RING_ID rng_id, char *buffer, int maxbytes);
static int              rng_buf_put (RING_ID rng_id, char *buffer, int nbytes);
static int              rng_free_bytes (RING_ID ring_id);
static int              rng_nBytes (RING_ID ring_id);
static void             rng_flush (RING_ID ring_id);

/* Associate the devices with AVR libc FILE streams to enable standard i/o calls. */

static FILE tty_console = FDEV_SETUP_STREAM(bsp_tty_putchar,   bsp_tty_getchar,   _FDEV_SETUP_RW);
static FILE tty_morse   = FDEV_SETUP_STREAM(bsp_morse_putchar, bsp_morse_getchar, _FDEV_SETUP_RW);


/**
 * \brief Entry point for morse code example
 *
 */

int main (void)
{
	char rc;

	/* set up the IO pins */
	board_init();

	/* start with the LED off */
	LED_Off(LED0);

	/* set and enable usart */
	const usart_rs232_options_t options ={
		.baudrate   = CONF_BAUDRATE,
		.charlength = CONF_CHARLENGTH,
		.paritytype = CONF_PARITY,
		.stopbits   = CONF_STOPBITS
	};
#if AVR8_PART_IS_DEFINED(ATmega324PB)
	usart_init_rs232(&USART1, &options);
#else
	usart_init_rs232(&USART0, &options);
#endif

	/* set up the system 100Hz timer */
	/* Set CTC compare value to 10ms @ 8Mhz Clock (Mode 4) */
	OCR1A   = 20000;
	/* Configure timer 1 for CTC mode (Mode 4) */
	TCCR1B |= (1 << WGM12);
	/* Enable interrupt on compare match */
	TIMSK1 |= (1 << OCIE1A);
	/* set prescaler to 8 and starts the timer */
	TCCR1B |= (1 << CS11);

	/* initialize ring buffers */
	morse_rx_ring_id = rng_init (&morse_rx_ring, MORSE_RX_BUF_SIZE, morse_rx_buf);
	
	/* Enable global interrupts */
	sei(); 

	/* attach the serial port to stdio streams */
	stdout = &tty_console;
	stdin  = &tty_console;
	stderr = &tty_console;

	/* encode/decode the user input to/from morse code */
	/* loop forever (timeouts restart here) */
	for (;;) {
		/* print out some instructions */
		remorse_instructions ();
		/* reset watchdog */
		remorse_restart = false;
		/* reinitialize watchdog timer */
		remorse_wd_cnt  = 0;

		/* check watchdog */
		while (!remorse_restart) {
			/* non-blocking in this application */
			rc = getchar ();

			/* process input from serial line */
			/* check if a character available */
			if (!ferror(stdin)) {
				// reinitialize watchdog timer
				remorse_wd_cnt = 0;
				// echo the morse code on console
				morse_fmtchar (rc);
				// encode the character to morse led
				fputc (rc, &tty_morse);
			} else {
				// no input, clear error
				clearerr (&tty_console);
			}

			/* process input from morse decoder */
			rc = fgetc (&tty_morse);
			// was a morse character available?
			if (!ferror(&tty_morse)) {
				// reinitialize watchdog timer
				remorse_wd_cnt = 0;
				// print the character to console
				putchar (rc);
			} else {
				// no input, clear error
				clearerr (&tty_morse);
			}
		}   
	} 

	/* we should never get here */
	return (0);
}

/**
 * \brief Push a character out uart transmitter
 *
 * Polling version of character write.  We spin here until character
 * can be placed in transmit register.
 */

static int bsp_tty_putchar(char c, FILE *stream)
{
	// handle linefeed - carriage return
	if (c == '\n') {
		bsp_tty_putchar('\r', stream);
	}
	
#if AVR8_PART_IS_DEFINED(ATmega324PB)
	while (!usart_data_register_is_empty(&USART1)) {
		// Do nothing until UDR is ready for more data to be written to it
	};

	// transmit character
	usart_put(&USART1, c);
#else
	while (!usart_data_register_is_empty(&USART0)) {
		// Do nothing until UDR is ready for more data to be written to it
	};

	// transmit character
	usart_put(&USART0, c);
#endif
	// transmit success
	return (0);
}

/**
 * \brief Fetch a character from uart receiver
 *
 * Polling version of a character read.  We spin here until receive
 * buffer has something to read.
 */

static int bsp_tty_getchar(FILE *stream)
{
#if AVR8_PART_IS_DEFINED(ATmega324PB)
	if (!usart_rx_is_complete(&USART1)) {
		// No character available, so we return error
		return (_FDEV_ERR);
	}	

	// Record the fact that we have received input
	user_input = true;
	// Return the received character
	return usart_get(&USART1);
#else
	if (!usart_rx_is_complete(&USART0)) {
		// No character available, so we return error
		return (_FDEV_ERR);
	}

	// Record the fact that we have received input
	user_input = true;
	// Return the received character
	return usart_get(&USART0);
#endif
}

/**
 * \brief Flash out morse code for character on led
 *
 * Polling version of character write.  We spin here until character
 * can be placed in transmit register.
 */

static int bsp_morse_putchar(char c, FILE *stream)
{
	int index = toupper(c) - '!';

	if (c == ' ') {
		// Delay between words is seven units
		LED_Off(LED0);
		// LED OFF between words
		_delay_ms (7 * DOT_TIME_IN_MS);
	} else if ((index >= 0) && (index < (sizeof (morse_ascii_tbl) / 2))) {
		unsigned int mask = (0x1 << (morse_ascii_tbl[index][0] - 1));
		while (mask != 0) {
			if (morse_ascii_tbl[index][1] & mask) {
				// DIT
				LED_On(LED0);
				// DIT length is one unit
				_delay_ms (DOT_TIME_IN_MS);
			} else {
				// DASH
				LED_On(LED0);
				// DASH length is three units
				_delay_ms (3 * DOT_TIME_IN_MS);
			}
			// DIT or DASH is finished
			LED_Off(LED0);

			// Delay between parts of same letter is one unit
			_delay_ms (DOT_TIME_IN_MS);
			mask = mask >> 1;
		}

		// Delay between letters is three units (two more)
		_delay_ms (2 * DOT_TIME_IN_MS);
	}
	// transmit success
	return (0);
}

/**
 * \brief Decode morse code input from button
 *
 * If we have any decoded morse code we return it or _FDEV_ERR if none.
 */

static int bsp_morse_getchar(FILE *stream)
{
	char morse_char;

	if (rng_buf_get (morse_rx_ring_id, &morse_char, 1) == 1){
		// return next character
		return (morse_char);
	}
	// No character available, so we return error
	return (_FDEV_ERR);
}

/**
 * \brief Print out character as ASCII morse code (. and -)
 */

static void morse_fmtchar (char rc)
{
	int index = toupper(rc) - '!';

	if ((index >= 0) && (index < (sizeof (morse_ascii_tbl) / 2))) {
		unsigned int mask = (0x1 << (morse_ascii_tbl[index][0] - 1));

		while (mask != 0) {
			printf_P (PSTR(" %c"), (morse_ascii_tbl[index][1] & mask) ? '.' : '-');
			mask = mask >> 1;
		}

		printf_P (PSTR(" [%c]"), toupper(rc));
	} else {
		/* handle non-printable characters */
		if (rc == '\r') {
			printf_P (PSTR(" . - . - [<CR>]\n-> "));
		}
	}
}


/**
 * \brief Parse morse code pattern into ASCII character
 *
 * \param[in] pattern morse code pattern to match
 * \param[in] len     morse code patter length
 * \return    the ASCII character that matches the mores code patter
 */

static char morse_parse (uint8_t pattern, int len)
{
	int index;

	for (index = 0; index < (sizeof (morse_ascii_tbl) / 2); index ++) {
		if ((morse_ascii_tbl[index][0] == len) && (morse_ascii_tbl[index][1] == pattern)) {
			return (index + '!');
		}
	}
	/* pattern not found! */
	// return asterix as error indicator (not a morse character)
	return ('*');
}

/**
 * \brief Print out remorse instructions
 */

static void remorse_instructions (void)
{
	if (!user_input) {
		printf_P (PSTR("\n"));
		printf_P (PSTR("Morse Encoder/Decoder\n"));
		printf_P (PSTR("Copyright (c) 2015 - 2016 Atmel Corporation. All rights reserved.\n"));
		printf_P (PSTR("\n"));
		printf_P (PSTR("Characters received from the serial port will be be echoed back in morse code.\n"));
		printf_P (PSTR("Morse code tapped into button will be converted to ASCII.\n"));
		printf_P (PSTR("\n"));
		printf_P (PSTR("(ie)"));

		morse_fmtchar ('A'); fputc ('A', &tty_morse);
		morse_fmtchar ('T'); fputc ('T', &tty_morse);
		morse_fmtchar ('M'); fputc ('M', &tty_morse);
		morse_fmtchar ('E'); fputc ('E', &tty_morse);
		morse_fmtchar ('L'); fputc ('L', &tty_morse);
	}
	// prompt for serial input
	printf_P (PSTR("\n-> "));
}

/**
 * Interrupt Service Routines
 */

/**
 * \brief Remorse clock interrupt routine
 *
 * This routine is called at interrupt level on each clock interrupt.
 */

ISR(TIMER1_COMPA_vect)
{
	// always advance remorse tick count
	++remorse_tick_cnt;
	// fire watchdog on timeout
	if (++remorse_wd_cnt > REMORSE_TIMEOUT)	{
		remorse_restart = true;
	}
	// poll for morse input
	morse_poll ();
}

/**
 * \brief Poll button for debounced conversion to Morse code
 */

static void morse_poll (void)
{
	// built up morse code input
	static uint8_t  morse_code      = 0;
	// morse code length
	static uint8_t  morse_code_len  = 0;
	// 8 sample button history
	static uint8_t  button_hist     = 0;
	// current state of button
	static bool     button_down     = false;
	// tick when button released
	static uint32_t tick_down       = 0;
	// tick when button pressed
	static uint32_t tick_up         = 0;
	// ticks with button down
	static uint32_t duration        = 0;
	// ticks with button up
	static uint32_t space           = 0;

	/* shift in the current state of the button */

	button_hist = (button_hist<<1) | (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) | 0xf0;
	// all samples are button down
	if (button_hist == 0xff) {
		// light the LED for user feedback
		LED_On(LED0);
		// we have interaction
		user_input = true;

		// are we changing state?
		if (!button_down) {
			// set debounced button state
			button_down = true;
			// timestamp the button press
			tick_down   = remorse_tick_cnt;
		}

		duration = remorse_tick_cnt - tick_down;

		// restart!
		if (duration > REMORSE_RESTART)	{
			// erase current code
			morse_code_len = 0;
			// erase last code
			morse_code     = 0;
			// flush the morse input ring
			rng_flush (morse_rx_ring_id);
			// start from begining
			user_input    = false;
			// RESTART REMORSE
			remorse_restart = true;
		}
	} else if (button_hist == 0xf0){
		// all samples are button up
		if (button_down) {
			// turn off LED between morse keys
			LED_Off(LED0);
			// set debounced button state
			button_down = false;
			// timestamp button release
			tick_up     = remorse_tick_cnt;

			/* emit the right code based on click duration */

			duration = tick_up - tick_down;

			if (duration <= (2 * DOT_TIME_IN_TICKS)) {
				morse_code = (morse_code << 1) | 1;
				morse_code_len++;
				rng_buf_put (morse_rx_ring_id, " .", 2);
			} else if (duration <= (5 * DOT_TIME_IN_TICKS)) {
				morse_code = (morse_code << 1) | 0;
				morse_code_len++;
				rng_buf_put (morse_rx_ring_id, " -", 2);
			}
		}
	}

	/* return now if there has been no input or button is currently down */

	if ((!user_input) || (button_down))
		return;

	/* now process spaces between button presses as follows
	 *     - one dot time between code (NOP)
	 *     - three dot time between letters (flush the letter)
	 *     - seven dot time between words (flush space chracter)
	 */

	space = remorse_tick_cnt - tick_up;
	// this will only happen once
	if (space == (4 * DOT_TIME_IN_TICKS)) {
		char morse_char = morse_parse (morse_code, morse_code_len);

		// letter seperator
		rng_buf_put (morse_rx_ring_id, " [", 2);
		rng_buf_put (morse_rx_ring_id, &morse_char, 1);
		rng_buf_put (morse_rx_ring_id, "]", 1);
		// get ready for next letter
		morse_code_len = 0;
		// erase last code
		morse_code     = 0;
	} else if (space == (10 * DOT_TIME_IN_TICKS)) {
		// word separator
		rng_buf_put (morse_rx_ring_id, " [ ]", 4);
	}
}

/**
 * Ring Utility Functions
 */

/**
 * \brief initialize an empty ring buffer
 *
 * This routine initializes a ring buffer of size "nbytes". The
 * buffer must be one byte larger than nBytes since the algorithm
 * always leaves at least one empty byte in the buffer.
 *
 * \param[in] p_ring  pointer to a RING struct
 * \param[in] nbytes  number of bytes in ring buffer
 * \param[in] buffer  pointer to RING buffer of size nBytes + 1
 * \return    ID of the ring buffer, or NULL if init fails
 */

RING_ID rng_init (RING * p_ring, int nbytes, char *buffer)
{
	/* RING_ID is simply a pointer to RING */
	RING_ID ring_id = p_ring;

	/* bump number of bytes requested because ring buffer algorithm
	 * always leaves at least one empty byte in buffer */

	ring_id->buf_size = nbytes + 1;
	ring_id->buf  = buffer;

	rng_flush (ring_id);

	return (ring_id);
}
/**
 * \brief make a ring buffer empty
 *
 * This routine initializes a specified ring buffer to be empty.
 * Any data currently in the buffer will be lost.
 *
 * \param[in] ring_id  ring buffer to initialize
 */

void rng_flush (RING_ID ring_id)
{
	ring_id->p_to_buf   = 0;
	ring_id->p_from_buf = 0;
}
/**
 * \brief - get characters from a ring buffer
 *
 * This routine copies bytes from the ring buffer <rng_id> into <buffer>.
 * It copies as many bytes as are available in the ring, up to <maxbytes>.
 * The bytes copied will be removed from the ring.
 *
 * param[in] ring_id  ring buffer to get data from
 * param[in] buffer   pointer to buffer to receive data
 * param[in] maxbytes maximum number of bytes to get
 * \return   The number of bytes actually received from the ring buffer;
 *           it may be zero if the ring buffer is empty at the time of the call.
 */

int rng_buf_get (RING_ID rng_id, char *buffer, int maxbytes)
{
	int bytesgot  = 0;
	int p_to_buf  = rng_id->p_to_buf;
	int bytes2;
	int p_rng_tmp = 0;

	if (p_to_buf >= rng_id->p_from_buf) {
		/* p_to_buf has not wrapped around */

		bytesgot = min (maxbytes, p_to_buf - rng_id->p_from_buf);
		memcpy (buffer, &rng_id->buf [rng_id->p_from_buf], bytesgot);
		rng_id->p_from_buf += bytesgot;
	} else {
		/* p_to_buf has wrapped around.  Grab chars up to the end of the
		 * buffer, then wrap around if we need to. */

		bytesgot  = min (maxbytes, rng_id->buf_size - rng_id->p_from_buf);
		memcpy (buffer, &rng_id->buf [rng_id->p_from_buf], bytesgot);
		p_rng_tmp = rng_id->p_from_buf + bytesgot;

		/* If p_from_buf is equal to buf_size, we've read the entire buffer,
		 * and need to wrap now.  If bytesgot < maxbytes, copy some more chars
		 * in now. */

		if (p_rng_tmp == rng_id->buf_size) {
			bytes2 = min (maxbytes - bytesgot, p_to_buf);
			memcpy (buffer + bytesgot, rng_id->buf, bytes2);
			rng_id->p_from_buf = bytes2;
			bytesgot += bytes2;
		}
		else
			rng_id->p_from_buf = p_rng_tmp;
	}
	return (bytesgot);
}
/**
 * \brief - put bytes into a ring buffer
 *
 * This routine puts bytes from <buffer> into ring buffer <ring_id>. The
 * specified number of bytes will be put into the ring, up to the number of
 * bytes available in the ring.
 *
 *
 * \param[in] rng_id  ring buffer to put data into
 * \param[in] buffer  buffer to get data from
 * \param[in] nbytes  number of bytes to try to put
 * \return    The number of bytes actually put into the ring buffer;
 *            it may be less than number requested, even zero,
 *            if there is insufficient room in the ring buffer at the time of the call.
 */

int rng_buf_put (RING_ID rng_id, char *buffer, int nbytes)
{
	int bytesput = 0;
	int p_from_buf = rng_id->p_from_buf;
	int bytes2;
	int p_rng_tmp = 0;

	if (p_from_buf > rng_id->p_to_buf) {
		/* p_from_buf is ahead of p_to_buf.	 We can fill up to two bytes
		 * before it */

		bytesput = min (nbytes, p_from_buf - rng_id->p_to_buf - 1);
		memcpy (&rng_id->buf [rng_id->p_to_buf], buffer, bytesput);
		rng_id->p_to_buf += bytesput;
	} else if (p_from_buf == 0) {
		/* p_from_buf is at the beginning of the buffer.	We can fill till
		 * the next-to-last element */

		bytesput = min (nbytes, rng_id->buf_size - rng_id->p_to_buf - 1);
		memcpy (&rng_id->buf [rng_id->p_to_buf], buffer, bytesput);
		rng_id->p_to_buf += bytesput;
	} else {
		/* p_from_buf has wrapped around, and its not 0, so we can fill
		 * at least to the end of the ring buffer.	Do so, then see if
		 * we need to wrap and put more at the beginning of the buffer. */

		bytesput = min (nbytes, rng_id->buf_size - rng_id->p_to_buf);
		memcpy (&rng_id->buf [rng_id->p_to_buf], buffer, bytesput);
		p_rng_tmp = rng_id->p_to_buf + bytesput;

		if (p_rng_tmp == rng_id->buf_size) {
			/* We need to wrap, and perhaps put some more chars */

			bytes2 = min (nbytes - bytesput, p_from_buf - 1);
			memcpy (rng_id->buf, buffer + bytesput, bytes2);
			rng_id->p_to_buf = bytes2;
			bytesput += bytes2;
		} else
			rng_id->p_to_buf = p_rng_tmp;
	}
	return (bytesput);
}
/**
 * \brief - test if a ring buffer is empty
 *
 * This routine determines if a specified ring buffer is empty.
 *
 * \param[in] rng_id  ring buffer to test
 * \return    TRUE if empty, FALSE if not.
 */

bool rng_is_empty (RING_ID ring_id)
{
	return (ring_id->p_to_buf == ring_id->p_from_buf);
}
/**
 * \brief - test if a ring buffer is full (no more room)
 *
 * This routine determines if a specified ring buffer is completely full.
 *
 * \param[in] rng_id ring buffer to test
 * \return    TRUE if full, FALSE if not.
 */

bool rng_is_full (RING_ID ring_id)
{
	int n = ring_id->p_to_buf - ring_id->p_from_buf + 1;

	return ((n == 0) || (n == ring_id->buf_size));
}
/**
 * \brief - determine the number of free bytes in a ring buffer
 *
 * This routine determines the number of bytes currently unused in a specified
 * ring buffer.
 *
 * \param[in] rng_id  ring buffer to examine
 * \return    The number of unused bytes in the ring buffer.
 */

int rng_free_bytes (RING_ID ring_id)
{
	int n = ring_id->p_from_buf - ring_id->p_to_buf - 1;

	if (n < 0)
		n += ring_id->buf_size;

	return (n);
}
/**
 *
 * rng_nBytes - determine the number of bytes in a ring buffer
 *
 * This routine determines the number of bytes currently in a specified
 * ring buffer.
 *
 * \param[in] rng_id  ring buffer to be enumerated
 * \return    The number of bytes filled in the ring buffer.
 */

int rng_nBytes (RING_ID ring_id)
{
	int n = ring_id->p_to_buf - ring_id->p_from_buf;

	if (n < 0)
		n += ring_id->buf_size;

	return (n);
}
/**
 * \brief - put a byte ahead in a ring buffer without moving ring pointers
 *
 * This routine writes a byte into the ring, but does not move the ring buffer
 * pointers. Thus the byte will not yet be available to rng_buf_get() calls.
 * The byte is written <offset> bytes ahead of the next input location in the
 * ring. Thus, an offset of 0 puts the byte in the same position as would
 * RNG_ELEM_PUT would put a byte, except that the input pointer is not updated.
 *
 * Bytes written ahead in the ring buffer with this routine can be made available
 * all at once by subsequently moving the ring buffer pointers with the routine
 * rng_move_ahead().
 *
 * Before calling nng_put_ahead(), the caller must verify that at least
 * <offset> + 1 bytes are available in the ring buffer.
 *
 * \param[in] ring_id  ring buffer to put byte in
 * \param[in] byte     byte to be put in ring
 * \param[in] offset   offset beyond next input byte where to put byte
 */

void rng_put_ahead (RING_ID ring_id, char byte, int offset)
{
	int n = ring_id->p_to_buf + offset;

	if (n >= ring_id->buf_size)
		n -= ring_id->buf_size;

	*(ring_id->buf + n) = byte;
}
/**
 * \brief - advance a ring pointer by <n> bytes
 *
 * This routine advances the ring buffer input pointer by <n> bytes. This makes
 * <n> bytes available in the ring buffer, after having been written ahead in
 * the ring buffer with rng_put_ahead().
 *
 * \param[in] ring_id  ring buffer to be advanced
 * \param[in] n        number of bytes ahead to move input pointer
 */

void rng_move_ahead (RING_ID ring_id, int n)
{
	n += ring_id->p_to_buf;

	if (n >= ring_id->buf_size)
		n -= ring_id->buf_size;

	ring_id->p_to_buf = n;
}
