/*
 *  MedianFilteringBuffer.h
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 22/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef MEDIAN_FILTERING_BUFFER
#define MEDIAN_FILTERING_BUFFER

#include "SpectrumRingBuffer.h"

class MedianFilteringBuffer : public SpectrumRingBuffer{
public:
	void init(int bSize, int fSize);
	void setUpMedianFilteringBuffers();
	void printMedianFiltering();//on the spectrum ring magnitudes

	double processBuffer(float* buffer, int numSamples);
	void deleteOldMedianValues();
	
	void findAndDelete(float& deleteMeVal, int& fftIndex);
	void addNewMedianValues();
	void addToSortedValues(float& newValue, int& fftIndex);
	
	void createPercussiveMedian();
	double createPercussiveMask();//returns the percussive detection function - sum of the percussive component
	
	float getHarmonicMedian(int& fftIndex);
	//vars
	int medianFFTsize;
	std::vector<std::vector<float> > sortedMagnitudes;
	
	int halfMedianSize;
	std::vector<float> percussiveMedian;
	std::vector<float> percussiveMask;
};
#endif