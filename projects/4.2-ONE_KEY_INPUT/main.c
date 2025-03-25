#include "base.h"
#include "button.h"
#include "led.h"

#define DISPLAY_PORT P0 // 显示端口
#define BUTTON_PORT P1 // 按键端口
#define LED_PORT P3 // LED 端口

void button_set(uchar status) {
    BUTTON_PORT = status;
}

uchar button_get() {
    return BUTTON_PORT;
}

void led_set(uchar status) {
    LED_PORT = ~status;
}

int main() {
    uchar button;
    uchar state = 0;
    while (1) {
        button = button_read(&button_set, &button_get);
        if (button_pressed(button, 0)) {
            state = state == 3 ? 1 : state + 1;
        }

        DISPLAY_PORT = digit(state, FALSE, TRUE);

        switch (state) {
        case 1:
            led_stream(TRUE, 100);
            break;
        case 2:
            led_stream(FALSE, 100);
            break;
        case 3:
            led_blink(0x55, 100, 100);
            break;
        }

    }
    return 0;
}