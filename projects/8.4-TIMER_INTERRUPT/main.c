#include "base.h"
#include <reg51.h>
#include "hex_display.h"

int a = 0;
int b = 0;

uchar counter_digit(uchar value, bool remove_zero) {
	return  digit((remove_zero && (value <= 0)) ? -1 : value, !remove_zero, false);
}

int main(void) {

	EX0 = 1;
	EX1 = 1;
	IT0 = 1;
	IT1 = 1;
	TMOD = 0x66;

	TH0 = 0xFF;
	TL0 = 0xFF;
	TH1 = 0xFF;
	TL1 = 0xFF;

	TR0 = 1;
	ET0 = 1; 
	TR1 = 1;
	ET1 = 1;

	EA = 1;

	while (1) {
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
}


void int0() interrupt 0  {
	a = 0;
}

void int1() interrupt 2 {
	b = 0;
}

void counter0() interrupt 1  {
	a++;
}

void counter1() interrupt 3 {
	b++;
}

