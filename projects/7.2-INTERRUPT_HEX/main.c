#include "base.h"
#include <reg51.h>
#include "hex_display.h"
#include "button.h"

int a = 0;
int b = 0;

uchar button_getter() {
	return P3;
}

uchar button_setter(uchar value) {
	P3 = value;
	return P3;
}

uchar counter_digit(uchar value, bool remove_zero) {
	return  digit((remove_zero && (value <= 0)) ? -1 : value, !remove_zero, false);
}

void update_display() {
	uchar display[6] = { 0 };
	uchar i = 0;
	uchar w = 0x01;

	display[0] = counter_digit(a / 100, true);
	display[1] = counter_digit((a / 10) % 10, true);
	display[2] = counter_digit(a % 10, false);
	display[3] = counter_digit(b / 100, true);
	display[4] = counter_digit((b / 10) % 10, true);
	display[5] = counter_digit(b % 10, false);

	for (i = 0; i < 6; i++) {
		P2 = ~w;
		P0 = display[i];
		delay(5);
		w <<= 1;
	}
	P0 = 0x00;
}

int main(void) {
	uchar state, j;

	EA = 1;
	EX0 = 1;
	EX1 = 1;
	IT0 = 1;
	IT1 = 1;

	while (1) {
		state = button_read(button_getter, button_setter);
		if (state & 0x10) {
			a = 0;
		}
		if (state & 0x20) {
			b = 0;
		}
		update_display();
	}
}


void int0() interrupt 0 using 1 {
	a++;
}

void int1() interrupt 2 using 2{
	b++;
}
