#ifndef NETPBM_TYPES_PPM_H_
#define NETPBM_TYPES_PPM_H_

#define PPM_MAX_COLOR 255
#define PPM_MAX_COLOR_F 255.0

#include <stdint.h>

#include "pixel.h"

/**
 * A PPM image.
 * This is a color image.
 */
typedef struct {
    uint32_t width_;    // The width of the image.
    uint32_t height_;   // The height of the image.
    uint16_t max_color_;// The maximum color value.
    Pixel *data_;       // The image data, stored in row-major order.
} PpmImage;

#endif// NETPBM_TYPES_PPM_H_
