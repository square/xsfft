#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "xsfft.h"

// Private functions

void _xsFormatInput(xsComplex *data, const long dataLength);
void _xsTransformHelper(xsComplex *data, const long dataLength, const double signedPI, const long maxFrequency);
void _xsScaleIFFT(xsComplex *data, const long dataLength);
xsComplex *_xsReverseCopy(xsComplex *data, const long dataLength);


// Complex array creation/destruction

xsComplex *xsAllocArrayDouble(double *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = *(data + dataIndex);
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

xsComplex *xsAllocArrayFloat(float *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = (double)(*(data + dataIndex));
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

xsComplex *xsAllocArrayLong(long *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = (double)(*(data + dataIndex));
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

xsComplex *xsAllocArrayInt(int *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = (double)(*(data + dataIndex));
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

xsComplex *xsAllocArrayShort(short *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = (double)(*(data + dataIndex));
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

xsComplex *xsAllocArrayChar(char *data, long dataLength)
{
    xsComplex *newData = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        (newData + dataIndex)->real = (double)(*(data + dataIndex));
        (newData + dataIndex)->imaginary = 0.0;
    }
    
    return newData;
}

void xsFreeArray(xsComplex *data)
{
    free(data);
    data = NULL;
}


// Public method implementations

long xsNextPowerOfTwo(const long value)
{
    if (!(value & (value - 1))) {
        return value;
    }
    
    long nextPowerOfTwo = 1;
    while (nextPowerOfTwo < value) {
        nextPowerOfTwo <<= 1;
    }
    
    return nextPowerOfTwo;
}

xsComplex *xsCoerceDataRadix2(xsComplex *data, long *dataLength)
{
    long newLength = xsNextPowerOfTwo(*dataLength);
    if (newLength == *dataLength) {
        return data;
    }
    
    xsComplex *newData = (xsComplex *)calloc(newLength, sizeof(xsComplex));
    memcpy(newData, data, sizeof(xsComplex) * (*dataLength));
    free(data);
    
    for (long index = *dataLength; index < newLength; ++index) {
        (newData + index)->real = 0.0;
        (newData + index)->imaginary = 0.0;
    }
    
    *dataLength = newLength;
    
    return newData;
}

int xsFFT(xsComplex *data, const long dataLength, const long maxFrequency)
{
	if (!data || dataLength < 1 || dataLength & (dataLength - 1)) {
        return 0;
    }
    
	_xsFormatInput(data, dataLength);
    _xsTransformHelper(data, dataLength, -xsPI, maxFrequency);
    
	return 1;
}

int xsIFFT(xsComplex *data, const long dataLength, const long maxFrequency)
{
	if (!data || dataLength < 1 || dataLength & (dataLength - 1)) {
        return 0;
    }
    
	_xsFormatInput(data, dataLength);
    _xsTransformHelper(data, dataLength, xsPI, maxFrequency);
	_xsScaleIFFT(data, dataLength);
    
	return 1;
}


// Useful functions that utilize FFTs

xsComplex *xsInterpolateWithFactor2(xsComplex *data, long *dataLength)
{
    // Setup
    data = xsCoerceDataRadix2(data, dataLength);
    long oldLength = *dataLength;
    long newLength = oldLength << 1;
    
    // FFT
    xsFFT(data, oldLength, oldLength);
    
    // Zero pad
    xsComplex *newData = (xsComplex *)calloc(newLength, sizeof(xsComplex));
    memcpy(newData, data, sizeof(xsComplex) * oldLength);
    free(data);
    data = NULL;
    
    long rightHalfFirstIndex = 3 * oldLength / 2;
    long zeroPadFirstIndex = oldLength / 2;
    
    xsComplex center = *(newData + zeroPadFirstIndex);
    (newData + zeroPadFirstIndex)->real = center.real / 2.0;
    (newData + zeroPadFirstIndex)->imaginary = center.imaginary / 2.0;
    
    (newData + rightHalfFirstIndex)->real = center.real / 2.0;
    (newData + rightHalfFirstIndex)->imaginary = center.imaginary / 2.0;
    
    for (long zeroPadIndex = 1; zeroPadIndex < oldLength / 2; ++zeroPadIndex) {
        *(newData + rightHalfFirstIndex + zeroPadIndex) = *(newData + zeroPadFirstIndex + zeroPadIndex);
        (newData + zeroPadFirstIndex + zeroPadIndex)->real = 0.0;
        (newData + zeroPadFirstIndex + zeroPadIndex)->imaginary = 0.0;
    }
    
    *dataLength = newLength;
    
    // Power has been halved at this point, we need to scale
    for (long index = 0; index < newLength; ++index) {
        (newData + index)->real *= 2.0;
        (newData + index)->imaginary *= 2.0;
    }
    
    // IFFT
    xsIFFT(newData, newLength, newLength);
    
    return newData;
}


// Private method implementations

void _xsFormatInput(xsComplex *data, const long dataLength)
{
	long target = 0;
	for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
		if (target > dataIndex) {
			xsComplex temp = *(data + target);
            *(data + target) = *(data + dataIndex);
			*(data + dataIndex) = temp;
		}
        
		long targetBitMask = dataLength;
        
		while (target & (targetBitMask >>= 1)) {
			target &= ~targetBitMask;
        }
		target |= targetBitMask;
	}
}

void _xsTransformHelper(xsComplex *data, const long dataLength, const double signedPI, const long maxFrequency)
{
    //   Perform butterflies...
	for (long step = 1; step < maxFrequency; step <<= 1) {
		double sine = sin(signedPI / (double)step * 0.5);
		xsComplex twiddleMultiplier = xsComplexFromComponents(-2.0 * sine * sine, sin(signedPI / (double)step));
		xsComplex twiddleFactor = xsComplexFromReal(1.0);
		for (long group = 0; group < step; ++group) {
			for (long pair = group; pair < dataLength; pair += (step << 1)) {
				long match = pair + step;
				xsComplex product = xsComplexProduct(twiddleFactor, *(data + match));
				*(data + match) = xsComplexDifference(*(data + pair), product);
                *(data + pair) = xsComplexSum(*(data + pair), product);
			}
            
			twiddleFactor = xsComplexSum(xsComplexProduct(twiddleMultiplier, twiddleFactor), twiddleFactor);
		}
	}
}

xsComplex *_xsReverseCopy(xsComplex *data, const long dataLength)
{
    xsComplex *reversedCopy = (xsComplex *)calloc(dataLength, sizeof(xsComplex));
    
    for (long index = 0; index < dataLength; ++index) {
        *(reversedCopy + (dataLength - index) - 1) = *(data + index);
    }
    
    return reversedCopy;
}


//   Scaling of inverse FFT result
void _xsScaleIFFT(xsComplex *data, const long dataLength)
{
	const double scaleFactor = 1.0 / (double)dataLength;
	//   Scale all data entries
	for (long dataIndex = 0; dataIndex < dataLength; ++dataIndex) {
        *(data + dataIndex) = xsComplexScale(*(data + dataIndex), scaleFactor);
    }
}
