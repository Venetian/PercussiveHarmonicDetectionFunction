/*
 *  AudioRingBuffer.cpp
 *  BeatSeekerOF
 *
 *  Created by Andrew Robertson on 20/02/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

#include "AudioRingBuffer.h"

AudioRingBuffer::AudioRingBuffer(){
	audiobuffer = NULL;
	count= 0;
	buffersize = 0;
	hopsize = 0;
	
	useDouble = true;
    
    printf("audiobuffer init staright\n");
    
}

AudioRingBuffer::AudioRingBuffer(int bufsize, int hop){
    printf("audiobuffer init with int int\n");
    
	if (bufsize > 0){
		buffersize = bufsize;
		hopsize = hop;
		if (audiobuffer)
			delete[] audiobuffer;
	
		audiobuffer = new float[buffersize];
		doubleBuffer = new double[buffersize];
		
		while (bufsize--){
			audiobuffer[bufsize] = 0;
			doubleBuffer[bufsize] = 0;
		//	printf("setting ring[%i] to 0\n", n);
		}
		
	}
	count = 0;
    
}

AudioRingBuffer::~AudioRingBuffer(){
	//if (audiobuffer != 0)
	//	delete[] audiobuffer;
}

int AudioRingBuffer::addToBuffer(float* samples, int n){
	if (n <= hopsize && hopsize%n == 0){
		//nice way
		int tmp = buffersize - n;
		float* buf = audiobuffer;
		while (tmp--){
			*buf = *(buf+n);
			buf++;
		}

		tmp = 0;
		while (tmp < n){
			audiobuffer[buffersize-n+tmp] = samples[tmp];
			tmp++;
		}
		count += n;
		
		if(count > hopsize)
			printf("SOMETHING WRONG, count %i\n", count);
		
		if (count == hopsize){
			if (DEBUG_MODE)
				printf("Audio_RINGBUF BOOM\n");
			count = 0;
			return 1;//buffer ready
		}
		else
			return 0;

	} else if (buffersize > 0){
	//	nasty way TBC
		//while (count < hopsize){
		//add to buffer
		//}
		//then add to overflow
	} else {
		printf("Buffer not initialised\n");
	}
}

void AudioRingBuffer::printBuffer(){
	for (int i = 0; i < buffersize; i++)
		printf("ring[%i] %f\n", i, audiobuffer[i]);
}

void AudioRingBuffer::printBuffer(int samples){
	for (int i = 0; i < samples; i++)
		printf("ring[%i] %f\n", i, audiobuffer[i]);
}
