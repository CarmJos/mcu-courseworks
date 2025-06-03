#include "base.h"
#include "hex_display.h"
#include "matrix4_keyspad.h"

// P0 控制数码管

#define LINE P0

sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

static uchar BIT_CODES[8] = {
   0xFE,   0xFD,  0xFB,  0xF7,
   0xEF,  0xDF,  0xBF,   0x7F
};
static uchar NUMS[8] = { 0,0,0,0,0,0,0,0 };

void display(uchar* val) {
    static int i = 0;
    for (i = 0; i < 8; i++) {
        LINE = BIT_CODES[i];
        LATCH_BIT = 1;
        LATCH_BIT = 0;

        LINE = digit(val[i], FALSE, FALSE);
        LATCH_SEG = 1;
        LATCH_SEG = 0;

        delay(2);
    }
}

void write(uchar val) {
    NUMS[7] = val % 10; // 个位
    NUMS[6] = val / 10; // 十位
    NUMS[5] = val / 100; // 百位
}
uchar pad_read() {
    return P3;
}

void pad_write(uchar val) {
    P3 = val; // 写入P3端口
}

int main() {
    static uint16 input;
    static uchar i, x, y;
    while (1) {
        display(NUMS);

        input = keypad4_transpose_read(pad_read, pad_write);

        for (i = 0; i < 16; i++) {
            if (input & (1 << i)) {
                x = i / 4; // 行
                y = i % 4; // 列
                write(x + y * 4 + 1);
                break;
            }

        }
    }


}
