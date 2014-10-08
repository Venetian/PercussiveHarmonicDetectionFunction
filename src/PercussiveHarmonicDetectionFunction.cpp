/*
 *  PercussiveHarmonicDetectionFunction.cpp
 *  BeatSeekerOF
 *
 *  Created by Andrew on 19/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "PercussiveHarmonicDetectionFunction.h"

PercussiveHarmonicDetectionFunction::PercussiveHarmonicDetectionFunction(){
	/*
	printf("RING BUFFER SET\n");
	ringbuffer = new AudioRingBuffer(8,4);
	float rubbish[4] = {1,2.2,3.3,4};
	ringbuffer->addToBuffer(rubbish, 4);
	ringbuffer->printBuffer();
	float rubbish2[4] = {3.3,5.6,1.2,0.763};
	ringbuffer->addToBuffer(rubbish2, 4);
	ringbuffer->printBuffer();
	
	specRing = new SpectrumRingBuffer;
	specRing->init(3,8);
	
	COMPLEX_SPLIT split;
	split.realp = (float *) malloc(8 * sizeof(float));
	split.imagp = (float *) malloc(8 * sizeof(float));
	
	specRing->fft->forward_FFT_f(ringbuffer->audiobuffer, split);
	std::vector<float> v;
	
	specRing->getMagnitude(v, split);
	//specRing->printSplit(split);
	
	
	delete ringbuffer;
	*/
	
	medianFilterSize = 17;
	fftsize = 2048;
	hopsize = 512;
	
	ringbuffer = new AudioRingBuffer(fftsize, hopsize);
	//fft = new accFFT(fftsize, 0);//0 is for float
	specRing = new MedianFilteringBuffer();
	specRing->init(medianFilterSize, fftsize);
	
	//need to call this to set up the buffers
	specRing->setUpMedianFilteringBuffers();
	
	sampleCount = 0;//main count in audio samples
	
	percussiveDetectionFunction.reserve(2048);
	
	maximumValue = 1;
}


PercussiveHarmonicDetectionFunction::~PercussiveHarmonicDetectionFunction(){
	delete specRing;
	delete ringbuffer;
	//delete fft;
}


void PercussiveHarmonicDetectionFunction::reset(){
	percussiveDetectionFunction.clear();
}

bool PercussiveHarmonicDetectionFunction::processFrames(float* samples, int n){
	
	bool returnVal = false;
	
	sampleCount += n;
	if (DEBUG_MODE)
		printf("PHDF count %i\n", sampleCount);
	//this registers the end of the buffer in audio samples
	//either from beginning of file or live from reset point
	//maybe worth getting systemtime too for sync purposes (with other programs)
	//internally we use sample count
	
	if (ringbuffer->addToBuffer(samples, n)){
		//ringbuffer->printBuffer(20);
		
		//then we reached hopsize
		//do this next bit in spectrum ring?
		//i.e. create the complex to hold data
		//then do fft right on that
//		fft->forward_fft_f(samples, )
		double newDfVal = specRing->MedianFilteringBuffer::processBuffer(ringbuffer->audiobuffer, ringbuffer->buffersize);
		percussiveDetectionFunction.push_back(newDfVal);
		
		if (newDfVal > maximumValue)
			maximumValue = newDfVal;
		else
			maximumValue *= 0.9997;//slow decay
		
		
		returnVal = true;//as we had a new df value
		
		//specRing->doMedianFiltering();now included above
	}

	return returnVal;
}

double PercussiveHarmonicDetectionFunction::recentValue(){
	if (percussiveDetectionFunction.size() > 0){
		return percussiveDetectionFunction[percussiveDetectionFunction.size()-1];
	} else
		return 0;
}

double PercussiveHarmonicDetectionFunction::valueAtIndex(int& index){
	if (index >= 0 && index < percussiveDetectionFunction.size()){
		return percussiveDetectionFunction[index];
	} else
		return 0;
}
