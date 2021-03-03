# RTIMULib - a versatile C++ and Python 9-dof, 10-dof and 11-dof IMU library

RTIMULib is the simplest way to connect a 9-dof, 10-dof or 11-dof IMU to an embedded Linux system and obtain Kalman-filtered quaternion or Euler angle pose data. Basically, two simple funtion calls (IMUInit() and IMURead()) are pretty much all that's needed to integrate RTIMULib.

## Have questions, need help or want to comment?

Please use the richards-tech user forum at https://groups.google.com/forum/#!forum/richards-tech-user-forum.

## Features

The Linux directory contains the main demo apps for embeeded Linux systems:

* RTIMULibDrive is a simple app that shows to to use the RTIMULib library in a basic way.
* RTIMULibDrive10 adds support for pressure/temperature sensors.
* RTIMULibDrive11 adds support for pressure/temperature/humidity sensors.
* RTIMULibCal is a command line calibration tool for the magnetometers and accelerometers.
* RTIMULibvrpn shows how to use RTIMULib with vrpn.
* RTIMULibDemo is a simple GUI app that displays the fused IMU data in real-time.
* RTIMULibDemoGL adds OpenGL visualization to RTIMULibDemo.

RTIMULib is a C++ library but there are also Python bindings in Linux/python. It's easy to build and install the Python RTIMULib library using the provided setup.py after which any Python script will have access to RTIMULib functionality.
See [Linux/python.README.md](Linux/python/README.md) for more details.
Two demo scripts show how to use the Python interface.

Check out www.richards-tech.com for more details, updates and news.

RTIMULib currently supports the following IMUs:

* InvenSense MPU-9150 single chip IMU.
* InvenSense MPU-6050 plus HMC5883 magnetometer on MPU-6050's aux bus (handled by the MPU-9150 driver).
* InvenSense MPU-6050 gyros + acclerometers. Treated as MPU-9150 without magnetometers.
* InvenSense MPU-9250 single chip IMU (I2C and SPI).
* STM LSM9DS0 single chip IMU.
* STM LSM9DS1 single chip IMU.
* L3GD20H + LSM303D (optionally with the LPS25H) as used on the Pololu AltIMU-10 v4.
* L3GD20 + LSM303DLHC as used on the Adafruit 9-dof (older version with GD20 gyro) IMU. 
* L3GD20H + LSM303DLHC (optionally with BMP180) as used on the new Adafruit 10-dof IMU.
* Bosch BMX055 (although magnetometer support is experimental currently).
* Bosch BNO055 IMU with onchip fusion. Note: will not work reliably with RaspberryPi/Pi2 due to clock-stretching issues.

The LSM9DS1 implementation was generously supplied by XECDesign.

Pressure/temperature sensing is supported for the following pressure sensors:

* BMP180
* LPS25H
* MS5611
* MS5637

Humidity/temperature sensing is supported for the following humidity sensors:

* HTS221
* HTU21D

The humidity infrastructure and HTS221 support was generously supplied by XECDesign. It follows the model used by the pressure infrastructure - see RTIMULibDrive11 for an example of how to use this.

Note that currently only pressure and humidity sensors connected via I2C are supported. Also, an MS5637 sensor will be auto-detected as an MS5611. To get the correct processing for the MS5637, edit the RTIMULib.ini file and set PressureType=5.

By default, RTIMULib will try to autodiscover IMUs, pressure and humidity sensors on I2C and SPI busses (only IMUs on the SPI bus). This will use I2C bus 1 and SPI bus 0 although this can be changed by hand editing the .ini settings file (usually called RTIMULib.ini) loaded/saved in the current working directory by any of the RTIMULib apps. RTIMULib.ini is self-documenting making it easy to edit. Alternatively, RTIMULibDemo and RTIMULibDemoGL provide a GUI interface for changing some of the major settings in the .ini file.

RTIMULib also supports multiple sensor integration fusion filters such as Kalman filters.

