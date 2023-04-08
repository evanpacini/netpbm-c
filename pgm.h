#ifndef NETPBM_PGM_H
#define NETPBM_PGM_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/random.h>
#include "pbm.h"

#define PGM_MAX_GRAY 255
#define PGM_MAX_GRAY_F 255.0

// PGM image format
typedef struct {
    uint32_t width;
    uint32_t height;
    uint16_t max_gray;
    uint8_t *data;
} PGMImage;

// Threshold function
typedef uint8_t (*ThresholdFn)();

/**
 * Allocate memory for a PGM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PGMImage, or NULL if an error occurred.
 */
PGMImage *allocate_pgm(uint32_t width, uint32_t height) {
    // Allocate memory for image data
    PGMImage *image = (PGMImage *) calloc(1, sizeof(PGMImage));
    if (!image) {
        fprintf(stderr, "Error: out of memory\n");
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->max_gray = PGM_MAX_GRAY;
    image->data = (uint8_t *) calloc(1, width * height * sizeof(uint8_t));
    if (!image->data) {
        fprintf(stderr, "Error: out of memory\n");
        free(image);
        return NULL;
    }

    return image;
}

/**
 * Read a PGM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
PGMImage *read_pgm(const char *filename) {
    // Open file for reading
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return NULL;
    }

    // Read header (magic number, width, height, and max gray value)
    char magic[3];
    uint32_t width;
    uint32_t height;
    uint16_t max_gray;
    if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*[ \t\r\n]%hu%*1[ \t\r\n]", magic, &width, &height, &max_gray) != 4) {
        fprintf(stderr, "Error: invalid header in file '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Make sure the magic number is "P5" (binary PGM format)
    if (magic[0] != 'P' || magic[1] != '5') {
        fprintf(stderr, "Error: unsupported file format in file '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Make sure the max gray value is PGM_MAX_GRAY
    if (max_gray != PGM_MAX_GRAY) {
        fprintf(stderr, "Error: max gray value must be PGM_MAX_GRAY\n");
        fclose(fp);
        return NULL;
    }

    // Allocate memory for image data
    PGMImage *image = allocate_pgm(width, height);

    // Read pixel data
    if (fread(image->data, sizeof(uint8_t), width * height, fp) != width * height) {
        fprintf(stderr, "Error: could not read pixel data from file '%s'\n", filename);
        free(image->data);
        free(image);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return image;
}

/**
 * Threshold function that always returns 128.
 *
 * @return 128
 */
uint8_t middle_threshold() {
    return 128;
}

/**
 * Threshold function that returns a random value between 0 and 255.
 *
 * @return Random value between 0 and 255
 */
uint8_t random_threshold() {
    uint8_t random;
    getrandom(&random, sizeof(uint8_t), 0);
    return random;
}

/**
 * Convert a PGM image to a PBM image.
 *
 * @param image     The PGM image to convert.
 * @param threshold The threshold value to use for the conversion.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
 */
PBMImage *pgm_to_pbm(const PGMImage *image, ThresholdFn threshold) {
    // Allocate memory for new image data
    PBMImage *pbm_image = allocate_pbm(image->width, image->height);

    // Convert pixel data
    for (uint32_t i = 0; i < image->width * image->height; i++) {
        uint8_t pixel = image->data[i];
        pbm_image->data[i] = pixel < threshold();
    }

    return pbm_image;
}

/**
 * Convert a PGM image to a PBM image using Floyd–Steinberg dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
 */
PBMImage *pgm_to_pbm_floyd_steinberg(const PGMImage *image) {
    // Allocate memory for new image data
    PBMImage *pbm_image = allocate_pbm(image->width, image->height);

    // Normalize pixel data to [0, 1] double values
    double *double_data = (double *) malloc(image->width * image->height * sizeof(double));
    for (uint32_t y = 0; y < image->height; y++) {
        for (uint32_t x = 0; x < image->width; x++) {
            uint32_t pos = y * image->width + x;
            double_data[pos] = (double) image->data[pos] / PGM_MAX_GRAY_F;
        }
    }

    // Convert pixel data using Floyd-Steinberg dithering
    for (uint32_t y = 0; y < image->height; y++) {
        for (uint32_t x = 0; x < image->width; x++) {
            uint32_t pos = y * image->width + x;
            double oldPixel = double_data[pos];
            double newPixel = round(oldPixel);

            // Invert pixel value (PBM is white 0 and black 1)
            pbm_image->data[pos] = newPixel == 0;

            // Calculate error
            double error = oldPixel - newPixel;

            // Propagate error
            if (x < image->width - 1) {
                double_data[pos + 1] += error * 7 / 16;
            }
            if (y < image->height - 1) {
                if (x > 0) {
                    double_data[pos + image->width - 1] += error * 3 / 16;
                }
                double_data[pos + image->width] += error * 5 / 16;
                if (x < image->width - 1) {
                    double_data[pos + image->width + 1] += error * 1 / 16;
                }
            }
        }
    }

    free(double_data);
    return pbm_image;
}

/**
 * Write a PGM image to a file
 *
 * @param filename  Name of file to write to
 * @param image     Image to write
 * @return          True if successful, false otherwise
 */
bool write_pgm(const char *filename, const PGMImage *image) {
    // Open file for writing
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s' for writing\n", filename);
        return false;
    }

    // Write header (magic number, width, height, and max gray value)
    if (fprintf(fp, "P5\n%u\n%u\n%hu\n", image->width, image->height, image->max_gray) < 0) {
        fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
        fclose(fp);
        return false;
    }

    // Write pixel data
    if (fwrite(image->data, sizeof(uint8_t), image->width * image->height, fp) != image->width * image->height) {
        fprintf(stderr, "Error: could not write pixel data to file '%s'\n", filename);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/**
 * Free memory used by a PGM image.
 *
 * @param image     Image to free
 */
void free_pgm(PGMImage *image) {
    free(image->data);
    free(image);
}

#endif //NETPBM_PGM_H
