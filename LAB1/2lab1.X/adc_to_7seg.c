#include <stdint.h>
#include <pic16f887.h>
#include "adc_to_7seg.h"

void hex(void){
    num0 = PORTD & 0x0F;
    num1 = PORTD & 0xF0 ;
}
