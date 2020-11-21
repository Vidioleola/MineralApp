/*
    A tiny image library.
    Can read and write png and jpg files.
    Can scale an image using the simplest algorithm.
    That's it.

    Simone Conti 2020
*/


#include "image.h"

namespace dz2 {


/* Default constructor and destructor */
Image::Image(std::string fname) {
    bitmap = NULL;
    _width = 0;
    _height = 0;
    read(fname);
    jpeg_quality = 95;
}
Image::~Image() {
    free(bitmap);
    bitmap = NULL;
    _width = 0;
    _height = 0;
}

/* Simple set/get functions */
int  Image::width() { return _width; }
int  Image::height() { return _height; }
std::string Image::error() { return errmsg; }
void Image::set_jpeg_quality(int q) {
    if (q>100) q=100;
    if (q<0) q=0;
    jpeg_quality=q;
}

/* Read image from file */
void Image::read(std::string fname) {
    std::string ext;
    size_t pos = fname.rfind(".");
    if (pos!=0 && pos!=std::string::npos) ext=fname.substr(pos+1);
    std::FILE *fp = std::fopen(fname.c_str(), "rb");
    if (!fp) {
        errmsg += "Could not open file for reading";
        return;
    }
    read(fp, ext);
    fclose(fp);
}

/* Read image from already opened *fp */
void Image::read(std::FILE *fp, std::string ext) {
    if (ext=="png" || ext=="PNG") return read_png(fp);
    if (ext=="jpg" || ext=="JPG" || ext=="jpeg" || ext=="JPEG") return read_jpg(fp);
    errmsg += std::string("Error! Unknown extension ") + ext;
}

/* Write image to file */
void Image::write(std::string fname) {
    std::string ext;
    size_t pos = fname.rfind(".");
    if (pos!=0 && pos!=std::string::npos) ext=fname.substr(pos+1);
    std::FILE *fp = std::fopen(fname.c_str(), "wb");
    if (!fp) {
        errmsg += "Could not open file for writing";
        return;
    }
    write(fp, ext);
    fclose(fp);
}

/* Write image to already opened *fp */
void Image::write(std::FILE *fp, std::string ext) {
    if (ext=="png" || ext=="PNG") return write_png(fp);
    if (ext=="jpg" || ext=="JPG" || ext=="jpeg" || ext=="JPEG") return write_jpg(fp);
    errmsg += std::string("Error! Unknown extension ") + ext;
}

/* Write image into a string buffer */
std::string Image::write_to_string(std::string format) {
    std::FILE* tmpf = std::tmpfile();
    write(tmpf, format);
    size_t size = std::ftell(tmpf);
    std::rewind(tmpf);
    char *memblock = (char*)malloc(size*sizeof(char));
    std::fread(memblock, sizeof(char), size, tmpf);
    std::fclose(tmpf);
    std::string out = std::string(memblock, size);
    free(memblock);
    return out;
}

/* Read PNG image */
void Image::read_png(std::FILE *fp) {
    unsigned char header[8];    /* Header to check if a real PNG */
    int           x,y;          /* Coordinates of the current pixel */
    int           ct;           /* Color type */
    int           bd;           /* Bit depth */
    png_structp   png_ptr;      /* PNG struct */
    png_infop     png_info;     /* PNG info */
    if (fread(header, 1, 8, fp) != 8) {
        errmsg += "Error reading first 8 bits";
        return;
    }
    if (png_sig_cmp(header, 0, 8)) {
        errmsg += "File is not recognized as a PNG file";
        return;
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        errmsg += "png_create_read_struct failed";
        return;
    }
    png_info = png_create_info_struct(png_ptr);
    if (png_info == NULL) {
        png_destroy_read_struct(&png_ptr, &png_info, NULL);
        errmsg += "png_create_info_struct failed";
        return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
       png_destroy_read_struct(&png_ptr, &png_info, NULL);
       return ;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, png_info);
    _width  = png_get_image_width  (png_ptr, png_info);
    _height = png_get_image_height (png_ptr, png_info);
    ct      = png_get_color_type   (png_ptr, png_info);
    bd      = png_get_bit_depth    (png_ptr, png_info);
    if (bd == 16) png_set_strip_16(png_ptr);
    if (ct == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (ct == PNG_COLOR_TYPE_GRAY && bd < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, png_info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    if (ct == PNG_COLOR_TYPE_RGB || ct == PNG_COLOR_TYPE_GRAY || ct == PNG_COLOR_TYPE_PALETTE) png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    if (ct == PNG_COLOR_TYPE_GRAY || ct == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);
    png_read_update_info(png_ptr, png_info);
    png_bytep *rows = (png_bytep*) malloc(sizeof(png_bytep) * _height);
    for (y=0; y<_height; y++) {
        rows[y] = (png_byte*) malloc(_width * 4 * sizeof(png_byte));
    }
    png_read_image(png_ptr, rows);
    bitmap = (uint8_t*) malloc(_height * _width * 4 * sizeof(uint8_t));
    for (y=0; y<_height; y++) {
        for (x=0; x<_width; x++) {
            bitmap[_width*y*4+x*4+0] = rows[y][x*4+0];
            bitmap[_width*y*4+x*4+1] = rows[y][x*4+1];
            bitmap[_width*y*4+x*4+2] = rows[y][x*4+2];
            bitmap[_width*y*4+x*4+3] = rows[y][x*4+3];
        }
    }
    for (y=0; y<_height; y++) {
        free(rows[y]);
    }
    free(rows);
    return;
}

/* Write PNG image */
void Image::write_png(std::FILE *fp) {
    png_structp png_ptr;
    png_infop   info_ptr;
    int x, y, png_type, bpp, b;
    png_type = PNG_COLOR_TYPE_RGB_ALPHA;
    bpp = 4;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        errmsg += "png_create_write_struct failed";
        return;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        errmsg += "png_create_info_struct failed";
        return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        errmsg += "Error during init_io";
        return;
    }
    png_init_io(png_ptr, fp);
    if (setjmp(png_jmpbuf(png_ptr))) {
        errmsg += "Error during writing header";
        return;
    }
    png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, png_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_set_compression_level(png_ptr, 4);
    png_write_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr))) {
        errmsg += "Error during writing bytes";
        return;
    }
    png_bytep *rows = (png_bytep*) malloc(sizeof(png_bytep) * _height);
    for (y=0; y<_height; y++) {
        rows[y] = (png_byte*) malloc(_width * bpp * sizeof(png_byte) );
    }
    for (y=0; y<_height; y++) {
        for (x=0; x<_width; x++) {
            for (b=0; b<bpp; b++) {
                rows[y][x*bpp+b] = bitmap[_width*y*bpp+x*bpp+b];
            }
        }
    }
    png_write_image(png_ptr, rows);
    if (setjmp(png_jmpbuf(png_ptr))) {
        errmsg += "Error during end of write";
        return;
    }
    png_write_end(png_ptr, NULL);
    for (y=0; y<_height; y++) {
        free(rows[y]);
    }
    free(rows);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1); 
}

