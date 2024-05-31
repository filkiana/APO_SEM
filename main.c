/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  main.c      - main file

  Author: Anatolii Filkin, 2024
  License: MIT License
 *******************************************************************/
/**
 * @file main.c
 * @brief Main file for the MicroZed based MZ_APO board project.
 */
#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "font_types.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "scanner.h"
#include "img_reader.h"
#include "menu.h"
#include "knob.h"
#include "diod.h"

// Function prototypes
unsigned char* initialize_memory(uint32_t phys_base, size_t size);
unsigned short* initialize_display(unsigned char* parlcd_mem_base);
void load_image(const char* file_path, gsl_matrix** gray_image, int* width, int* height);
void transform_image(gsl_matrix* src_mat, gsl_matrix* dst_mat, gsl_matrix* H, int width, int height);
void capture_points(uint16_t* xs, uint16_t* ys, unsigned char* spiled_base, unsigned short* fb, gsl_matrix* formatted_image, unsigned char* parlcd_mem_base);
void save_transformed_image(const gsl_matrix* image, const char* filename);
/**
 * @brief Application loop.
 */
void app_loop(void) {
  print_dir();

  unsigned char *spiled_base = initialize_memory(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE);
  unsigned char *parlcd_mem_base = initialize_memory(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE);

  unsigned short *fb = initialize_display(parlcd_mem_base);

  char file_path[100] = APP_DIR;
  char file_name[32] = {0};
  int file_number, width, height;

  printf("Choose file: ");
  scanf("%d", &file_number);
  get_file_name(file_name, file_number);
  strcat(file_path, file_name);
  printf("File path: %s\n", file_path);

  gsl_matrix *gray_image;
  load_image(file_path, &gray_image, &width, &height);

  gsl_matrix *src_mat = gsl_matrix_alloc(4, 2);
  gsl_matrix *dst_mat = gsl_matrix_alloc(4, 2);
  gsl_matrix *H = gsl_matrix_alloc(3, 3);
  gsl_matrix *formatted_image = gsl_matrix_alloc(A4_HEIGHT, A4_WIDTH);

  if (!gray_image) exit(1);

  transform_image(src_mat, dst_mat, H, width, height);
  apply_perspective_transform(gray_image, H, formatted_image, width, height);
  gsl_matrix_free(gray_image);

  uint16_t xs[4] = {0};
  uint16_t ys[4] = {0};
  capture_points(xs, ys, spiled_base, fb, formatted_image, parlcd_mem_base);

  for (int i = 0; i < 4; i++) {
    gsl_matrix_set(src_mat, i, 0, ys[i]);
    gsl_matrix_set(src_mat, i, 1, xs[i]);
  }

  compute_perspective_transform(src_mat, dst_mat, H);
  gsl_matrix *image_wrapped = gsl_matrix_alloc(A4_HEIGHT, A4_WIDTH);
  apply_perspective_transform(formatted_image, H, image_wrapped, A4_WIDTH, A4_HEIGHT);

  lcd_draw_image(fb, A4_WIDTH, A4_HEIGHT, image_wrapped);
  lcd_update_display(fb, parlcd_mem_base);

  save_transformed_image(image_wrapped, "wrapped_image.jpg");

  gsl_matrix_free(src_mat);
  gsl_matrix_free(dst_mat);
  gsl_matrix_free(H);
  gsl_matrix_free(image_wrapped);
  gsl_matrix_free(formatted_image);
}
/**
 * @brief Initialize memory mapping.
 *
 * @param phys_base The physical base address.
 * @param size The size of the memory region.
 * @return Pointer to the mapped memory.
 */
unsigned char* initialize_memory(uint32_t phys_base, size_t size) {
  unsigned char *mem_base = map_phys_address(phys_base, size, 0);
  if (mem_base == NULL) exit(1);
  return mem_base;
}
/**
 * @brief Initialize the display.
 *
 * @param parlcd_mem_base Base address of the LCD memory.
 * @return Pointer to the framebuffer.
 */
unsigned short* initialize_display(unsigned char* parlcd_mem_base) {
  unsigned short *fb = lcd_init(parlcd_mem_base);
  lcd_fill_screen(fb, lcd_color(255, 255, 255));
  lcd_update_display(fb, parlcd_mem_base);
  lcd_draw_text(fb, 10, 20, &font_winFreeSystem14x16, "Choose file", lcd_color(0, 0, 0), 5);
  lcd_update_display(fb, parlcd_mem_base);
  return fb;
}

