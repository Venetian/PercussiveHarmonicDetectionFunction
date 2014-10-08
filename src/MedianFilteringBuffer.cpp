/*
 *  MedianFilteringBuffer.cpp
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 22/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "MedianFilteringBuffer.h"

#include <assert.h>

//#define DBL_EPSILON 2.2204460492503131e-16


bool printOn = false;

void MedianFilteringBuffer::init(int bSize, int fSize){
	printf("MFB: Median filtering init called\n");
	SpectrumRingBuffer::init(bSize, fSize);

	halfMedianSize = (bSize-1)/2;
}

void MedianFilteringBuffer::setUpMedianFilteringBuffers(){
	medianFFTsize = 64;
	for (int i = 0; i < medianFFTsize; i++){
		std::vector<float> v;
		v.assign(bufferSize, 0.0);
		sortedMagnitudes.push_back(v);
	}
	percussiveMedian.reserve(medianFFTsize);//reserve memory for it
	percussiveMask.reserve(medianFFTsize);
	
}	

void MedianFilteringBuffer::printMedianFiltering(){
	/*
	for (int k = 0; k < bufferSize; k++){
		for (int i = 0; i < 3; i++){
			printf("MFB: Mag[%i][%i] %.5f\n", k, i, magnitudes[k][i]);
		//checking copying is okay
		
		}
		
	}
	*/
	if (printOn){
		for (int i = 0; i < 7; i++){
		//	for (int k = 0; k < bufferSize; k++){
		//		printf("MFB: sort[%i][%i] %.5f\n", i, k, sortedMagnitudes[i][k]);
		//	}
			printf("MFB: harmonic median[%i] %f\n", i, sortedMagnitudes[i][halfMedianSize]);
		}
	}
}


double MedianFilteringBuffer::processBuffer(float* buffer, int numSamples){
	//printf("median buffer process called\n");
	deleteOldMedianValues();
	SpectrumRingBuffer::processBuffer(buffer, numSamples);//std to add new mag vector at end
	//printf("addvals\n");
	addNewMedianValues();
	printMedianFiltering();
	
	createPercussiveMedian();
	double dfVal = createPercussiveMask();
	return dfVal;
}

void MedianFilteringBuffer::deleteOldMedianValues(){
	//needs to happen before we add the new values on the end
	for (int fftIndex = 0; fftIndex < medianFFTsize; fftIndex++){
		float deleteVal = magnitudes[0][fftIndex];
		//now find this val in the correct fftIndex of sorted and delete
		findAndDelete(deleteVal, fftIndex);
	}
	
}

void MedianFilteringBuffer::addNewMedianValues(){
	//we have just processed buffer so new vals are in place
	for (int fftIndex = 0; fftIndex < medianFFTsize; fftIndex++){
		float newValue = magnitudes[magnitudes.size()-1][fftIndex];
		//printf("add new value %f to %i\n", newValue, fftIndex);
		addToSortedValues(newValue, fftIndex);
	}
}

void MedianFilteringBuffer::findAndDelete(float& deleteMeVal, int& fftIndex){
	int tmpIndex = 0;
	while (tmpIndex < bufferSize-1 && sortedMagnitudes[fftIndex][tmpIndex] != deleteMeVal)
		tmpIndex++;
	
	assert (tmpIndex < bufferSize);	
	
	sortedMagnitudes[fftIndex].erase(sortedMagnitudes[fftIndex].begin()+tmpIndex);

}

void MedianFilteringBuffer::addToSortedValues(float& newValue, int& fftIndex){
	//printf("size of sorted mags, index %i is %i\n", fftIndex, (int)sortedMagnitudes[fftIndex].size());
	
	int tmpIndex = 0;
	while (tmpIndex < bufferSize-1 && newValue > sortedMagnitudes[fftIndex][tmpIndex])
		tmpIndex++;
	
	assert (tmpIndex < bufferSize);
	
	sortedMagnitudes[fftIndex].insert(sortedMagnitudes[fftIndex].begin()+tmpIndex, newValue);
	
	assert ((int)sortedMagnitudes[fftIndex].size() == bufferSize);

}



