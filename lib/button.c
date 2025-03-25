
#include "button.h"

uchar button_read(uchar(*supplier)(void), uchar(*setter)(uchar)) {
    uchar state = 0x00;
    setter(0xFF); // 0xFF means pull-up
    if (supplier() == 0xFF) return state;
    delay(5);
    if (supplier() == 0xFF) return state;
    state = ~supplier();
    return state;
}