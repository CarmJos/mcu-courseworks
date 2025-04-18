#include "base.h"
#include "lcd1602.h"

int main(void) {

	LCD1602_initialize(); // Initialize the LCD1602.
	LCD1602_display(0, 5, "Welcome");
	LCD1602_display(1, 2, "Harbin China!");
	while (1) {
		// delay(1000);
		LCD1602_scroll(RIGHT);
		// LCD1602_clear();
		delay(100);
	}
}

