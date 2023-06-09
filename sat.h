#ifndef NETPBM_INPUT_SAT_H_
#define NETPBM_INPUT_SAT_H_

#include <malloc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "types/pgm.h"
#include "types/sat.h"

/**
 * Allocate memory for a summed area table.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @return          A pointer to the SummedAreaTable, or NULL if an error
 * occurred.
 */
SummedAreaTable *AllocateSat(uint32_t width, uint32_t height) {
  // Allocate memory for image data
  SummedAreaTable *sat = (SummedAreaTable *)malloc(sizeof(SummedAreaTable));
  if (!sat) {
    fprintf(stderr, "Error: out of memory\n");
    return NULL;
  }
  sat->width_  = width;
  sat->height_ = height;
  sat->data_   = (uint64_t *)calloc(width * height, sizeof(uint64_t));
  if (!sat->data_) {
    fprintf(stderr, "Error: out of memory\n");
    free(sat);
    return NULL;
  }

  return sat;
}

/**
 * Free memory for a summed area table.
 *
 * @param sat  The summed area table to free.
 */
void FreeSat(SummedAreaTable *sat) {
    free(sat->data_);
    free(sat);
}

#endif// NETPBM_INPUT_SAT_H_
