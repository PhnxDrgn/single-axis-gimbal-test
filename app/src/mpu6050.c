#include "mpu6050.h"
#include "i2c.h"
#include "string.h"
#include "math.h"

MPU6050_status_t MPU6050_init(MPU6050_data_t *data)
{
    I2C_status_t i2cStatus;
    uint8_t pwrMgmt1 = 0;
    uint8_t gyroConfig = MPU6050_st_gyro_x | MPU6050_st_gyro_y | MPU6050_st_gyro_z;
    uint8_t accelConfig = MPU6050_st_accel_x | MPU6050_st_accel_y | MPU6050_st_accel_z;
    uint8_t intConfigSetting = MPU6050_int_cfg_latch_int_en;
    uint8_t intEnableSetting = MPU6050_int_data_rdy_en;
    uint8_t sampleRateDivider = 8;
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
    data->pitch = 0.0;
    data->roll = 0.0;
    data->yaw = 0.0;
    data->lastMillis = 0;

    // reset int status
    uint8_t intStatus;
    MPU6050_getIntStatus(&intStatus);

    // reset calibration
    MPU6050_init_cal(&data->calibration);

    return MPU6050_ok;
}

void MPU6050_init_cal(MPU6050_cal_t *cal)
{
    cal->accelOffset.x = 0;
    cal->accelOffset.y = 0;
    cal->accelOffset.z = 0;
    cal->gyroOffset.x = 0;
    cal->gyroOffset.y = 0;
    cal->gyroOffset.z = 0;
    cal->dataIndex = 0;
    cal->calibrated = false;
}

void MPU6050_updateCalibration(MPU6050_cal_t *cal, MPU6050_axis_t accelData, MPU6050_axis_t gyroData)
{
    cal->accelOffset.x += ((atan((accelData.y) / sqrt(pow((accelData.x), 2) + pow((accelData.z), 2))) * 180 / M_PI));
    cal->accelOffset.y += ((atan(-1 * (accelData.x) / sqrt(pow((accelData.y), 2) + pow((accelData.z), 2))) * 180 / M_PI));
    cal->gyroOffset.x += gyroData.x;
    cal->gyroOffset.y += gyroData.y;
    cal->gyroOffset.z += gyroData.z;
    cal->dataIndex++;

    if (cal->dataIndex >= MPU6050_CAL_PTS)
    {
        cal->accelOffset.x /= cal->dataIndex;
        cal->accelOffset.y /= cal->dataIndex;
        cal->gyroOffset.x /= cal->dataIndex;
        cal->gyroOffset.y /= cal->dataIndex;
        cal->gyroOffset.z /= cal->dataIndex;
        cal->calibrated = true;
    }
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
    MPU6050_axis_t accelData = {0.0, 0.0, 0.0};
    MPU6050_axis_t gyroData = {0.0, 0.0, 0.0};

    if (I2C_rx(MPU6050_ADDR, MPU6050_reg_accel_xout_h, dataBurst, 6) != I2C_OK) // read 6 bytes for all 3 axis
        return MPU6050_err;
    accelData.x = ((int16_t)(dataBurst[0] << 8 | dataBurst[1])) / 16384.0;
    accelData.y = ((int16_t)(dataBurst[2] << 8 | dataBurst[3])) / 16384.0;
    accelData.z = ((int16_t)(dataBurst[4] << 8 | dataBurst[5])) / 16384.0;

    if (I2C_rx(MPU6050_ADDR, MPU6050_reg_gyro_xout_h, dataBurst, 6) != I2C_OK) // read 6 bytes for all 3 axis
        return MPU6050_err;
    gyroData.x = ((int16_t)(dataBurst[0] << 8 | dataBurst[1])) / 131.0; // in deg / sec
    gyroData.y = ((int16_t)(dataBurst[2] << 8 | dataBurst[3])) / 131.0; // in deg / sec
    gyroData.z = ((int16_t)(dataBurst[4] << 8 | dataBurst[5])) / 131.0; // in deg / sec

    if (MPU6050_getIntStatus(&intStatus) != MPU6050_ok)
        return MPU6050_err;

    if (intStatus & MPU6050_int_data_rdy_en) // if new data was present
    {
        if (!data->calibration.calibrated) // intercept data for calibration
        {
            MPU6050_updateCalibration(&data->calibration, accelData, gyroData);
            return MPU6050_ok;
        }

        // correct gyro data with calibrated values
        gyroData.x -= data->calibration.gyroOffset.x;
        gyroData.y -= data->calibration.gyroOffset.y;
        gyroData.z -= data->calibration.gyroOffset.z;

        // get angle by multiplying by time delta
        float timeDelta = (refMillis - data->lastMillis) / 1000.0; // sec
        gyroData.x *= timeDelta;
        gyroData.y *= timeDelta;
        gyroData.z *= timeDelta;

        // update pitch roll yaw
        data->pitch += gyroData.x;
        data->roll += gyroData.y;
        data->yaw += gyroData.z;

        // update ref time
        data->lastMillis = refMillis;
    }

    return MPU6050_ok;
}