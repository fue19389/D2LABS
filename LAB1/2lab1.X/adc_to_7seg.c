#include <stdint.h>
#include <pic16f887.h>
#include "adc_to_7seg.h"

void hex(void){
    num0 = adcdig & 0x0F;
    num1 = (adcdig & 0xF0)/(0x10);
}
