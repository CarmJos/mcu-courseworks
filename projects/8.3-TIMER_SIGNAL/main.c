#include "base.h"

int main(void) {


	TMOD = 0x20;

	// [T1] 定时器单次定时器中断
	// X = 256 - (250 * 12 / 12) = 6
	TH1 = 0x06;
	TL1 = 0x06;

	TR1 = 1;
	ET1 = 1;
	EA = 1;

	while (1) {
	}
}

void timer1(void) interrupt 3 {
	static uint16 count = 0;

	count++;
	if (count >= 240) {
		count = 0;
		P1 ^= 0x01;
	}
}