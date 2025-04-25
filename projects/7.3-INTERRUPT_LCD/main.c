#include "base.h"
#include "STDIO.H"
#include <reg51.h>
#include "lcd1602.h"
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

int main(void) {
	uchar state;
	char line1[16], line2[16];

	EA = 1;
	EX0 = 1;
	EX1 = 1;
	IT0 = 1;
	IT1 = 1;

	LCD1602_initialize();

	while (1) {
		state = button_read(button_getter, button_setter);
		if (state & 0x10) {
			a = 0;
		}
		if (state & 0x20) {
			b = 0;
		}

		sprintf(line1, "A: %03d", a);
		sprintf(line2, "B: %03d", b);
		LCD1602_display(0, 0, line1);
		LCD1602_display(1, 0, line2);
	}
}


void int0() interrupt 0 using 1 {
	a++;
}

void int1() interrupt 2 using 2{
	b++;
}
