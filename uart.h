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

#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include <stdint.h>

void uart_init(void);
void uart_printf(const char *fmt, ...);

void uart_send_byte(uint8_t ch);
void uart_send_strn(const char *str, size_t len);
void uart_send_strn_cooked(const char *str, size_t len);


#endif  // UART_H
