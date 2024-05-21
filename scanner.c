#include "scanner.h"

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

