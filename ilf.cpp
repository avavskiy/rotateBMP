#include "ilf.h"
#include <cmath>

const double pi = 3.14159265358979323846;

double gaussF(double x, double y, double sigma) {
    return (1.0 / (2.0 * pi * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

void GaussFilter(unsigned char* inputPixels, unsigned char* outputPixels, int32_t width, int32_t height, int16_t bpp) {
    int sizecore = 11; //размер ядра
    double sigma = 11.0; //степень размытия
    double sum = 0.0;
    std::vector<std::vector<double>> core(sizecore, std::vector<double>(sizecore));

    for (int i = 0; i < sizecore; i++) {
        for (int j = 0; j < sizecore; j++) {
            double x = i - sizecore / 2;
            double y = j - sizecore / 2;
            core[i][j] = gaussF(x, y, sigma);
            sum += core[i][j];
        }
    }

    for (int i = 0; i < sizecore; i++) {
        for (int j = 0; j < sizecore; j++) {
            core[i][j] /= sum;
        }
    }

    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            double r = 0.0, g = 0.0, b = 0.0;

            for (int i = 0; i < sizecore; i++) {
                for (int j = 0; j < sizecore; j++) {
                    int xOffset = x - sizecore / 2 + i;
                    int yOffset = y - sizecore / 2 + j;

                    if (xOffset >= 0 && xOffset < width && yOffset >= 0 && yOffset < height) {
                        int index = (yOffset * width + xOffset) * bpp / 8;
                        r += inputPixels[index + 2] * core[i][j];
                        g += inputPixels[index + 1] * core[i][j];
                        b += inputPixels[index] * core[i][j];
                    }
                }
            }

            int index = (y * width + x) * bpp / 8;
            outputPixels[index + 2] = static_cast<unsigned char>(r);
            outputPixels[index + 1] = static_cast<unsigned char>(g);
            outputPixels[index] = static_cast<unsigned char>(b);
        }
    }
}

void RotatePhotoRight(unsigned char* originalPixels, unsigned char* rotatedPixels, int32_t width, int32_t height, int16_t bpp) {
    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            int32_t sourceIndex = (y * width + x) * (bpp / 8);
            int32_t destIndex = ((width - x - 1) * height + y) * (bpp / 8);
            for (int32_t byteIndex = 0; byteIndex < (bpp / 8); ++byteIndex) {
                rotatedPixels[destIndex + byteIndex] = originalPixels[sourceIndex + byteIndex];
            }
        }
    }
}

void RotatePhotoLeft(unsigned char* originalPixels, unsigned char* rotatedPixels, int32_t width, int32_t height, int16_t bpp) {

    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            int32_t sourceIndex = (y * width + x) * (bpp / 8);
            int32_t destIndex = (x * height + (height - y - 1)) * (bpp / 8);
            for (int32_t byteIndex = 0; byteIndex < (bpp / 8); ++byteIndex) {
                rotatedPixels[destIndex + byteIndex] = originalPixels[sourceIndex + byteIndex];
            }
        }
    }
}