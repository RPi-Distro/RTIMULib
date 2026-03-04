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


#include "RTIMULib.h"
#include "RTIMUMagCal.h"
#include "RTIMUAccelCal.h"

#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

//  where to find the ellipsoid fitting code

#define ELLIPSOID_FIT_DIR               RTIMUCALDEFS_OCTAVE_PATH

//  function prototypes

void doMagMinMaxCal();
void doMagEllipsoidCal();
void processEllipsoid();
void doAccelCal();
void newIMU();
bool pollIMU();
char getUserChar();
void displayMenu();
void displayMagMinMax();
void displayMagEllipsoid();
void displayAccelMinMax();

//  global variables

static RTIMUSettings *settings;
static RTIMU_DATA imuData;
static RTIMU *imu;
static RTIMUMagCal *magCal;
static RTIMUAccelCal *accelCal;
static bool magMinMaxDone;
static bool accelEnables[3];
static int accelCurrentAxis;

int main(int argc, char **argv)
{
    char *settingsFile;

    if (argc == 2)
        settingsFile = argv[1];
    else
        settingsFile = (char *)"RTIMULib";

    printf("RTIMULibCal - using %s.ini\n", settingsFile);

    settings = new RTIMUSettings(settingsFile);

    bool mustExit = false;
    imu = NULL;
    newIMU();

    //  set up for calibration run

    imu->setCompassCalibrationMode(true);
    imu->setAccelCalibrationMode(true);
    magCal = new RTIMUMagCal(settings);
    magCal->magCalInit();
    magMinMaxDone = false;
    accelCal = new RTIMUAccelCal(settings);
    accelCal->accelCalInit();

    //  set up console io

    struct termios	ctty;

    tcgetattr(fileno(stdout), &ctty);
    ctty.c_lflag &= ~(ICANON);
    tcsetattr(fileno(stdout), TCSANOW, &ctty);

    //  the main loop

    while (!mustExit) {
        displayMenu();
        switch (tolower(getchar())) {
        case 'x' :
            mustExit = true;
            break;

        case 'm' :
            doMagMinMaxCal();
            break;

        case 'e' :
            doMagEllipsoidCal();
            break;

        case 'a' :
            doAccelCal();
            break;
        }
    }

    printf("\nRTIMULibCal exiting\n");
    return 0;
}

