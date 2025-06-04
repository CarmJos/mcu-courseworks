#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"
#include "lcd1602.h"

#define KEYS_PORT P3

sbit BEEP = P2 ^ 3;  // 蜂鸣器控制引脚

typedef enum {
    NONE,        // 非设置状态，正常显示
    SET_SECOND,  // 设置秒
    SET_MINUTE,  // 设置分
    SET_HOUR,    // 设置时
    SET_ALARM_SECOND, // 设置闹钟秒
    SET_ALARM_MINUTE, // 设置闹钟分
    SET_ALARM_HOUR,   // 设置闹钟时
    SET_ALARM_ENABLE  // 设置闹钟开关
} SettingMode;

typedef struct {
    int hour;   // 时
    int minute; // 分
    int second; // 秒
} Time;

SettingMode mode = NONE; // 当前设置的位置

Time current_time = { 12, 0, 0 }; // 当前时间，初始为12:00:00

bool alarm_enabled = FALSE; // 闹钟是否启用
Time alarm_time = { 12, 2, 0 }; // 闹钟时间，初始为12:05:00
uchar alarm = 0; // 闹钟剩余次数，为0时表示闹钟无效

char separator = ':'; // 时间分隔符
char line1[16]; // 用于显示的字符串
char line2[16]; // 用于显示的字符串

uint16 input; // 按键输入

uint16 read_keys() {
    uchar i, scan1, scan2, tmp;
    input = 0x0000;

    KEYS_PORT = 0xF0;
    scan1 = KEYS_PORT;
    if (scan1 == 0xF0) return 0;
    delay(1);

    scan1 = KEYS_PORT;
    if (scan1 == 0xF0) return 0;
    delay(1);

    KEYS_PORT = 0xF0;
    scan2 = KEYS_PORT;

    for (i = 0; i < 4; i++) {
        if (!(scan1 & (1 << (4 + i)))) {
            tmp = (~scan2) & 0x0F;
            input |= (tmp << (i * 4));
        }
    }

    return input;
}

bool pressing(uchar x, uchar y) {
    return (input & (1 << (x * 4 + y))) != 0;
}

void display_time() {
    sprintf(
        line1, "    %02d%c%02d%c%02d   %c",
        current_time.hour, separator, current_time.minute, separator, current_time.second,
        (alarm_enabled ? 'A' : 'X')
    );
    sprintf(line2, "                ");
    LCD1602_display(0, 0, line1);
    LCD1602_display(1, 0, line2);
}

void display_timeset() {
    sprintf(line1, "T:  %02d:%02d:%02d    ", current_time.hour, current_time.minute, current_time.second);
    sprintf(line2, "SET             ");
    // 根据当前设置位置显示指示符
    if (mode == SET_HOUR) {
        line2[4] = '^';
        line2[5] = '^';
    } else if (mode == SET_MINUTE) {
        line2[7] = '^';
        line2[8] = '^';
    } else if (mode == SET_SECOND) {
        line2[10] = '^';
        line2[11] = '^';
    }
    LCD1602_display(0, 0, line1);
    LCD1602_display(1, 0, line2);
}

void display_alarm() {
    sprintf(
        line1, "AL: %02d:%02d:%02d   %c",
        alarm_time.hour, alarm_time.minute, alarm_time.second,
        (alarm_enabled ? 'Y' : 'N')
    );
    sprintf(line2, "SET             ");
    // 根据当前设置位置显示指示符      
    if (mode == SET_ALARM_HOUR) {
        line2[4] = '^';
        line2[5] = '^';
    } else if (mode == SET_ALARM_MINUTE) {
        line2[7] = '^';
        line2[8] = '^';
    } else if (mode == SET_ALARM_SECOND) {
        line2[10] = '^';
        line2[11] = '^';
    } else if (mode == SET_ALARM_ENABLE) {
        line2[15] = '^';
    }
    LCD1602_display(0, 0, line1);
    LCD1602_display(1, 0, line2);
}

