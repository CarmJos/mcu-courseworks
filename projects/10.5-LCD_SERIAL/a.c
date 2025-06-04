#include "base.h"
#include <stdio.h>
#include "matrix4_keyspad.h"
#include "lcd1602.h"

#define MAX_PASSWORD_LENGTH 8
#define KEYS_PORT P3

sbit TX = P3^1;  // 串行发送引脚

typedef enum {
    INPUT_MODE,      // 输入模式
    SENDING_MODE     // 发送模式
} WorkMode;

WorkMode mode = INPUT_MODE;
uchar password[MAX_PASSWORD_LENGTH + 1] = {0}; // +1 用于存储结束符
uchar password_length = 0;

char line1[16];
char line2[16];

uint16 input;

// 读取键盘输入
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

// 检查按键是否按下
bool pressing(uchar x, uchar y) {
    return (input & (1 << (x * 4 + y))) != 0;
}

// 获取按下的数字
char get_pressed_number() {
    if (pressing(0, 0)) return '7';
    if (pressing(0, 1)) return '8';
    if (pressing(0, 2)) return '9';
    if (pressing(1, 0)) return '4';
    if (pressing(1, 1)) return '5';
    if (pressing(1, 2)) return '6';
    if (pressing(2, 0)) return '1';
    if (pressing(2, 1)) return '2';
    if (pressing(2, 2)) return '3';
    if (pressing(3, 1)) return '0';
    return -1;
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
    uchar i;
    sprintf(line1, "Password:%*s", MAX_PASSWORD_LENGTH, "");
    for (i = 0; i < password_length; i++) {
        line1[9 + i] = password[i];
    }
    sprintf(line2, "Length: %d/8", password_length);
    LCD1602_display(0, 0, line1);
    LCD1602_display(1, 0, line2);
}

void main() {
    char key;
    
    // 初始化串口
    TMOD = 0x20;    // 设置定时器1为模式2
    SCON = 0x50;    // 设置串口为模式1
    PCON = 0x00;    // SMOD = 0
    TH1 = 0xFD;     // 波特率9600
    TL1 = 0xFD;
    TR1 = 1;        // 启动定时器1
    
    // 初始化LCD1602
    LCD1602_initialize();
    LCD1602_display(0, 0, "Password System");
    delay(1000);
    LCD1602_clear();
    
    while (1) {
        read_keys();
        
        if (mode == INPUT_MODE) {
            // 检查数字输入
            key = get_pressed_number();
            if (key != -1 && password_length < MAX_PASSWORD_LENGTH) {
                password[password_length++] = key;
                display_input();
                delay(200); // 防抖
            }
            
            // 检查发送按钮 (0,3)
            if (pressing(0, 3) && password_length > 0) {
                send_password();
                sprintf(line1, "Sending...     ");
                sprintf(line2, "              ");
                LCD1602_display(0, 0, line1);
                LCD1602_display(1, 0, line2);
                delay(1000);
                // 清空密码
                password_length = 0;
                display_input();
            }
            
            // 检查销毁按钮 (1,3)
            if (pressing(1, 3)) {
                send_destroy();
                sprintf(line1, "Destroying...  ");
                sprintf(line2, "              ");
                LCD1602_display(0, 0, line1);
                LCD1602_display(1, 0, line2);
                delay(1000);
                display_input();
            }
        }
        
        delay(10); // 防抖
    }
}
