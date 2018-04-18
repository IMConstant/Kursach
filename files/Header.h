#ifndef Header_h
#define Header_h

#include <iostream>
#include <fstream>

#pragma pack(push, 1)

struct File_Info {
	short int BM;
	int size;
	int reserved;
	int image_bias;

	int caption;
	int width;
	int height;
	short int one;
	short int color_depth;
	int compression;
	int image_size;
	int gres;
	int vres;
	long int mus;
};

#pragma pack(pop)

static char mass[3] = { 0 };

struct COORD {
	long int X;
	long int Y;
};

#pragma pack(push, 1)

struct Color {
	char blue;
	char green;
	char red;
};

#pragma pack(pop)

class BMP_File {
    friend int isValidCoord(BMP_File *file, COORD lc, COORD rc);
    friend int isValidCoord(BMP_File *file, COORD crd);
public:
    explicit BMP_File(const char *_file_name = NULL);
	~BMP_File();

    File_Info getFileInfo() const { return file_info; }

	void create(const char *_file_name);
	void open(const char *_file_name);
	void drawRect(COORD coord, int side, int width, Color color, bool pour, Color pour_color);
	void ImageRotation(COORD lcoord, COORD rcoord, int angle);
	void writeImage();
    void RGBChange(int offset, char value);
private:
    mutable Color **file_buffer;
	File_Info file_info;

	char *file_name;

	void readImage(std::ifstream &fin);
};

#endif
