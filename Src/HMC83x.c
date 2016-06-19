#include "HMC83x.h"

// XTAL/R <= 80 МГц 
float XTAL = 25; //частота кварцевого генератора в МГц
float R = 1; // Делитель частоты кварцевого генератора

uint32_t BufferTX[1];
uint32_t BufferRX[1];

void HMC83x_write(uint32_t a, uint32_t d) 
{	
	BufferTX[0] = 0;

	// формируем слово для HMC mode
	BufferTX[0] = BufferTX[0] | a << 25;
	BufferTX[0] = BufferTX[0] | d << 1;

        
	// формируем слово для OPEN mode
 	//BufferTX[0] = BufferTX[0] | d << 8;
	//BufferTX[0] = BufferTX[0] | a << 3;
   
        
        // convert from little endian to big endian
        BufferTX[0] = ( BufferTX[0] >> 24 ) | (( BufferTX[0] << 8) & 0x00ff0000 )| ((BufferTX[0] >> 8) & 0x0000ff00) | ( BufferTX[0] << 24)  ; 

        
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_SPI_Transmit(&hspi1, (uint8_t*)BufferTX, 4, 10);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

}

uint32_t HMC83x_read(uint8_t adr) 
{	
  BufferTX[0] = 0x00000080 | (adr << 1) ;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)BufferTX, (uint8_t*)BufferTX, 4, 64);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  BufferTX[0] = ( BufferTX[0] >> 24 ) | (( BufferTX[0] << 8) & 0x00ff0000 )| ((BufferTX[0] >> 8) & 0x0000ff00) | ( BufferTX[0] << 24)  ; 
  return BufferTX[0];

}

void SetFreq(float freq)
{
	int K = 1;
	int Nint = 0;
	int Nfract = 0;

	float freq1 = 0;


	if (freq >=25 && freq <= 1500) // Divide by N mode
		{	// ищем K
			for (K = 2;;K = K + 2) { 
				freq1 = freq * K; 
				if (freq1 > 1500) break; }
		
			freq = freq1;
			
			Nint = (int)(freq*R/XTAL);
			Nfract = (int)((((freq*R/XTAL)  - Nint) *0x00FFFFFF ) + 0.5);
		
			HMC83x_write(2, (uint8_t)R); // REFDIV 
	

			
			// VCO Subsystems
			//				   data		    addr	  id
			HMC83x_write(5, (0x2C << 7) | (5 << 3) | 0); //  5.5
			HMC83x_write(5, (0xC1 << 7) | (4 << 3) | 0); //  5.4

			
			HMC83x_write(5, (0x11 << 7) | (3 << 3) | 0); //  5.3 // 0x41 or 0x51 ???
			if (K < 4) 
                          HMC83x_write(5, ((0x1C0 | K)  << 7) | (2 << 3) | 0); //  5.2 
			else
                          HMC83x_write(5, ((0x0C0 | K)  << 7) | (2 << 3) | 0); //  5.2 

			HMC83x_write(5, 0); // конец записи в VCO


			//
			HMC83x_write(6,  0x200B4A); 
			HMC83x_write(7,  0x00014D); 
			HMC83x_write(8,  0xC1BEFF); 
			HMC83x_write(9,  0x153FFF); 
			HMC83x_write(10, 0x002046);
			HMC83x_write(11, 0x07C061);
			HMC83x_write(12, 0x000000);
			HMC83x_write(15, 0x000001);


			//
			HMC83x_write(4, Nfract);  //FRACT
			HMC83x_write(3, Nint); // INT
			freq = 0;

		}

	if (freq >1500 && freq <= 3000) // fundamental mode
		{
			Nint = (int)(freq*R/XTAL);
			Nfract = (int)((((freq*R/XTAL)  - Nint) *0x00FFFFFF ) + 0.5);
		
			HMC83x_write(2, (uint8_t)R); // REFDIV 


			// VCO Subsystems
			//				   data		    addr	  id
			HMC83x_write(5, (0x2C << 7) | (5 << 3) | 0); //  5.5
			HMC83x_write(5, (0xC1 << 7) | (4 << 3) | 0); //  5.4
			HMC83x_write(5, (0x51 << 7) | (3 << 3) | 0); //  5.3 // 0x41 or 0x51 ???
			HMC83x_write(5, ((0x1C0 | 1)  << 7) | (2 << 3) | 0); //  5.2 
			HMC83x_write(5, 0); // конец записи в VCO


			//
			HMC83x_write(6,  0x200B4A); 
			HMC83x_write(7,  0x00014D); 
			HMC83x_write(8,  0xC1BEFF); 
			HMC83x_write(9,  0x153FFF); 
			HMC83x_write(10, 0x002046);
			HMC83x_write(11, 0x07C061);
			HMC83x_write(12, 0x000000);
			HMC83x_write(15, 0x000001);


			//
			HMC83x_write(4, Nfract);  //FRACT
			HMC83x_write(3, Nint); // INT
			freq = 0;
		}

	if (freq >3000 && freq <= 6000) // doubler mode
		{   
			freq = freq / 2;

			Nint = (int)(freq*R/XTAL);
			Nfract = (int)((((freq*R/XTAL)  - Nint) *0x00FFFFFF ) + 0.5);
		
			HMC83x_write(2, (uint8_t)R); // REFDIV 


			// VCO Subsystems
			//				   data		    addr	  id
			HMC83x_write(5, (0x2C << 7) | (5 << 3) | 0); //  5.5
			HMC83x_write(5, (0xC1 << 7) | (4 << 3) | 0); //  5.4
			HMC83x_write(5, (0x00 << 7) | (3 << 3) | 0); //  5.3
			HMC83x_write(5, (0x01 << 7) | (2 << 3) | 0); //  5.2 
			HMC83x_write(5, 0); // конец записи в VCO


			//
			HMC83x_write(6,  0x200B4A); 
			HMC83x_write(7,  0x00014D); 
			HMC83x_write(8,  0xC1BEFF); 
			HMC83x_write(9,  0x153FFF); 
			HMC83x_write(10, 0x002046);
			HMC83x_write(11, 0x07C061);
			HMC83x_write(12, 0x000000);
			HMC83x_write(15, 0x000001);


			//
			HMC83x_write(4, Nfract);  //FRACT
			HMC83x_write(3, Nint); // INT	
		}


}      

void En(uint8_t enable)
{
	if (enable == 0 ) // выключение
	{ 
		HMC83x_write(5, (0x00 << 7) | (1 << 3) | 0); //  5.1 = Disable All 
                HMC83x_write(5, 0); // конец записи в VCO
		//HMC83x_write(1,  0x00); // PLL disable
	}

	else // включение
	{        
   
		//HMC83x_write(1,  0x02); // PLL enable
		HMC83x_write(5, (0x1F << 7) | (1 << 3) | 0); //  5.1 = Enable All 
                HMC83x_write(5, 0); // конец записи в VCO
                
		SetFreq(freq);
               
	}


}