#define DRAW_FILE_SQUARE(file, init_square) file.drawRect(init_square.lcoord, \
                                                init_square.side, init_square.width, \
                                                init_square.color, init_square.pour, \
                                                init_square.pour_color);

#define CHANGE_FILE_RGB(file, init_chrgb) file.RGBChange(init_chrgb.offset, init_chrgb.value);

#define ROTATE_FILE_IMAGE(file, init_rotation) file.ImageRotation(init_rotation.lcoord, init_rotation.rcoord, \
                                                    init_rotation.angle);

#include <getopt.h>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <functional>
#include <cstdlib>

#include "Header.h"

struct __argflags {
    bool isActive() {
        return square_flag || chrgb_flag || rotation_flag;
    }

    bool square_flag   : 1;
    bool chrgb_flag    : 1;
    bool rotation_flag : 1;
    bool coordflags    : 1;
    bool help_flag     : 1;
} argflags;

struct __init_chrgb {
    unsigned int offset;
    unsigned int value;
} init_chrgb;

struct __init_square {
    __rgb          color;
    __rgb          pour_color;
    COORD          lcoord;
    unsigned int   side;
    unsigned int   width;
    unsigned int   pour;
} init_square;

struct __init_rotation {
    COORD        lcoord;
    COORD        rcoord;
    unsigned int angle;
} init_rotation;

