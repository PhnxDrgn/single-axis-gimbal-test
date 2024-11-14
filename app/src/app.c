#include "main.h"
#include "app.h"
#include "mpu6050.h"
#include "stdio.h"
#include "string.h"

MPU6050_data_t mpuData;
volatile bool mpuDataUpdated = false;

void APP_main()
{
    // init accelerometer/gyro
    if (MPU6050_init(&mpuData) == MPU6050_ok)
        serialPrint("MPU initialized successfully.\n");
    else
    {
        serialPrint("MPU failed to initialize.\n");
    }

    // wait until mpu is calibrated
    serialPrint("Waiting for MPU calibration... ");
    while (!mpuData.calibration.calibrated)
        ;
    serialPrint("complete.\n");

    while (1)
    {
        if (mpuDataUpdated)
        {
            char buffer[64];
            memset(buffer, '\0', sizeof(buffer));

            // snprintf(buffer, sizeof(buffer), "pitch: %7.02f\troll: %7.02f\tyaw: %7.02f\n\r", mpuData.pitch, mpuData.roll, mpuData.yaw);
            snprintf(buffer, sizeof(buffer), "pitch: %7.02f\n\r", mpuData.pitch);

            serialPrint(buffer);
        }
    }
}

void APP_mpuIntReqHandler()
{
    MPU6050_status_t status = MPU6050_getData(&mpuData);
    mpuDataUpdated = status == MPU6050_ok;
    setLed(mpuDataUpdated);
}