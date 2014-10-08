//
//  accFFT.cpp
//  AccelerateFFTtool
//
//  Created by Adam Stark on 17/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "accFFT.h"

accFFT :: accFFT(int fft_size, int type)
{
	//doubel vs float?
    fft_type = type;
    
    fftSize = fft_size;           
    fftSizeOver2 = fftSize/2;
    log2n = log2f(fftSize);  
    log2nOver2 = log2n/2;
    
    if (fft_type == 0)
    {
        split.realp = (float *) malloc(fftSize * sizeof(float));
        split.imagp = (float *) malloc(fftSize * sizeof(float));
        
        // allocate the fft object once
        fftSetup = vDSP_create_fftsetup(log2n, FFT_RADIX2);
        if (fftSetup == NULL) {
            //printf("FFT Setup failed\n");
        }
    }
    else if (fft_type == 1) 
    {
        d_split.realp = (double *) malloc(fftSize * sizeof(double));
        d_split.imagp = (double *) malloc(fftSize * sizeof(double));
        
        // allocate the fft object once
        fftSetupD = vDSP_create_fftsetupD(log2n, FFT_RADIX2);
        if (fftSetupD == NULL) {
            //printf("FFT Setup failed\n");
        }
    }
        
    
    
}

accFFT :: ~accFFT()
{
    if (fft_type == 0)
    {
        free(split.realp);
        free(split.imagp);
        vDSP_destroy_fftsetup(fftSetup);
    }
    else if (fft_type == 1)
    {
        free(d_split.realp);
        free(d_split.imagp);
        vDSP_destroy_fftsetupD(fftSetupD);
    }
    //printf("delete called accFFT\n");
    
}





void accFFT :: forward_FFT_f(float *buffer,float *real,float *imag)
{        
    //convert to split complex format with evens in real and odds in imag
    vDSP_ctoz((COMPLEX *) buffer, 2, &split, 1, fftSizeOver2);
    
    //calc fft
    vDSP_fft_zrip(fftSetup, &split, 1, log2n, FFT_FORWARD);
    
    // set Nyquist component to imaginary of 0 component
    split.realp[fftSizeOver2] = split.imagp[0];
    split.imagp[fftSizeOver2] = 0.0;
    
    // set 0 component to zero
    split.imagp[0] = 0.0;
    
    // multiply by 0.5 to get correct output (to do with Apple's FFT implementation)
    for (i = 0; i <= fftSizeOver2; i++)
    {
        split.realp[i] *= 0.5;
        split.imagp[i] *= 0.5;
    }
    
    // set values above N/2+1 which are complex conjugate mirror image of those below
    for (i = fftSizeOver2 - 1;i > 0;--i)
    {
        split.realp[2*fftSizeOver2 - i] = split.realp[i];
        split.imagp[2*fftSizeOver2 - i] = -1*split.imagp[i];
        
        //cout << split_data.realp[2*fftSizeOver2 - i] << "   " << split_data.imagp[2*fftSizeOver2 - i] << "i" << endl;
    }
    
    for (i = 0;i < fftSize;i++)
    {
        real[i] = split.realp[i];
        imag[i] = split.imagp[i];
    }
}


void accFFT :: forward_FFT_f(float *buffer, fft_complex_double *out)
{        
    //convert to split complex format with evens in real and odds in imag
    vDSP_ctoz((COMPLEX *) buffer, 2, &split, 1, fftSizeOver2);
    
    //calc fft
    vDSP_fft_zrip(fftSetup, &split, 1, log2n, FFT_FORWARD);
    
    // set Nyquist component to imaginary of 0 component
    split.realp[fftSizeOver2] = split.imagp[0];
    split.imagp[fftSizeOver2] = 0.0;
    
    // set 0 component to zero
    split.imagp[0] = 0.0;
    
    // multiply by 0.5 to get correct output (to do with Apple's FFT implementation)
    for (i = 0; i <= fftSizeOver2; i++)
    {
        split.realp[i] *= 0.5;
        split.imagp[i] *= 0.5;
    }
    
    // set values above N/2+1 which are complex conjugate mirror image of those below
    for (i = fftSizeOver2 - 1;i > 0;--i)
    {
        split.realp[2*fftSizeOver2 - i] = split.realp[i];
        split.imagp[2*fftSizeOver2 - i] = -1*split.imagp[i];
    }
    
    for (i = 0;i < fftSize;i++)
    {
        out[i][0] = split.realp[i];
        out[i][1] = split.imagp[i];
    }
}




