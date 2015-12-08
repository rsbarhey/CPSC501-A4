#ifndef WAVEFILE
#define WAVEFILE

using namespace std;

class WaveFile
{
public:
        int 	chunkSize;
        int	subChunk1Size;
        short 	format;
        short 	channels;
        int   	sampleRate;
        int   	byteRate;
        short 	blockAlign;
        short 	bitsPerSample;
        int	dataSize;

        short*  signal;
        int     signalSize;

public:
        void ReadInput(char *filename);

private:
        void parseFile(ifstream& in);
        char* 	_data;
};

#endif
