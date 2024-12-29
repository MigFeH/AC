/*
 * Main.cpp
 *
 *  Created on: Fall 2019
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <CImg.h>
#include <time.h>

using namespace cimg_library;

// Data type for image components
// FIXME: Change this type according to your group assignment
typedef long data_t;

const char* SOURCE_IMG = "bailarina_wb1.bmp"; 
const char* DESTINATION_IMG = "bailarina2.bmp";

const uint NREPEAT = 23;
const int NTHREADS = 10; // number of the threads of the program

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


void* filter (void* arg /* puntero void que apunta a la dirección de memoria con los datos de la estructura correspondiente al hilo i */) /* retorna puntero void y se le pasa por parámetro un puntero void para que se cuadren con los parámetros de la funcion que crea los hilos */ {
	filter_args_t* args = (filter_args_t*) arg; // hacemos casting del puntero void que le pasamos a filter por parámetro a un puntero a la estructura filter_args_t 

	double rm = 0.0; // local variable for red average
	double gm = 0.0; // local variable for green average
	double bm = 0.0; // local variable for blue average
		
	for (uint i = 0; i < args->/* accedemos al contenido de un valor de una estructura apuntada por un puntero */pixelCount; i++)
	{
		rm += *(args->pRsrc + i); // process the total value of red pixels
		gm += *(args->pGsrc + i); // process the total value of green pixels
		bm += *(args->pBsrc + i); // process the total value of blue pixels
	}
	double maxValue = 255.0; // set the max value for the red pixels of destination's image
	
	rm = rm/args->pixelCount; // set the average red value of the source image 
	gm = gm/args->pixelCount; // set the average green value of the source image
	bm = bm/args->pixelCount; // set the average blue value of the source image

	double auxR = gm / rm;
	double auxB = gm / bm;

	for (uint i = 0; i < args->pixelCount; i++)
	{
		// ----------- RED ----------- //
		if(*(args->pRsrc + i) * auxR > maxValue)
		{
			*(args->pRdst + i) = maxValue;
		} else
		{
			*(args->pRdst + i) = *(args->pRsrc + i) * auxR;
		}

		// ----------- GREEN ----------- //
		if(*(args->pGsrc + i) > maxValue)
		{
			*(args->pGdst + i) = maxValue;
		} else
		{
			*(args->pGdst + i) = *(args->pGsrc + i);
		}

		// ----------- BLUE ----------- //
		if(*(args->pBsrc + i) * auxB > maxValue)
		{
			 *(args->pBdst + i) = maxValue;
		} else
		{
			*(args->pBdst + i) = *(args->pBsrc + i) * auxB;	
		}
	}

	return NULL; // retornamos puntero void null porque no retornamos nada en el filter, solo procesamos pixeles de una imagen o zona de esta
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

	/***************************************************
	 * TODO: Variables initialization.
	 *   - Prepare variables for the algorithm
	 *   - This is not included in the benchmark time
	 */
	filter_args_t filter_args;
	data_t *pDstImage; // Pointer to the new image pixels

	struct timespec tStart, tEnd;
	double dElapsedTimeS;

	srcImage.display(); // Displays the source image
	uint width = srcImage.width();// Getting information from the source image
	uint height = srcImage.height();	
	uint nComp = srcImage.spectrum();// source image number of components
	         // Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)

	// Calculating image size in pixels
	filter_args.pixelCount = width * height;
	uint pixelCountPerThread = filter_args.pixelCount / NTHREADS; /* numero de pixeles a ser trabajados por cada hilo */


	// Allocate memory space for destination image components
	pDstImage = (data_t *) malloc (filter_args.pixelCount * nComp * sizeof(data_t));
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
	 *   - Measure initial time
	 */
	if(clock_gettime(CLOCK_REALTIME, &tStart) < 0) // no se ejecuta correctamente
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}


	/************************************************
	 * TODO: Algorithm.
	 */ 
	uint i;
	for(i = 0; i < NREPEAT; i++)
	{
    	pthread_t thread[NTHREADS]; // array con los 10 hilos con los que el programa trabaja
    	uint i;
    	filter_args_t a[NTHREADS]; // Un hilo pasará por parametro todo lo de la estructura array con un puntero a la estructura de los parámetros de la funcion filter para cada hilo


    	// Thread creation
    	for (i = 0; i < NTHREADS; i++)
    	{
	    	a[i].pRsrc = filter_args.pRsrc + i * pixelCountPerThread; // xi pixeles rojos fuente para el hilo i
			a[i].pGsrc = filter_args.pGsrc + i * pixelCountPerThread; // xi pixeles verdes fuente para el hilo i
     	   	a[i].pBsrc = filter_args.pBsrc + i * pixelCountPerThread; // xi pixeles azules fuente para el hilo i

        	a[i].pRdst = filter_args.pRdst + i * pixelCountPerThread; // xi pixeles rojos destino para el hilo i
        	a[i].pGdst = filter_args.pGdst + i * pixelCountPerThread; // xi pixeles verdes destino para el hilo i
        	a[i].pBdst = filter_args.pBdst + i * pixelCountPerThread; // xi pixeles azules destino para el hilo i

			a[i].pixelCount = pixelCountPerThread; // el hilo i trabaja con x pixeles

        	if (pthread_create(&thread[i], NULL, filter, &a[i]) != 0) // hilo no se crea correctamente...
        	{
            	fprintf(stderr, "ERROR: Creating thread %d\n", i);
            	return EXIT_FAILURE;
        	}

    	}

        printf("Main thread, message %d\n", i);

    	printf("Main thread waiting...\n");
    	for (i = 0; i < NTHREADS; i++)
    	{
        	pthread_join(thread[i], NULL); // hilo principal espera a que terminen todos los hilos
    	}
    	printf("Main thread finished.\n");
	}


	/***********************************************
	 * TODO: End of the algorithm.
	 *   - Measure the end time
	 *   - Calculate the elapsed time
	 */
	if(clock_gettime(CLOCK_REALTIME, &tEnd) < 0) // no se ejecuta correctamente
	{
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}

	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("Elapsed time    : %f s.\n", dElapsedTimeS);
	// Create a new image object with the calculated pixels
	// In case of normal color images use nComp=3,
	// In case of B/W images use nComp=1.
	CImg<data_t> dstImage(pDstImage, width, height, 1, nComp);

	// Store destination image in disk
	dstImage.save(DESTINATION_IMG); 

	// Display destination image
	dstImage.display();
	
	// Free memory
	free(pDstImage);

	return 0;
}