void accFFT :: forward_FFT_f(float *buffer, COMPLEX_SPLIT& outSplit)
{        
    //convert to split complex format with evens in real and odds in imag
    vDSP_ctoz((COMPLEX *) buffer, 2, &outSplit, 1, fftSizeOver2);
    
    //calc fft
    vDSP_fft_zrip(fftSetup, &outSplit, 1, log2n, FFT_FORWARD);
    
	
	//CONVERT APPLE'S COMPACT REPRESENTATION INTO PROPER FFT
	//SEE Apple documentation on vDSP for explanation
	//effectively, the imag value at [0] is always 0, so stores the value at Nyquist (N/2) instead
	//firstly we need to scale by 0.5 when we have done complex forward fft
	//due to the way apple performs
	float scale = 0.5;
	vDSP_vsmul(outSplit.realp, 1, &scale, outSplit.realp, 1, fftSizeOver2);
	vDSP_vsmul(outSplit.imagp, 1, &scale, outSplit.imagp, 1, fftSizeOver2);
	/*
	example here:
	ring[0] 1.000000
	ring[1] 2.200000
	ring[2] 3.300000
	ring[3] 4.000000
	ring[4] 3.300000
	ring[5] 5.600000
	ring[6] 1.200000
	ring[7] 0.763000
	fft[0] real 21.363, imag -3.763
	fft[1] real -6.993, imag -1.985
	fft[2] real -0.200, imag -3.037
	fft[3] real 2.393, imag 2.215
	 fft[4] real 0.000, imag 0.000
	 fft[5] real 0.000, imag 0.000
	 fft[6] real 0.000, imag 0.000
	 fft[7] real 0.000, imag 0.000
	 fft[8] real -3.763, imag 0.000
	 mag[0] 21.363
	 mag[1] 7.269
	 mag[2] 3.044
	 mag[3] 3.261
	 mag[4] -3.763
	 */
	//for (int i = 0; i <= fftSize; i++)
	//{
    //    printf("fft[%i] real %.3f, imag %.3f\n", i, outSplit.realp[i], outSplit.imagp[i]);
    //}
	
	
	/*
    // set Nyquist component to imaginary of 0 component
    split.realp[fftSizeOver2] = split.imagp[0];
    split.imagp[fftSizeOver2] = 0.0;
    
    // set 0 component to zero
    split.imagp[0] = 0.0;
    
    // multiply by 0.5 to get correct output (to do with Apple's FFT implementation)
    for (i = 0; i <= fftSizeOver2; i++)
    {
        split.realp[i] *= 0.5;
        split.imagp[i] *= 0.5;
    }
    
    // set values above N/2+1 which are complex conjugate mirror image of those below
    for (i = fftSizeOver2 - 1;i > 0;--i)
    {
        split.realp[2*fftSizeOver2 - i] = split.realp[i];
        split.imagp[2*fftSizeOver2 - i] = -1*split.imagp[i];
    }
    
    for (i = 0;i < fftSize;i++)
    {
        out[i][0] = split.realp[i];
        out[i][1] = split.imagp[i];
    }
	 */
}







void accFFT :: forward_FFT_d(double *buffer,fft_complex_double *out)
{        
    //convert to split complex format with evens in real and odds in imag
    vDSP_ctozD((DOUBLE_COMPLEX *) buffer, 2, &d_split, 1, fftSizeOver2);
    
    //calc fft
    vDSP_fft_zripD(fftSetupD, &d_split, 1, log2n, FFT_FORWARD);
    
    // set Nyquist component to imaginary of 0 component
    d_split.realp[fftSizeOver2] = d_split.imagp[0];
    d_split.imagp[fftSizeOver2] = 0.0;
    
    // set 0 component to zero
    d_split.imagp[0] = 0.0;
    
    // multiply by 0.5 to get correct output (to do with Apple's FFT implementation)
    for (i = 0; i <= fftSizeOver2; i++)
    {
        d_split.realp[i] *= 0.5;
        d_split.imagp[i] *= 0.5;
    }
    
    // set values above N/2+1 which are complex conjugate mirror image of those below
    for (i = fftSizeOver2 - 1;i > 0;--i)
    {
        d_split.realp[2*fftSizeOver2 - i] = d_split.realp[i];
        d_split.imagp[2*fftSizeOver2 - i] = -1*d_split.imagp[i];
    }
    
    for (i = 0;i < fftSize;i++)
    {
        out[i][0] = d_split.realp[i]; 
        out[i][1] = d_split.imagp[i];
    }
}