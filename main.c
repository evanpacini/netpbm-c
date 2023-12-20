#include <string.h>

#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include "sat.h"

void doAllTheThings(char *input, char *output) {
    char outputPath[100] = "../output/";
    strcat(outputPath, output);

    // Read a ppm image
    PpmImage *ppm = ReadPpm(input);

    // Write the ppm image
    char outputPpm[100];
    strcpy(outputPpm, outputPath);
    strcat(outputPpm, ".ppm");
    WritePpm(ppm, outputPpm);

    // Convert to pgm
    PgmImage *pgm = PpmToPgm(ppm, SRgbLuminance);

    // Write the pgm image
    char outputPgm[100];
    strcpy(outputPgm, outputPath);
    strcat(outputPgm, ".pgm");
    WritePgm(pgm, outputPgm);

    // Convert to pbm (thresholding)
    PbmImage *pbm_threshold = PgmToPbm(pgm, MiddleThreshold);

    // Write the pbm image
    char outputPbmThreshold[100];
    strcpy(outputPbmThreshold, outputPath);
    strcat(outputPbmThreshold, "_threshold.pbm");
    WritePbm(pbm_threshold, outputPbmThreshold);

    // Free the image
    FreePbm(pbm_threshold);

    // Convert to pbm (random thresholding)
    PbmImage *pbm_random_threshold = PgmToPbm(pgm, RandomThreshold);

    // Write the pbm image
    char outputPbmRandomThreshold[100];
    strcpy(outputPbmRandomThreshold, outputPath);
    strcat(outputPbmRandomThreshold, "_random_threshold.pbm");
    WritePbm(pbm_random_threshold, outputPbmRandomThreshold);

    // Free the image
    FreePbm(pbm_random_threshold);

    // Convert to pgm (IGN thresholding)
    PbmImage *pbm_ign = PgmToPbm(pgm, IgnThreshold);

    // Write the pbm image
    char outputPbmIgn[100];
    strcpy(outputPbmIgn, outputPath);
    strcat(outputPbmIgn, "_ign.pbm");
    WritePbm(pbm_ign, outputPbmIgn);

    // Free the image
    FreePbm(pbm_ign);

    // Convert to pbm (atkinson)
    PbmImage *pbm_atkinson = PgmToPbmAtkinson(pgm);

    // Write the pbm image
    char outputPbmAtkinson[100];
    strcpy(outputPbmAtkinson, outputPath);
    strcat(outputPbmAtkinson, "_atkinson.pbm");
    WritePbm(pbm_atkinson, outputPbmAtkinson);

    // Free the image
    FreePbm(pbm_atkinson);

    // Convert to pbm (floyd-steinberg)
    PbmImage *pbm_floyd_steinberg = PgmToPbmFloydSteinberg(pgm);

    // Write the pbm image
    char outputPbmFloydSteinberg[100];
    strcpy(outputPbmFloydSteinberg, outputPath);
    strcat(outputPbmFloydSteinberg, "_floyd_steinberg.pbm");
    WritePbm(pbm_floyd_steinberg, outputPbmFloydSteinberg);

    // Free the image
    FreePbm(pbm_floyd_steinberg);

    // Convert to pbm (jarvis-judice-ninke)
    PbmImage *pbm_jarvis_judice_ninke = PgmToPbmJarvisJudiceNinke(pgm);

    // Write the pbm image
    char outputPbmJarvisJudiceNinke[100];
    strcpy(outputPbmJarvisJudiceNinke, outputPath);
    strcat(outputPbmJarvisJudiceNinke, "_jarvis_judice_ninke.pbm");
    WritePbm(pbm_jarvis_judice_ninke, outputPbmJarvisJudiceNinke);

    // Free the image
    FreePbm(pbm_jarvis_judice_ninke);

    // Convert to pbm (bayer 2x2)
    PgmImage *pgm_bayer_2x2 = ReadPgm("../textures/bayer/2x2.pgm");
    PbmImage *pbm_bayer_2x2 = PgmToPbmOrdered(pgm, pgm_bayer_2x2);

    // Write the pbm image
    char outputPbmBayer2x2[100];
    strcpy(outputPbmBayer2x2, outputPath);
    strcat(outputPbmBayer2x2, "_bayer_2x2.pbm");
    WritePbm(pbm_bayer_2x2, outputPbmBayer2x2);

    // Free the image
    FreePbm(pbm_bayer_2x2);
    FreePgm(pgm_bayer_2x2);

    // Convert to pbm (bayer 4x4)
    PgmImage *pgm_bayer_4x4 = ReadPgm("../textures/bayer/4x4.pgm");
    PbmImage *pbm_bayer_4x4 = PgmToPbmOrdered(pgm, pgm_bayer_4x4);

    // Write the pbm image
    char outputPbmBayer4x4[100];
    strcpy(outputPbmBayer4x4, outputPath);
    strcat(outputPbmBayer4x4, "_bayer_4x4.pbm");
    WritePbm(pbm_bayer_4x4, outputPbmBayer4x4);

    // Free the image
    FreePbm(pbm_bayer_4x4);

    // Convert to pbm (bayer 8x8)
    PgmImage *pgm_bayer_8x8 = ReadPgm("../textures/bayer/8x8.pgm");
    PbmImage *pbm_bayer_8x8 = PgmToPbmOrdered(pgm, pgm_bayer_8x8);

    // Write the pbm image
    char outputPbmBayer8x8[100];
    strcpy(outputPbmBayer8x8, outputPath);
    strcat(outputPbmBayer8x8, "_bayer_8x8.pbm");
    WritePbm(pbm_bayer_8x8, outputPbmBayer8x8);

    // Free the image
    FreePbm(pbm_bayer_8x8);

    // Convert to pbm (bayer 16x16)
    PgmImage *pgm_bayer_16x16 = ReadPgm("../textures/bayer/16x16.pgm");
    PbmImage *pbm_bayer_16x16 = PgmToPbmOrdered(pgm, pgm_bayer_16x16);

    // Write the pbm image
    char outputPbmBayer16x16[100];
    strcpy(outputPbmBayer16x16, outputPath);
    strcat(outputPbmBayer16x16, "_bayer_16x16.pbm");
    WritePbm(pbm_bayer_16x16, outputPbmBayer16x16);

    // Free the image
    FreePbm(pbm_bayer_16x16);

    // Convert to pbm (blue noise 16x16)
    PgmImage *pgm_blue_noise_16x16 = ReadPgm("../textures/blue_noise/16x16/0.pgm");
    PbmImage *pbm_blue_noise_16x16 = PgmToPbmOrdered(pgm, pgm_blue_noise_16x16);

    // Write the pbm image
    char outputPbmBlueNoise16x16[100];
    strcpy(outputPbmBlueNoise16x16, outputPath);
    strcat(outputPbmBlueNoise16x16, "_blue_noise_16x16.pbm");
    WritePbm(pbm_blue_noise_16x16, outputPbmBlueNoise16x16);

    // Free the image
    FreePgm(pgm_blue_noise_16x16);
    FreePbm(pbm_blue_noise_16x16);

    // Convert to pbm (blue noise 32x32)
    PgmImage *pgm_blue_noise_32x32 = ReadPgm("../textures/blue_noise/32x32/0.pgm");
    PbmImage *pbm_blue_noise_32x32 = PgmToPbmOrdered(pgm, pgm_blue_noise_32x32);

    // Write the pbm image
    char outputPbmBlueNoise32x32[100];
    strcpy(outputPbmBlueNoise32x32, outputPath);
    strcat(outputPbmBlueNoise32x32, "_blue_noise_32x32.pbm");
    WritePbm(pbm_blue_noise_32x32, outputPbmBlueNoise32x32);

    // Free the image
    FreePgm(pgm_blue_noise_32x32);
    FreePbm(pbm_blue_noise_32x32);

    // Convert to pbm (blue noise 64x64)
    PgmImage *pgm_blue_noise_64x64 = ReadPgm("../textures/blue_noise/64x64/0.pgm");
    PbmImage *pbm_blue_noise_64x64 = PgmToPbmOrdered(pgm, pgm_blue_noise_64x64);

    // Write the pbm image
    char outputPbmBlueNoise64x64[100];
    strcpy(outputPbmBlueNoise64x64, outputPath);
    strcat(outputPbmBlueNoise64x64, "_blue_noise_64x64.pbm");
    WritePbm(pbm_blue_noise_64x64, outputPbmBlueNoise64x64);

    // Free the image
    FreePgm(pgm_blue_noise_64x64);
    FreePbm(pbm_blue_noise_64x64);

    // Convert to pbm (blue noise 128x128)
    PgmImage *pgm_blue_noise_128x128 = ReadPgm("../textures/blue_noise/128x128/0.pgm");
    PbmImage *pbm_blue_noise_128x128 = PgmToPbmOrdered(pgm, pgm_blue_noise_128x128);

    // Write the pbm image
    char outputPbmBlueNoise128x128[100];
    strcpy(outputPbmBlueNoise128x128, outputPath);
    strcat(outputPbmBlueNoise128x128, "_blue_noise_128x128.pbm");
    WritePbm(pbm_blue_noise_128x128, outputPbmBlueNoise128x128);

    // Free the image
    FreePgm(pgm_blue_noise_128x128);
    FreePbm(pbm_blue_noise_128x128);


    // Free remaining images
    FreePgm(pgm);
    FreePpm(ppm);
}

int main(void) {
    doAllTheThings("../input/lena.ppm", "lena");
    return 0;
}
