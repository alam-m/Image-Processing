#include <iostream>
#include <fstream>
#include "Image.h"

int main (int argc, char** argv)
{
    std::ifstream img_in_file (argv[1]);

    Image i (img_in_file);
    i.grayscale_weighted();
    i.print_rgb_matrix();
    i.print_gs_matrix();

    return 0;
}