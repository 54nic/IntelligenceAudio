#include "dht11.h"
#include "Delay.h"

/*-----------------------------------------------------------------------
DHT11_Rst  			 							  : ��λDHT11


�������                   			��

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
void DHT11_Rst(void)
{
	DHT11_IO_OUT();	  // SET OUTPUT
	DHT11_DQ_OUT = 0; // ����DQ
	Delay_ms(20);	  // ��������18ms
	DHT11_DQ_OUT = 1; // DQ=1
	Delay_us(30);	  // ��������20~40us
}

/*-----------------------------------------------------------------------
DHT11_Check  			 						  : �ȴ�DHT11�Ļ�Ӧ


�������                   			������1:δ��⵽DHT11�Ĵ��� ����0:����

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
uint8_t DHT11_Check(void)
{
	uint8_t retry = 0;
	DHT11_IO_IN();					   // SET INPUT
	while (DHT11_DQ_IN && retry < 100) // DHT11������40~80us
	{
		retry++;
		Delay_us(1);
	};
	if (retry >= 100)
		return 1;
	else
		retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		Delay_us(1);
	};
	if (retry >= 100)
		return 1;
	return 0;
}

/*-----------------------------------------------------------------------
DHT11_Read_Bit  			 				  : ��DHT11��ȡһ��λ


�������                   			��

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0;
	while (DHT11_DQ_IN && retry < 100) // �ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_us(1);
	}
	retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // �ȴ���ߵ�ƽ
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40); // �ȴ�40us
	if (DHT11_DQ_IN)
		return 1;
	else
		return 0;
}

/*-----------------------------------------------------------------------
DHT11_Read_Byte  			 				  : ��DHT11��ȡһ���ֽ�


�������                   			����������

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
uint8_t DHT11_Read_Byte(void)
{
	uint8_t i, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;
}

/*-----------------------------------------------------------------------
DHT11_Read_Data  			 				  : ��DHT11��ȡһ������


�������                   			��temp:�¶�ֵ(��Χ:0~50��)humi:ʪ��ֵ(��Χ:20%~90%)����ֵ��0,����;1,��ȡʧ��

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
uint8_t DHT11_Read_Data(uint8_t *temph, uint8_t *templ, uint8_t *humih, uint8_t *humil, uint8_t *Val)
{
	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if (DHT11_Check() == 0)
	{
		for (i = 0; i < 5; i++) // ��ȡ40λ����
		{
			buf[i] = DHT11_Read_Byte();
		}
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			*humih = buf[0];
			*humil = buf[1];

			*temph = buf[2];
			*templ = buf[3];
		}

		if ((buf[1] & 0x80) == 0) // �¶�>0
		{
			*Val = 0;
		}
		if ((buf[1] & 0x80) == 1) // �¶�<0
		{
			*Val = 1;
		}
	}
	else
		return 1;
	return 0;
}

/*-----------------------------------------------------------------------
DHT11_Init      			 				  : ��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
																									 IOKOU B7 TO A6

�������                   			������1:������  ����0:����

��д����                        	��2018��11��21��
����޸�����                  		��2018��11��21��
-----------------------------------------------------------------------*/
uint8_t DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��PG�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;

	// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	                    // ʹ��PG�˿�ʱ��
	//
	// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				                          // PA6�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��IO��
	GPIO_SetBits(GPIOA, GPIO_Pin_6);	   // PA6 �����

	DHT11_Rst();		  // ��λDHT11
	return DHT11_Check(); // �ȴ�DHT11�Ļ�Ӧ
}
