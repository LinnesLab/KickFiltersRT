/*
 FILENAME:	KickFiltersRT.h
 AUTHOR:	Orlando S. Hoilett
 CONTACT:	orlandohoilett@gmail.com
 VERSION:	1.1.0
 
 
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


const uint8_t KickFiltersRT_MAX_MOVING_AVERAGE_ORDER = 25;

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


//	bandpass();
//	notch();
	
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
	pos = 0; //sets current position to 0 since we're initializing filter
	arr[pos] = input; //sets first value of array "input" parameter
	for(uint8_t i = 1; i < order; i++) arr[i] = 0; //explicity sets other array values to 0
	
	//ensures filter order doesn't exceed size of compiled data array
	if(samples > KickFiltersRT_MAX_MOVING_AVERAGE_ORDER) order = KickFiltersRT_MAX_MOVING_AVERAGE_ORDER;
	else order = samples;
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
	//Do not increment pos here since pos was incremented on the previous function call
	arr[pos] = input;
	for(uint8_t i = 0; i < order; i++) sum += arr[i];
	
	//increments pos variable and wraps around when
	//we get to the length-1 of arr, the data arr
	if(++pos > order) pos = 0;
	
	return sum/order;
}



#endif /* KickFiltersRT_h */
