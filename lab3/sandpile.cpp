#include "fileheader.h"

void Sandpile::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open the file";
        return;
    }

    grid = new int64_t*[height];
    for (int64_t i = 0; i < height; ++i) {
        grid[i] = new int64_t[width]();
    }

    std::string line;
    int64_t value, x, y;
    while (std::getline(file, line)) {
        std::stringstream stream(line);
        stream >> x >> y >> value;
        grid[y - minY][x - minX] = value;
    }
}

void Sandpile::ParseParams(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open the file";
        return;
    }

    int16_t maxY = 0, maxX = 0, value;
    while (file >> value) {
        maxX = std::max(maxX, value);
        minX = std::min(minX, value);
        file >> value;
        maxY = std::max(maxY, value);
        minY = std::min(minY, value);
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    width = maxX - minX + 1;
    height = maxY - minY + 1;
}

void Sandpile::Show() const {
    for (int64_t i = actY; i < actY + actHeight + 1; ++i) {
        for (int64_t j = actX; j < actX + actWidth + 1; ++j) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << '\n';
    }
} 

void Sandpile::CalcDims() {
    int64_t minXPos = width, minYPos = height, maxXPos = 0, maxYPos = 0;
    for (int64_t i = 0; i < height; ++i) {
        for (int64_t j = 0; j < width; ++j) {
            if (grid[i][j] != 0) {
                minXPos = std::min(minXPos, j);
                minYPos = std::min(minYPos, i);
                maxXPos = std::max(maxXPos, j);
                maxYPos = std::max(maxYPos, i);
            }
        }
    }
    actX = minXPos;
    actY = minYPos;
    actWidth = maxXPos - minXPos + 1;
    actHeight = maxYPos - minYPos + 1;
}

void Sandpile::Resize(char dir) {
    int64_t newHeight = (dir == kUp || dir == kDown) ? height * 2 : height;
    int64_t newWidth = (dir == kLeft || dir == kRight) ? width * 2 : width;

    int64_t** tempGrid = new int64_t*[newHeight];
    for (int64_t i = 0; i < newHeight; ++i) {
        tempGrid[i] = new int64_t[newWidth]();
    }

    int64_t rowOffset = (dir == kUp) ? height : 0;
    int64_t colOffset = (dir == kLeft) ? width : 0;

    for (int64_t i = 0; i < height; ++i) {
        for (int64_t j = 0; j < width; ++j) {
            tempGrid[i + rowOffset][j + colOffset] = grid[i][j];
        }
    }

    for (int64_t i = 0; i < height; ++i) {
        delete[] grid[i];
    }
    delete[] grid;

    grid = tempGrid;
    height = newHeight;
    width = newWidth;
}

void Sandpile::Step() {
    runIters = false;
    bool resized = false;

    for (int64_t i = actY; i < actY + actHeight + 1 && !resized; ++i) {
        for (int64_t j = actX; j < actX + actWidth + 1 && !resized; ++j) {
            if (grid[i][j] > 3) {
                if (j == 0) {
                    Resize(kLeft);
                    actX += width / 2;
                    resized = true;
                } else if (j == width - 1) {
                    Resize(kRight);
                    resized = true;
                } else if (i == 0) {
                    Resize(kUp);
                    actY += height / 2;
                    resized = true;
                } else if (i == height - 1) {
                    Resize(kDown);
                    resized = true;
                }
                if (resized) break;
                if (j == actX) {
                    ++actWidth;
                    --actX;
                } else if (i == actY) {
                    --actY;
                    ++actHeight;
                } else if (i == actY + actHeight) {
                    ++actHeight;
                } else if (j == actX + actWidth) {
                    ++actWidth;
                }
                grid[i - 1][j]++;
                grid[i + 1][j]++;
                grid[i][j - 1]++;
                grid[i][j + 1]++;
                grid[i][j] -= 4;
                if (grid[i - 1][j] > 3 || grid[i + 1][j] > 3 || grid[i][j - 1] > 3 || grid[i][j + 1] > 3 || grid[i][j] > 3) {
                    runIters = true;
                }
            }
        }
    }
}

