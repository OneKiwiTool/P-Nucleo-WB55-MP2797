#ifndef __HAL_I2C_H
#define __HAL_I2C_H

#include <stdint.h>

void i2c1_init(void);
void i2c_detect(void);
void i2c_write16(uint8_t device_address, uint8_t register_address, uint16_t data);
void i2c_read16(uint8_t device_address, uint8_t register_address, uint16_t *data);

#endif /* __HAL_I2C_H */
