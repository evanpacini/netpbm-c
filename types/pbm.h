#ifndef NETPBM_TYPES_PBM_H_
#define NETPBM_TYPES_PBM_H_

#include <stdint.h>

/**
 * A PBM image.
 * This is a black and white image.
 * We use a byte to represent a pixel of one bit, for ease of use.
 */
typedef struct {
    uint32_t width_; // The width of the image.
    uint32_t height_;// The height of the image.
    uint8_t *data_;  // The image data, stored in row-major order.
} PbmImage;

// Threshold function
typedef uint8_t (*ThresholdFn)(uint32_t x, uint32_t y);

#endif// NETPBM_TYPES_PBM_H_
