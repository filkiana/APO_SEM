/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  main.c      - main file

  include your name there and license for distribution.
  Anatolii Filkin, 2024
 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "scanner.h"
#include "bmp_reader.h"
#include "menu.h"
#include "knob.h"



void app_loop(void){
  print_dir();
  char file_name[32] = {0};
  int file_number;
  printf("choose file: ");
  scanf("%d", &file_number);
  get_file_name(&file_name, file_number);
  int width, height;
  // concat APP_DIR and file_name
  char file_path[100] = APP_DIR;
  strcat(file_path, file_name);
  printf("file path: %s\n", file_path);
  gsl_matrix *gray_image = read_image(file_path, &width, &height);
  
  gsl_matrix *src_mat = gsl_matrix_alloc(4, 2);
  gsl_matrix *dst_mat = gsl_matrix_alloc(4, 2);

  if (!gray_image) return EXIT_FAILURE;

 
  //transform from width height to WIDTH HEIGHT
  gsl_matrix_set(src_mat, 0, 0, 0);        gsl_matrix_set(src_mat, 0, 1, 0);
  gsl_matrix_set(src_mat, 1, 0, width); gsl_matrix_set(src_mat, 1, 1, 0);
  gsl_matrix_set(src_mat, 2, 0, width); gsl_matrix_set(src_mat, 2, 1, height);
  gsl_matrix_set(src_mat, 3, 0, 0);        gsl_matrix_set(src_mat, 3, 1, height);

  gsl_matrix_set(dst_mat, 0, 0, 0);        gsl_matrix_set(dst_mat, 0, 1, 0);
  gsl_matrix_set(dst_mat, 1, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 1, 1, 0);
  gsl_matrix_set(dst_mat, 2, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 2, 1, A4_HEIGHT);
  gsl_matrix_set(dst_mat, 3, 0, 0);        gsl_matrix_set(dst_mat, 3, 1, A4_HEIGHT);
  gsl_matrix *H = gsl_matrix_alloc(3, 3);
  compute_perspective_transform(src_mat, dst_mat, H);
  gsl_matrix *formatted_image = gsl_matrix_alloc(A4_HEIGHT, A4_WIDTH);
  apply_perspective_transform(gray_image, H, formatted_image, width, height);
  gsl_matrix_free(gray_image);
unsigned char *mem_base;
  unsigned char *parlcd_mem_base;
  unsigned char* spiled_base;

  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (spiled_base == NULL)
    exit(1);
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  if (mem_base == NULL)
    exit(1);

  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);

  unsigned short * fb = lcd_init(parlcd_mem_base);
  struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
  uint16_t current_blue = *(spiled_base + BLUE_KNOB);
  uint16_t current_red = *(spiled_base + RED_KNOB);
  uint16_t current_x = 0;
  uint16_t current_y = 0;
  int green_press = 0;
  int set_points = 0;
  uint16_t xs[4] = {0};
  uint16_t ys[4] = {0};
  while (set_points < 4){
    lcd_draw_image(fb, A4_WIDTH, A4_HEIGHT, formatted_image);
    int diff = get_knob_value(spiled_base, BLUE_KNOB, current_blue);
    current_x += diff;
    current_blue += diff;
    diff = get_knob_value(spiled_base, RED_KNOB, current_red);
    current_y += diff;
    current_red += diff;
    if (current_x < 0)
      current_x = A4_HEIGHT - 1;
    if(current_x > A4_HEIGHT)
      current_x = 0;
    if (current_y < 0)
      current_y = A4_WIDTH - 1;
    if(current_y > A4_WIDTH)
      current_y = 0;
    
    printf("x: %d, y: %d\n", current_x, current_y);
    lcd_draw_plus(fb, current_x, current_y, lcd_color(127, 0, 0));
    lcd_update_display(fb,parlcd_mem_base);
    green_press = (*(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o) >> 25) & 1;
    if (green_press){
      xs[set_points] = current_x;
      ys[set_points] = current_y;
      set_points++;
      printf("set_points: %d\n", set_points);
      while (green_press){
        green_press = (*(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o) >> 25) & 1;
      }
    }
  } 
  for(int i = 0; i < 4; i++){
    printf("x: %d, y: %d\n", xs[i], ys[i]);
    gsl_matrix_set(src_mat, i, 0, ys[i]);
    gsl_matrix_set(src_mat, i, 1, xs[i]);
  }
  compute_perspective_transform(src_mat, dst_mat, H);
  printf("got H\n");
  gsl_matrix* image_wrapped = gsl_matrix_alloc(A4_HEIGHT, A4_WIDTH);
  apply_perspective_transform(formatted_image, H, image_wrapped, A4_WIDTH, A4_HEIGHT);
  printf("got wrapped image\n");
  lcd_draw_image(fb, A4_WIDTH, A4_HEIGHT, image_wrapped);
  lcd_update_display(fb,parlcd_mem_base);
  clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  printf("saving image\n");
  save_image(image_wrapped, "wrapped_image.bmp");

  gsl_matrix_free(src_mat);
  gsl_matrix_free(dst_mat);
  gsl_matrix_free(H);
  gsl_matrix_free(image_wrapped);
  gsl_matrix_free(formatted_image);

}





int main(void) {
  app_loop();
  
    
  
  return 0;
}






// int main(int argc, char *argv[])
// {

//   /* Serialize execution of applications */

//   /* Try to acquire lock the first */
//   if (serialize_lock(1) <= 0) {
//     printf("System is occupied\n");

//     if (1) {
//       printf("Waitting\n");
//       /* Wait till application holding lock releases it or exits */
//       serialize_lock(0);
//     }
//   }

//   printf("Hello world\n");

//   sleep(4);

//   printf("Goodbye world\n");

//   /* Release the lock */
//   serialize_unlock();

//   return 0;
// }
