#include "base.h"

bool enable = false;

int main(void) {

	// 定时器0 计数器模式2, 5次计数。
	// 定时器1 工作在模式2，8位自动重装载
	// TMOD < GATE | C/T | M1 | M2 || GATE | C/T  | M1 | M2 >
	// VAL= <  0   | 1   | 1  | 0  ||  0   |  0   | 1  | 0  >
	TMOD = 0x26;

	// [T0] 计数器模式2，5次按钮触发
	TH0 = 0xFB;
	TL0 = 0xFB;

	// [T1] 2.5ms 定时器
	//（2^8-X）× 12 ÷ 12=5000
	// X = 256 - (250 * 12 / 12) = 6
	TH1 = 0x06;
	TL1 = 0x06;

	TR0 = 1;
	ET0 = 1; 
	TR1 = 1;
	ET1 = 1;
	EA = 1;

	while (1) {
	}
}

void counter0(void) interrupt 1 {
	enable = true;
}

// 定时器1(模式2，5ms) 中断函数，实现每500ms对P1翻转
void timer1(void) interrupt 3 {
	static uint16 count = 0; // 定义静态变量count，初值为0

	if (!enable) {
		count = 0; // 清零计数器
		return;
	}

	count++; // 每次中断count加1
	if (count >= 2000) { // 每500ms翻转一次
		count = 0; // 清零计数器
		P1 = ~P1; // 翻转P1口的状态
	}
}
