#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"
#include "sys.h"

#define DHT11_IO_IN()             \
    {                             \
        GPIOA->CRL &= 0XF0FFFFFF; \
        GPIOA->CRL |= 8 << 24;    \
    } // IO��������(PA6 ����(CRL 0-7 CRH 8-15)
#define DHT11_IO_OUT()            \
    {                             \
        GPIOA->CRL &= 0XF0FFFFFF; \
        GPIOA->CRL |= 3 << 24;    \
    }
#define DHT11_DQ_OUT PAout(6) // ���ݶ˿�
#define DHT11_DQ_IN PAin(6)   // ���ݶ˿�

uint8_t DHT11_Init(void);                                                                              // ��ʼ��DHT11
uint8_t DHT11_Read_Data(uint8_t *temph, uint8_t *templ, uint8_t *humih, uint8_t *humil, uint8_t *Val); // ��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);                                                                         // ����һ���ֽ�
uint8_t DHT11_Read_Bit(void);                                                                          // ����һ��λ
uint8_t DHT11_Check(void);                                                                             // ����Ƿ����DHT11
void DHT11_Rst(void);                                                                                  // ��λDHT11

#endif
