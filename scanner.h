#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "bmp_reader.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>


void print_matrix(const gsl_matrix *m, const char *name);


void compute_perspective_transform(const gsl_matrix *src, const gsl_matrix *dst, gsl_matrix *H);


void apply_perspective_transform(const gsl_matrix *gray_image, gsl_matrix *H, gsl_matrix *image_wrapped, int width, int height);


#endif /*SCANNER_H*/