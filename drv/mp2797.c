#include "mp2797.h"
#include "hal_i2c.h"
#include <stdio.h>

void mp2797_info_reg(void)
{
    uint16_t temp = 0x00;
    uint8_t reg = 0x4d;
    i2c_read16(MP2797_ADDR, reg, &temp);
    printf("read1 0x%02x: 0x%04x\n", reg, temp);

    i2c_write16(MP2797_ADDR, reg, 0x02);

    i2c_read16(MP2797_ADDR, reg, &temp);
    printf("read2 0x%02x: 0x%04x\n", reg, temp);

    reg = 0x00;
    i2c_read16(MP2797_ADDR, reg, &temp);
    printf("read1 0x%02x: 0x%04x\n", reg, temp);

    i2c_write16(MP2797_ADDR, reg, 0x0f);

    i2c_read16(MP2797_ADDR, reg, &temp);
    printf("read2 0x%02x: 0x%04x\n", reg, temp);
}
