#include <iostream>
#include <fstream>

using namespace std;

int main ()
{
    ofstream test_image ("test_img.ppm");

    int x = 10;
    int y = x;

    test_image << "P3" << '\n';
    test_image << x << ' ' << y << '\n';
    test_image << x << '\n';
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            test_image << (i * j) % x << ' ' << (i * j) % x << ' ' << (i * j) % x << ' ';
        }
        test_image << '\n';
    }

    test_image.close();

    return 0;
}