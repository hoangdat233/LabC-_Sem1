#pragma once

#include <sstream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cmath>
#include <cstring>
#include <limits>
#include <iostream>

static const int8_t kBase = 10;

struct Args {
    uint64_t maxIters = 0;
    uint64_t freq = 0;
    bool isValid = true;

    std::string file;
    std::string outDir;
};

bool CheckValue(const std::string &input);

Args ParseArgs(int argc, char** argv);


struct Sandpile {
    static const int16_t kMaxCoord = INT16_MAX;
    static const char kUp = 'u';
    static const char kDown = 'd';
    static const char kRight = 'r';
    static const char kLeft = 'l';

    int64_t actX = 0;
    int64_t actY = 0;
    int64_t actWidth = 0;
    int64_t actHeight = 0; 

    int64_t **grid = nullptr;

    int64_t width = 0;
    int64_t height = 0;

    bool runIters = true;

    int16_t minX = kMaxCoord;
    int16_t minY = kMaxCoord;

    void CalcDims();
    void ParseParams(const std::string& params);
    void Resize(char dir);
    void Step();
    void Show() const;
    void Load(const std::string& file);
};

static const int8_t kFileHdrSize = 14;
static const int8_t kInfoHdrSize = 60;
static const uint8_t kPlaneCount = 1;
static const uint8_t kBitsPerPx = 4;    
static const uint8_t kAlignWidth = 4;
static const uint8_t kShiftByte1 = 8;
static const uint8_t kShiftByte2 = 16;
static const uint8_t kShiftByte3 = 24;
static const uint8_t kColorCount = 16;
static const uint8_t kInfoHdrFixedSize = 40;

static const uint16_t kColor255 = 255;
static const uint16_t kColor139 = 139;
static const uint16_t kColor0 = 0;

void SetWhite(uint8_t *info);
void SetGreen(uint8_t *info);
void SetPurple(uint8_t *info);
void SetYellow(uint8_t *info);
void ConvertToBmp(int64_t w, int64_t h, int64_t startX, int64_t startY, int64_t **array, const std::string& output);

void Output(int argc, char** argv);
