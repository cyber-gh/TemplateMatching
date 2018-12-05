
#include "utility.h"
#include "tests.c"

Pixel reversePixel(Pixel p){
    /*
     * decodes pixel from little endian binary represantion B G R -> R G B
     */
    uchar temp = p.color.R;
    p.color.R = p.color.B;
    p.color.B = temp;
    return p;
}

uint* xorShift32(uint seed, uint size){
    /*
     * return size random generated numbers,
     * first numbers randArr[0] is the seed
     */
    uint *randArr = calloc(size + 1, sizeof(uint));
    randArr[0] = seed;
    int i;
    for (i = 1; i <= size;i++){
        seed ^= seed << (uint)13;
        seed ^= seed >> (uint)17;
        seed ^= seed << (uint)5;
        randArr[i] = seed;
    }
    return randArr;
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
    int padding = 3*width % 4;
    if (padding != 0)
        padding = 4 - padding;

    for (i = height - 1; i >= 0;i--){
    //for(i = 0 ; i < height;i++){
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
    int padding = 3*img->width % 4;
    if (padding != 0)
        padding = 4 - padding;

    int i,j;
    for (i = img->height - 1 ; i >= 0;i--){
    //for (i = 0; i < img->height;i++){
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


void deconstructImage(Image_ptr img){
    free(img->pixels);
    free(img);
}


uint *getRandomPermutation(uint* randomArr, uint size){
    uint *arr = calloc(size, sizeof(uint));
    uint i, k, temp;
    for (k = 0; k < size;k++)
        arr[k] = k;
    for (k = size - 1, i = 1;k >= 1;k--, i++){
        uint rand = randomArr[i] % (k + 1);
        temp = arr[rand];
        arr[rand] = arr[k];
        arr[k] = temp;
    }

    return arr;

}

int main() {
    // here we go


    //testLoadBMP();
    //loadAndSave();
    //testLoadKeys();
    //testPermuteImage();
    //testEncrypt();
    return 0;
}



void testLoadBMP(){
    char path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";
    Image_ptr img = loadBMPImage(path);
    printf("%d %d\n", img->width, img->height);
    int i = 0;
    for (i = 0; i <= 4;i++) {
        printf("%d %d %d \n", img->pixels[i].color.R, img->pixels[i].color.G, img->pixels[i].color.B);
    }
    deconstructImage(img);

}

void loadAndSave(){
    char load_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bmp";

    char save_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/peppers.bak.bmp";
    Image_ptr img = loadBMPImage(load_path);
//    printf("%u %u %u\n", img->pixels[0].color.R, img->pixels[0].color.G, img->pixels[0].color.B);
//    printf("%u %u %u\n", img->pixels[1].color.R, img->pixels[1].color.G, img->pixels[1].color.B);
//
//    xor(img->pixels[0], img->pixels[1]);
//    printf("%u %u %u\n", img->pixels[0].color.R, img->pixels[0].color.G, img->pixels[0].color.B);
    saveBMPImage(img, save_path);
    deconstructImage(img);
}

void testLoadKeys(){
    char load_path[] = "/home/cybergh/CLionProjects/ProiectUnversitate/date/criptografie/secret_key.txt";
    uint key_1, key_2;
    loadSecretKeys(load_path, &key_1, &key_2);
    printf("%u %u", key_1, key_2);
}

