#ifndef PINS_H
#define PINS_H

#define VERSATERM 1
#define EXTDISP   2
#define PICOLCD   3

#if BOARD == VERSATERM
#define BOARD_VERSATERM
#include "boards/versaterm/board_def.h"
#elif BOARD == EXTDISP
#define BOARD_EXTDISP
#include "boards/extdisp/board_def.h"
#elif BOARD == PICOLCD
#define BOARD_PICOLCD
#include "boards/picolcd/board_def.h"
#endif

#ifndef USE_VGA
#define USE_VGA 1
#endif

#ifndef USE_HDMI
#define USE_HDMI 1
#endif

#if !USE_HDMI && !USE_VGA
#error "Cannot disable both VGA and HDMI output"
#endif

#ifndef USE_DEVICE_INIT
#define USE_DEVICE_INIT 0
#endif

// Default splash screen
#ifndef SPLASH_CONTENT
#define SPLASH_CONTENT \
{ \
		"\016lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk\n", \
		"x\017                     VersaTerm 1.0                     \016x", \
		"x\017                 (C) 2022 David Hansel                 \016x", \
		"x\017          https://github.com/dhansel/VersaTerm         \016x", \
		"tqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqu", \
		"x\017  DVI output  via https://github.com/Wren6991/PicoDVI  \016x", \
		"x\017  VGA output  via https://github.com/Panda381/PicoVGA  \016x", \
		"x\017  USB support via https://github.com/hathach/tinyusb   \016x", \
		"mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj\017" \
}
#endif

// Default BUZZER implementation
#if !defined(USE_BUZZER) && defined(PIN_BUZZER)
#define USE_BUZZER 1
#endif

// Use HDMI detect pin
#if !defined(USE_HDMI_DETECT) && defined(PIN_HDMI_DETECT)
#define USE_HDMI_DETECT 1
#endif

// Use PS/2 keyboard
#if !defined(USE_PS2_KEYBOARD) && defined(PIN_PS2_DATA) && defined(PIN_PS2_CLOCK)
#define USE_PS2_KEYBOARD 1
#endif

// Use UART
#if !defined(USE_UART) && defined(PIN_UART_TX) && defined(PIN_UART_RX)
#define USE_UART 1
#endif


// Default LED implementation
#if !defined(USE_LED) && defined(PIN_LED)

#define USE_LED 1

// number of LEDs
#ifndef LED_NUM
#define LED_NUM 1
#endif

#ifndef LED_GPIO_TAB
#define LED_GPIO_TAB { {PIN_LED, 0} }
#endif

#endif

// Default key / button implementation
#if !defined(USE_KEY) && defined(PIN_DEFAULTS)

#define USE_KEY 1

// number of buttons
#ifndef KEY_NUM
#define KEY_NUM 1
#endif

// buttons GPIOs
#ifndef KEY_GPIO_TAB
#define KEY_GPIO_TAB { PIN_DEFAULTS }
#endif

#endif

#if USE_HDMI

#include "common_dvi_pin_configs.h"
#include "boards/custom_dvi_pin_configs.h"

#endif

#endif
