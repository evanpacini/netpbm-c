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

    // No dithering
    PBMImage *pgmToPbm = pgm_to_pbm(ppmToPgm, middle_threshold);
    write_pbm("../output/binary.pbm", pgmToPbm);
    free_pbm(pgmToPbm);

    // Random dithering
    PBMImage *pgmToPbmRandom = pgm_to_pbm(ppmToPgm, random_threshold);
    write_pbm("../output/binary_random.pbm", pgmToPbmRandom);
    free_pbm(pgmToPbmRandom);

    free_pgm(ppmToPgm);

    PPMImage *linear = ppm_pixel_convert(readPpm, linearRGB);
    write_ppm("../output/linear.ppm", linear);
    free_ppm(readPpm);

    PGMImage *ppmToPgmSrgb = ppm_to_pgm(srgb, sRGB_luminance);
    write_pgm("../output/grayscale_srgb.pgm", ppmToPgmSrgb);
    free_ppm(srgb);

    // No dithering
    PBMImage *pgmToPbmSrgb = pgm_to_pbm(ppmToPgmSrgb, middle_threshold);
    write_pbm("../output/binary_srgb.pbm", pgmToPbmSrgb);
    free_pbm(pgmToPbmSrgb);

    // Random dithering
    PBMImage *pgmToPbmSrgbRandom = pgm_to_pbm(ppmToPgmSrgb, random_threshold);
    write_pbm("../output/binary_srgb_random.pbm", pgmToPbmSrgbRandom);
    free_pbm(pgmToPbmSrgbRandom);

    free_pgm(ppmToPgmSrgb);

    PGMImage *ppmToPgmLinear = ppm_to_pgm(linear, linear_luminance);
    write_pgm("../output/grayscale_linear.pgm", ppmToPgmLinear);
    free_ppm(linear);

    // No dithering
    PBMImage *pgmToPbmLinear = pgm_to_pbm(ppmToPgmLinear, middle_threshold);
    write_pbm("../output/binary_linear.pbm", pgmToPbmLinear);
    free_pbm(pgmToPbmLinear);

    // Random dithering
    PBMImage *pgmToPbmLinearRandom = pgm_to_pbm(ppmToPgmLinear, random_threshold);
    write_pbm("../output/binary_linear_random.pbm", pgmToPbmLinearRandom);
    free_pbm(pgmToPbmLinearRandom);

    free_pgm(ppmToPgmLinear);

    return 0;
}