/* Read JPEG image */
void Image::read_jpg(std::FILE *fp) {
    int x, y;
    struct jpeg_decompress_struct cinfo;    /* Main JPEG info struct */
    struct jpeg_error_mgr jerr;             /* Error struct */
    JSAMPROW row[1];                        /* Output row buffer */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    _width  = cinfo.output_width;
    _height = cinfo.output_height;
    bitmap = (uint8_t*) malloc(sizeof( uint8_t) * _width * _height * 4);
    if (!bitmap){
        errmsg += "No memory for jpeg converter";
        return;
    }
    row[0] = (JSAMPLE*) malloc(_width * cinfo.output_components * sizeof(JSAMPLE));
    for (y=0; y<_height; y++) {
        jpeg_read_scanlines(&cinfo, row, 1);
        for (x=0; x<_width; x++) {
            if (cinfo.output_components==3 ) {
                bitmap[_width*y*4+x*4+0] = row[0][3*x+0];
                bitmap[_width*y*4+x*4+1] = row[0][3*x+1];
                bitmap[_width*y*4+x*4+2] = row[0][3*x+2];
                bitmap[_width*y*4+x*4+3] = 255;
            } else if (cinfo.output_components==1) {
                bitmap[_width*y*4+x*4+0] = row[0][x];
                bitmap[_width*y*4+x*4+1] = row[0][x];
                bitmap[_width*y*4+x*4+2] = row[0][x];
                bitmap[_width*y*4+x*4+3] = 255;
            }
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row[0]);
}

/* Write JPEG image */
void Image::write_jpg(std::FILE *fp) {
    int x, y;
    struct jpeg_compress_struct cinfo;  /* JPEG struct */
    struct jpeg_error_mgr jerr;         /* JPEG error struct */
    JSAMPROW row[1];                    /* Pointer to a single row */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = _width;         /* Image width */
    cinfo.image_height = _height;        /* Image height */
    cinfo.input_components = 3;         /* Number of color components per pixel: 3 if RGB, 1 if grayscale */
    cinfo.in_color_space = JCS_RGB;     /* Colorspace of input image: JCS_RGB or JCS_GRAYSCALE */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, jpeg_quality, TRUE);
    cinfo.dct_method = JDCT_ISLOW;
    cinfo.optimize_coding = TRUE;
    jpeg_start_compress(&cinfo, TRUE);
    row[0] = (JSAMPLE*) malloc(_width * cinfo.input_components * sizeof(JSAMPLE));
    if (cinfo.input_components==3 ) {
        for (y=0; y<_height; y++) {
            for (x=0; x<_width; x++) {
                row[0][3*x+0] = bitmap[_width*y*4+x*4+0]; 
                row[0][3*x+1] = bitmap[_width*y*4+x*4+1]; 
                row[0][3*x+2] = bitmap[_width*y*4+x*4+2]; 
            }
            jpeg_write_scanlines(&cinfo, row, 1);
        }
    } else if (cinfo.input_components==1) {
        for (y=0; y<_height; y++) {
            for (x=0; x<_width; x++) {
                row[0][x] = bitmap[_width*y*4+x*4+0];
            }
            jpeg_write_scanlines(&cinfo, row, 1);
        }
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    free(row[0]);
}

/*
    Rescale an image. Only implemented a downscale.
    max_size is the desired maximum dimension of the final image.
    Very simple algorithm: just pick the closest pixel from the original image.
*/
void Image::resize(int max_size) {
    float scale = (float)max_size / std::max(_width, _height);
    int new_width  = _width*scale;
    int new_height = _height*scale;
    int px, py;
    uint8_t *new_bitmap = (uint8_t*) malloc(new_height * new_width * 4 * sizeof(uint8_t));
    if (!new_bitmap) {
        errmsg += "Failed to allocate memory for rescaling";
        return;
    }
    for (int y=0; y<new_height; y++) {
        for (int x=0; x<new_width; x++) {
            px = (int)((float)x/scale);
            py = (int)((float)y/scale);
            new_bitmap[new_width*y*4+x*4+0] = bitmap[_width*py*4+px*4+0];
            new_bitmap[new_width*y*4+x*4+1] = bitmap[_width*py*4+px*4+1];
            new_bitmap[new_width*y*4+x*4+2] = bitmap[_width*py*4+px*4+2];
            new_bitmap[new_width*y*4+x*4+3] = bitmap[_width*py*4+px*4+3];
        }
    }
    free(bitmap);
    _width  = new_width;
    _height = new_height;
    bitmap  = new_bitmap;
}

} // Namespace

