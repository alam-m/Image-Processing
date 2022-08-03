#pragma once
#ifndef PIXEL_H
#define PIXEL_H

#include <string>

struct Pixel
{
    int red, green, blue, gray;
    Pixel ()
    {
        red = green = blue = gray = 0;
    }
    Pixel (const int & gray)
    {
        red = green = blue = 0;
        this->gray = gray;
    }
    Pixel (const int & red, const int & green, const int & blue)
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
        gray = 0;
    }
    std::string print_rgb ()
    { 
        return std::to_string(red) + ' ' + std::to_string(green) + ' ' + std::to_string(blue);
    }
    std::string print_gs ()
    { 
        return std::to_string(gray);
    }
    friend std::ostream& operator<<(std::ostream& os, const Pixel& pixel);
};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel)
{
    os << pixel.red << ' ' << pixel.green << ' ' << pixel.blue << ' ' << pixel.gray;
    return os;
}
#endif