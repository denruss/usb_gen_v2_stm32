#include "HMC625.h"



void SetAtt(uint8_t data)
{
    uint8_t BufferTX[1];
    BufferTX[0] = data;
  
    HAL_SPI_Transmit(&hspi2, (uint8_t*)BufferTX, 1, 10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

}

uint8_t ReadAtt(void)
{
    uint8_t BufferRX[1];
    HAL_SPI_Receive(&hspi2, (uint8_t*)BufferRX, 1, 10);
    return BufferRX[0] >> 2;
}