#ifndef MATRIX4_KEYPAD_H
#define MATRIX4_KEYPAD_H

#include "base.h"

uint16 keypad4_parallel_read(uchar(*port_get)(void), void(*port_set)(uchar));

uint16 keypad4_transpose_read(uchar(*port_get)(void), void(*port_set)(uchar));

#endif // MATRIX4_KEYPAD_H