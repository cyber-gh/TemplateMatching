#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "utility.h"

Pixel reversePixel(Pixel p){
    /*
     * decodes pixel from little endian binary represantion B G R -> R G B
     */
    uchar temp = p.R;
    p.R = p.B;
    p.B = temp;
    return p;
}




void encryptImage(char *inputPath, char *outputPath, char *keyPath){

    Image_ptr img = loadBMPImage(inputPath);
    uint key_1, key_2;
    loadSecretKeys(keyPath, &key_1, &key_2);
    uint *randomNumbers = xorShift32(key_1, img->size * 2 - 1);

    img = permuteImage(img, randomNumbers);
    saveBMPImage(img,save_permuted_path);
    // xor the permuted image
    int i, k;
    PixelKey *xorKey = malloc(sizeof(PixelKey)), *randomNumber = malloc(sizeof(PixelKey));
    xorKey->key = key_2;
    randomNumber->key = randomNumbers[img->size - 1];

    printf("pixel bytes R G B   -> %u %u %u\n", img->pixels[0].R, img->pixels[0].G, img->pixels[0].B);
    printf("key bytes           -> %u %u %u\n", xorKey->colors[0], xorKey->colors[1], xorKey->colors[2]);
    printf("random number bytes -> %u %u %u\n", randomNumber->colors[0], randomNumber->colors[1], randomNumber->colors[2]);

    img->pixels[0].R ^= xorKey->colors[2];
    img->pixels[0].G ^= xorKey->colors[1];
    img->pixels[0].B ^= xorKey->colors[0];


    img->pixels[0].R ^= randomNumber->colors[2];
    img->pixels[0].G ^= randomNumber->colors[1];
    img->pixels[0].B ^= randomNumber->colors[0];

    //xorKey = img->pixels[0];

    printf("xor pixel bytes R G B-> %u %u %u\n", img->pixels[0].R, img->pixels[0].G, img->pixels[0].B);
    memcpy(xorKey, &img->pixels[0], sizeof(Pixel));

    for (i = 1, k = 0; i < img->size;i++, k++){
        img->pixels[i].R ^= xorKey->colors[0];
        img->pixels[i].G ^= xorKey->colors[1];
        img->pixels[i].B ^= xorKey->colors[2];

        randomNumber->key = randomNumbers[img->size + k];
        img->pixels[i].R ^= randomNumber->colors[0];
        img->pixels[i].G ^= randomNumber->colors[1];
        img->pixels[i].B ^= randomNumber->colors[2];

        memcpy(xorKey, &img->pixels[i], sizeof(Pixel));

    }
    saveBMPImage(img, outputPath);
    deconstructImage(img);
}

Image_ptr permuteImage(Image_ptr img, uint *randomNumbers){
    int i = 0,j;
    for (i = img->size - 1, j = 0; i >= 1;i--,j++ ){
        uint randNr = randomNumbers[j] % (i);
        Pixel temp = img->pixels[randNr];
        img->pixels[randNr] = img->pixels[i];
        img->pixels[i] = temp;
    }
    printf("j=%d\n", j);
    return img;

}

void loadSecretKeys(char *keyPath, uint* key_1, uint* key_2){
    FILE* fin = fopen(keyPath,"r");
    if (fin == NULL){
        printf("ERROR: can't open secret keys file\n");
        exit(0);
    }
    fscanf(fin, "%u %u", key_1, key_2);

    fclose(fin);
}

uint* xorShift32(uint seed, size_t nr){
    /*
     * generates nr numbers from the seed in an array and returns the array
     */
    uint *arr = calloc(nr, sizeof(int));
    uint i, currValue = seed;
    for (i = 0; i < nr;i++){
        currValue = currValue ^ currValue << 13;
        currValue = currValue ^ currValue >> 17;
        currValue = currValue ^ currValue << 5;
        arr[i] = currValue;
    }

    return arr;

}

