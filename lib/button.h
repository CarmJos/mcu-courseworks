#ifndef BUTTON_H
#define BUTTON_H

#include "base.h"

uchar button_read(uchar(*supplier)(void), uchar(*setter)(uchar));

#endif // BUTTON_H