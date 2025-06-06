#include "base.h"
#include "hex_display.h"

#define MAX_PASSWORD_LENGTH 8
#define LINE P0  // 数码管控制端口

// 数码管控制引脚
sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

// 串行接收引脚
sbit RX = P3 ^ 0;

// 接收状态
typedef enum {
    WAITING_START,   // 等待起始标记
    READING_LENGTH,  // 读取长度
    READING_DATA,    // 读取数据
    WAITING_END     // 等待结束标记
} ReceiveState;

// 全局变量
uchar received_data[MAX_PASSWORD_LENGTH];  // 接收到的数据
uchar data_length = 0;                     // 数据长度
uchar current_pos = 0;                     // 当前接收位置
ReceiveState rx_state = WAITING_START;     // 接收状态

// 数码管位选代码
static uchar BIT_CODES[8] = {
    0xFE, 0xFD, 0xFB, 0xF7,
    0xEF, 0xDF, 0xBF, 0x7F
};

void display(uchar* val, int len) {
    static int i = 0;
    for (i = 0; i < 8; i++) {
        LINE = BIT_CODES[i];
        LATCH_BIT = 1;
        LATCH_BIT = 0;

        if (i < len) {
            LINE = digit(val[i] - '0', FALSE, FALSE);  // 显示数字
        } else {
            LINE = 0x00;  // 关闭未使用的数码管
        }

        LATCH_SEG = 1;
        LATCH_SEG = 0;

        delay(2);
    }
}

// 显示数据到数码管
void display_data() {
    display(received_data, data_length);  // 显示接收到的数据
}

void clear_data() {
    static uchar i;
    data_length = 0;
    current_pos = 0;
    for (i = 0; i < MAX_PASSWORD_LENGTH; i++) {
        received_data[i] = ' ';  // 清空接收数据
    }
}

// 串行接收中断处理
void serial() interrupt 4 {
    uchar received;

    if (RI) {
        // P2.3 设为 0
        P2 &= ~(1 << 3);  // 清除P2.3位，表示接收中断



        RI = 0;  // 清除接收中断标志
        received = SBUF;  // 读取接收到的数据

        switch (rx_state) {
        case WAITING_START:
            if (received == 0xFF) {
                rx_state = READING_LENGTH;
            }
            break;

        case READING_LENGTH:
            data_length = received;
            if (data_length == 0) {
                // 收到销毁命令
                clear_data();
                rx_state = WAITING_END;
            } else if (data_length <= MAX_PASSWORD_LENGTH) {
                current_pos = 0;
                rx_state = READING_DATA;
            } else {
                // 长度错误，重置状态
                rx_state = WAITING_START;
            }
            break;

        case READING_DATA:
            received_data[current_pos++] = received;
            if (current_pos >= data_length) {
                rx_state = WAITING_END;
            }
            break;

        case WAITING_END:
            rx_state = WAITING_START;
            break;
        }

        // P2.3 设为 1
        P2 |= (1 << 3);  // 设置P2.3位，表示接收完成
    }
}

void main() {
    // 初始化串口
    TMOD = 0x20;    // 设置定时器1为模式2
    SCON = 0x50;    // 设置串口为模式1
    PCON = 0x00;    // SMOD = 0
    TH1 = 0xFD;     // 波特率9600
    TL1 = 0xFD;
    TR1 = 1;        // 启动定时器1

    // 开启中断
    ES = 1;         // 开启串行中断
    EA = 1;         // 开启总中断

    while (1) {
        display_data();  // 持续刷新显示
    }
}
