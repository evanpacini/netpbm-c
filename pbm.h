#ifndef NETPBM__PBM_H_
#define NETPBM__PBM_H_

#include "bayer.h"
#include "types/pbm.h"
#include "types/pgm.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined __GLIBC__ && defined __linux__

#include <sys/random.h>

ssize_t MyRandom(void *buf, size_t len) {
  return getrandom(buf, len, 0);
}

#else /* not linux */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t MyRandom(void *buf, size_t len) {
  unsigned char *buffer = (unsigned char *)buf;
  size_t i;

  for (i = 0; i < len; ++i) {
    buffer[i] = (unsigned char)rand();
  }

  return (ssize_t)len;
}

#endif

/**
 * Allocate memory for a PBM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PbmImage, or NULL if an error occurred.
 */
PbmImage *AllocatePbm(uint32_t width, uint32_t height) {
  // Allocate memory for image data
  PbmImage *image = (PbmImage *)malloc(sizeof(PbmImage));
  if (!image) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  image->width_ = width;
  image->height_ = height;
  image->data_ = (uint8_t *)malloc(width * height * sizeof(uint8_t));
  if (!image->data_) {
    fprintf(stderr, "Error: out of memory\n");
    free(image);
    return NULL;
  }
  return image;
}

/**
 * Read a PBM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
PbmImage *ReadPbm(const char *filename) {
  // Open file for reading
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error: could not open file '%s'\n", filename);
    return NULL;
  }

  // Read header (magic number, width, and height)
  char magic[3];
  uint32_t width;
  uint32_t height;
  if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*1[ \t\r\n]", magic, &width,
             &height) != 3) {
    fprintf(stderr, "Error: invalid header in file '%s'\n", filename);
    fclose(fp);
    return NULL;
  }

  // Make sure the magic number is "P4" (binary PBM format)
  if (magic[0] != 'P' || magic[1] != '4') {
    fprintf(stderr, "Error: unsupported file format in file '%s'\n", filename);
    fclose(fp);
    return NULL;
  }

  // Allocate memory for buffer
  size_t buffer_size = (size_t)((width * height + 7) / 8);
  uint8_t *buffer = (uint8_t *)malloc(buffer_size);
  if (!buffer) {
    fprintf(stderr, "Error: out of memory\n");
    fclose(fp);
    return NULL;
  }

  // Read pixel data into buffer
  if (fread(buffer, sizeof(uint8_t), buffer_size, fp) != buffer_size) {
    fprintf(stderr, "Error: failed to read pixel data from file '%s'\n",
            filename);
    free(buffer);
    fclose(fp);
    return NULL;
  }

  // Close file
  fclose(fp);

  // Allocate memory for image data
  PbmImage *image = AllocatePbm(width, height);

// Decode the pixel data from the buffer
#pragma omp parallel for default(none) shared(image, buffer, width, height)
  for (size_t i = 0; i < width * height; i += 8) {
    // Read the next byte from the buffer
    uint8_t byte = buffer[i / 8];

    // Decode the byte into the image data (loop through the 8 bits)
    for (size_t j = 0; j < 8; j++) {
      size_t index = i + j;
      if (index < width * height) {
        image->data_[index] = (byte & (1 << (7 - j))) >> (7 - j);
      }
    }
  }

  free(buffer);
  return image;
}

/**
 * Normalizes pixel values from 0-255 to double 0-1.
 *
 * @param image Input PgmImage
 * @return Normalized image data
 */
double *NormalizePgm(const PgmImage *image) {
  double *double_data =
      (double *)malloc(image->width_ * image->height_ * sizeof(double));
#pragma omp parallel for default(none) shared(image, double_data)
  for (uint32_t i = 0; i < image->height_ * image->width_; i++)
    double_data[i] = (double)image->data_[i] / PGM_MAX_GRAY_F;
  return double_data;
}

/**
 * Threshold function that always returns 128.
 *
 * @return 128
 */
uint8_t MiddleThreshold() { return 128; }

/**
 * Threshold function that returns a random value between 0 and 255.
 *
 * @return Random value between 0 and 255
 */
uint8_t RandomThreshold() {
  uint8_t random;
  MyRandom(&random, sizeof(uint8_t));
  return random;
}

