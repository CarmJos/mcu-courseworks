#include <reg51.h>
#include <intrins.h>
#include "base.h"

void alternately(){
	unsigned char status = 0;
	while(1){
		P1 = status ? 0xAA : 0x55;
		status = !status;
		delay(100);
	}
}

void stream(){
	P1 = 0xf3;		
	while(1){
		P1 = _crol_(P1,1);
		delay(100);
	}
}

int main(void){
	//alternately();
	stream();
}

