
#include "matrix8_led.h"

#define LED_DISPLAY_DELAY 1 // LED Display delay in milliseconds

uchar* maxtrix8_compile(bool* states) {
    static uchar result[8];
    uchar i, j;
    for (i = 0; i < 8; i++) {
        result[i] = 0;
        for (j = 0; j < 8; j++) {
            if (states[i * 8 + j]) {
                result[i] |= (1 << j);
            }
        }
    }
    return result;
}

uchar* matrix8_compile_scroll(bool characters[][64], int characters_count, int offset) {
    static uchar result[8];
    uchar i, j;
    int next_char_index, next_bit_index, column_index;
    int char_index = offset / 8; // Determine which character to start from
    int bit_offset = offset % 8; // Determine the bit offset within the character

    for (i = 0; i < 8; i++) {
        result[i] = 0;
        for (j = 0; j < 8; j++) {
            column_index = j + bit_offset;
            next_char_index = char_index + (column_index / 8);
            next_bit_index = column_index % 8;
            if (next_char_index < characters_count && characters[next_char_index][i * 8 + next_bit_index]) {
                result[i] |= (1 << j);
            }
        }
    }
    return result;
}


void matrix8_set(uchar* contents) {
    uchar i;
    for (i = 0; i < 8; i++) {
        matrix8_rows_set(0x00); // 拉低所有行
        matrix8_columns_set(contents[i]); // 显示当前行的数据
        matrix8_rows_set(1 << i); // 拉高当前行
        delay(LED_DISPLAY_DELAY); // 延时，保持当前行的显示
    }
}

void matrix8_display(bool* states) {
    matrix8_set(maxtrix8_compile(states));
}

void matrix8_scoll_display(bool characters[][64], int characters_count, int offset) {
    matrix8_set(matrix8_compile_scroll(characters, characters_count, offset));
}