//
// Created by cybergh on 03.12.2018.
//

#ifndef PROIECTUNVERSITATE_UTILITY_H
#define PROIECTUNVERSITATE_UTILITY_H

#define PIXEL_ARRAY_BEGIN_OFFSET 54
#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 24
#define HEADER_SIZE 54

char load_key_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/secret_key.txt";
char load_image_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";
char save_encrypted_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.encrypted.bmp";
char save_permuted_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.permuted.bmp";

typedef unsigned int uint;
typedef unsigned char uchar;

struct Pixel_t{
    uchar R,G,B;
};
typedef struct Pixel_t Pixel;

union PixelKey_t{
    uchar colors[3];
    uint key;
};
typedef union PixelKey_t PixelKey;

struct Image_t{
    uint width, height, size;
    Pixel *pixels;
    uchar header[HEADER_SIZE + 1];
};


typedef struct Image_t Image;
typedef Image* Image_ptr;

void deconstructImage(Image_ptr );

Image_ptr loadBMPImage(char* path);
void saveBMPImage(Image_ptr img, char *path);
Pixel reversePixel(Pixel);
void encryptImage(char *inputPath, char *outputPath, char *keyPath);
void loadSecretKeys(char *keyPath, uint* key_1, uint* key_2);
Image_ptr permuteImage(Image_ptr img, uint *randomNumbers);
uint* xorShift32(uint seed, size_t nr);

#endif //PROIECTUNVERSITATE_UTILITY_H
