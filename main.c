#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "pgm.h"
#include "ppm.h"
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
  // SETUP
  int8_t radius = 1;
  PpmImage *ppm = ReadPpm("../input/tud1.ppm");
  if (!ppm) {
    return 1;
  }
  PgmImage *pgm = PpmToPgm(ppm, SRgbLuminance);

  // TIME OLD BLUR
  clock_t old_blur_start = clock();
  PgmImage *pgm_blur     = KasperBlur(pgm, radius);
  clock_t old_blur_end   = clock();
  double old_blur_time =
      (double)(old_blur_end - old_blur_start) / CLOCKS_PER_SEC;

  // WAIT FOR THINGS TO SETTLE
  sleep(1);

  // TIME NEW BLUR AND SAT SEPARATELY
  clock_t sat_start      = clock();
  SummedAreaTable *sat   = PgmToSat(pgm);
  clock_t sat_end        = clock();
  PgmImage *pgm_box_blur = BoxBlur(sat, radius);
  clock_t sat_blur_end   = clock();
  double sat_time        = (double)(sat_end - sat_start) / CLOCKS_PER_SEC;
  double box_blur_time   = (double)(sat_blur_end - sat_end) / CLOCKS_PER_SEC;
  double sat_blur_time   = (double)(sat_blur_end - sat_start) / CLOCKS_PER_SEC;

  // WRITE
  WritePgm("../output/tud1.pgm", pgm);
  WritePgm("../output/tud1_blur.pgm", pgm_blur);
  WritePgm("../output/tud1_box_blur.pgm", pgm_box_blur);

  // FREE
  FreePpm(ppm);
  FreePgm(pgm);
  FreePgm(pgm_blur);
  FreePgm(pgm_box_blur);

  // PRINT
  printf("Height: %d\n", sat->height_);
  printf("Width: %d\n", sat->width_);
  printf("Radius: %d\n", radius);
  printf("Old blur time: %f\n", old_blur_time);
  printf("Sat time: %f\n", sat_time);
  printf("Box blur time: %f\n", box_blur_time);
  printf("Sat blur time: %f\n", sat_blur_time);
  return 0;
}
