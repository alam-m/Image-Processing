#include <iostream>
#include <fstream>
#include "Image.h"

int main (int argc, char** argv)
{
    std::ifstream file_in   (argv[1]);
    std::ofstream file_out1 (argv[2]);
    std::ofstream file_out2 (argv[3]);
    std::ofstream file_out3 (argv[4]);
    std::ofstream file_out4 (argv[5]);

    Image i (file_in);
    file_in.close ();

    i.grayscale_avg ();
    i.print_gs_matrix (file_out1);
    i.create_histogram ();
    i.print_histogram (file_out3);

    i.grayscale_weighted ();
    i.print_gs_matrix (file_out2);
    i.create_histogram ();
    i.print_histogram (file_out3);

    return 0;
}

// g++ main.cpp -o main
// ./main in.ppm out1.ppm out2.ppm out3.csv