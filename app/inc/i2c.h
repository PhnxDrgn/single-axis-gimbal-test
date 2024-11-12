#ifndef I2C
#define I2C

#ifdef __cplusplus
extern "C"
{
#endif

#define I2C_BUFFER_SIZE 256
#define I2C_REQ_TIMEOUT 1

#include "stdint.h"
#include "main.h"

    typedef enum
    {
        I2C_OK = HAL_OK,
        I2C_ERROR = HAL_ERROR,
        I2C_BUSY = HAL_BUSY,
        I2C_TIMEOUT = HAL_TIMEOUT,
    } I2C_status_t;

    I2C_status_t I2C_rx(uint16_t devAddr, uint8_t reg, uint8_t *dataBuffer, uint16_t dataSize);
    I2C_status_t I2C_tx(uint16_t devAddr, uint8_t reg, uint8_t *dataBuffer, uint16_t dataSize);

#ifdef __cplusplus
}
#endif

#endif /* I2C */