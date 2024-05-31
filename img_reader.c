/**
 * @file img_reader.c
 * @brief Image reading and saving functions using JPEG format.
 */
#include "img_reader.h"
/**
 * @brief Initialize JPEG decompression.
 *
 * @param cinfo Pointer to the JPEG decompression structure.
 * @param infile Pointer to the input file.
 */
static void initialize_decompression(struct jpeg_decompress_struct *cinfo, FILE *infile) {
    struct jpeg_error_mgr jerr;
    cinfo->err = jpeg_std_error(&jerr);
    jpeg_create_decompress(cinfo);
    jpeg_stdio_src(cinfo, infile);
    jpeg_read_header(cinfo, TRUE);
    jpeg_start_decompress(cinfo);
}

/**
 * @brief Convert a row of pixels to grayscale and store it in the matrix.
 *
 * @param buffer Buffer containing the pixel data.
 * @param gray_image Matrix to store the grayscale image.
 * @param row_stride The row stride of the image.
 * @param row_index The current row index.
 * @param width The width of the image.
 */
static void convert_row_to_grayscale(JSAMPARRAY buffer, gsl_matrix *gray_image, int row_stride, int row_index, int width) {
    for (int x = 0; x < width; x++) {
        unsigned char r = buffer[0][x * 3];
        unsigned char g = buffer[0][x * 3 + 1];
        unsigned char b = buffer[0][x * 3 + 2];
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
        gsl_matrix_set(gray_image, row_index, x, gray);
    }
}

/**
 * @brief Initialize JPEG compression.
 *
 * @param cinfo Pointer to the JPEG compression structure.
 * @param outfile Pointer to the output file.
 * @param width The width of the image.
 * @param height The height of the image.
 */
static void initialize_compression(struct jpeg_compress_struct *cinfo, FILE *outfile, int width, int height) {
    struct jpeg_error_mgr jerr;
    cinfo->err = jpeg_std_error(&jerr);
    jpeg_create_compress(cinfo);
    jpeg_stdio_dest(cinfo, outfile);

    cinfo->image_width = width;
    cinfo->image_height = height;
    cinfo->input_components = 1;
    cinfo->in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(cinfo);
    jpeg_start_compress(cinfo, TRUE);
}

/**
 * @brief Write a row of grayscale values from the matrix.
 *
 * @param row_pointer Pointer to the row data.
 * @param image The grayscale image matrix.
 * @param row_index The current row index.
 * @param width The width of the image.
 */
static void write_grayscale_row(JSAMPROW row_pointer, const gsl_matrix *image, int row_index, int width) {
    for (int x = 0; x < width; x++) {
        row_pointer[x] = (unsigned char)gsl_matrix_get(image, row_index, x);
    }
}
/**
 * @brief Read an image from a file and convert it to grayscale.
 *
 * @param filename The name of the image file.
 * @param width Pointer to store the width of the image.
 * @param height Pointer to store the height of the image.
 * @return Pointer to the grayscale image matrix.
 */
gsl_matrix* read_image(const char *filename, int *width, int *height) {
    FILE *infile;
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return NULL;
    }

    struct jpeg_decompress_struct cinfo;
    initialize_decompression(&cinfo, infile);

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    int row_stride = cinfo.output_width * cinfo.output_components;

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    gsl_matrix *gray_image = gsl_matrix_alloc(*height, *width);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        convert_row_to_grayscale(buffer, gray_image, row_stride, cinfo.output_scanline - 1, *width);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return gray_image;
}
/**
 * @brief Save a grayscale image to a file.
 *
 * @param image The grayscale image matrix.
 * @param filename The name of the output file.
 */
void save_image(const gsl_matrix *image, const char *filename) {
    FILE *outfile;
    if ((outfile = fopen(filename, "wb")) == NULL) {
        perror("Error opening file");
        return;
    }

    struct jpeg_compress_struct cinfo;
    int width = image->size2;
    int height = image->size1;
    initialize_compression(&cinfo, outfile, width, height);

    JSAMPROW row_pointer = (JSAMPROW)malloc(width * sizeof(unsigned char));

    while (cinfo.next_scanline < cinfo.image_height) {
        write_grayscale_row(row_pointer, image, cinfo.next_scanline, width);
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    free(row_pointer);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}