/*
 * Copyright (C) 2016 Dave Hylands <dhylands@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "uart.h"

#include <stdarg.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "StrPrintf.h"

void uart_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);

	/* Setup USART2 parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART2);

    // Setup USART2 Tx on A2
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
}

static int uart_putc(void *out_param, int ch) {
	(void)out_param;
	if (ch == '\n') {
		usart_send_blocking(USART2, '\r');
	}
	usart_send_blocking(USART2, ch);
	return 1;
}

void uart_printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vStrXPrintf(uart_putc, NULL, fmt, args);
	va_end(args);
}

void uart_send_byte(uint8_t ch) {
    usart_send_blocking(USART2, ch);
}

void uart_send_strn(const char *str, size_t len) {
	for (const char *end = str + len; str < end; str++) {
		uart_send_byte(*str);
	}
}

void uart_send_strn_cooked(const char *str, size_t len) {
	for (const char *end = str + len; str < end; str++) {
		if (*str == '\n') {
			uart_send_byte('\r');
		}
		uart_send_byte(*str);
	}
}

