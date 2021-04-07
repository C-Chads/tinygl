//unlicense'd
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/


//#pragma once
#ifndef RESWEEP_H
#define RESWEEP_H

#ifdef RESWEEP_IMPLEMENTATION

#include <math.h>

#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif

#ifndef M_1_PI
#define	M_1_PI 0.31830988618379067154
#endif

#define SIDELOBE_HEIGHT 96
#define UP_TRANSITION_WIDTH (1.0 / 32.0)
#define DOWN_TRANSITION_WIDTH (1.0 / 128.0)
#define MAX_SINC_WINDOW_SIZE 2048
#define RESAMPLE_LUT_STEP 128

typedef struct
{
	float value;
	float delta;
}
lutEntry_t;

lutEntry_t dynamicLut[RESAMPLE_LUT_STEP * MAX_SINC_WINDOW_SIZE];

static inline unsigned int calc_gcd(unsigned int a, unsigned int b)
{
	while (b)
	{
		unsigned int t = b;
		b = a % b;
		a = t;
	}

	return a;
}

static inline double exact_nsinc(double x)
{
	if (x == 0.0)
		return 1.0;

	return ((double)(M_1_PI) / x) * sin(M_PI * x);
}

// Modified Bessel function of the first kind, order 0
// https://ccrma.stanford.edu/~jos/sasp/Kaiser_Window.html
static inline double I0(double x)
{
	double r = 1.0, xx = x * x, xpow = xx, coeff = 0.25;
	int k;

	// iterations until coeff ~= 0
	// 19 for float32, 89 for float64, 880 for float80
	for (k = 1; k < 89; k++)
	{
		r += xpow * coeff;
		coeff /= (4 * k + 8) * k + 4;
		xpow *= xx;
	}

	return r;
}

// https://ccrma.stanford.edu/~jos/sasp/Kaiser_Window.html
static inline double kaiser(int n, int length, double beta)
{
	double mid = 2 * n / (double)(length - 1) - 1.0;

	return I0(beta * sqrt(1.0 - mid * mid)) / I0(beta);
}

static inline void sinc_resample_createLut(int inFreq, int cutoffFreq2, int windowSize, double beta)
{
	double windowLut[windowSize];
	double freqAdjust = (double)cutoffFreq2 / (double)inFreq;
	lutEntry_t *out, *in;
	int i, j;

	for (i = 0; i < windowSize; i++)
		windowLut[i] = kaiser(i, windowSize, beta);

	out = dynamicLut;
	for (i = 0; i < RESAMPLE_LUT_STEP; i++)
	{
		double offset = i / (double)(RESAMPLE_LUT_STEP - 1) - windowSize / 2;
		double sum = 0.0;
		for (j = 0; j < windowSize; j++)
		{
			double s = exact_nsinc((j + offset) * freqAdjust);
			out->value = s * windowLut[j];
			sum += s;
			out++;
		}

		out -= windowSize;
		for (j = 0; j < windowSize; j++)
		{
			out->value /= sum;
			out++;
		}
	}

	out = dynamicLut;
	in = out + windowSize;
	for (i = 0; i < RESAMPLE_LUT_STEP - 1; i++)
	{
		for (j = 0; j < windowSize; j++)
		{
			out->delta = in->value - out->value;
			out++;
			in++;
		}
	}

	for (j = 0; j < windowSize; j++)
	{
		out->delta = 0;
		out++;
	}
}

