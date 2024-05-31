/**
 * @file diod.h
 * @brief Header file for LED control functions.
 */
#ifndef DIOD_H
#define DIOD_H
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcd.h"

/**
 * @brief Set the color of the LED.
 *
 * @param spiled_base Base address of the SPILED.
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void diod_set_color(unsigned char *spiled_base, unsigned char r, unsigned char g, unsigned char b);

#endif // DIOD_H