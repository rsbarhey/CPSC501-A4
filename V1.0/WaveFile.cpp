#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "WaveFile.h"

void WaveFile::ReadInput(char *filename)
{
    ifstream inFile( filename, ios::in | ios::binary);
    parseFile(inFile);

    signal = NULL;

    if ( bitsPerSample == 8 )
    {
        signalSize = dataSize;
        signal = new short[signalSize];
        for ( int i = 0; i < dataSize; i++ )
            signal[i] = (short)( (unsigned char) _data[i] );
    }
    else if ( bitsPerSample == 16 )
    {
        signalSize = dataSize / 2;
        signal = new short[signalSize];
        short val;
        for ( int i = 0; i < dataSize; i+=2 )
        {
            val = (short)( (unsigned char) _data[i] );
            val += (short)( (unsigned char) _data[i+1] ) * 256;
            signal[i/2] = val;
        }
    }
}

void WaveFile::parseFile(ifstream &in)
{
    // ChunkSize header
    in.seekg(4, ios::beg);
    in.read( (char*) &chunkSize, 4 );

    //SubChunk1Size header
    in.seekg(16, ios::beg);
    in.read( (char*) &subChunk1Size, 4 );

    // Format header
    in.seekg(20, ios::beg);
    in.read( (char*) &format, sizeof(short) );

    // NumChannels header
    in.read( (char*) &channels, sizeof(short) );

    // SampleRate header
    in.read( (char*) &sampleRate, sizeof(int) );

    // ByteRate header
    in.read( (char*) &byteRate, sizeof(int) );

    // BlockAlign header
    in.read( (char*) &blockAlign, sizeof(short) );

    // BitsPerSample header
    in.read( (char*) &bitsPerSample, sizeof(short) );

    // SubChunk2Size(data size) header
    in.seekg(40, ios::beg);
    in.read( (char*) &dataSize, sizeof(int) ); // read the size of the data


    // read the data chunk
    _data = new char[dataSize];
    in.seekg(44, ios::beg);
    in.read(_data, dataSize);

    in.close(); // close the input file
}
