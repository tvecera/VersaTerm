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

#ifndef SERIAL_UART
#define SERIAL_UART

float serial_uart_get_baudrate();
void serial_uart_set_baudrate(unsigned long baudrate);
void serial_uart_set_break(bool set);
void serial_uart_send_char(char c);
void serial_uart_send_string(const char *s);
bool serial_uart_readable();
int  serial_uart_can_send();

void serial_uart_task(bool processInput);
void serial_uart_apply_settings();
void serial_uart_init();

bool serial_uart_is_writable();
bool serial_uart_is_readable();

void serial_uart_write_blocking(const unsigned char *src, unsigned int len);
char serial_uart_getc();
void serial_uart_putc(unsigned char c);

#endif
