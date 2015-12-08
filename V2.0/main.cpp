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

void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
                                                 double sampleRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);

void manipulate(WaveFile *original, WaveFile *impulse, char *outputFile);

void convolve(float x[], int N, float h[], int M, float y[], int P);
void convolve(WaveFile *original, WaveFile *impulse, float y[], int P);

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
    WaveFile *inputSignal = new WaveFile();
    inputSignal->ReadInput(argv[1]);

    WaveFile *impulse = new WaveFile();
    impulse->ReadInput(argv[2]);

    cout << "Input Signal: " << inputSignal->signalSize << ", Impulse Size: " << impulse->signalSize << endl;

    manipulate(inputSignal, impulse, argv[3]);

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

void manipulate(WaveFile *original, WaveFile *impulse, char *outputFile)
{
    int output_size = original->signalSize + impulse->signalSize - 1;

    float *output_signal = new float[output_size];

    convolve(original, impulse, output_signal, output_size);

    int i;

    /*  Calculate the number of sound samples to create,
    rounding upwards if necessary  */
    int numberOfSamples = output_size;

    /*  Open a binary output file stream for writing */
    FILE *outputFileStream = fopen(outputFile, "wb");

    /*  Write the WAVE file header  */
    writeWaveFileHeader(original->channels, numberOfSamples, original->bitsPerSample,
                                            original->sampleRate, outputFileStream);

    float maxValInResult = -1;
    for (i = 0; i < numberOfSamples; i++ )
        if ( output_signal[i] > maxValInResult )
            maxValInResult = output_signal[i];

    float maxValInInput = -1;
    for (i = 0; i < numberOfSamples; i++ )
        if ( original->signal[i] > maxValInInput )
            maxValInInput = original->signal[i];

    for (i = 0; i < numberOfSamples; i++ )
        fwriteShortLSB((short)(output_signal[i] / maxValInResult * maxValInInput), outputFileStream);


    /*  Close the output file stream  */
    fclose(outputFileStream);
}

/******************************************************************************
 *
 *       function:       writeWaveFileHeader
 *
 *       purpose:        Writes the header in WAVE format to the output file.
 *
 *		arguments:		channels:  the number of sound output channels
 *						numberSamples:  the number of sound samples
 *                       outputRate:  the sample rate
 *						outputFile:  the output file stream to write to
 *
 *       internal
 *       functions:      fwriteIntLSB, fwriteShortLSB
 *
 *       library
 *       functions:      ceil, fputs
 *
 ******************************************************************************/
//------------------------------------------------------------------------
// This method belongs to WaveFile if time permits it has to be refactored
//------------------------------------------------------------------------
void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample,
                                                 double sampleRate, FILE *outputFile)
{
    /*  Calculate the total number of bytes for the data chunk  */
    int dataChunkSize = channels * numberSamples * (bitsPerSample / 8);

    /*  Calculate the total number of bytes for the form size  */
    int formSize = 36 + dataChunkSize;

    /*  Calculate the total number of bytes per frame  */
    short int frameSize = channels * (bitsPerSample / 8);

    /*  Calculate the byte rate  */
    int bytesPerSecond = (int)ceil(sampleRate * frameSize);

    /*  Write header to file  */
    /*  Form container identifier  */
    fputs("RIFF", outputFile);

    /*  Form size  */
    fwriteIntLSB(formSize, outputFile);

    /*  Form container type  */
    fputs("WAVE", outputFile);

    /*  Format chunk identifier (Note: space after 't' needed)  */
    fputs("fmt ", outputFile);

    /*  Format chunk size (fixed at 16 bytes)  */
    fwriteIntLSB(16, outputFile);

    /*  Compression code:  1 = PCM  */
    fwriteShortLSB(1, outputFile);

    /*  Number of channels  */
    fwriteShortLSB((short)channels, outputFile);

    /*  Output Sample Rate  */
    fwriteIntLSB((int)sampleRate, outputFile);

    /*  Bytes per second  */
    fwriteIntLSB(bytesPerSecond, outputFile);

    /*  Block alignment (frame size)  */
    fwriteShortLSB(frameSize, outputFile);

    /*  Bits per sample  */
    fwriteShortLSB(bitsPerSample, outputFile);

    /*  Sound Data chunk identifier  */
    fputs("data", outputFile);

    /*  Chunk size  */
    fwriteIntLSB(dataChunkSize, outputFile);
}



/******************************************************************************
 *
 *       function:       fwriteIntLSB
 *
 *       purpose:        Writes a 4-byte integer to the file stream, starting
 *                       with the least significant byte (i.e. writes the int
 *                       in little-endian form).  This routine will work on both
 *                       big-endian and little-endian architectures.
 *
 *       internal
 *       functions:      none
 *
 *       library
 *       functions:      fwrite
 *
 ******************************************************************************/

size_t fwriteIntLSB(int data, FILE *stream)
{
    unsigned char array[4];

    array[3] = (unsigned char)((data >> 24) & 0xFF);
    array[2] = (unsigned char)((data >> 16) & 0xFF);
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 4, stream);
}



/******************************************************************************
 *
 *       function:       fwriteShortLSB
 *
 *       purpose:        Writes a 2-byte integer to the file stream, starting
 *                       with the least significant byte (i.e. writes the int
 *                       in little-endian form).  This routine will work on both
 *                       big-endian and little-endian architectures.
 *
 *       internal
 *       functions:      none
 *
 *       library
 *       functions:      fwrite
 *
 ******************************************************************************/

size_t fwriteShortLSB(short int data, FILE *stream)
{
    unsigned char array[2];

    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 2, stream);
}


void convolve(float x[], int N, float h[], int M, float y[], int P) {
    int n, m;
    /*	Make sure the output buffer is the right size: P = N + M - 1	*/
    if (P != (N + M - 1)) {
        printf("Output signal vector is the wrong size\n"); printf("It is %-d, but should be %-d\n", P, (N + M - 1)); printf("Aborting convolution\n"); return;
    }
    /* Clear the output buffer y[] to all zero values */
    for (n = 0; n < P; n++)
        y[n] = 0.0;
    /* Do the convolution */ /* Outer loop: process each input value x[n] in turn */
        /* Inner loop: process x[n] with each sample of h[] */
    for (n = 0; n < N; n++) {
        for (m = 0; m < M; m++)
        {
            y[n+m] += (x[n] * h[m]);
        }
    }
}

void convolve(WaveFile *original, WaveFile *impulse, float y[], int P) {
    float *h = new float[impulse->signalSize];
    for ( int i = 0; i < impulse->signalSize; i++ )
        h[i] = (float)impulse->signal[i];

    float *x = new float[original->signalSize];
    for ( int i = 0; i < original->signalSize; i++ )
        x[i] = (float)original->signal[i];

    convolve(x, original->signalSize, h, impulse->signalSize, y, P);
}
