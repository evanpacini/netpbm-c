#ifndef NETPBM_PGM_H
#define NETPBM_PGM_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// PGM image format
typedef struct {
    uint32_t width;
    uint32_t height;
    uint16_t max_gray;
    uint8_t *data;
} PGMImage;

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

    // Make sure the max gray value is 255
    if (max_gray != 255) {
        fprintf(stderr, "Error: max gray value must be 255\n");
        fclose(fp);
        return NULL;
    }

    // Allocate memory for image data
    PGMImage *image = (PGMImage *) malloc(sizeof(PGMImage));
    if (!image) {
        fprintf(stderr, "Error: out of memory\n");
        fclose(fp);
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->max_gray = max_gray;
    image->data = (uint8_t *) malloc(width * height * sizeof(uint8_t));
    if (!image->data) {
        fprintf(stderr, "Error: out of memory\n");
        free(image);
        fclose(fp);
        return NULL;
    }

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
 * Free memory used by a PGM image
 *
 * @param image     Image to free
 */
void free_pgm(PGMImage *image) {
    free(image->data);
    free(image);
}

#endif //NETPBM_PGM_H
