/*
About:
    This is the header file for the Image class which defines the interface for working with image files. The Image class is used to manipulate and perform 
    operations on image data. Currently the only image data that is supported is JPG and PNG.It utilizes the stb_image.h and stb_image_write.h libraries 
    to read and write image files respectively. The class provides a constructor for loading image data from a file or creating an image with specified 
    width, height and number of channels. It also provides methods for reading and writing image data to files. Other operations include grayscale 
    conversion, coloring an image mask with specified RGB values, and image to ascii conversion.

Warning:
    Please note I am by no means a professional and there likely bugs/mistakes in my code. Use at your own risk.

Example Usuage:
    Image img("test.jpg");
    img.grayscale_avg().write("test_gray.png");
    img.color(1.0, 0.0, 0.0).write("test_red.png");


    Created by Dax McPartland, 3/6/2023
*/
#include <cstdio>
#include <stdint.h>
#include <vector>
#include <string>
#ifndef IMAGE_H
#define IMAGE_H

enum ImageType{
    PNG, JPG
};

class Image{
private:
    uint8_t* data = NULL;  
    size_t size = 0;
    int width;
    int height;
    int channels;

    char mapChar(const std::string densityString, int average);
    ImageType getFileType(const char* filename);

public:
    Image(const char* filename);
    Image(int width, int height, int channels);
    Image(const Image& other);
    ~Image();
    
    bool read(const char* filename);
    bool write(const char* filename);

    Image& grayscale_avg();
    Image& grayscale_lum();

    Image& colorMask(float r, float g, float b);

    std::vector<std::string> convertToAscii(const std::string densityString);
};
#endif