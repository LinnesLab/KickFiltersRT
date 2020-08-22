/*
 * FILENAME: EXAMPLE03_notch.ino
 * AUTHOR:   Orlando S. Hoilett, Benjamin D. Walters
 * CONTACT:  orlandohoilett@gmail.com
 * VERSION:  1.0.0
 * 
 * 
 * AFFILIATIONS
 * Linnes Lab, Weldon School of Biomedical Engineering,
 * Purdue University, West Lafayette, IN 47907
 * 
 * 
 * DESCRIPTION
 * Basic test of the KickFiltersRT class to evaluate the notch filter
 * 
 * The input signal is a simulated photoplethysmogram used to
 * measure heart rate. Each major peak is a new heart beat. The
 * smaller peak is a dicrotic notch which is generated when
 * the aortic valve closes.
 * 
 * 
 * UPDATES
 * Version 1.0.0
 * 2020/08/21:1956>
 *           - Initial release
 *           
 * 
 * DISCLAIMER
 * Linnes Lab code, firmware, and software is released under the
 * MIT License (http://opensource.org/licenses/MIT).
 * 
 * The MIT License (MIT)
 * 
 * Copyright (c) 2020 Linnes Lab, Purdue University
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */


#include "KickFiltersRT.h"


const float sinefreq = 60;
const float fs = 15*sinefreq; //samples 15x the frequency of the wave
const float dt = 1.0/fs;
const uint16_t samples = 20*(1/sinefreq)/dt; //prints 20 cycles of the wave

const float scale = 100;


KickFiltersRT<int16_t> filtersRT1;
KickFiltersRT<int16_t> filtersRT2;
KickFiltersRT<int16_t> filtersRT3;


void setup()
{
  Serial.begin(9600); //Use SerialUSB for SparkFun SAMD21 boards
  while(!Serial); //will not run until Serial Monitor or Plotter is open


  //initial values can be set to 0
  //using default r_coeff value of 0.8
  filtersRT1.initnotch(0, 0, 50, fs);
  filtersRT2.initnotch(0, 0, 58, fs);
  filtersRT3.initnotch(0, 0, 60, fs);

  //setting your own r_coeff value
//  filtersRT1.initnotch(0, 0, 50, fs, 0.1);
//  filtersRT2.initnotch(0, 0, 58, fs, 0.1);
//  filtersRT3.initnotch(0, 0, 60, fs, 0.1);


  for(uint16_t i = 0; i < samples; i++)
  {
    //input is a int16_t to match the declaration of the notch filter "KickFiltersRT<int16_t>"
    //can change to a different numeric type if desired, but must match the object declaration
    int16_t input = scale*sin(2*PI*sinefreq*i*dt); //generates sine wave that will be filtered
    
    Serial.print(input); //unfiltered signal
    Serial.print(",");
    Serial.print(filtersRT1.notch(input));
    Serial.print(",");
    Serial.print(filtersRT2.notch(input));
    Serial.print(",");
    Serial.print(filtersRT3.notch(input));
    Serial.println();
  }

}


void loop()
{
}
