#ifndef NETPBM_TYPES_PPM_H_
#define NETPBM_TYPES_PPM_H_

#include <stdint.h>
#include "pixel.h"

// PPM image format
typedef struct {
    uint32_t width_;
    uint32_t height_;
    uint16_t max_color_;
    Pixel *data_;
} PpmImage;

#endif //NETPBM_TYPES_PPM_H_
