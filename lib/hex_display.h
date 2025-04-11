#ifndef HEX_DISPLAY_H
#define HEX_DISPLAY_H

#include "base.h"

/***********************************************************
 * @param value    : The display digit, from 0x00 to 0x16 .
 * @param dp  : Whether to show the display point.
 * @param annode   : Whether this hex display is ANNODE.
 * @return The digit reg value.
************************************************************/
uchar digit(const uchar value, const bool dp, const bool anode);


#endif // HEX_DISPLAY_H