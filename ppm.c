#include "ppm.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Allocate memory for a PPM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PpmImage, or NULL if an error occurred.
 */
PpmImage *AllocatePpm(uint32_t width, uint32_t height) {
  // Allocate memory for image data
  PpmImage *image = (PpmImage *)malloc(sizeof(PpmImage));
  if (!image) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  image->width_     = width;
  image->height_    = height;
  image->max_color_ = PPM_MAX_COLOR;
  image->data_      = (Pixel *)calloc(width * height, sizeof(Pixel));
  if (!image->data_) {
    fprintf(stderr, "Error: out of memory\n");
    free(image);
    return NULL;
  }

  return image;
}

/**
 * Read a PPM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
PpmImage *ReadPpm(const char *filename) {
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
  if (fscanf(fp, "%2s%*[ \t\r\n]%u%*[ \t\r\n]%u%*[ \t\r\n]%hu%*1[ \t\r\n]",
             magic, &width, &height, &max_color) != 4) {
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

  // Make sure the max color value is PPM_MAX_COLOR
  if (max_color != PPM_MAX_COLOR) {
    fprintf(stderr, "Error: max color value must be PPM_MAX_COLOR\n");
    fclose(fp);
    return NULL;
  }

  // Allocate memory for image data
  PpmImage *image = AllocatePpm(width, height);

  // Read pixel data
  if (fread(image->data_, sizeof(Pixel), width * height, fp) !=
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
 * Convert SRgb value to linear RGB value
 *
 * @param s_rgb SRgb value
 * @return Linear RGB value
 */
double LinearRgbValue(double s_rgb) {
  return s_rgb <= 0.04045 ? s_rgb / 12.92 : pow(((s_rgb + 0.055) / 1.055), 2.4);
}

/**
 * Convert linear RGB value to SRgb value
 *
 * @param linear_rgb Linear RGB value
 * @return SRgb value
 */
double SRgbValue(double linear_rgb) {
  return linear_rgb <= 0.0031308 ? 12.92 * linear_rgb
                                 : 1.055 * pow(linear_rgb, 1.0 / 2.4) - 0.055;
}

/**
 * Convert SRgb color to linear RGB color
 *
 * @param s_rgb SRgb Pixel
 */
void LinearRgb(Pixel *s_rgb) {
  s_rgb->r_ =
      (uint8_t)(LinearRgbValue(s_rgb->r_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
  s_rgb->g_ =
      (uint8_t)(LinearRgbValue(s_rgb->g_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
  s_rgb->b_ =
      (uint8_t)(LinearRgbValue(s_rgb->b_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
}

/**
 * Convert linear RGB color to SRgb color
 *
 * @param linear_rgb Linear RGB color
 */
void SRgb(Pixel *linear_rgb) {
  linear_rgb->r_ =
      (uint8_t)(SRgbValue(linear_rgb->r_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
  linear_rgb->g_ =
      (uint8_t)(SRgbValue(linear_rgb->g_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
  linear_rgb->b_ =
      (uint8_t)(SRgbValue(linear_rgb->b_ / PPM_MAX_COLOR_F) * PPM_MAX_COLOR_F);
}

/**
 * Convert an image to a new image using the given pixel conversion function
 *
 * @param image         Pointer to the original image
 * @param conversion_fn Reference to the pixel conversion function
 * @return              Pointer to the new image
 */
PpmImage *PpmPixelConvert(PpmImage *image, void (*conversion_fn)(Pixel *)) {
  // Allocate memory for the new image
  PpmImage *new_image = AllocatePpm(image->width_, image->height_);
  if (!new_image) {
    fprintf(stderr, "Error: could not allocate memory for new PPM image");
    return NULL;
  }

  // Copy the pixel data of the original image to the new image
  memcpy(new_image->data_, image->data_,
         sizeof(Pixel) * new_image->width_ * new_image->height_);

#pragma omp parallel for default(none) shared(new_image, conversion_fn)
  // Convert the pixel data of the new image using the given conversion function
  for (int i = 0; i < new_image->width_ * new_image->height_; i++)
    conversion_fn(&new_image->data_[i]);

  return new_image;
}

/**
 * Calculate the LinearLuminance of a pixel.
 *
 * @param p The pixel.
 * @return  The LinearLuminance.
 */
double LinearLuminance(const Pixel *p) {
  return 0.299 * p->r_ + 0.587 * p->g_ + 0.114 * p->b_;
}

/**
 * Calculate the SRgbLuminance of a pixel.
 *
 * @param p The pixel.
 * @return  The SRgbLuminance.
 */
double SRgbLuminance(const Pixel *p) {
  return 0.2126 * p->r_ + 0.7152 * p->g_ + 0.0722 * p->b_;
}

/**
 * Write a PPM image to a file.
 *
 * @param filename  The name of the file to write.
 * @param image     The image data.
 * @return          true if the image was written successfully, false otherwise.
 */
bool WritePpm(const char *filename, const PpmImage *image) {
  // Open file for writing
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Error: could not open file '%s' for writing\n", filename);
    return false;
  }

  // Write magic number, width, height, and max color value
  if (fprintf(fp, "P6\n%u\n%u\n%hu\n", image->width_, image->height_,
              image->max_color_) < 0) {
    fprintf(stderr, "Error: could not write header to file '%s'\n", filename);
    fclose(fp);
    return false;
  }

  // Write pixel data
  if (fwrite(image->data_, sizeof(uint8_t), image->width_ * image->height_ * 3,
             fp) != image->width_ * image->height_ * 3) {
    fprintf(stderr, "Error: could not write pixel data to file '%s'\n",
            filename);
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
void FreePpm(PpmImage *image) {
  free(image->data_);
  free(image);
}
