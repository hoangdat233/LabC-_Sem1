#include "fileheader.h"

void SetWhite(uint8_t* info) {
    info[40] = kColor255;
    info[41] = kColor255;
    info[42] = kColor255;
}

void SetGreen(uint8_t* info) {
    info[44] = kColor0;
    info[45] = kColor255;
    info[46] = kColor0;
}

void SetPurple(uint8_t* info) {
    info[48] = kColor255;
    info[49] = kColor0;
    info[50] = kColor139;
}
    
void SetYellow(uint8_t* info) {
    info[52] = kColor0;
    info[53] = kColor255;
    info[54] = kColor255;
}
    
void ConvertToBmp(int64_t width, int64_t height, int64_t startX, int64_t startY, int64_t** array, const std::string& output) {

    ++width; ++height;
    int64_t alignedWidth = (width + 1) / 2;
    int64_t paddingSize = (kAlignWidth - alignedWidth % kAlignWidth) % kAlignWidth;
    int64_t fileSize = kInfoHdrSize + kFileHdrSize + height * (alignedWidth + paddingSize);

    uint8_t fileHeader[kFileHdrSize] = { 'B', 'M' };
    fileHeader[2] = (uint8_t) fileSize;
    fileHeader[3] = (uint8_t) (fileSize >> kShiftByte1);
    fileHeader[4] = (uint8_t) (fileSize >> kShiftByte2);
    fileHeader[5] = (uint8_t) (fileSize >> kShiftByte3);
    fileHeader[10] = (uint8_t) (kFileHdrSize + kInfoHdrSize);

    uint8_t infoHeader[kInfoHdrSize] = { 0 };
    infoHeader[0] = (uint8_t) kInfoHdrFixedSize;
    infoHeader[4] = (uint8_t) width;
    infoHeader[5] = (uint8_t) (width >> kShiftByte1);
    infoHeader[6] = (uint8_t) (width >> kShiftByte2);
    infoHeader[7] = (uint8_t) (width >> kShiftByte3);
    infoHeader[8] = (uint8_t) height;
    infoHeader[9] = (uint8_t) (height >> kShiftByte1);
    infoHeader[10] = (uint8_t) (height >> kShiftByte2);
    infoHeader[11] = (uint8_t) (height >> kShiftByte3);
    infoHeader[12] = (uint8_t) kPlaneCount;
    infoHeader[14] = (uint8_t) kBitsPerPx;
    infoHeader[32] = (uint8_t) kColorCount;

    SetWhite(infoHeader);
    SetGreen(infoHeader);
    SetYellow(infoHeader);
    SetPurple(infoHeader);

    std::ofstream fileWriteOutput(output, std::ios::out | std::ios::binary);
    if (!fileWriteOutput.is_open()) {
        std::cerr << "Can't open the file for writing";
        return;
    }

    fileWriteOutput.write((char*)fileHeader, kFileHdrSize);
    fileWriteOutput.write((char*)infoHeader, kInfoHdrSize);

    uint8_t padding = 0;

    for (int64_t i = startY; i < height + startY; ++i) {
        for (int64_t j = startX; j < width + startX; j += 2) {
            uint8_t previousColor = (j < width + startX) ? static_cast<uint8_t>(array[i][j]) : 0;
            uint8_t nextColor = (j + 1 < width + startX) ? static_cast<uint8_t>(array[i][j + 1]) : 0;
            uint8_t currentColor = (previousColor << 4) | nextColor;
            fileWriteOutput.write((char*)&currentColor, 1);
        }
        fileWriteOutput.write((char*)&padding, paddingSize);
    }

    fileWriteOutput.close();
}
