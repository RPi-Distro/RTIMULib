////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech
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


#include "RTIMU.h"
#include "RTFusionKalman4.h"
#include "RTFusionRTQF.h"

#include "RTIMUNull.h"
#include "RTIMUMPU9150.h"
#include "RTIMUMPU9250.h"
#include "RTIMUGD20HM303D.h"
#include "RTIMUGD20M303DLHC.h"
#include "RTIMUGD20HM303DLHC.h"
#include "RTIMULSM9DS0.h"
#include "RTIMULSM9DS1.h"
#include "RTIMUBMX055.h"
#include "RTIMUBNO055.h"

//  this sets the learning rate for compass running average calculation

#define COMPASS_ALPHA 0.2f

//  this defines the accelerometer noise level

#define RTIMU_FUZZY_GYRO_ZERO      0.20

//  this defines the accelerometer noise level

#define RTIMU_FUZZY_ACCEL_ZERO      0.05

//  Axis rotation arrays

float RTIMU::m_axisRotation[RTIMU_AXIS_ROTATION_COUNT][9] = {
    {1, 0, 0, 0, 1, 0, 0, 0, 1},                    // RTIMU_XNORTH_YEAST
    {0, -1, 0, 1, 0, 0, 0, 0, 1},                   // RTIMU_XEAST_YSOUTH
    {-1, 0, 0, 0, -1, 0, 0, 0, 1},                  // RTIMU_XSOUTH_YWEST
    {0, 1, 0, -1, 0, 0, 0, 0, 1},                   // RTIMU_XWEST_YNORTH

    {1, 0, 0, 0, -1, 0, 0, 0, -1},                  // RTIMU_XNORTH_YWEST
    {0, 1, 0, 1, 0, 0, 0, 0, -1},                   // RTIMU_XEAST_YNORTH
    {-1, 0, 0, 0, 1, 0, 0, 0, -1},                  // RTIMU_XSOUTH_YEAST
    {0, -1, 0, -1, 0, 0, 0, 0, -1},                 // RTIMU_XWEST_YSOUTH

    {0, 1, 0, 0, 0, -1, -1, 0, 0},                  // RTIMU_XUP_YNORTH
    {0, 0, 1, 0, 1, 0, -1, 0, 0},                   // RTIMU_XUP_YEAST
    {0, -1, 0, 0, 0, 1, -1, 0, 0},                  // RTIMU_XUP_YSOUTH
    {0, 0, -1, 0, -1, 0, -1, 0, 0},                 // RTIMU_XUP_YWEST

    {0, 1, 0, 0, 0, 1, 1, 0, 0},                    // RTIMU_XDOWN_YNORTH
    {0, 0, -1, 0, 1, 0, 1, 0, 0},                   // RTIMU_XDOWN_YEAST
    {0, -1, 0, 0, 0, -1, 1, 0, 0},                  // RTIMU_XDOWN_YSOUTH
    {0, 0, 1, 0, -1, 0, 1, 0, 0},                   // RTIMU_XDOWN_YWEST

    {1, 0, 0, 0, 0, 1, 0, -1, 0},                   // RTIMU_XNORTH_YUP
    {0, 0, -1, 1, 0, 0, 0, -1, 0},                  // RTIMU_XEAST_YUP
    {-1, 0, 0, 0, 0, -1, 0, -1, 0},                 // RTIMU_XSOUTH_YUP
    {0, 0, 1, -1, 0, 0, 0, -1, 0},                  // RTIMU_XWEST_YUP

    {1, 0, 0, 0, 0, -1, 0, 1, 0},                   // RTIMU_XNORTH_YDOWN
    {0, 0, 1, 1, 0, 0, 0, 1, 0},                    // RTIMU_XEAST_YDOWN
    {-1, 0, 0, 0, 0, 1, 0, 1, 0},                   // RTIMU_XSOUTH_YDOWN
    {0, 0, -1, -1, 0, 0, 0, 1, 0}                   // RTIMU_XWEST_YDOWN
};

