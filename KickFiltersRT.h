/*
 FILENAME:	KickFiltersRT.h
 AUTHOR:	Orlando S. Hoilett and Benjamin D. Walters
 CONTACT:	orlandohoilett@gmail.com
 VERSION:	1.3.0
 
 
 AFFILIATIONS
 Linnes Lab, Weldon School of Biomedical Engineering,
 Purdue University, West Lafayette, IN 47907
 
 
 DESCRIPTION
 Templated class for running a few basic real-time filters.
 
 
 UPDATES
 Version 0.0.0
 2020/05/21:1543> (UTC-5)
 			- Initial real-time lowpass and highpass filters
 Version 1.0.0
 2020/05/22:0348> (UTC-5)
 			- Initial release
 2020/07/11:0723> (UTC-5)
 			- Moved to templated class.
 			- Updated comments.
 2020/07/19:1022> (UTC-5)
 			- Added moving average filter.
 2020/07/20:0530> (UTC-5)
 			- Fixed array indexing in movingAverage function to reset array
			position on wrap around.
 Version 1.2.0
 2020/08/21:1558> (UTC-5)
 			- Added a notch filter.
 Version 1.3.0
 2020/08/22:1743> (UTC-5)
 			- Added a median filter.
 
 
 FUTURE UPDATES TO INCLUDE
 1. Add bandpass filter function
 2. Have an initialization flag that keeps track of whether the object is has
 already been called on a certain filter.
 
 
 
 DISCLAIMER
 Linnes Lab code, firmware, and software is released under the
 MIT License (http://opensource.org/licenses/MIT).
 
 The MIT License (MIT)
 
 Copyright (c) 2020 Linnes Lab, Purdue University
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */


#ifndef KickFiltersRT_h
#define KickFiltersRT_h


//Standard Arduino libraries
#include <Arduino.h>

//Kick LL Libraries
#include "KickMath.h"


const uint16_t KickFiltersRT_MAX_MOVING_AVERAGE_ORDER = 50;
const uint16_t KickFiltersRT_MAX_MEDIAN_FILTER_ORDER = 50;


template<typename Type>


class KickFiltersRT
{
	
private:
	
	//Highpass Filter
	Type prevHPInput;
	Type prevHPOutput;
	float alpha_HP;
	
	//Lowpass Filter
	Type prevLPOutput;
	float alpha_LP;
	
	//Bandpass Filter
	Type prevBPOutput;
	Type prevBPInput;
	float alpha_HP_BP;
	float alpha_LP_BP;
	
	//Moving Average Filter
	uint8_t pos;
	Type arr[KickFiltersRT_MAX_MOVING_AVERAGE_ORDER];
	uint8_t order;
	
	// Notch filter parameters & constants
	float r;
	float b0;
	float b1; //{Equation: -2.0*cos(2*PI*fc/float(fs));}
	float b2;
	float a1; //{Equation: 2*r*cos(2*PI*fc/float(fs));}
	float a2;//{Equation: -1.0 * pow(r,2);}
	
	float bs_filter[2]; //stores previous filter outputs
	Type prevNotchInput[2]; //stores previous filter inputs
	
	
	//Median Filter
	uint16_t posMedian;
	Type arrMedian[KickFiltersRT_MAX_MEDIAN_FILTER_ORDER];
	Type tmpArrMedian[KickFiltersRT_MAX_MEDIAN_FILTER_ORDER];
	uint16_t orderMedian;
	
	
public:

	//DEFAULT CONSTRUCTOR
	KickFiltersRT();
	
	//Highpass Filter
	Type highpass(const Type input, float fc, uint16_t dt);
	Type highpass(const Type input);
	void inithighpass(const Type input0, float fc, uint16_t dt);
	
	//Lowpass Filter
	Type lowpass(const Type input, float fc, uint16_t dt);
	Type lowpass(const Type input);
	void initlowpass(const Type input0, float fc, uint16_t dt);

	//Moving Average Filter
	void initmovingAverage(const Type input, uint8_t samples);
	Type movingAverage(const Type input);
	
	//Notch Filter
	void initnotch(const Type input0, const Type input1, float fc, float fs);
	void initnotch(const Type input0, const Type input1, float fc, float fs, float r_coeff);
	float notch(const Type input);
	
	//Median Filter
	void initmedian(const uint16_t filter_order);
	Type median(const Type input);
	Type median(const Type input, const uint16_t filter_order);
	
};



