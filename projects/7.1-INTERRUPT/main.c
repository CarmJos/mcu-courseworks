#include "base.h"
#include <reg51.h>
#include "led.h"


int main(void) {
	EA = 1;
	EX0 = 1;
	EX1 = 1;
	IT0 = 1;
	IT1 = 1;
	PX0 = 0;
	PX1 = 1;

	while (1) {
		P1 = 0;
	}
}


void int0() interrupt 0 {
	led_blink(0x0F, 5, 200);
}

void int1() interrupt 2 {
	led_blink(0xAA, 5, 200);
}


void led_set(uchar status) {
	P1 = status;
}
