////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef RTIMUCALDEFS_H
#define RTIMUCALDEFS_H

#define RTIMUCALDEFS_DEFAULT_MIN        1000                // a large min
#define RTIMUCALDEFS_DEFAULT_MAX        -1000               // a small max

#define	RTIMUCALDEFS_MAX_MAG_SAMPLES	   20000            // max saved mag records

#define RTIMUCALDEFS_OCTANT_MIN_SAMPLES    200              // must have at least this in each octant

#define RTIMUCALDEFS_ELLIPSOID_MIN_SPACING  0.1f            // min distnace between ellipsoid samples to be recorded

//  Octant defs

#define RTIMUCALDEFS_OCTANT_COUNT       8                   // there are 8 octants of course

#define RTIMUCALDEFS_OCTANT_NNN         0                   // x, y, z all negative
#define RTIMUCALDEFS_OCTANT_PNN         1                   // x positive - y, z neagtive
#define RTIMUCALDEFS_OCTANT_NPN         2                   // y positive - x, z negative
#define RTIMUCALDEFS_OCTANT_PPN         3                   // x, y positive - z negative
#define RTIMUCALDEFS_OCTANT_NNP         4                   // z positive - x, y negative
#define RTIMUCALDEFS_OCTANT_PNP         5                   // x, z positive - y negative
#define RTIMUCALDEFS_OCTANT_NPP         6                   // y, z positive - x negative
#define RTIMUCALDEFS_OCTANT_PPP         7                   // x, y, z all positive

//  File name for Octave processing

#define RTIMUCALDEFS_MAG_RAW_FILE          "magRaw.dta"     // the raw sample file - input to ellispoid fit code
#define RTIMUCALDEFS_MAG_CORR_FILE         "magCorr.dta"    // the output from the ellipsoid fit code

#define RTIMUCALDEFS_OCTAVE_PATH           "/usr/share/librtimulib-utils/RTEllipsoidFit"
#define RTIMUCALDEFS_OCTAVE_CODE           "RTEllipsoidFit.m"
#define RTIMUCALDEFS_OCTAVE_COMMAND        "octave --path " RTIMUCALDEFS_OCTAVE_PATH " " RTIMUCALDEFS_OCTAVE_PATH "/" RTIMUCALDEFS_OCTAVE_CODE

#endif // RTIMUCALDEFS_H
