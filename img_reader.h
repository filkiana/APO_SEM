/**
 * @file img_reader.h
 * @brief Header file for image reading and saving functions.
 */
#ifndef IMG_READER_H
#define IMG_READER_H
#define A4_WIDTH 320
#define A4_HEIGHT 480

#include <stdint.h>
#include <gsl/gsl_matrix.h>
#include <jpeglib.h>
/**
 * @brief Read an image from a file and convert it to grayscale.
 *
 * @param filename The name of the image file.
 * @param width Pointer to store the width of the image.
 * @param height Pointer to store the height of the image.
 * @return Pointer to the grayscale image matrix.
 */
gsl_matrix* read_image(const char *filename, int *width, int *height);
/**
 * @brief Save a grayscale image to a file.
 *
 * @param image The grayscale image matrix.
 * @param filename The name of the output file.
 */
void save_image(const gsl_matrix *image, const char *filename);

#endif /*IMG_READER_H*/