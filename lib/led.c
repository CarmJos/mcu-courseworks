#include "led.h"

void led_reset(void) {
    led_set(0x00);
}

void led_stream(const bool reverse, const  int ticks) {
    uchar i;
    uchar led = reverse ? 0x01 : 0x80;
    uchar(*p)(int x, int  y) = reverse ? _crol_ : _cror_;
    for (i = 0; i < 8; i++) {
        led_set(led);
        led = p(led, 1);
        delay(ticks);
    }
    led_reset();
}


void led_blink(const uchar start, const int times, const int ticks) {
    bool status = TRUE;
    uchar i;
    for (i = 0; i < times; i++) {
        led_set(status ? start : ~start);
        status = !status;
        delay(ticks);
    }
    led_reset();
}