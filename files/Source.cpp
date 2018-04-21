#define GET_YBORDER(crd) (((crd) < 0) ? 0 : (((crd) >= file_info.height) ? file_info.height - 1 : (crd)))
#define GET_XBORDER(crd) (((crd) < 0) ? 0 : (((crd) >= file_info.width) ? file_info.width - 1 : (crd)))

#include <cstring>
#include <cstdlib>

#include "Header.h"

int isValidCoord(BMP_File *file, COORD lc, COORD rc) {
    if (lc.X >= rc.X || lc.Y >= rc.Y)
        return 0;

    if (lc.X < 0 || lc.Y < 0 || rc.X < 0 || rc.Y < 0 || rc.X > file->file_info.width ||
        rc.Y > file->file_info.height || lc.X > file->file_info.width ||
        lc.Y > file->file_info.height)
        return 0;
}

int isValidCoord(BMP_File *file, COORD crd) {
    if (crd.X < 0 || crd.Y < 0 || crd.X > file->file_info.width ||
        crd.Y > file->file_info.height)
        return 0;
}

BMP_File::BMP_File(const char *_file_name) {
    file_buffer = nullptr;
    memset(&file_info, 0, sizeof(File_Info));
    file_name = const_cast<char *>(_file_name);

    if (_file_name != nullptr) {
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

void BMP_File::drawRect(COORD coord, int side, int width, __rgb color, bool pour, __rgb pour_color) {
    if (side < 0 || width < 0 || side - 2 < width * 2 || !isValidCoord(this, coord)) {
        std::cout << "DrawRect error!\n Check input args!\n";
        return;
    }

    std::cout << 1 << std::endl;

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
                file_buffer[GET_YBORDER(coord.Y + j)][GET_XBORDER(coord.X + j + i)] =
                file_buffer[GET_YBORDER(coord.Y + j + i)][GET_XBORDER(coord.X + j)] = color;
                file_buffer[GET_YBORDER(coord.Y + side - j - 1)][GET_XBORDER(coord.X + j + i)] =
                file_buffer[GET_YBORDER(coord.Y + side - j - i - 1)][GET_XBORDER(coord.X + j)] = color;
            }
        }
    }
}

void BMP_File::ImageRotation(COORD lcoord, COORD rcoord, int angle) {
    if (!isValidCoord(this, lcoord, rcoord)) {
        std::cout << "ImageRotation error! Check input args!\n";
        return;
    }

    unsigned long l = rcoord.X - lcoord.X, h = rcoord.Y - lcoord.Y;

    COORD new_lcoord = { lcoord.X + l / 2 - h / 2, lcoord.Y + h / 2 - l / 2 };
    COORD new_rcoord = { new_lcoord.X + h, new_lcoord.Y + l };

    auto **buffer = new __rgb *[h];

    for (int i = 0; i < h; i++)
        buffer[i] = new __rgb[l];

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < l; j++) {
            buffer[i][j] = file_buffer[lcoord.Y + i][lcoord.X + j];
            file_buffer[lcoord.Y + i][lcoord.X + j] = PIXEL;
        }
    }

    switch (angle) {
        case 90:
            for (int i = 0; i < new_rcoord.Y - new_lcoord.Y; i++)
                for (int j = 0; j < new_rcoord.X - new_lcoord.X; j++)
                    file_buffer[GET_YBORDER(new_lcoord.Y + i)][GET_XBORDER(new_lcoord.X + j)] = buffer[j][l - i - 1];

            break;
        case 180:
            for (int i = 0; i < rcoord.Y - lcoord.Y; i++)
                for (int j = 0; j < rcoord.X - lcoord.X; j++)
                    file_buffer[GET_YBORDER(lcoord.Y + i)][GET_XBORDER(lcoord.X + j)] = buffer[h - i - 1][l - j - 1];

            break;
        case 270:
            for (int i = 0; i < new_rcoord.Y - new_lcoord.Y; i++)
                for (int j = 0; j < new_rcoord.X - new_lcoord.X; j++)
                    file_buffer[GET_YBORDER(new_lcoord.Y + i)][GET_XBORDER(new_lcoord.X + j)] = buffer[h - j - 1][i];

            break;
        default:
            break;
    }
}

void BMP_File::writeImage() {
    std::ofstream fout(file_name, std::ios::out | std::ios::binary);

    fout.write(reinterpret_cast<const char *>(&file_info), sizeof(File_Info));

    fout.seekp(file_info.image_offset, fout.beg);

    for (int i = file_info.height - 1; i >= 0; i--) {
        for (int j = 0; j < file_info.width; j++)
            fout.write(reinterpret_cast<const char *>(&file_buffer[i][j]), sizeof(file_buffer[i][j]));

        fout.write(reinterpret_cast<const char *>(&mass), file_info.width % 4);
    }

    fout.close();
}

void BMP_File::readImage(std::ifstream &fin) {
    fin.seekg(file_info.image_offset, fin.beg);

    for (int i = file_info.height - 1; i >= 0; i--) {
        for (int j = 0; j < file_info.width; j++) {
            fin.read(reinterpret_cast<char *>(&file_buffer[i][j]), sizeof(file_buffer[i][j]));
        }

        fin.seekg((file_info.width) % 4, fin.cur);
    }
}

void BMP_File::RGBChange(int offset, byte value) {
    for (int i = 0; i < file_info.height; i++)
        for (int j = 0; j < file_info.width; j++)
            *((byte *)&file_buffer[i][j] + offset) = value;
}

void BMP_File::open(const char *_file_name) {
    std::cout << "File exists\n";

    std::ifstream fin(_file_name, std::ios::in | std::ios::binary);

    fin.read(reinterpret_cast<char *>(&file_info), sizeof(File_Info));

    file_buffer = new __rgb *[file_info.height];

    for (int i = 0; i < file_info.height; i++)
        file_buffer[i] = new __rgb[file_info.width];

    readImage(fin);

    fin.close();
}

void BMP_File::create(const char *_file_name) {
    std::cout << "New file created\n";

    std::ofstream fout(_file_name, std::ios::out | std::ios::binary);

    *(byte *)&file_info.signature = 'B';
    *((byte *)&file_info.signature + 1) = 'M';
    file_info.file_size = 14 + 40 + 1000 * 1000 * 3;
    file_info.reserved = 0;
    file_info.image_offset = 14 * 40;
    file_info.header_size = 40;
    file_info.width = file_info.height = 1000;
    file_info.unit = 1;
    file_info.color_depth = 24;
    file_info.compression = 0;
    file_info.image_size = 1000 * 1000;
    file_info.gres = 2795;
    file_info.vres = 2795;
    file_info.mus = 0;

    fout.write(reinterpret_cast<const char *>(&file_info), sizeof(File_Info));

    file_buffer = new __rgb *[file_info.height];

    for (int i = 0; i < file_info.height; i++)
        file_buffer[i] = new __rgb[file_info.width];

    fout.close();
}
