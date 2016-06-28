#ifndef __INIT_H
#define __INIT_H


#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "HMC83x.h"
#include "stm32f1xx_hal_tim.h"


void InitAll(void);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI1_Init(void);
void MX_SPI2_Init(void);
void MX_TIM3_Init(void);
void USB_Reconnect(void);

#endif