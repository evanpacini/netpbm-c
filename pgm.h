#ifndef NETPBM__PGM_H_
#define NETPBM__PGM_H_

#include "types/pbm.h"
#include "types/pgm.h"
#include "types/ppm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Allocate memory for a PGM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PgmImage, or NULL if an error occurred.
 */
PgmImage *AllocatePgm(uint32_t width, uint32_t height) {
  // Allocate memory for image data
  PgmImage *image = (PgmImage *) calloc(1, sizeof(PgmImage));
  if (!image) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  image->width_ = width;
  image->height_ = height;
  image->max_gray_ = PGM_MAX_GRAY;
  image->data_ = (uint8_t *) calloc(1, width * height * sizeof(uint8_t));
  if (!image->data_) {
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
PgmImage *ReadPgm(const char *filename) {
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
  if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*[ \t\r\n]%hu%*1[ \t\r\n]",
             magic, &width, &height, &max_gray) != 4) {
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
  PgmImage *image = AllocatePgm(width, height);

  // Read pixel data
  if (fread(image->data_, sizeof(uint8_t), width * height, fp) !=
      width * height) {
    fprintf(stderr, "Error: could not read pixel data from file '%s'\n",
            filename);
    free(image->data_);
    free(image);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  return image;
}

/**
 * Convert an image to a new image using the given pixel conversion function.
 *
 * @param image     Pointer to the original image
 * @param luminance Reference to the luminance function
 * @return          Pointer to the new image
 */
PgmImage *PpmToPgm(const PpmImage *image, LuminanceFn luminance) {
  /* Allocate memory for PGM image */
  PgmImage *pgm_image = AllocatePgm(image->width_, image->height_);
  if (!pgm_image) {
    fprintf(stderr, "Error: could not allocate memory for PGM image\n");
    return NULL;
  }

// Convert pixel data from PPM image to PGM image
#pragma omp parallel for default(none) shared(pgm_image, image, luminance)
  for (uint32_t i = 0; i < pgm_image->height_ * pgm_image->width_; i++) {
    // Get pixel from PPM image
    Pixel p = image->data_[i];

    // Calculate luminance of pixel
    uint8_t y = (uint8_t) luminance(&p);

    // Set luminance value in PGM image
    pgm_image->data_[i] = y;
  }

  return pgm_image;
}

/**
 * Convert a PBM image to a PGM image.
 *
 * @param image     Pointer to the image data
 * @return          Pointer to the new image data
 */
PgmImage *PbmToPgm(const PbmImage *image) {
  // Allocate memory for image data
  PgmImage *pgm = AllocatePgm(image->width_, image->height_);
  if (!pgm) {
    return NULL;
  }

  // Convert pixel data
#pragma omp parallel for default(none) shared(pgm, image)
  for (uint32_t i = 0; i < image->height_ * image->width_; i++) {
    // Set pixel value in PGM image
    pgm->data_[i] = image->data_[i] ? 0 : PGM_MAX_GRAY;
  }

  return pgm;
}

/**
 * Blur effect first designed by KaspervanM in Jan 22, 2021
 * Each pixel becomes the average of a square surrounding that pixel with side
 * 2r + 1.
 * @param image Input PgmImage
 * @param radius Radius of the square
 * @return
 */
PgmImage *KasperBlur(PgmImage *image, int8_t radius) {
  // Allocate memory for new image data
  PgmImage *new_image = AllocatePgm(image->width_, image->height_);

// Blur pixel data
#pragma omp parallel for default(none) shared(image, new_image, radius)        \
    collapse(2)
  for (uint32_t y = 0; y < image->height_; y++) {
    for (uint32_t x = 0; x < image->width_; x++) {
      uint64_t sum = 0;
      uint16_t count = 0;
      for (int64_t ry = -radius; ry <= radius; ry++) {
        for (int64_t rx = -radius; rx <= radius; rx++) {
          if (y + ry >= 0 && y + ry < image->height_ && x + rx >= 0 &&
              x + rx < image->width_) {
            sum += image->data_[(y + ry) * image->width_ + (x + rx)];
            count++;
          }
        }
      }
      new_image->data_[y * image->width_ + x] = (uint8_t) (sum / count);
    }
  }

  return new_image;
}

/**
 * Write a PGM image to a file
 *
 * @param filename  Name of file to write to
 * @param image     Image to write
 * @return          True if successful, false otherwise
 */
bool WritePgm(const char *filename, const PgmImage *image) {
  // Open file for writing
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Error: could not open file '%s' for writing\n", filename);
    return false;
  }

  // Write header (magic number, width, height, and max gray value)
  if (fprintf(fp, "P5\n%u\n%u\n%hu\n", image->width_, image->height_,
              image->max_gray_) < 0) {
    fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
    fclose(fp);
    return false;
  }

  // Write pixel data
  if (fwrite(image->data_, sizeof(uint8_t), image->width_ * image->height_,
             fp) != image->width_ * image->height_) {
    fprintf(stderr, "Error: could not write pixel data to file '%s'\n",
            filename);
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
void FreePgm(PgmImage *image) {
  free(image->data_);
  free(image);
}

#endif // NETPBM__PGM_H_
