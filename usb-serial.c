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

#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/rcc.h>

#include "button_boot.h"
#include "led.h"
#include "systick.h"
#include "uart.h"
#include "usb.h"

int main(void)
{
#if defined(BOARD_1BITSY)
	// button_boot checks to see if the USER button is pushed during powerup
	// and if so, reboots into DFU mode.
	button_boot();
	rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
#elif defined(BOARD_STM32F4DISC)
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
#else
#error Unrecognized BOARD
#endif

	led_init();
	systick_init();
	uart_init();
	usb_vcp_init();

	uart_printf("\n*****\n");
	uart_printf("***** Starting (UART) ...\n");
	uart_printf("*****\n");

	usb_vcp_printf("\n*****\n");
	usb_vcp_printf("***** Starting (USB) ...\n");
	usb_vcp_printf("*****\n");


	uint32_t last_millis = system_millis;
	uint32_t blink = 0;

	while (1) {

		char buf[128];
		size_t len = 0;
		while (1) {
			if (len >= sizeof(buf)) {
				break;
			}
			if (usb_vcp_avail()) {
				char ch = usb_vcp_recv_byte();
				usb_vcp_send_byte(ch);
				if (ch == '\r') {
					usb_vcp_send_byte('\n');
				}
				if (ch == '\r' || ch == '\n') {
					break;
				}
				buf[len++] = ch;
			}

			if (system_millis - last_millis > 100) {
				if (blink <= 3) {
					led_toggle(0);
				}
				blink = (blink + 1) % 10;
				last_millis = system_millis;
			}
			__WFI();
		}
		uart_send_strn("Line: ", 6);
		uart_send_strn(buf, len);
		uart_send_strn("\r\n", 2);

		usb_vcp_send_strn("Line: ", 6);
		usb_vcp_send_strn(buf, len);
		usb_vcp_send_strn("\r\n", 2);
	}
}