//DEFAULT CONSTRUCTOR
//Initializes private memeber variables
template<typename Type>
KickFiltersRT<Type>::KickFiltersRT()
{
	prevHPInput = 0;
	prevHPOutput = 0;
	alpha_HP = 0;
	
	prevLPOutput = 0;
	alpha_LP = 0;
	
	prevBPOutput = 0;
	prevBPInput = 0;
	alpha_HP_BP = 0;
	alpha_LP_BP = 0;
	
	r = 0;
	b0 = 0;
	b1 = 0; //{Equation: -2.0*cos(2*PI*fc/float(fs));}
	b2 = 0;
	a1 = 0; //{Equation: 2*r*cos(2*PI*fc/float(fs));}
	a2 = 0;//{Equation: -1.0 * pow(r,2);}
	
	bs_filter[0] = 0;
	bs_filter[1] = 0;
	prevNotchInput[0] = 0;
	prevNotchInput[1] = 0;
	
	
	//Median Filter
	posMedian = 0;
	orderMedian = 0;
	
	//explicity sets array values to 0 to avoid undefined values
	for(uint16_t i = 0; i < KickFiltersRT_MAX_MEDIAN_FILTER_ORDER; i++)
	{
		arrMedian[i] = 0;
		tmpArrMedian[i] = 0;
	}
}


//void KickFiltersRT<Type>::initnotch(const Type input0, const Type input1, float fc, float fs)
//return		output of the filter
//
//input0		input data
//input1		input data
//fc			desired center frequency of the notch filter (the frequency to
//					be filtered
//fs			sampling frequency (Hz) of signal being filtered
//
//Implements a simple notch filter. Adapted from Wang and Xiao - 2013 - Second-Order
//IIR Notch Filter Design and Implementation of Digital Signal Processing System.
//Ppaer is also stored in extras folder.
template<typename Type>
void KickFiltersRT<Type>::initnotch(const Type input0, const Type input1, float fc, float fs)
{
	// Notch filter parameters & constants
	//Values calculated manually for increased precision
	r = 0.8;
	b0 = 1;
	b1 = -2*cos(2*PI*fc/fs); //{Equation: -2.0*cos(2*PI*fc/float(fs));}
	b2 = 1;
	a1 = 2*r*cos(2*PI*fc/fs); //{Equation: 2*r*cos(2*PI*fc/float(fs));}
	a2 = -(r*r); //{Equation: -1.0 * pow(r,2);}
	
	
	//calculate filter output
	float bs_Val = input0 + (b1 * 0) + (b2 * 0) + (a1 * bs_filter[1]) + (a2 * bs_filter[0]);
	//update filter output values
	bs_filter[0] = 0;
	bs_filter[1] = bs_Val;
	
	
	//calculate filter output
	bs_Val = input1 + (b1 * input0) + (b2 * 0) + (a1 * bs_filter[1]) + (a2 * bs_filter[0]);
	//update filter output values
	bs_filter[0] = bs_filter[1];
	bs_filter[1] = bs_Val;
	
	
	//update old input values
	prevNotchInput[0] = input0;
	prevNotchInput[1] = input1;
}


//void KickFiltersRT<Type>::initnotch(const Type input0, const Type input1, float fc, float fs, float r_coeff)
//return		output of the filter
//
//input0		input data
//input1		input data
//fc			desired center frequency of the notch filter (the frequency to
//					be filtered
//fs			sampling frequency (Hz) of signal being filtered
//r_coeff		controls filter bandwith (wideness and steepness of the notch)
//					0.8 appears be most ideal, but the user may experiment
//					with the value if they so choose
//
//Implements a simple notch filter. Adapted from Wang and Xiao - 2013 - Second-Order
//IIR Notch Filter Design and Implementation of Digital Signal Processing System.
//Ppaer is also stored in extras folder.
template<typename Type>
void KickFiltersRT<Type>::initnotch(const Type input0, const Type input1, float fc, float fs, float r_coeff)
{
	// Notch filter parameters & constants
	//Values calculated manually for increased precision
	r = r_coeff;
	b0 = 1;
	b1 = -2*cos(2*PI*fc/fs); //{Equation: -2.0*cos(2*PI*fc/float(fs));}
	b2 = 1;
	a1 = 2*r*cos(2*PI*fc/fs); //{Equation: 2*r*cos(2*PI*fc/float(fs));}
	a2 = -(r*r); //{Equation: -1.0 * pow(r,2);}
	
	
	//calculate filter output
	float bs_Val = input0 + (b1 * 0) + (b2 * 0) + (a1 * bs_filter[1]) + (a2 * bs_filter[0]);
	//update filter output values
	bs_filter[0] = 0;
	bs_filter[1] = bs_Val;
	
	
	//calculate filter output
	bs_Val = input1 + (b1 * input0) + (b2 * 0) + (a1 * bs_filter[1]) + (a2 * bs_filter[0]);
	//update filter output values
	bs_filter[0] = bs_filter[1];
	bs_filter[1] = bs_Val;
	
	
	//update old input values
	prevNotchInput[0] = input0;
	prevNotchInput[1] = input1;
}


