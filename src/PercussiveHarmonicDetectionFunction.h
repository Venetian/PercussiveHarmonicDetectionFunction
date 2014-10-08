/*
 *  PercussiveHarmonicDetectionFunction.h
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 19/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef PERCUSSIVE_HARMONIC_DET_FN
#define PERCUSSIVE_HARMONIC_DET_FN

#include "AudioRingBuffer.h"
//#include "SpectrumRingBuffer.h"
#include "MedianFilteringBuffer.h"
#include "Debug.h"

struct PercussiveInfo {
	double dfVal;
	int sampleCount;
};

class PercussiveHarmonicDetectionFunction{
public:
	//get valiue back
	
	PercussiveHarmonicDetectionFunction();
	~PercussiveHarmonicDetectionFunction();
	
	void reset();
	
	//int processAudio(float* audioBuffer, int numSamples);
	bool processFrames(float* samples, int n);
	double recentValue();
	double valueAtIndex(int& index);
	
	AudioRingBuffer* ringbuffer;
	//pass audio to ringbuffer
	//when buffer at hopsize, get timestamp
	//do fft, store the spectrum complex values, store the mag spec for filtering
	//maybe beginning we set a load of zeros for these so we can go right away
	
	//want to get back a pdf value
	
	int fftsize, hopsize;
	int medianFilterSize;
	
//	SpectrumRingBuffer* specRing;
	
	MedianFilteringBuffer	*specRing;
	
	std::vector<double> percussiveDetectionFunction;
	float maximumValue;
	
protected:
	int sampleCount;

	//accFFT* fft;
};
#endif