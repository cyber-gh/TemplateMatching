#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#include "utility.h"

/**
 * inverseaza culorile dintr-un pixel
 * @param p Pixel initial
 * @return Pixel inversat
 */
Pixel reversePixel(Pixel p) {
    /*
     * decodes pixel from little endian binary represantion B G R -> R G B
     */
    uchar temp = p.color.R;
    p.color.R = p.color.B;
    p.color.B = temp;
    return p;
}


void showPixel(Pixel p) {
    printf("%u %u %u\n", p.color.R, p.color.G, p.color.B);
}

/**
 * generareaza numere pseudo aleatoare
 * @param seed numar initial
 * @param size cate numere sa genereze
 * @return pointer la tablou de numere generate
 *
 */
uint *xorShift32(uint seed, uint size) {
    /*
     * return size random generated numbers,
     * first numbers randArr[0] is the seed
     */
    uint *randArr = calloc(size + 1, sizeof(uint));
    randArr[0] = seed;
    int i;
    for (i = 1; i <= size; i++) {
        seed ^= seed << (uint) 13;
        seed ^= seed >> (uint) 17;
        seed ^= seed << (uint) 5;
        randArr[i] = seed;
    }

    return randArr;
}

/**
 * citeste fisierul cu chei si le returneaza ca parametrie
 * @param keyPath drum spre fisier cu chei
 * @param key_1 adresa var cheia 1
 * @param key_2 adresa var cheia 2
 */
void loadSecretKeys(char *keyPath, uint *key_1, uint *key_2) {
    FILE *fin = fopen(keyPath, "r");
    if (fin == NULL) {
        printf("ERROR: can't open secret keys file\n");
        exit(0);
    }
    fscanf(fin, "%u %u", key_1, key_2);

    fclose(fin);
}

/**
 * incarca o imagine de pe disk in ram
 * construieste Image si liniarizeaza pixelii
 * apeleaza functia de a construi matricea de pixeli
 * @param path drum spre imagine
 * @return pointer la imagine in ram
 */
Image_ptr loadBMPImage(char *path) {

    FILE *fin = fopen(path, "rb");
    if (fin == NULL) {
        printf("ERROR: can't open the bmp image file = %s", path);
        exit(0);
    }
    Image_ptr img = malloc(sizeof(Image));

    fseek(fin, WIDTH_OFFSET, SEEK_SET);
    uint width, height;
    fread(&width, sizeof(uint), 1, fin);
    fread(&height, sizeof(uint), 1, fin);

    img->width = width;
    img->height = height;
    img->size = height * width;
    //TODO: reading twice, improve it
    fseek(fin, 0, SEEK_SET);
    fread(img->header, HEADER_SIZE, sizeof(uchar), fin);

    fseek(fin, PIXEL_ARRAY_BEGIN_OFFSET, SEEK_SET);
    Pixel *pixelArray = calloc(width * height, sizeof(Pixel));
    int i = 0, j = 0;
    // line must be divisor of 4,
    // else add padding to width
    int padding = 3 * width % 4;
    if (padding != 0)
        padding = 4 - padding;

    for (i = height - 1; i >= 0; i--) {
        //for(i = 0 ; i < height;i++){
        for (j = 0; j < width; j++) {
            Pixel currPixel;
            fread(&currPixel, sizeof(Pixel), 1, fin);
            currPixel = reversePixel(currPixel);
            pixelArray[i * width + j] = currPixel;
        }
        int dummyValue;
        if (padding != 0)
            fread(&dummyValue, sizeof(uchar), padding, fin);

    }
    img->pixels = pixelArray;
    buildSmartMatrixImg(img);
    fclose(fin);
    return img;
}

/**
 * salveaza imaginea din ram pe disk, folosind tabloul liniarizat al pixelilor
 * @param img pointer la imaginea din ram
 * @param path cale de salvare a imaginii
 */
