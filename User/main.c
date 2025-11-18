#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Serial.h"
#include "Key.h"
#include "ds1302.h"
#include "dht11.h"
#include "encoder.h"

uint8_t ReadTime[7];
extern uint8_t MenuIdx, MenuStatu;
	char MainMenu[10][20]={" ",
	"1.Music Statu",
	"2.Music List",
	"3.Playing Mode",
	"4.Alarm",
	"5.Environment",
	"     ",
	"     "};
	char MusicStatu[10][20]={" ",	"Play",	"Next",	"Last",	"Volumn",	"Back",	"    ",	"   ",	"    "};
int main(void)
{
	char MusicList[15][20]={" ","List", "List","List","List","List","List","List","List"," "};
	char PlayingMode[10][20]={" ",	"Earphone",	"Speaker",	"Back",	"   ",	"    "};
	char Alarm[10][20]={" ",	"Alarm",	"Alarm",	"Alarm",	"Alarm",	"Alarm",	"Alarm","    "};
	char Environment[10][20]={" ",	"Play",	"Next",	"Last",	"Volumn",	"Back",	"    ",	"   ",	"    "};
	/*模块初始化*/
	LED_Init();		//LED初始化
	OLED_Init();		//OLED初始化
	Key_Init();			//按键初始化
	Serial_Init();		//串口初始化
	ds1302_init();		//1302初始化
	DHT11_Init();
  Encoder_GPIO_Init();
	
	//Ds1302ReadTime(ReadTime);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	//定义结构体变量
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//GPIO引脚，赋值为第0号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			
	
	/*设置发送数据包数组的初始值，用于测试*/
	
	//OLED_ShowHanZi(1, 1, 95);
	//OLED_ShowHanZi(1, 3, 97);
	//OLED_ShowHanZi(1, 5, 99);
	//OLED_ShowHanZi(2, 1, 101);
	//OLED_ShowHanZi(2, 3, 103);
	OLED_ShowString(2,2,"Welcome");
	Delay_ms(1000);
	while (1)
	{			
		OLED_Clear();
		if(MenuStatu == 0){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1+MenuIdx) / 2;
			if(opt<=0) opt = 1;
			//OLED_ShowNum(1,1, opt, 2);
			OLED_ShowString(2,1, MainMenu[opt-1]);   
			OLED_ShowString(3,1, ">");     
			OLED_ShowString(3,2, MainMenu[opt]);    
			OLED_ShowString(4,1, MainMenu[opt+1]);    
			Delay_ms(50);
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);		//LED灯灭一下
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
					MenuStatu = opt;
					MenuIdx = 2;
			}
		}
		//	"1.Music Statu",
		else if(MenuStatu == 1){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1+MenuIdx) / 2;
			if(opt<=0) opt = 1;
			OLED_ShowString(2,1, MusicStatu[opt-1]);   
			OLED_ShowString(3,1, ">");     
			OLED_ShowString(3,2, MusicStatu[opt]);    
			OLED_ShowString(4,1, MusicStatu[opt+1]);
			
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
				if(opt == 1){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 2){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 3){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 4){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 5){
					MenuStatu = 0;
					MenuIdx = 2;
			}
		}
		}
		//	"2.Music List",
		else if(MenuStatu == 2){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1 + MenuIdx) / 2;
			if(opt <= 0) opt = 1;
			if(opt > 1){
				OLED_ShowString(2,1, "List");
				OLED_ShowNum(2,5,opt-1,1);    
			}
			if(opt < 10){
				OLED_ShowString(3,1, ">"); 
				OLED_ShowString(3,2, "List");
				OLED_ShowNum(3,6,opt,1);
			}
			else if(opt == 10){
				OLED_ShowString(3,1, ">"); 
				OLED_ShowString(3,2, "Back");
			}
		
			if(opt < 9){
				OLED_ShowString(4,1, "List");
				OLED_ShowNum(4,5,opt+1,1);    
			}
			else if(opt == 9) {
				OLED_ShowString(4,1, "Back");
			}
			
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
				if(opt == 1){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 2){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 3){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 4){
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x01;
					Serial_TxPacket[2] = 0x02;
					Serial_SendPacket();		//串口发送数据包Serial_TxPacket
				}
				else if(opt == 10){
					MenuStatu = 0;
					MenuIdx = 2;
			}
		}
			//	"3.Playing Mode",
		else if(MenuStatu == 3){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1+MenuIdx) / 2;
			if(opt<=0) opt = 1;
			OLED_ShowString(2,1, MusicStatu[opt-1]);   
			OLED_ShowString(3,1, ">");     
			OLED_ShowString(3,2, MusicStatu[opt]);    
			OLED_ShowString(4,1, MusicStatu[opt+1]);
			
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
					if(opt == 1){}
					else if(opt == 2){}
					else if(opt == 3){}
					else if(opt == 4){}
					else if(opt == 5){
					MenuStatu = 0;
					MenuIdx = 2;
			}
		}}
			//	"4.Alarm",
		else if(MenuStatu == 4){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1+MenuIdx) / 2;
			if(opt<=0) opt = 1;
			OLED_ShowString(2,1, MusicStatu[opt-1]);   
			OLED_ShowString(3,1, ">");     
			OLED_ShowString(3,2, MusicStatu[opt]);    
			OLED_ShowString(4,1, MusicStatu[opt+1]);
			
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
				if(opt == 1);
				else if(opt == 2);
				else if(opt == 3);
				else if(opt == 4);
				else if(opt == 5)
					MenuStatu = 0;
					MenuIdx = 2;
			}
		}}
		//	"5.Environment",
		else if(MenuStatu == 5){
			OLED_ShowString(1,1,"Select a option");
			uint8_t opt = (1+MenuIdx) / 2;
			if(opt<=0) opt = 1;
			OLED_ShowString(2,1, MusicStatu[opt-1]);   
			OLED_ShowString(3,1, ">");     
			OLED_ShowString(3,2, MusicStatu[opt]);    
			OLED_ShowString(4,1, MusicStatu[opt+1]);
			
			if(Encoder_Sw_Down() == 1 )//旋转编码器被按下
			{
					GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
					Delay_ms(50);	
				if(opt == 1);
				else if(opt == 2);
				else if(opt == 3);
				else if(opt == 4);
				else if(opt == 5)
					MenuStatu = 0;
					MenuIdx = 2;
			}
		}
		else 
					MenuIdx = 0;
		//if (KeyNum == 1)	
		//{	
		//	GPIO_SetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
		//	Delay_ms(100);	
		//		
		//		Serial_TxPacket[0] = 0x03;
		//		Serial_TxPacket[1] = 0x01;
		//		Serial_TxPacket[2] = 0x02;
		//		
		//		Serial_SendPacket();		//串口发送数据包Serial_TxPacket
		//		
		//		OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);	//显示发送的数据包
		//		OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
		//		OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
		//}
		//	if (KeyNum == 2)	
		//	{
		//	GPIO_SetBits(GPIOA, GPIO_Pin_7);					//将PA0引脚设置为高电平
		//	Delay_ms(100);	
		//		Serial_TxPacket[0] = 0x03;
		//		Serial_TxPacket[1] = 0x02;
		//		Serial_TxPacket[2] = 0x01;
		//		
		//		Serial_SendPacket();		//串口发送数据包Serial_TxPacket
		//		
		//		OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);	//显示发送的数据包
		//		OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
		//		OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
		//	}
		//	if (KeyNum == 3)				
		//	{
		//	GPIO_SetBits(GPIOA, GPIO_Pin_7);					//将PA0引脚设置为高电平
		//	Delay_ms(100);	
		//		Serial_TxPacket[0] = 0x03;
		//		Serial_TxPacket[1] = 0x03;
		//		Serial_TxPacket[2] = 0x00;
		//		
		//		Serial_SendPacket();		//串口发送数据包Serial_TxPacket
		//		
		//		OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);	//显示发送的数据包
		//		OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
		//		OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
		//	}
		//	if (KeyNum == 4)
		//	{
		//	GPIO_SetBits(GPIOA, GPIO_Pin_7);					//将PA0引脚设置为高电平
		//	Delay_ms(100);	
		//		Serial_TxPacket[0] = 0x03;
		//		Serial_TxPacket[1] = 0x05;
		//		Serial_TxPacket[2] = 0x06;
		//		
		//		Serial_SendPacket();		//串口发送数据包Serial_TxPacket
		//		
		//		OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);	//显示发送的数据包
		//		OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
		//		OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
		//	}
		//	if (KeyNum == 5)
		//	{
		//	GPIO_SetBits(GPIOA, GPIO_Pin_7);					//将PA0引脚设置为高电平
		//	Delay_ms(100);	
		//		
		//		Serial_TxPacket[0] = 0x03;
		//		Serial_TxPacket[1] = 0x06;
		//		Serial_TxPacket[2] = 0x05;
		//		
		//		Serial_SendPacket();		//串口发送数据包Serial_TxPacket
		//		
		//		OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);	//显示发送的数据包
		//		OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
		//		OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
		//	}
		//	
		//	if (Serial_GetRxFlag() == 1)	//如果接收到数据包
		//	{
		//		OLED_ShowHexNum(4, 1, Serial_RxPacket[0], 2);	//显示接收的数据包
		//		OLED_ShowHexNum(4, 4, Serial_RxPacket[1], 2);
		//		OLED_ShowHexNum(4, 7, Serial_RxPacket[2], 2);
		//	}
		}
	
}
