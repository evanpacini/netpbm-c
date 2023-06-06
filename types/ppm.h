#ifndef NETPBM_TYPES_PPM_H_
#define NETPBM_TYPES_PPM_H_

#define PPM_MAX_COLOR 255
#define PPM_MAX_COLOR_F 255.0

#include "pixel.h"
#include <stdint.h>

// PPM image format
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint16_t max_color_;
  Pixel *data_;
} PpmImage;

#endif // NETPBM_TYPES_PPM_H_
