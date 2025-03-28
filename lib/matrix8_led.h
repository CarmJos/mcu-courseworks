#ifndef MATRIX8_LED_H
#define MATRIX8_LED_H

#include "base.h"

/***
 * @brief 8x8 LED matrix display. Used to define the rows of the LED matrix.
 *
 * @param rows 8x8 LED rows data
 */
void matrix8_rows_set(uchar rows);

/**
 *  @brief 8x8 LED matrix display. Used to define the columns of the LED matrix.
 *  @param columns 8x8 LED columns data
 */
void matrix8_columns_set(uchar columns);

/**
 *  @brief Display the 8x8 LED matrix.
 *  @param bits 8x8 LED state data
 */
void matrix8_display(bool* bits);


/**
 * @brief Display the 8x8 LED matrix.
 * @param characters 8x8 LED state data
 * @param characters_count Number of characters to display
 * @param offset Offset for scrolling display
 */
void matrix8_scoll_display(bool characters[][64], int characters_count, int offset);

void matrix8_set(uchar* contents);

#endif // MATRIX8_LED_H