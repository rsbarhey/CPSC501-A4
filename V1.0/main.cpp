/******************************************************************************
 *
 *     Program:       testtone
 *
 *     Description:   This program generates a two-second 440 Hz test tone and
 *                    writes it to a .wav file.  The sound file has 16-bit
 *                    samples at a sample rate of 44.1 kHz, and is monophonic.
 *
 *     Author:        Leonard Manzara
 *
 *     Date:          November 21, 2009
 *
 ******************************************************************************/


/*  HEADER FILES  ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include "WaveFile.h"
using namespace std;

#define DEBUG_MODE

/*  CONSTANTS  ***************************************************************/
#define PI					3.14159265358979

/*  Test tone frequency in Hz  */
#define FREQUENCY			440.0

/*  Test tone duration in seconds  */
#define DURATION			2.0

/*  Standard sample rate in Hz  */
#define SAMPLE_RATE			44100.0

/*  Standard sample size in bits  */
#define BITS_PER_SAMPLE		16

/*  Standard sample size in bytes  */
#define BYTES_PER_SAMPLE	(BITS_PER_SAMPLE/8)

/*  Number of channels  */
#define MONOPHONIC			1
#define STEREOPHONIC		2


/*  FUNCTION PROTOTYPES  *****************************************************/
bool checkFileExist(char* fileName);
void convolve(float x[], int N, float h[], int M, float y[], int P);

/******************************************************************************
 *
 *	function:	main
 *
 *	purpose:	Creates the test tone and writes it to the
 *               specified .wav file.
 *
 *   arguments:	argv[1]:  the filename for the output .wav file
 *
 ******************************************************************************/

int main (int argc, char *argv[])
{
    if(argc < 4 || argc > 4)
    {
        cout << "Error using the program" << endl;
        cout << "please use ./app [InputAudio].wav [ImpulseResponse].wav [OutputAudio].wav" << endl;
        return 0;
    }


    /*  Create the sine wave test tone, using the specified
        frequency, duration, and number of channels, writing to
        a .wav file with the specified output filename  */
    if(!checkFileExist(argv[1]) || !checkFileExist(argv[2]))
    {
        cout << "Error: one or both of input and impulse files don't exist " << endl;
        return 0;
    }
    WaveFile *inputWave = new WaveFile();
    WaveFile *impulseWave = new WaveFile();

    float* inputData;
    int inputSize;
    inputData = inputWave->ReadInput(argv[1], inputData, &inputSize);
    
    float* impulseData;
    int impulseSize;
    impulseData = impulseWave->ReadInput(argv[2], impulseData, &impulseSize);
    
    int outputSize = inputSize + impulseSize - 1;
    float* outputData = new float[outputSize];
    
    cout << "Convolving..." << endl;
    convolve(inputData, inputSize, impulseData, impulseSize, outputData, outputSize);
    inputWave->writeWaveFile(argv[3], outputSize, outputData);

    /*  End of program  */
    return 0;
}

bool checkFileExist(char* fileName)
{
    ifstream f(fileName);
    if (f.good()) {
        f.close();
        return true;
    }
    else {
        f.close();
        return false;
    }
}

void convolve(float x[], int N, float h[], int M, float y[], int P) {
	int n, m;

	for (n = 0; n < P; n++)
		y[n] = 0.0;

	for (n = 0; n < N; n++) {
		for (m = 0; m < M; m++)
		{
			y[n+m] += x[n] * h[m];
		}
	}

}