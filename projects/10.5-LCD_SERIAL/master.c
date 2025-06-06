#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"
#include "lcd1602.h"

#define MAX_PASSWORD_LENGTH 8
#define KEYS_PORT P3

sbit TX = P3 ^ 1;  // 串行发送引脚

typedef enum {
    STANDBY, IDEL
} WorkMode;

WorkMode mode = STANDBY;

uchar password[MAX_PASSWORD_LENGTH + 1] = { 0 }; // +1 用于存储结束符
int password_length = 0;

char line1[16];
char line2[16];

uint16 input;

uchar pad_read() {
    return P3;
}

void pad_write(uchar val) {
    P3 = val; // 写入P3端口
}

bool pressing(int x, int y) {
    return (input & (1 << (x * 4 + y))) != 0;
}

void clear_password() {
    static uchar i;
    password_length = 0;
    for (i = 0; i < MAX_PASSWORD_LENGTH; i++) {
        password[i] = ' ';
    }
    password[MAX_PASSWORD_LENGTH] = '\0'; // 添加字符串结束符
}

// 获取按下的数字
char get_pressed_number() {
    if (pressing(0, 0)) return '7';
    else if (pressing(1, 0)) return '8';
    else  if (pressing(2, 0)) return '9';
    else  if (pressing(0, 1)) return '4';
    else  if (pressing(1, 1)) return '5';
    else  if (pressing(2, 1)) return '6';
    else  if (pressing(0, 2)) return '1';
    else  if (pressing(1, 2)) return '2';
    else  if (pressing(2, 2)) return '3';
    else  if (pressing(1, 3)) return '0';
    else return -1;
}

// 发送一个字节
void send_byte(uchar dat) {
    SBUF = dat;
    while (!TI);  // 等待发送完成
    TI = 0;       // 清除发送完成标志
}

// 发送密码
void send_password() {
    uchar i;
    // 发送起始标记
    send_byte(0xFF);
    // 发送密码长度
    send_byte(password_length);
    // 发送密码内容
    for (i = 0; i < password_length; i++) {
        send_byte(password[i]);
    }
    // 发送结束标记
    send_byte(0xFE);
}

// 发送销毁命令
void send_destroy() {
    send_byte(0xFF);
    send_byte(0);
    send_byte(0xFE);
}

// 显示当前输入
void display_input() {
    sprintf(line1, "PW=%s", password);
    sprintf(line2, "Length: %d/8     ", password_length);
    LCD1602_display(0, 0, line1);
    LCD1602_display(1, 0, line2);
}

void main() {
    static uchar number;

    // { // 初始化串口
    //     TMOD = 0x20;    // 设置定时器1为模式2
    //     SCON = 0x50;    // 设置串口为模式1
    //     PCON = 0x00;    // SMOD = 0
    //     TH1 = 0xFD;     // 波特率9600
    //     TL1 = 0xFD;
    //     TR1 = 1;        // 启动定时器1
    // }

    // 初始化LCD1602
    LCD1602_initialize();
    LCD1602_display(0, 0, "Password System");
    delay(1000);
    LCD1602_clear();
    display_input();

    while (1) {
        if (mode != STANDBY) continue;

        input = keypad4_transpose_read(pad_read, pad_write);

        number = get_pressed_number();
        if (number != -1 && password_length < MAX_PASSWORD_LENGTH) {
            password[password_length++] = number;
            display_input();
            delay(200);
            continue;
        }

        if (pressing(3, 0) && password_length > 0) {
            mode = IDEL;
            sprintf(line2, "Sending...      ");
            LCD1602_display(1, 0, line2);
            send_password();
            delay(500);
            display_input();
            mode = STANDBY; // 切换回待机模式
        }

        if (pressing(3, 1)) {
            mode = IDEL;
            clear_password(); // 清除密码
            display_input();
            send_destroy();
            sprintf(line2, "Destroying...   ");
            LCD1602_display(1, 0, line2);
            delay(500);
            display_input();
            mode = STANDBY; // 切换回待机模式
        }

        delay(10); // 防抖
    }
}
