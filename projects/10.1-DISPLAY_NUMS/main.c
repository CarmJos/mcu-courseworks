#include "base.h"
#include "hex_display.h"

// P0 控制数码管

#define LINE P0

sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

bool state = 0; // 0 = 学号， 1 = 生日
static uchar SCHOOL_NUMS[8] = { 0,0,0,0,0,0,0,0 };
static uchar BIRTH_DAY[8] = { 0,0,0,0,0,0,0,0 };
static uchar BIT_CODES[8] = {
   0xFE,  0xFD,  0xFB,  0xF7,
   0xEF,  0xDF,  0xBF,  0x7F
};

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
    TH1 = 0xFF;
    TL1 = 0xFF;

    TR0 = 1;
    ET0 = 1;
    TR1 = 1;
    ET1 = 1;

    EA = 1;

    while (1) {
        if (state) {
            display(BIRTH_DAY);
        } else {
            display(SCHOOL_NUMS);
        }
    }

}


void counter0() interrupt 1  {
    state = FALSE;
}

void counter1() interrupt 3 {
    state = TRUE;
}


