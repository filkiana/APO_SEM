#ifndef IMG_READER_H
#define IMG_READER_H
#define A4_WIDTH 320
#define A4_HEIGHT 480

#include <stdint.h>
#include <gsl/gsl_matrix.h>
#include <jpeglib.h>

gsl_matrix* read_image(const char *filename, int *width, int *height);
void save_image(const gsl_matrix *image, const char *filename);

#endif /*IMG_READER_H*/