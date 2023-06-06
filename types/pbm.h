#ifndef NETPBM_TYPES_PBM_H_
#define NETPBM_TYPES_PBM_H_

#include <stdint.h>

// PBM image format
// We use one byte per bit, so we can easily access individual pixels
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint8_t *data_;
} PbmImage;

// Threshold function
typedef uint8_t (*ThresholdFn)();

#endif// NETPBM_TYPES_PBM_H_
