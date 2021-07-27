#include <stdint.h>
#include <pic16f887.h>
#include "adc.h"

double valadc(unsigned char adcval){
    double adcv; //int adcv;
    adcv = ADRESH ;
    return adcv;
}