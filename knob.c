/**
 * @file knob.c
 * @brief Implementation of knob value reading functions.
 */
#include "knob.h"


/**
 * @brief Get the value of a knob.
 *
 * @param spiled_base Base address of the SPILED.
 * @param knob The knob identifier.
 * @param current The current value of the knob.
 * @return The difference between the knob value and the current value.
 */
int8_t get_knob_value(unsigned char *spiled_base,const uint8_t knob,  const int current){
    int8_t knob_value = *(spiled_base + knob);
    int8_t diff = knob_value - current;
    return diff;
}