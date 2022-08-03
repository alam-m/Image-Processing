#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <fstream>
#include <iostream>
#include <string>
#include "Pixel.h"

/**
 *  Stores passed in .ppm image file.
 *  Only works with P2, P3 images for now.
 *  Does not filter comments (lines that start with '#')
 */
struct Image
{
    char    ppm_type[2];
    int     img_width;
    int     img_height;
    int     max_pixel_val;
    Pixel** pixel_matrix;

    Image (std::ifstream& file_in)
    {
        extract_header (file_in);
        extract_matrix (file_in);
    }
    ~Image ()
    {
        
    }

    void extract_header (std::ifstream & file_in)
    {
        file_in >> ppm_type;
        file_in >> img_height;
        file_in >> img_width;
        file_in >> max_pixel_val;
    }

    // Must be called after extract_header to work.
    void extract_matrix (std::ifstream & file_in)
    {
        pixel_matrix = new Pixel* [img_height];
        for (int i = 0; i < img_height; i++)
        {
            pixel_matrix [i] = new Pixel [img_width];
            for (int j = 0; j < img_width; j++)
            {
                if (ppm_type[1] == '3')
                {
                    file_in >> pixel_matrix [i][j].red;
                    file_in >> pixel_matrix [i][j].green;
                }
                file_in >> pixel_matrix [i][j].blue;
            }
        }
    }

    void grayscale_avg ()
    {
        int max_gray = max_pixel_val;
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                Pixel* p = &pixel_matrix[i][j];
                p->gray = (double)p->red / 3 + (double)p->green / 3 + (double)p->blue / 3;
                if (p->gray > max_gray)
                {
                    max_gray = 
                }
            }
        }
    }
    void grayscale_weighted ()
    {
        int max_gray = max_pixel_val;
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                Pixel* p = &pixel_matrix[i][j];
                p->gray = (double)p->red * 0.299 + (double)p->green * 0.587 + (double)p->blue * 0.114;
            }
        }
    }


    // debugging functions -------------------------------
    void print_rgb_matrix (std::ofstream& file_out)
    {
        file_out << "P3" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << max_pixel_val << '\n';
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                file_out << pixel_matrix[i][j].print_rgb() << ' ';
            }
            file_out << '\n';
        }
    }
    void print_gs_matrix (std::ofstream& file_out)
    {
        file_out << "P2" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << max_pixel_val << '\n';
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                file_out << pixel_matrix[i][j].print_gs() << ' ';
            }
            file_out << '\n';
        }
    }
};
#endif