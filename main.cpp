#include "Image.h"

int main (int argc, char** argv)
{
    Image i (argv[1]);

    // i.grayscale_avg ();
    i.grayscale_weighted ();
    i.print_gs_matrix ();

    i.create_histogram ();
    i.print_histogram ();

    i.threshold (i.get_median ());
    i.print_bi_matrix ();

    i.mirror_frame ();
    i.gaussian_smooth ();

    i.edge_detect_sobel ();
    i.historesis_threshold (0.05, 0.2);

    i.hough_transform ();

    return 0;
}

// g++ main.cpp -o main -std=c++17
// ./main images-in/landscape.ppm
// ./main images-in/monument.ppm
// ./main images-in/skylight.ppm
// ./main images-in/apollo.ppm
// ./main images-in/in.ppm
// ./main images-in/canny_test_sq.ppm
// ./main images-in/canny_test_oct.ppm
// ./main images-in/canny_test_diag1.ppm