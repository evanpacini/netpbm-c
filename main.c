#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include "sat.h"
#include "threshold_maps.h"

int main(void) {
  // Read image and convert to grayscale
  PpmImage *image     = ReadPpm("../input/tud1.ppm");
  PgmImage *pgm_image = PpmToPgm(image, SRgbLuminance);
  FreePpm(image);

  // Get blue noise map
  PgmImage *blue_noise = ReadPgm("../textures/blue_noise/1024x1024/0.pgm");
  ThresholdMap *map    = PgmToThresholdMap(blue_noise);
  FreePgm(blue_noise);

  // Convert to PBM
  PbmImage *pbm = PgmToPbmOrdered(pgm_image, map);
  FreePgm(pgm_image);

  // Write image and free memory
  WritePbm("../output/test.pbm", pbm);
  FreePbm(pbm);
  FreeThresholdMap(map);
  return 0;
}
