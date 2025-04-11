#include "base.h"
#include "matrix4_keyspad.h"
#include "hex_display.h"

#define KEYS_PAD_PORT P0
#define HEX_DISPLAY_PORT P1

uchar covert(uint16 value) {
	uchar i = 0;
	for (i = 0; i < 16; i++) {
		if (value & (1 << i)) {
			return i; // Return the first pressed key
		}
	}
	return 20;
}

void keys_set(uchar status) {
	KEYS_PAD_PORT = status;
}

uchar keys_get() {
	return KEYS_PAD_PORT;
}

int main(void) {
	uint16 keys_val;
	uchar display;
	while (1) {
		keys_val = keypad4_transpose_read(&keys_get, &keys_set);
		if (keys_val != 0) {
			display = covert(keys_val);
		}
		HEX_DISPLAY_PORT = digit(display, FALSE, TRUE);

		// (DEBUG) Display the pressed key states.
		P2 = (uchar) (keys_val & 0x00FF);
		P3 = (uchar) ((keys_val >> 8) & 0x00FF);
	}
}