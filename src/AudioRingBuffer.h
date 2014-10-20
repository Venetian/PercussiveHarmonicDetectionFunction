/*
 *  AudioRingBuffer.h
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 20/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#ifndef AUDIO_RINGBUFFER
#define AUDIO_RINGBUFFER

#include "Debug.h"
//#include "ofMain.h"//get rid of this (to use printf and null)

//#include <iostream>
//#include <cstdio>
//#include <cstdlib>


class AudioRingBuffer{
public:
	AudioRingBuffer();
	~AudioRingBuffer();
	AudioRingBuffer(int bufsize, int hop);

	int addToBuffer(float* samples, int n);
	void printBuffer();
	void printBuffer(int samples);
	//vars
	float* audiobuffer;
	
	double* doubleBuffer;
	
	int buffersize, hopsize;
	int count;//counts when we reached hopsize
	
	bool useDouble;
private:	
	

	
	float* overflowBuffer;//will be for samples that do not fit hopsize exactly
	int overflowCount;
	
};
#endif