RTIMU *RTIMU::createIMU(RTIMUSettings *settings)
{
    switch (settings->m_imuType) {
    case RTIMU_TYPE_MPU9150:
        return new RTIMUMPU9150(settings);

    case RTIMU_TYPE_GD20HM303D:
        return new RTIMUGD20HM303D(settings);

    case RTIMU_TYPE_GD20M303DLHC:
        return new RTIMUGD20M303DLHC(settings);

    case RTIMU_TYPE_LSM9DS0:
        return new RTIMULSM9DS0(settings);
    case RTIMU_TYPE_LSM9DS1:
        return new RTIMULSM9DS1(settings);

    case RTIMU_TYPE_MPU9250:
        return new RTIMUMPU9250(settings);

    case RTIMU_TYPE_GD20HM303DLHC:
        return new RTIMUGD20HM303DLHC(settings);

    case RTIMU_TYPE_BMX055:
        return new RTIMUBMX055(settings);

    case RTIMU_TYPE_BNO055:
        return new RTIMUBNO055(settings);

    case RTIMU_TYPE_AUTODISCOVER:
        if (settings->discoverIMU(settings->m_imuType, settings->m_busIsI2C, settings->m_I2CSlaveAddress)) {
            settings->saveSettings();
            return RTIMU::createIMU(settings);
        }
        return new RTIMUNull(settings);

    case RTIMU_TYPE_NULL:
        return new RTIMUNull(settings);

    default:
        return NULL;
    }
}


RTIMU::RTIMU(RTIMUSettings *settings)
{
    m_settings = settings;

    m_compassCalibrationMode = false;
    m_accelCalibrationMode = false;

    switch (m_settings->m_fusionType) {
    case RTFUSION_TYPE_KALMANSTATE4:
        m_fusion = new RTFusionKalman4();
        break;

    case RTFUSION_TYPE_RTQF:
        m_fusion = new RTFusionRTQF();
        break;

    default:
        m_fusion = new RTFusion();
        break;
    }
    HAL_INFO1("Using fusion algorithm %s\n", RTFusion::fusionName(m_settings->m_fusionType));
}

RTIMU::~RTIMU()
{
    delete m_fusion;
    m_fusion = NULL;
}

void RTIMU::setCalibrationData()
{
    float maxDelta = -1;
    float delta;

    if (m_settings->m_compassCalValid) {
        //  find biggest range

        for (int i = 0; i < 3; i++) {
            if ((m_settings->m_compassCalMax.data(i) - m_settings->m_compassCalMin.data(i)) > maxDelta)
                maxDelta = m_settings->m_compassCalMax.data(i) - m_settings->m_compassCalMin.data(i);
        }
        if (maxDelta < 0) {
            HAL_ERROR("Error in compass calibration data\n");
            return;
        }
        maxDelta /= 2.0f;                                       // this is the max +/- range

        for (int i = 0; i < 3; i++) {
            delta = (m_settings->m_compassCalMax.data(i) - m_settings->m_compassCalMin.data(i)) / 2.0f;
            m_compassCalScale[i] = maxDelta / delta;            // makes everything the same range
            m_compassCalOffset[i] = (m_settings->m_compassCalMax.data(i) + m_settings->m_compassCalMin.data(i)) / 2.0f;
        }
    }

    if (m_settings->m_compassCalValid) {
        HAL_INFO("Using min/max compass calibration\n");
    } else {
        HAL_INFO("min/max compass calibration not in use\n");
    }

    if (m_settings->m_compassCalEllipsoidValid) {
        HAL_INFO("Using ellipsoid compass calibration\n");
    } else {
        HAL_INFO("Ellipsoid compass calibration not in use\n");
    }

    if (m_settings->m_accelCalValid) {
        HAL_INFO("Using accel calibration\n");
    } else {
        HAL_INFO("Accel calibration not in use\n");
    }
}

bool RTIMU::setGyroContinuousLearningAlpha(RTFLOAT alpha)
{
    if ((alpha < 0.0) || (alpha >= 1.0))
        return false;

    m_gyroContinuousAlpha = alpha;
    return true;
}


void RTIMU::gyroBiasInit()
{
    m_gyroLearningAlpha = 2.0f / m_sampleRate;
    m_gyroContinuousAlpha = 0.0f / m_sampleRate;
    m_gyroSampleCount = 0;
}

//  Note - code assumes that this is the first thing called after axis swapping
//  for each specific IMU chip has occurred.

