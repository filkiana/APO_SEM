#include "lcd.h"




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

void lcd_draw_pixel(unsigned short *fb, int x, int y, unsigned short color) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    fb[x + WIDTH * y] = color;
  }
}

void lcd_draw_plus(unsigned short * fb, int current_x, int current_y, unsigned short color) {

    lcd_draw_pixel(fb, current_x+1, current_y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x-1, current_y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x+2, current_y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x-2, current_y, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x, current_y+1, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x, current_y-1, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x, current_y+2, lcd_color(127, 0, 0));
    lcd_draw_pixel(fb, current_x, current_y-2, lcd_color(127, 0, 0));
}

void lcd_update_display(unsigned short * fb,unsigned char *parlcd_mem_base) {
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < WIDTH * HEIGHT; ptr++) {
    parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
}

unsigned short lcd_color(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0x1F) << 11) | ((green & 0x3F) << 5) | (blue & 0x1F);
}

unsigned short lcd_grey(uint8_t intensity) {
  // Scale 8-bit intensity (0-255) to 5-bit and 6-bit ranges for RGB565
  uint8_t red = intensity >> 3;   // Scale 0-255 to 0-31
  uint8_t green = intensity >> 2; // Scale 0-255 to 0-63
  uint8_t blue = intensity >> 3;  // Scale 0-255 to 0-31
  return lcd_color(red, green, blue);
}

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

void lcd_fill_screen(unsigned short * fb, unsigned short color){
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      fb[i * WIDTH + j] = color;
    }
  }
}



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

int lcd_char_width(char ch, font_descriptor_t *fdes) {
    if (fdes->width) {
        return fdes->width[ch - fdes->firstchar];
    } else {
        return fdes->maxwidth;
    }
}

void lcd_draw_scaled_pixel_block(unsigned short *fb, int x, int y, unsigned short color, unsigned int scale) {
    for (int sy = 0; sy < scale; sy++) {
        for (int sx = 0; sx < scale; sx++) {
            lcd_draw_pixel(fb, x + sx, y + sy, color);
        }
    }
}

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