Two types of platforms are supported:

* Embedded Linux. RTIMULib is supported for the Raspberry Pi (Raspbian) and Intel Edison. Demo apps for these can be found in the Linux directory and instructions for building and running can be found there. Its prerequisites are very simple - just I2C support on the target system along with the standard build-essential (included in the Raspberry Pi Raspbian distribution by default).

* Desktop (Ubuntu/Windows/Mac). There are two apps (RTHostIMU and RTHostIMUGL) that allow the sensor fusion to be separated from the sensor interfacing and data collection. An Arduino (running the RTArduLinkIMU sketch from the RTIMULib-Arduino repo) fitted with an IMU chip collects the sensor data and sends it to the desktop. RTHostIMU and RTHostIMUGL (this one has an OpenGL visualization of the data) communicate with the Arduino via a USB connection.

The MPU-9250 and SPI driver code is based on code generously supplied by staslock@gmail.com (www.clickdrive.io). I am sure that any bugs that may exist are due to my integration efforts and not the quality of the supplied code!

RTIMULib is licensed under the MIT license.

## Repo structure

### RTIMULib

This is the actual RTIMULib library source. Custom apps only need to include this library.

### Linux

This directory contains the embedded Linux demo apps (for Raspberry Pi and Intel Edison) and also the Python interface to RTIMULib.

### RTHost

RTHost contains the two apps, RTHost and RTHostGL, that can be used by desktops that don't have direct connection to an IMU (as they don't have I2C or SPI interfaces). An Arduino running RTArduLinkIMU from the RTIMULib-Arduino repo provides the hardware interface and a USB cable provides the connection between the desktop and the Arduino.

### RTEllipsoidFit

This contains Octave code used by the ellipsiod fit data generation in RTIMULibCal, RTIMULibDemo, RTIMULibDemoGL, RTHostIMU and RTHostIMUGL. It's important that a copy of this directory is at the same level, or the one above, the app's working directory or ellipsoid fit data generation will fail.

## Note about magnetometer (compass) calibration

It is essential to calibrate the magnetometer or else very poor fusion results will be obtained. For more about this, see http://wp.me/p4qcHg-b4. RTIMULibDemo (GUI) and RTIMULibCal (command line) can be used to do this. They both support magnetometer min/max, magnetometer ellipsoid fit and accelerometer min/max calibration.

