// ****************************************************************************
//
//                              PicoPad LEDs
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

#if USE_LED

#include "led.h"

// LED configuration
typedef struct {
		unsigned char pin;   // GPIO pin for the LED
		unsigned char inverted; // Inversion flag for the LED
} led_config_t;

// GPIO LED configurations
const led_config_t led_gpio_tab[LED_NUM] = LED_GPIO_TAB;

// set LED ON (inx = LED index LED?)
void led_on(unsigned char inx) {
	gpio_put(led_gpio_tab[inx].pin, true);
}

// set LED OFF (inx = LED index LED?)
void led_off(unsigned char inx) {
	gpio_put(led_gpio_tab[inx].pin, false);
}

// flip LED (inx = LED index LED?)
void led_flip(unsigned char inx) {
	gpio_xor_mask(1UL << led_gpio_tab[inx].pin);
}

// set LED (inx = LED index LED?)
void led_set(unsigned char inx, unsigned char val) {
	if (val == 0) led_off(inx); else led_on(inx);
}

// initialize LEDs
void __in_flash(".device") led_init() {
	int i;
	for (i = 0; i < LED_NUM; i++) {
		gpio_init(led_gpio_tab[i].pin);
		gpio_set_dir(led_gpio_tab[i].pin, GPIO_OUT);
		if (led_gpio_tab[i].inverted) gpio_set_outover(led_gpio_tab[i].pin, GPIO_OVERRIDE_INVERT);
		led_off(i);
	}
}

#endif