/**
 * Convert a PGM image to a PBM image.
 *
 * @param image     The PGM image to convert.
 * @param threshold The threshold function (0-255) to use for the conversion.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
PbmImage *PgmToPbm(const PgmImage *image, ThresholdFn threshold) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

// Convert pixel data
#pragma omp parallel for default(none) shared(image, pbm_image, threshold)
  for (uint32_t i = 0; i < pbm_image->width_ * pbm_image->height_; i++)
    pbm_image->data_[i] = image->data_[i] < threshold();

  return pbm_image;
}

/**
 * Convert a PGM image to a PBM image using Atkinson dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
PbmImage *PgmToPbmAtkinson(const PgmImage *image) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

  // Normalize pixel data to [0, 1] double values
  double *double_data = NormalizePgm(image);

  // Convert pixel data using Atkinson dithering
  for (uint32_t y = 0; y < pbm_image->height_; y++) {
    for (uint32_t x = 0; x < pbm_image->width_; x++) {
      uint32_t pos = y * pbm_image->width_ + x;
      double old_pixel = double_data[pos];
      double new_pixel = round(old_pixel);

      // Invert pixel value (PBM is white 0 and black 1)
      pbm_image->data_[pos] = new_pixel == 0;

      // Calculate error
      double error = old_pixel - new_pixel;

      // Propagate error
      if (x < pbm_image->width_ - 1) {
        double_data[pos + 1] += error * 1 / 8;
      }
      if (y < pbm_image->height_ - 1) {
        if (x > 0) {
          double_data[pos + pbm_image->width_ - 1] += error * 1 / 8;
        }
        double_data[pos + pbm_image->width_] += error * 1 / 8;
        if (x < pbm_image->width_ - 1) {
          double_data[pos + pbm_image->width_ + 1] += error * 1 / 8;
        }
        if (x < pbm_image->width_ - 2) {
          double_data[pos + pbm_image->width_ + 2] += error * 1 / 8;
        }
      }
      if (y < pbm_image->height_ - 2) {
        if (x > 0) {
          double_data[pos + 2 * pbm_image->width_ - 1] += error * 1 / 8;
        }
        double_data[pos + 2 * pbm_image->width_] += error * 1 / 8;
        if (x < pbm_image->width_ - 1) {
          double_data[pos + 2 * pbm_image->width_ + 1] += error * 1 / 8;
        }
      }
    }
  }

  free(double_data);
  return pbm_image;
}

/**
 * Convert a PGM image to a PBM image using Bayer (Ordered) Dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
PbmImage *PgmToPbmBayer(const PgmImage *image) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

  // Normalize pixel data to [0, 1] double values
  double *double_data = NormalizePgm(image);

// Convert using Bayer (Ordered) Dithering
#pragma omp parallel for default(none)                                         \
    shared(kBayer8X8, pbm_image, double_data) collapse(2)
  for (uint32_t y = 0; y < pbm_image->height_; y++) {
    for (uint32_t x = 0; x < pbm_image->width_; x++) {
      uint32_t pos = y * pbm_image->width_ + x;
      pbm_image->data_[pos] = double_data[pos] < kBayer8X8[x & 7][y & 7];
    }
  }

  return pbm_image;
}

/**
 * Convert a PGM image to a PBM image using Floyd–Steinberg dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
PbmImage *PgmToPbmFloydSteinberg(const PgmImage *image) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

  // Normalize pixel data to [0, 1] double values
  double *double_data = NormalizePgm(image);

  // Convert pixel data using Floyd-Steinberg dithering
  for (uint32_t y = 0; y < pbm_image->height_; y++) {
    for (uint32_t x = 0; x < pbm_image->width_; x++) {
      uint32_t pos = y * pbm_image->width_ + x;
      double old_pixel = double_data[pos];
      double new_pixel = round(old_pixel);

      // Invert pixel value (PBM is white 0 and black 1)
      pbm_image->data_[pos] = new_pixel == 0;

      // Calculate error
      double error = old_pixel - new_pixel;

      // Propagate error
      if (x < pbm_image->width_ - 1) {
        double_data[pos + 1] += error * 7 / 16;
      }
      if (y < pbm_image->height_ - 1) {
        if (x > 0) {
          double_data[pos + pbm_image->width_ - 1] += error * 3 / 16;
        }
        double_data[pos + pbm_image->width_] += error * 5 / 16;
        if (x < pbm_image->width_ - 1) {
          double_data[pos + pbm_image->width_ + 1] += error * 1 / 16;
        }
      }
    }
  }

  free(double_data);
  return pbm_image;
}

/**
 * Convert a PGM image to a PBM image using Jarvis, Judice, and Ninke dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
PbmImage *PgmToPbmJarvisJudiceNinke(const PgmImage *image) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

  // Normalize pixel data to [0, 1] double values
  double *double_data = NormalizePgm(image);

  // Convert pixel data using Jarvis, Judice, and Ninke dithering
  for (uint32_t y = 0; y < pbm_image->height_; y++) {
    for (uint32_t x = 0; x < pbm_image->width_; x++) {
      uint32_t pos = y * pbm_image->width_ + x;
      double old_pixel = double_data[pos];
      double new_pixel = round(old_pixel);

      // Invert pixel value (PBM is white 0 and black 1)
      pbm_image->data_[pos] = new_pixel == 0;

      // Calculate error
      double error = old_pixel - new_pixel;

      // Propagate error
      if (x < pbm_image->width_ - 1) {
        double_data[pos + 1] += error * 7 / 48;
      }
      if (x < pbm_image->width_ - 2) {
        double_data[pos + 2] += error * 5 / 48;
      }
      if (y < pbm_image->height_ - 1) {
        if (x > 0) {
          double_data[pos + pbm_image->width_ - 1] += error * 3 / 48;
        }
        double_data[pos + pbm_image->width_] += error * 5 / 48;
        if (x < pbm_image->width_ - 1) {
          double_data[pos + pbm_image->width_ + 1] += error * 7 / 48;
        }
        if (x < pbm_image->width_ - 2) {
          double_data[pos + pbm_image->width_ + 2] += error * 5 / 48;
        }
      }
      if (y < pbm_image->height_ - 2) {
        if (x > 0) {
          double_data[pos + 2 * pbm_image->width_ - 1] += error * 1 / 48;
        }
        double_data[pos + 2 * pbm_image->width_] += error * 3 / 48;
        if (x < pbm_image->width_ - 1) {
          double_data[pos + 2 * pbm_image->width_ + 1] += error * 5 / 48;
        }
        if (x < pbm_image->width_ - 2) {
          double_data[pos + 2 * pbm_image->width_ + 2] += error * 3 / 48;
        }
      }
    }
  }

  free(double_data);
  return pbm_image;
}

/**
 * Write a PBM image to a file.
 *
 * @param filename  The name of the file to write.
 * @param image     The image data to write.
 * @return          True if successful, false otherwise.
 */
