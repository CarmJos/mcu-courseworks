#include "base.h"
#include "led.h"

void led_set(uchar val){
	P1 = val;
}

int main(void) {
	// led_blink(0XAA,5,100);
	led_stream(FALSE, 100);
	return 0;
}

