#include "mpu6050.h"
#include "i2c.h"
#include "string.h"

MPU6050_status_t MPU6050_init(MPU6050_data_t *data)
{
    I2C_status_t i2cStatus;
    uint8_t pwrMgmt1 = 0;
    uint8_t gyroConfig = MPU6050_st_gyro_x | MPU6050_st_gyro_y | MPU6050_st_gyro_z;
    uint8_t accelConfig = MPU6050_st_accel_x | MPU6050_st_accel_y | MPU6050_st_accel_z;
    uint8_t intConfigSetting = MPU6050_int_cfg_latch_int_en;
    uint8_t intEnableSetting = MPU6050_int_data_rdy_en;
    uint8_t sampleRateDivider = 255;
    uint8_t regData = 0;

    // sample rate divider setting
    i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_smprt_div, &sampleRateDivider, sizeof(sampleRateDivider));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;

    // setting gyro config and self tests
    i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_gyro_config, &gyroConfig, sizeof(gyroConfig));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;

    // setting accel config and self tests
    i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_accel_config, &accelConfig, sizeof(accelConfig));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;

    // set power management 1 to 0
    i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_pwr_mgmt_1, &pwrMgmt1, sizeof(pwrMgmt1));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;

    // int config setting
    i2cStatus = I2C_rx(MPU6050_ADDR, MPU6050_reg_int_pin_cfg, &regData, sizeof(regData));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;
    if (regData != intConfigSetting)
    {
        i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_int_pin_cfg, &intConfigSetting, sizeof(intConfigSetting));
        if (i2cStatus != I2C_OK)
            return MPU6050_err;
    }

    // int enable setting
    i2cStatus = I2C_rx(MPU6050_ADDR, MPU6050_reg_int_enable, &regData, sizeof(regData));
    if (i2cStatus != I2C_OK)
        return MPU6050_err;
    if (regData != intEnableSetting)
    {
        i2cStatus = I2C_tx(MPU6050_ADDR, MPU6050_reg_int_enable, &intEnableSetting, sizeof(intEnableSetting));
        if (i2cStatus != I2C_OK)
            return MPU6050_err;
    }

    // reset data values
    data->accel.x = 0.0;
    data->accel.y = 0.0;
    data->accel.z = 0.0;
    data->gyro.x = 0.0;
    data->gyro.y = 0.0;
    data->gyro.z = 0.0;
    data->lastMillis = 0;

    // reset int status
    uint8_t intStatus;
    MPU6050_getIntStatus(&intStatus);

    return MPU6050_ok;
}

MPU6050_status_t MPU6050_getIntStatus(uint8_t *status)
{

    I2C_status_t i2cStatus = I2C_rx(MPU6050_ADDR, MPU6050_reg_int_status, status, 1);

    if (i2cStatus != I2C_OK)
        return MPU6050_err;

    return MPU6050_ok;
}

MPU6050_status_t MPU6050_getData(MPU6050_data_t *data)
{
    uint32_t refMillis = millis();
    uint8_t intStatus = 0;
    uint8_t dataBurst[6] = {0, 0, 0, 0, 0, 0}; // 2 bytes per axis
    MPU6050_accel_t accelData = {0.0, 0.0, 0.0};
    MPU6050_gyro_t gyroData = {0.0, 0.0, 0.0};

    if (I2C_rx(MPU6050_ADDR, MPU6050_reg_accel_xout_h, dataBurst, 6) != I2C_OK) // read 6 bytes for all 3 axis
        return MPU6050_err;
    accelData.x = (float)(dataBurst[0] << 8 | dataBurst[1]) / 16384.0;
    accelData.y = (float)(dataBurst[2] << 8 | dataBurst[3]) / 16384.0;
    accelData.z = (float)(dataBurst[4] << 8 | dataBurst[5]) / 16384.0;

    if (I2C_rx(MPU6050_ADDR, MPU6050_reg_gyro_xout_h, dataBurst, 6) != I2C_OK) // read 6 bytes for all 3 axis
        return MPU6050_err;
    gyroData.x = (float)(dataBurst[0] << 8 | dataBurst[1]) / 131.0; // in deg / sec
    gyroData.y = (float)(dataBurst[2] << 8 | dataBurst[3]) / 131.0; // in deg / sec
    gyroData.z = (float)(dataBurst[4] << 8 | dataBurst[5]) / 131.0; // in deg / sec
    
    // convert to deg
    float timeDelta = (refMillis - data->lastMillis) / 1000.0; // time delta millis to seconds
    gyroData.x *= timeDelta;
    gyroData.y *= timeDelta;
    gyroData.z *= timeDelta;

    if (MPU6050_getIntStatus(&intStatus) != MPU6050_ok)
        return MPU6050_err;

    if (intStatus & MPU6050_int_data_rdy_en)
    {
        data->accel = accelData;
        data->gyro = gyroData;
        data->lastMillis = refMillis;
    }

    return MPU6050_ok;
}