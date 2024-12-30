#include "fileheader.h"

bool CheckValue(const std::string &input) {
    for (char c : input) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

Args ParseArgs(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        size_t argSize = strlen(argv[i]);
        if (strncmp(argv[i], "-i", argSize) == 0 || strncmp(argv[i], "--input", argSize) == 0) {
            args.file = argv[++i];
        } else if (strncmp(argv[i], "-o", argSize) == 0 || strncmp(argv[i], "--output", argSize) == 0) {
            args.outDir = argv[++i];
        } else if ((strncmp(argv[i], "-m", argSize) == 0 || strncmp(argv[i], "--max-iter", argSize) == 0) && CheckValue(argv[i + 1])) {
            args.maxIters = std::stoull(argv[++i]);
        } else if ((strncmp(argv[i], "-f", argSize) == 0 || strncmp(argv[i], "--freq", argSize) == 0) && CheckValue(argv[i + 1])) {
            args.freq = std::stoull(argv[++i]);
        } else {
            args.isValid = false;
            break;
        }
    }
    return args;
}

void Output(int argc, char** argv) {
    Args args = ParseArgs(argc, argv);
    if (!args.isValid) {
        std::cerr << "Incorrect input";
        return;
    }

    Sandpile sandpile;
    sandpile.ParseParams(args.file);
    sandpile.Load(args.file);
    sandpile.CalcDims();
    std::string outputPathBase = args.outDir + "\\image";

    auto SaveBmp = [&](uint64_t iterCount) {
        std::string outputPath = outputPathBase + (iterCount ? std::to_string(iterCount) : "") + ".bmp";
        std::cout << outputPath << '\n';
        ConvertToBmp(sandpile.actWidth, sandpile.actHeight, sandpile.actX, sandpile.actY, sandpile.grid, outputPath);
    };

    uint64_t iterCount = 0;
    bool isUnbounded = args.maxIters == 0;

    if (args.freq == 0) {
        if (isUnbounded) {
            while (sandpile.runIters) sandpile.Step();
        } else {
            for (; iterCount < args.maxIters; ++iterCount) sandpile.Step();
        }
        sandpile.Show();
        SaveBmp(0);
    } else {
        while (isUnbounded ? sandpile.runIters : (iterCount < args.maxIters)) {
            sandpile.Step();
            if (iterCount % args.freq == 0) SaveBmp(iterCount);
            ++iterCount;
        }
    }
    for (int64_t i = 0; i < sandpile.height / 2; ++i) {
        delete[] sandpile.grid[i];
    }
    delete[] sandpile.grid;

    std::cout << "\nFile created!!!";
}

int main(int argc, char** argv) {
    Output(argc, argv);
    return 0;
}

