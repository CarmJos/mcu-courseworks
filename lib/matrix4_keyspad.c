#include "base.h"

#define KEYS_DEBOUNCE_DELAY 10 // Debounce delay in milliseconds

#define KEYS_COLUMN_MASK 0x0F // Mask for the column bits
#define KEYS_ROW_MASK 0xF0 // Mask for the row bits

uint16 keypad4_read(uchar(*port_get)(void), uchar(*port_set)(uchar)) {
    static row, col;
    uint16 state = 0x0000; // Initialize state to 0

    port_set(0xF0); // Set all rows to high (inactive state)
    if (port_get() & 0xF0 == 0xF0) return 0; // If no key is pressed, return 0

    for (row = 0; row < 4; row++) {
        port_set(0xF0 | (1 << row)); // Set the current row to low (active state)
        delay(KEYS_DEBOUNCE_DELAY); // Wait for debounce

        col = port_get() & KEYS_COLUMN_MASK; // Read the column state
        if (col != 0x0F) { // If a key is pressed in this row
            state |= (col << (row * 4)); // Update the state with the pressed key
            delay(KEYS_DEBOUNCE_DELAY); // Wait for debounce
            while ((port_get() & KEYS_COLUMN_MASK) != 0x0F); // Wait until the key is released
        }
    }

    return state; // Return the state of the keys pressed

}