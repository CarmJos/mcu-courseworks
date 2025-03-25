#include "base.h"

#define DISPLAY_PORT P0 // 显示端口
#define BUTTON_PORT P1 // 按键端口
#define LED_PORT P3 // LED 端口

void led_reset(void);
void led_set(uchar status);
void led_stream(bool reverse);
void led_alter(int times);
void led_blink(int times);

uchar read_button() {
    // 从 BUTTON_PORT 读取按键状态，利用delay消除抖动，低电平为按下
    uchar keyboard_status = 0x00;
    BUTTON_PORT = 0xFF;
    if (BUTTON_PORT == 0xFF) return keyboard_status;
    delay(5);
    if (BUTTON_PORT == 0xFF) return keyboard_status;
    keyboard_status = ~BUTTON_PORT;
    return keyboard_status;
}

bool pressed(uchar status, uchar key) {
    return (status >> key) & 1;
}

int main() {
    uchar button;
    uchar status = 0;
    while (1) {
        button = read_button();
        if (pressed(button, 0)) {
            status = status == 3 ? 1 : status + 1;
        }

        DISPLAY_PORT = digit(status, FALSE, TRUE);

        switch (status) {
        case 1:
            led_stream(TRUE);
            break;
        case 2:
            led_stream(FALSE);
            break;
        case 3:
            led_blink(2);
            break;
        }

    }
    return 0;
}

void led_set(uchar status) {
    LED_PORT = ~status;
}

void led_reset(void) {
    led_set(0x00);
}

void led_stream(bool reverse) {
    // 流水点亮一次 LED, reverse 为 true 时，反向流水
    uchar i;
    uchar led = reverse ? 0x01 : 0x80;
    if (reverse) {
        for (i = 0; i < 8; i++) {
            led_set(led);
            led = _crol_(led, 1);
            delay(100);
        }
    } else {
        for (i = 0; i < 8; i++) {
            led_set(led);
            led = _cror_(led, 1);
            delay(100);
        }
    }
    led_reset();
}


void led_blink(int times) {
    // 闪烁交替点亮 8个 LED(0101 0101 -> 1010 1010)
    bool status = TRUE;
    uchar i;
    for (i = 0; i < times; i++) {
        led_set(status ? 0x55 : 0xAA);
        status = !status;
        delay(100);
    }
    led_reset();
}


void led_alter(int times) {
    // 4 4 交替 点亮 8 个LED ( 0000 1111 -> 1111 0000)
    uchar i;
    for (i = 0; i < times; i++) {
        led_set(0x0F);
        delay(100);
        led_set(0xF0);
        delay(100);
    }
    led_reset();
}
