#include "main.h"
#include "app.h"
#include "mpu6050.h"

MPU6050_data_t mpuData;
volatile bool mpuDataUpdated = false;

void APP_main()
{
    MPU6050_init(&mpuData);
    while (1)
    {


        }
    }
}

void APP_mpuIntReqHandler()
{
    MPU6050_status_t status = MPU6050_getData(&mpuData);
    mpuDataUpdated = status == MPU6050_ok;
    setLed(mpuDataUpdated);
}