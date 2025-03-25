#include "base.h"

void delay(unsigned int i) {
    unsigned char t;
    while (i--)  for (t = 0;t < 120;t++);
}