int ParseArgs(int _argc, char *_argv[], char *_file_name) {
    int res, index;
    unsigned int value, angle, side, width, offset, end = 0;
    char buffer[3] = { 0 };
    const char *_colors[] = { "blue", "red", "green" };

    argflags = { 0, 0, 0, 0, 1 };
    memset(&init_square, 0, sizeof(__init_square));
    memset(&init_chrgb, 0, sizeof(__init_chrgb));
    memset(&init_rotation, 0, sizeof(__init_rotation));

    const char *short_args = "scrbeC:S:p:P:x:y:v:a:w:o:h";
    const struct option long_args[] = {
            {"square",no_argument,nullptr,'s'},
            {"chrgb",no_argument,nullptr,'c'},
            {"rotation",no_argument, nullptr,'r'},
            {"begin",no_argument,nullptr,'b'},
            {"end",no_argument,nullptr,'e'},
            {"angle",required_argument,nullptr,'a'},
            {"color",required_argument,nullptr,'C'},
            {"side",required_argument,nullptr,'S'},
            {"width",required_argument,nullptr,'w'},
            {"pour",no_argument,nullptr,'p'},
            {"pour-color",required_argument,nullptr,'P'},
            {"x",required_argument,nullptr,'x'},
            {"y",required_argument,nullptr,'y'},
            {"value",required_argument,nullptr,'v'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr,0,nullptr,0}
    };

    while ((res = getopt_long(_argc, _argv, short_args, long_args, &index)) != -1) {
        argflags.help_flag = 0;

        if (!argflags.isActive()) {
            switch(res) {
                case 's': //square
                    argflags.square_flag = 1;

                    break;
                case 'r': //rotation
                    argflags.rotation_flag = 1;

                    break;
                case 'c': //chrgb
                    argflags.chrgb_flag = 1;

                    break;
                default:
                    if (res == 'o' || res == 'h')
                        break;

                    std::cout << "You should read documentation\n";
                    return 0;
            }

            continue;
        }

        if (argflags.coordflags) {
            switch(res) {
                case 'x':
                    if (end)
                        init_rotation.rcoord.X = (unsigned)strtol(optarg, nullptr, 10);
                    else if (argflags.rotation_flag)
                        init_rotation.lcoord.X = (unsigned)strtol(optarg, nullptr, 10);
                    else {
                        init_square.lcoord.X = (unsigned)strtol(optarg, nullptr, 10);
                    }

                    break;
                case 'y':
                    if (end)
                        init_rotation.rcoord.Y = (unsigned)strtol(optarg, nullptr, 10);
                    else if (argflags.rotation_flag)
                        init_rotation.lcoord.Y = (unsigned)strtol(optarg, nullptr, 10);
                    else
                        init_square.lcoord.Y = (unsigned)strtol(optarg, nullptr, 10);

                    break;
                default:
                    argflags.coordflags = 0;
                    end = 0;
            }

            if (argflags.coordflags)
                continue;
        }

        switch(res) {
            case 'C': //Color
                if ((!argflags.chrgb_flag && !argflags.square_flag)) {
                    std::cout << "Error, cant find --chrgb\n";
                    return 0;
                }

                if (argflags.square_flag) {
                    if (strlen(optarg) != 6) {
                        std::cout << "color error!";
                        return 0;
                    }

                    for (int i = 0; i < 3; i++) {
                        strncpy(buffer, optarg + i * 2, 2);
                        *((byte *)&init_square.color + i) = (byte)(strtol(buffer, nullptr, 16));

                        if (errno == ERANGE) {
                            std::cout << "color error!";
                            return 0;
                        }
                    }
                }

                if (argflags.chrgb_flag) {
                    for (offset = 0; offset < 3; offset++)
                        if (strcmp(optarg, _colors[offset]) == 0)
                            break;

                    if (offset == 3) {
                        std::cout << "color error!";
                        return 0;
                    }

                    init_chrgb.offset = offset;
                }

                break;
            case 'v':  //value
                if (!argflags.chrgb_flag) {
                    std::cout << "Error, cant find --chrgb\n";
                    return 0;
                }

                value = (unsigned)strtol(optarg, nullptr, 10);

                if (value > 255 || (optarg[0] != '0' && value == 0)) {
                    std::cout << "Value Error!\n";
                    return 0;
                }

                init_chrgb.value = value;

                break;
            case 'b': //begin
                if (!argflags.square_flag && !argflags.rotation_flag) {
                    std::cout << "Error, cant find --square or --rotation\n";
                    return 0;
                }

                argflags.coordflags = 1;

                break;
            case 'e': //end
                if (!argflags.rotation_flag) {
                    std::cout << "Error, cant find --rotation\n";
                    return 0;
                }

                argflags.coordflags = 1;
                end = 1;

                break;
            case 'a': //angle
                if (!argflags.rotation_flag || argflags.coordflags) {
                    std::cout << "Error, cant find --rotation\n";
                    return 0;
                }

                angle = (unsigned)(((strtol(optarg, nullptr, 10) / 90) % 4) * 90);

                init_rotation.angle = angle;

                break;
            case 'S': //Side
                if (!argflags.square_flag) {
                    std::cout << "Error, cant find --square\n";
                    return 0;
                }

                side = (unsigned)strtol(optarg, nullptr, 10);

                init_square.side = side;

                break;
            case 'w': //width
                if (!argflags.square_flag) {
                    std::cout << "Error, cant find --square\n";
                    return 0;
                }

                width = (unsigned)strtol(optarg, nullptr, 10);

                init_square.width = width;

                break;
            case 'p': //pour
                if (!argflags.square_flag) {
                    std::cout << "Error, cant find --square\n";
                    return 0;
                }

                init_square.pour = 1;

                break;
            case 'P': //Pour_color
                if (!argflags.square_flag) {
                    std::cout << "Error, cant find --square\n";
                    return 0;
                }

                if (strlen(optarg) != 6) {
                    std::cout << "color error!";
                    return 0;
                }

                for (int i = 0; i < 3; i++) {
                    strncpy(buffer, optarg + i * 2, 2);
                    *((byte *)&init_square.pour_color + i) = (byte)(strtol(buffer, nullptr, 16));

                    if (errno == ERANGE) {
                        std::cout << "color error!";
                        return 0;
                    }
                }

                break;
            case 'o':
                if (strlen(optarg) > 255) {
                    std::cout << "File name error!\n";
                    return 0;
                }

                strcpy(_file_name, optarg);

                break;
            case 'h':
                argflags.help_flag = 1;

                break;
            default:
                std::cout << "You should read documentation\n";
                return 0;
        }

    }
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "rus");

    srand((unsigned)time(nullptr));

    char file_name[256] = { 0 };
    strcpy(file_name, "new.bmp");

    if (!ParseArgs(argc, argv, file_name)) {
        std::cout << ":(\n";
        return 0;
    }

    if (argflags.help_flag)
        std::cout << help_info;
    else {
        BMP_File file(file_name);
        file.getFileInfo()();

        if (argflags.square_flag)
            DRAW_FILE_SQUARE(file, init_square);

        if (argflags.chrgb_flag)
            CHANGE_FILE_RGB(file, init_chrgb);

        if (argflags.rotation_flag)
            ROTATE_FILE_IMAGE(file, init_rotation);

        file.writeImage();
    }

    return 0;
}
