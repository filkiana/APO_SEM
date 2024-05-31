/**
 * @file lcd.h
 * @brief Header file for LCD display functions.
 */
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


/**
 * @brief Initialize the LCD display.
 *
 * @param parlcd_mem_base Base address of the LCD memory.
 * @return Pointer to the framebuffer.
 */
unsigned short * lcd_init(unsigned char *parlcd_mem_base);


/**
 * @brief Draw a pixel on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void lcd_draw_pixel(unsigned short *fb,int x, int y, unsigned short color);

/**
 * @brief Draw a character on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the character.
 * @param y The y-coordinate of the character.
 * @param fdes Pointer to the font descriptor.
 * @param ch The character to draw.
 * @param color The color of the character.
 * @param font_size The size of the font.
 */
void lcd_draw_char(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char ch, unsigned short color, unsigned int font_size);
/**
 * @brief Get the width of a character in a specific font.
 *
 * @param ch The character.
 * @param fdes Pointer to the font descriptor.
 * @return The width of the character.
 */
int lcd_char_width(char ch, font_descriptor_t *fdes);

/**
 * @brief Draw a block of scaled pixels on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the block.
 * @param y The y-coordinate of the block.
 * @param color The color of the block.
 * @param scale The scaling factor.
 */
void lcd_draw_scaled_pixel_block(unsigned short *fb, int x, int y, unsigned short color, unsigned int scale);
/**
 * @brief Draw a bitmap of a character on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param ptr Pointer to the bitmap data.
 * @param x The x-coordinate of the bitmap.
 * @param y The y-coordinate of the bitmap.
 * @param w The width of the bitmap.
 * @param h The height of the bitmap.
 * @param color The color of the bitmap.
 * @param font_size The size of the font.
 */
void lcd_draw_char_bitmap(unsigned short *fb, const font_bits_t *ptr, int x, int y, int w, int h, unsigned short color, unsigned int font_size);
/**
 * @brief Draw text on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the text.
 * @param y The y-coordinate of the text.
 * @param fdes Pointer to the font descriptor.
 * @param text The text to draw.
 * @param color The color of the text.
 * @param font_size The size of the font.
 */
void lcd_draw_text(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char *text, unsigned short color, unsigned int font_size);

/**
 * @brief Update the LCD display with the framebuffer content.
 *
 * @param fb Pointer to the framebuffer.
 * @param parlcd_mem_base Base address of the LCD memory.
 */
void lcd_update_display(unsigned short * fb,unsigned char *parlcd_mem_base);

/**
 * @brief Convert an 8-bit grayscale intensity to a 16-bit color.
 *
 * @param intensity The grayscale intensity.
 * @return The 16-bit color value.
 */
unsigned short lcd_grey(uint8_t intensity);

/**
 * @brief Convert RGB values to a 16-bit color.
 *
 * @param red The red component.
 * @param green The green component.
 * @param blue The blue component.
 * @return The 16-bit color value.
 */
unsigned short lcd_color(uint8_t red, uint8_t green, uint8_t blue);


/**
 * @brief Draw an image on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param image The grayscale image matrix.
 */
void lcd_draw_image(unsigned short * fb, int width, int height, gsl_matrix * image);
/**
 * @brief Draw a plus sign on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the center.
 * @param y The y-coordinate of the center.
 * @param color The color of the plus sign.
 */
void lcd_draw_plus(unsigned short * fb, int x, int y, unsigned short color);
/**
 * @brief Fill the entire screen with a specific color.
 *
 * @param fb Pointer to the framebuffer.
 * @param color The color to fill the screen with.
 */
void lcd_fill_screen(unsigned short * fb, unsigned short color);

#endif // LCD_H