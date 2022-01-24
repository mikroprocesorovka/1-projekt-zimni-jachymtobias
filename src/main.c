#include "stm8s.h"
#include "milis.h"
#include "swspi.h"

#define DECODEMODE        (0x9<<8)
#define INTENSITY         (0xa<<8)
#define SCANLIMIT         (0xb<<8)
#define SHUTDOWN          (0xc<<8)
#define DTEST             (0xf<<8)

#define ZNAK1             (0x1<<8)
#define ZNAK2             (0x2<<8)
#define ZNAK3             (0x3<<8)
#define ZNAK4             (0x4<<8)
#define ZNAK5             (0x5<<8)
#define ZNAK6             (0x6<<8)
#define ZNAK7             (0x7<<8)
#define ZNAK8             (0x8<<8)

void max7219_init(void);
void zobraz(void);
void aktualizace(void);
void stisk(void);
void kontrola(void);

uint8_t cas1,cas2,cas3,cas4,cislo;


void main(void){
	GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW);
	
	GPIO_Init(GPIOB,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_SLOW);
	
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	init_milis();
	swspi_init();
	max7219_init();
	
	cas1=0;  //sec
	cas2=1;
	
	cas3=0;  //min
	cas4=0;
	
	
	
	while(1){
		zobraz();
		kontrola();
		aktualizace();
		stisk();
	}
}

void kontrola(void){
	if(cas4+cas3+cas2+cas1==0){
		GPIO_WriteHigh(GPIOC,GPIO_PIN_5);
		GPIO_WriteLow(GPIOB,GPIO_PIN_7);
		delay_ms(300);
		GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	}else{
		GPIO_WriteLow(GPIOC,GPIO_PIN_5);
		GPIO_WriteHigh(GPIOB,GPIO_PIN_7);
	}
}

void aktualizace(void){
		static uint16_t last_time = 0;
		
		if (milis() - last_time >= 1000){			//každou sekundu
		last_time = milis();
		
		if(cas1>0){  
			cas1=cas1-1;
		}else if(cas2>0){
			cas2=cas2-1;
			cas1=9;
		}else if(cas3>0){ //min
			cas3=cas3-1;
			cas2=5;
			cas1=9; 
		}else if(cas4>0){
			cas4=cas4-1;
			cas3=9;
			cas2=5;
			cas1=9;
		}
	}
}

void stisk(void){
	static minule;
	
	if (minule != RESET && GPIO_ReadInputPin(GPIOE,GPIO_PIN_4) == RESET){
		cas3=cas3+1;
		if (cas3==9){
			cas3=0;
			cas4=cas4+1;
		}
	}
	minule = GPIO_ReadInputPin(GPIOE,GPIO_PIN_4);
}

void zobraz(void){
		swspi_tx16(ZNAK1 | 0);
		swspi_tx16(ZNAK2 | 0);
		swspi_tx16(ZNAK3 | cas1);  //sec
		swspi_tx16(ZNAK4 | cas2);
		
		swspi_tx16(ZNAK5 | cas3);  //min
		swspi_tx16(ZNAK6 | cas4);
		swspi_tx16(ZNAK7 | 0);
		swspi_tx16(ZNAK8 | 0);
}

void max7219_init(void){
	swspi_tx16(DECODEMODE | 0xff);
	swspi_tx16(INTENSITY  | 0);
	swspi_tx16(SCANLIMIT  | 7);
	swspi_tx16(DTEST      | 0);
	swspi_tx16(SHUTDOWN   | 1);

}


// pod tímto komentářem nic neměňte 
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */                                                
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */                               
  while (1)
  {
  }
}
#endif