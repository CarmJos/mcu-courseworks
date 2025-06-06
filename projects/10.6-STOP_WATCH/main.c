#include "base.h"
#include "hex_display.h"
#include "matrix4_keyspad.h"

// P0 控制数码管

#define LINE P0
#define LENGTH 8

sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

static uchar BIT_CODES[8] = {
   0xFE,   0xFD,  0xFB,  0xF7,
   0xEF,  0xDF,  0xBF,   0x7F
};

static uchar NUMS[LENGTH] = { 0,0,0,0,0,0,0,0 }; // 数码管显示内容

static bool pause = TRUE; // 暂停状态
static int count = 59; // 秒计数

void write(int val) {
    NUMS[7] = count % 10; // 个位
    NUMS[6] = (count / 10) % 10; // 十位
    NUMS[5] = 99; // 百位
    NUMS[4] = 99; // 千位
    NUMS[3] = 99; // 万位
    NUMS[2] = 99; // 十万位
    NUMS[1] = 99; // 百万位
    NUMS[0] = 99; // 千万位
}

void display(uchar* val) {
    static int i = 0;
    for (i = 0; i < LENGTH; i++) {
        LINE = BIT_CODES[i];
        LATCH_BIT = 1;
        LATCH_BIT = 0;

        LINE = digit(val[i], FALSE, FALSE);
        LATCH_SEG = 1;
        LATCH_SEG = 0;

        delay(1);
    }
}

uchar pad_read() {
    return P3;
}

void pad_write(uchar val) {
    P3 = val; // 写入P3端口
}

int main() {
    static uint16 input;

    TMOD = 0x20; // 定时器1工作在模式2，8位自动重装载

    // 0.25ms 定时器初值计算
    //（2^8-X）× 12 ÷ 12=5000
    // X = 256 - (250 * 12 / 12) = 6

    TH1 = 0x06; // 定时器初值
    TL1 = 0x06; // 定时器初值

    TR1 = 1; // 启动定时器1
    ET1 = 1; // 允许定时器1中断
    EA = 1; // 允许总中断

    while (1) {
        write(count); // 更新显示内容
        display(NUMS); // 显示内容

        input = keypad4_transpose_read(pad_read, pad_write);

        if (input & 0x01) { // 按下启动
            pause = FALSE;
        }
        if (input & 0x02) { // 按下暂停
            pause = TRUE;
        }
        if (input & 0x04) { // 按下清零
            pause = TRUE; // 暂停
            count = 59; // 重置计数
        }
    }

}


void counter1() interrupt 3  {
    static uint16 v = 0; // 定义静态变量count，初值为0

    v++; // 每次中断count加1
    if (v >= 4000) {
        v = 0; // 清零计数器
        if (!pause && count > 0) {
            count--;
        }
    }
}
