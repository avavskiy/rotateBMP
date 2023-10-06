#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xResolution;
    int32_t yResolution;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)

const double pi = 3.14159265358979323846;

double gaussF(double x, double y, double sigma) {
    return (1.0 / (2.0 * pi * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

void GaussFilter(unsigned char* inputPixels, unsigned char* outputPixels, int32_t width, int32_t height, int16_t bpp) {
    int sizecore = 11; //размер ядра
    double sigma = 11.0; //степень размытия
    double sum = 0.0;
    double core[sizecore][sizecore];

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

    //размытие к каждому пикселю 
    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            double r = 0.0, g = 0.0, b = 0.0;

            for (int i = 0; i < sizecore; i++) {
                for (int j = 0; j < sizecore; j++) {
                    int xOffset = x - sizecore / 2 + i;
                    int yOffset = y - sizecore / 2 + j;

                    //чтобы не выйти за границы 
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

int main() {
    std::ifstream file("image.bmp", std::ios::binary);
    
    BMPHeader head;
    file.read(reinterpret_cast<char*>(&head), sizeof(BMPHeader));
    
    int32_t width = head.width;
    int32_t height = head.height;
    int16_t bpp = head.bpp;
    unsigned char* originalPixels = new unsigned char[width * height * (bpp / 8)];
    file.read(reinterpret_cast<char*>(originalPixels), width * height * (bpp / 8));

    int32_t newWidth = height;
    int32_t newHeight = width;

    unsigned char* RotatePixelRight = new unsigned char[newWidth * newHeight * (bpp / 8)];
    unsigned char* RotatePixelLeft = new unsigned char[newWidth * newHeight * (bpp / 8)];

    RotatePhotoRight(originalPixels, RotatePixelRight, width, height, bpp);
    RotatePhotoLeft(originalPixels, RotatePixelLeft, width, height, bpp);

    unsigned char* filteredPixelsGauss = new unsigned char[newWidth * newHeight * (bpp / 8)];

    GaussFilter(RotatePixelRight, filteredPixelsGauss, newWidth, newHeight, bpp);

    std::swap(head.width,head.height);

    std::ofstream outputFileRightGauss("image_right_gauss.bmp", std::ios::binary);
    std::ofstream outputFileLeft("image_left.bmp", std::ios::binary);
    std::ofstream outputFileRight("image_right.bmp", std::ios::binary);

    outputFileRight.write(reinterpret_cast<char*>(&head), sizeof(BMPHeader));
    outputFileRight.write(reinterpret_cast<char*>(RotatePixelRight), newWidth * newHeight * (bpp / 8));
    outputFileRight.close();

    outputFileRightGauss.write(reinterpret_cast<char*>(&head), sizeof(BMPHeader));
    outputFileRightGauss.write(reinterpret_cast<char*>(filteredPixelsGauss), newWidth * newHeight * (bpp / 8));
    outputFileRightGauss.close();

    outputFileLeft.write(reinterpret_cast<char*>(&head), sizeof(BMPHeader));
    outputFileLeft.write(reinterpret_cast<char*>(RotatePixelLeft), newWidth * newHeight * (bpp / 8));
    outputFileLeft.close();
        
    delete[] originalPixels;
    delete[] RotatePixelRight;
    delete[] RotatePixelLeft;
    delete[] filteredPixelsGauss;

    return 0;
}