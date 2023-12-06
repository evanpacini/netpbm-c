#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include "sat.h"

int main(void) {
    // Read the input image.
    PpmImage *image = ReadPpm("../input/lenna.ppm");

    // Convert the image to grayscale.
    PgmImage *grayscale = PpmToPgm(image, SRgbLuminance);

    // Read a threshold map.
    PgmImage *map = ReadPgm("../textures/bayer/2x2.pgm");

    // Dither the image to 1-bit images.
    PbmImage *ditheredBayer = PgmToPbmOrdered(grayscale, map);
    PbmImage *ditheredIgn   = PgmToPbm(grayscale, IgnThreshold);

    // Write the dithered images to a file.
    WritePbm(ditheredBayer, "../output/lenna_bayer_2x2.pbm");
    WritePbm(ditheredIgn, "../output/lenna_ign.pbm");

    // Free the memory used by the images.
    FreePbm(ditheredIgn);
    FreePbm(ditheredBayer);
    FreePgm(map);
    FreePgm(grayscale);
    FreePpm(image);

    return 0;
}
