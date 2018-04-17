#define GET_YBORDER(crd) ((crd < 0) ? 0 : ((crd > file_info.height) ? file_info.height : crd))
#define GET_XBORDER(crd) ((crd < 0) ? 0 : ((crd > file_info.width) ? file_info.width : crd))

#include <string.h>
#include <stdlib.h>

#include "Header.h"

BMP_File::BMP_File(const char *_file_name) {
    file_buffer = NULL;
	memset(&file_info, 0, sizeof(File_Info));
	file_name = (char *)_file_name;

	if (_file_name != NULL) {
        if (std::ifstream(_file_name, std::ios::in).is_open())
			open(_file_name);
		else
			create(_file_name);
	}
}

BMP_File::~BMP_File() {
    for (int i = 0; i < file_info.height; i++)
            delete file_buffer[i];

    delete file_buffer;
}

void BMP_File::drawRect(COORD coord, int side, int width, Color color, bool pour, Color pour_color) {
    if (side < 0 || width < 0 || side - 2 < width * 2)
        return;

	for (int i = width; i >= -width; i--) {
		for (int j = -i; j < side + i; j++) {
            file_buffer[GET_YBORDER(coord.Y - i)][GET_XBORDER(coord.X + j)] =
                file_buffer[GET_YBORDER(coord.Y + i + side - 1)][GET_XBORDER(coord.X + side - j - 1)] =
                file_buffer[GET_YBORDER(coord.Y + j)][GET_XBORDER(coord.X - i)] =
                file_buffer[GET_YBORDER(coord.Y + j)][GET_XBORDER(coord.X + side + i - 1)] = color;
		}
	}

	if (pour) {
		for (int i = width + 1; i < side - width - 1; i++)
			for (int j = width + 1; j < side - width - 1; j++)
                file_buffer[GET_YBORDER(coord.Y + i)][GET_XBORDER(coord.X + j)] = pour_color;
	}

	if (side >= 2 * width + 2) { 
	for (int i = 0; i <= width; i++) {
			for (int j = width + 1; j < side - width - i - 1; j++) {
				file_buffer[coord.Y + j][coord.X + j + i] =
					file_buffer[coord.Y + j + i][coord.X + j] = color;
				file_buffer[coord.Y + side - j - 1][coord.X + j + i] =
					file_buffer[coord.Y + side - j - i - 1][coord.X + j] = color;
			}
		}
	}
}

void BMP_File::ImageRotation(COORD lcoord, COORD rcoord, int angle) {
	int l = rcoord.X - lcoord.X, h = rcoord.Y - lcoord.Y;

	COORD new_lcoord = { lcoord.X + l / 2 - h / 2, lcoord.Y + h / 2 - l / 2 };
	COORD new_rcoord = { new_lcoord.X + h, new_lcoord.Y + l };

	Color **buffer = new Color *[h];

	for (int i = 0; i < h; i++)
		buffer[i] = new Color[l];

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < l; j++) {
			buffer[i][j] = file_buffer[lcoord.Y + i][lcoord.X + j];
			file_buffer[lcoord.Y + i][lcoord.X + j] = Color{ rand() % 256, rand() % 256, rand() % 256 };
		}
	}

	switch (angle) {
	case 90:
		for (int i = 0; i < new_rcoord.Y - new_lcoord.Y; i++)
            for (int j = 0; j < new_rcoord.X - new_lcoord.X; j++)
                file_buffer[GET_YBORDER(new_lcoord.Y + i)][GET_XBORDER(new_lcoord.X + j)] = buffer[h - j - 1][i];

		break;
	case 180:
		for (int i = 0; i < rcoord.Y - lcoord.Y; i++)
            for (int j = 0; j < rcoord.X - lcoord.X; j++)
                file_buffer[GET_YBORDER(lcoord.Y + i)][GET_XBORDER(lcoord.X + j)] = buffer[h - i - 1][l - j - 1];

		break;
	case 270:
		for (int i = 0; i < new_rcoord.Y - new_lcoord.Y; i++)
            for (int j = 0; j < new_rcoord.X - new_lcoord.X; j++)
                file_buffer[GET_YBORDER(new_lcoord.Y + i)][GET_XBORDER(new_lcoord.X + j)] = buffer[j][l - i - 1];

		break;
	}

}

void BMP_File::writeImage() {
	std::ofstream fout(file_name, std::ios::binary);

	fout.write((char *)&file_info, sizeof(File_Info));

	fout.seekp(file_info.image_bias, fout.beg);

	for (int i = file_info.height - 1; i >= 0; i--) {
		for (int j = 0; j < file_info.width; j++)
			fout.write((char *)&file_buffer[i][j], sizeof(file_buffer[i][j]));

		fout.write((char *)&mass, file_info.width % 4);
	}

	fout.close();
}

void BMP_File::readImage(std::ifstream &fin) {
	fin.seekg(file_info.image_bias, fin.beg);

	for (int i = file_info.height - 1; i >= 0; i--) {
		for (int j = 0; j < file_info.width; j++) {
			fin.read((char *)&file_buffer[i][j], sizeof(file_buffer[i][j]));
		}

		fin.seekg((file_info.width) % 4, fin.cur);
	}
}

void BMP_File::RGBChange(int offset, char value) {
	for (int i = 0; i < file_info.height; i++)
		for (int j = 0; j < file_info.width; j++)
			*((char *)&file_buffer[i][j] + offset) = value;
}

void BMP_File::open(const char *_file_name) {
	std::cout << "File exists\n";

	std::ifstream fin(_file_name, std::ios::binary);

	fin.read((char *)&file_info, sizeof(File_Info));

	file_buffer = new Color *[file_info.height];

	for (int i = 0; i < file_info.height; i++)
		file_buffer[i] = new Color[file_info.width];

	readImage(fin);

	fin.close();
}

void BMP_File::create(const char *_file_name) {
	std::cout << "New file created\n";

	std::ofstream fout(_file_name, std::ios::binary);

	*(char *)&file_info.BM = 'B';
	*((char *)&file_info.BM + 1) = 'M';
	file_info.size = 14 + 40 + 1000 * 1000 * 3;
	file_info.reserved = 0;
	file_info.image_bias = 14 * 40;
	file_info.caption = 40;
	file_info.width = file_info.height = 1000;
	file_info.one = 1;
	file_info.color_depth = 24;
	file_info.compression = 0;
	file_info.image_size = 1000 * 1000;
	file_info.gres = 2795;
	file_info.vres = 2795;
	file_info.mus = 0;

	fout.write((char *)&file_info, sizeof(File_Info));

	file_buffer = new Color *[file_info.height];

	for (int i = 0; i < file_info.height; i++)
		file_buffer[i] = new Color[file_info.width];

	fout.close();
}