Image_ptr loadBMPImage(char* path){

    /*
     * loads image as Pixel array in memory and returns array
     *
     */

    FILE* fin = fopen(path, "rb");
    if (fin == NULL){
        printf("ERROR: can't open the bmp image file");
        exit(0);
    }
    Image_ptr img = malloc(sizeof(Image));

    //TODO: check if it's a valid bmp file
    fseek(fin, WIDTH_OFFSET, SEEK_SET);
    uint width, height;
    fread(&width, sizeof(uint), 1, fin);
    fread(&height, sizeof(uint), 1, fin);

    img->width = width;
    img->height = height;
    img->size = height*width;

    //TODO: reading twice, improve it
    fseek(fin, 0, SEEK_SET);
    fread(img->header,HEADER_SIZE,sizeof(uchar), fin);

    fseek(fin, PIXEL_ARRAY_BEGIN_OFFSET, SEEK_SET);
    Pixel *pixelArray = calloc(width*height, sizeof(Pixel));
    int i = 0, j = 0;
    // line must be divisor of 4,
    // else add padding to width
    int padding = width % 4;
    if (padding != 0)
        padding = 4 - padding;

    for (i = height - 1; i >= 0;i--){
        for (j = 0; j < width;j++){
            Pixel currPixel;
            fread(&currPixel, sizeof(Pixel), 1, fin);
            currPixel = reversePixel(currPixel);
            pixelArray[i*width + j] = currPixel;
        }
        int dummyValue;
        if (padding != 0)
            fread(&dummyValue, sizeof(uchar), padding, fin);

    }
    img->pixels = pixelArray;
    fclose(fin);
    return img;
}


void saveBMPImage(Image_ptr img, char *path){
    FILE* fout = fopen(path, "wb");
    if (fout == NULL){
        printf("ERROR: can't open file to save bmp image");
        return ;
    }
    fwrite(img->header,HEADER_SIZE, sizeof(uchar), fout);
    //TODO: null pointer exception possibility
    int padding = img->width % 4;
    if (padding != 0)
        padding = 4 - padding;

    int i,j;
    for (i = img->height - 1 ; i >= 0;i--){
        for (j = 0; j < img->width;j++){
            Pixel currPixel = img->pixels[i*img->width + j];
            currPixel = reversePixel(currPixel);
            fwrite(&currPixel,sizeof(Pixel), 1, fout);

            //fread(&currPixel, sizeof(Pixel), 1, fout);
            //currPixel = reversePixel(currPixel);
            // = currPixel;
        }
        uchar dummyValue = 0;
        if (padding != 0)
            fwrite(&dummyValue, sizeof(uchar), (size_t) padding, fout);

    }
    fclose(fout);

}

void testXorShift32(){
    unsigned int secret = 123456789;
    size_t nr = 10;
    unsigned int *arr = xorShift32(secret, nr);
    int i;
    for (i = 0; i < nr;i++) printf("%u ", arr[i]);

}

void testLoadBMP(){
    char path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";
    Image_ptr img = loadBMPImage(path);
    printf("%d %d\n", img->width, img->height);
    int i = 0;
    for (i = 0; i <= 4;i++) {
        printf("%d %d %d \n", img->pixels[i].R, img->pixels[i].G, img->pixels[i].B);
    }
    deconstructImage(img);

}

void deconstructImage(Image_ptr img) {
    free(img->pixels);
    free(img);
    img = NULL;

}

void loadAndSave(){
    char load_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";

    char save_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bak.bmp";
    Image_ptr img = loadBMPImage(load_path);
    saveBMPImage(img, save_path);
    deconstructImage(img);
}

void testLoadKeys(){
    char load_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/secret_key.txt";
    uint key_1, key_2;
    loadSecretKeys(load_path, &key_1, &key_2);
    printf("%u %u", key_1, key_2);
}

void testPermuteImage(){
    char load_key_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/secret_key.txt";
    char load_image_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";
    char save_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.permuted.bmp";

    uint key_1, key_2;
    Image_ptr rawImg = loadBMPImage(load_image_path);
    loadSecretKeys(load_key_path, &key_1, &key_2);
    uint* randomNumbers = xorShift32(key_1, rawImg->size*2 - 1);
    Image_ptr permutedImg = permuteImage(rawImg, randomNumbers);
    saveBMPImage(permutedImg, save_path);
    deconstructImage(permutedImg);

}

void testEncrypt(){
    char load_key_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/secret_key.txt";
    char load_image_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";
    char save_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.encrypted.bmp";

    encryptImage(load_image_path,save_path,load_key_path);

}

int main() {
    // here we go
    printf("%d\n", sizeof(Image));
    //testLoadBMP();
    //loadAndSave();
    //testLoadKeys();
    //testPermuteImage();
    testEncrypt();
    return 0;
}