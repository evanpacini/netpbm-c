#ifndef NETPBM_TYPES_PIXEL_H_
#define NETPBM_TYPES_PIXEL_H_

#include <stdint.h>

/**
 * A pixel in a PBM image.
 */
typedef struct {
    uint8_t r_;// The red component of the pixel (0-255).
    uint8_t g_;// The green component of the pixel (0-255).
    uint8_t b_;// The blue component of the pixel (0-255).
} Pixel;

#endif// NETPBM_TYPES_PIXEL_H_
