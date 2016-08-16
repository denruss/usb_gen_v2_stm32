#ifndef __HMC83x_H
#define __HMC83x_H

#include "stm32f1xx_hal.h"
#include "Init.h"
#include "main.h"

void HMC83x_write(uint32_t, uint32_t); 
uint32_t HMC83x_read(uint8_t);

uint32_t HMC83x_wordHM(int, int); 

void SetFreq(float);
void En(uint8_t);
void InitHMC83x(void);


extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern float freq;

#endif