/*
 *  SpectrumRingBuffer.h
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 20/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef SPECTRUM_RING_BUFFER
#define SPECTRUM_RING_BUFFER

#include "accFFT.h"
#include <deque.h>
#include <vector.h>

#include "Debug.h"

/*
Processes an audio buffer sequentially to get FFT
Holds the results in a circular buffer as COMPLEX_SPLIT
Also, holds the magnitudes in a circular buffer
Both the above are implemented as std::deque

*/


class SpectrumRingBuffer{
public:
	SpectrumRingBuffer();
	~SpectrumRingBuffer();
	void init(int bSize, int fSize);
	void zapSplit(COMPLEX_SPLIT split);
	void processBuffer(float* buffer, int numSamples);
	void printBufferEnd();
	void printSplit(COMPLEX_SPLIT& split);
	
	
	void setFFTinput(float* buffer);
	void setWindowHanning(float* window, const int framesize);
	
	//vars
	float* window;
	float* windowedBuffer;
	
	std::deque<COMPLEX_SPLIT> splitBuffer;
	void getMagnitude(std::vector<float>& v, COMPLEX_SPLIT& split);
	std::deque<std::vector<float> > magnitudes;//[%temporal index][%fft index]
	
	int bufferSize, fftSize, fftSizeOver2;
	
	accFFT* fft;
};
#endif