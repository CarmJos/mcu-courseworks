#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"
#include "lcd1602.h"

sbit BEEP = P2 ^ 3; // 蜂鸣器控制引脚

typedef enum {
    DISPLAY,
    TIME_SETTING,
    ALARM_SETTING,
    ALARM_ENABLE
} ClockMode;

char separator = ':'; // 时间分隔符
int hour, minute, second = 0; // 时分秒

bool alarm_enabled = FALSE; // 闹钟是否启用
int alarm_hour, alarm_minute, alarm_second = 0; // 闹钟时分秒

uchar alarm = 0; // 闹钟剩余次数，为0时表示闹钟无效

bool setting = FALSE; // 设置状态

uchar pad_read() { return P3; }

void pad_write(uchar val) { P3 = val; }

char* get_time() {
    static char time_str[16]; // 格式为 "HH:MM:SS"，分隔符可能是空格或冒号
    sprintf(time_str, "%02d%c%02d%c%02d", hour, separator, minute, separator, second);
    return time_str;
}

int main() {
    static uint16 input;
    static char line1[16]; // 用于显示的字符串

    TMOD = 0x22;

    TH0 = 0x06;
    TL0 = 0x06;
    TH1 = 0x06;
    TL1 = 0x06;

    TR0 = 1;
    ET0 = 1;
    TR1 = 1;
    ET1 = 1;
    EA = 1; // 允许总中断

    LCD1602_initialize(); // Initialize the LCD1602.
    LCD1602_display(0, 2, "SIMPLE CLOCK");
    delay(200); // 等待200ms
    LCD1602_clear();

    while (1) {
        if (!setting) {
            // 非设置模式，正常
            sprintf(line1, "%02d%c%02d%c%02d   %c", hour, separator, minute, separator, second, (alarm_enabled ? 'A' : 'X'));
            LCD1602_display(0, 4, line1);
        }

        input = keypad4_transpose_read(pad_read, pad_write);
    }

}

// 计时器0，控制蜂鸣器闹铃
void counter0() interrupt 1  {
    static uint16 t = 0; // 定义静态变量v，初值为0

    t++; // 每次中断v加1
    if (t < 500) return; // 每0.5秒钟滴一次
    t = 0; // 清零计数器

    // 通过 alarm的奇偶性来控制蜂鸣器
    // alarm % 2 == 0 时打开蜂鸣器
    // alarm % 2 == 1 时关闭蜂鸣器
    // 与此同时，为了产生滴滴间隔，
    // 每开关两次蜂鸣器暂停两次

    if (alarm <= 0) {
        BEEP = 1; // 确保蜂鸣器关闭
        return;
    }

    if (alarm % 2 == 1 || (alarm % 2 == 0 && (alarm / 4) % 2 == 1)) {
        BEEP = 1;
    } else {
        BEEP = 0;
    }

    alarm--; // 闹钟次数减1
}


// 计时器1，控制时钟
void counter1() interrupt 3  {
    static uint16 v = 0; // 定义静态变量count，初值为0
    v++; // 每次中断count加1
    if (v < 4000) return;
    v = 0; // 清零计数器

    separator = (separator == ':') ? ' ' : ':';

    second += 1; // 秒加1
    if (second < 60) return; // 秒未满60，直接返回

    second = 0; // 秒清零
    minute += 1; // 分加1
    if (minute < 60) return; // 分未满60，直接返回

    minute = 0; // 分清零
    hour += 1; // 时加1

    if (hour < 24) return; // 时未满24，直接返回
    hour = 0; // 时清零
}
