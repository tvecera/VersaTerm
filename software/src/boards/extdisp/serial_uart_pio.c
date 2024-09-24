// -----------------------------------------------------------------------------
// VersaTerm - A versatile serial terminal
// Copyright (C) 2022 David Hansel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------------------

#include "pins.h"

#ifdef BOARD_EXTDISP

#include "uart_pico.pio.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"

#include "serial_uart.h"
#include "serial_cdc.h"
#include "config.h"
#include "terminal.h"
#include "led.h"

#define XON  17
#define XOFF 19

// extended TX FIFO (512 bytes) f that is not affected by disabling the UART fifos
static queue_t uart_tx_queue;

// RX FIFO used for Xon/Xoff flow control
static queue_t uart_rx_queue;

// timeout when to turn off blink LED
static absolute_time_t offtime = 0;

// PIO
static uint state_machine_tx = 0;
static uint state_machine_rx = 0;

static void blink_led(uint16_t ms) {
	if (ms > 0) {
		led_on(LED1);
		offtime = make_timeout_time_ms(ms);
	}
}

bool serial_uart_is_writable() {
	return !pio_sm_is_tx_fifo_full(UART_PIO, state_machine_tx) && gpio_get(LCD_CS) != 0;
}

bool serial_uart_is_readable() {
	bool result = false;

	// Critical section
	if (!pio_sm_is_rx_fifo_empty(UART_PIO, state_machine_rx)) {
		if (gpio_get(LCD_CS) != 0) {
			result = true;
		} else {
			// ignore char
			unsigned char ch = 0;
			uart_rx_program_getc(UART_PIO, state_machine_rx, &ch);
		}
	}

	return result;
}

char serial_uart_getc() {
	unsigned char ch = 0;

	if (serial_uart_is_readable()) {
		uart_rx_program_getc(UART_PIO, state_machine_rx, &ch);
		blink_led(config_get_serial_blink());
	}
	return ch;
}

void serial_uart_putc(unsigned char c) {
	uart_tx_program_putc(UART_PIO, state_machine_tx, c);
}

void serial_uart_write_blocking(const unsigned char *src, unsigned int len) {
	for (size_t i = 0; i < len; ++i) {
		while (!serial_uart_is_writable())
			tight_loop_contents();
		serial_uart_putc(*src++);
	}
}

void serial_uart_set_baudrate(unsigned long baudrate) {
	uart_tx_program_set_baudrate(UART_PIO, state_machine_tx, baudrate);
	uart_rx_program_set_baudrate(UART_PIO, state_machine_rx, baudrate);
}

float serial_uart_get_baudrate() {
	uint32_t clkdiv_fixed = UART_PIO->sm[state_machine_rx].clkdiv;
	float clkdiv = (float) clkdiv_fixed / (1 << 16);

	return (float) clock_get_hz(clk_sys) / (clkdiv * 8);
}

void serial_uart_set_break(bool set) {

}

int serial_uart_can_send() {
	return (int)(512 - queue_get_level(&uart_tx_queue));
}

void serial_uart_send_char(char c) {
	if (serial_uart_is_writable() && queue_is_empty(&uart_tx_queue)) {
		blink_led(config_get_serial_blink());
		serial_uart_putc(c);
	} else
		queue_try_add(&uart_tx_queue, (uint8_t *) &c);
}

void serial_uart_send_string(const char *s) {
	while (*s) serial_uart_send_char(*s++);
}

bool serial_uart_receive_char(uint8_t *b) {
	bool res = false;

	if (config_get_serial_xonxoff()) {
		// if xon/xoff is enabled then we maintain our own RX queue
		// so we can react faster to XON/XOFF requests.
		res = queue_try_remove(&uart_rx_queue, b);
	} else {
		// if xon/xoff is disabled then we use the built-in
		// UART RX buffer for better performance
		if (serial_uart_is_readable()) {
			*b = serial_uart_getc();
			res = true;
		}
	}

	return res;
}

bool serial_uart_readable() {
	return config_get_serial_xonxoff() ? !queue_is_empty(&uart_rx_queue) : serial_uart_is_readable();
}

void serial_uart_apply_settings() {
	if (pio_can_add_program(UART_PIO, &uart_rx_program)) {
		state_machine_rx = pio_claim_unused_sm(UART_PIO, true);
		uint offset_rx = pio_add_program(UART_PIO, &uart_rx_program);
		uart_rx_program_init(UART_PIO, state_machine_rx, offset_rx, PIO_UART_RX, LCD_CS, config_get_serial_baud());
		pio_sm_clear_fifos(UART_PIO, state_machine_rx);
	} else {
		panic("Can't add Serial PIO RX program");
	}
	if (pio_can_add_program(UART_PIO, &uart_tx_program)) {
		state_machine_tx = pio_claim_unused_sm(UART_PIO, true);
		uint offset_tx = pio_add_program(UART_PIO, &uart_tx_program);
		uart_tx_program_init(UART_PIO, state_machine_tx, offset_tx, PIO_UART_TX, LCD_CS, config_get_serial_baud());
		pio_sm_clear_fifos(UART_PIO, state_machine_tx);
	} else {
		panic("Can't add Serial PIO TX program");
	}
}

void serial_uart_task(bool processInput) {
	static bool isxon = true;
	uint8_t b = 0;

	if (!queue_is_empty(&uart_tx_queue) && serial_uart_is_writable()) {
		if (queue_try_remove(&uart_tx_queue, &b)) {
			blink_led(config_get_serial_blink());
			serial_uart_putc(b);
		}
	}

	// handle XON/XOFF flow control
	if (config_get_serial_xonxoff() > 0) {
		// if xon/xoff is enabled then we maintain our own RX queue
		// so we can react faster to XON/XOFF requests.
		if (serial_uart_is_readable()) {
			b = serial_uart_getc();
			if (b != XON && b != XOFF) {
				queue_try_add(&uart_rx_queue, &b);

				// send XOFF if our receive queue is almost full
				if (queue_get_level(&uart_rx_queue) > 20 && isxon) {
					serial_uart_putc(XOFF);
					isxon = false;
				}
			}
		} else if (queue_get_level(&uart_rx_queue) < 12 && !isxon) {
			// send XON if our receive queue is emptying again
			serial_uart_putc(XON);
			isxon = true;
		}
	}

	// handle LED flashing
	if (offtime > 0 && get_absolute_time() >= offtime) {
		offtime = 0;
		led_off(LED1);
	}

	// handle serial input
	if (processInput && serial_uart_receive_char(&b)) {
		switch (config_get_usb_cdcmode()) {
			case 0: // disabled
			case 1: // regular serial
				terminal_receive_char(b);
				break;

			case 2: // pass-through
				terminal_receive_char(b);
				serial_cdc_send_char(b);
				break;

			case 3: // pass-through (terminal disabled)
				serial_cdc_send_char(b);
				break;
		}
	}
}

void serial_uart_init() {
	offtime = 0;

	queue_init(&uart_tx_queue, 1, 512);
	queue_init(&uart_rx_queue, 1, 32);
	serial_uart_apply_settings();

	blink_led(1000);
}

#endif
