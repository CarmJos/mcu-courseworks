#include "base.h"
#include "hex_display.h"

// P0 控制数码管

#define LINE P0

sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

static uchar BIT_CODES[8] = {
   0xFE,   0xFD,  0xFB,  0xF7,
   0xEF,  0xDF,  0xBF,   0x7F
};


static uchar NUMS[8] = { 0,0,0,0,0,0,0,0 };
static int counter = 0;

void display(uchar* val) {
    static int i = 0;
    for (i = 0; i < 8; i++) {
        LINE = BIT_CODES[i];
        LATCH_BIT = 1;
        LATCH_BIT = 0;

        LINE = digit(val[i], FALSE, FALSE);
        LATCH_SEG = 1;
        LATCH_SEG = 0;

        delay(1);
    }
}

int main() {

    TMOD = 0x66;

    TH0 = 0xFF;
    TL0 = 0xFF;

    TR0 = 1;
    ET0 = 1;

    EA = 1;

    while (1) {
        NUMS[7] = counter % 10; // 个位
        NUMS[6] = (counter / 10) % 10; // 十位
        NUMS[5] = (counter / 100) % 10; // 百位
        NUMS[4] = (counter / 1000) % 10; // 千位
        NUMS[3] = (counter / 10000) % 10; // 万位
        NUMS[2] = (counter / 100000) % 10; // 十万位
        NUMS[1] = (counter / 1000000) % 10; // 百万位
        NUMS[0] = (counter / 10000000) % 10; // 千万位

        display(NUMS);
    }

}


void counter0() interrupt 1  {
    counter++;
}