void RTIMU::handleGyroBias()
{
    // do axis rotation

    if ((m_settings->m_axisRotation > 0) && (m_settings->m_axisRotation < RTIMU_AXIS_ROTATION_COUNT)) {
        // need to do an axis rotation
        float *matrix = m_axisRotation[m_settings->m_axisRotation];
        RTIMU_DATA tempIMU = m_imuData;

        // do new x value
        if (matrix[0] != 0) {
            m_imuData.gyro.setX(tempIMU.gyro.x() * matrix[0]);
            m_imuData.accel.setX(tempIMU.accel.x() * matrix[0]);
            m_imuData.compass.setX(tempIMU.compass.x() * matrix[0]);
        } else if (matrix[1] != 0) {
            m_imuData.gyro.setX(tempIMU.gyro.y() * matrix[1]);
            m_imuData.accel.setX(tempIMU.accel.y() * matrix[1]);
            m_imuData.compass.setX(tempIMU.compass.y() * matrix[1]);
        } else if (matrix[2] != 0) {
            m_imuData.gyro.setX(tempIMU.gyro.z() * matrix[2]);
            m_imuData.accel.setX(tempIMU.accel.z() * matrix[2]);
            m_imuData.compass.setX(tempIMU.compass.z() * matrix[2]);
        }

        // do new y value
        if (matrix[3] != 0) {
            m_imuData.gyro.setY(tempIMU.gyro.x() * matrix[3]);
            m_imuData.accel.setY(tempIMU.accel.x() * matrix[3]);
            m_imuData.compass.setY(tempIMU.compass.x() * matrix[3]);
        } else if (matrix[4] != 0) {
            m_imuData.gyro.setY(tempIMU.gyro.y() * matrix[4]);
            m_imuData.accel.setY(tempIMU.accel.y() * matrix[4]);
            m_imuData.compass.setY(tempIMU.compass.y() * matrix[4]);
        } else if (matrix[5] != 0) {
            m_imuData.gyro.setY(tempIMU.gyro.z() * matrix[5]);
            m_imuData.accel.setY(tempIMU.accel.z() * matrix[5]);
            m_imuData.compass.setY(tempIMU.compass.z() * matrix[5]);
        }

        // do new z value
        if (matrix[6] != 0) {
            m_imuData.gyro.setZ(tempIMU.gyro.x() * matrix[6]);
            m_imuData.accel.setZ(tempIMU.accel.x() * matrix[6]);
            m_imuData.compass.setZ(tempIMU.compass.x() * matrix[6]);
        } else if (matrix[7] != 0) {
            m_imuData.gyro.setZ(tempIMU.gyro.y() * matrix[7]);
            m_imuData.accel.setZ(tempIMU.accel.y() * matrix[7]);
            m_imuData.compass.setZ(tempIMU.compass.y() * matrix[7]);
        } else if (matrix[8] != 0) {
            m_imuData.gyro.setZ(tempIMU.gyro.z() * matrix[8]);
            m_imuData.accel.setZ(tempIMU.accel.z() * matrix[8]);
            m_imuData.compass.setZ(tempIMU.compass.z() * matrix[8]);
        }
    }

    RTVector3 deltaAccel = m_previousAccel;
    deltaAccel -= m_imuData.accel;   // compute difference
    m_previousAccel = m_imuData.accel;

    if ((deltaAccel.length() < RTIMU_FUZZY_ACCEL_ZERO) && (m_imuData.gyro.length() < RTIMU_FUZZY_GYRO_ZERO) && (m_settings->m_gyroBiasValid == false)) {
        // what we are seeing on the gyros should be bias only so learn from this

        if (m_gyroSampleCount < (5 * m_sampleRate)) {
            m_settings->m_gyroBias.setX((1.0 - m_gyroLearningAlpha) * m_settings->m_gyroBias.x() + m_gyroLearningAlpha * m_imuData.gyro.x());
            m_settings->m_gyroBias.setY((1.0 - m_gyroLearningAlpha) * m_settings->m_gyroBias.y() + m_gyroLearningAlpha * m_imuData.gyro.y());
            m_settings->m_gyroBias.setZ((1.0 - m_gyroLearningAlpha) * m_settings->m_gyroBias.z() + m_gyroLearningAlpha * m_imuData.gyro.z());

            m_gyroSampleCount++;

            if (m_gyroSampleCount == (5 * m_sampleRate)) {
                // this could have been true already of course
                m_settings->m_gyroBiasValid = true;
                m_settings->saveSettings();
            }
        } else {
            m_settings->m_gyroBias.setX((1.0 - m_gyroContinuousAlpha) * m_settings->m_gyroBias.x() + m_gyroContinuousAlpha * m_imuData.gyro.x());
            m_settings->m_gyroBias.setY((1.0 - m_gyroContinuousAlpha) * m_settings->m_gyroBias.y() + m_gyroContinuousAlpha * m_imuData.gyro.y());
            m_settings->m_gyroBias.setZ((1.0 - m_gyroContinuousAlpha) * m_settings->m_gyroBias.z() + m_gyroContinuousAlpha * m_imuData.gyro.z());
        }
    }

    m_imuData.gyro -= m_settings->m_gyroBias;
}

