#ifndef __DS1302_H
#define __DS1302_H

#include "stm32f10x.h"  
#include "sys.h"

#define DS_CLK PBout(0)
#define DS_DIO PBout(1)
#define DS_RST PBout(10)


//#define DS_CLK PAout(2)
//#define DS_DIO PAout(1)
//#define DS_RST PAout(0)

extern uint8_t TIME[7];
 
uint8_t read_1302(uint8_t add);
void write_1302byte(uint8_t dat) ;
void write_1302(uint8_t add,uint8_t dat);
void ds1302_init(void) ;
void Ds1302ReadTime(uint8_t *Read_Time);
uint8_t Judge_Week(uint16_t y,uint8_t m,uint8_t d); 
uint8_t Z10BCD( uint8_t date);   
uint8_t BCDTO10( uint8_t date );    
 
void GPIO_Configuration(void);

#endif



