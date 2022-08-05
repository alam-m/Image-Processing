#include <iostream>
#include "Image.h"

int main (int argc, char** argv)
{
    Image i (argv[1]);

    i.grayscale_avg ();
    i.print_gs_matrix ();
    i.create_histogram ();
    i.print_histogram ();

    // i.grayscale_weighted ();
    // i.print_gs_matrix ();
    // i.create_histogram ();
    // i.print_histogram ();

    return 0;
}

// g++ main.cpp -o main
// ./main in.ppm out1.ppm out2.ppm out3.csv