int main() {

    { // 初始化LCD1602
        LCD1602_initialize(); // Initialize the LCD1602.
        LCD1602_display(0, 2, "SIMPLE CLOCK");
        delay(500); // 等待500ms
        LCD1602_clear();
        delay(200); // 等待200ms
    }

    { // 测试蜂鸣器
        BEEP = 0;
        delay(100);
        BEEP = 1;
        delay(100);
        BEEP = 0;
        delay(100);
        BEEP = 1;
    }

    { // 初始化中断
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
    }

    while (1) {
        read_keys(); // 读取按键输入

        if (pressing(0, 0)) {
            switch (mode) {
            case SET_SECOND:
            case SET_MINUTE:
            case SET_HOUR:
                mode = SET_ALARM_ENABLE;
                display_alarm(); // 显示闹钟设置
                break;
            case SET_ALARM_SECOND:
            case SET_ALARM_MINUTE:
            case SET_ALARM_HOUR:
                mode = NONE; // 返回到时间设置的秒
                display_time(); // 显示当前时间
                break;
            default:
                mode = SET_SECOND;
                display_timeset(); // 显示时间设置
                break;
            }
        } else if (pressing(1, 0)) {
            switch (mode) {
            case SET_HOUR:
            case SET_MINUTE:
            case SET_SECOND:
                mode++;
                if (mode > SET_HOUR) {
                    mode = SET_SECOND; // 回到设置秒
                }
                display_timeset(); // s刷新时间设置
                break;
            case SET_ALARM_SECOND:
            case SET_ALARM_MINUTE:
            case SET_ALARM_HOUR:
                // 切换到下一个闹钟设置
                mode++;
                display_alarm(); // 刷新闹钟设置
                break;
            case SET_ALARM_ENABLE:
                mode = SET_ALARM_SECOND; // 回到闹钟秒设置
                display_alarm(); // 刷新闹钟设置
                break;
            default:
                alarm = 0; // 清除闹钟
                break;
            }
        } else if (pressing(2, 0) || pressing(3, 0)) {
            switch (mode) {
            case SET_SECOND:
                current_time.second = (current_time.second + (pressing(3, 0) ? 59 : 1)) % 60;
                display_timeset();
                break;
            case SET_MINUTE:
                current_time.minute = (current_time.minute + (pressing(3, 0) ? 59 : 1)) % 60;
                display_timeset();
                break;
            case SET_HOUR:
                current_time.hour = (current_time.hour + (pressing(3, 0) ? 23 : 1)) % 24;
                display_timeset();
                break;
            case SET_ALARM_SECOND:
                alarm_time.second = (alarm_time.second + (pressing(3, 0) ? 59 : 1)) % 60;
                display_alarm();
                break;
            case SET_ALARM_MINUTE:
                alarm_time.minute = (alarm_time.minute + (pressing(3, 0) ? 59 : 1)) % 60;
                display_alarm();
                break;
            case SET_ALARM_HOUR:
                alarm_time.hour = (alarm_time.hour + (pressing(3, 0) ? 23 : 1)) % 24;
                display_alarm();
                break;
            case SET_ALARM_ENABLE:
                alarm_enabled = !alarm_enabled; // 切换闹钟启用状态
                display_alarm();
                break;
            default:
                break;
            }
        }
        delay(50); // 防抖延时
    }

}

// 计时器0，控制蜂鸣器闹铃
void counter0() interrupt 1  {
    static uint16 t = 0; // 定义静态变量v，初值为0

    t++; // 每次中断v加1
    if (t < 500) return; // 每1/8秒钟滴一次
    t = 0; // 清零计数器

    if (alarm <= 0) {
        BEEP = 1; // 确保蜂鸣器关闭
        return;
    }

    // 通过 alarm的奇偶性来控制蜂鸣器
    // alarm % 2 == 0 时打开蜂鸣器
    // alarm % 2 == 1 时关闭蜂鸣器
    // 与此同时，为了产生滴滴间隔，
    // 每开关两次蜂鸣器暂停两次

    if (alarm % 2 == 1 || (alarm % 2 == 0 && (alarm / 4) % 2 == 1)) {
        BEEP = 1;
    } else {
        BEEP = 0;
    }

    alarm--; // 闹钟次数减1
}

bool time_equals(Time t1, Time t2) {
    return (t1.hour == t2.hour && t1.minute == t2.minute && t1.second == t2.second);
}

void time_tick(Time* t) {
    t->second++; // 秒加1
    if (t->second < 60) return; // 如果秒小于60，直接返回
    t->second = 0; // 秒归零
    t->minute++; // 分加1
    if (t->minute < 60) return; // 如果分小于60，直接返回
    t->minute = 0; // 分归零
    t->hour++; // 时加1
    if (t->hour < 24) return; // 如果时小于24，直接返回
    t->hour = 0; // 时归零
}


// 计时器1，控制时钟
void counter1() interrupt 3  {
    static uint16 v = 0;

    // 如果在设置时间模式中，不更新秒
    if (mode != NONE) {
        v = 0;
        return;
    }

    v++;
    if (v < 2000) return; // 每0.5s刷新一次
    v = 0;

    separator = (separator == ':') ? ' ' : ':';

    if (separator == ':') {
        time_tick(&current_time); // 更新时间
        if (alarm_enabled && time_equals(current_time, alarm_time)) {     // 检查闹钟
            alarm = 90;
        }
    }

    display_time(); // 刷新显示时间
}