bool WritePbm(const char *filename, const PbmImage *image) {
  // Open file for writing
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Error: could not open file '%s' for writing\n", filename);
    return false;
  }

  // Write header (magic number, width, height)
  if (fprintf(fp, "P4\n%u\n%u\n", image->width_, image->height_) < 0) {
    fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
    fclose(fp);
    return false;
  }

  // Allocate buffer for encoded pixel data
  size_t buffer_size = (image->width_ * image->height_ + 7) / 8;
  uint8_t *buffer = (uint8_t *)calloc(1, buffer_size);
  if (!buffer) {
    fprintf(stderr, "Error: out of memory\n");
    fclose(fp);
    return false;
  }

// Encode pixel data
#pragma omp parallel for default(none) shared(image, buffer)
  for (size_t i = 0; i < image->width_ * image->height_; i++) {
    buffer[i / 8] |= (image->data_[i] << (7 - i % 8));
  }

  // Write encoded pixel data to file
  if (fwrite(buffer, 1, buffer_size, fp) != buffer_size) {
    fprintf(stderr, "Error: could not write pixel data to file '%s'\n",
            filename);
    free(buffer);
    fclose(fp);
    return false;
  }

  free(buffer);
  fclose(fp);
  return true;
}

/**
 * Free memory used by a PBM image
 *
 * @param image     Image to free
 */
void FreePbm(PbmImage *image) {
  free(image->data_);
  free(image);
}

#endif // NETPBM__PBM_H_
