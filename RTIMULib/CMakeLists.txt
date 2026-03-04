#////////////////////////////////////////////////////////////////////////////
#//
#//  This file is part of RTIMULib
#//
#//  Copyright (c) 2014-2015, richards-tech
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

#// The cmake support was based on work by Moritz Fischer at ettus.com.
#// Original copyright notice:
#
# Copyright 2014 Ettus Research LLC
#

########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(RTIMULib CXX)
ENABLE_TESTING()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../RTIMULibVersion.txt)

SET(LIBRTIMU_SRCS
    RTFusion.cpp
    RTFusionRTQF.cpp
    RTMath.cpp
    RTFusionKalman4.cpp
    RTIMUAccelCal.cpp
    RTIMUHal.cpp
    RTIMUMagCal.cpp
    RTIMUSettings.cpp
    IMUDrivers/RTIMU.cpp
    IMUDrivers/RTIMUGD20M303DLHC.cpp
    IMUDrivers/RTIMUGD20HM303DLHC.cpp
    IMUDrivers/RTIMUGD20HM303D.cpp
    IMUDrivers/RTIMULSM9DS0.cpp
    IMUDrivers/RTIMULSM9DS1.cpp
    IMUDrivers/RTIMUMPU9150.cpp
    IMUDrivers/RTIMUMPU9250.cpp
    IMUDrivers/RTIMUBMX055.cpp
    IMUDrivers/RTIMUBNO055.cpp
    IMUDrivers/RTIMUNull.cpp
    IMUDrivers/RTPressure.cpp
    IMUDrivers/RTPressureBMP180.cpp
    IMUDrivers/RTPressureLPS25H.cpp
    IMUDrivers/RTPressureMS5611.cpp
    IMUDrivers/RTPressureMS5637.cpp
    IMUDrivers/RTHumidity.cpp
    IMUDrivers/RTHumidityHTS221.cpp
    IMUDrivers/RTHumidityHTU21D.cpp
)

IF(WIN32 AND QT5)
    FIND_PACKAGE(Qt5Widgets)
    FIND_PACKAGE(Qt5Gui)
    qt5_wrap_ui(UI_HEADERS RTIMULibDemo.ui)
    ADD_LIBRARY(RTIMULib STATIC ${LIBRTIMU_SRCS})
    qt5_use_modules(RTIMULib Widgets Gui)
ENDIF(WIN32 AND QT5)

IF(WIN32 AND (NOT QT5))
    FIND_PACKAGE(Qt4 REQUIRED)
    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})
    ADD_LIBRARY(RTIMULib STATIC ${LIBRTIMU_SRCS})
    TARGET_LINK_LIBRARIES(RTIMULib ${QT_LIBRARIES})
ENDIF(WIN32 AND (NOT QT5))

IF(UNIX)
    ADD_LIBRARY(RTIMULib SHARED ${LIBRTIMU_SRCS})
    SET_PROPERTY(TARGET RTIMULib PROPERTY VERSION ${RTIMULIB_VERSION})
    SET_PROPERTY(TARGET RTIMULib PROPERTY SOVERSION ${RTIMULIB_VERSION_MAJOR})
    INSTALL(TARGETS RTIMULib DESTINATION lib)
    INSTALL(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.h")
ENDIF(UNIX)

