#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "ilf.h"

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