#include "base.h"

int main(void) {
    unsigned char temp;
    while (1) {
        P2 = ~P1;
        delay(100);
    }
}

