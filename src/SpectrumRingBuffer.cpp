/*
 *  SpectrumRingBuffer.cpp
 *  BeatSeekerOF
 *
 *  Created by Andrew on 20/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "SpectrumRingBuffer.h"
#include "assert.h"



SpectrumRingBuffer::SpectrumRingBuffer(){
	
	bufferSize = 7;
	fftSize = 8;
		
	
}

SpectrumRingBuffer::~SpectrumRingBuffer(){
	
}

void SpectrumRingBuffer::init(int bSize, int fSize){
	//bufferSize is the number of magnitudes to hold
	printf("initialising buffersize to %i\n", bSize);
	bufferSize = bSize;
	fftSize = fSize;
	fftSizeOver2 = fftSize/2;
	
	for (int k = 0; k < bufferSize; k++){
		COMPLEX_SPLIT split;
		split.realp = (float *) malloc(fftSize * sizeof(float));
		split.imagp = (float *) malloc(fftSize * sizeof(float));
		zapSplit(split);
		/*for (int i = 0; i < fftSize; i++){
		 split.realp[i] = 0;
		 split.imagp[i] = 0;
		 }
		 */
		splitBuffer.push_back(split);
		
		std::vector<float> v;
		v.assign(fftSizeOver2, 0.0);
		magnitudes.push_back(v);
	}
	
	fft = new accFFT(fftSize, 0);//0 for float type
	
	window = new float[fftSize];
	setWindowHanning(window, fftSize);
	windowedBuffer = new float[fftSize];

}


void SpectrumRingBuffer::zapSplit(COMPLEX_SPLIT split){
	for (int i = 0; i < fftSize; i++){
		split.realp[i] = 0;
		split.imagp[i] = 0;
	}
}

void SpectrumRingBuffer::processBuffer(float* buffer, int numSamples){
	
	//printf("spec ring buf process called\n");
	
	assert (numSamples == fftSize);
	splitBuffer.pop_front();//take one off the front

	//window audio input for fft
	setFFTinput(buffer);
	

	COMPLEX_SPLIT newSplit;
	newSplit.realp = (float *) malloc(fftSize * sizeof(float));
	newSplit.imagp = (float *) malloc(fftSize * sizeof(float));
	zapSplit(newSplit);//probably dont need to as we will replace all values anyway	
	
	fft->forward_FFT_f(windowedBuffer, newSplit);

//	for (int i = 0; i < 15; i++){
//		printf("real [%i] %.3f, imag[%i] %.3f\n", i, newSplit.realp[i], i, newSplit.imagp[i]);
//	}
//	printSplit(newSplit);
	splitBuffer.push_back(newSplit);
//	for (int k = 14; k < 17; k++){
//		for (int i = 0; i < 15; i++){
//			printf("B[%i] real [%i] %.3f, imag[%i] %.3f\n", k, i, splitBuffer[k].realp[i], i, splitBuffer[k].imagp[i]);
//		}
//	}
	
	//convert the split version from FFT (Apple way of storing data) to a straightforward magnitude vector
	std::vector<float> v;
	getMagnitude(v, newSplit);
	
//	magnitudes.erase(magnitudes.begin());
	
	//add this to our deque of magnitudes
	magnitudes.pop_front();
	magnitudes.push_back(v);
	
}

void SpectrumRingBuffer::getMagnitude(std::vector<float>& v, COMPLEX_SPLIT& split){
	v.reserve(fftSizeOver2+1);
	v.assign(fftSizeOver2+1, 0.0);
	v[0] = fabs(split.realp[0]);
	
	//printf("SpecRing: mag[%i] %.3f\n", 0, v[0]);
	
	for (int i = 1; i < fftSizeOver2; i++){
		v[i] = sqrt((split.realp[i]*split.realp[i]) + (split.imagp[i]*split.imagp[i]));
	
	}
	
	v[fftSizeOver2] = fabs(split.imagp[0]);
	
	if (DEBUG_MODE){
		for (int i = 0; i < 6; i++)
			printf("SpecRing: mag[%i] %.5f\n", i, v[i]);
	}
	//printf("mag[%i] %.3f\n", fftSizeOver2, v[fftSizeOver2]);
}

void SpectrumRingBuffer::printBufferEnd(){
	for (int k = max(0, bufferSize-3); k < bufferSize; k++){
		for (int i = 0; i < 15; i++){
			printf("B[%i] real [%i] %.3f, imag[%i] %.3f\n", k, i, splitBuffer[k].realp[i], i, splitBuffer[k].imagp[i]);
		}
	}
}


void SpectrumRingBuffer::printSplit(COMPLEX_SPLIT& split){
	
	for (int i = 0; i < fftSize; i++){
			printf("real [%i] %.3f, imag[%i] %.4f\n", i, split.realp[i], i, split.imagp[i]);
	}
}

void SpectrumRingBuffer::setFFTinput(float* buffer){
	//copies the buffer to fft input with windowing
	for (int i = 0; i < fftSize; i++){
		windowedBuffer[i] = buffer[i] * window[i];
	}
	if (DEBUG_MODE){
		for (int i = 0; i < 10; i++)
			printf("spoecring: buffer[%i] %.5f\n", i, buffer[i]);
	}
}

void SpectrumRingBuffer::setWindowHanning(float* window, const int framesize){
	double N;		// variable to store framesize minus 1
	N = (double) (framesize-1);	// framesize minus 1
	
	// Hanning window calculation
	for (int n = 0;n < framesize;n++)
	{
		window[n] = 0.5*(1-cos(2*M_PI*(n/N)));
	}

}