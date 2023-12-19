#ifndef NETPBM_TYPES_PGM_H_
#define NETPBM_TYPES_PGM_H_

#define PGM_MAX_GRAY 65535
#define PGM_MAX_GRAY_F 65535.0

#include <stdint.h>

#include "pixel.h"

/**
 * A PGM image.
 * This is a grayscale image.
 */
typedef struct {
    uint32_t width_;   // The width of the image.
    uint32_t height_;  // The height of the image.
    uint16_t max_gray_;// The maximum gray value.
    void *data_;    // The image data, stored in row-major order.
} PgmImage;

// Luminance function
typedef double (*LuminanceFn)(const Pixel *);

#endif// NETPBM_TYPES_PGM_H_
