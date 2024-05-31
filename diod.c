/**
 * @file diod.c
 * @brief Implementation of Diod LED control functions.
 */
#include "diod.h"

/**
 * @brief Set the color of the Diod.
 *
 * @param spiled_base Base address of the SPILED.
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void diod_set_color(unsigned char *spiled_base, unsigned char r, unsigned char g, unsigned char b){
    *(spiled_base + SPILED_REG_LED_RGB1_o) = r;
    *(spiled_base + SPILED_REG_LED_RGB1_o+1) = g;
    *(spiled_base + SPILED_REG_LED_RGB1_o+2) = b;
}