#ifndef NETPBM__SAT_H_
#define NETPBM__SAT_H_

#include <stdint.h>

#include "types/pgm.h"
#include "types/sat.h"

/**
 * Allocate memory for a summed area table.
 *
 * @param width     The width of the image.
 * @param height    The height of the image.
 * @param max       The maximum value of an original pixel.
 * @return          A pointer to the SummedAreaTable, or NULL if an error
 * occurred.
 */
extern SummedAreaTable *AllocateSat(uint32_t width, uint32_t height,
                                    uint16_t max);

/**
 * Compute the summed area table of a PGM image.
 *
 * @param pgm   The PGM image.
 * @return      The summed area table, or NULL if an error occurred.
 */
extern SummedAreaTable *PgmToSat(const PgmImage *pgm);

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
extern uint64_t SatQuery(const SummedAreaTable *sat, uint32_t tlx, uint32_t tly,
                         uint32_t brx, uint32_t bry);

/**
 * Free memory for a summed area table.
 *
 * @param sat  The summed area table to free.
 */
extern void FreeSat(SummedAreaTable *sat);

#endif// NETPBM__SAT_H_