void newIMU()
{
    if (imu != NULL)
        delete imu;

    imu = RTIMU::createIMU(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    //  set up IMU

    imu->IMUInit();
}

void doMagMinMaxCal()
{
    uint64_t displayTimer;
    uint64_t now;
    char input;

    magCal->magCalInit();
    magMinMaxDone = false;

    //  now collect data

    printf("\n\nMagnetometer min/max calibration\n");
    printf("--------------------------------\n");
    printf("Waggle the IMU chip around, ensuring that all six axes\n");
    printf("(+x, -x, +y, -y and +z, -z) go through their extrema.\n");
    printf("When all extrema have been achieved, enter 's' to save, 'r' to reset\n");
    printf("or 'x' to abort and discard the data.\n");
    printf("\nPress any key to start...");
    getchar();

    displayTimer = RTMath::currentUSecsSinceEpoch();

    while (1) {
        //  poll at the rate recommended by the IMU

        usleep(imu->IMUGetPollInterval() * 1000);

        while (pollIMU()) {
            magCal->newMinMaxData(imuData.compass);
            now = RTMath::currentUSecsSinceEpoch();

            //  display 10 times per second

            if ((now - displayTimer) > 100000) {
                displayMagMinMax();
                displayTimer = now;
            }
        }

        if ((input = getUserChar()) != 0) {
            switch (input) {
            case 's' :
                printf("\nSaving min/max data.\n\n");
                magCal->magCalSaveMinMax();
                magMinMaxDone = true;
                return;

            case 'x' :
                printf("\nAborting.\n");
                return;

            case 'r' :
                printf("\nResetting min/max data.\n");
                magCal->magCalReset();
                break;
            }
        }
    }
}


void doMagEllipsoidCal()
{
    uint64_t displayTimer;
    uint64_t now;
    char input;

    if (!magMinMaxDone) {
        printf("\nYou cannot collect ellipsoid data until magnetometer min/max\n");
        printf("calibration has been performed.\n");
        return;
    }

    printf("\n\nMagnetometer ellipsoid calibration\n");
    printf("\n\n----------------------------------\n");
    printf("Move the magnetometer around in as many poses as possible.\n");
    printf("The counts for each of the 8 pose quadrants will be displayed.\n");
    printf("When enough data (%d samples per octant) has been collected,\n", RTIMUCALDEFS_OCTANT_MIN_SAMPLES);
    printf("ellipsoid processing will begin.\n");
    printf("Enter 'x' at any time to abort and discard the data.\n");
    printf("\nPress any key to start...");
    getchar();

    displayTimer = RTMath::currentUSecsSinceEpoch();

    while (1) {
        //  poll at the rate recommended by the IMU

        usleep(imu->IMUGetPollInterval() * 1000);

        while (pollIMU()) {
            magCal->newEllipsoidData(imuData.compass);

            if (magCal->magCalEllipsoidValid()) {
                magCal->magCalSaveRaw(".");
                processEllipsoid();
                return;
            }
            now = RTMath::currentUSecsSinceEpoch();

            //  display 10 times per second

            if ((now - displayTimer) > 100000) {
                displayMagEllipsoid();
                displayTimer = now;
            }
        }

        if ((input = getUserChar()) != 0) {
            switch (input) {
            case 'x' :
                printf("\nAborting.\n");
                return;
            }
        }
    }
}

void processEllipsoid()
{
    pid_t pid;
    int status;

    printf("\n\nProcessing ellipsoid fit data...\n");

    pid = fork();
    if (pid == 0) {
        //  child process
        execl("/bin/sh", "/bin/sh", "-c", RTIMUCALDEFS_OCTAVE_COMMAND, NULL);
        printf("here");
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        printf("\nFailed to start ellipsoid fitting code.\n");
        return;
    } else {
        //  parent process - wait for child
        if (waitpid(pid, &status, 0) != pid) {
            printf("\n\nEllipsoid fit failed, %d\n", status);
        } else {
            if (status == 0) {
                printf("\nEllipsoid fit completed - saving data to file.");
                magCal->magCalSaveCorr(".");
            } else {
                printf("\nEllipsoid fit returned %d - aborting.\n", status);
            }
        }
    }
}

void doAccelCal()
{
    uint64_t displayTimer;
    uint64_t now;
    char input;

    printf("\n\nAccelerometer Calibration\n");
    printf("-------------------------\n");
    printf("The code normally ignores readings until an axis has been enabled.\n");
    printf("The idea is to orient the IMU near the current extrema (+x, -x, +y, -y, +z, -z)\n");
    printf("and then enable the axis, moving the IMU very gently around to find the\n");
    printf("extreme value. Now disable the axis again so that the IMU can be inverted.\n");
    printf("When the IMU has been inverted, enable the axis again and find the extreme\n");
    printf("point. Disable the axis again and press the space bar to move to the next\n");
    printf("axis and repeat. The software will display the current axis and enable state.\n");
    printf("Available options are:\n");
    printf("  e - enable the current axis.\n");
    printf("  d - disable the current axis.\n");
    printf("  space bar - move to the next axis (x then y then z then x etc.\n");
    printf("  r - reset the current axis (if enabled).\n");
    printf("  s - save the data once all 6 extrema have been collected.\n");
    printf("  x - abort and discard the data.\n");
    printf("\nPress any key to start...");
    getchar();

    //  perform all axis reset
    for (int i = 0; i < 3; i++)
        accelCal->accelCalEnable(i, true);
    accelCal->accelCalReset();
    for (int i = 0; i < 3; i++)
        accelCal->accelCalEnable(i, false);

    accelCurrentAxis = 0;

    for (int i = 0; i < 3; i++)
        accelEnables[i] = false;

    displayTimer = RTMath::currentUSecsSinceEpoch();

    while (1) {
        //  poll at the rate recommended by the IMU

        usleep(imu->IMUGetPollInterval() * 1000);

        while (pollIMU()) {

            for (int i = 0; i < 3; i++)
                accelCal->accelCalEnable(i, accelEnables[i]);
            accelCal->newAccelCalData(imuData.accel);

            now = RTMath::currentUSecsSinceEpoch();

            //  display 10 times per second

            if ((now - displayTimer) > 100000) {
                displayAccelMinMax();
                displayTimer = now;
            }
        }

        if ((input = getUserChar()) != 0) {
            switch (input) {
            case 'e' :
                accelEnables[accelCurrentAxis] = true;
                break;

            case 'd' :
                accelEnables[accelCurrentAxis] = false;
                break;

            case 'r' :
                accelCal->accelCalReset();
                break;

            case ' ' :
                if (++accelCurrentAxis == 3)
                    accelCurrentAxis = 0;
                break;

            case 's' :
                accelCal->accelCalSave();
                printf("\nAccelerometer calibration data saved to file.\n");
                return;

            case 'x' :
                printf("\nAborting.\n");
                return;
            }
        }
    }
}


bool pollIMU()
{
    if (imu->IMURead()) {
        imuData = imu->getIMUData();
        return true;
    } else {
        return false;
    }
}

char getUserChar()
{
    int i;

    ioctl(0, FIONREAD, &i);
    if (i <= 0)
        return 0;
    return tolower(getchar());
}

void displayMenu()
{
    printf("\n");
    printf("Options are: \n\n");
    printf("  m - calibrate magnetometer with min/max\n");
    printf("  e - calibrate magnetometer with ellipsoid (do min/max first)\n");
    printf("  a - calibrate accelerometers\n");
    printf("  x - exit\n\n");
    printf("Enter option: ");
}

void displayMagMinMax()
{
    printf("\n\n");
    printf("Min x: %6.2f  min y: %6.2f  min z: %6.2f\n", magCal->m_magMin.data(0),
           magCal->m_magMin.data(1), magCal->m_magMin.data(2));
    printf("Max x: %6.2f  max y: %6.2f  max z: %6.2f\n", magCal->m_magMax.data(0),
           magCal->m_magMax.data(1), magCal->m_magMax.data(2));
    fflush(stdout);
}

void displayMagEllipsoid()
{
    int counts[RTIMUCALDEFS_OCTANT_COUNT];

    printf("\n\n");

    magCal->magCalOctantCounts(counts);

    printf("---: %d  +--: %d  -+-: %d  ++-: %d\n", counts[0], counts[1], counts[2], counts[3]);
    printf("--+: %d  +-+: %d  -++: %d  +++: %d\n", counts[4], counts[5], counts[6], counts[7]);
    fflush(stdout);
}

void displayAccelMinMax()
{
    printf("\n\n");

    printf("Current axis: ");
    if (accelCurrentAxis == 0) {
        printf("x - %s", accelEnables[0] ? "enabled" : "disabled");
    } else     if (accelCurrentAxis == 1) {
        printf("y - %s", accelEnables[1] ? "enabled" : "disabled");
    } else     if (accelCurrentAxis == 2) {
        printf("z - %s", accelEnables[2] ? "enabled" : "disabled");
    }

    printf("\nMin x: %6.2f  min y: %6.2f  min z: %6.2f\n", accelCal->m_accelMin.data(0),
           accelCal->m_accelMin.data(1), accelCal->m_accelMin.data(2));
    printf("Max x: %6.2f  max y: %6.2f  max z: %6.2f\n", accelCal->m_accelMax.data(0),
           accelCal->m_accelMax.data(1), accelCal->m_accelMax.data(2));
    fflush(stdout);
}
