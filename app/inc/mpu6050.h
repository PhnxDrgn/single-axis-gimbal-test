#ifndef MPU6050
#define MPU6050

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

#define MPU6050_ADDR 0b1101000

    typedef enum
    {
        MPU6050_st_gyro_x = 1 << 7,
        MPU6050_st_gyro_y = 1 << 6,
        MPU6050_st_gyro_z = 1 << 5,
    } MPU6050_st_gyro_t;

    typedef enum
    {
        MPU6050_st_accel_x = 1 << 7,
        MPU6050_st_accel_y = 1 << 6,
        MPU6050_st_accel_z = 1 << 5,
    } MPU6050_st_accel_t;

    typedef enum
    {
        MPU6050_int_cfg_int_level = 1 << 7,
        MPU6050_int_cfg_int_open = 1 << 6,
        MPU6050_int_cfg_latch_int_en = 1 << 5,
        MPU6050_int_cfg_int_rd_clear = 1 << 4,
        MPU6050_int_cfg_int_status = 1 << 3,
        MPU6050_int_cfg_fsync_int_level = 1 << 2,
        MPU6050_int_cfg_fsync_int_en = 1 << 1,
    } MPU6050_int_cfg_t;

    typedef enum
    {
        MPU6050_int_mot_en = 1 << 6,
        MPU6050_int_fifo_oflow_en = 1 << 4,
        MPU6050_int_i2c_mst_int_en = 1 << 3,
        MPU6050_int_data_rdy_en = 1 << 0,
    } MPU6050_int_en_t;

    typedef enum
    {
        MPU6050_reg_smprt_div = 25,    // 0x19
        MPU6050_reg_gyro_config = 27,  // 0x1B
        MPU6050_reg_accel_config = 28, // 0x1C
        MPU6050_reg_int_pin_cfg = 55,  // 0x37
        MPU6050_reg_int_enable = 56,   // 0x38
        MPU6050_reg_int_status = 58,   // 0x3A
        MPU6050_reg_pwr_mgmt_1 = 107,  // 0x6B
        MPU6050_reg_accel_xout_h = 59, // 0x3B
        MPU6050_reg_gyro_xout_h = 67,  // 0x43
    } MPU6050_reg_t;

    typedef enum
    {
        MPU6050_ok,
        MPU6050_err,
    } MPU6050_status_t;

    typedef struct
    {
        float x;
        float y;
        float z;
    } MPU6050_accel_t;

    typedef struct
    {
        float x;
        float y;
        float z;
    } MPU6050_gyro_t;

    typedef struct
    {
        MPU6050_accel_t accel;
        MPU6050_gyro_t gyro;
        uint32_t lastMillis;
    } MPU6050_data_t;

    MPU6050_status_t MPU6050_init(MPU6050_data_t *data);
    MPU6050_status_t MPU6050_getIntStatus(uint8_t *status);
    MPU6050_status_t MPU6050_getData(MPU6050_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050 */