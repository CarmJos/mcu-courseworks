#include "base.h"
#include "hex_display.h"

#define MAX_PASSWORD_LENGTH 8

// 数码管控制引脚
sbit LATCH_SEG = P2 ^ 6;  // 数码管段选锁存器（显示内容）
sbit LATCH_BIT = P2 ^ 7;  // 数码管位选锁存器（位置选择）

// 串行接收引脚
sbit RX = P3^0;

// 接收状态
typedef enum {
    WAITING_START,   // 等待起始标记
    READING_LENGTH,  // 读取长度
    READING_DATA,    // 读取数据
    WAITING_END     // 等待结束标记
} ReceiveState;

// 显示状态
typedef enum {
    DISPLAY_ON,     // 显示开启
    DISPLAY_OFF     // 显示关闭
} DisplayState;

// 全局变量
uchar received_data[MAX_PASSWORD_LENGTH];  // 接收到的数据
uchar data_length = 0;                     // 数据长度
uchar current_pos = 0;                     // 当前接收位置
ReceiveState rx_state = WAITING_START;     // 接收状态
DisplayState disp_state = DISPLAY_OFF;     // 显示状态

// 数码管位选代码
static uchar BIT_CODES[8] = {
    0xFE, 0xFD, 0xFB, 0xF7,
    0xEF, 0xDF, 0xBF, 0x7F
};

// 显示数据到数码管
void display_data() {
    static int i = 0;
    
    if (disp_state == DISPLAY_OFF) {
        // 关闭所有数码管
        P0 = 0xFF;
        LATCH_SEG = 1;
        LATCH_SEG = 0;
        return;
    }
    
    for (i = 0; i < 8; i++) {
        // 选择位置
        P0 = BIT_CODES[i];
        LATCH_BIT = 1;
        LATCH_BIT = 0;
        
        // 显示内容
        if (i < data_length) {
            P0 = digit(received_data[i] - '0', TRUE, FALSE);
        } else {
            P0 = 0xFF;  // 关闭未使用的数码管
        }
        LATCH_SEG = 1;
        LATCH_SEG = 0;
        
        delay(1);  // 短暂延时
    }
}

// 串行接收中断处理
void serial() interrupt 4 {
    uchar received;
    
    if (RI) {
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
                    disp_state = DISPLAY_OFF;
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
                if (received == 0xFE) {
                    if (data_length > 0) {
                        disp_state = DISPLAY_ON;
                    }
                }
                rx_state = WAITING_START;
                break;
        }
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
