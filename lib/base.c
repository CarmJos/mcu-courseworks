#include "base.h"

void delay(unsigned int i) {
    unsigned int j;
    for (; i > 0; i--) {
        for (j = 0; j < 125; j++) {
            _nop_();
        }
    }
}
