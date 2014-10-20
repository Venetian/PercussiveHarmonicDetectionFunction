PercussiveHarmonicDetectionFunction
===================================

Implementation of perucssive harmonic detection function in C++


requires ofxWindowRegion and ofxPlotFunction

also here on Github.com/Venetian

1. generate a project using project generator and copy source from AudioInputExample
2. test that compiles okay
3. add files to project, both the src and the ofxWindowRegion, ofxPlotFunction files
4. remove ofxPlotOnsetDetectionFunction (not required and has a dependency)
5. compile

This uses Accelerate , Apple's own framework, for the FFT

Perhaps another version is required for alternative use - eg on Windows machine



Code implementation
Percussive Detection Function: sets up buffers to be used and parameters

AudioRingBuffer: used to keep the audio samples and call FFT when required
//ringbuffer->addToBuffer(samples, n) is true when we reach the hopsize

accFFT: does the FFT on each frame

SpectrumRingBuffer: holds a std::deque of magnitudes from FFT
needs to window the buffer, do FFT, pop result onto the deque
the implementation is for Apple's accelerate but you could do an alternative FFT version easily


MedianFiltering: once we have the above, we do median filtering process as described in paper by Derry Fitzgerald. This implementation descibed in Robertson, Davies and Stark paper


Still to do:
 some kind of IFFT that would actually return audio frames back to you. There is a latency (100ms) here to get the detection function, so would also be in such an audio separation algorithm.