/**
 * @brief Load an image from a file.
 *
 * @param file_path The path to the image file.
 * @param gray_image Pointer to the grayscale image matrix.
 * @param width Pointer to the width of the image.
 * @param height Pointer to the height of the image.
 */
void load_image(const char* file_path, gsl_matrix** gray_image, int* width, int* height) {
  *gray_image = read_image(file_path, width, height);
}
/**
 * @brief Transform an image using perspective transformation.
 *
 * @param src_mat Source matrix of points.
 * @param dst_mat Destination matrix of points.
 * @param H Homography matrix.
 * @param width The width of the image.
 * @param height The height of the image.
 */
void transform_image(gsl_matrix* src_mat, gsl_matrix* dst_mat, gsl_matrix* H, int width, int height) {
  gsl_matrix_set(src_mat, 0, 0, 0);        gsl_matrix_set(src_mat, 0, 1, 0);
  gsl_matrix_set(src_mat, 1, 0, width);    gsl_matrix_set(src_mat, 1, 1, 0);
  gsl_matrix_set(src_mat, 2, 0, width);    gsl_matrix_set(src_mat, 2, 1, height);
  gsl_matrix_set(src_mat, 3, 0, 0);        gsl_matrix_set(src_mat, 3, 1, height);

  gsl_matrix_set(dst_mat, 0, 0, 0);        gsl_matrix_set(dst_mat, 0, 1, 0);
  gsl_matrix_set(dst_mat, 1, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 1, 1, 0);
  gsl_matrix_set(dst_mat, 2, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 2, 1, A4_HEIGHT);
  gsl_matrix_set(dst_mat, 3, 0, 0);        gsl_matrix_set(dst_mat, 3, 1, A4_HEIGHT);

  compute_perspective_transform(src_mat, dst_mat, H);
}

/**
 * @brief Capture points using the knobs and display.
 *
 * @param xs Array to store the x-coordinates of the points.
 * @param ys Array to store the y-coordinates of the points.
 * @param spiled_base Base address of the SPILED.
 * @param fb Pointer to the framebuffer.
 * @param formatted_image The formatted image matrix.
 * @param parlcd_mem_base Base address of the LCD memory.
 */
void capture_points(uint16_t* xs, uint16_t* ys, unsigned char* spiled_base, unsigned short* fb, gsl_matrix* formatted_image, unsigned char* parlcd_mem_base) {
  uint16_t current_blue = *(spiled_base + BLUE_KNOB);
  uint16_t current_red = *(spiled_base + RED_KNOB);
  uint16_t current_x = 0;
  uint16_t current_y = 0;
  int green_press = 0;
  int set_points = 0;

  while (set_points < 4) {
    lcd_draw_image(fb, A4_WIDTH, A4_HEIGHT, formatted_image);
    int diff = get_knob_value(spiled_base, BLUE_KNOB, current_blue);
    current_x += diff;
    current_blue += diff;
    diff = get_knob_value(spiled_base, RED_KNOB, current_red);
    current_y += diff;
    current_red += diff;

    if (current_x < 0) current_x = A4_HEIGHT - 1;
    if (current_x > A4_HEIGHT) current_x = 0;
    if (current_y < 0) current_y = A4_WIDTH - 1;
    if (current_y > A4_WIDTH) current_y = 0;

    diod_set_color(spiled_base, gsl_matrix_get(formatted_image, current_x, current_y),
                   gsl_matrix_get(formatted_image, current_x, current_y),
                   gsl_matrix_get(formatted_image, current_x, current_y));

    printf("x: %d, y: %d\n", current_x, current_y);
    lcd_draw_plus(fb, current_x, current_y, lcd_color(127, 0, 0));
    lcd_update_display(fb, parlcd_mem_base);

    green_press = (*(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o) >> 25) & 1;
    if (green_press) {
      xs[set_points] = current_x;
      ys[set_points] = current_y;
      set_points++;
      printf("set_points: %d\n", set_points);
      while (green_press) {
        green_press = (*(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o) >> 25) & 1;
      }
    }
  }
}
/**
 * @brief Save the transformed image to a file.
 *
 * @param image The transformed image matrix.
 * @param filename The name of the output file.
 */
void save_transformed_image(const gsl_matrix* image, const char* filename) {
  clock_nanosleep(CLOCK_MONOTONIC, 0, &(struct timespec){.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000}, NULL);
  printf("Saving image\n");
  save_image(image, filename);
}
/**
 * @brief Main function.
 *
 * @return Exit status.
 */
int main(void) {
  app_loop();
  return 0;
}
