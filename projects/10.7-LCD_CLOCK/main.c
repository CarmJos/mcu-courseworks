#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"
#include "lcd1602.h"

sbit BEEP = P2 ^ 3; // 蜂鸣器控制引脚

typedef enum {
    DISPLAY, // 显示模式
    TIME_SETTING, // 时间设置模式
    ALARM_SETTING // 闹钟设置模式
} ClockMode;

ClockMode mode = DISPLAY; // 时钟模式，初始为显示模式

typedef enum {
    SET_SECOND,  // 设置秒
    SET_MINUTE,  // 设置分
    SET_HOUR,    // 设置时
    SET_ALARM_SECOND, // 设置闹钟秒
    SET_ALARM_MINUTE, // 设置闹钟分
    SET_ALARM_HOUR,   // 设置闹钟时
    SET_ALARM_ENABLE  // 设置闹钟开关
} SettingMode;

SettingMode setting_mode = SET_SECOND; // 当前设置的位置

char separator = ':'; // 时间分隔符
int hour, minute, second = 0; // 时分秒

bool alarm_enabled = FALSE; // 闹钟是否启用
int alarm_hour, alarm_minute, alarm_second = 0; // 闹钟时分秒

uchar alarm = 0; // 闹钟剩余次数，为0时表示闹钟无效

uint16 input; // 按键输入

uint16 read_keys();

inline bool pressed(uchar x, uchar y) {
    return (input & (1 << (x * 4 + y))) != 0;
}

int main() {
    static char line1[16]; // 用于显示的字符串
    static char line2[16]; // 用于显示的字符串

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

    { // 初始化LCD1602
        LCD1602_initialize(); // Initialize the LCD1602.
        LCD1602_display(0, 2, "SIMPLE CLOCK");
        delay(200); // 等待200ms
        LCD1602_clear();
    }

    while (1) {
        read_keys(); // 读取按键输入

        switch (mode) {
        case TIME_SETTING:
            sprintf(line1, "    %02d%c%02d%c%02d    ", hour, separator, minute, separator, second);
            memset(line2, ' ', 16);
            
            // 根据当前设置位置显示指示符
            if (setting_mode == SET_HOUR) {
                line2[4] = '^';
                line2[5] = '^';
            } else if (setting_mode == SET_MINUTE) {
                line2[7] = '^';
                line2[8] = '^';
            } else if (setting_mode == SET_SECOND) {
                line2[10] = '^';
                line2[11] = '^';
            }
            
            // 处理按键
            if (pressed(0, 1)) { // 切换设置位置
                setting_mode++;
                if (setting_mode > SET_HOUR) {
                    setting_mode = SET_SECOND;
                }
            } else if (pressed(0, 2)) { // 增加值
                switch (setting_mode) {
                    case SET_SECOND:
                        second = (second + 1) % 60;
                        break;
                    case SET_MINUTE:
                        minute = (minute + 1) % 60;
                        break;
                    case SET_HOUR:
                        hour = (hour + 1) % 24;
                        break;
                }
            } else if (pressed(0, 3)) { // 减少值
                switch (setting_mode) {
                    case SET_SECOND:
                        second = (second + 59) % 60;
                        break;
                    case SET_MINUTE:
                        minute = (minute + 59) % 60;
                        break;
                    case SET_HOUR:
                        hour = (hour + 23) % 24;
                        break;
                }
            } else if (pressed(0, 0)) { // 进入闹钟设置模式
                mode = ALARM_SETTING;
                setting_mode = SET_ALARM_SECOND;
            }
            break;
            
        case ALARM_SETTING:
            sprintf(line1, "AL: %02d%c%02d%c%02d %c", alarm_hour, separator, alarm_minute, separator, alarm_second, alarm_enabled ? 'Y' : 'N');
            memset(line2, ' ', 16);
            
            // 根据当前设置位置显示指示符
            if (setting_mode == SET_ALARM_HOUR) {
                line2[4] = '^';
                line2[5] = '^';
            } else if (setting_mode == SET_ALARM_MINUTE) {
                line2[7] = '^';
                line2[8] = '^';
            } else if (setting_mode == SET_ALARM_SECOND) {
                line2[10] = '^';
                line2[11] = '^';
            } else if (setting_mode == SET_ALARM_ENABLE) {
                line2[13] = '^';
            }
            
            // 处理按键
            if (pressed(0, 1)) { // 切换设置位置
                setting_mode++;
                if (setting_mode > SET_ALARM_ENABLE) {
                    setting_mode = SET_ALARM_SECOND;
                }
            } else if (pressed(0, 2)) { // 增加值
                switch (setting_mode) {
                    case SET_ALARM_SECOND:
                        alarm_second = (alarm_second + 1) % 60;
                        break;
                    case SET_ALARM_MINUTE:
                        alarm_minute = (alarm_minute + 1) % 60;
                        break;
                    case SET_ALARM_HOUR:
                        alarm_hour = (alarm_hour + 1) % 24;
                        break;
                    case SET_ALARM_ENABLE:
                        alarm_enabled = !alarm_enabled;
                        break;
                }
            } else if (pressed(0, 3)) { // 减少值
                switch (setting_mode) {
                    case SET_ALARM_SECOND:
                        alarm_second = (alarm_second + 59) % 60;
                        break;
                    case SET_ALARM_MINUTE:
                        alarm_minute = (alarm_minute + 59) % 60;
                        break;
                    case SET_ALARM_HOUR:
                        alarm_hour = (alarm_hour + 23) % 24;
                        break;
                    case SET_ALARM_ENABLE:
                        alarm_enabled = !alarm_enabled;
                        break;
                }
            } else if (pressed(0, 0)) { // 返回显示模式
                mode = DISPLAY;
                setting_mode = SET_SECOND;
            }
            break;
            
        default:
            sprintf(line1, "    %02d%c%02d%c%02d   %c", hour, separator, minute, separator, second, (alarm_enabled ? 'A' : 'X'));

            if (pressed(0, 1)) {
                alarm = 0; // 按下按键0,1，清除闹钟
            } else if (pressed(0, 0)) {
                mode = TIME_SETTING; // 进入时间设置模式
                setting_mode = SET_SECOND;
            }
            break;
        }

        LCD1602_display(0, 0, line1);
        LCD1602_display(1, 0, line2);
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

    second++; // 秒加1
    if (second >= 60) {
        second = 0; // 秒归零
        minute++; // 分加1
        if (minute >= 60) {
            minute = 0; // 分归零
            hour++; // 时加1
            if (hour >= 24) {
                hour = 0; // 时归零
            }
        }
    }

    // 检查闹钟
    if (alarm_enabled && hour == alarm_hour && minute == alarm_minute && second == alarm_second) {
        alarm = 200; // 设置闹钟计数为200次
    }
}



uchar pad_read() { return P3; }

void pad_write(uchar val) { P3 = val; }

uint16 read_keys() {
    input = keypad4_transpose_read(pad_read, pad_write);
    return input;
}