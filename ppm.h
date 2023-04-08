#ifndef NETPBM_PPM_H
#define NETPBM_PPM_H

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pgm.h"

// Pixel struct
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

// PPM image format
typedef struct {
    uint32_t width;
    uint32_t height;
    uint16_t max_color;
    Pixel *data;
} PPMImage;

/**
 * Read a PPM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
PPMImage *read_ppm(const char *filename) {
    // Open file for reading
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return NULL;
    }

    // Read header (magic number, width, height, and max color value)
    char magic[3];
    uint32_t width;
    uint32_t height;
    uint16_t max_color;
    if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*[ \t\r\n]%hu%*1[ \t\r\n]", magic, &width, &height, &max_color) !=
        // NOLINT(cert-err34-c)
        4) {
        fprintf(stderr, "Error: invalid header in file '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Make sure the magic number is "P6" (binary PPM format)
    if (magic[0] != 'P' || magic[1] != '6') {
        fprintf(stderr, "Error: unsupported file format in file '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Make sure the max color value is 255
    if (max_color != 255) {
        fprintf(stderr, "Error: max color value must be 255\n");
        fclose(fp);
        return NULL;
    }

    // Allocate memory for image data
    PPMImage *image = (PPMImage *) malloc(sizeof(PPMImage));
    if (!image) {
        fprintf(stderr, "Error: out of memory\n");
        fclose(fp);
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->max_color = max_color;
    image->data = (Pixel *) malloc(width * height * sizeof(Pixel));
    if (!image->data) {
        fprintf(stderr, "Error: out of memory\n");
        free(image);
        fclose(fp);
        return NULL;
    }


    // Read pixel data
    if (fread(image->data, sizeof(Pixel), width * height, fp) != width * height) {
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
 * Write a PPM image to a file.
 *
 * @param filename  The name of the file to write.
 * @param image     The image data.
 * @return          true if the image was written successfully, false otherwise.
 */
bool write_ppm(const char *filename, const PPMImage *image) {
    // Open file for writing
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s' for writing\n", filename);
        return false;
    }

    // Write magic number, width, height, and max color value
    if (fprintf(fp, "P6\n%u\n%u\n%hu\n", image->width, image->height, image->max_color) < 0) {
        fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
        fclose(fp);
        return false;
    }

    // Write pixel data
    if (fwrite(image->data, sizeof(uint8_t), image->width * image->height * 3, fp) !=
        image->width * image->height * 3) {
        fprintf(stderr, "Error: could not write pixel data to file '%s'\n", filename);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/**
 * Free the memory used by an image.
 *
 * @param image The image to free.
 */
void free_ppm(PPMImage *image) {
    free(image->data);
    free(image);
}

/**
 * Calculate the linear_luminance of a pixel.
 *
 * @param p The pixel.
 * @return The linear_luminance.
 */
double linear_luminance(const Pixel *p) {
    return 0.299 * p->r + 0.587 * p->g + 0.114 * p->b;
}

/**
 * Calculate the sRGB_luminance of a pixel.
 *
 * @param p The pixel.
 * @return The sRGB_luminance.
 */
double sRGB_luminance(const Pixel *p) {
    return 0.2126 * p->r + 0.7152 * p->g + 0.0722 * p->b;
}

/**
 * Convert sRGB value to linear RGB value
 *
 * @param sRGB sRGB value
 * @return Linear RGB value
 */
double linearRGB_value(double sRGB) {
    return sRGB <= 0.04045 ? sRGB / 12.92 : pow(((sRGB + 0.055) / 1.055), 2.4);
}

/**
 * Convert linear RGB value to sRGB value
 *
 * @param linearRGB Linear RGB value
 * @return sRGB value
 */
double sRGB_value(double linearRGB) {
    return linearRGB <= 0.0031308 ? 12.92 * linearRGB : 1.055 * pow(linearRGB, 1.0 / 2.4) - 0.055;
}

/**
 * Convert sRGB color to linear RGB color
 *
 * @param sRGB sRGB Pixel
 */
void linearRGB(Pixel *sRGB) {
    sRGB->r = (uint8_t) (linearRGB_value(sRGB->r / 255.0) * 255.0);
    sRGB->g = (uint8_t) (linearRGB_value(sRGB->g / 255.0) * 255.0);
    sRGB->b = (uint8_t) (linearRGB_value(sRGB->b / 255.0) * 255.0);
}

/**
 * Convert linear RGB color to sRGB color
 *
 * @param linearRGB Linear RGB color
 */
void sRGB(Pixel *linearRGB) {
    linearRGB->r = (uint8_t) (sRGB_value(linearRGB->r / 255.0) * 255.0);
    linearRGB->g = (uint8_t) (sRGB_value(linearRGB->g / 255.0) * 255.0);
    linearRGB->b = (uint8_t) (sRGB_value(linearRGB->b / 255.0) * 255.0);
}

/**
 * Convert an image to a new image using the given pixel conversion function
 *
 * @param image Pointer to the original image
 * @param conversionFn Reference to the pixel conversion function
 * @return Pointer to the new image
 */
PPMImage *ppm_pixel_convert(PPMImage *image, void (*conversionFn)(Pixel *)) {
    // Allocate memory for the new image
    PPMImage *newImage = (PPMImage *) malloc(sizeof(PPMImage));

    // Copy the image header to the new image
    memcpy(newImage, image, sizeof(PPMImage));

    // Allocate memory for the pixel data of the new image
    newImage->data = (Pixel *) malloc(sizeof(Pixel) * newImage->width * newImage->height);

    // Copy the pixel data of the original image to the new image
    memcpy(newImage->data, image->data, sizeof(Pixel) * newImage->width * newImage->height);

    // Convert the pixel data of the new image using the given conversion function
    for (int i = 0; i < newImage->width * newImage->height; i++)
        conversionFn(&newImage->data[i]);

    return newImage;
}

/**
 * Convert an image to a new image using the given pixel conversion function.
 *
 * @param image     Pointer to the original image
 * @param luminance Reference to the luminance function
 * @return          Pointer to the new image
 */
PGMImage *ppm_to_pgm(const PPMImage *image, double (*luminance)(const Pixel *)) {
    /* Allocate memory for PGM image */
    PGMImage *pgm_image = (PGMImage *) malloc(sizeof(PGMImage));
    if (!pgm_image) {
        fprintf(stderr, "Error: could not allocate memory for PGM image\n");
        return NULL;
    }

    // Copy image header to PGM image
    memcpy(pgm_image, image, sizeof(PPMImage));

    // Allocate memory for image data
    pgm_image->data = (uint8_t *) malloc(pgm_image->width * pgm_image->height * sizeof(uint8_t) + pgm_image->height);
    if (!pgm_image->data) {
        fprintf(stderr, "Error: could not allocate memory for PGM image data\n");
        free(pgm_image);
        return NULL;
    }

    // Convert pixel data from PPM image to PGM image
    for (uint32_t i = 0; i < pgm_image->height; i++) {
        for (uint32_t j = 0; j < pgm_image->width; j++) {
            // Get pixel from PPM image
            Pixel p = image->data[i * image->width + j];

            // Calculate linear_luminance of pixel
            uint8_t y = (uint8_t) luminance(&p);

            // Set linear_luminance value in PGM image
            pgm_image->data[i * pgm_image->width + j] = y;
        }
    }

    return pgm_image;
}

#endif //NETPBM_PPM_H