//float KickFiltersRT<Type>::notch(const Type input)
//return		output of the filter
//
//input			input data
//
//Implements a simple notch filter. Adapted from Wang and Xiao - 2013 - Second-Order
//IIR Notch Filter Design and Implementation of Digital Signal Processing System.
//Reference also stored in extras folder
template<typename Type>
float KickFiltersRT<Type>::notch(const Type input)
{
	//calculate filter output
	float bs_Val = input + (b1 * prevNotchInput[1]) + (b2 * prevNotchInput[0]) + (a1 * bs_filter[1]) + (a2 * bs_filter[0]);
	
	//update filter output values
	bs_filter[0] = bs_filter[1];
	bs_filter[1] = bs_Val;
	
	//update old input values
	prevNotchInput[0] = prevNotchInput[1];
	prevNotchInput[1] = input;
	
	//return filter output
	return bs_Val;
}


//int16_t KickFiltersRT::highpass(const int16_t input, float fc, uint16_t dt)
//return		output of the filter
//
//input			input data
//fc			desired cut-off frequency of the filter (in Hertz)
//dt			sampling period (ms)...time between samples in milliseconds
//
//Implements a simple first-order high pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/High-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
Type KickFiltersRT<Type>::highpass(const Type input, float fc, uint16_t dt)
{
	//Tau = Resistance(R)*Capacitance(C)
	//re-arranging the cut-off frequency equaion [1/(2*pi*R*C)] to solve for R*C
	float tau = 1/(2.0*PI*fc);
	alpha_HP = tau / (tau + (dt/1000.0));
	
	//filtering function
	Type output = alpha_HP*(prevHPOutput + input - prevHPInput);
	
	//re-setting member variables for the next time the filter is called
	prevHPInput = input;
	prevHPOutput = output;
	
	return output;
}


//int16_t KickFiltersRT::highpass(const int16_t input)
//return		output of the filter
//
//input			input data
//
//This function only works if the filter has already been initialized
//i.e. fc, dt, alpha_HP, prevHPInput, and prevHPOutput have already been defined.
//
//Implements a simple first-order high pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/High-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
Type KickFiltersRT<Type>::highpass(const Type input)
{
	//filtering function
	Type output = alpha_HP*(prevHPOutput + input - prevHPInput);
	
	//re-setting member variables for the next time the filter is called
	prevHPInput = input;
	prevHPOutput = output;
	
	return output;
}


//void KickFiltersRT::inithighpass(const int16_t input0, float fc, uint16_t dt)
//
//input0		First value of input data. Feel free to set to 0.
//fc			desired cut-off frequency of the filter (in Hertz)
//dt			sampling period (ms)...time between samples in milliseconds
//
//Initializes filter by defining necessary variables.
//
//Implements a simple first-order high pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/High-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
void KickFiltersRT<Type>::inithighpass(const Type input0, float fc, uint16_t dt)
{
	//Tau = Resistance(R)*Capacitance(C)
	//re-arranging the cut-off frequency equaion [1/(2*pi*R*C)] to solve for R*C
	float tau = 1/(2.0*PI*fc);
	alpha_HP = tau / (tau + (dt/1000.0));
	
	
	//setting member variables for the next time the filter is called
	prevHPInput = input0;
	prevHPOutput = input0;
}


//int16_t KickFiltersRT::lowpass(const int16_t input, float fc, uint16_t dt)
//return		output of the filter
//
//input			input data
//fc			desired cut-off frequency of the filter (in Hertz)
//dt			sampling period (ms)...time between samples in milliseconds
//
//Implements a simple first-order low pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/Low-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
Type KickFiltersRT<Type>::lowpass(const Type input, float fc, uint16_t dt)
{
	//Tau = Resistance*Capacitance
	//re-arranging the cut-off frequency equaion [1/(2*pi*R*C)] to solve for R*C
	float tau = 1/(2.0*PI*fc);
	alpha_LP = (dt/1000.0) / (tau + (dt/1000.0));
	
	//filtering function
	Type output = prevLPOutput + alpha_LP*(input - prevLPOutput);
	
	//re-setting member variables for the next time the filter is called
	prevLPOutput = output;
	
	return output;
}


//int16_t KickFiltersRT::lowpass(const int16_t input)
//return		output of the filter
//
//input			input data
//
//This function only works if the filter has already been initialized
//i.e. fc, dt, alpha_LP, and prevLPOutput have already been defined.
//
//Implements a simple first-order low pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/Low-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
Type KickFiltersRT<Type>::lowpass(const Type input)
{
	//filtering function
	Type output = prevLPOutput + alpha_LP*(input - prevLPOutput);
	
	//re-setting member variables for the next time the filter is called
	prevLPOutput = output;
	
	return output;
}