static inline void sinc_resample_internal(short *wavOut, int sizeOut, int outFreq, const short *wavIn, int sizeIn, int inFreq, int cutoffFreq2, int numChannels, int windowSize, double beta)
{
	float y[windowSize * numChannels];
	const short *sampleIn, *wavInEnd = wavIn + (sizeIn / 2);
	short *sampleOut, *wavOutEnd = wavOut + (sizeOut / 2);
	float outPeriod;
	int subpos = 0;
	int gcd = calc_gcd(inFreq, outFreq);
	int i, c, next;
	float dither[numChannels];

	sinc_resample_createLut(inFreq, cutoffFreq2, windowSize, beta);

	inFreq /= gcd;
	outFreq /= gcd;
	outPeriod = 1.0f / outFreq;

	for (c = 0; c < numChannels; c++)
		dither[c] = 0.0f;

	for (i = 0; i < windowSize / 2 - 1; i++)
	{
		for (c = 0; c < numChannels; c++)
			y[i * numChannels + c] = 0;
	}

	sampleIn = wavIn;
	for (; i < windowSize; i++)
	{
		for (c = 0; c < numChannels; c++)
			y[i * numChannels + c] = (sampleIn < wavInEnd) ? *sampleIn++ : 0;
	}

	sampleOut = wavOut;
	next = 0;
	while (sampleOut < wavOutEnd)
	{
		float samples[numChannels];
		float offset = 1.0f - subpos * outPeriod;
		float interp;
		lutEntry_t *lutPart;
		int index;

		for (c = 0; c < numChannels; c++)
			samples[c] = 0.0f;

		interp = offset * (RESAMPLE_LUT_STEP - 1);
		index = interp;
		interp -= index;
		lutPart = dynamicLut + index * windowSize;

		for (i = next; i < windowSize; i++, lutPart++)
		{
			float scale = lutPart->value + lutPart->delta * interp;

			for (c = 0; c < numChannels; c++)
				samples[c] += y[i * numChannels + c] * scale;
		}

		for (i = 0; i < next; i++, lutPart++)
		{
			float scale = lutPart->value + lutPart->delta * interp;

			for (c = 0; c < numChannels; c++)
				samples[c] += y[i * numChannels + c] * scale;
		}

		for (c = 0; c < numChannels; c++)
		{
			float r = roundf(samples[c] + dither[c]);
			dither[c] += samples[c] - r;

			if (r > 32767)
				*sampleOut++ = 32767;
			else if (r < -32768)
				*sampleOut++ = -32768;
			else
				*sampleOut++ = r;
		}

		subpos += inFreq;
		while (subpos >= outFreq)
		{
			subpos -= outFreq;

			for (c = 0; c < numChannels; c++)
				y[next * numChannels + c] = (sampleIn < wavInEnd) ? *sampleIn++ : 0;

			next = (next + 1) % windowSize;
		}
	}
}

void sinc_resample(short *wavOut, int sizeOut, int outFreq, const short *wavIn, int sizeIn, int inFreq, int numChannels)
{
	double sidelobeHeight = SIDELOBE_HEIGHT;
	double transitionWidth;
	double beta = 0.0;
	int cutoffFreq2;
	int windowSize;

	// Just copy if no resampling necessary
	if (outFreq == inFreq)
	{
		memcpy(wavOut, wavIn, (sizeOut < sizeIn) ? sizeOut : sizeIn);
		return;
	}

	transitionWidth = (outFreq > inFreq) ? UP_TRANSITION_WIDTH : DOWN_TRANSITION_WIDTH;

	// cutoff freq is ideally half transition width away from output freq
	cutoffFreq2 = outFreq - transitionWidth * inFreq * 0.5;

	// FIXME: Figure out why there are bad effects with cutoffFreq2 > inFreq
	if (cutoffFreq2 > inFreq)
		cutoffFreq2 = inFreq;

	// https://www.mathworks.com/help/signal/ug/kaiser-window.html
	if (sidelobeHeight > 50)
		beta = 0.1102 * (sidelobeHeight - 8.7);
	else if (sidelobeHeight >= 21)
		beta = 0.5842 * pow(sidelobeHeight - 21.0, 0.4) + 0.07886 * (sidelobeHeight - 21.0);

	windowSize = (sidelobeHeight - 8.0) / (2.285 * transitionWidth * M_PI) + 1;

	if (windowSize > MAX_SINC_WINDOW_SIZE)
		windowSize = MAX_SINC_WINDOW_SIZE;

	// should compile as different paths
	// number of channels need to be compiled as separate paths to ensure good
	// vectorization by the compiler
	if (numChannels == 1)
		sinc_resample_internal(wavOut, sizeOut, outFreq, wavIn, sizeIn, inFreq, cutoffFreq2, 1, windowSize, beta);
	else if (numChannels == 2)
		sinc_resample_internal(wavOut, sizeOut, outFreq, wavIn, sizeIn, inFreq, cutoffFreq2, 2, windowSize, beta);
	else
		sinc_resample_internal(wavOut, sizeOut, outFreq, wavIn, sizeIn, inFreq, cutoffFreq2, numChannels, windowSize, beta);

}
// RESWEEP_IMPLEMENTATION
#endif 
//RESWEEP_H
#endif 
