#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <fstream>

/**
 *  Stores passed in .ppm image file.
 *  Only works with P1, P2, P3 images for now.
 */
class Image
{
public:

    Image (std::ifstream& img_in_file)
    {
        extract_header (img_in_file);
        extract_matrix (img_in_file);

    }

private:

    struct Pixel
    {
        int r, g, b;
        Pixel (const int& red)
        {
            r = red;
            g = 0;
            b = 0;
        }
        Pixel (const int& red, const int& green, const int& blue)
        {
            r = red;
            g = green;
            b = blue;
        }
    };

    char    ppm_type[2];
    int     img_width;
    int     img_height;
    int     max_pixel_val;
    Pixel** img_matrix;
    int     matrix_width;
    int     matrix_height;

    ~Image ()
    {
        
    }

    void extract_header (std::ifstream& img_in_file)
    {
        img_in_file >> ppm_type;
        img_in_file >> img_height;
        img_in_file >> img_width;
        img_in_file >> max_pixel_val;
    }

    void extract_matrix (std::ifstream& img_in_file)
    {

    }
};
#endif