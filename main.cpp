#include <iostream>
#include <fstream>
#include "Image.h"

int main (int argc, char** argv)
{
    std::ifstream file_in   (argv[1]);
    std::ofstream file_out1 (argv[2]);
    std::ofstream file_out2 (argv[3]);

    Image i (file_in);
    i.grayscale_weighted ();
    i.print_gs_matrix (file_out1);

    i.grayscale_avg ();
    i.print_gs_matrix (file_out2);

    return 0;
}

// g++ main.cpp -o main
// ./main in.ppm out1.ppm out2.ppm