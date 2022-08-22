#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <algorithm>
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
    // How many extra arrays on each 4 sides of image
    const int       ARRAY_PADDING_SIZE = 2;

    // Original Image Variables
    char            ppm_type[2];
    int             img_width;
    int             img_height;
    int             max_pixel_val;
    Pixel**         pixel_matrix;

    // Processed Variables
    std::string     og_file_name;
    int             max_gray_val;
    int*            histogram;

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
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
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
        init_pixel_matrix ();
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
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
    void init_pixel_matrix ()
    {
        pixel_matrix = new Pixel*[img_height + (ARRAY_PADDING_SIZE * 2)];
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            pixel_matrix[i] = new Pixel[img_width + (ARRAY_PADDING_SIZE * 2)];
        }
    }

    /*----------------------------------------------------------------------------------------------------*/
    // grayscale 
    void grayscale_avg ()
    {
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                Pixel* p = & pixel_matrix[i][j];
                p->gray = (double)p->red / 3 + (double)p->green / 3 + (double)p->blue / 3;
                if (p->gray > max_gray_val) 
                { 
                    max_gray_val = p->gray; 
                }
            }
        }
    }
    void grayscale_weighted ()
    {
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                Pixel* p = & pixel_matrix[i][j];
                p->gray = (double)p->red * 0.299 + (double)p->green * 0.587 + (double)p->blue * 0.114;
                if (p->gray > max_gray_val) 
                { 
                    max_gray_val = p->gray; 
                }
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

    /*----------------------------------------------------------------------------------------------------*/
    // For binary image conversion
    void threshold (const int& threshold_value)
    {
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                if (pixel_matrix[i][j].gray < threshold_value)
                {
                    pixel_matrix[i][j].black = 0;
                }
                else
                {
                    pixel_matrix[i][j].black = 1;
                }
            }
        }
    }
    int get_median ()
    {
        int* ar = new int [img_height * img_width];
        int median = 0;
        int index = 0;
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                ar[index++] = pixel_matrix[i][j].gray;
            }
        }
        std::sort (ar, ar + sizeof (ar) / sizeof (ar[0]));
        median = ar[(img_height * img_width) / 2];
        delete[] ar;
        return median;
    }

    /*----------------------------------------------------------------------------------------------------*/
    // Gaussian mask
    void gaussian_smooth ()
    {
        // Gaussian mask array I got online
        int mask[5][5] = 
        {
             {1,  4,  7,  4, 1}
            ,{4, 16, 26, 16, 4}
            ,{7, 26, 41, 26, 7}
            ,{4, 16, 26, 16, 7}
            ,{1,  4,  7,  4, 1}
        };
        int weight = get_weight (mask, 5, 5);

        // to hold the temporary smoothed values
        int** temp;
        temp = new int*[img_height + (ARRAY_PADDING_SIZE * 2)];
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            temp[i] = new int[img_width + (ARRAY_PADDING_SIZE * 2)];
        }

        // convolute
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                temp[i][j] = convolution (mask, i, j) / weight;
            }
        }

        // move the values over to pixel matrix
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                pixel_matrix[i][j].gray = temp[i][j];
            }
        }

        // clean up temp array
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            delete[] temp[i];
        }
        delete temp;
    }
    void mirror_frame ()
    {
        // sides
        for (int i = 0; i < ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_height + ARRAY_PADDING_SIZE; j++)
            {
                // left
                pixel_matrix[j][ARRAY_PADDING_SIZE - 1 - i] = pixel_matrix[j][ARRAY_PADDING_SIZE + i];
                // right
                pixel_matrix[j][ARRAY_PADDING_SIZE + img_width + i] = pixel_matrix[j][ARRAY_PADDING_SIZE + img_width - 1 - i];
            }
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                // top
                pixel_matrix[ARRAY_PADDING_SIZE - 1 - i][j] = pixel_matrix[ARRAY_PADDING_SIZE + i][j];
                // bottom
                pixel_matrix[ARRAY_PADDING_SIZE + img_height + i][j] = pixel_matrix[ARRAY_PADDING_SIZE + img_height - 1 - i][j];
            }
        }
        
        // corners
        // top left
        rotate_matrix_90_clockwise (ARRAY_PADDING_SIZE             , 0                             , 0                              , 0);
        // top right
        rotate_matrix_90_clockwise (0                              , img_width                     , 0                              , img_width + ARRAY_PADDING_SIZE);
        // bottom left
        rotate_matrix_90_clockwise (img_height + ARRAY_PADDING_SIZE, ARRAY_PADDING_SIZE            , img_height + ARRAY_PADDING_SIZE, 0                             );
        // bottom right
        rotate_matrix_90_clockwise (img_height                     , img_width + ARRAY_PADDING_SIZE, img_height + ARRAY_PADDING_SIZE, img_width + ARRAY_PADDING_SIZE);
    }
    void rotate_matrix_90_clockwise (const int& i1, const int& j1, const int& i2, const int& j2)
    {
        int x1 = i1;
        int y2 = j2 + ARRAY_PADDING_SIZE - 1;
        for (; x1 < i1 + ARRAY_PADDING_SIZE; x1++)
        {
            int y1 = j1;
            int x2 = i2;
            for (; y1 < j1 + ARRAY_PADDING_SIZE; y1++)
            {
                pixel_matrix[x2][y2] = pixel_matrix[x1][y1];
                x2++;
            }
            y2--;
        }
    }
    int get_weight (int arr_2D[5][5], const int& rows, const int& columns)
    {
        int weight = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                weight += arr_2D[i][j];
            }
        }
        return weight;
    }
    int convolution (int mask[5][5], const int& i, const int& j)
    {
        int sum = 0;
        int x1 = i - ARRAY_PADDING_SIZE;
        for (int x2 = 0; x2 < 5; x2++)
        {
            int y1 = j - ARRAY_PADDING_SIZE;
            for (int y2 = 0; y2 < 5; y2++)
            {
                sum += pixel_matrix[x1][y1].gray * mask[x2][y2];
                y1++;
            }
            x1++;
        }
        return sum;
    }

    /*----------------------------------------------------------------------------------------------------*/
    // Canny Edge detection
    

    /*----------------------------------------------------------------------------------------------------*/
    // Hough Transform


    /*----------------------------------------------------------------------------------------------------*/
    // debugging functions
    void print_rgb_matrix () { print_rgb_matrix (og_file_name + "_rgb"); }
    void print_rgb_matrix (const std::string& file_out_name)
    {
        std::ofstream file_out ("images-out/" + file_out_name + ".ppm");

        file_out << "P3" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << max_pixel_val << '\n';
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                file_out << pixel_matrix[i][j].print_rgb() << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }

    void print_gs_matrix () { print_gs_matrix (og_file_name + "_gs"); }
    void print_gs_matrix (const std::string& file_out_name)
    {
        std::ofstream file_out ("images-out/" + file_out_name + ".ppm");

        file_out << "P2" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << max_gray_val << '\n';
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                file_out << pixel_matrix[i][j].print_gs() << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }

    void print_histogram () { print_histogram (og_file_name + "_histogram"); }
    void print_histogram (const std::string& file_out_name)
    {
        std::ofstream file_out ("images-out/" + file_out_name + ".csv");

        file_out << "Gray Value,Count" << '\n';
        for (int i = 0; i < max_gray_val + 1; i++)
        {
            file_out << i << ',' << histogram[i] << '\n';
        }
        file_out.close ();
    }

    void print_bi_matrix () { print_bi_matrix (og_file_name + "_bi"); }
    void print_bi_matrix (const std::string& file_out_name)
    {
        std::ofstream file_out ("images-out/" + file_out_name + ".ppm");

        file_out << "P2" << '\n';
        file_out << img_width << ' ' << img_height << '\n';
        file_out << 1 << '\n';
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                file_out << pixel_matrix[i][j].print_bi() << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }

    void print_matrix () { print_matrix (og_file_name + "_test"); }
    void print_matrix (const std::string& file_out_name)
    {
        std::ofstream file_out ("images-out/" + file_out_name + ".txt");
        for (int i = 0; i < img_height + ARRAY_PADDING_SIZE * 2; i++)
        {
            for (int j = 0; j < img_width + ARRAY_PADDING_SIZE * 2; j++)
            {
                file_out << pixel_matrix[i][j].print_gs() << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }

};
#endif