#ifndef NETPBM_TYPES_PGM_H_
#define NETPBM_TYPES_PGM_H_

#include "pixel.h"
#include <stdint.h>

// PGM image format
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint16_t max_gray_;
  uint8_t *data_;
} PgmImage;

// Luminance function
typedef double (*LuminanceFn)(const Pixel *);

#endif // NETPBM_TYPES_PGM_H_
