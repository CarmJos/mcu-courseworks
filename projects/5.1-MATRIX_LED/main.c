#include "base.h"
#include "matrix8_led.h"

#define TIMER0_RELOAD_VALUE 65536 - 9216 // For ~10ms delay with 11.0592MHz crystal

#define MATRIX_ROWS_PORT P1
#define MATRIX_COLUMNS_PORT P0
#define DISPLAY_TIME 10
#define MAX_STATE 48

uchar state = 0;

code bool CONTENTS[6][64] = {
    { // L
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0
    },{// X
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 1, 1, 0, 0, 0, 1, 1
    },{ // Y
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
    },{ // 1
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 0, 0, 0
    },{ // 1
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0
    }, { // 2
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 1
    }
};

void timer0_init(void) {
	TMOD |= 0x01; // Timer 0 in mode 1 (16-bit timer mode)
	TH0 = (TIMER0_RELOAD_VALUE >> 8) & 0xFF; // Load high byte
	TL0 = TIMER0_RELOAD_VALUE & 0xFF;        // Load low byte
	ET0 = 1; // Enable Timer 0 interrupt
	EA = 1;  // Enable global interrupts
	TR0 = 1; // Start Timer 0
}

void timer0_isr(void) interrupt 1 {
	static uint count = 0;

	TH0 = (TIMER0_RELOAD_VALUE >> 8) & 0xFF; // Reload high byte
	TL0 = TIMER0_RELOAD_VALUE & 0xFF;        // Reload low byte

	count++;
	if (count >= DISPLAY_TIME) { // 100 * 10ms = 1000ms
		count = 0;
		state = (state + 1) % MAX_STATE; // Switch state every 1000ms
	}
}

int main(void) {
	timer0_init(); // Initialize Timer 0
	while (1) {
		// matrix8_display(&CONTENTS[state]); // Display the current character
		matrix8_scoll_display(CONTENTS, 6, state); // Scroll display
	}
}

void matrix8_rows_set(uchar rows) {
	MATRIX_ROWS_PORT = rows;
}

void matrix8_columns_set(uchar columns) {
	MATRIX_COLUMNS_PORT = ~columns;
}