void saveBMPImage(Image_ptr img, char *path) {
    FILE *fout = fopen(path, "wb");
    if (fout == NULL) {
        printf("ERROR: can't open file to save bmp image = %s\n", path);
        return;
    }
    fwrite(img->header, HEADER_SIZE, sizeof(uchar), fout);
    int padding = 3 * img->width % 4;
    if (padding != 0)
        padding = 4 - padding;

    int i, j;
    for (i = img->height - 1; i >= 0; i--) {
        //for (i = 0; i < img->height;i++){
        for (j = 0; j < img->width; j++) {
            Pixel currPixel = img->pixels[i * img->width + j];
            currPixel = reversePixel(currPixel);
            fwrite(&currPixel, sizeof(Pixel), 1, fout);
        }
        uchar dummyValue = 0;
        if (padding != 0)
            fwrite(&dummyValue, sizeof(uchar), (size_t) padding, fout);

    }
    fclose(fout);

}

/**
 * elibereaze memoria folosita de o imagine
 * @param img pointer la imaginea din ram
 */
void deconstructImage(Image_ptr img) {
    if (img->pixels) free(img->pixels);
    if (img->pixelsM) free(img->pixelsM);
    free(img);
}

/**
 * genereaza o permutare aleatorie
 * @param randomArr tablou de numere aleatorii
 * @param size marimea permutarii
 * @return pointer la tablou de permutare
 */
uint *getRandomPermutation(uint *randomArr, uint size) {

    uint *arr = calloc(size, sizeof(uint));
    uint i, k, temp;
    for (k = 0; k < size; k++)
        arr[k] = k;
    for (k = size - 1, i = 1; k >= 1; k--, i++) {
        uint rand = randomArr[i] % (k + 1);
        temp = arr[rand];
        arr[rand] = arr[k];
        arr[k] = temp;
    }
    return arr;

}

/**
 * permuta pixelii din imagine conform permutarii primite
 * @param img pointer la imaginea din ram
 * @param randomPermutation tablou cu permutare random
 */
void dePermutePixels(Image_ptr img, uint *randomPermutation) {

    Pixel *permutedPixels = calloc(img->size, sizeof(Pixel));
    uint size = img->size;
    uint i;
    for (i = 0; i < size; i++) {
        permutedPixels[i] = img->pixels[randomPermutation[i]];
    }
    free(img->pixels);
    img->pixels = permutedPixels;

}

/**
 * permuta invers pixelii conform permutarii primite
 * @param img pointer la imaginea din ram
 * @param randomPermutation tablou cu permutarea aleatorie
 */
void permutePixels(Image_ptr img, uint *randomPermutation) {
    Pixel *Pixels = calloc(img->size, sizeof(Pixel));
    uint size = img->size;
    uint i;
    for (i = 0; i < size; i++) {
        Pixels[randomPermutation[i]] = img->pixels[i];
    }
    free(img->pixels);
    img->pixels = Pixels;
}

/**
 *
 * @param nr numar fara semn pe 32 biti
 * @return echivalent ca pixel de 3 octeti, fara a lua in consideratie cel mai semnficiativ octet
 */
Pixel numberToPixel(uint nr) {
    PixelKey fakePixel;
    fakePixel.key = nr;
    Pixel pix;
    pix.colors[2] = fakePixel.colors[0];
    pix.colors[1] = fakePixel.colors[1];
    pix.colors[0] = fakePixel.colors[2];
    return pix;
}

/**
 * xor la tabloul de pixeli conform formulei
 * @param img pointer la imaginea din ram
 * @param randArr pointer la tablou cu nr random
 * @param key starting value
 */
void xorEncryptImage(Image_ptr img, uint *randArr, uint key) {
    Pixel starting_value = numberToPixel(key);
    uint index = img->size;
    Pixel randomNr = numberToPixel(randArr[index++]);
    xor(img->pixels[0], starting_value);
    xor(img->pixels[0], randomNr);
    int i;
    for (i = 1; i < img->size; i++, index++) {
        randomNr = numberToPixel(randArr[index]);
        xor(img->pixels[i], img->pixels[i - 1]);
        xor(img->pixels[i], randomNr);
    }

}

