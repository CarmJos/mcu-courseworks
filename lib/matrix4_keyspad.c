#include "base.h"

#define KEYS_DEBOUNCE_DELAY 50 // Debounce delay in milliseconds

uint16 keypad4_parallel_read(uchar(*port_get)(void), void(*port_set)(uchar)) {
    uchar row, col, keys;
    uint16 state = 0x0000;

    for (col = 0; col < 4; col++) {
        port_set(~(1 << col)); // Set one column low at a time
        delay(KEYS_DEBOUNCE_DELAY); // Debounce delay

        keys = port_get() & 0xF0; // Read the row states (high nibble)
        for (row = 0; row < 4; row++) {
            if (!(keys & (1 << (4 + row)))) { // Check if the key is pressed
                state |= (1 << (row * 4 + col)); // Update the state
            }
        }
    }


    return state; // Return the state of the keys pressed
}

uint16 keypad4_transpose_read(uchar(*port_get)(void), void(*port_set)(uchar)) {
    uint16 state = 0x0000;
    uchar i, scan1, scan2, keys;

    port_set(0xF0);
    scan1 = port_get();
    if (scan1 == 0xF0) return 0;

    scan1 = port_get();
    if (scan1 == 0xF0) return 0;

    port_set(0x0F);
    scan2 = port_get();

    for (i = 0; i < 4; i++) {
        if (!(scan1 & (1 << (4 + i)))) {
            keys = (~scan2) & 0x0F;
            state |= (keys << (i * 4));
        }
    }

    return state;
}