//void KickFiltersRT::initlowpass(const int16_t input0, float fc, uint16_t dt)
//
//input0		First value of input data. Feel free to set to 0.
//fc			desired cut-off frequency of the filter (in Hertz)
//dt			sampling period (ms)...time between samples in milliseconds
//
//Initializes filter by defining necessary variables.
//
//Implements a simple first-order low pass filter according to the algorithm
//here: https://en.wikipedia.org/wiki/Low-pass_filter
//The Wikipedia article is also located in the library in the "extras/references/" folder.
template<typename Type>
void KickFiltersRT<Type>::initlowpass(const Type input0, float fc, uint16_t dt)
{
	//Tau = Resistance*Capacitance
	//re-arranging the cut-off frequency equaion [1/(2*pi*R*C)] to solve for R*C
	float tau = 1/(2.0*PI*fc);
	alpha_LP = (dt/1000.0) / (tau + (dt/1000.0));
	
	
	//setting member variables for the next time the filter is called
	prevLPOutput = alpha_LP*input0;
}


//int16_t KickFiltersRT::bandpass(const int16_t input)
//{
//	int16_t output_intermediate = alpha_HP_BP*(prevBPOutput + input - prevBPInput);
//	int16_t output = prevBPOutput + alpha_LP_BP*(output_intermediate - prevBPOutput);
//
//	prevBPOutput = output;
//
//	return output;
//}


//void KickFiltersRT::initmovingAverage(const Type input, uint16_t samples)
//input			data array...declared as const so it's read-only
//samples		order of the filter (how many samples should be averaged together)
//
//Implements a simple moving average filter.
template<typename Type>
void KickFiltersRT<Type>::initmovingAverage(const Type input, uint8_t samples)
{
	//ensures filter order doesn't exceed size of compiled data array
	if(samples > KickFiltersRT_MAX_MOVING_AVERAGE_ORDER) order = KickFiltersRT_MAX_MOVING_AVERAGE_ORDER;
	else order = samples;
	
	
	pos = 0; //sets current position to 0 since we're initializing filter
	arr[pos] = input; //sets first value of array "input" parameter
	for(uint8_t i = 1; i < order; i++) arr[i] = 0; //explicity sets other array values to 0
}


//Type KickFiltersRT::movingAverage(const Type input)
//input			data array...declared as const so it's read-only
//
//Implements a simple moving average filter.
template<typename Type>
Type KickFiltersRT<Type>::movingAverage(const Type input)
{
	if(order == 0) return 0;
	
	Type sum = 0;
	//Do not increment pos here since pos will be incremented later in this method
	arr[pos] = input;
	for(uint8_t i = 0; i < order; i++) sum += arr[i];
	
	//increments pos variable and wraps around when
	//we get to the length-1 of arr, the data arr
	if(++pos > order) pos = 0;
	
	return sum/order;
}


//Median Filter
template<typename Type>
void KickFiltersRT<Type>::initmedian(const uint16_t filter_order)
{
	//ensures filter order doesn't exceed size of compiled data array
	if(filter_order > KickFiltersRT_MAX_MEDIAN_FILTER_ORDER) orderMedian = KickFiltersRT_MAX_MEDIAN_FILTER_ORDER;
	else orderMedian = filter_order;
	
	
	//explicity sets array values to 0 to avoid undefined values
	posMedian = 0;
	for(uint16_t i = 0; i < KickFiltersRT_MAX_MEDIAN_FILTER_ORDER; i++)
	{
		arrMedian[i] = 0;
		tmpArrMedian[i] = 0;
	}
}


template<typename Type>
Type KickFiltersRT<Type>::median(const Type input)
{
	arrMedian[posMedian] = input;
	if(++posMedian > orderMedian-1) posMedian = 0;

	
	return KickMath<Type>::calcMedian(orderMedian, arrMedian, tmpArrMedian);
}


template<typename Type>
Type KickFiltersRT<Type>::median(const Type input, const uint16_t filter_order)
{
	if(filter_order > KickFiltersRT_MAX_MEDIAN_FILTER_ORDER) orderMedian = KickFiltersRT_MAX_MEDIAN_FILTER_ORDER;
	else orderMedian = filter_order;

	
	arrMedian[posMedian] = input;
	if(++posMedian > orderMedian-1) posMedian = 0;
	
	
	return KickMath<Type>::calcMedian(orderMedian, arrMedian, tmpArrMedian);
}



#endif /* KickFiltersRT_h */
