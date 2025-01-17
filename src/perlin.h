#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

typedef struct {
    float x;
    float y;
} vector2; // 2D vector.

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXpelsPerMeter;
    int32_t biYpelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

// Creates a psuedo-Random value using a seed values, this is done so that the result of the alorithim can be replicated in perlin noise generation
vector2 randomGradient(int ix, int iy, int seed) {
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix + seed, b = iy + seed;
    // Peforms a large multication operation using a large prime number making the number look more random
    a *= 3284157443;
    // Creates a value using a and the size of w + w / 2 making B appear more random
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
    // Merges a and b seamlessly
    a ^= b << s | b >> (w - s);
    a *= 2048419325;

    // Weighs the result of a to from 0 to 2 radiants
    float random = a * (3.14159265 / ~(~0u >> 1)); // [0, 2*Pi]
    //Creates a vector value ranging from -1 to 0
    vector2 v = { sin(random), cos(random) };
    return v;
}

float dotProduct(int ix, int iy, float x, float y, int seed) {
    // Creates a random Gradient for usage in creating a random value for producing a dot product value between the corners and interception point
    vector2 grad = randomGradient(ix, iy, seed);

    // Obtains the fractionial parts of x and y
    float dx = x - (float)ix;
    float dy = y - (float)iy;
    return dx * grad.x + dy * grad.y;
}

float smoothStep(float a, float b, float w) {
    // Smooths the results of each corner to produce a smoother looking perlin noise result using a weight.
    return (b - a) * (3.0 - w * 2.0) * w * w + a;
}

float perlinNoise(float x, float y, int seed) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float xf = x - (float)x0;
    float yf = y - (float)y0;

    // Creates the values of the bottom left and bottom right corners using the dot product function to make the noise have a gradient effect
    float c0 = dotProduct(x0, y0, x, y, seed);
    float c1 = dotProduct(x1, y0, x, y, seed);
    // Makes the noise look "Smoother"
    float val1 = smoothStep(c0, c1, xf);

    //Top left and Top right this time
    float c2 = dotProduct(x0, y1, x, y, seed);
    float c3 = dotProduct(x1, y1, x, y, seed);
    float val2 = smoothStep(c2, c3, xf);

    // Returns the noise value using the given coordinates
    return smoothStep(val1, val2, yf);
}

void writeBMP(const char* filename, int* pixels, int width, int height) {
    FILE* file = fopen(filename, "wb");
    // Simple Error Handling to see if the application has opened or created the file correctly
    if (!file) {
        perror("Failed to create file!\n");
        return;
    }

    int rowSize = (3 * width + 3) & ~3; // ~3 Is used here to that the result is in factors of 3
    uint32_t pixelArraySize = rowSize * height;

    BITMAPFILEHEADER fileHeader = {
        .bfType = 0x4D42,
        .bfSize = (uint32_t)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pixelArraySize),
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    };

    BITMAPINFOHEADER fileInfoHeader = {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = width,
        .biHeight = -height, 
        .biPlanes = 1,
        .biBitCount = 24,
        .biCompression = 0,
        .biSizeImage = pixelArraySize,
        .biXpelsPerMeter = 0,
        .biYpelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0
    };

    /* 
    Write both the headers to the file so that the data can be added afterwards. 
    This is done so that the operating system can understand what type of file this is and how to display it.
    */
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&fileInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    uint8_t padding[3] = { 0, 0, 0 };

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t pixel = (uint8_t)pixels[y * width + x];
            fwrite(&pixel, 1, 1, file); // Red
            fwrite(&pixel, 1, 1, file); // Green
            fwrite(&pixel, 1, 1, file);  // Blue
        }
        fwrite(padding, 1, (rowSize - width * 3), file); // Write the pixel data to the file.
    }

    fclose(file);
}

int createPerlinNoise(float cellSize, int width, int height, int seed) {
    // Sets the seed, used in making a peusdo-random gradient to make it so that the result can be replicated.
    
    // Allocates memory for each of the pixel noise values
    int* values = (int*)malloc(sizeof(int) * width * height);

    // Fills values with the perlin noise values used in creating the image.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Noise value is created so the value can be scaled to [0, 255]
            float noise = perlinNoise(x / cellSize, y / cellSize, seed);
            noise = (noise + 1.0f) * 0.5f * 255.0f; // Scaled to [0, 255]
            values[y * width + x] = (int)noise;
        }
    }

    // Creates a bitmap file for usage in other applications, in terms of this project; The 3D enviroment
    writeBMP("perlin.bmp", values, width, height);
    //I'd rather not have the program have a memory leak
    free(values);

    return 0;
}