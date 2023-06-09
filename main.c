#include <stdint.h>
#include <stdio.h>

#include "pgm.h"
#include "sat.h"

void PrintPgm(PgmImage *pgm) {
  for (uint32_t y = 0; y < pgm->height_; y++) {
    for (uint32_t x = 0; x < pgm->width_; x++) {
      printf("%d\t", pgm->data_[y * pgm->width_ + x]);
    }
    printf("\n");
  }
}

void PrintSat(SummedAreaTable *sat) {
  for (uint32_t y = 0; y < sat->height_; y++) {
    for (uint32_t x = 0; x < sat->width_; x++) {
      printf("%lu\t", sat->data_[y * sat->width_ + x]);
    }
    printf("\n");
  }
}

int main(void) {
  PgmImage *pgm = AllocatePgm(4, 4);
  for (uint32_t y = 0; y < pgm->height_; y++) {
    for (uint32_t x = 0; x < pgm->width_; x++) {
      pgm->data_[y * pgm->width_ + x] = 1;
    }
  }
  PrintPgm(pgm);

  SummedAreaTable *sat = PgmToSat(pgm);

  printf("\n");

  PrintSat(sat);

  uint64_t sum = SatQuery(sat, 0, 0, sat->width_ - 1, sat->height_ - 1);
  printf("Sum: %lu\n", sum);

  uint64_t sum2 = SatQuery(sat, 1, 1, 2, 2);
  printf("Sum: %lu\n", sum2);

  FreePgm(pgm);

  FreeSat(sat);
  return 0;
}
