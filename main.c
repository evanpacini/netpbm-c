#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include <time.h>

int main(void) {
    srand(time(NULL));
    // Read PPM image
    PpmImage *read_ppm = ReadPpm("../input/tud2.ppm");
    WritePpm("../output/normal.ppm", read_ppm);

    PpmImage *srgb = PpmPixelConvert(read_ppm, SRgb);
    WritePpm("../output/srgb.ppm", srgb);

    PgmImage *ppm_to_pgm = PpmToPgm(read_ppm, SRgbLuminance);
    WritePgm("../output/grayscale.pgm", ppm_to_pgm);

    // Add square for iterations frames to greyscale image
    const int iterations = 3;
    PgmImage **pgm_squares = AddSquare(ppm_to_pgm, 100, 125, iterations, RandomInteger);
    for (int i = 0; i < iterations; i++) {
        char filename[100];
        sprintf(filename, "../output/grayscale_square_%d.pgm", i + 1);
        WritePgm(filename, pgm_squares[i]);
    }


    // Blur the greyscale image
    PgmImage *pgm_blur = KasperBlur(ppm_to_pgm, 5);
    WritePgm("../output/grayscale_blur.pgm", pgm_blur);

    // No dithering
    PbmImage *pgm_to_pbm = PgmToPbm(ppm_to_pgm, MiddleThreshold);
    WritePbm("../output/binary.pbm", pgm_to_pbm);
    FreePbm(pgm_to_pbm);

    // Random dithering
    PbmImage *pgm_to_pbm_random = PgmToPbm(ppm_to_pgm, RandomThreshold);
    WritePbm("../output/binary_random.pbm", pgm_to_pbm_random);

    // Convert random to PGM
    PgmImage *pbm_to_pgm_random = PbmToPgm(pgm_to_pbm_random);
    FreePbm(pgm_to_pbm_random);
    WritePgm("../output/random.pgm", pbm_to_pgm_random);

    // Random dithering pgm_squares with difference
    for (int i = 0; i < iterations; i++) {
        char filename[100];
        sprintf(filename, "../output/square_random_%d.pbm", i + 1);
        PbmImage *pbm_square_random = PgmToPbm(pgm_squares[i], RandomThreshold);
        WritePbm(filename, pbm_square_random);

        PgmImage *pgm_square_random = PbmToPgm(pbm_square_random);
        PgmImage *square_random_diff =
            PgmDiff(pgm_square_random, pbm_to_pgm_random);
        char filename2[100];
        sprintf(filename2, "../output/square_random_diff_%d.pgm", i + 1);
        WritePgm(filename2, square_random_diff);

        FreePbm(pbm_square_random);
        FreePgm(pgm_square_random);
    }

    // Apply blur
    PgmImage *pbm_to_pgm_random_blur = KasperBlur(pbm_to_pgm_random, 5);
    WritePgm("../output/random_blur.pgm", pbm_to_pgm_random_blur);

    FreePgm(pbm_to_pgm_random);

    // compare using difference
    PgmImage *pbm_to_pgm_random_blur_diff =
        PgmDiff(pbm_to_pgm_random_blur, pgm_blur);
    WritePgm("../output/random_blur_diff.pgm", pbm_to_pgm_random_blur_diff);

    // show sum of difference
    double pbm_to_pgm_random_blur_diff_sum =
        PgmSum(pbm_to_pgm_random_blur_diff, 1);
    printf("Sum of difference: %f\n", pbm_to_pgm_random_blur_diff_sum);
    printf(
        "Average difference: %f\n",
        pbm_to_pgm_random_blur_diff_sum / (pbm_to_pgm_random_blur_diff->width_ *
                                           pbm_to_pgm_random_blur_diff->height_));
    printf("Variance: %f\n", PgmVariance(pbm_to_pgm_random_blur_diff));
    printf("Standard deviation: %f\n",
           pow(PgmVariance(pbm_to_pgm_random_blur_diff), .5));

    FreePgm(pbm_to_pgm_random_blur_diff);

    FreePgm(pbm_to_pgm_random_blur);

    FreePgm(pgm_blur);

    // Floyd-Steinberg dithering
    PbmImage *pgm_to_pbm_floyd = PgmToPbmFloydSteinberg(ppm_to_pgm);
    WritePbm("../output/binary_floyd.pbm", pgm_to_pbm_floyd);
    FreePbm(pgm_to_pbm_floyd);

    // Atkinson dithering
    PbmImage *pgm_to_pbm_atkinson = PgmToPbmAtkinson(ppm_to_pgm);
    WritePbm("../output/binary_atkinson.pbm", pgm_to_pbm_atkinson);
    FreePbm(pgm_to_pbm_atkinson);

    // Jarvis-Judice-Ninke dithering
    PbmImage *pgm_to_pbm_jarvis = PgmToPbmJarvisJudiceNinke(ppm_to_pgm);
    WritePbm("../output/binary_jarvis.pbm", pgm_to_pbm_jarvis);

    // Convert Jarvis-Judice-Ninke to PGM
    PgmImage *pbm_to_pgm_jarvis = PbmToPgm(pgm_to_pbm_jarvis);
    WritePgm("../output/jarvis.pgm", pbm_to_pgm_jarvis);

    // Apply blur
    PgmImage *pbm_to_pgm_jarvis_blur = KasperBlur(pbm_to_pgm_jarvis, 5);
    WritePgm("../output/jarvis_blur.pgm", pbm_to_pgm_jarvis_blur);
    FreePgm(pbm_to_pgm_jarvis_blur);

    FreePgm(pbm_to_pgm_jarvis);

    // Bayer dithering
    PbmImage *pgm_to_pbm_bayer = PgmToPbmBayer(ppm_to_pgm);
    WritePbm("../output/binary_bayer.pbm", pgm_to_pbm_bayer);

    // Bayer to pgm
    PgmImage *pbm_to_pgm_bayer = PbmToPgm(pgm_to_pbm_bayer);
    WritePgm("../output/bayer.pgm", pbm_to_pgm_bayer);

    FreePbm(pgm_to_pbm_bayer);

    // Bayer dithering pgm_squares with difference
    for (int i = 0; i < iterations; i++) {
        char filename[100];
        sprintf(filename, "../output/square_bayer_%d.pbm", i + 1);
        PbmImage *pbm_square_bayer = PgmToPbmBayer(pgm_squares[i]);
        WritePbm(filename, pbm_square_bayer);

        PgmImage *pgm_square_bayer = PbmToPgm(pbm_square_bayer);
        PgmImage *square_bayer_diff = PgmDiff(pgm_square_bayer, pbm_to_pgm_bayer);
        char filename2[100];
        sprintf(filename2, "../output/square_bayer_diff_%d.pgm", i + 1);
        WritePgm(filename2, square_bayer_diff);

        FreePbm(pbm_square_bayer);
        FreePgm(pgm_square_bayer);
    }

    // Kasper blur
    PgmImage *pgm_to_pgm_kasper_blur = KasperBlur(ppm_to_pgm, 3);
    WritePgm("../output/grayscale_kasper_blur.pgm", pgm_to_pgm_kasper_blur);

    FreePgm(ppm_to_pgm);

    // Kasper blur then Bayer dithering
    PbmImage *pgm_to_pbm_kasper_blur_bayer =
        PgmToPbmBayer(pgm_to_pgm_kasper_blur);
    WritePbm("../output/binary_kasper_blur_bayer.pbm",
             pgm_to_pbm_kasper_blur_bayer);
    FreePbm(pgm_to_pbm_kasper_blur_bayer);

    // Kasper blur then Floyd-Steinberg dithering
    PbmImage *pgm_to_pbm_kasper_blur_floyd =
        PgmToPbmFloydSteinberg(pgm_to_pgm_kasper_blur);
    WritePbm("../output/binary_kasper_blur_floyd.pbm",
             pgm_to_pbm_kasper_blur_floyd);
    FreePbm(pgm_to_pbm_kasper_blur_floyd);

    FreePgm(pgm_to_pgm_kasper_blur);

    PpmImage *linear = PpmPixelConvert(read_ppm, LinearRgb);
    WritePpm("../output/linear.ppm", linear);
    FreePpm(read_ppm);

    PgmImage *ppm_to_pgm_srgb = PpmToPgm(srgb, SRgbLuminance);
    WritePgm("../output/grayscale_srgb.pgm", ppm_to_pgm_srgb);
    FreePpm(srgb);

    // No dithering
    PbmImage *pgm_to_pbm_srgb = PgmToPbm(ppm_to_pgm_srgb, MiddleThreshold);
    WritePbm("../output/binary_srgb.pbm", pgm_to_pbm_srgb);
    FreePbm(pgm_to_pbm_srgb);

    // Random dithering
    PbmImage *pgm_to_pbm_srgb_random = PgmToPbm(ppm_to_pgm_srgb, RandomThreshold);
    WritePbm("../output/binary_srgb_random.pbm", pgm_to_pbm_srgb_random);
    FreePbm(pgm_to_pbm_srgb_random);

    // Floyd-Steinberg dithering
    PbmImage *pgm_to_pbm_srgb_floyd = PgmToPbmFloydSteinberg(ppm_to_pgm_srgb);
    WritePbm("../output/binary_srgb_floyd.pbm", pgm_to_pbm_srgb_floyd);
    FreePbm(pgm_to_pbm_srgb_floyd);

    // Atkinson dithering
    PbmImage *pgm_to_pbm_srgb_atkinson = PgmToPbmAtkinson(ppm_to_pgm_srgb);
    WritePbm("../output/binary_srgb_atkinson.pbm", pgm_to_pbm_srgb_atkinson);
    FreePbm(pgm_to_pbm_srgb_atkinson);

    // Jarvis-Judice-Ninke dithering
    PbmImage *pgm_to_pbm_srgb_jarvis = PgmToPbmJarvisJudiceNinke(ppm_to_pgm_srgb);
    WritePbm("../output/binary_srgb_jarvis.pbm", pgm_to_pbm_srgb_jarvis);
    FreePbm(pgm_to_pbm_srgb_jarvis);

    // Bayer dithering
    PbmImage *pgm_to_pbm_srgb_bayer = PgmToPbmBayer(ppm_to_pgm_srgb);
    WritePbm("../output/binary_srgb_bayer.pbm", pgm_to_pbm_srgb_bayer);
    FreePbm(pgm_to_pbm_srgb_bayer);

    FreePgm(ppm_to_pgm_srgb);

    PgmImage *ppm_to_pgm_linear = PpmToPgm(linear, LinearLuminance);
    WritePgm("../output/grayscale_linear.pgm", ppm_to_pgm_linear);
    FreePpm(linear);

    // No dithering
    PbmImage *pgm_to_pbm_linear = PgmToPbm(ppm_to_pgm_linear, MiddleThreshold);
    WritePbm("../output/binary_linear.pbm", pgm_to_pbm_linear);
    FreePbm(pgm_to_pbm_linear);

    // Random dithering
    PbmImage *pgm_to_pbm_linear_random =
        PgmToPbm(ppm_to_pgm_linear, RandomThreshold);
    WritePbm("../output/binary_linear_random.pbm", pgm_to_pbm_linear_random);
    FreePbm(pgm_to_pbm_linear_random);

    // Floyd-Steinberg dithering
    PbmImage *pgm_to_pbm_linear_floyd = PgmToPbmFloydSteinberg(ppm_to_pgm_linear);
    WritePbm("../output/binary_linear_floyd.pbm", pgm_to_pbm_linear_floyd);
    FreePbm(pgm_to_pbm_linear_floyd);

    // Atkinson dithering
    PbmImage *pgm_to_pbm_linear_atkinson = PgmToPbmAtkinson(ppm_to_pgm_linear);
    WritePbm("../output/binary_linear_atkinson.pbm", pgm_to_pbm_linear_atkinson);
    FreePbm(pgm_to_pbm_linear_atkinson);

    // Jarvis-Judice-Ninke dithering
    PbmImage *pgm_to_pbm_linear_jarvis =
        PgmToPbmJarvisJudiceNinke(ppm_to_pgm_linear);
    WritePbm("../output/binary_linear_jarvis.pbm", pgm_to_pbm_linear_jarvis);
    FreePbm(pgm_to_pbm_linear_jarvis);

    // Bayer dithering
    PbmImage *pgm_to_pbm_linear_bayer = PgmToPbmBayer(ppm_to_pgm_linear);
    WritePbm("../output/binary_linear_bayer.pbm", pgm_to_pbm_linear_bayer);
    FreePbm(pgm_to_pbm_linear_bayer);

    FreePgm(ppm_to_pgm_linear);

    // Free pgm_squares
    for (int i = 0; i < 10; i++) {
        FreePgm(pgm_squares[i]);
    }
    free(pgm_squares);

    return 0;
}
