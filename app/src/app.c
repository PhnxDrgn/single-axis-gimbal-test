#include "main.h"
#include "app.h"
#include "mpu6050.h"
#include "stdio.h"
#include "string.h"
#include "servo.h"
#include "helpers.h"

#define ERROR_SUM_MAX 100.0
#define ERROR_SUM_MIN -100.0

MPU6050_data_t mpuData;
volatile bool mpuDataUpdated = false;
float servoAngle = 0;
volatile float pitchError = 0;

float P = 0.15, I = 0.004, D = 1.0;
volatile float error = 0.0;
volatile float errorPrev = 0.0;
volatile float errorSum = 0.0;
float servoAngleDelta = 0.0;

void APP_main()
{
    // enable servo
    serialPrint("enabling servo\n\r");
    enableServo();
    setServo(servoAngle);

    // init accelerometer/gyro
    if (MPU6050_init(&mpuData) == MPU6050_ok)
        serialPrint("MPU initialized successfully.\n\r");
    else
    {
        serialPrint("MPU failed to initialize.\n\r");
    }

    // wait until mpu is calibrated
    serialPrint("Waiting for MPU calibration... ");
    while (!mpuData.calibration.calibrated)
        ;
    serialPrint("complete.\n\r");

    while (1)
    {
        if (mpuDataUpdated)
        {
            char buffer[64];
            memset(buffer, '\0', sizeof(buffer));

            // snprintf(buffer, sizeof(buffer), "pitch: %7.02f\troll: %7.02f\tyaw: %7.02f\n\r", mpuData.pitch, mpuData.roll, mpuData.yaw);
            snprintf(buffer, sizeof(buffer), "pitch: %7.02f\tservoAngleDelta: %7.02f\terrSum: %7.02f\n\r", mpuData.pitch, servoAngleDelta, errorSum);

            serialPrint(buffer);
        }

        // angle to be applied to current position
        servoAngleDelta = (P * error) + (I * errorSum) + (D * (error - errorPrev));

        // limiting the change to be within bounds of possible angles
        float servoAngleDeltaMax = SERVO_ANGLE_MAX - servoAngle;
        float servoAngleDeltaMin = SERVO_ANGLE_MIN - servoAngle;
        servoAngleDelta = min(servoAngleDelta, servoAngleDeltaMax);
        servoAngleDelta = max(servoAngleDelta, servoAngleDeltaMin);

        servoAngle += servoAngleDelta;

        setServo(servoAngle);
    }
}

void APP_mpuIntReqHandler()
{
    MPU6050_status_t status = MPU6050_getData(&mpuData);
    mpuDataUpdated = status == MPU6050_ok;
    setLed(mpuDataUpdated);

    if (mpuDataUpdated)
    {
        errorPrev = error;
        error = mpuData.pitch;
        errorSum += error;

        // limit error sum
        errorSum = min(errorSum, ERROR_SUM_MAX);
        errorSum = max(errorSum, ERROR_SUM_MIN);
    }
}