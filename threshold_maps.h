#ifndef NETPBM__THRESHOLD_MAPS_H_
#define NETPBM__THRESHOLD_MAPS_H_

#include <stdint.h>

#include "types/pgm.h"
#include "types/threshold_map.h"

// Threshold maps

// 2x2 Bayer matrix
extern const ThresholdMap kBayer2X2;

// 4x4 Bayer matrix
extern const ThresholdMap kBayer4X4;

// 8x8 Bayer matrix
extern const ThresholdMap kBayer8X8;

// 16x16 Bayer matrix
extern const ThresholdMap kBayer16X16;

/**
 * Allocate memory for a threshold map.
 *
 * @param width     The width of the map.
 * @param height    The height of the map.
 * @return          A pointer to the ThresholdMap, or NULL if an error occurred.
 */
ThresholdMap *AllocateThresholdMap(uint32_t width, uint32_t height);

/**
 * Convert a PGM image to a threshold map.
 *
 * @param pgm   The PGM image to convert.
 * @return      A pointer to the ThresholdMap, or NULL if an error occurred.
 */
ThresholdMap *PgmToThresholdMap(PgmImage *pgm);

/**
 * Free the memory used by a threshold map.
 *
 * @param map   The map to free.
 */
void FreeThresholdMap(ThresholdMap *map);

#endif// NETPBM__THRESHOLD_MAPS_H_
