#ifndef Header_h
#define Header_h

#define PIXEL (__rgb){ rand() % 256, rand() % 256, rand() % 256 }

#include <iostream>
#include <fstream>

typedef unsigned char byte;

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

    void operator()() {
        std::cout << "signature     " << std::hex << signature    << " " << std::dec << signature    << std::endl;
        std::cout << "file_size     " << std::hex << file_size    << " " << std::dec << file_size    << std::endl;
        std::cout << "reserved      " << std::hex << reserved     << " " << std::dec << reserved     << std::endl;
        std::cout << "image_offset  " << std::hex << image_offset << " " << std::dec << image_offset << std::endl;
        std::cout << "header_size   " << std::hex << header_size  << " " << std::dec << header_size  << std::endl;
        std::cout << "width         " << std::hex << width        << " " << std::dec << width        << std::endl;
        std::cout << "height        " << std::hex << height       << " " << std::dec << height       << std::endl;
        std::cout << "unit          " << std::hex << unit         << " " << std::dec << unit         << std::endl;
        std::cout << "color_depth   " << std::hex << color_depth  << " " << std::dec << color_depth  << std::endl;
        std::cout << "compression   " << std::hex << compression  << " " << std::dec << compression  << std::endl;
        std::cout << "image_size    " << std::hex << image_size   << " " << std::dec << image_size   << std::endl;
        std::cout << "gres          " << std::hex << gres         << " " << std::dec << gres         << std::endl;
        std::cout << "vres          " << std::hex << vres         << " " << std::dec << vres         << std::endl;
        std::cout << "mus           " << std::hex << mus          << " " << std::dec << mus          << std::endl;
    }
};

static char mass[3] = { 0 };

struct COORD {
    unsigned long X;
    unsigned long Y;
};

struct __rgb {
    byte b;
    byte g;
    byte r;
};

#pragma pack(pop)

static const char *help_info = "Выбор основного действия:\n"
                               "-s, --square\tрисование квадрата\n"
                               "-r, --rotation\tвращение части изображения\n"
                               "-c, --chrgb\tизменение компонента цвета всего изображения\n\n"
                               "Параментры функций:\n"
                               "-S, --side\tдлина стороны квадрата\n"
                               "-w, --width\tтолщина линии\n"
                               "-C, --color\tустанавливает цвет кисти. Пример: [ff00ff]\n"
                               "-p, --pour\tустанавливает заливку квадрата\n"
                               "-P, --pour-color\tцвет заливки\n"
                               "-b, --begin\tначальные координаты области\n"
                               "-e, --end\tконечные координаты области\n"
                               "-x\tкоордината по горизонтали\n"
                               "-y\tкоордината по вертикали\n"
                               "-v, --value\tзначение компоненты цвета\n"
                               "-a, --angle\tугол поворота изображения\n";

class BMP_File {
    friend int isValidCoord(BMP_File *file, COORD lc, COORD rc);
    friend int isValidCoord(BMP_File *file, COORD crd);
public:
    explicit BMP_File(const char *_file_name = nullptr);
    ~BMP_File();

    File_Info getFileInfo() const { return file_info; }

    void create(const char *_file_name);
    void open(const char *_file_name);
    void drawRect(COORD coord, int side, int width, __rgb color, bool pour, __rgb pour_color);
    void ImageRotation(COORD lcoord, COORD rcoord, int angle);
    void writeImage();
    void RGBChange(int offset, byte value);
private:
    mutable __rgb **file_buffer;
    File_Info       file_info;

    char           *file_name;

    void readImage(std::ifstream &fin);
};

#endif
