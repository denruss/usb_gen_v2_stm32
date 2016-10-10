
#include "main.h"

uint8_t version = 1; 
uint8_t subversion = 9; 


float freq = 2000.0; //действующие значения от 25 до 6000 МГц

uint32_t att = 0; //действующие значения от 0 до 63, 63 - максимальный сигнал, 0- минимальный, шаг 0,5 дБ
uint8_t enable = 0; //
uint8_t mode = 0; // режим работы, 0 - ГСС, 1 - ГКЧ, 2 - ППРЧ
uint8_t new_flag = 1; 


uint8_t dataToSend[64] = {0}; // Массив байт, для отправки ПК

// для режима ГКЧ 
float freq_start = 1000.0; 
float freq_stop = 1010.0; 
float freq_step = 0.1; 
uint8_t delay = 1; // в милисекундах


extern uint32_t __checksum;                                 // контрольная сумма
#pragma section=".checksum"                                 //

int main(void)
{
  
  uint32_t crc = __checksum;                               // без этой строчки ошибка линкера
  float temp = 0;    
   
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


  InitHMC83x(); 
  
  // загружаем сохранённые настройки
  ReadParam();
  SetAtt(att);
//  En(enable);  Led(enable); 

  
  while (1)
  {     
        switch ( mode )
        {
         case 0: // ГСС
           if (new_flag != 0) {
             HAL_Delay(2);
             En(enable);  
             Led(enable);
             HAL_Delay(2);             
             new_flag = 0; }
           break;
         case 1: // ГКЧ
           if (enable == 1) {
              for (float i = freq_start; i < freq_stop;  i = i + freq_step)
              {   SetFreq(i);
                  HAL_Delay(delay);
                  do
                  {
                    data = HMC83x_read(0x12) & 2 ; 
                  }
                   while (data != 2 && new_flag == 0); // ждём Lock Detect
                   if (new_flag == 1) break;
                  
              }
           }  
           if (new_flag != 0 && mode == 1) {
              HAL_Delay(2);
              En(enable);  
              Led(enable);
              HAL_Delay(2);
              new_flag = 0; }           
           
           break;
         case 2: // ППРЧ
           if (enable == 1) {
             temp =  (float)rand();
             temp = (float)(temp / RAND_MAX) * (freq_stop - freq_start) + freq_start;
             SetFreq(temp);
             do
               {
                 data = HMC83x_read(0x12) & 2 ; 
               }
             while (data != 2 && new_flag == 0); // ждём Lock Detect
             if (new_flag == 1 && mode == 2) {
                HAL_Delay(2);
                En(enable);  
                Led(enable);
                HAL_Delay(2);
                new_flag = 0; }   
           }  
 
            break;
         default:
            break;
        }

    
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
  
  uint64_t temp;  
  //temp = freq_step;
  *((float*) &temp) = freq_step;
  
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
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *((uint64_t*)&freq));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+4, (uint64_t)att);  
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+8, (uint64_t)enable);  
  
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+12, *((uint64_t*)&freq_start));    
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+16, *((uint64_t*)&freq_stop));  
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+20, *((uint64_t*)&freq_step));  
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+24, (uint64_t)delay);  

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+28, (uint64_t)mode); 
  
  //Закрываем память
  flash_ok = HAL_ERROR;
  while(flash_ok != HAL_OK){
	flash_ok = HAL_FLASH_Lock();
  }
  
  
}

void ReadParam(void)
{
  uint32_t Address = 0x08010000; //
  freq = (*(__IO float*)(Address));
  att = (*(__IO uint32_t*)(Address+4));
  enable = (uint8_t)(*(__IO uint32_t*)(Address+8));
 
  freq_start = (*(__IO float*)(Address+12));
  freq_stop = (*(__IO float*)(Address+16));
  freq_step = (*(__IO float*)(Address+20));
  delay = (uint8_t)(*(__IO uint32_t*)(Address+24));  

  mode = (uint8_t)(*(__IO uint32_t*)(Address+28));    
  
  // защита
  if (enable > 1) { enable = 0; }
  if (freq < 25 || freq > 6000 || *(uint32_t*)&freq == 0xFFFFFFFF) { enable = 0; freq = 2000; }
  if (att > 63) {att = 0; enable = 0; freq = 2000; }  
 
  if (freq_start < 25 || freq_start > 6000 || *(uint32_t*)&freq_start == 0xFFFFFFFF) { freq_start = 1000; }
  if (freq_stop < 25 || freq_stop > 6000 || *(uint32_t*)&freq_stop == 0xFFFFFFFF) {  freq_stop = 1010; }
  if (freq_step < 0.1 || freq_step > 99.9 || *(uint32_t*)&freq_step == 0xFFFFFFFF) { freq_step = 0.5; }  
  //if (delay > 255) { delay = 1; }  
  
  if (mode > 2) { mode = 0; }    
  
}

void Error_Handler(void)
{

  while(1) 
  {
  }

}