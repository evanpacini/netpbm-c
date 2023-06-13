#include <stdio.h>

#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include "sat.h"

int main(void) {
    // Read a PPM image from a file.
    PpmImage *image = ReadPpm("../input/tud1.ppm");

    // Write a PPM image to a file.
    WritePpm("../output/tud1.ppm", image);

    // Convert an image to a new image using the given pixel conversion function
    PpmImage *new_image = PpmPixelConvert(image, LinearRgb);

    // Write a PPM image to a file.
    WritePpm("../output/tud1_linear_rgb.ppm", new_image);

    // Free the memory for the image.
    FreePpm(new_image);

    // Convert an image to a new image using the given pixel conversion function
    PpmImage *new_image2 = PpmPixelConvert(image, SRgb);

    // Write a PPM image to a file.
    WritePpm("../output/tud1_srgb.ppm", new_image2);

    // Free the memory for the image.
    FreePpm(new_image2);

    // Convert PPM image to PGM image
    PgmImage *pgm_image = PpmToPgm(image, SRgbLuminance);

    // Write a PGM image to a file.
    WritePgm("../output/tud1_srgb_luminance.pgm", pgm_image);

    // Free the memory for the image.
    FreePpm(image);

    // Free the memory for the image.
    FreePgm(pgm_image);

    // Read a PGM image from a file.
    PgmImage *pgm_image2 = ReadPgm("../output/tud1_srgb_luminance.pgm");

    // Apply KasperBlur to the image
    PgmImage *pgm_image3 = KasperBlur(pgm_image2, 5);

    // Write a PGM image to a file.
    WritePgm("../output/tud1_srgb_luminance_kasper_blur.pgm", pgm_image3);

    // Free the memory for the image.
    FreePgm(pgm_image3);

    // Compute the summed area table of the image
    SummedAreaTable *sat = PgmToSat(pgm_image2);

    // Apply BoxBlur to the image
    PgmImage *pgm_image4 = BoxBlur(sat, 5);

    // Write a PGM image to a file.
    WritePgm("../output/tud1_srgb_luminance_box_blur.pgm", pgm_image4);

    // Free the memory for the image.
    FreePgm(pgm_image4);

    // Free the memory for the summed area table.
    FreeSat(sat);

    // Convert PGM image to PBM image using a threshold of 128
    PbmImage *pbm_image = PgmToPbm(pgm_image2, MiddleThreshold);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_middle_threshold.pbm", pbm_image);

    // Free the memory for the image.
    FreePbm(pbm_image);

    // Convert PGM image to PBM image using a random threshold
    PbmImage *pbm_image2 = PgmToPbm(pgm_image2, RandomThreshold);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_random_threshold.pbm", pbm_image2);

    // Convert PBM image to PGM image
    PgmImage *pgm_image5 = PbmToPgm(pbm_image2);

    // Write a PGM image to a file.
    WritePgm("../output/tud1_srgb_luminance_random_threshold.pgm", pgm_image5);

    // Compute difference between two PGM images
    PgmImage *pgm_image6 = PgmDiff(pgm_image2, pgm_image5);

    // Write a PGM image to a file.
    WritePgm("../output/tud1_srgb_luminance_random_threshold_diff.pgm",
             pgm_image6);

    // Print the sum of the differences
    printf("Sum of differences: %f\n", PgmSum(pgm_image6, 1.0));

    // Print the variance of the differences
    printf("Variance of differences: %f\n", PgmVariance(pgm_image6));

    // Free the memory for the image.
    FreePgm(pgm_image6);

    // Free the memory for the image.
    FreePgm(pgm_image5);

    // Free the memory for the image.
    FreePbm(pbm_image2);

    // Convert PGM image to PBM image using Atkinson dithering
    PbmImage *pbm_image3 = PgmToPbmAtkinson(pgm_image2);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_atkinson.pbm", pbm_image3);

    // Free the memory for the image.
    FreePbm(pbm_image3);

    // Convert PGM image to PBM image using Bayer dithering
    PbmImage *pbm_image4 = PgmToPbmBayer(pgm_image2);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_bayer.pbm", pbm_image4);

    // Free the memory for the image.
    FreePbm(pbm_image4);

    // Convert PGM image to PBM image using Floyd-Steinberg dithering
    PbmImage *pbm_image5 = PgmToPbmFloydSteinberg(pgm_image2);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_floyd_steinberg.pbm", pbm_image5);

    // Free the memory for the image.
    FreePbm(pbm_image5);

    // Convert PGM image to PBM image using Jarvis-Judice-Ninke dithering
    PbmImage *pbm_image6 = PgmToPbmJarvisJudiceNinke(pgm_image2);

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_jarvis_judice_ninke.pbm", pbm_image6);

    // Free the memory for the image.
    FreePbm(pbm_image6);

    // Free the memory for the image.
    FreePgm(pgm_image2);

    // Read a PBm image from a file.
    PbmImage *pbm_image7 = ReadPbm("../output/tud1_srgb_luminance_middle_threshold.pbm");

    // Write a PBM image to a file.
    WritePbm("../output/tud1_srgb_luminance_middle_threshold_2.pbm", pbm_image7);

    // Free the memory for the image.
    FreePbm(pbm_image7);

    return 0;
}
