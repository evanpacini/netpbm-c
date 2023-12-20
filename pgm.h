#ifndef NETPBM__PGM_H_
#define NETPBM__PGM_H_

#include <stdbool.h>
#include <stdint.h>

#include "types/pbm.h"
#include "types/pgm.h"
#include "types/ppm.h"
#include "types/sat.h"

/**
 * Allocate memory for a PGM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @param max_gray  The maximum gray value.
 * @return          A pointer to the PgmImage, or NULL if an error occurred.
 */
extern PgmImage *AllocatePgm(uint32_t width, uint32_t height,
                             uint16_t max_gray);

/**
 * Read a PGM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
extern PgmImage *ReadPgm(const char *filename);

/**
 * Get the pixel at the given index in a PGM image.
 *
 * @param image Pointer to the image data.
 * @param i     The index of the pixel to get.
 * @return      The pixel at the given index.
 */
extern uint16_t GetPixelPgm(const PgmImage *image, uint32_t i);

/**
 * Set the pixel at the given index in a PGM image.
 *
 * @param image Pointer to the image data.
 * @param i     The index of the pixel to set.
 * @param value The value to set the pixel to.
 */
extern void SetPixelPgm(PgmImage *image, uint32_t i, uint16_t value);

/**
 * Convert an image to a new image using the given pixel conversion function.
 *
 * @param image     Pointer to the original image
 * @param luminance Reference to the luminance function
 * @return          Pointer to the new image
 */
extern PgmImage *PpmToPgm(const PpmImage *image, LuminanceFn luminance);

/**
 * Convert a PBM image to a PGM image.
 *
 * @param image     Pointer to the image data
 * @return          Pointer to the new image data
 */
extern PgmImage *PbmToPgm(const PbmImage *image);

/**
 * Blur effect first designed by KaspervanM in Jan 22, 2021
 * Each pixel becomes the average of a square surrounding that pixel with side
 * 2r + 1.
 * @param image     Input PgmImage
 * @param radius    Radius of the square
 * @return          Pointer to the new image data
 */
extern PgmImage *KasperBlur(const PgmImage *image, int8_t radius);

/**
 * Each pixel becomes the average of a box surrounding that pixel with side
 * 2r + 1.
 *
 * @param sat       Summed area table of the image
 * @param radius    Radius of the square
 * @return          Blurred image
 */
extern PgmImage *BoxBlur(const SummedAreaTable *sat, int8_t radius);

/**
 * Difference between two pgm images expressed as an image itself.
 * Each pixel of the returned image is the absolute difference between the
 * corresponding pixels of the input images.
 *
 * @param image1 First image
 * @param image2 Second image
 * @return Difference image
 */
extern PgmImage *PgmDiff(const PgmImage *image1, const PgmImage *image2);

/**
 * Sum the pixels of an image raised to some power p.
 *
 * @param image Image to sum
 * @param p     Power to raise pixels to
 * @return      Sum of pixels raised to p
 */
extern double PgmSum(const PgmImage *image, double p);

/**
 * Calculate the variance of the distribution of pixel values in an image.
 *
 * @param image Image to calculate variance of
 * @return      Variance of pixel values
 */
extern double PgmVariance(const PgmImage *image);

/**
 * Write a PGM image to a file
 *
 * @param filename  Name of file to write to
 * @param image     Image to write
 * @return          True if successful, false otherwise
 */
extern bool WritePgm(const PgmImage *image, const char *filename);

/**
 * Free memory used by a PGM image.
 *
 * @param image     Image to free
 */
extern void FreePgm(PgmImage *image);

#endif// NETPBM__PGM_H_