void MedianFilteringBuffer::createPercussiveMedian(){
	//going vertically upwards
	//the result is the percussive median for our current strip
	
	percussiveMedian.clear();
	
	std::vector<float> stripValues;
	std::vector<float> sortedStripValues;
	
	int tmpIndex = 0;
	for (int fftIndex = 0; fftIndex < bufferSize; fftIndex++){
		//for early values we need odd median [0] is just [0]
		//for [1], median of [0],[1]and[2]
		//for [2], median of [0]-[4] etc
		
		tmpIndex = 0;
		while (tmpIndex < (int)sortedStripValues.size() && magnitudes[halfMedianSize][fftIndex] > sortedStripValues[tmpIndex])
			tmpIndex++;
		
		
		stripValues.push_back(magnitudes[halfMedianSize][fftIndex]);	   
		sortedStripValues.insert(sortedStripValues.begin()+tmpIndex, magnitudes[halfMedianSize][fftIndex]);
		
		if (stripValues.size() % 2 == 1){
			percussiveMedian.push_back(sortedStripValues[(sortedStripValues.size()-1)/2]);
		}
	}
	
	assert ((int)stripValues.size() == bufferSize);
	assert ((int)sortedStripValues.size() == bufferSize);
	
	//printf("size of circ med vals %i magsize %i\n", (int)circularMagnitudeValues.size(), magSize);
	
	for (int fftNewIndex = bufferSize; (int)percussiveMedian.size() < medianFFTsize; fftNewIndex++){
		//need to delete old val from sorted array
		tmpIndex = 0;
		while (sortedStripValues[tmpIndex] != stripValues[0])
			tmpIndex++;
		
		assert (tmpIndex < bufferSize);
		
		//printf("fft index %i\n", fftNewIndex);
		//get rid of old
		sortedStripValues.erase(sortedStripValues.begin()+tmpIndex);
		stripValues.erase(stripValues.begin());
		
		//assert (fftNewIndex < circularMagnitudeValues.size());
		//now we add the new value
		double tmp_val = 0;
		//printf("size is %i\n", (int)magnitudes[halfMedianSize].size());
		if (fftNewIndex < (int)magnitudes[halfMedianSize].size())
			tmp_val = magnitudes[halfMedianSize][fftNewIndex];
		else 
			printf("MFB: out of range\n");
		
		stripValues.push_back(tmp_val);
		
		tmpIndex = 0;
		while (tmpIndex < (int)sortedStripValues.size() && tmp_val > sortedStripValues[tmpIndex])
			tmpIndex++;
		
		assert (tmpIndex < bufferSize);
		sortedStripValues.insert(sortedStripValues.begin()+tmpIndex,tmp_val);
		
		percussiveMedian.push_back(sortedStripValues[(sortedStripValues.size()-1)/2]);
	}
	
	//WORKS FINE SINCE THE CIRCULARFFT IS LESS THAN FULL VALUES AVAILABLE
	//OTHERWISE WE COULD USE SIMILAR TRICK TO AT BEGINNING (TAKING MEDIAN FOR ODD VALS) AT TOP
	
	
	if (printOn){
		for (int i = 0; i < std::min(1200,medianFFTsize); i++){
			printf("MFB: percMedian FFTraw [%i] %.5f median %.5f\n", i, magnitudes[halfMedianSize][i], percussiveMedian[i]);
		}
	
		for (int i = medianFFTsize; i < medianFFTsize+12; i++)
			printf("MFB: mag[%i] %f\n", i, magnitudes[halfMedianSize][i]);
	}
}