/**
 * xor inver la tabloul de pixeli conform formulei
 * @param img pointer la imaginea din ram
 * @param randArr pointer la tablou cu nr aleatorii
 * @param key starting value
 */
void xorDecryptImage(Image_ptr img, uint *randArr, uint key) {
    Pixel *pixels = calloc(img->size, sizeof(Pixel));
    memcpy(pixels, img->pixels, sizeof(Pixel) * img->size);
    Pixel starting_value = numberToPixel(key);
    uint index = img->size;
    Pixel randomNr = numberToPixel(randArr[index++]);
    xor(img->pixels[0], starting_value);
    xor(img->pixels[0], randomNr);
    int i;
    for (i = 1; i < img->size; i++, index++) {
        randomNr = numberToPixel(randArr[index]);
        xor(img->pixels[i], pixels[i - 1]);
        xor(img->pixels[i], randomNr);
    }
    free(pixels);

}

/**
 * cripteaza imaginea
 * @param initial_img_path drum spre imaginea initiala
 * @param encrypted_img_path drum spre imaginea care va fi criptata
 * @param secret_keys_path drum spre fisier cu chei
 */
void encryptImage(char *initial_img_path, char *encrypted_img_path, char *secret_keys_path) {
    Image_ptr img = loadBMPImage(initial_img_path);
    uint seed, starting_value;
    loadSecretKeys(secret_keys_path, &seed, &starting_value);
    uint *randomNumbers = xorShift32(seed, 2 * img->size - 1);
    uint *randomPermutation = getRandomPermutation(randomNumbers, img->size);
    permutePixels(img, randomPermutation);
    xorEncryptImage(img, randomNumbers, starting_value);
    saveBMPImage(img, encrypted_img_path);
    deconstructImage(img);
    free(randomNumbers);
    free(randomPermutation);

}

/**
 * decripteaza imaginea
 * @param ecrypted_img_path drum spre imaginea criptata
 * @param raw_img_path drum sprea imaginea care va fi decriptata
 * @param secret_keys_path drum spre fisier cu chei
 */
void decryptImage(char *ecrypted_img_path, char *raw_img_path, char *secret_keys_path) {
    Image_ptr img = loadBMPImage(ecrypted_img_path);
    uint seed, starting_value;
    loadSecretKeys(secret_keys_path, &seed, &starting_value);
    uint *randomNumbers = xorShift32(seed, 2 * img->size - 1);
    uint *randomPermutation = getRandomPermutation(randomNumbers, img->size);
    xorDecryptImage(img, randomNumbers, starting_value);
    dePermutePixels(img, randomPermutation);
    saveBMPImage(img, raw_img_path);
    deconstructImage(img);
    free(randomNumbers);
    free(randomPermutation);
}

/**
 * calculeaza chi squared test pentru un canal de culoare
 * @param img imaginea incarcata in ram
 * @param channel canalul de culoare
 * @return
 */
double chiSquaredTest(Image_ptr img, uchar channel) {
    uint *freq = calloc(256, sizeof(uint));

    int i, j;
    for (i = 0; i <= 255; i++) freq[i] = 0;
    double teoretic_freq = ((double) img->width * (double) img->height / 256);
    for (i = 0; i < img->height; i++)
        for (j = 0; j < img->width; j++) {
            freq[img->pixels[i * img->width + j].colors[channel]]++;
        }
    double res = 0;
    for (i = 0; i <= 255; i++)
        res += ((double) freq[i] - teoretic_freq) * ((double) freq[i] - teoretic_freq) / teoretic_freq;


    free(freq);
    return res;

}

/**
 * afiseaza rezultatul testul chi pentru o imagine pe 3 canale
 * @param path drumul spre imagine
 */
void chiTest(char *path) {
    Image_ptr img = loadBMPImage(path);
    char channels[] = {'R', 'G', 'B'};
    for (uchar channel = 0; channel < 3; channel++) {
        double res = chiSquaredTest(img, channel);
        printf("%c: %10.4lf\n", channels[channel], res);
    }
    deconstructImage(img);
}

