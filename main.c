#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "pgm.h"

PGMImage *ppm_to_pgm(const PPMImage *image, double (*luminance)(const Pixel *)) {
    /* Allocate memory for PGM image */
    PGMImage *pgm_image = (PGMImage *) malloc(sizeof(PGMImage));
    if (!pgm_image) {
        fprintf(stderr, "Error: could not allocate memory for PGM image\n");
        return NULL;
    }

    // Copy image header to PGM image
    memcpy(pgm_image, image, sizeof(PPMImage));

    // Allocate memory for image data
    pgm_image->data = (uint8_t *) malloc(pgm_image->width * pgm_image->height * sizeof(uint8_t) + pgm_image->height);
    if (!pgm_image->data) {
        fprintf(stderr, "Error: could not allocate memory for PGM image data\n");
        free(pgm_image);
        return NULL;
    }

    // Convert pixel data from PPM image to PGM image
    for (uint32_t i = 0; i < pgm_image->height; i++) {
        for (uint32_t j = 0; j < pgm_image->width; j++) {
            // Get pixel from PPM image
            Pixel p = image->data[i * image->width + j];

            // Calculate linear_luminance of pixel
            uint8_t y = (uint8_t) luminance(&p);

            // Set linear_luminance value in PGM image
            pgm_image->data[i * pgm_image->width + j] = y;
        }
    }

    return pgm_image;
}

int main(void) {
    PPMImage *readPpm = read_ppm("../input/tud2.ppm");
    write_ppm("../output/normal.ppm", readPpm);

    PPMImage *srgb = ppm_pixel_convert(readPpm, sRGB);
    write_ppm("../output/srgb.ppm", srgb);

    PGMImage *ppmToPgm = ppm_to_pgm(readPpm, sRGB_luminance);
    write_pgm("../output/grayscale.pgm", ppmToPgm);
    free_pgm(ppmToPgm);

    PPMImage *linear = ppm_pixel_convert(readPpm, linearRGB);
    write_ppm("../output/linear.ppm", linear);
    free_ppm(readPpm);

    PGMImage *ppmToPgmSrgb = ppm_to_pgm(srgb, sRGB_luminance);
    write_pgm("../output/grayscale_srgb.pgm", ppmToPgmSrgb);
    free_ppm(srgb);
    free_pgm(ppmToPgmSrgb);

    PGMImage *ppmToPgmLinear = ppm_to_pgm(linear, linear_luminance);
    write_pgm("../output/grayscale_linear.pgm", ppmToPgmLinear);
    free_ppm(linear);
    free_pgm(ppmToPgmLinear);

    return 0;
}
