/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 *  This file is ported from the esp8266 core for Arduino environment
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "noduino.h"
#include "uart.h"

/////////////////////////////////////////////////////////////////////
static uart_t *uart0 = NULL;
static uart_t *uart1 = NULL;

static int _peek_char = -1;
static int _uart_nr = 0;

irom uart_t * __serial_begin(int uart_nr, unsigned long baud)
{
	_uart_nr = uart_nr;
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    uart_t *_uart = __uart_init(_uart_nr, baud, SERIAL_8N1, SERIAL_FULL, 1);
    _peek_char = -1;
	return _uart;
}

irom void __serial_end(uart_t *uxt)
{
    if(uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
    }

    uart_uninit(uxt);
    uxt = NULL;
}

/* serial1 can not rx data, just tx for debug */
irom int serial_available()
{
	uart_t *_uart = uart0;
    if(!_uart || !uart_rx_enabled(_uart)) {
        return 0;
    }

    int result = uart_rx_available(_uart);
    if (_peek_char != -1) {
        result += 1;
    }
    if (!result) {
        optimistic_yield(10000);
    }
    return result;
}

/* serial1 can not rx data, just tx for debug */
irom uint8_t serial_read()
{
	uart_t *_uart = uart0;
    if(!_uart || !uart_rx_enabled(_uart)) {
        return -1;
    }

    if (_peek_char != -1) {
        uint8_t tmp = _peek_char;
        _peek_char = -1;
        return tmp;
    }
    return uart_read_char(_uart);
}

irom uint8_t __serial_availableForWrite(uart_t *_uart)
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return 0;
    }

    return uart_tx_free(_uart);
}

irom void __serial_flush(uart_t *_uart)
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return;
    }

    uart_wait_tx_empty(_uart);
}

irom size_t __serial_write(uart_t *_uart, char c)
{
    if(!_uart || !uart_tx_enabled(_uart)) {
        return 0;
    }

    uart_write_char(_uart, c);
    return 1;
}

irom void serial_begin(unsigned long baud)
{
	uart0 = __serial_begin(UART0, baud);
}

irom void serial1_begin(unsigned long baud)
{
	uart1 = __serial_begin(UART1, baud);
}

irom void serial_end()
{
	__serial_end(uart0);
}

irom void serial1_end()
{
	__serial_end(uart1);
}

irom uint8_t serial_availableForWrite()
{
	return __serial_availableForWrite(uart0);
}

irom uint8_t serial1_availableForWrite()
{
	return __serial_availableForWrite(uart1);
}

irom void serial_flush()
{
	__serial_flush(uart0);
}

irom void serial1_flush()
{
	__serial_flush(uart1);
}

irom size_t serial_write(char c)
{
	return __serial_write(uart0, c);
}

irom size_t serial1_write(char c)
{
	return __serial_write(uart1, c);
}

irom void serial_print(const char *buf)
{
	while(*buf != '\0') {
		serial_write(*buf);
		buf++;
	}
}

irom void serial1_print(const char *buf)
{
	while(*buf != '\0') {
		serial1_write(*buf);
		buf++;
	}
}
