#ifndef NETPBM_TYPES_THRESHOLD_MAP_H_
#define NETPBM_TYPES_THRESHOLD_MAP_H_

#include <stdint.h>

// Threshold map
typedef struct {
  uint32_t width_;
  uint32_t height_;
  double* data_;
} ThresholdMap;

#endif// NETPBM_TYPES_THRESHOLD_MAP_H_
