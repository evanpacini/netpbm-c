#ifndef NETPBM_TYPES_SAT_H_
#define NETPBM_TYPES_SAT_H_

#include <stdint.h>

// A struct to represent a summed area table.
typedef struct {
  uint32_t width_;
  uint32_t height_;
  uint64_t *data_;
} SummedAreaTable;

#endif// NETPBM_TYPES_SAT_H_
