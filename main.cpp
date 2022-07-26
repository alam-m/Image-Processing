#include <iostream>
#include <fstream>

using namespace std;

int main ()
{
    ofstream test_image ("test_img.ppm");

    int img_size = 10;
    int x = img_size;
    int y = img_size;

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