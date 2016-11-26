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

#include "led.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static const struct {
	uint32_t gpioport;
	uint16_t gpios;
    uint8_t  on_value;
} led[] = {
#if defined(BOARD_1BITSY)
    { GPIOA, GPIO8, 0 },
#elif defined(BOARD_STM32F4DISC)
    { GPIOD, GPIO12, 1 },
    { GPIOD, GPIO13, 1 },
    { GPIOD, GPIO14, 1 },
    { GPIOD, GPIO15, 1 },
#else
#error Unrecognized BOARD
#endif
};
#define NUM_LEDS	(sizeof(led) / sizeof(led[0]))

void led_init(void)
{
	for (unsigned i = 0; i < NUM_LEDS; i++) {
        uint32_t port = led[i].gpioport;
        rcc_periph_clock_enable(RCC_GPIOA + ((port - GPIOA) / (GPIOB - GPIOA)));
		gpio_mode_setup(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, led[i].gpios);
        led_off(i);
	}
}

void led_on(unsigned led_num) {
    if (led_num < NUM_LEDS) {
        if (led[led_num].on_value) {
            gpio_set(led[led_num].gpioport, led[led_num].gpios);
        } else {
            gpio_clear(led[led_num].gpioport, led[led_num].gpios);
        }
    }
}

void led_off(unsigned led_num) {
    if (led_num < NUM_LEDS) {
        if (led[led_num].on_value) {
            gpio_clear(led[led_num].gpioport, led[led_num].gpios);
        } else {
            gpio_set(led[led_num].gpioport, led[led_num].gpios);
        }
    }
}

void led_toggle(unsigned led_num) {
    if (led_num < NUM_LEDS) {
        gpio_toggle(led[led_num].gpioport, led[led_num].gpios);
    }
}
