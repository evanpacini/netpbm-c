#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "pgm.h"
#include "pbm.h"

int main(void) {
    PPMImage *readPpm = read_ppm("../input/tud2.ppm");
    write_ppm("../output/normal.ppm", readPpm);

    PPMImage *srgb = ppm_pixel_convert(readPpm, sRGB);
    write_ppm("../output/srgb.ppm", srgb);

    PGMImage *ppmToPgm = ppm_to_pgm(readPpm, sRGB_luminance);
    write_pgm("../output/grayscale.pgm", ppmToPgm);
    PBMImage *pgmToPbm = pgm_to_pbm(ppmToPgm, 128);
    write_pbm("../output/binary.pbm", pgmToPbm);
    free_pgm(ppmToPgm);
    free_pbm(pgmToPbm);

    PPMImage *linear = ppm_pixel_convert(readPpm, linearRGB);
    write_ppm("../output/linear.ppm", linear);
    free_ppm(readPpm);

    PGMImage *ppmToPgmSrgb = ppm_to_pgm(srgb, sRGB_luminance);
    write_pgm("../output/grayscale_srgb.pgm", ppmToPgmSrgb);
    free_ppm(srgb);
    PBMImage *pgmToPbmSrgb = pgm_to_pbm(ppmToPgmSrgb, 128);
    write_pbm("../output/binary_srgb.pbm", pgmToPbmSrgb);
    free_pgm(ppmToPgmSrgb);
    free_pbm(pgmToPbmSrgb);

    PGMImage *ppmToPgmLinear = ppm_to_pgm(linear, linear_luminance);
    write_pgm("../output/grayscale_linear.pgm", ppmToPgmLinear);
    free_ppm(linear);
    PBMImage *pgmToPbmLinear = pgm_to_pbm(ppmToPgmLinear, 128);
    write_pbm("../output/binary_linear.pbm", pgmToPbmLinear);
    free_pgm(ppmToPgmLinear);
    free_pbm(pgmToPbmLinear);

    return 0;
}
