#include <stdint.h>
#include <pic16f887.h>
#include "adc.h"

uint8_t valadc(uint8_t aa){
    aa = ADRESH;
    return aa;
}