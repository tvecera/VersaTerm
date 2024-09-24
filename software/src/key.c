// ****************************************************************************
//
//                             PicoPad Buttons
//
// ****************************************************************************
// Code taken from:
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.
#include "pins.h"
#include "key.h"

#if USE_KEY

#include "pico/stdlib.h"

// buttons GPIOs
const uint8_t key_gpio_tab[KEY_NUM] = KEY_GPIO_TAB;

// time of last press/release
uint16_t key_last_press[KEY_NUM];
uint16_t key_last_release[KEY_NUM];

// keys are currently pressed (index = button code - 1)
volatile bool key_press_map[KEY_NUM];

// keyboard buffer
#define KEYBUF_SIZE  10 // size of keyboard buffer
char key_buf[KEYBUF_SIZE]; // keyboard buffer
char key_ret_buf;  // returned key to keyboard buffer (NOKEY = no key)
volatile uint8_t key_write_off; // write offset to keyboard buffer
volatile uint8_t key_read_off;  // read offset from keyboard buffer

#if SYSTICK_KEYSCAN
struct repeating_timer timer0;

void key_scan();

bool repeating_timer_callback(struct repeating_timer *t) {
	key_scan();
	return true;
}

#endif

// initialize keys
void __in_flash(".device") key_init() {
	int i;
	for (i = 0; i < KEY_NUM; i++) {
		gpio_init(key_gpio_tab[i]); // initialize GPIO to input
		gpio_set_dir(key_gpio_tab[i], GPIO_IN);
		gpio_pull_up(key_gpio_tab[i]); // pull-up
		key_press_map[i] = false;
	}

	key_ret_buf = NOKEY;
	key_write_off = 0;
	key_read_off = 0;

#if SYSTICK_KEYSCAN
	add_repeating_timer_us(50000, repeating_timer_callback, NULL, &timer0);
#endif
}

// write key to keyboard buffer
void key_write_key(char key) {
	uint8_t w = key_write_off;
	uint8_t w2 = w + 1;
	if (w2 >= KEYBUF_SIZE) w2 = 0;

	__asm volatile (" dmb\n":: : "memory");

	if (w2 != key_read_off) {
		key_buf[w] = key;
		key_write_off = w2;
	}
}

// scan keys
void key_scan() {
	int i;
	uint16_t t = to_ms_since_boot(get_absolute_time()); // time in [ms]
	for (i = 0; i < KEY_NUM; i++) {
		// check if button is pressed
		if (gpio_get(key_gpio_tab[i]) == 0) {
			// button is pressed for the first time
			if (!key_press_map[i]) {
				key_last_press[i] = t + (KEY_REP_TIME1 - KEY_REP_TIME2);
				key_press_map[i] = true;
				key_write_key(i + 1);
			}

				// button is already pressed - check repeat interval
			else {
				if ((signed long int) (t - key_last_press[i]) >= (signed short) KEY_REP_TIME2) {
					key_last_press[i] = t;
					key_write_key(i + 1);
				}
			}
			key_last_release[i] = t;
		}

			// button is release - check stop of press
		else {
			if (key_press_map[i]) {
				if ((signed short) (t - key_last_release[i]) >= (signed short) KEY_REL_TIME) {
					key_press_map[i] = false;
				}
			}
		}
	}
}

// check if key KEY_* is currently pressed
bool key_pressed(char key) {
#if !SYSTICK_KEYSCAN  // call KeyScan() function from SysTick system timer
	key_scan();
#endif
	if ((key < 1) || (key > KEY_NUM)) return false;
	return key_press_map[key - 1];
}

// get button from keyboard buffer (returns NOKEY if no scan code)
char key_get() {
#if !SYSTICK_KEYSCAN  // call KeyScan() function from SysTick system timer
	// scan keyboard
	key_scan();
#endif

	// get key from temporary 1-key buffer
	char ch = key_ret_buf;
	if (ch != NOKEY) {
		key_ret_buf = NOKEY;
		return ch;
	}

	// check if keyboard buffer is empty
	uint8_t r = key_read_off;

	__asm volatile (" dmb\n":: : "memory");

	if (r == key_write_off) return NOKEY;

	// get key from keyboard buffer
	ch = key_buf[r];

	// write new read offset
	r++;
	if (r >= KEYBUF_SIZE) r = 0;
	key_read_off = r;

	return ch;
}

// flush keyboard buffer
void key_flush() {
	key_read_off = key_write_off;
	key_ret_buf = NOKEY;
}

// return key to keyboard buffer (can hold only 1 key)
void key_ret(char key) {
	key_ret_buf = key;
}

// check no pressed key
bool key_no_pressed() {
	int i;
	for (i = 0; i < KEY_NUM; i++) if (key_press_map[i]) return false;
	return true;
}

// wait for no key pressed
void key_wait_no_pressed() {
	while (!key_no_pressed()) {}
}

#endif