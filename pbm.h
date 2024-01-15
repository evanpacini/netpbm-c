#ifndef NETPBM__PBM_H_
#define NETPBM__PBM_H_

#include <stdbool.h>
#include <stdint.h>

#include "types/pbm.h"
#include "types/pgm.h"

/**
 * Allocate memory for a PBM image.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the PbmImage, or NULL if an error occurred.
 */
extern PbmImage *AllocatePbm(uint32_t width, uint32_t height);

/**
 * Read a PBM image from a file.
 *
 * @param filename  The name of the file to read.
 * @return          A pointer to the image data, or NULL if an error occurred.
 */
extern PbmImage *ReadPbm(const char *filename);

/**
 * Normalizes pixel values from 0-max_gray to double 0-1.
 *
 * @param image Input PgmImage
 * @return      Normalized image data
 */
extern double *NormalizePgm(const PgmImage *image);

/**
 * Threshold function that always returns a 50% threshold.
 *
 * @param x Unused
 * @param y Unused
 * @param max_gray The maximum gray value.
 * @return max_gray / 2
 */
extern uint16_t MiddleThreshold(__attribute__((unused)) uint32_t x,
                                __attribute__((unused)) uint32_t y,
                                uint16_t max_gray);

/**
 * Threshold function that returns a random value between 0 and max_gray.
 *
 * @param x Unused
 * @param y Unused
 * @param max_gray The maximum gray value.
 * @return Random value between 0 and max_gray
 */
extern uint16_t RandomThreshold(__attribute__((unused)) uint32_t x,
                                __attribute__((unused)) uint32_t y,
                                uint16_t max_gray);

/**
 * Threshold function based on IGN (Interleaved Gradient Noise)
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param max_gray The maximum gray value.
 * @return  Threshold value between 0 and max_gray
 */
extern uint16_t IgnThreshold(uint32_t x, uint32_t y, uint16_t max_gray);

/**
 * Convert a PGM image to a PBM image.
 *
 * @param image     The PGM image to convert.
 * @param threshold The threshold function (0-255) to use for the conversion.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
extern PbmImage *PgmToPbm(const PgmImage *image, ThresholdFn threshold);

/**
 * Convert a PGM image to a PBM image using Atkinson dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
extern PbmImage *PgmToPbmAtkinson(const PgmImage *image);

/**
 * Convert a PGM image to a PBM image using Bayer (Ordered) Dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
extern PbmImage *PgmToPbmOrdered(const PgmImage *image, const PgmImage *map);

/**
 * Convert a PGM image to a PBM image using Floyd–Steinberg dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
extern PbmImage *PgmToPbmFloydSteinberg(const PgmImage *image);

/**
 * Convert a PGM image to a PBM image using Jarvis, Judice, and Ninke dithering.
 *
 * @param image     The PGM image to convert.
 * @return          A pointer to the new PBM image, or NULL if an error
 * occurred.
 */
extern PbmImage *PgmToPbmJarvisJudiceNinke(const PgmImage *image);

/**
 * Write a PBM image to a file.
 *
 * @param filename  The name of the file to write.
 * @param image     The image data to write.
 * @return          True if successful, false otherwise.
 */
extern bool WritePbm(const PbmImage *image, const char *filename);

/**
 * Free memory used by a PBM image
 *
 * @param image     Image to free
 */
extern void FreePbm(PbmImage *image);

#endif// NETPBM__PBM_H_
