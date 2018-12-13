//
// Created by cybergh on 03.12.2018.
//

#ifndef PROIECTUNVERSITATE_UTILITY_H
#define PROIECTUNVERSITATE_UTILITY_H

#define PIXEL_ARRAY_BEGIN_OFFSET 54
#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22
#define HEADER_SIZE 54
#define DEFAULT_THRESHOLD 0.5
#define OVERLAPPING_THRESHOLD 0.2
#define BUFFER_SIZE 200

#define xor(a, b) (a.colors[0]^=b.colors[0], a.colors[1]^=b.colors[1], a.colors[2]^=b.colors[2])

const char INPUT_FILE_PATH[] = "input.txt";

typedef unsigned int uint;
typedef unsigned char uchar;

/**
 * struct pentru Pixeli
 * 3 metoda de a accesa
 * cu tablou colors
 * cu struct color.culoare (R, G, B)
 * cu grey daca imaginea e grayscale
 */
union Pixel_t{
    struct {
        uchar R,G,B;
    } color;
    uchar colors[3];
    uchar gray;
};
typedef union Pixel_t Pixel;

/**
 * @useless
 */
union PixelKey_t{
    uchar colors[3];
    uint key;
};
typedef union PixelKey_t PixelKey;

/**
 * imaginea in ram
 * contine marimea in pixeli
 * pointer la tablou liniarziat de pixeli
 * pointer la matricea de pixeli
 */
struct Image_t{
    uint width, height, size; // size in pixels
    Pixel *pixels;
    Pixel **pixelsM;
    uchar header[HEADER_SIZE + 1];
};
typedef struct Image_t Image;
typedef Image* Image_ptr;

struct Dot_t{
    int x,y;
};
typedef struct Dot_t Point;

/**
 * fereastra a unei imagine
 * contine punct de mijloc
 * dimensiunea ferestrei in pixeli
 * pointer la intreaga imagine di memorie
 */
struct Window_t{
    Point mid;
    int height, width;
    int size;
    Pixel **M;
};
typedef struct Window_t Window;

/**
 * Detectie
 * contine punct de mijloc
 * punct din stanga sus si din dreapta jos
 * tipul detectie -> cifra
 * acuratetea, valoare la standard deviatat intre -1 si 1
 * good reprezinta daca e maxima sau nu, daca va ramane in tabloul de detectii
 * dupa stergea nonmaximelor
 */
struct Detection_t{
    Point mid;
    Point leftUpCorner;
    Point rightDownCorner;
    int type;
    double accuracy;
    short good;
};
typedef struct Detection_t Detection;

struct DetectionArr_t{
    Detection* array;
    int size;
};
typedef struct DetectionArr_t DetectionArr;


// encryption
void deconstructImage(Image_ptr );
Image_ptr loadBMPImage(char* path);
void saveBMPImage(Image_ptr img, char *path);
void loadSecretKeys(char *keyPath, uint* key_1, uint* key_2);
uint *xorShift32(uint seed, uint size);
uint *getRandomPermutation(uint* randomArr, uint size);
void permutePixels(Image_ptr img, uint* randomPermutation);
void dePermutePixels(Image_ptr img, uint* randomPermutation);
Pixel numberToPixel(uint nr);
void encryptImage(char *initial_img_path, char *encrypted_img_path, char *secret_keys_path);
void xorEncryptImage(Image_ptr img, uint* randArr, uint key);
void xorDecryptImage(Image_ptr img, uint* randArr, uint key);
void showPixel(Pixel p);
double chiSquaredTest(Image_ptr img, uchar channel);
void chiTest(char *path);

// templates matching
void toGrayscale(Image_ptr img);
double avgGrayIntensity(Window window);
void buildMatrixImg(Image_ptr img);
void buildSmartMatrixImg(Image_ptr img);
double standardDeviation(Window window, double avg);
double crossCorrelation(Window template, Window target);
void drawDetection(Pixel **M, Detection detection);
Pixel getColor(int number);
Detection convertDetection(Window targetWindow, int nr, double accuracy);
DetectionArr getDetections(Image_ptr target, char* path_to_template, int nr, double threshold);
void copyArray(DetectionArr *destination, DetectionArr *source);
DetectionArr getAllDetections(Image_ptr img, char paths[][BUFFER_SIZE], int nr);
double overlap(Detection a, Detection b);
int intersect(Detection a, Detection b);
DetectionArr removeOverlapping(DetectionArr detArr);

int min(int a, int b){ return (a<b?a:b);}
int max(int a, int b) {return (a>b?a:b);}


//colors
const Pixel COLOR_DIGIT_0 = {255, 0, 0};
const Pixel COLOR_DIGIT_1 = {255, 255, 0};
const Pixel COLOR_DIGIT_2 = {0, 255, 0};
const Pixel COLOR_DIGIT_3 = {0, 255, 255};
const Pixel COLOR_DIGIT_4 = {255, 0, 255};
const Pixel COLOR_DIGIT_5 = {0, 0, 255};
const Pixel COLOR_DIGIT_6 = {192, 192, 192};
const Pixel COLOR_DIGIT_7 = {255, 140, 0};
const Pixel COLOR_DIGIT_8 = {128, 0, 128};
const Pixel COLOR_DIGIT_9 = {128, 0, 0};
const Pixel COLOR_DEFAULT = { 255, 255, 255};




#endif //PROIECTUNVERSITATE_UTILITY_H
