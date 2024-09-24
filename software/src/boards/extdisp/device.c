#include "pins.h"

#ifdef BOARD_EXTDISP

#include "device.h"
#include "pico/stdlib.h"
#include "serial.h"
#include "extdisp_program.h"
#include "key.h"
#include "led.h"

// Define VT100 control characters
#define BEL      7   // Bell character (activates buzzer)
#define DC1      17  // Device Control 1 (XON)
#define DC2      18  // Device Control 2 (Custom: LED ON)
#define DC3      19  // Device Control 3 (XOFF)
#define DC4      20  // Device Control 4 (Custom: LED OFF)

// Device initialization function
void __in_flash(".device") device_init() {
	// Initialize the LCD Chip Select pin (LCD_CS) as input with pull-up
	gpio_init(LCD_CS);
	gpio_set_function(LCD_CS, GPIO_FUNC_SIO);
	gpio_set_dir(LCD_CS, GPIO_IN);

	// Initialize the LCD Offline pin (LCD_OFFLINE) as input with pull-up
	gpio_init(LCD_OFFLINE);
	gpio_set_function(LCD_OFFLINE, GPIO_FUNC_SIO);
	gpio_set_dir(LCD_OFFLINE, GPIO_IN);
	gpio_pull_up(LCD_OFFLINE);

	// Get the current time to start timing
	absolute_time_t start_time = get_absolute_time();
	absolute_time_t high_start_time = start_time;
	bool pin_low_detected = false;

	// Loop until the maximum wait time is reached
	while (absolute_time_diff_us(start_time, get_absolute_time()) < DC_PIN_MAX_WAIT_MS * 1000) {
		// Check if the LCD_OFFLINE pin is held LOW
		if (gpio_get(LCD_OFFLINE) == 0) {
			if (!pin_low_detected) {
				// Record the time when the pin first goes LOW
				high_start_time = get_absolute_time();
				pin_low_detected = true;
			}
			// If the pin has been LOW for the required duration, break the loop
			if (absolute_time_diff_us(high_start_time, get_absolute_time()) >= DC_PIN_LOW_MS * 1000) {
				break;
			}
		} else {
			// Reset if the pin goes HIGH again
			pin_low_detected = false;
		}
	}

	// If the pin was not held LOW long enough, load the original ExtDisp program
	if (!pin_low_detected) {
		// Re-initialize pins to their default states
		gpio_init(LCD_CS);
		gpio_set_dir(LCD_CS, false);       // Set as input
		gpio_disable_pulls(LCD_CS);        // Disable pull-up/down resistors
		gpio_init(LCD_OFFLINE);
		gpio_set_dir(LCD_OFFLINE, false);  // Set as input
		gpio_disable_pulls(LCD_OFFLINE);   // Disable pull-up/down resistors

		// Copy the ExtDisp program from flash memory to SRAM
		uint8_t *s = (uint8_t *) extdisp_program;     // Source address in flash
		uint8_t *d = (uint8_t *) SRAM_BASE;           // Destination address in SRAM
		uint num = extdisp_program_len;               // Length of the program
		for (; num > 0; num--) *d++ = *s++;

		// Jump to the copied program in SRAM (add 1 for Thumb mode)
		((void (*)(void)) (SRAM_BASE + 1))();

		// Infinite loop (should not reach here)
		while (true) {}
	}
}

// Function called after the device has started
void __in_flash(".device") device_started() {
	// If the serial port is ready, send XON and BEL characters
	if (serial_is_writable()) {
		serial_putc(DC1);  // Send Device Control 1 (resume transmission)
		serial_putc(BEL);  // Send Bell character (activate buzzer)
	}
}

// Function to play a tone using the buzzer
void sound_play_tone(uint16_t frequency, uint16_t duration_ms, uint8_t volume, bool wait) {
	(void) frequency;    // Frequency is ignored
	(void) duration_ms;  // Duration is ignored
	(void) volume;       // Volume is ignored
	if (wait) {
		// Wait until the serial port is writable
		while (!serial_is_writable()) {}
	}
	// Send the BEL character to activate the buzzer
	serial_putc(BEL);
}

// Initialize sound system (no action needed here)
void __in_flash(".device") sound_init() {}

// Function to turn on the LED
void led_on(unsigned char inx) {
	(void) inx;  // LED index is ignored (only one LED)
	if (serial_is_writable()) {
		// Send DC2 to turn the LED on
		serial_putc(DC2);
	}
}

// Function to turn off the LED
void led_off(unsigned char inx) {
	(void) inx;  // LED index is ignored
	if (serial_is_writable()) {
		// Send DC4 to turn the LED off
		serial_putc(DC4);
	}
}

// Initialize LED system (no action needed here)
void __in_flash(".device") led_init() {}

// Empty key functions (no keys on this board)
void __in_flash(".device") key_init() {}
void __in_flash(".device") key_term() {}
char key_get() { return NOKEY; }

// Empty PS2 keyboard functions (no keyboard on this board)
void keyboard_ps2_set_led_status(uint8_t leds) {}
void keyboard_ps2_task() {}
void __in_flash(".device") keyboard_ps2_init() {}
void __in_flash(".device") keyboard_ps2_apply_settings() {}


#endif
