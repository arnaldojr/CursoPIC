#include <p18cxxx.h>

void Delay(unsigned int delay_val) {
	unsigned int i;
	
	for (i=0; i<delay_val; i++) {
		Nop();	
	}
}