/**
 * calculeaza intensitatea medie de gri
 * @param window fereastra in care se face calculul
 * @return intensitatea medie
 */
double avgGrayIntensity(Window window) {
    int i, j;
    int halfW = window.width / 2;
    int halfH = window.height / 2;
    double sum = 0;
    for (i = window.mid.x - halfH; i <= window.mid.x + halfH; i++) {
        for (j = window.mid.y - halfW; j <= window.mid.y + halfW; j++) {
            sum += window.M[i][j].gray;
        }
    }

    sum /= (double) window.size;
    return sum;

}
/**
 * @deprecated
 * constuieste matrice de pixeli din imagine copiind valorile
 * din tablou in matrice
 * @param img
 */
void buildMatrixImg(Image_ptr img) {
    img->pixelsM = calloc(img->height, sizeof(Pixel *));
    int i, j;
    for (i = 0; i < img->height; i++)
        img->pixelsM[i] = calloc(img->width, sizeof(Pixel));

    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->pixelsM[i][j] = img->pixels[i * img->width + j];
        }
    }

}

/**
 * construieste o matrice de pixeli din imaginea liniarizata
 * fara a copia, matricea si tabloul liniarizat arata spre aceeasi portiune de memorie
 * @param img imaginea incarcata in ram
 */
void buildSmartMatrixImg(Image_ptr img) {
    int len = sizeof(Pixel *) * img->height;
    img->pixelsM = malloc((size_t) len);
    int i;
    for (i = 0; i < img->height; i++) {
        img->pixelsM[i] = (img->pixels + img->width * i);
    }
}



/**
 * converteste imaginea in grayscale
 * @param img pointer la imaginea incarcata in ram
 */
void toGrayscale(Image_ptr img) {
    int i, j;
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            uchar aux =
                    img->pixelsM[i][j].colors[0] * 0.299 +
                    img->pixelsM[i][j].colors[1] * 0.587 +
                    img->pixelsM[i][j].colors[2] * 0.114;

            img->pixelsM[i][j].colors[0] =
            img->pixelsM[i][j].colors[1] =
            img->pixelsM[i][j].colors[2] = aux;
        }
    }
}

/**
 *
 * @param window fereastra din imagine
 * @param avg media culorii de gri pentru aceasta fereastra
 * @return deviatia standard a ferestrei
 */
double standardDeviation(Window window, double avg) {
    double sum = 0;
    int i, j;
    int halfW = window.width / 2;
    int halfH = window.height / 2;
    for (i = window.mid.x - halfH; i <= window.mid.x + halfH; i++) {
        for (j = window.mid.y - halfW; j <= window.mid.y + halfW; j++) {
            sum += (window.M[i][j].gray - avg) * ((window.M[i][j].gray - avg));
        }
    }

    sum /= (double) (window.size - 1);
    return sqrt(sum);
}

/**
 *
 * @param template imaginea template
 * @param target imaginea tinta
 * @return valoare intre -1 si 1, reprezinta cat de asemanatoare este fereastra din target cu template
 */
double crossCorrelation(Window template, Window target) {
    if (template.width != target.width || template.height != target.height) {
        printf("ERROR: trying to cross relate windows of different size");
        exit(-1);
    }
    double avgGrayTemplate = avgGrayIntensity(template);
    double avgGrayTarget = avgGrayIntensity(target);
    double standardDevTemplate = standardDeviation(template, avgGrayTemplate);
    double standardDevTarget = standardDeviation(target, avgGrayTarget);

    double res = 0;

    int i1, i2, j1, j2;
    // i1 j1 for template
    // i2 j2 for target
    int halfW = template.width / 2;
    int halfH = template.height / 2;
    for (i1 = template.mid.x - halfH, i2 = target.mid.x - halfH; i1 <= template.mid.x + halfH; i1++, i2++) {
        for (j1 = template.mid.y - halfW, j2 = target.mid.y - halfW; j1 <= template.mid.y + halfW; j1++, j2++) {
            res += ((double) 1 / (standardDevTarget * standardDevTemplate)) *
                   (target.M[i2][j2].gray - avgGrayTarget) * (template.M[i1][j1].gray - avgGrayTemplate);

        }
    }
    res /= (double) template.size;
    return res;
}

