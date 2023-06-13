#ifndef NETPBM__PPM_H_
#define NETPBM__PPM_H_

#include <stdbool.h>
#include <stdint.h>

#include "types/pixel.h"
#include "types/ppm.h"

/**
 * Allocate memory for a PPM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PpmImage, or NULL if an error occurred.
 */
extern PpmImage *AllocatePpm(uint32_t width, uint32_t height);

/**
 * Read a PPM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
extern PpmImage *ReadPpm(const char *filename);

/**
 * Convert SRgb value to linear RGB value
 *
 * @param s_rgb SRgb value
 * @return Linear RGB value
 */
extern double LinearRgbValue(double s_rgb);

/**
 * Convert linear RGB value to SRgb value
 *
 * @param linear_rgb Linear RGB value
 * @return SRgb value
 */
extern double SRgbValue(double linear_rgb);

/**
 * Convert SRgb color to linear RGB color
 *
 * @param s_rgb SRgb Pixel
 */
extern void LinearRgb(Pixel *s_rgb);

/**
 * Convert linear RGB color to SRgb color
 *
 * @param linear_rgb Linear RGB color
 */
extern void SRgb(Pixel *linear_rgb);

/**
 * Convert an image to a new image using the given pixel conversion function
 *
 * @param image         Pointer to the original image
 * @param conversion_fn Reference to the pixel conversion function
 * @return              Pointer to the new image
 */
extern PpmImage *PpmPixelConvert(PpmImage *image,
                                 void (*conversion_fn)(Pixel *));

/**
 * Calculate the LinearLuminance of a pixel.
 *
 * @param p The pixel.
 * @return  The LinearLuminance.
 */
extern double LinearLuminance(const Pixel *p);

/**
 * Calculate the SRgbLuminance of a pixel.
 *
 * @param p The pixel.
 * @return  The SRgbLuminance.
 */
extern double SRgbLuminance(const Pixel *p);

/**
 * Write a PPM image to a file.
 *
 * @param filename  The name of the file to write.
 * @param image     The image data.
 * @return          true if the image was written successfully, false otherwise.
 */
extern bool WritePpm(const char *filename, const PpmImage *image);

/**
 * Free the memory used by an image.
 *
 * @param image The image to free.
 */
extern void FreePpm(PpmImage *image);

#endif// NETPBM__PPM_H_
