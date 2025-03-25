#include "hex_display.h"

uchar digit(const uchar value, const bool dp, const bool anode) {
	uchar out = value < 16 ? SEGMENTS[value] : 0x00;
	if (dp) { out |= 0x80; }
	if (anode) { out = ~out; }
	return out;
}