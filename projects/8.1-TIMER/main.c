#include "base.h"

int main(void) {


	TMOD = 0x20; // 定时器1工作在模式2，8位自动重装载

	// 2.5ms 定时器初值计算
	//（2^8-X）× 12 ÷ 12=5000
	// X = 256 - (250 * 12 / 12) = 6

	TH1 = 0x06; // 定时器初值
	TL1 = 0x06; // 定时器初值

	TR1 = 1; // 启动定时器1
	ET1 = 1; // 允许定时器1中断
	EA = 1; // 允许总中断

	while (1) {
	}
}


// 定时器1(模式2，5ms) 中断函数，实现每500ms对P1翻转
void Timer1(void) interrupt 3 using 1 {
	static uint16 count = 0; // 定义静态变量count，初值为0

	count++; // 每次中断count加1
	if (count >= 2000) { // 每500ms翻转一次
		count = 0; // 清零计数器
		P1 = ~P1; // 翻转P1口的状态
	}
}
