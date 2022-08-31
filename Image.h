#pragma once
#ifndef IMAGE_H
#define IMAGE_H

#include <algorithm>    // sort(), planning on just making my own sorting algo
#include <filesystem>   // create_directory()
#include <fstream>      // ifstream, ofstream
#include <iostream>     // cout
#include <iomanip>
#include <math.h>       // sqrt(), pow()
#include <string>       // string
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
        // remove me
        if (ppm_type[1] == '2')
        {
            for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
            {
                for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
                {
                    Pixel* p = & pixel_matrix[i][j];
                    p->gray = p->blue;
                    if (p->gray > max_gray_val) 
                    { 
                        max_gray_val = p->gray; 
                    }
                }
            }
        }

        else 
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
                temp[i][j] = convolution_5x5 (mask, i, j) / weight;
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

        print_gs_matrix (og_file_name + "_gs_smooth");
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
    int convolution_5x5 (int mask[5][5], const int& i, const int& j)
    {
        int sum = 0;
        int x1 = i - 2;
        for (int x2 = 0; x2 < 5; x2++)
        {
            int y1 = j - 2;
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
    // Sobel Edge detection
    void edge_detect_sobel ()
    {
        // Sobel mask arrays I got online
        int v_mask_3x3[3][3] = 
        {
             {-1,  0,  1}
            ,{-2,  0,  1}
            ,{-1,  0,  1}
        };
        int h_mask_3x3[3][3] = 
        {
             { 1,  2,  1}
            ,{ 0,  0,  0}
            ,{-1, -2, -1}
        };
        int v_mask_5x5[5][5] = 
        {
             {-1, -2,  0,  2,  1}
            ,{-2, -3,  0,  3,  2}
            ,{-3, -5,  0,  5,  3}
            ,{-2, -3,  0,  3,  2}
            ,{-1, -2,  0,  2,  1}
        };
        int h_mask_5x5[5][5] = 
        {
             { 1,  2,  3,  2,  1}
            ,{ 2,  3,  5,  3,  2}
            ,{ 0,  0,  0,  0,  0}
            ,{-2, -3, -5, -3, -2}
            ,{-1, -2, -3, -2, -1}
        };

        // holds vertical and horizontal Sobel's
        int** v_temp;
        int** h_temp;
        v_temp = new int*[img_height + (ARRAY_PADDING_SIZE * 2)];
        h_temp = new int*[img_height + (ARRAY_PADDING_SIZE * 2)];
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            v_temp[i] = new int[img_width + (ARRAY_PADDING_SIZE * 2)];
            h_temp[i] = new int[img_width + (ARRAY_PADDING_SIZE * 2)];
        }

        // convolute
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                v_temp[i][j] = convolution_5x5 (v_mask_5x5, i, j);
                h_temp[i][j] = convolution_5x5 (h_mask_5x5, i, j);
            }
        }

        // move the values over to pixel matrix
        int max_v = 0;
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                pixel_matrix[i][j].gray = v_temp[i][j];
                v_temp[i][j] > max_v ? max_v = v_temp[i][j] : 0;
            }
        }
        max_gray_val = max_v;
        print_gs_matrix (og_file_name + "_sobel_v");

        int max_h = 0;
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                pixel_matrix[i][j].gray = h_temp[i][j];
                h_temp[i][j] > max_h ? max_h = h_temp[i][j] : 0;
            }
        }
        max_gray_val = max_h;
        print_gs_matrix (og_file_name + "_sobel_h");

        int max = 0;
        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                // int dist = abs (v_temp[i][j]) + abs (h_temp[i][j]);
                int dist = sqrt (pow (v_temp[i][j], 2) + pow (h_temp[i][j], 2));
                pixel_matrix[i][j].gray = dist;
                dist > max ? max = dist : 0;
            }
        }
        max_gray_val = max;
        print_gs_matrix (og_file_name + "_sobel");

        edge_detect_canny (v_temp, h_temp);
    }
    int convolution_3x3 (int mask[3][3], const int& i, const int& j)
    {
        int sum = 0;
        int x1 = i - 1;
        for (int x2 = 0; x2 < 3; x2++)
        {
            int y1 = j - 1;
            for (int y2 = 0; y2 < 3; y2++)
            {
                sum += pixel_matrix[x1][y1].gray * mask[x2][y2];
                y1++;
            }
            x1++;
        }
        return sum;
    }

    /*----------------------------------------------------------------------------------------------------*/
    // Canny Edge
    void edge_detect_canny (int** v_temp, int** h_temp)
    {
        const double PI = 3.14159265;

        //init angles array
        int** gradient_angles = new int*[img_height + (ARRAY_PADDING_SIZE * 2)];
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            gradient_angles[i] = new int[img_width + (ARRAY_PADDING_SIZE * 2)];
        }

        for (int i = ARRAY_PADDING_SIZE; i < img_height + ARRAY_PADDING_SIZE; i++)
        {
            for (int j = ARRAY_PADDING_SIZE; j < img_width + ARRAY_PADDING_SIZE; j++)
            {
                if (pixel_matrix[i][j].gray > 0)
                {
                    gradient_angles[i][j] = atan2 (v_temp[i][j], h_temp[i][j]) * 180 / PI;
                }
                else
                {
                    gradient_angles[i][j] = -1;
                }
            }
        }
        print_debug_matrix ("gradient_angles", gradient_angles, 0, img_height + (ARRAY_PADDING_SIZE * 2), 0, img_width + (ARRAY_PADDING_SIZE * 2));

        // clean up temp arrays
        for (int i = 0; i < img_height + (ARRAY_PADDING_SIZE * 2); i++)
        {
            delete[] v_temp[i];
            delete[] h_temp[i];
            delete[] gradient_angles[i];
        }
        delete[] v_temp;
        delete[] h_temp;
        delete[] gradient_angles;
    }

    /*----------------------------------------------------------------------------------------------------*/
    // Hough Transform


    /*----------------------------------------------------------------------------------------------------*/
    // debugging functions
    void print_rgb_matrix () { print_rgb_matrix (og_file_name + "_rgb"); }
    void print_rgb_matrix (const std::string& file_out_name)
    {
        std::filesystem::create_directory ("images-out/" + og_file_name);
        std::ofstream file_out ("images-out/" + og_file_name + "/" + file_out_name + ".ppm");

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
        std::filesystem::create_directory ("images-out/" + og_file_name);
        std::ofstream file_out ("images-out/" + og_file_name + "/" + file_out_name + ".ppm");

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
        std::filesystem::create_directory ("graphs-out/");
        std::ofstream file_out ("graphs-out/" + file_out_name + ".csv");

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
        std::filesystem::create_directory ("images-out/" + og_file_name);
        std::ofstream file_out ("images-out/" + og_file_name + "/" + file_out_name + ".ppm");

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
        std::filesystem::create_directory ("images-out/" + og_file_name);
        std::ofstream file_out ("images-out/" + og_file_name + "/" + file_out_name + ".txt");
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

    void print_debug_matrix (const std::string& file_name, int** arr, const int& i_start, const int& i_end, const int& j_start, const int& j_end)
    {
        int buffer = 4;
        std::ofstream file_out ("images-out/" + og_file_name + "/" + og_file_name + "_" + file_name + ".txt");
        for (int i = i_start; i < i_end; i++)
        {
            for (int j = j_start; j < j_end; j++)
            {
                file_out << std::right << std::setw(buffer) << arr[i][j] << ' ';
            }
            file_out << '\n';
        }
        file_out.close ();
    }

};
#endif