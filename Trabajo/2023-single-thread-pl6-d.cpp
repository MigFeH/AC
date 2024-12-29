/*
 * Main.cpp
 *
 *  Created on: Fall 2019
 */

#include <stdio.h>
#include <math.h>
#include <CImg.h>
#include <time.h>

using namespace cimg_library;

const uint NUMBER_OF_REPEATS = 23;

// Data type for image components
typedef double data_t;

const char* SOURCE_IMG      = "bailarina_wb1.bmp";
const char* DESTINATION_IMG = "bailarina2.bmp";

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

/***********************************************
 * 
 * Algorithm. Image filter.
 * In this example, the algorithm is a components swap
 *
 * TO BE REPLACED BY YOUR ALGORITHM
 * 		
 * *********************************************/
void filter (filter_args_t args) {
		double rm = 0.0; // local variable for red average
		double gm = 0.0; // local variable for green average
		double bm = 0.0; // local variable for blue average
		
		for (uint i = 0; i < args.pixelCount; i++)
		{
			rm += *(args.pRsrc + i); // process the total value of red pixels
			gm += *(args.pGsrc + i); // process the total value of green pixels
			bm += *(args.pBsrc + i); // process the total value of blue pixels
		}
		double maxValue = 255.0; // set the max value for the red pixels of destination's image
	
		rm = rm/args.pixelCount; // set the average red value of the source image 
		gm = gm/args.pixelCount; // set the average green value of the source image
		bm = bm/args.pixelCount; // set the average blue value of the source image

		double auxR = gm / rm;
		double auxB = gm / bm;

		for (uint i = 0; i < args.pixelCount; i++)
		{
			// ----------- RED ----------- //
			if(*(args.pRsrc + i) * auxR > maxValue)
			{
				*(args.pRdst + i) = maxValue;
			} else
			{
				*(args.pRdst + i) = *(args.pRsrc + i) * auxR;
			}

			// ----------- GREEN ----------- //
			if(*(args.pGsrc + i) > maxValue)
			{
				*(args.pGdst + i) = maxValue;
			} else
			{
				*(args.pGdst + i) = *(args.pGsrc + i);
			}

			// ----------- BLUE ----------- //
			if(*(args.pBsrc + i) * auxB > maxValue)
			{
				 *(args.pBdst + i) = maxValue;
			} else
			{
				*(args.pBdst + i) = *(args.pBsrc + i) * auxB;	
			}
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
	 *   - Prepare variables for the algorithm
	 *   - This is not included in the benchmark time
	 */
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
	 * Algorithm.
	 */
	uint i;
	for(i = 0; i < NUMBER_OF_REPEATS; i++)
	{
		filter(filter_args);
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
