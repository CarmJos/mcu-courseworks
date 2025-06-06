#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"

#define KEYS_PORT P3

sbit BEEP = P2 ^ 3;  // 蜂鸣器控制引脚

bool enabled = FALSE; // 闹钟是否启用
uchar alarm = 0; // 闹钟剩余次数，为0时表示闹钟无效

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
int main() {


    { // 测试蜂鸣器
        BEEP = 0;
        delay(100);
        BEEP = 1;
    }

    { // 初始化中断
        TMOD = 0x22;

        TH0 = 0x06;
        TL0 = 0x06;

        TR0 = 1;
        ET0 = 1;
        EA = 1; // 允许总中断
    }

    while (1) {
        read_keys(); // 读取按键输入

        if (pressing(0, 0)) {
            if (alarm > 0) {
                alarm = 0;
            } else {
                enabled = !enabled; // 切换闹钟启用状态
            }
        } else if (pressing(1, 0)) {
            alarm = 20;
        }
        delay(50); // 防抖延时
    }

}

// 计时器0，控制蜂鸣器闹铃
void counter0() interrupt 1  {
    static uint16 t = 0; // 定义静态变量v，初值为0

    if (alarm <= 0) {
        BEEP = 1; // 确保蜂鸣器关闭
        t = 0; // 清零计数器
        return;
    }

    t++; // 每次中断v加1
    if (t < 1000) return; // 每1/4秒钟滴一次
    t = 0; // 清零计数器

    if (alarm % 2 == 1) {
        BEEP = 1;
    } else {
        BEEP = 0;
    }

    alarm--; // 闹钟次数减1
}