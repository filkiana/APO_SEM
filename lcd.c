/**
 * @file lcd.c
 * @brief Implementation of LCD display functions.
 */
#include "lcd.h"



/**
 * @brief Initialize the LCD display.
 *
 * @param parlcd_mem_base Base address of the LCD memory.
 * @return Pointer to the framebuffer.
 */
unsigned short * lcd_init(unsigned char *parlcd_mem_base) {
  unsigned short * fb = (unsigned short *)malloc(HEIGHT * WIDTH * 2);

  if (fb == NULL) {
    perror("Failed to allocate framebuffer");
    exit(EXIT_FAILURE);
  }

  parlcd_hx8357_init(parlcd_mem_base);

  // Clear the screen
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      fb[i * WIDTH + j] = 0;
      parlcd_write_data(parlcd_mem_base, 0);
    }
  }
  return fb;
}
/**
 * @brief Draw a pixel on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void lcd_draw_pixel(unsigned short *fb, int x, int y, unsigned short color) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    fb[x + WIDTH * y] = color;
  }
}
/**
 * @brief Draw a plus sign on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the center.
 * @param y The y-coordinate of the center.
 * @param color The color of the plus sign.
 */
void lcd_draw_plus(unsigned short * fb, int x, int y, unsigned short color) {

    lcd_draw_pixel(fb, x+1, y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x-1, y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x+2, y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x-2, y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x, y+1, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x, y-1, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x, y+2, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, x, y-2, lcd_color(127, 0, 0));
}
/**
 * @brief Update the LCD display with the framebuffer content.
 *
 * @param fb Pointer to the framebuffer.
 * @param parlcd_mem_base Base address of the LCD memory.
 */
void lcd_update_display(unsigned short * fb,unsigned char *parlcd_mem_base) {
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < WIDTH * HEIGHT; ptr++) {
    parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
}
/**
 * @brief Convert RGB values to a 16-bit color.
 *
 * @param red The red component.
 * @param green The green component.
 * @param blue The blue component.
 * @return The 16-bit color value.
 */
unsigned short lcd_color(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0x1F) << 11) | ((green & 0x3F) << 5) | (blue & 0x1F);
}
/**
 * @brief Convert an 8-bit grayscale intensity to a 16-bit color.
 *
 * @param intensity The grayscale intensity.
 * @return The 16-bit color value.
 */
unsigned short lcd_grey(uint8_t intensity) {
  // Scale 8-bit intensity (0-255) to 5-bit and 6-bit ranges for RGB565
  uint8_t red = intensity >> 3;   // Scale 0-255 to 0-31
  uint8_t green = intensity >> 2; // Scale 0-255 to 0-63
  uint8_t blue = intensity >> 3;  // Scale 0-255 to 0-31
  return lcd_color(red, green, blue);
}
/**
 * @brief Draw an image on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param image The grayscale image matrix.
 */
void lcd_draw_image(unsigned short * fb, int width, int height, gsl_matrix * image){
  unsigned int c;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
          c = (unsigned int)gsl_matrix_get(image, i, j);
          c = lcd_grey((uint8_t)c);
          lcd_draw_pixel(fb, i, j, c);
      }
  }
}
/**
 * @brief Fill the entire screen with a specific color.
 *
 * @param fb Pointer to the framebuffer.
 * @param color The color to fill the screen with.
 */
void lcd_fill_screen(unsigned short * fb, unsigned short color){
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      fb[i * WIDTH + j] = color;
    }
  }
}


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
void lcd_draw_char(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char ch, unsigned short color, unsigned int font_size) {
    int w = lcd_char_width(ch, fdes);
    const font_bits_t *ptr;

    if ((ch >= fdes->firstchar) && (ch - fdes->firstchar < fdes->size)) {
        if (fdes->offset) {
            ptr = &fdes->bits[fdes->offset[ch - fdes->firstchar]];
        } else {
            int bw = (fdes->maxwidth + 15) / 16;
            ptr = &fdes->bits[(ch - fdes->firstchar) * bw * fdes->height];
        }
        lcd_draw_char_bitmap(fb, ptr, x, y, w, fdes->height, color, font_size);
    }
}
/**
 * @brief Get the width of a character in a specific font.
 *
 * @param ch The character.
 * @param fdes Pointer to the font descriptor.
 * @return The width of the character.
 */
int lcd_char_width(char ch, font_descriptor_t *fdes) {
    if (fdes->width) {
        return fdes->width[ch - fdes->firstchar];
    } else {
        return fdes->maxwidth;
    }
}
/**
 * @brief Draw a block of scaled pixels on the LCD.
 *
 * @param fb Pointer to the framebuffer.
 * @param x The x-coordinate of the block.
 * @param y The y-coordinate of the block.
 * @param color The color of the block.
 * @param scale The scaling factor.
 */
void lcd_draw_scaled_pixel_block(unsigned short *fb, int x, int y, unsigned short color, unsigned int scale) {
    for (int sy = 0; sy < scale; sy++) {
        for (int sx = 0; sx < scale; sx++) {
            lcd_draw_pixel(fb, x + sx, y + sy, color);
        }
    }
}

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
void lcd_draw_char_bitmap(unsigned short *fb, const font_bits_t *ptr, int x, int y, int w, int h, unsigned short color, unsigned int font_size) {
    for (int i = 0; i < h; i++) {
        font_bits_t val = *ptr;
        for (int j = 0; j < w; j++) {
            if ((val & 0x8000) != 0) {
                lcd_draw_scaled_pixel_block(fb, x + font_size * j, y + font_size * i, color, font_size);
            }
            val <<= 1;
        }
        ptr++;
    }
}

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
void lcd_draw_text(unsigned short *fb, int x, int y, font_descriptor_t *fdes, char *text, unsigned short color, unsigned int font_size) {
    int cursor_x = x;
    int cursor_y = y;

    while (*text) {
        if (*text == '\n') {
            cursor_x = x;  // Reset x position to the start of the line
            cursor_y += fdes->height * font_size;  // Move y position down by the height of the font
        } else if (*text == '\r') {
            cursor_x = x;  // Reset x position to the start of the line
        } else {
            lcd_draw_char(fb, cursor_x, cursor_y, fdes, *text, color, font_size);
            cursor_x += lcd_char_width(*text, fdes) * font_size;  // Move x position to the right by the width of the character
        }
        text++;  // Move to the next character
    }
}