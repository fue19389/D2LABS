#include <stdint.h>
#include <pic16f887.h>
#include "adc_to_7seg.h"

void hex(void){
    
    num = adcdig >> 4;
    num1 = num & 0x0F;
    num0 = adcdig & 0x0F;
}
