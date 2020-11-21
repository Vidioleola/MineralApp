/*
    A tiny image library.
    Can read and write png and jpg files.
    Can scale an image using the simplest algorithm.
    That's it.

    Simone Conti 2020
*/


#ifndef dz2_image
#define dz2_image

#include <string>
#include <cstdio>
#include <algorithm>
#include <png.h> 
#include <jpeglib.h> 

namespace dz2 {

class Image {
    private:
        int _width;
        int _height;
        uint8_t *bitmap;
        std::string errmsg;
        int jpeg_quality;
    public:
        Image(std::string);
        ~Image();
        void read(std::string filename);
        void read(std::FILE *fp, std::string format);
        void write(std::string filename);
        void write(std::FILE *fp, std::string format);
        std::string write_to_string(std::string format);
        int  width();
        int  height();
        void set_jpeg_quality(int quality);
        void resize(int max_size);
        std::string error();
    private:
        void read_png(std::FILE *fp);
        void read_jpg(std::FILE *fp);
        void write_png(std::FILE *fp);
        void write_jpg(std::FILE *fp);
};

} // namespace dz2
#endif

