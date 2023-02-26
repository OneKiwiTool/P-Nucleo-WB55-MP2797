#include <stdio.h>
#include "hal_i2c.h"
#include "main.h"

I2C_HandleTypeDef hi2c1;

void i2c1_init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x10707DBC;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

void i2c_detect(void)
{
    uint8_t devices = 0u;

    printf("Searching for I2C devices on the bus...\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:         ");

    /* Values outside 0x03 and 0x77 are invalid. */
    for (uint8_t i = 0x00u; i < 0x78u; i++)
    {
        uint8_t address = i << 1u ;
        if( i%16 == 0){
            printf("\n%02x:", i);
        }
        /* In case there is a positive feedback, print it out. */
        if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, address, 3u, 10u))
        {
            printf(" %02x", i);
            devices++;
        } else {
            printf(" --");
        }
    }
    /* Feedback of the total number of devices. */
    if (0u == devices)
    {
        printf("\nNo device found.\n");
    }
    else
    {
        printf("\nTotal found devices: %d\n", devices);
    }
}

void i2c_write16(uint8_t device_address, uint8_t register_address, uint16_t data) {
    uint8_t address = device_address << 1;
    uint8_t buffer[3] = {register_address, (data) & 0xFF, (data >> 8) & 0xFF};
    HAL_I2C_Master_Transmit(&hi2c1, address, buffer, 3, 1000);
}

void i2c_read16(uint8_t device_address, uint8_t register_address, uint16_t *data) {
    uint8_t address = device_address << 1;
    HAL_I2C_Master_Transmit(&hi2c1, address, &register_address, 1, 1000);

    uint8_t buffer[2] = {0x00, 0x00};
    HAL_I2C_Master_Receive(&hi2c1, address, buffer, 2, 1000);
    printf("b0: 0x%02x, b1: 0x%02x\n", buffer[0], buffer[1]);
    *data = (buffer[1] << 8) | (buffer[0]);
}

// https://github.com/zenitheesc/Drivers/blob/main/INA3221/INA3221.c
// https://github.com/zenitheesc/Drivers/blob/ec21364a7e1e5a40ef59ba48b198f86fcd332537/platform/arch/stm32x/platform_stm32.h
