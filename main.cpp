#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#pragma pack(push,1)
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

void GaussFilter(unsigned char* inputPixels, unsigned char* outputPixels, int32_t width, int32_t height, int16_t bpp) {
    float blur[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    for (int32_t y = 1; y < height - 1; ++y) {
        for (int32_t x = 1; x < width - 1; ++x) {
            int32_t destIndex = (y * width + x) * (bpp / 8);
            for (int32_t byteIndex = 0; byteIndex < (bpp / 8); ++byteIndex) {
                float sum = 0.0f;
                for (int32_t ky = -1; ky <= 1; ++ky) {
                    for (int32_t kx = -1; kx <= 1; ++kx) {
                        int32_t sourceX = x + kx;
                        int32_t sourceY = y + ky;
                        int32_t sourceIndex = (sourceY * width + sourceX) * (bpp / 8);
                        sum += inputPixels[sourceIndex + byteIndex] * blur[ky + 1][kx + 1];
                    }
                }
                outputPixels[destIndex + byteIndex] = static_cast<unsigned char>(sum);
            }
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
    ifstream file("furina.bmp", ios::binary);

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

    ofstream outputFileRightGauss("furina_right_gauss.bmp", ios::binary);
    ofstream outputFileLeft("furina_left.bmp", ios::binary);
    ofstream outputFileRight("furina_right.bmp", ios::binary);

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