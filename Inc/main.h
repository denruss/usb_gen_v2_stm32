#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "HMC83x.h"
#include "HMC625.h"
#include "Init.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

void Led(uint8_t);
void Error_Handler(void);
void WriteParam(void);
void ReadParam(void);


extern USBD_HandleTypeDef  hUsbDeviceFS;
extern uint8_t dataToReceive[CUSTOM_HID_EPOUT_SIZE];
extern uint8_t Receive;
extern uint8_t Timer;

/* Definition for SPIx's DMA */
#define SPIx_DMA_TX_IRQHandler           DMA1_Channel5_IRQHandler
#define SPIx_DMA_RX_IRQHandler           DMA1_Channel4_IRQHandler

#define SPIx_TX_DMA_CHANNEL              DMA1_Channel5
#define SPIx_RX_DMA_CHANNEL              DMA1_Channel4

#endif