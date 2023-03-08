/*
About:
    This file contains the implementation of the Image class. The Image class is used to manipulate and perform operations on image data.
    Currently the only image data that is supported is JPG and PNG.It utilizes the stb_image.h and stb_image_write.h libraries to read and write image 
    files respectively. The class provides a constructor for loading image data from a file or creating an image with specified width, height and number 
    of channels. It also provides methods for reading and writing image data to files. Other operations include grayscale conversion, coloring an image mask 
    with specified RGB values, and image to ascii conversion.

Warning:
    Please note I am by no means a professional and there likely bugs/mistakes in my code. Use at your own risk.

Example Usuage:
    Image img("test.jpg");
    img.grayscale_avg().write("test_gray.png");
    img.color(1.0, 0.0, 0.0).write("test_red.png");


    Created by Dax McPartland, 3/6/2023
*/
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

    /// @brief Constructor for Image object, takes a string for a filename. This will either print out "Read (filename)" if it worked or "Failed to read 
    ///  (filename)" if it could not read the file.
    /// @param filename - a string to the PNG or JPG
    Image::Image(const char* filename){
        if(read(filename)){
            printf("Read %s\n", filename);
            size = width*height*channels;
        }
        else
            printf("Failed to read %s\n", filename);
    }

    /// @brief Constructor for Image object, takes the width of the image, the height, and the amount of color channels. For a standard image which is RGB 
    /// the channel is 3. 
    /// @param width - width of the image
    /// @param height - height of the image
    /// @param channels - amount of color channels in the image
    Image::Image(int width, int height, int channels) : width(width), height(height), channels(channels){
        size = width * height * channels;
        data = new uint8_t[size];
    }

    /// @brief Copy constructor for Images
    /// @param other - another image object
    Image::Image(const Image& other) : Image(other.width, other.height, other.channels){
        memcpy(data, other.data, size);
    }

    /// @brief Destructor for the image object. Free's the image data
    Image::~Image(){
        stbi_image_free(data); 
    }
    
    /// @brief This method is used to read a given file and load the data for this image object.
    /// @param filename 
    /// @return true if it could read the file, false otherwise
    bool Image::read(const char* filename){
        data = stbi_load(filename, &width, &height, &channels, 0);

        if(data == NULL)
            return false;
        return true;
    }

    /// @brief This method is used to make an image using the current image data values. If the given file is supposed to be a png this will create a png,
    /// otherwise it will creat a jpg.
    /// @param filename 
    /// @return returns true if it succeed in creating the file, false otherwise.
    bool Image::write(const char* filename){
        ImageType type = getFileType(filename);
        int success;

        if(type == PNG){
            success = stbi_write_png(filename, width, height, channels, data, width*channels);
        }else{
            success = stbi_write_jpg(filename, width, height, channels, data, 100);
        }

        if(success == 0)
            return false;
        return true;
    }

    /// @brief Used to get the type of the file. Does this by looking at the extenstion after the '.'. If the extension is not found it will return a png by
    /// default. JPG can also work. 
    /// @param filename 
    /// @return 
    ImageType Image::getFileType(const char* filename){
        const char* extentsion = strrchr(filename, '.');

        if(extentsion != nullptr){
            if(strcmp(extentsion, "JPG"))
                return JPG;
        }
        return PNG;
    }

    /// @brief Used to grayscale an image. This method uses the average of the RGB values((r+g+b)/3) to generate a gray value and sets that pixel to that gray color.
    /// @return Returns this image
    Image& Image::grayscale_avg(){
        if(channels < 3){
            printf("Image %p has less than 3 channels, grayscaling will not work.", this);
        }
        else{
            for(unsigned int i = 0; i < size; i+=channels){
                // (r+g+b)/3
                int gray = (data[i] + data[i+1] + data[i+2])/3;
                memset(data+i, gray, 3);
            }
        }
        return *this;
    }

    /// @brief Used to grayscale an image. This method uses the weighted average of the RGB values to generate a gray value and sets that pixel to that gray 
    /// color. The equation for weighted average is on google.
    /// @return Returns this image
    Image& Image::grayscale_lum(){
         if(channels < 3){
            printf("Image %d has less than 3 channels, grayscaling will not work.", this->channels);
        }
        else{
            for(unsigned int i = 0; i < size; i+=channels){
                // Using linear equation for weighted average of grayscale
                int gray = 0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2];
                memset(data+i, gray, 3);
            }
        }
        return *this;
    }

    /// @brief Used to color the image in a mask using the given rgb floats. For example if given r = 1, g = 0, b = 0, then the image will be masked in red. 
    /// It does this by going thru the pixels and multiplying each rgb value by the multipliers provided in the arugments.
    /// @param r - a float value for red
    /// @param g - a float value for green
    /// @param b - a float value for blue
    /// @return Returns this imgae
    Image& Image::colorMask(float r, float g, float b){
        if(channels < 3){
            printf("Image %d has less than 3 channels, color masking will not work.", this->channels);
        }
        else{
            for(unsigned int i = 0; i < size; i+=channels){
                data[i] = r * data[i];
                data[i+1] = g * data[i+1];
                data[i+2] = b * data[i+2];
            }
        }
        return *this;
    }

    /// @brief Used to convert image into ascii. It does this by first converting the image into grayscale and using the average of the rgb pixels to determine
    /// an ascii character from the density string. The vector that this method returns contains strings, each string represents a row of the image. So, if
    /// the image is 20x20 there should be 20 strings in the resulting vector.
    /// @param densityString - a string containing ascii characters used to determine what the image looks like
    /// @return a vector that contains strings for each row in the image
    std::vector<std::string> Image::convertToAscii(const std::string densityString){
        std::vector<std::string> result;
        if(channels < 3){
                printf("Image %d has less than 3 channels, converting to ascii will not work.", this->channels); 
                return result;
        }

        std::string row;
        for(unsigned int i = 0; i < size; i+=channels){
                if((i/channels) % width == 0 && i != 0){
                    result.push_back(row); 
                    row.clear();
                }
                int average = (data[i] + data[i+1] + data[i+2])/3;
                char ascii = mapChar(densityString, average);
                row.push_back(ascii);
        }

        result.push_back(row); 
        return result;
    }

    /// @brief Used to map an average to an ascii character in the density string provided. 
    /// @param densityString  - a string containing ascii characters used to determine what the image looks like
    /// @param average - an integer that represents the average of the rgb values
    /// @return the ascii character that should result from the given average
    char Image::mapChar(const std::string densityString, int average){
        int charValue = int(round(densityString.length() / 255.0 * average));
        charValue = std::max(charValue, 0);
        charValue = std::min(charValue, (int)(densityString.length()) - 1);
        
        return densityString.at(charValue);
    }

    
