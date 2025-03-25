#ifndef LED_H
#define LED_H

#include "base.h"

void led_set(const uchar status);

/***************************************************
 * Reset LED, turn off all LEDs.
**************************************************/
void led_reset(void);

/***************************************************
* Run a led stream.
*
* @param reverse: whether the stream is reversed
* @param period: the period of the stream
**************************************************/
void led_stream(const bool reverse, const int period);


/***************************************************
* Run a led blink.
*
* @param start: the start status of the blink, will be toggled when blink.
* @param times: the times of the blink
* @param period: the period of the blink
**************************************************/
void led_blink(const uchar start, const int times, const int period);

#endif // LED_H