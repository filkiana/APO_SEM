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



int main(void) {
    int width, height;
    gsl_matrix *gray_image = read_image("/tmp/filkiana/IMG_6441.bmp", &width, &height);
    if (!gray_image) return EXIT_FAILURE;

    gsl_matrix *src_mat = gsl_matrix_alloc(4, 2);
    gsl_matrix *dst_mat = gsl_matrix_alloc(4, 2);
    // 6441
    gsl_matrix_set(src_mat, 0, 0, 1604); gsl_matrix_set(src_mat, 0, 1, 4032 - 920);
    gsl_matrix_set(src_mat, 1, 0, 2847); gsl_matrix_set(src_mat, 1, 1, 4032 - 1417);
    gsl_matrix_set(src_mat, 2, 0, 1457); gsl_matrix_set(src_mat, 2, 1, 4032 - 3162);
    gsl_matrix_set(src_mat, 3, 0, 77);   gsl_matrix_set(src_mat, 3, 1, 4032 - 2066);
    // 6437
    // gsl_matrix_set(src_mat, 0, 0, 890);  gsl_matrix_set(src_mat, 0, 1, 4032 - 620);
    // gsl_matrix_set(src_mat, 1, 0, 2182); gsl_matrix_set(src_mat, 1, 1, 4032 - 593);
    // gsl_matrix_set(src_mat, 2, 0, 2853); gsl_matrix_set(src_mat, 2, 1, 4032 - 915);
    // gsl_matrix_set(src_mat, 3, 0, 110);  gsl_matrix_set(src_mat, 3, 1, 4032 - 964);

    gsl_matrix_set(dst_mat, 0, 0, 0);        gsl_matrix_set(dst_mat, 0, 1, 0);
    gsl_matrix_set(dst_mat, 1, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 1, 1, 0);
    gsl_matrix_set(dst_mat, 2, 0, A4_WIDTH); gsl_matrix_set(dst_mat, 2, 1, A4_HEIGHT);
    gsl_matrix_set(dst_mat, 3, 0, 0);        gsl_matrix_set(dst_mat, 3, 1, A4_HEIGHT);
    gsl_matrix *H = gsl_matrix_alloc(3, 3);
    compute_perspective_transform(src_mat, dst_mat, H);
    gsl_matrix *image_wrapped = gsl_matrix_alloc(A4_HEIGHT, A4_WIDTH);
    apply_perspective_transform(gray_image, H, image_wrapped, width, height);
    
    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line = 5;
    int i;


    printf("Hello world\n");

    sleep(1);

    /*
    * Setup memory mapping which provides access to the peripheral
    * registers region of RGB LEDs, knobs and line of yellow LEDs.
    */
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

    /* If mapping fails exit with error code */
    if (mem_base == NULL)
        exit(1);
 struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
  for (i = 0; i < 30; i++) {
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    val_line <<= 1;
    printf("LED val 0x%x\n", val_line);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }

  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);

  unsigned short * fb = lcd_init(parlcd_mem_base);

    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 150 * 1000 * 1000;
    //draw an image
    lcd_draw_image(fb, A4_WIDTH, A4_HEIGHT, image_wrapped);
    lcd_update_display(fb,parlcd_mem_base);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    save_image(image_wrapped, "wrapped_image.bmp");

    gsl_matrix_free(src_mat);
    gsl_matrix_free(dst_mat);
    gsl_matrix_free(H);
    gsl_matrix_free(image_wrapped);
    gsl_matrix_free(gray_image);

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
