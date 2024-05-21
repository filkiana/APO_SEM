#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "bmp_reader.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

void print_matrix(const gsl_matrix *m, const char *name) {
    printf("Matrix %s:\n", name);
    for (size_t i = 0; i < m->size1; i++) {
        for (size_t j = 0; j < m->size2; j++) {
            printf("%f ", gsl_matrix_get(m, i, j));
        }
        printf("\n");
    }
}


void compute_perspective_transform(const gsl_matrix *src, const gsl_matrix *dst, gsl_matrix *H) {
    gsl_matrix *A = gsl_matrix_calloc(9, 9);
    for (int i = 0; i < 4; i++) {
        double sx = gsl_matrix_get(src, i, 0);
        double sy = gsl_matrix_get(src, i, 1);
        double dx = gsl_matrix_get(dst, i, 0);
        double dy = gsl_matrix_get(dst, i, 1);
        gsl_matrix_set(A, 2 * i, 0, -sx);
        gsl_matrix_set(A, 2 * i, 1, -sy);
        gsl_matrix_set(A, 2 * i, 2, -1);
        gsl_matrix_set(A, 2 * i, 3, 0);
        gsl_matrix_set(A, 2 * i, 4, 0);
        gsl_matrix_set(A, 2 * i, 5, 0);
        gsl_matrix_set(A, 2 * i, 6, sx * dx);
        gsl_matrix_set(A, 2 * i, 7, sy * dx);
        gsl_matrix_set(A, 2 * i, 8, dx);

        gsl_matrix_set(A, 2 * i + 1, 0, 0);
        gsl_matrix_set(A, 2 * i + 1, 1, 0);
        gsl_matrix_set(A, 2 * i + 1, 2, 0);
        gsl_matrix_set(A, 2 * i + 1, 3, -sx);
        gsl_matrix_set(A, 2 * i + 1, 4, -sy);
        gsl_matrix_set(A, 2 * i + 1, 5, -1);
        gsl_matrix_set(A, 2 * i + 1, 6, sx * dy);
        gsl_matrix_set(A, 2 * i + 1, 7, sy * dy);
        gsl_matrix_set(A, 2 * i + 1, 8, dy);
    }
    gsl_matrix_set_zero(H);
    gsl_matrix *V = gsl_matrix_calloc(9, 9);
    gsl_vector *S = gsl_vector_calloc(9);
    gsl_vector *work = gsl_vector_alloc(9);
    gsl_matrix *X = gsl_matrix_alloc(9, 9);
    
    gsl_linalg_SV_decomp_mod(A, X, V, S, work);
    for (int i = 0; i < 9; i++) {
        double val = gsl_matrix_get(V, i, 8);
        gsl_matrix_set(H, i / 3, i % 3, val);
    }

    gsl_matrix_free(A);
    gsl_matrix_free(V);
    gsl_matrix_free(X);
    gsl_vector_free(S);
    gsl_vector_free(work);
}


void apply_perspective_transform(const gsl_matrix *gray_image, gsl_matrix *H, gsl_matrix *image_wrapped, int width, int height) {
    gsl_matrix *M = gsl_matrix_alloc(3, 3);
    gsl_matrix_memcpy(M, H);

    gsl_matrix *M_inv = gsl_matrix_alloc(3, 3);
    gsl_permutation *p = gsl_permutation_alloc(3);
    int signum;
    gsl_linalg_LU_decomp(M, p, &signum);
    gsl_linalg_LU_invert(M, p, M_inv);
    for (int y = 0; y < A4_HEIGHT; y++) {
        for (int x = 0; x < A4_WIDTH; x++) {
            gsl_vector *pt = gsl_vector_alloc(3);
            gsl_vector_set(pt, 0, x);
            gsl_vector_set(pt, 1, y);
            gsl_vector_set(pt, 2, 1);
            
            gsl_vector *pt_transformed = gsl_vector_alloc(3);
            gsl_blas_dgemv(CblasNoTrans, 1.0, M_inv, pt, 0.0, pt_transformed);
            
            double x_transformed = gsl_vector_get(pt_transformed, 0) / gsl_vector_get(pt_transformed, 2);
            double y_transformed = gsl_vector_get(pt_transformed, 1) / gsl_vector_get(pt_transformed, 2);
            
            int px = (int)x_transformed;
            int py = (int)y_transformed;
            if (px >= 0 && px < width && py >= 0 && py < height) {
                double val = gsl_matrix_get(gray_image, py, px);
                gsl_matrix_set(image_wrapped, y, x, val);
            }
            gsl_vector_free(pt);
            gsl_vector_free(pt_transformed);
        }
    }
    
    gsl_matrix_free(M);
    gsl_matrix_free(M_inv);
    gsl_permutation_free(p);
}

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
    int i, j, k;
    int ptr;
    unsigned int c;

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
    for (int i = 0; i < A4_HEIGHT; i++) {
        for (int j = 0; j < A4_WIDTH; j++) {
            c = (unsigned int)gsl_matrix_get(image_wrapped, i, j);
            //RGB 565 - 
            c = lcd_grey((uint8_t)c);
            lcd_draw_pixel(fb, i, j, c);

        }
    }
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