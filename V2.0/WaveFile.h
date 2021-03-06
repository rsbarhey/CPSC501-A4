#ifndef WAVEFILE
#define WAVEFILE

using namespace std;

class WaveFile
{
public:
        float* ReadInput(char *filename, float *signal, int *Thesize);
        void writeWaveFile(char* fileName, int numSamples, float *size);
        
        char *format;
        int chunkSize;
        char *chunkID;
        char *subChunk1ID;

        int16_t blockAlign;
        int16_t bitsPerSample;
        int16_t audioFormat;
        int16_t numChannels;
        int sampleRate;
        char *subChunk2ID;
        int dataSize;
        short* fileData;
        int subChunk1Size;
        int byteRate; 
};

#endif
