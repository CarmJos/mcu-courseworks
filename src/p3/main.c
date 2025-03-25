#include "base.h"
#include "hex_display.h"

void hex1() {
	uchar curr = 0;
	bool dp = FALSE;
	while (1) {
		P2 = digit(curr, dp, TRUE);
		curr = curr == 15 ? 0 : curr + 1;
		dp = !dp;
		delay(200);
	}
}

void hex2() {
	uchar curr = 0;
	bool dp = FALSE;
	while (1) {
		P0 = digit(curr / 10, !dp, TRUE);
		P2 = digit(curr % 10, dp, TRUE);
		curr = curr == 99 ? 0 : curr + 1;

		dp = !dp;
		delay(100);
	}
}

void school_num() {
	uchar selector = 0;
	uchar display[] = {0,9,2,1,0,1,1,2};
	while (1) {
		P1 = 1 << selector;		
		P3 = digit(display[selector], FALSE, TRUE);
		selector = selector >= 7 ? 0 : selector+1;
		delay(1);
	}
}


int main() {
	// hex1();
	// hex2();
	school_num();
	return 0;
}