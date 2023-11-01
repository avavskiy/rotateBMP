#pragma once

#include <vector>
#include <cstdint>

#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t headerSize;
    uint32_t dataOffset;
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

void GaussFilter(unsigned char* inputPixels, unsigned char* outputPixels, int32_t width, int32_t height, int16_t bpp);
void RotatePhotoRight(unsigned char* originalPixels, unsigned char* rotatedPixels, int32_t width, int32_t height, int16_t bpp);
void RotatePhotoLeft(unsigned char* originalPixels, unsigned char* rotatedPixels, int32_t width, int32_t height, int16_t bpp);
