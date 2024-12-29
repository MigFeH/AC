/*
* Main.cpp 
*
* Created on: Fall 2019 
*/ 

#include <stdio.h> 
#include <math.h> 
#include <CImg.h> 
#include <time.h> 
#include <immintrin.h>

using namespace cimg_library; 

// Data type for image components 

typedef double data_t; 

const char* SOURCE_IMG = "bailarina_wb1.bmp"; 
const char* DESTINATION_IMG = "bailarina2.bmp"; 

const uint NREPEAT = 23;

// Filter argument data type 

typedef struct { 

    data_t *pRsrc; // Pointers to the R, G and B components 
    data_t *pGsrc; 
    data_t *pBsrc; 
    data_t *pRdst; 
    data_t *pGdst; 
    data_t *pBdst; 
    uint pixelCount; // Size of the image in pixels 
} filter_args_t; 

 
 
#define VECTOR_SIZE       18 // Array size. Note: It is not a multiple of 8 
#define ITEMS_PER_PACKET (sizeof(__m256)/sizeof(data_t)) 

 
 

/***********************************************
*
* Algorithm. Image filter. 
* In this example, the algorithm is a components swap
*
* TO BE REPLACED BY YOUR ALGORITHM
*
* *********************************************/ 
void filter (filter_args_t args) { 
    const __m256d maxLimit = _mm256_set1_pd(255.0);

    // inicializa las variables para la suma
    __m256d sumR = _mm256_setzero_pd();
    __m256d sumG = _mm256_setzero_pd();
    __m256d sumB = _mm256_setzero_pd();

    // realizar las sumas de pixeles usando SIMD
    for (uint i = 0; i < args.pixelCount; i += ITEMS_PER_PACKET) {
        // carga los pixeles fuente en los registros SIMD
        __m256d rSrc = _mm256_loadu_pd(args.pRsrc + i);
        __m256d gSrc = _mm256_loadu_pd(args.pGsrc + i);
        __m256d bSrc = _mm256_loadu_pd(args.pBsrc + i);

        // suma todos los pixeles
        sumR = _mm256_add_pd(sumR, rSrc);
        sumG = _mm256_add_pd(sumG, gSrc);
        sumB = _mm256_add_pd(sumB, bSrc);
    }

    // calcula la media de pixeles
    __m256d avgR = _mm256_div_pd(sumR, _mm256_set1_pd(args.pixelCount));
    __m256d avgG = _mm256_div_pd(sumG, _mm256_set1_pd(args.pixelCount));
    __m256d avgB = _mm256_div_pd(sumB, _mm256_set1_pd(args.pixelCount));

    // Calcular los nuevos valores con el limite 255
    for (uint i = 0; i < args.pixelCount; i += ITEMS_PER_PACKET) {
        // carga los pixeles destinos a los registros SIMD
        __m256d rSrc = _mm256_loadu_pd(args.pRsrc + i);
        __m256d gSrc = _mm256_loadu_pd(args.pGsrc + i);
        __m256d bSrc = _mm256_loadu_pd(args.pBsrc + i);

        // calcula los nuevos valores usando los valores medios
        __m256d newR = _mm256_min_pd(_mm256_mul_pd(rSrc, _mm256_div_pd(avgG, avgR)), maxLimit);
        __m256d newG = _mm256_min_pd(gSrc, maxLimit);
        __m256d newB = _mm256_min_pd(_mm256_mul_pd(bSrc, _mm256_div_pd(avgG, avgB)), maxLimit);

        // guardo los resultados en los pixeles destino
        _mm256_storeu_pd(args.pRdst + i, newR);
        _mm256_storeu_pd(args.pGdst + i, newG);
        _mm256_storeu_pd(args.pBdst + i, newB);
    }
} 

int main() { 

    // Open file and object initialization 

    CImg<data_t> srcImage; 

    try 
    { 
        srcImage.load(SOURCE_IMG); // Open file 
    } 

    catch(const CImgIOException& e) // file not found 
    { 
        perror("load source image"); 
        exit(EXIT_FAILURE); 
    } 

    filter_args_t filter_args; 
    data_t *pDstImage; // Pointer to the new image pixels 

 
 

    /*************************************************** 
     * TODO: Variables initialization. 
     * - Prepare variables for the algorithm 
     * - This is not included in the benchmark time 
     */ 
    struct timespec tStart, tEnd; 
    double dElapsedTimeS; 


    srcImage.display(); // Displays the source image 
    uint width = srcImage.width();// Getting information from the source image 
    uint height = srcImage.height();     
    uint nComp = srcImage.spectrum();// source image number of components 
     // Common values for spectrum (number of image components): 
                // B&W images = 1 
                //  Normal color images = 3 (RGB) 
                // Special color images = 4 (RGB and alpha/transparency channel) 
 
 

    // Calculating image size in pixels 
    filter_args.pixelCount = width * height; 
     
    pDstImage = (data_t *)_mm_malloc(filter_args.pixelCount * nComp * sizeof(data_t), sizeof(__m256d));
    // Allocate memory space for destination image components 
    if (pDstImage == NULL) { 
        perror("Allocating destination image"); 
        exit(-2); 
    } 

 
 
    // Pointers to the componet arrays of the source image 
    filter_args.pRsrc = srcImage.data(); // pRcomp points to the R component array 
    filter_args.pGsrc = filter_args.pRsrc + filter_args.pixelCount; // pGcomp points to the G component array 
    filter_args.pBsrc = filter_args.pGsrc + filter_args.pixelCount; // pBcomp points to B component array 
     

    // Pointers to the RGB arrays of the destination image 
    filter_args.pRdst = pDstImage; 
    filter_args.pGdst = filter_args.pRdst + filter_args.pixelCount; 
    filter_args.pBdst = filter_args.pGdst + filter_args.pixelCount; 
 
 
 

    /*********************************************** 
     * TODO: Algorithm start. 
     * - Measure initial time 
     */ 
    if(clock_gettime(CLOCK_REALTIME, &tStart) < 0) // no se ejecuta correctamente 
    { 
        perror("clock_gettime"); 
        exit(EXIT_FAILURE); 
    } 
 
 

    /************************************************ 
     * Algorithm. 
     */ 
    for(uint i=0;i<NREPEAT;i++){
		filter(filter_args);
	}

 
 

    /*********************************************** 
     * TODO: End of the algorithm. 
     * - Measure the end time 
     * - Calculate the elapsed time 
     */ 
    if(clock_gettime(CLOCK_REALTIME, &tEnd) < 0) // no se ejecuta correctamente 
    { 
        perror("clock_gettime"); 
        exit(EXIT_FAILURE); 
    } 
     

    dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec); 
    dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9; 
    printf("Elapsed time : %f s.\n", dElapsedTimeS); 
    // Create a new image object with the calculated pixels 
    // In case of normal color images use nComp=3, 
    // In case of B/W images use nComp=1. 
    CImg<data_t> dstImage(pDstImage, width, height, 1, nComp); 
 
 

    // Store destination image in disk 
    dstImage.save(DESTINATION_IMG);  
 
 

    // Display destination image 
    dstImage.display(); 
     

    // Free memory 
    _mm_free(pDstImage);
 
 

    return 0; 

} 

 