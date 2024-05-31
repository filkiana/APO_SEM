/**
 * @file scanner.h
 * @brief Header file for matrix and transformation functions.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "img_reader.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

/**
 * @brief Print a matrix to the console.
 *
 * @param m The matrix to print.
 * @param name The name of the matrix.
 */
void print_matrix(const gsl_matrix *m, const char *name);

/**
 * @brief Compute the perspective transform matrix.
 *
 * @param src The source points matrix.
 * @param dst The destination points matrix.
 * @param H The homography matrix.
 */
void compute_perspective_transform(const gsl_matrix *src, const gsl_matrix *dst, gsl_matrix *H);

/**
 * @brief Apply a perspective transform to an image.
 *
 * @param gray_image The source grayscale image matrix.
 * @param H The homography matrix.
 * @param image_wrapped The destination image matrix.
 * @param width The width of the source image.
 * @param height The height of the source image.
 */
void apply_perspective_transform(const gsl_matrix *gray_image, gsl_matrix *H, gsl_matrix *image_wrapped, int width, int height);

#endif /* SCANNER_H */
