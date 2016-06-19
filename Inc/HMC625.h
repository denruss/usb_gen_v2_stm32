#include "stm32f1xx_hal.h"
#include "Init.h"

void SetAtt(uint8_t);
uint8_t ReadAtt(void);

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;