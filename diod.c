#include "diod.h"


void diod_set_color(unsigned char *spiled_base, unsigned char r, unsigned char g, unsigned char b){
    *(spiled_base + SPILED_REG_LED_RGB1_o) = r;
    *(spiled_base + SPILED_REG_LED_RGB1_o+1) = g;
    *(spiled_base + SPILED_REG_LED_RGB1_o+2) = b;
}