#include "main.h"
#include "app.h"
#include "mpu6050.h"
#include "stdio.h"
#include "string.h"

MPU6050_data_t mpuData;
volatile bool mpuDataUpdated = false;

void APP_main()
{
    MPU6050_init(&mpuData);
    while (1)
    {
        if (mpuDataUpdated)
        {
            char buffer[256];
            memset(buffer, '\0', sizeof(buffer));

            snprintf(buffer, sizeof(buffer), "gyroX: %7.02f\tgyroY: %7.02f\tgyroZ: %7.02f\n\r", mpuData.gyro.x, mpuData.gyro.y, mpuData.gyro.z);

            serialPrint(buffer, strnlen(buffer, sizeof(buffer)));
        }
    }
}

void APP_mpuIntReqHandler()
{
    MPU6050_status_t status = MPU6050_getData(&mpuData);
    mpuDataUpdated = status == MPU6050_ok;
    setLed(mpuDataUpdated);
}