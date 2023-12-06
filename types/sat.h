#ifndef NETPBM_TYPES_SAT_H_
#define NETPBM_TYPES_SAT_H_

#include <stdint.h>

/**
 * A summed area table
 * This is a table of the sum of all pixels above and to the left of the current
 * pixel. This allows for fast calculation of the sum of any rectangular area.
 * See https://en.wikipedia.org/wiki/Summed-area_table for more information.
 * This is particularly useful for computing the box blur.
 */
typedef struct {
    uint32_t width_; // The width of the table
    uint32_t height_;// The height of the table
    uint64_t *data_; // The data of the table, stored in row-major order
} SummedAreaTable;

#endif// NETPBM_TYPES_SAT_H_
