#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"  
#include "sys.h"
 


#define DHT11_IO_IN()  {GPIOA->CRL&=0XF0FFFFFF;GPIOA->CRL|=8<<24;}                                   // IO方向设置(PA6 上拉(CRL 0-7 CRH 8-15)
 #define DHT11_IO_OUT() {GPIOA->CRL&=0XF0FFFFFF;GPIOA->CRL|=3<<24;}
#define	DHT11_DQ_OUT PAout(6)                                                                         // 数据端口	 
#define	DHT11_DQ_IN  PAin(6)                                                                          // 数据端口  





uint8_t DHT11_Init(void);                                                                              // 初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temph,uint8_t *templ,uint8_t *humih,uint8_t *humil,uint8_t * Val);    // 读取温湿度
uint8_t DHT11_Read_Byte(void);                                                                         // 读出一个字节
uint8_t DHT11_Read_Bit(void);                                                                          // 读出一个位
uint8_t DHT11_Check(void);                                                                             // 检测是否存在DHT11
void DHT11_Rst(void);                                                                                  // 复位DHT11  


#endif










