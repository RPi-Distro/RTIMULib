#////////////////////////////////////////////////////////////////////////////
#//
#//  This file is part of RTIMULib
#//
#//  Copyright (c) 2014, richards-tech
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
PROJECT(RTIMUHOST CXX)
ENABLE_TESTING()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../RTIMULib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTIMULibGL)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTIMULibGL/QtGLLib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTIMULibGL/VRWidgetLib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTArduLinkHost)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTHostIMUCommon)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTSerialPort/src)

IF(APPLE)
cmake_policy(SET CMP0042 NEW)
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")
ENDIF(APPLE)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../RTIMULib RTIMULib)
ADD_SUBDIRECTORY(RTIMULibGL)
ADD_SUBDIRECTORY(RTSerialPort)
ADD_SUBDIRECTORY(RTArduLinkHost)
ADD_SUBDIRECTORY(RTHostIMUCommon)
ADD_SUBDIRECTORY(RTHostIMU)
ADD_SUBDIRECTORY(RTHostIMUGL)

MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