Also, if using a non-standard axis rotation (see http://wp.me/p4qcHg-cO), magnetometer calibration (and accelerometer calibration if that has been performed) MUST be run AFTER changing the axis rotation.

## Next Steps

SyntroPiNav (an app for the Raspberry Pi) and SyntroNavView can be used as a convenient system to experiment with IMU chips, drivers and filters. SyntroPiNav runs on the Pi and transmits IMU data along with filter outputs over a LAN to SyntroNavView running on an Ubuntu PC. SyntroNavView also displays the data and provides a 3D graphics view of the calculated pose.

Since all IMU data is sent to SyntroNavView, SyntroNavView can run its own local filter. This makes it a very convenient testbed for new filter development as the speed of the desktop can be used to accelerate implementation and testing. When ready, the updated RTIMULib can be compiled into SyntroPiNav and should work exactly the same as on the desktop.

SyntroPiNav is available as part of the richards-tech SyntroPiApps repo (https://github.com/richards-tech/SyntroPiApps) while SyntroNavView is available as part of the richards-tech SyntroApps repo (https://github.com/richards-tech/SyntroApps).

## Release history

### June 22 2015 - 7.2.1

Improvement to Linux CMakeLists.txt. Added temp comp to HTU21D.

### June 21 2015 - 7.2.0

Added support for HTU21D humidity sensor.

### June 17 2015 - 7.1.0

Added humidity support to the demo apps and created RTIMULibDrive11 and Fusion11.py. Fixed some related build problems. Updated some source headers.

### June 15 2015 - 7.0.3

Improved CMake versioning system.

### June 12 2015 - 7.0.2

Also added SOVERSION to RTIMULibGL build via CMake. Note - on Linux it may be necessary to run "sudo ldconfig" after building.

### June 12 2015 - 7.0.1

Added SOVERSION to CMake build library.

### June 9 2015 - 7.0.0

New humidity infrastructure and HTS221 support added. Thanks to XECDesign for this. Due to lack of hardware for testing at this time, this release is somewhat experimental - use 6.3.0 if problems are encountered.

LSM9DS1 support added.

### May 17 2015 - 6.3.0

Added support for the BNO055. The BNO055 always uses its onchip fusion rather than RTIMULib filters. This will not work reliably with the Raspberry Pi/Pi2 due to clock-stretching issues.

### April 30 2015 - 6.2.1

Added second order temperature compensation for MS5611 and MS5637. MS5637 still seems to be affected by temperature - this is being investigated.

### April 24 2015 - 6.2.0

Add support for Bosch BMX055 IMU and MS5637 pressure sensor. See notes above about auto-detection for the MS5637.

The BMX055 implementation is slightly experimental as the magnetometer results show significant asymmetry about zero. The processing of the magnetometer data is fairly complex and there could be an error in it. Calibration is essential for this IMU at the moment.

### March 31 2015 - 6.1.0

Allow RTQF Slerp power to be changed while running while fusion running. Some performance improvements and cleanups.

### March 29 2015 - 6.0.0

Changed RTQF state correction mechanism to use quaternion SLERP. This is a little experimental - if you encounter problems, please use the 5.6.0 release (from the Releases tab).

### March 21 2015 - 5.6.0

Added support for MPU6050 + HMC5883 IMUs (HMC5883 on MPU-6050's aux bus).

### March 20 2015 - 5.5.0

Added support for the MS5611 pressure sensor and also modified MPU-9150 driver to also support the MPU-6050.

### February 21 2015 - 5.4.0

Python API now works with Python 2.7 and Python 3.4.

Changed MPU9150 and MPU9250 drivers so that compass adjust is performed before axis swap.

### January 31 2015 - 5.3.0

Added abilty to set magnetic declination in the .ini file. This value in radians is subtracted from the measured heading before being used by the fusion algorithm.

### January 24 2015 - 5.2.3

Fixed problem with CMakeLists.txt for RTIMULibGL.

### January 19 2015 - 5.2.2

Improved some CMakeLists. RTIMULib can now be built with cmake independently.

### December 29 2014 - 5.2.1

Some improvements to the RTHost CMakelists.txt. Changed Visual Studio version to VS2013.

### December 29 2014 - 5.2.0

Added support for vrpn. There is a new demo app, RTIMULibvrpn, that shows how this works.

RTSettings constructor now optionally allows the directory used for the .ini settings file to be specified. The original constructor uses the working directory whereas the additional constructor allows this bevaiour to be overridden.

Changed install directory to /usr/local/bin when using the supplid Makefiles and qmake instead of /usr/bin. This is to be consistent with cmake-generated makefiles.

### December 15 2014 - 5.1.0

Added support for the LPS25H pressure/temperature sensor.

Addeed support for SPI chip select 1 in addition to chip select 0. A new field, SPISelect, has been added. RTIMULib will try to autodetect SPI bus 0, select 0 and SPI bus 0 select 1 but others can be used if the RTIMULib.ini file is hand-edited.

### December 10 2014 - 5.0.0

Top level directory structure completely reorganized.

Support for pressure sensors.

New demo app, RTIMULibDrive10, to demonstrate use of pressure sensors with RTIMULib.

RTIMULibDemo and RTIMULibDemoGL upgraded to support pressure sensors.

Python library upgraded to support pressure sensors. A new demo script, Fusion10.py, shows how to use the interface.

### December 3 2014 - 4.4.0

Added RTIMULibDemoGL and reorganized the OpenGL components.

### December 3 2014 - 4.3.2

CMake build system now works for the RTHostIMU and RTHostIMUGL apps on Windows and Mac OS X. The full set of apps are built on Linux.

Some minor driver fixes.

RTHostIMUGL now runs on the Raspberry Pi. Simpler (non-ADS) shading is used to imporve performance.

### December 2 2014 - 4.3.1

Fixed the CMakeLists.txt for RTIMULibDemo.

### December 2 2014 - 4.3.0

Added cmake support (see build instructions for more info). This was based on work by Moritz Fischer at ettus.com. As part of this, the qextserialport folder was renamed to RTSerialPort. There are also some small fixes in the MPU-9150/9250 and GD20HM303D drivers.

### November 18 2014 - 4.2.0

Add the IMU axis rotation capability to better handle IMUs in non-standard orientations. See http://wp.me/p4qcHg-cO for more details of how to use this capability.

### November 14 2014 - 4.1.0

Corrected some problems with the continuous gyro bias update system. There is a new function in RTIMU called setGyroContinuousLearningAlpha() that allows the continuous learning rate to be set. RTIMULIB uses a rapid learning rate to collect the initial gyro bias data but then uses a much slower rate for continuous tracking of bias. This function allows the rate to be set if necessary - values can be between 0.0 and 1.0. Setting it to 0.0 turns off continuous learning completely so that gyro bias calculation only occurs during the rapid learning period.

loadSettings() and saveSettings() in RTSettings.cpp are now virtual to give more flexibility in how settings are stored.

### November 8 2014 - 4.0.1

Fixed some missing MPU-9250 related defs in python interface.

### November 7 2014 - 4.0.0

Restructured library to add support for the MPU-9250 and SPI bus. This is a little experimental right now - use V3.1.1 if problems are encountered with existing supported IMUs. The MPU-9250 has been seen to hang when used on the SPI bus at sample rates above 300 samples per second. However, sample rates up to 1000 seem to work fine using I2C.

The RTIMULib apps are now able to auto-detect on the I2C and SPI bus so, if only one supported IMU is present on either bus, the code should find it. Note that only the MPU-9250 is supported by the SPI driver at the moment. There are some new settings in the RTIMULib.ini file related to the SPI bus that may need editing in some systems. The default SPI bus is set 0 which works nicely for the Raspberry Pi. Connect the MPU-9250 to SPI0 and CS0 and it should work without needing to change anythin in RTIMULib.ini.

### November 4 2014 - 3.1.1

Can now supply the .ini name as a command line argument. For example:

    RTIMULibCal Palm
    
would calibrate a settings file called Palm.ini.

### November 1 2014 - 3.1.0

Added the RTIMULibCal application. This implements IMU calibration in no GUI (command line) environments.

### October 13 2014 - 3.0.3

Increased time allowed for ellipse fitting to complete before declaring it finished.

### October 13 2014 - 3.0.2

Added license information to Calibrate.pdf.

### October 13 2014 - 3.0.1

Fixed missing license header in RTEllipsoidFit.

### October 13 2014 - 3.0.0

RTIMULib now support accelerometer calibration and enhanced magnetometer calibration using ellipsoid fitting. Please check the Calibration.pdf document for instructions on how to create the calibration data.

### October 8 2014 - 2.1.2

Fixed some missed license header changes.

### October 8 2014 - 2.1.1

Fixed bug where the first com put was missed on the GUI dropdown in RTHostIMU and RTHostIMUGL.

### October 8 2014 - 2.1.0

Changed license to MIT.

Added RTHostIMU and RTHostIMUGL. These apps use RTArduLink to connect the host system to an IMU connected to an Arduino. This allows processing to be split between the Arduino and the host system. Sensor data collection is performed on the Arduino, sensor fusion and display is performed on the host. This means that the apps will run on hosts without I2C ports (such as PCs). See below for more details.

### October 2 2014 - 2.0.0

Changed the gyro bias calculation to run automatically when the IMU is detected as being stable. This means
that the IMU no longer needs to be kept still for 5 seconds after restart and gyro bias is continually tracked. IMUGyroBiasValid can be called to check if enough stable samples have been obtained for a reasonable bias calculation to be made. If the IMU is stable, this will normally occur within 5 seconds. If not stable at the start, it may take longer but it will occur eventually once enough stable samples have been obtained. If RTIMULibDemo never indicates a valid bias, the #defines RTIMU_FUZZY_GYRO_ZERO and/or RTIMU_FUZZY_ACCEL_ZERO may need to be increased if the gyro bias or accelerometer noise is unusually high. These should be set to be greater than the readings observed using RTIMULibDemo when the IMU is completely stable. In the case of the gyros, this should be the absolute values when the IMU isn't being moved. In the case of the accels, this should be the maximum change in values when the IMU isn't being moved.

Stable gyro bias values are saved to the RTIMULib.ini file in order to speed up restarts. The values will once again be 
updated after enough stable samples have been obtained in order to track long term changes in gyro bias.

If problems are encountered, try version 1.0.4 which is available under the GitHub repo releases tab. Please also report any issues via the GitHub issue system to help improve RTIMULib!

### September 3 2014 - 1.0.4

Fixed message error in RTIMUSettings.

### September 2 2014 - 1.0.3

CompassCalMax was returning m_compassCalMin in PyRTIMU_settings.cpp - changed to max instead. Thanks to Stefan Grufman for finding that.

### August 6 2014 - 1.0.2

Added missing compass sample rate defines for LSM303DLHC and updated settings comments. Thanks to Bill Robertson (broberts4) for spotting that!

### July 29 2014 - 1.0.1

Fixed the python getIMUData function.

### July 7 2014 - 1.0.0

#### Added python bindings

Thanks to avishorp for the python code and Luke Heidelberger for a bug fix.

### April 13 2014 - 0.9.4

#### Added new RTQF fusion filter

RTQF is a very highly stripped down Kalman filter that avoids matrix inversion and lot of other matrix operations. It makes a small performance difference on the Raspberry Pi but would have more impact on lower powered processors.

### April 10 2014 - 0.9.3

#### STM LSM9DS0 IMU Implementation now working

The single chip IMU LSM9DS0 is now working with RTIMULib. An example breakout is available from Sparkfun - https://www.sparkfun.com/products/12636.

### April 9 2014 - 0.9.2

#### STM L3GD20H + LSM303D IMU Implementation now working

The combination of the L3GD20H gyro and LSM303D accel/mag chip is now working. The physical configuration supported is as used on the Pololu Altimu V3 - http://www.pololu.com/product/2469. The pressure chip on the 10-dof version will be supported shortly but 9-dof is working now.

#### STM L3GD20 + LSM303DLHC IMU Implementation now working

The combination of the L3GD20 and LSM303DLHC accel/mag chip is now working. The physical configuration supported is as used on the Adafruit 9-dof IMU - http://www.adafruit.com/products/1714.

### April 7 2014 - 0.9.1

#### Improved performance with MPU-9150

A new caching strategy for the MPU-9150 seems to be achieving 1000 samples per second without fifo overflows using a 900MHz Raspberry Pi and 400kHz I2C bus. This is as reported by RTIMULibDrive with a CPU utilization of 28%. RTIMULibDemo manages 890 samples per second with the MPU-9150 set to 1000 samples per second. The driver gracefully handles this situation although there is increased delay when the application cannot handle the full sample rate.

#### Auto detection of IMU

RTIMULib can now scan for supported IMUs and configure automatically. This is the default behavior now. It handles IMUs at alternate address automatically as well (for example, it will detect an MPU-9150 at 0x68 or 0x69).

#### Partial support for STM L3GD20H/LSM303D IMUs

This is in a very early state and only supports the gyro sensor at the moment.

### April 4 2014 - 0.9.0

Initial release with support for MPU9150.





