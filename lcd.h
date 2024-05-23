#ifndef LCD_H
#define LCD_H
#include <stdint.h>
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "font_types.h"
#include "mzapo_regs.h"

#include <gsl/gsl_matrix.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define HEIGHT 320
#define WIDTH 480


// Function to initialize the LCD
unsigned short * lcd_init(unsigned char *parlcd_mem_base);

// Function to draw a pixel on the LCD
void lcd_draw_pixel(unsigned short *fb,int x, int y, unsigned short color);

// Function to draw a character on the LCD
void lcd_draw_char(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char ch, unsigned short color, unsigned int font_size);
int lcd_char_width(char ch, font_descriptor_t *fdes);
void lcd_draw_scaled_pixel_block(unsigned short *fb, int x, int y, unsigned short color, unsigned int scale);
void lcd_draw_char_bitmap(unsigned short *fb, const font_bits_t *ptr, int x, int y, int w, int h, unsigned short color, unsigned int font_size);
void lcd_draw_text(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char *text, unsigned short color, unsigned int font_size);

void lcd_update_display(unsigned short * fb,unsigned char *parlcd_mem_base);


unsigned short lcd_grey(uint8_t intensity);


unsigned short lcd_color(uint8_t red, uint8_t green, uint8_t blue);


void lcd_draw_image(unsigned short * fb, int width, int height, gsl_matrix * image);

void lcd_draw_plus(unsigned short * fb, int x, int y, unsigned short color);

void lcd_fill_screen(unsigned short * fb, unsigned short color);

#endif // LCD_H