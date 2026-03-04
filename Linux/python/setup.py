#////////////////////////////////////////////////////////////////////////////
#//
#//  This file is part of RTIMULib
#//
#//  Copyright (c) 2014-2015, richards-tech
#//  Copyright (c) 2014, avishorp
#//
#//  Permission is hereby granted, free of charge, to any person obtaining a copy of
#//  this software and associated documentation files (the "Software"), to deal in
#//  the Software without restriction, including without limitation the rights to use,
#//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
#//  Software, and to permit persons to whom the Software is furnished to do so,
#//  subject to the following conditions:
#//
#//  The above copyright notice and this permission notice shall be included in all
#//  copies or substantial portions of the Software.
#//
#//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
#//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
#//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

from setuptools import setup, Extension
import os.path

RTIMU_sources = [
    "RTMath.cpp",
    "RTIMUHal.cpp",
    "RTFusion.cpp",
    "RTFusionKalman4.cpp",
    "RTFusionRTQF.cpp",
    "RTIMUSettings.cpp",
    "IMUDrivers/RTIMU.cpp",
    "IMUDrivers/RTIMUNull.cpp",
    "IMUDrivers/RTIMUMPU9150.cpp",
    "IMUDrivers/RTIMUMPU9250.cpp",
    "IMUDrivers/RTIMUGD20HM303D.cpp",
    "IMUDrivers/RTIMUGD20HM303DLHC.cpp",
    "IMUDrivers/RTIMUGD20M303DLHC.cpp",
    "IMUDrivers/RTIMULSM9DS0.cpp",
    "IMUDrivers/RTIMULSM9DS1.cpp",
    "IMUDrivers/RTIMUBMX055.cpp",
    "IMUDrivers/RTIMUBNO055.cpp",
    "IMUDrivers/RTPressure.cpp",
    "IMUDrivers/RTPressureBMP180.cpp",
    "IMUDrivers/RTPressureLPS25H.cpp",
    "IMUDrivers/RTPressureMS5611.cpp",
    "IMUDrivers/RTPressureMS5637.cpp",
    "IMUDrivers/RTHumidity.cpp",
    "IMUDrivers/RTHumidityHTS221.cpp",
    "IMUDrivers/RTHumidityHTU21D.cpp",
   ]
RTIMU_sourcedir = "../../RTIMULib"

mod = Extension('RTIMU',
                sources = ['PyRTIMU.cpp', 'PyRTIMU_Settings.cpp', 'PyRTIMU_RTIMU.cpp', 
                'PyRTIMU_RTPressure.cpp', 'PyRTIMU_RTHumidity.cpp'] +
                [ os.path.join(RTIMU_sourcedir, sr) for sr in RTIMU_sources],
                include_dirs = [RTIMU_sourcedir],
                extra_compile_args = ['-std=c++0x'],
                define_macros = [("HAL_QUIET", None)]
                )

setup (name = 'RTIMULib',
       version = '7.2.1',
       description = 'richards-tech IMU Sensor Fusion Library',
       ext_modules = [mod])
