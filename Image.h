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
    // Original Image Variables
    char        ppm_type[2];
    int         img_width;
    int         img_height;
    int         max_pixel_val;
    Pixel**     pixel_matrix;

    // Processed Variables
    std::string og_file_name;
    int         max_gray_val;
    int*        histogram;

    /*----------------------------------------------------------------------------------------------------*/
    // Constructor 
    Image (const std::string& file_in)
    {
        std::ifstream in_file (file_in);
        if (!in_file.is_open ()) { std::cout << "couldn't open " << file_in << '\n'; exit; }

        extract_header (in_file);
        extract_matrix (in_file);
        in_file.close ();

        og_file_name = file_in.substr (og_file_name.find("images-in/") + 11);
        og_file_name.resize (og_file_name.length() - 4);
        max_gray_val = 0;
    }
    ~Image ()
    {
        for (int i = 0; i < img_height; i++)
        {
            delete[] pixel_matrix[i];
        }
        delete[] pixel_matrix;
        delete[] histogram;
    }

    void extract_header (std::ifstream& file_in)
    {
        file_in >> ppm_type;
        file_in >> img_width;
        file_in >> img_height;
        file_in >> max_pixel_val;
    }
    // Must be called after extract_header to work.
    void extract_matrix (std::ifstream& file_in)
    {
        pixel_matrix = new Pixel*[img_height];
        for (int i = 0; i < img_height; i++)
        {
            pixel_matrix[i] = new Pixel[img_width];
            for (int j = 0; j < img_width; j++)
            {
                if (ppm_type[1] == '3')
                {
                    file_in >> pixel_matrix[i][j].red;
                    file_in >> pixel_matrix[i][j].green;
                }
                file_in >> pixel_matrix[i][j].blue;
            }
        }
    }

    /*----------------------------------------------------------------------------------------------------*/

    void grayscale_avg ()
    {
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                Pixel* p = & pixel_matrix[i][j];
                p->gray = (double)p->red / 3 + (double)p->green / 3 + (double)p->blue / 3;
                if (p->gray > max_gray_val) { max_gray_val = p->gray; }
            }
        }
    }
    void grayscale_weighted ()
    {
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                Pixel* p = & pixel_matrix[i][j];
                p->gray = (double)p->red * 0.299 + (double)p->green * 0.587 + (double)p->blue * 0.114;
                if (p->gray > max_gray_val) { max_gray_val = p->gray; }
            }
        }
    }
    void create_histogram ()
    {
        histogram = new int[max_gray_val + 1];
        for (int i = 0; i < max_gray_val + 1; i++)
        {
            histogram[i] = 0;
        }
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                histogram[pixel_matrix[i][j].gray]++;
            }
        }
    }
    void auto_threshold ()
    {

    }

    /*----------------------------------------------------------------------------------------------------*/
    // debugging functions
    void print_rgb_matrix ()
    {
        std::ofstream file_out ("images-out/" + og_file_name + "_rgb.ppm");

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
        file_out.close ();
    }
    void print_gs_matrix ()
    {
        std::ofstream file_out ("images-out/" + og_file_name + "_gs.ppm");

        file_out << "P2" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << max_gray_val << '\n';
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                file_out << pixel_matrix[i][j].print_gs() << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }
    void print_histogram ()
    {
        std::ofstream file_out ("images-out/" + og_file_name + "_histogram.csv");

        file_out << "Gray Value,Count" << '\n';
        for (int i = 0; i < max_gray_val + 1; i++)
        {
            file_out << i << ',' << histogram[i] << '\n';
        }
        file_out.close ();
    }
};
#endif