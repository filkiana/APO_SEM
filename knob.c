#include "knob.h"


int8_t get_knob_value(unsigned char *spiled_base,const uint8_t knob,  const int current){
    int8_t knob_value = *(spiled_base + knob);
    int8_t diff = knob_value - current;
    return diff;
}