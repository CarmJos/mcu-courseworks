#ifndef HEX_DISPLAY_H
#define HEX_DISPLAY_H

#include "base.h"

uchar code SEGMENTS[] = {
	0x3F, 0x06, 0x5B, 0x4F,	0x66, 0x6D, 0x7D, 0x07,
	0x7F, 0x6F,	0x77, 0x7C,	0x39, 0x5E,	0x79, 0x71
};

/***********************************************************
 * @param value    : The display digit, from 0x00 to 0x16 .
 * @param dp  : Whether to show the display point.
 * @param annode   : Whether this hex display is ANNODE.
 * @return The digit reg value.
************************************************************/
uchar digit(const uchar value, const bool dp, const bool anode);


#endif // HEX_DISPLAY_H