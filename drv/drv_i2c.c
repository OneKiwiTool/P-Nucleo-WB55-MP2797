#if 0
#include "drv_i2c.h"
#include "stm32wbxx_ll_i2c.h"

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */

  /* Wait for TXE flag to be raised */
  while (!LL_USART_IsActiveFlag_TXE(USARTx_INSTANCE))
  {
  }

  /* Write character in Transmit Data register.
     TXE flag is cleared by writing data in TDR register */
  LL_USART_TransmitData8(USARTx_INSTANCE, ch);

  /* Wait for TC flag to be raised for last char */
  while (!LL_USART_IsActiveFlag_TC(USARTx_INSTANCE))
  {
  }
  
  return ch;
}

#define I2C_DRV             I2C1
#define I2C_REQUEST_WRITE   0x00
#define I2C_REQUEST_READ    0x01

void i2c1_init()
{
    LL_I2C_InitTypeDef I2C_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    /**I2C1 GPIO Configuration
    PB8   ------> I2C1_SCL
    PB9   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /** I2C Initialization */
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing = 0x10707DBC;
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
    LL_I2C_EnableAutoEndMode(I2C1);
    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    LL_I2C_EnableClockStretching(I2C1);
}

void i2c_write16b(uint8_t device_address, uint8_t register_address, uint16_t data){
    uint8_t address = register_address << 1;
    uint8_t data_high = (data >> 8) & 0xFF; /* data high byte */
    uint8_t data_low = (data) & 0xFF;      /* data low byte */

    // wait I2C bus is idle
    while(LL_I2C_IsActiveFlag_BUSY(I2C_DRV));

    //START
    LL_I2C_GenerateStartCondition(I2C_DRV);
    //while(!LL_I2C_IsActiveFlag_SB(I2C_DRV));

    LL_I2C_TransmitData8(I2C_DRV, (address| I2C_REQUEST_WRITE)); //ADDRESS + WRITE
    while(!LL_I2C_IsActiveFlag_ADDR(I2C_DRV));
    LL_I2C_ClearFlag_ADDR(I2C_DRV);

    LL_I2C_TransmitData8(I2C_DRV, register_address); //Transmit mode register address
    while(!LL_I2C_IsActiveFlag_TXE(I2C_DRV));

    LL_I2C_TransmitData8(I2C_DRV, data_low); //Transmit data low byte
    while(!LL_I2C_IsActiveFlag_TXE(I2C_DRV));

    LL_I2C_TransmitData8(I2C_DRV, data_high); //Transmit data high byte
    while(!LL_I2C_IsActiveFlag_TXE(I2C_DRV));

    //LL_I2C_AcknowledgeNextData(I2C_DRV, LL_I2C_NACK);
    // STOP
    LL_I2C_GenerateStopCondition(I2C_DRV);
}

void i2c_read16b(uint8_t device_address, uint8_t register_address, uint16_t* data){
    uint8_t data_high = 0x00; /* data high byte */
    uint8_t data_low = 0x00;      /* data low byte */

    // wait I2C bus is idle
    while(LL_I2C_IsActiveFlag_BUSY(I2C_DRV));

    //START
    LL_I2C_GenerateStartCondition(I2C_DRV);
    //while(!LL_I2C_IsActiveFlag_SB(I2C_DRV));

    LL_I2C_TransmitData8(I2C_DRV, (address| I2C_REQUEST_WRITE)); //ADDRESS + WRITE
    while(!LL_I2C_IsActiveFlag_ADDR(I2C_DRV));
    LL_I2C_ClearFlag_ADDR(I2C_DRV);

    LL_I2C_TransmitData8(I2C_DRV, register_address); //Transmit register address
    while(!LL_I2C_IsActiveFlag_TXE(I2C_DRV));

    //START
    LL_I2C_GenerateStartCondition(I2C_DRV);
    //while(!LL_I2C_IsActiveFlag_SB(I2C_DRV));
    LL_I2C_TransmitData8(I2C_DRV, (address| I2C_REQUEST_READ)); //ADDRESS + READ
    while(!LL_I2C_IsActiveFlag_ADDR(I2C_DRV));
    LL_I2C_ClearFlag_ADDR(I2C_DRV);

    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK); // ACK Incoming Data
	while (!LL_I2C_IsActiveFlag_RXNE (I2C1)); // sets when received data is copied in received data register, resets when data register is read
	data_low = LL_I2C_ReceiveData8(I2C1); // data byte

    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK); // ACK Incoming Data
	while (!LL_I2C_IsActiveFlag_RXNE (I2C1)); // sets when received data is copied in received data register, resets when data register is read
	data_high = LL_I2C_ReceiveData8(I2C1); // data byte

    // STOP
    LL_I2C_GenerateStopCondition(I2C_DRV);
    *data = (uint16_t)((data_high<<8)|data_low)
}
#endif
