#ifndef NETPBM__PBM_H_
#define NETPBM__PBM_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// PBM image format
// We use one byte per bit, so we can easily access individual pixels
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint8_t *data_;
} PbmImage;

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
  if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*1[ \t\r\n]", magic, &width, &height) != 3) {
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
	fprintf(stderr, "Error: failed to read pixel data from file '%s'\n", filename);
	free(buffer);
	fclose(fp);
	return NULL;
  }

  // Allocate memory for image data
  PbmImage *image = AllocatePbm(width, height);

  // Decode the pixel data from the buffer
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

  fclose(fp);
  free(buffer);

  return image;
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
	fprintf(stderr, "Error: could not write pixel data to file '%s'\n", filename);
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

#endif //NETPBM__PBM_H_
