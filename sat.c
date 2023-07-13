#include "sat.h"

#include <stdio.h>
#include <stdlib.h>

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
 * Compute the summed area table of a PGM image.
 *
 * @param pgm   The PGM image.
 * @return      The summed area table, or NULL if an error occurred.
 */
SummedAreaTable *PgmToSat(const PgmImage *pgm) {
    uint32_t width  = pgm->width_;
    uint32_t height = pgm->height_;

    // Allocate memory for summed area table
    SummedAreaTable *sat = AllocateSat(width, height);
    if (!sat) {
        return NULL;
    }

    #pragma omp parallel for default(none) shared(sat, pgm, width)
    // Copy first row
    for (uint32_t x = 0; x < width; x++) {
        sat->data_[x] = (uint64_t)pgm->data_[x];
    }

    // Column-wise sum
    for (uint32_t y = 1; y < height; y++) {
        #pragma omp parallel for default(none) shared(sat, pgm, width, y)
        for (uint32_t x = 0; x < width; x++) {
            sat->data_[y * width + x] =
                (uint64_t)pgm->data_[y * width + x] + sat->data_[(y - 1) * width + x];
        }
    }
    #pragma omp parallel for default(none) shared(sat, width, height)
    //  Row-wise sum
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 1; x < width; x++) {
            sat->data_[y * width + x] += sat->data_[y * width + x - 1];
        }
    }
    return sat;
}

/**
 * Query the summed area table.
 *
 * @param sat   The summed area table.
 * @param tlx   Top-left x coordinate.
 * @param tly   Top-left y coordinate.
 * @param brx   Bottom-right x coordinate.
 * @param bry   Bottom-right y coordinate.
 * @return      The sum of the pixels in the rectangle defined by the given
 * coordinates.
 */
uint64_t SatQuery(const SummedAreaTable *sat, uint32_t tlx, uint32_t tly,
                  uint32_t brx, uint32_t bry) {
    uint64_t res = sat->data_[bry * sat->width_ + brx];
    if (tly > 0) res -= sat->data_[(tly - 1) * sat->width_ + brx];
    if (tlx > 0) res -= sat->data_[bry * sat->width_ + tlx - 1];
    if (tlx > 0 && tly > 0) res += sat->data_[(tly - 1) * sat->width_ + tlx - 1];
    return res;
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