/**
 * @param number cifra
 * @return culoarea in functie de cifra a primit
 */
Pixel getColor(int number) {
    switch (number) {
        case 0:
            return COLOR_DIGIT_0;
        case 1:
            return COLOR_DIGIT_1;
        case 2:
            return COLOR_DIGIT_2;
        case 3:
            return COLOR_DIGIT_3;
        case 4:
            return COLOR_DIGIT_4;
        case 5:
            return COLOR_DIGIT_5;
        case 6:
            return COLOR_DIGIT_6;
        case 7:
            return COLOR_DIGIT_7;
        case 8:
            return COLOR_DIGIT_8;
        case 9:
            return COLOR_DIGIT_9;
        default:
            return COLOR_DEFAULT;
    }
}

/**
 * deseneaza o detectie in imaginea target
 * @param M pointer la matricea de pixeli a imaginii target
 * @param detection ferestra detectata
 */
void drawDetection(Pixel **M, Detection detection) {
    Pixel color = getColor(detection.type);
    int i;

    // draw up side
    for (i = detection.leftUpCorner.y; i <= detection.rightDownCorner.y; i++) {
        M[detection.leftUpCorner.x][i] = color;
    }
    // draw right side
    for (i = detection.leftUpCorner.x; i <= detection.rightDownCorner.x; i++) {
        M[i][detection.rightDownCorner.y] = color;
    }
    // draw down side
    for (i = detection.leftUpCorner.y; i <= detection.rightDownCorner.y; i++) {
        M[detection.rightDownCorner.x][i] = color;
    }
    // draw left side
    for (i = detection.leftUpCorner.x; i <= detection.rightDownCorner.x; i++) {
        M[i][detection.leftUpCorner.y] = color;
    }
}

/**
 *
 * @param targetWindow fereastra target
 * @param nr cifra detectata
 * @param accuracy acuratetea, valoare la sandtard deviation
 * @return detectia in formatul necesar - Detection
 */
Detection convertDetection(Window targetWindow, int nr, double accuracy) {
    Detection detection;
    detection.mid = targetWindow.mid;
    detection.leftUpCorner.x = targetWindow.mid.x - targetWindow.height / 2;
    detection.leftUpCorner.y = targetWindow.mid.y - targetWindow.width / 2;
    detection.rightDownCorner.x = targetWindow.mid.x + targetWindow.height / 2;
    detection.rightDownCorner.y = targetWindow.mid.y + targetWindow.width / 2;
    detection.type = nr;
    detection.accuracy = accuracy;
    detection.good = 1;

    return detection;
}

/**
 *
 * @param target pointer la imaginea target incarcata in ram
 * @param path_to_template calea spre imaginea template
 * @param nr cifra corespunzatoare
 * @param threshold limita minima de acuratete a detectiei
 * @return tablou de detectii in format de DetectionArr
 */
DetectionArr getDetections(Image_ptr target, char *path_to_template, int nr, double threshold) {
    Image_ptr templateImg = loadBMPImage(path_to_template);
    Detection *detArr = calloc(1, sizeof(Detection));
    int numberOfDetections = 0;
    double res;
    Window templateWindow = {{templateImg->height / 2, templateImg->width / 2}, templateImg->height, templateImg->width,
                             templateImg->width * templateImg->height, templateImg->pixelsM};
    Window targetWindow = {{templateImg->height / 2, templateImg->width / 2 + templateWindow.width},
                           templateImg->height, templateImg->width,
                           templateImg->width * templateImg->height, target->pixelsM};

    int i, j;
    for (i = templateImg->height / 2; i < target->height - templateImg->height / 2 - 1; i++) {
        for (j = templateImg->width / 2; j < target->width - templateImg->width / 2 - 1; j++) {
            targetWindow.mid.x = i;
            targetWindow.mid.y = j;

            res = crossCorrelation(templateWindow, targetWindow);

            if (res > threshold) {
                Detection detection = convertDetection(targetWindow, nr, res);
                numberOfDetections++;
                //TODO check if realloc returns NULL
                detArr = realloc(detArr, numberOfDetections * sizeof(Detection));
                detArr[numberOfDetections - 1] = detection;
            }

        }
    }
    deconstructImage(templateImg);
    DetectionArr result = {detArr, numberOfDetections};
    return result;
}

