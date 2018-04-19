#ifndef Header_h
#define Header_h

#define PIXEL (__rgb){ rand() % 256, rand() % 256, rand() % 256 }

#include <iostream>
#include <fstream>

typedef char byte;

#pragma pack(push, 1)

struct File_Info {
    unsigned short signature;
    unsigned int   file_size;
    unsigned int   reserved;
    unsigned int   image_offset;

    unsigned int   header_size;
    unsigned int   width;
    unsigned int   height;
    unsigned short unit;
    unsigned short color_depth;
    unsigned int   compression;
    unsigned int   image_size;
    unsigned int   gres;
    unsigned int   vres;
    unsigned long  mus;
};

static char mass[3] = { 0 };

struct COORD {
    unsigned long X;
    unsigned long Y;
};

struct __rgb {
    byte blue;
    byte green;
    byte red;
};

#pragma pack(pop)

class BMP_File {
    friend int isValidCoord(BMP_File *file, COORD lc, COORD rc);
    friend int isValidCoord(BMP_File *file, COORD crd);
public:
    explicit BMP_File(const char *_file_name = NULL);
	~BMP_File();
    void operator()() {
        std::cout << "signature     " << std::hex << file_info.signature    << " " << std::dec << file_info.signature    << std::endl;
        std::cout << "file_size     " << std::hex << file_info.file_size    << " " << std::dec << file_info.file_size    << std::endl;
        std::cout << "reserved      " << std::hex << file_info.reserved     << " " << std::dec << file_info.reserved     << std::endl;
        std::cout << "image_offset  " << std::hex << file_info.image_offset << " " << std::dec << file_info.image_offset << std::endl;
        std::cout << "header_size   " << std::hex << file_info.header_size  << " " << std::dec << file_info.header_size  << std::endl;
        std::cout << "width         " << std::hex << file_info.width        << " " << std::dec << file_info.width        << std::endl;
        std::cout << "height        " << std::hex << file_info.height       << " " << std::dec << file_info.height       << std::endl;
        std::cout << "unit          " << std::hex << file_info.unit         << " " << std::dec << file_info.unit         << std::endl;
        std::cout << "color_depth   " << std::hex << file_info.color_depth  << " " << std::dec << file_info.color_depth  << std::endl;
        std::cout << "compression   " << std::hex << file_info.compression  << " " << std::dec << file_info.compression  << std::endl;
        std::cout << "image_size    " << std::hex << file_info.image_size   << " " << std::dec << file_info.image_size   << std::endl;
        std::cout << "gres          " << std::hex << file_info.gres         << " " << std::dec << file_info.gres         << std::endl;
        std::cout << "vres          " << std::hex << file_info.vres         << " " << std::dec << file_info.vres         << std::endl;
        std::cout << "mus           " << std::hex << file_info.mus          << " " << std::dec << file_info.mus          << std::endl;
    }

    File_Info getFileInfo() const { return file_info; }

	void create(const char *_file_name);
	void open(const char *_file_name);
    void drawRect(COORD coord, int side, int width, __rgb color, bool pour, __rgb pour_color);
	void ImageRotation(COORD lcoord, COORD rcoord, int angle);
	void writeImage();
    void RGBChange(int offset, char value);
private:
    mutable __rgb **file_buffer;
    File_Info       file_info;

    char           *file_name;

	void readImage(std::ifstream &fin);
};

#endif
