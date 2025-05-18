
#ifndef __LCD1602_H_
#define __LCD1602_H_

#include "base.h"

#define DB P0 // Data bus port

sbit RS = P2 ^ 0; // Register Select pin (Data=1/Command=0)
sbit RW = P2 ^ 1; // Read/Write pin (Read=1/Write=0)
sbit EN = P2 ^ 2; // Enable pin

typedef enum {
    STILL, LEFT, RIGHT
} RollState; // LCD1602 scrolling state

void LCD1602_initialize();
void LCD1602_clear();
void LCD1602_scroll(const RollState state);
void LCD1602_display(uint8 line, uint8 offset, uint8* content);
void LCD1602_mode(bool display, bool cursor, bool blink);

#endif