/**
 * copie tabloul source in destination si elibereaza source
 * @param destination
 * @param source
 */
void copyArray(DetectionArr *destination, DetectionArr *source) {
    destination->array = realloc(destination->array, (destination->size + source->size) * sizeof(Detection));
    memcpy(destination->array + destination->size, source->array, source->size * sizeof(Detection));
    destination->size += source->size;
    free(source->array);
}

/**
 *
 * @param img pointer la imaginea target incarcata in ram
 * @param paths caile spre imaginile template
 * @param nr numarul de imagini template
 * @return tablou cu toate detectiile ale imagnilor template gasite in imaginea target
 */
DetectionArr getAllDetections(Image_ptr img, char paths[][BUFFER_SIZE], int nr) {

    int k = 0;
    DetectionArr arr = getDetections(img, paths[0], 0, DEFAULT_THRESHOLD);
    DetectionArr tempArr;
    int i;
    for (i = 1; i < nr; i++) {
        tempArr = getDetections(img, paths[i], i, DEFAULT_THRESHOLD);
        copyArray(&arr, &tempArr);
    }

    return arr;

}

/**
 * functie pentru a compara detectii dupa acuratete
 * pentru a putea apela quicksort
 * @param a
 * @param b
 * @return
 */
int compareDetection(const void *a, const void *b) {
    Detection detA = *(Detection *) a;
    Detection detB = *(Detection *) b;

    if (detA.accuracy > detB.accuracy) return -1;
    if (detA.accuracy < detB.accuracy) return 1;
    return 0;
}

/**
 *
 * @param a
 * @param b
 * @return raportul de suprapunere
 */
double overlap(Detection a, Detection b) {

    if (intersect(a, b) == 0) return 0;
    Point l2 = a.leftUpCorner, r2 = a.rightDownCorner;
    Point l1 = b.leftUpCorner, r1 = b.rightDownCorner;

    int area1 = abs(l1.x - r1.x) *
                abs(l1.y - r1.y);

    int area2 = abs(l2.x - r2.x) *
                abs(l2.y - r2.y);


    int areaI = (min(r1.x, r2.x) -
                 max(l1.x, l2.x)) *
                (min(r1.y, r2.y) -
                 max(l1.y, l2.y));

    double overlapArea = (area1 + area2 - areaI);
    return 1.0 * overlapArea / (1.0 * area1 + area2 - overlapArea);

}

/**
 *
 * @param a
 * @param b
 * @return 1 daca se intersecteaza si 0 daca nu
 */

int intersect(Detection a, Detection b) {
    Point l2 = {a.leftUpCorner.x, a.rightDownCorner.y}, r2 = {a.rightDownCorner.x, a.leftUpCorner.y};

    Point l1 = {b.leftUpCorner.x, b.rightDownCorner.y}, r1 = {b.rightDownCorner.x, b.leftUpCorner.y};

    if (l1.x >= r2.x || l2.x >= r1.x)
        return 0;

    // If one rectangle is above other
    if (l1.y <= r2.y || l2.y <= r1.y)
        return 0;

    return 1;
}

/**
 *
 * @param detArr tablou cu toate detectiile sortate dupa acuratete
 * @return tablou de detectii cu cele non maxime sterse
 */