void RTIMU::calibrateAverageCompass()
{
    //  calibrate if required

    if (getCompassCalibrationValid()) {
        m_imuData.compass.setX((m_imuData.compass.x() - m_compassCalOffset[0]) * m_compassCalScale[0]);
        m_imuData.compass.setY((m_imuData.compass.y() - m_compassCalOffset[1]) * m_compassCalScale[1]);
        m_imuData.compass.setZ((m_imuData.compass.z() - m_compassCalOffset[2]) * m_compassCalScale[2]);

        if (m_settings->m_compassCalEllipsoidValid) {
            RTVector3 ev = m_imuData.compass;
            ev -= m_settings->m_compassCalEllipsoidOffset;

            m_imuData.compass.setX(ev.x() * m_settings->m_compassCalEllipsoidCorr[0][0] +
                ev.y() * m_settings->m_compassCalEllipsoidCorr[0][1] +
                ev.z() * m_settings->m_compassCalEllipsoidCorr[0][2]);

            m_imuData.compass.setY(ev.x() * m_settings->m_compassCalEllipsoidCorr[1][0] +
                ev.y() * m_settings->m_compassCalEllipsoidCorr[1][1] +
                ev.z() * m_settings->m_compassCalEllipsoidCorr[1][2]);

            m_imuData.compass.setZ(ev.x() * m_settings->m_compassCalEllipsoidCorr[2][0] +
                ev.y() * m_settings->m_compassCalEllipsoidCorr[2][1] +
                ev.z() * m_settings->m_compassCalEllipsoidCorr[2][2]);
        }
    }

    //  update running average

    m_compassAverage.setX(m_imuData.compass.x() * COMPASS_ALPHA + m_compassAverage.x() * (1.0 - COMPASS_ALPHA));
    m_compassAverage.setY(m_imuData.compass.y() * COMPASS_ALPHA + m_compassAverage.y() * (1.0 - COMPASS_ALPHA));
    m_compassAverage.setZ(m_imuData.compass.z() * COMPASS_ALPHA + m_compassAverage.z() * (1.0 - COMPASS_ALPHA));

    m_imuData.compass = m_compassAverage;
}

void RTIMU::calibrateAccel()
{
    if (!getAccelCalibrationValid())
        return;

    if (m_imuData.accel.x() >= 0)
        m_imuData.accel.setX(m_imuData.accel.x() / m_settings->m_accelCalMax.x());
    else
        m_imuData.accel.setX(m_imuData.accel.x() / -m_settings->m_accelCalMin.x());

    if (m_imuData.accel.y() >= 0)
        m_imuData.accel.setY(m_imuData.accel.y() / m_settings->m_accelCalMax.y());
    else
        m_imuData.accel.setY(m_imuData.accel.y() / -m_settings->m_accelCalMin.y());

    if (m_imuData.accel.z() >= 0)
        m_imuData.accel.setZ(m_imuData.accel.z() / m_settings->m_accelCalMax.z());
    else
        m_imuData.accel.setZ(m_imuData.accel.z() / -m_settings->m_accelCalMin.z());
}

void RTIMU::updateFusion()
{
    m_fusion->newIMUData(m_imuData, m_settings);
}

bool RTIMU::IMUGyroBiasValid()
{
    return m_settings->m_gyroBiasValid;
}

 void RTIMU::setExtIMUData(RTFLOAT gx, RTFLOAT gy, RTFLOAT gz, RTFLOAT ax, RTFLOAT ay, RTFLOAT az,
        RTFLOAT mx, RTFLOAT my, RTFLOAT mz, uint64_t timestamp)
 {
     m_imuData.gyro.setX(gx);
     m_imuData.gyro.setY(gy);
     m_imuData.gyro.setZ(gz);
     m_imuData.accel.setX(ax);
     m_imuData.accel.setY(ay);
     m_imuData.accel.setZ(az);
     m_imuData.compass.setX(mx);
     m_imuData.compass.setY(my);
     m_imuData.compass.setZ(mz);
     m_imuData.timestamp = timestamp;
     updateFusion();
}
