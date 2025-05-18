#include "lcd1602.h"

void LCD1602_validate() {
    uint8 status;
    DB = 0xFF;
    do {
        status = 0xFF;
        RS = 0;
        EN = 0;
        RW = 1;
        EN = 1;
        status = DB;
    } while (status & 0x80);
    EN = 0;
}

void LCD1602_execute(uint8 cmd) {
    LCD1602_validate();
    EN = 0;
    RS = 0;
    RW = 0;
    DB = cmd;
    EN = 1;
    _nop_();
    EN = 0;
}

void LCD1602_write(uint8 dat) {
    LCD1602_validate();
    EN = 0;
    RS = 1;
    RW = 0;
    DB = dat;
    EN = 1;
    _nop_();
    EN = 0;
}

void LCD1602_cursor(uint8 x, uint8 y) {
    uint8 addr = (y == 0 ? 0x00 : 0x40) + x; // Address offset
    LCD1602_execute(addr | 0x80);
}

void LCD1602_initialize() {
    delay(15);
    LCD1602_execute(0x38);
    _nop_();
    LCD1602_execute(0x38);
    _nop_();
    LCD1602_execute(0x38);
    LCD1602_execute(0x0C);
    LCD1602_execute(0x06);
    LCD1602_clear();
}

void LCD1602_clear() {
    LCD1602_execute(0x01);
}

void LCD1602_scroll(const RollState state) {
    switch (state) {
    case LEFT:
        LCD1602_execute(0x18);
        break;
    case RIGHT:
        LCD1602_execute(0x1C);
        break;
    default:
        LCD1602_execute(0x06);
        break;
    }
}

void LCD1602_display(uint8 y, uint8 x, uint8* str) {
    LCD1602_cursor(x, y);
    while (*str != '\0') {
        LCD1602_write(*str++);
        delay(20);
    }
}

void LCD1602_mode(bool display, bool cursor, bool blink) {
    uint8 mode = 0x08; // Default mode
    if (display) mode |= 0x04;
    if (cursor) mode |= 0x02;
    if (blink) mode |= 0x01;
    LCD1602_execute(mode);
}
