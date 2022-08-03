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
class Image
{
public:

    char    ppm_type[2];
    int     img_width;
    int     img_height;
    int     max_pixel_val;
    Pixel** pixel_matrix;
    int**   grayscale_matrix;

    Image (std::ifstream& img_in_file)
    {
        extract_header (img_in_file);
        extract_matrix (img_in_file);
    }
    ~Image ()
    {
        
    }
    
    void print_rgb_matrix ()
    {
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                std::cout << pixel_matrix[i][j].to_string() << ' ';
            }
            std::cout << '\n';
        }
    }
    void print_gs_matrix ()
    {
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                std::cout << grayscale_matrix[i][j] << ' ';
            }
            std::cout << '\n';
        }
    }

    void extract_header (std::ifstream & img_in_file)
    {
        img_in_file >> ppm_type;
        img_in_file >> img_height;
        img_in_file >> img_width;
        img_in_file >> max_pixel_val;
    }

    // Must be called after extract_header to work.
    void extract_matrix (std::ifstream & img_in_file)
    {
        pixel_matrix = new Pixel* [img_height];
        for (int i = 0; i < img_height; i++)
        {
            pixel_matrix [i] = new Pixel [img_width];
            for (int j = 0; j < img_width; j++)
            {
                if (ppm_type[1] == '3')
                {
                    img_in_file >> pixel_matrix [i][j].r;
                    img_in_file >> pixel_matrix [i][j].g;
                }
                img_in_file >> pixel_matrix [i][j].b;
            }
        }
    }

    void grayscale_avg ()
    {
        grayscale_matrix = new int * [img_height];
        for (int i = 0; i < img_height; i++)
        {
            grayscale_matrix [i] = new int [img_width];
            for (int j = 0; j < img_width; j++)
            {
                grayscale_matrix[i][j] = (double)pixel_matrix[i][j].r / 3 + (double)pixel_matrix[i][j].g / 3 + (double)pixel_matrix[i][j].b / 3;
            }
        }
    }
    void grayscale_weighted ()
    {
        grayscale_matrix = new int * [img_height];
        for (int i = 0; i < img_height; i++)
        {
            grayscale_matrix [i] = new int [img_width];
            for (int j = 0; j < img_width; j++)
            {
                grayscale_matrix[i][j] = (double)pixel_matrix[i][j].r * 0.299 + (double)pixel_matrix[i][j].g * 0.587 + (double)pixel_matrix[i][j].b * 0.114;
            }
        }
    }
};
#endif