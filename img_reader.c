#include "img_reader.h"
// Helper function to read JPEG header and initialize decompression
static void initialize_decompression(struct jpeg_decompress_struct *cinfo, FILE *infile) {
    struct jpeg_error_mgr jerr;
    cinfo->err = jpeg_std_error(&jerr);
    jpeg_create_decompress(cinfo);
    jpeg_stdio_src(cinfo, infile);
    jpeg_read_header(cinfo, TRUE);
    jpeg_start_decompress(cinfo);
}

// Helper function to convert a row of pixels to grayscale and store in the matrix
static void convert_row_to_grayscale(JSAMPARRAY buffer, gsl_matrix *gray_image, int row_stride, int row_index, int width) {
    for (int x = 0; x < width; x++) {
        unsigned char r = buffer[0][x * 3];
        unsigned char g = buffer[0][x * 3 + 1];
        unsigned char b = buffer[0][x * 3 + 2];
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
        gsl_matrix_set(gray_image, row_index, x, gray);
    }
}

// Helper function to initialize JPEG compression
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

// Helper function to write a row of grayscale values from the matrix
static void write_grayscale_row(JSAMPROW row_pointer, const gsl_matrix *image, int row_index, int width) {
    for (int x = 0; x < width; x++) {
        row_pointer[x] = (unsigned char)gsl_matrix_get(image, row_index, x);
    }
}

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