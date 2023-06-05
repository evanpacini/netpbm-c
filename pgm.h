#ifndef NETPBM__PGM_H_
#define NETPBM__PGM_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/random.h>
#include "bayer.h"
#include "pbm.h"

#define PGM_MAX_GRAY 255
#define PGM_MAX_GRAY_F 255.0

// PGM image format
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint16_t max_gray_;
  uint8_t *data_;
} PgmImage;

// Threshold function
typedef uint8_t (*ThresholdFn)();

/**
 * Allocate memory for a PGM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PgmImage, or NULL if an error occurred.
 */
PgmImage *AllocatePgm(uint32_t width, uint32_t height) {
  // Allocate memory for image data
  PgmImage *image = (PgmImage *)calloc(1, sizeof(PgmImage));
  if (!image) {
	fprintf(stderr, "Error: out of memory\n");
	return NULL;
  }
  image->width_ = width;
  image->height_ = height;
  image->max_gray_ = PGM_MAX_GRAY;
  image->data_ = (uint8_t *)calloc(1, width * height * sizeof(uint8_t));
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
  PgmImage *image = AllocatePgm(width, height);

  // Read pixel data
  if (fread(image->data_, sizeof(uint8_t), width * height, fp) != width * height) {
	fprintf(stderr, "Error: could not read pixel data from file '%s'\n", filename);
	free(image->data_);
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
uint8_t MiddleThreshold() {
  return 128;
}

/**
 * Threshold function that returns a random value between 0 and 255.
 *
 * @return Random value between 0 and 255
 */
uint8_t RandomThreshold() {
  uint8_t random;
  getrandom(&random, sizeof(uint8_t), 0);
  return random;
}

/**
 * Normalizes pixel values from 0-255 to double 0-1.
 *
 * @param image Input PgmImage
 * @return Normalized image data
 */
double *NormalizePgm(const PgmImage *image) {
  double *double_data = (double *)malloc(image->width_ * image->height_ * sizeof(double));
#pragma omp parallel for default(none) shared(image, double_data)
  for (uint32_t i = 0; i < image->height_ * image->width_; i++)
	double_data[i] = (double)image->data_[i] / PGM_MAX_GRAY_F;
  return double_data;
}

/**
 * Convert a PGM image to a PBM image.
 *
 * @param image     The PGM image to convert.
 * @param threshold The threshold function (0-255) to use for the conversion.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
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
 * Convert a PGM image to a PBM image using Floyd–Steinberg dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
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
 * Convert a PGM image to a PBM image using Atkinson dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
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
 * Convert a PGM image to a PBM image using Jarvis, Judice, and Ninke dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error occurred.
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

PbmImage *PgmToPbmBayer(const PgmImage *image) {
  // Allocate memory for new image data
  PbmImage *pbm_image = AllocatePbm(image->width_, image->height_);

  // Normalize pixel data to [0, 1] double values
  double *double_data = NormalizePgm(image);

  // Convert using Bayer (Ordered) Dithering
#pragma omp parallel for default(none) shared(kBayer8X8, pbm_image, double_data) collapse(2)
  for (uint32_t y = 0; y < pbm_image->height_; y++) {
	for (uint32_t x = 0; x < pbm_image->width_; x++) {
	  uint32_t pos = y * pbm_image->width_ + x;
	  pbm_image->data_[pos] = double_data[pos] < kBayer8X8[x & 7][y & 7];
	}
  }

  return pbm_image;
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
  if (fprintf(fp, "P5\n%u\n%u\n%hu\n", image->width_, image->height_, image->max_gray_) < 0) {
	fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
	fclose(fp);
	return false;
  }

  // Write pixel data
  if (fwrite(image->data_, sizeof(uint8_t), image->width_ * image->height_, fp) != image->width_ * image->height_) {
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
void FreePgm(PgmImage *image) {
  free(image->data_);
  free(image);
}

#endif //NETPBM__PGM_H_
