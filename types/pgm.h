#ifndef NETPBM_TYPES_PGM_H_
#define NETPBM_TYPES_PGM_H_

#define PGM_MAX_GRAY 255
#define PGM_MAX_GRAY_F 255.0

#include <stdint.h>

#include "pixel.h"

// PGM image format
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint16_t max_gray_;
  uint8_t *data_;
} PgmImage;

// Luminance function
typedef double (*LuminanceFn)(const Pixel *);

#endif// NETPBM_TYPES_PGM_H_
