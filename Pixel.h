#pragma once
#ifndef PIXEL_H
#define PIXEL_H

#include <string>

struct Pixel
{
    int red, green, blue, gray;
    bool black;
    Pixel ()
    {
        red = green = blue = gray = black = 0;
    }
    Pixel (const int& gray)
    {
        red = green = blue = black = 0;
        this->gray = gray;
    }
    Pixel (const int& red, const int& green, const int& blue)
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
        gray = black = 0;
    }
    std::string print_rgb ()
    { 
        return std::to_string (red) + ' ' + std::to_string (green) + ' ' + std::to_string (blue);
    }
    std::string print_gs ()
    { 
        return std::to_string (gray);
    }
    std::string print_bi ()
    { 
        return std::to_string (black);
    }
};
#endif