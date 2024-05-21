#include "bmp_reader.h"


gsl_matrix* read_image(const char *filename, int *width, int *height) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr,"Error opening file\n");
        return NULL;
    }

    BITMAPFILEHEADER fileHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    if (fileHeader.bfType != 0x4D42) { 
        fclose(fp);
        fprintf(stderr, "Not a BMP file\n");
        return NULL;
    }

    BITMAPINFOHEADER infoHeader;
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
    if (infoHeader.biBitCount != 24) {
        fclose(fp);
        fprintf(stderr, "Only 24-bit BMP files are supported\n");
        return NULL;
    }

    *width = infoHeader.biWidth;
    *height = abs(infoHeader.biHeight);
    int row_padded = (*width * 3 + 3) & (~3);

    unsigned char *data = (unsigned char *)malloc(row_padded * (*height));
    fseek(fp, fileHeader.bfOffBits, SEEK_SET);
    fread(data, sizeof(unsigned char), row_padded * (*height), fp);
    fclose(fp);

    gsl_matrix *gray_image = gsl_matrix_alloc(*height, *width);
    for (int y = 0; y < *height; y++) {
        for (int x = 0; x < *width; x++) {
            int idx = (y * row_padded) + (x * 3);
            unsigned char r = data[idx + 2];
            unsigned char g = data[idx + 1];
            unsigned char b = data[idx];
            double gray = 0.299 * r + 0.587 * g + 0.114 * b;
            gsl_matrix_set(gray_image, y, x, gray);
        }
    }
    free(data);
    return gray_image;
}

void save_image(const gsl_matrix *image, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    BITMAPFILEHEADER fileHeader_wrapped = {0x4D42, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + A4_WIDTH * A4_HEIGHT, 0, 0, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)};
    BITMAPINFOHEADER infoHeader_wrapped = {sizeof(BITMAPINFOHEADER), A4_WIDTH, -A4_HEIGHT, 1, 8, 0, A4_WIDTH * A4_HEIGHT, 0, 0, 256, 256};

    fwrite(&fileHeader_wrapped, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&infoHeader_wrapped, sizeof(BITMAPINFOHEADER), 1, fp);

    for (int i = 0; i < 256; i++) {
        unsigned char grayscale[4] = {i, i, i, 0};
        fwrite(grayscale, sizeof(unsigned char), 4, fp);
    }

    unsigned char *output_image = (unsigned char *)malloc(A4_WIDTH * A4_HEIGHT);
    for (int y = 0; y < A4_HEIGHT; y++) {
        for (int x = 0; x < A4_WIDTH; x++) {
            output_image[y * A4_WIDTH + x] = (unsigned char)gsl_matrix_get(image, y, x);
        }
    }
    fwrite(output_image, sizeof(unsigned char), A4_WIDTH * A4_HEIGHT, fp);
    free(output_image);
    fclose(fp);
}