
#include "main.h"

float freq = 2000.0; //действующие значения от 25 до 6000 МГц
uint32_t att = 0; //действующие значения от 0 до 63, 63 - максимальный сигнал, 0- минимальный, шаг 0,5 дБ
uint32_t enable = 0; //
uint8_t mode = 0; // режим работы, 0 - ГСС, 1 - ГКЧ, 2 - 

uint8_t dataToSend[64] = {0}; // Массив байт, для отправки ПК

int main(void)
{
   
  /* Configure the system clock and initialize all configured peripherals */
  InitAll();


  
  uint32_t data = 0;
  
  // проверка микросхем HMC
  SetAtt(0x2A);
  if (ReadAtt() == 0x2A) 
      dataToSend[3] = 2; 
   
  data = HMC83x_read(0) & 0x000FFFFF; //читаем ID 0xA7975
  if (data == 0xA7975)
      dataToSend[3] = dataToSend[3] | 1;


  // загружаем сохранённые настройки
  ReadParam();
  
  
  SetFreq(freq);
  SetAtt(att);
  HAL_Delay(100);
  En(enable);  Led(enable); 

  
  while (1)
  {
    
  }


}


void Led(uint8_t enable)
{
	if (enable == 0 ) // выключение
	{ 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	}

	else // включение
	{
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	}
}


void WriteParam(void)
{
  HAL_StatusTypeDef	flash_ok = HAL_ERROR;
  FLASH_EraseInitTypeDef pEraseInit;
  uint32_t PageError;
  
  
  
  uint32_t Address = 0x08010000; //
  pEraseInit.PageAddress = Address;
  pEraseInit.NbPages = 1;
    
  //Делаем память открытой
  while(flash_ok != HAL_OK){
	flash_ok = HAL_FLASH_Unlock();
  }
  
  // Стираем страницу
  HAL_FLASHEx_Erase(&pEraseInit, &PageError);
  
  // Сохраняем настройки
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, (uint64_t)freq);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+4, (uint64_t)att);  
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+8, (uint64_t)enable);   
    
  //Закрываем память
  flash_ok = HAL_ERROR;
  while(flash_ok != HAL_OK){
	flash_ok = HAL_FLASH_Lock();
  }
  
  
}

void ReadParam(void)
{
  uint32_t Address = 0x08010000; //
  freq = (*(__IO uint32_t*)(Address));
  att = (*(__IO uint32_t*)(Address+4));
  enable = (*(__IO uint32_t*)(Address+8));
  
  
  // защита
  if (enable > 1) { enable = 0; }
  if (freq < 25 || freq > 6000) { enable = 0; freq = 2000; }
  if (att > 63) {att = 0; enable = 0; freq = 2000; }  
  
}

void Error_Handler(void)
{

  while(1) 
  {
  }

}