double MedianFilteringBuffer::createPercussiveMask(){
	//involves both the harmonic and percussive medians
	double pdfVal = 0;
	double kickVal = 0;
	double snareVal = 0;
	double cymbalVal = 0;
	
	percussiveMask.clear();
	
//	DoubleVector v;
//	DoubleVector harmSpecFrame;
	
//	std::vector<float> pMask;
//	std::vector<float> hMask;
	
	double percussiveDifferenceVal = 0;
	for (int fftIndex = 0; fftIndex < medianFFTsize; fftIndex++){
		
		double maskVal = percussiveMedian[fftIndex]*percussiveMedian[fftIndex];
		maskVal /= (maskVal+(sortedMagnitudes[fftIndex][halfMedianSize]*sortedMagnitudes[fftIndex][halfMedianSize])+DBL_EPSILON);
		
		if (printOn && fftIndex < 12)
			printf("MFB: harmonic[%i] %f, perc[%i] %f\n", fftIndex, sortedMagnitudes[fftIndex][halfMedianSize], fftIndex, percussiveMedian[fftIndex]);
		
		//v.push_back(percussiveMedian[fftIndex]);// 
		
//		pMask.push_back(maskVal*magnitudes[medianIndex][fftIndex]);
//		hMask.push_back((1-maskVal)*magnitudes[medianIndex][fftIndex]);
		
//		harmSpecFrame.push_back(circularSortedMagnitudeValues[fftIndex][medianIndex]);
		
		bool strangeSum = true;//summing lower frequencies more
		if (fftIndex < 10 || !strangeSum)
			pdfVal += maskVal*magnitudes[halfMedianSize][fftIndex];
		else
			pdfVal += 0.3*maskVal*magnitudes[halfMedianSize][fftIndex];
		
		if (printOn && fftIndex < 12)
			printf("MFB: mag [%i] maskVal %f orig magnitudes %f sum %f\n", fftIndex, maskVal, magnitudes[halfMedianSize][fftIndex], pdfVal);
		
		percussiveMask.push_back(maskVal);
		/*
		if (fftIndex >= kickMinIndex && fftIndex < kickMaxIndex)
			kickVal += maskVal*magnitudes[halfMedianSize][fftIndex];
		
		if (fftIndex >= snareMinIndex && fftIndex < snareMaxIndex)
			snareVal += maskVal*magnitudes[halfMedianSize][fftIndex];
		
		if (fftIndex >= cymbalMinIndex && fftIndex < cymbalMaxIndex)
			cymbalVal += maskVal*magnitudes[halfMedianSize][fftIndex];
		
		
		int tmpIndex = fftIndex-1;
		//		assert (tmpIndex < circularMagnitudeValues.size());
		if (tmpIndex >= 0 && tmpIndex < (int)percussiveMaskedSpectrum.size() && maskVal*circularMagnitudeValues[fftIndex][medianIndex] > percussiveMaskedSpectrum[tmpIndex][medianIndex]){
			percussiveDifferenceVal += circularMagnitudeValues[fftIndex][medianIndex];
		}
		 */
		/*
		 if (tmpIndex >= 0 && tmpIndex < circularMagnitudeValues.size() && (1-maskVal)*circularMagnitudeValues[fftIndex][medianIndex] > circularMagnitudeValues[tmpIndex][medianIndex]){
		 harmonicDifferenceVal += circularMagnitudeValues[fftIndex][medianIndex];
		 }
		 */	
		
		//if (printValues)
		//	printf("FFT [%i] is %.3f\n", fftIndex, maskVal*circularMagnitudeValues[fftIndex][medianIndex]);
	}
	
	
	//check snare > kick
	if (snareVal < kickVal)
		snareVal = 0;
	
	
	
	
//	percussiveSpectrum.push_back(v);
//	harmonicSpectrum.push_back(harmSpecFrame);

	//percussiveDetectionFunction.push_back(pdfVal);
	
//	percussiveDifferenceFunction.push_back(percussiveDifferenceVal);
	
//	percussiveMaskedSpectrum.push_back(pMask);
//	harmonicMaskedSpectrum.push_back(hMask);
	
	/*
	if (usingDeques){
		pruneDeque(harmonicSpectrum, maxDequeSize);
		pruneDeque(percussiveSpectrum, maxDequeSize);
		pruneDeque(harmonicMaskedSpectrum, maxDequeSize);
		pruneDeque(percussiveMaskedSpectrum, maxDequeSize);
	}
	*/
	
	//now add to the individual functions
	/*
	kickStrength.push_back(kickVal);
	if (kickDetectionMedian.testNewEvent(kickVal, percussiveDetectionFunction.size()-1)){
		kicksDetected.push_back(percussiveDetectionFunction.size()-1);
		kicksOnBeat.push_back(checkKickOnBeat(percussiveDetectionFunction.size()-1));
		circDrumPattern.newEvent(percussiveDetectionFunction.size()-1, 0);
		//drumAnalyser.newEvent(percussiveDetectionFunction.size()-1, 0);
	}
	
	snareStrength.push_back(snareVal);
	if (snareDetectionMedian.testNewEvent(snareVal, percussiveDetectionFunction.size()-1)){
		snaresDetected.push_back(percussiveDetectionFunction.size()-1);
		snaresOnBeat.push_back(checkKickOnBeat(percussiveDetectionFunction.size()-1));
		circDrumPattern.newEvent(percussiveDetectionFunction.size()-1, 1);
		//drumAnalyser.newEvent(percussiveDetectionFunction.size()-1, 1);
	}
	
	
	
	cymbalStrength.push_back(cymbalVal);
	
	int tmpTwo = 2;
	printf("median[2] %f\n", getHarmonicMedian(tmpTwo));
	*/
	
	if (printOn)
		printf("MFB: pdf %f\n", pdfVal);
	
	return pdfVal;
}

float MedianFilteringBuffer::getHarmonicMedian(int& fftIndex){
	if (fftIndex >= 0 && fftIndex < medianFFTsize)
		return sortedMagnitudes[halfMedianSize][fftIndex];
	else
		return 0;
}