DetectionArr removeOverlapping(DetectionArr detArr) {
    DetectionArr newArr;
    newArr.size = 1;
    newArr.array = calloc((size_t) newArr.size, sizeof(Detection));
    int i;
    int k = 0;
    for (i = 0; i < detArr.size; i++) {
        if (detArr.array[i].good) {
            newArr.array[newArr.size - 1] = detArr.array[i];
            newArr.size++;
            newArr.array = realloc(newArr.array, sizeof(Detection) * newArr.size);
            int j;
            for (j = i + 1; j < detArr.size; j++) {
                if (overlap(detArr.array[i], detArr.array[j]) > OVERLAPPING_THRESHOLD)
                    detArr.array[j].good = 0;
            }
            k++;
        }

    }
    newArr.size--;
    free(detArr.array);
    return newArr;

}



int main() {

    FILE *fin = fopen(INPUT_FILE_PATH, "r");
    if (fin == NULL) {
        printf("ERROR: Can't open input file");
        exit(-1);
    }

    char raw_img_path[BUFFER_SIZE];
    char encrypted_img_path[BUFFER_SIZE];
    char secret_key_path[BUFFER_SIZE];

    // encrypt image and save to disk
    fgets(raw_img_path, BUFFER_SIZE, fin);
    fgets(encrypted_img_path, BUFFER_SIZE, fin);
    fgets(secret_key_path, BUFFER_SIZE, fin);

    raw_img_path[strlen(raw_img_path) - 1] = '\0';
    encrypted_img_path[strlen(encrypted_img_path) - 1] = '\0';
    secret_key_path[strlen(secret_key_path) - 1] = '\0';

    printf("Encrypting initial image = %s\n", raw_img_path);
    encryptImage(raw_img_path, encrypted_img_path, secret_key_path);
    printf("Initial image encrypted = %s\n", encrypted_img_path);

    //decrypt image and save to disk
    fgets(encrypted_img_path, BUFFER_SIZE, fin);
    fgets(raw_img_path, BUFFER_SIZE, fin);
    fgets(secret_key_path, BUFFER_SIZE, fin);

    raw_img_path[strlen(raw_img_path) - 1] = '\0';
    encrypted_img_path[strlen(encrypted_img_path) - 1] = '\0';
    secret_key_path[strlen(secret_key_path) - 1] = '\0';

    printf("Decrypting image = %s\n", encrypted_img_path);
    decryptImage(encrypted_img_path, raw_img_path, secret_key_path);
    printf("Image decrypted to = %s\n", raw_img_path);

    printf("Chi-squared for inital image = %s\n", raw_img_path);
    chiTest(raw_img_path);
    printf("Chi-squared for encrypted image = %s\n", encrypted_img_path);
    chiTest(encrypted_img_path);

    char target_image_path[BUFFER_SIZE];
    fgets(target_image_path, BUFFER_SIZE, fin);
    target_image_path[strlen(target_image_path) - 1] = '\0';


    int nr_templates = 0;
    fscanf(fin, "%d", &nr_templates);
    fgetc(fin);
    printf("Nr templates = %d\n", nr_templates);
    assert(nr_templates > 0 && "Can't have empty templates\n");
    char path_templates[nr_templates][BUFFER_SIZE];
    int i;
    for (i = 0; i < nr_templates; i++) {
        fgets(path_templates[i], BUFFER_SIZE, fin);
        path_templates[i][strlen(path_templates[i]) - 1] = '\0';
    }
    Image_ptr img = loadBMPImage(target_image_path);

    char path_target_draw_optim[BUFFER_SIZE];

    fgets(path_target_draw_optim, BUFFER_SIZE, fin);
    path_target_draw_optim[strlen(path_target_draw_optim)] = '\0';

    printf("Getting all detections...\n");

    DetectionArr detArr = getAllDetections(img, path_templates, nr_templates);

    qsort(detArr.array, (size_t) detArr.size, sizeof(Detection), compareDetection);
    printf("Removing non maxim detections.. \n");
    DetectionArr finalArr = removeOverlapping(detArr);
    for (i = 0; i < finalArr.size; i++)
        drawDetection(img->pixelsM, finalArr.array[i]);

    printf("Saving result to file = %s\n", path_target_draw_optim);
    saveBMPImage(img, path_target_draw_optim);

    free(finalArr.array);
    deconstructImage(img);

    fclose(fin);

    /*
     * That was easy
     */

    return 0;
}
