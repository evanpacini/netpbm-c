#include "pbm.h"
#include "pgm.h"
#include "ppm.h"
#include "sat.h"

int main(void) {
    // Read a pbm image
    PbmImage *pbm = ReadPbm("../output/lena_ign.pbm");

    // Convert to pgm
    PgmImage *pgm = PbmToPgm(pbm);

    // Do something illegal
    SetPixelPgm(pgm, 0, 0);

    // Write the pgm image
    WritePgm(pgm, "../output/lena_ign.pgm");

    // Free the image
    FreePgm(pgm);
    FreePbm(pbm);

    return 0;
}
