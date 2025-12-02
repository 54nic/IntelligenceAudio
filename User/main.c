#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "ds1302.h"
#include "dht11.h"
#include "encoder.h"

uint8_t ReadTime[7];
extern uint8_t TIME[7];
extern uint8_t MenuIdx, MenuStatu;
extern uint8_t MenuIdxLimit[100];
char MainMenu[10][20] = {" ",
						 "1.Music Statu",
						 "2.Music List",
						 "3.Alarm",
						 "4.Environment",
						 "5.Settings",
						 "     ",
						 "     "};
char MusicStatu[10][20] = {" ", "Play", "Next", "Last", "Volumn", "All Songs", "Mode", "Back", "    "};
char MusicList[15][20] = {" ", "List1", "List2", "List3", "List4", "List5", "List6", "List7", "List8", "List9", "Back", " "};
char PlayingMode[10][20] = {" ", "Earphone", "Speaker", "Back", "   ", "    "};
char Alarm[10][20] = {" ", "Alarm1", "Alarm2", "Alarm3", "Alarm4", "Alarm5", "Back", "    "};
char Environment[10][20] = {" ", "Temperature", "Humidity", "Back", "  ", "   ", "    "};
char Settings[10][20] = {" ", "Time", "PlayingMode", "Back", " ", " ", " ", "    "};
char Songs[30][30] = {" ", "9420", "Black Magic", "Bleeding Love", "Bones", "Drown", "Flower dance", "In The Shadow Of The Sun", "Like I Love You", "Komorebi",
					  "Letting Go", "Star Unkind", "Stay", "Take a While", "Take Me Back", "Take Me to Infinity", "There For You", "Tonight", "Umbrella", "Viva La Vida", "Wait Another Day", "Watch Me Work", "What Do You Mean", "Back", "    "};
char ListChange[15][16] = {" ", "Add", "Back", " "};
uint8_t List[15][20] = {0};
uint8_t ring[15][3] = {{20, 24, 4}};
uint8_t ListNum[15] = {0};

uint8_t optRecord;
int getSong(int MenuStatu_)
{
	uint8_t inner_optRecord;
	MenuStatu = 20;
	while (1)
	{
		uint8_t opt = (1 + MenuIdx) / 2;
		if (inner_optRecord != opt)
			OLED_Clear();
		OLED_ShowString(1, 1, "Select a option");
		if (opt <= 0)
			opt = 1;
		OLED_ShowString(2, 1, Songs[opt - 1]);
		OLED_ShowString(3, 1, ">");
		OLED_ShowString(3, 2, Songs[opt]);
		OLED_ShowString(4, 1, Songs[opt + 1]);
		if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
		{
			OLED_Clear();
			MenuStatu = MenuStatu_;
			return opt;
		}

		inner_optRecord = opt;
	}
}
int main(void)
{
	int8_t playing = 0;
	/*模块初始化*/
	OLED_Init();   // OLED初始化
	Serial_Init(); // 串口初始化
	ds1302_init(); // 1302初始化
	DHT11_Init();
	Encoder_GPIO_Init();

	// Ds1302ReadTime(ReadTime);

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	// 定义结构体变量
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // GPIO速度，赋值为50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_4; // GPIO引脚，赋值为第0号引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

	/*设置发送数据包数组的初始值，用于测试*/

	// OLED_ShowHanZi(1, 1, 95);
	// OLED_ShowHanZi(1, 3, 97);
	// OLED_ShowHanZi(1, 5, 99);
	// OLED_ShowHanZi(2, 1, 101);
	// OLED_ShowHanZi(2, 3, 103);

	Serial_TxPacket[0] = 0x03; // 暂停
	Serial_TxPacket[1] = 0x02;
	Serial_TxPacket[2] = 0x01;
	Serial_SendPacket();
	OLED_ShowString(2, 2, "Welcome");

	Delay_ms(1000);
	while (1)
	{
		/* 读取当前时间 */
		Ds1302ReadTime(ReadTime);
		uint8_t current_hour = BCDTO10(ReadTime[2] & 0x7F); // 时(24小时制)
		uint8_t current_minute = BCDTO10(ReadTime[1]);		// 分

		for (int i = 0; i < 5; i++)
		{
			if (current_hour == ring[i][0] && current_minute == ring[i][1] && ring[i][2] != 0 && BCDTO10(ReadTime[0]) == 0)
			{
				Serial_SendByte(0x7e);
				Serial_SendByte(0x05);
				Serial_SendByte(0x41);
				Serial_SendByte(0x00);
				Serial_SendByte(ring[i][2]);
				Serial_SendByte(05 ^ (0x41) ^ 00 ^ ring[i][2]);
				Serial_SendByte(0xef);
				playing = 1;
			}
		}

		Serial_TxPacket[0] = 0x00;
		Serial_TxPacket[1] = 0x00;
		Serial_TxPacket[2] = 0x00;
		uint8_t opt = (1 + MenuIdx) / 2;
		if (opt != optRecord)
			OLED_Clear();
		// GPIO_SetBits(GPIOA, GPIO_Pin_4);		//LED灯亮一下
		// GPIO_ResetBits(GPIOA, GPIO_Pin_7);		//LED灯亮一下
		//	Delay_ms(50);
		if (MenuStatu == 0)
		{
			OLED_ShowString(1, 1, "Select a option");
			if (opt <= 0)
				opt = 1;
			// OLED_ShowNum(1,1, MenuIdx, 2);
			OLED_ShowString(2, 1, MainMenu[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, MainMenu[opt]);
			OLED_ShowString(4, 1, MainMenu[opt + 1]);
			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				MenuStatu = opt;
				MenuIdx = 2;
				Delay_ms(50);
				OLED_Clear();
			}
		}
		//"1.Music Statu",
		if (MenuStatu == 1)
		{
			if (Encoder_Sw_Down() == 1 && optRecord == 4) // 旋转编码器被按下
			{
				if (opt < optRecord)
				{
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x05;
					Serial_TxPacket[2] = 0x06;
					OLED_ShowString(3, 9, "++");
				}
				if (opt > optRecord)
				{
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x06;
					Serial_TxPacket[2] = 0x05;
					OLED_ShowString(3, 9, "--");
				}
				Serial_SendPacket(); // 串口发送数据包Serial_TxPacket
				opt = optRecord;
				MenuIdx = 2 * opt - 1;
			}
			OLED_ShowString(1, 1, "Select a option");
			if (opt <= 0)
				opt = 1;
			OLED_ShowString(2, 1, MusicStatu[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, MusicStatu[opt]);
			OLED_ShowString(4, 1, MusicStatu[opt + 1]);

			if (Encoder_Sw_Down() == 1 && opt != 4) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				if (opt == 1)
				{
					if (!playing)
					{
						Serial_TxPacket[0] = 0x03;
						Serial_TxPacket[1] = 0x01;
						Serial_TxPacket[2] = 0x02;
						playing = 1;
					}
					else
					{
						Serial_TxPacket[0] = 0x03; // 暂停
						Serial_TxPacket[1] = 0x02;
						Serial_TxPacket[2] = 0x01;
						playing = 0;
					}
				}
				else if (opt == 2)
				{

					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x03;
					Serial_TxPacket[2] = 0x00;
				}
				else if (opt == 3)
				{
					Serial_TxPacket[0] = 0x03;
					Serial_TxPacket[1] = 0x04;
					Serial_TxPacket[2] = 0x07;
				}
				else if (opt == 5)
				{
					MenuStatu = 11;
					MenuIdx = 2;
				}
				else if (opt == 6)
				{
				}
				else if (opt == 7)
				{
					MenuStatu = 0;
					MenuIdx = 2;
				}
				Serial_SendPacket();						  // 串口发送数据包Serial_TxPacket
				OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2); // 显示发送的数据包
				OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
				OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
				Delay_ms(100);
				OLED_Clear();
			}
		}
		//	"2.Music List",
		if (MenuStatu == 2)
		{
			OLED_ShowString(1, 1, "Select a option");
			if (opt <= 0)
				opt = 1;
			OLED_ShowString(2, 1, MusicList[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, MusicList[opt]);
			OLED_ShowString(4, 1, MusicList[opt + 1]);
			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				if (opt < 10)
				{
					MenuStatu = 20 + opt;
					MenuIdx = 2;
				}
				else if (opt == 10)
				{
					MenuStatu = 0;
					MenuIdx = 2;
				}
			}
		}
		//	"3.Alarm",
		if (MenuStatu == 3)
		{
			OLED_ShowString(1, 1, "Select a option");

			if (opt <= 0)
				opt = 1;

			OLED_ShowString(2, 1, Alarm[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, Alarm[opt]);
			OLED_ShowString(4, 1, Alarm[opt + 1]);

			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				if (opt < 6)
				{
					MenuStatu = 30 + opt;
				}
				else if (opt == 6)
					MenuStatu = 0;
				MenuIdx = 2;
			}
		}
		//	"4.Environment",
		if (MenuStatu == 4)
		{

			/* 在 if(MenuStatu == 5) 代码块顶部添加时间显示 */
			OLED_ShowNum(1, 1, current_hour, 2);
			OLED_ShowString(1, 3, ":");
			OLED_ShowNum(1, 4, current_minute, 2);
			OLED_ShowString(1, 6, ":");
			OLED_ShowNum(1, 7, BCDTO10(ReadTime[0]), 2); // 秒

			OLED_ShowString(2, 1, "Temperature");
			OLED_ShowString(3, 1, "Humidity");
			OLED_ShowString(4, 1, ">Back");

			uint8_t temph, templ, humih, humil, Val;
			uint8_t result = DHT11_Read_Data(&temph, &templ, &humih, &humil, &Val);
			if (result == 0) // 如果读取成功
			{

				OLED_ShowNum(2, 13, temph, 2); // 显示接收的数据包
				OLED_ShowNum(3, 10, humih, 2);
			}

			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				MenuStatu = 0;
				MenuIdx = 2;
			}
		}

		//	"5.Settings",
		if (MenuStatu == 5)
		{
			OLED_ShowString(1, 1, "Select a option");

			if (opt <= 0)
				opt = 1;

			OLED_ShowString(2, 1, Settings[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, Settings[opt]);
			OLED_ShowString(4, 1, Settings[opt + 1]);

			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				if (opt == 1)
				{
					MenuStatu = 51;
				}
				else if (opt == 2)
				{
					MenuStatu = 52;
				}
				else if (opt == 3)
				{
					MenuStatu = 0;
				}
				MenuIdx = 2;
			}
		}
		if (MenuStatu == 11)
		{
			OLED_ShowString(1, 1, "Select a option");
			if (opt <= 0)
				opt = 1;
			OLED_ShowString(2, 1, Songs[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, Songs[opt]);
			OLED_ShowString(4, 1, Songs[opt + 1]);

			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				if (opt < 23)
				{
					Serial_SendByte(0x7e);
					Serial_SendByte(0x05);
					Serial_SendByte(0x41);
					Serial_SendByte(0x00);
					Serial_SendByte(opt);
					Serial_SendByte(05 ^ (0x41) ^ 00 ^ opt);
					Serial_SendByte(0xef);
					playing = 1;
				}
				if (opt == 23)
				{
					MenuStatu = 0;
					MenuIdx = 2;
				}
			}
		}
		if (MenuStatu > 20 && MenuStatu < 30)
		{
			OLED_ShowString(1, 1, "Edit Your Songs");

			if (opt <= 0)
				opt = 1;
			uint8_t id = MenuStatu - 20;

			if (opt < ListNum[id])
			{
				OLED_ShowString(2, 1, Songs[List[id][opt - 1]]);
				OLED_ShowString(3, 1, ">");
				OLED_ShowString(3, 2, Songs[List[id][opt]]);
				OLED_ShowString(4, 1, Songs[List[id][opt + 1]]);
			}
			else if (opt == ListNum[id])
			{
				OLED_ShowString(2, 1, Songs[List[id][opt - 1]]);
				OLED_ShowString(3, 1, ">");
				OLED_ShowString(3, 2, Songs[List[id][opt]]);
				OLED_ShowString(4, 1, "Add");
			}
			else if (opt == ListNum[id] + 1)
			{

				OLED_ShowString(2, 1, Songs[List[id][opt - 1]]);
				OLED_ShowString(3, 1, ">");
				OLED_ShowString(3, 2, "Add");
				OLED_ShowString(4, 1, "Back");
			}
			else if (opt == ListNum[id] + 2)
			{

				OLED_ShowString(2, 1, "Add");
				OLED_ShowString(3, 1, ">");
				OLED_ShowString(3, 2, "Back");
			}

			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				if (opt == ListNum[id] + 1)
				{
					int NSong = getSong(MenuStatu);
					List[id][opt] = NSong;
					ListNum[id]++;
					MenuIdxLimit[MenuStatu] += 2;
				}
				else if (opt == ListNum[id] + 2)
				{
					MenuStatu = 0;
				}

				else if (opt <= ListNum[id])
				{
					for (int i = opt; i < ListNum[id]; i++)
					{
						List[id][i] = List[id][i + 1];
					}
					ListNum[id]--;
					MenuIdxLimit[MenuStatu] -= 2;
				}
				MenuIdx = 2;
			}
		}

		if (MenuStatu > 30 && MenuStatu < 40)
		{

			uint8_t timesetting[3] = {BCDTO10(TIME[0]), BCDTO10(TIME[1]), BCDTO10(TIME[2])};

			OLED_ShowString(1, 2, "hour:");
			OLED_ShowNum(1, 9, timesetting[2], 2);
			OLED_ShowString(2, 2, "Minute:");
			OLED_ShowNum(2, 9, timesetting[1], 2);
			OLED_ShowString(3, 2, "Second:");
			OLED_ShowNum(3, 9, timesetting[0], 2);
			OLED_ShowString(4, 2, "Back");
		}
		if (MenuStatu == 51)
		{
			if (opt <= 0)
				opt = 1;
			uint8_t timesetting[3] = {BCDTO10(TIME[0]), BCDTO10(TIME[1]), BCDTO10(TIME[2])};

			OLED_ShowString(1, 2, "hour:");
			OLED_ShowNum(1, 9, timesetting[2], 2);
			OLED_ShowString(2, 2, "Minute:");
			OLED_ShowNum(2, 9, timesetting[1], 2);
			OLED_ShowString(3, 2, "Second:");
			OLED_ShowNum(3, 9, timesetting[0], 2);
			OLED_ShowString(4, 2, "Back");

			OLED_ShowString(opt, 1, ">");

			if (Encoder_Sw_Down() == 1)
			{
				if (opt < 4)
				{
					timesetting[3 - opt]++;
					if (timesetting[2] >= 24)
						timesetting[2] -= 24;
					if (timesetting[1] >= 60)
						timesetting[1] -= 60;
					if (timesetting[0] >= 60)
						timesetting[0] -= 60;
					TIME[0] = Z10BCD(timesetting[0]);
					TIME[1] = Z10BCD(timesetting[1]);
					TIME[2] = Z10BCD(timesetting[2]);
				}
				else if (opt == 4)
				{
					Ds1302Set();
					MenuStatu = 0;
					MenuIdx = 2;
				}
			}
		}
		//	"52.Playing Mode",
		if (MenuStatu == 52)
		{

			OLED_ShowString(1, 1, "Select a option");
			if (opt <= 0)
				opt = 1;
			OLED_ShowString(2, 1, PlayingMode[opt - 1]);
			OLED_ShowString(3, 1, ">");
			OLED_ShowString(3, 2, PlayingMode[opt]);
			OLED_ShowString(4, 1, PlayingMode[opt + 1]);
			for (int i = 0; i < 3; i++)
			{
			}
			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				Delay_ms(50);
				OLED_Clear();
				if (opt == 1)
				{
					GPIO_SetBits(GPIOA, GPIO_Pin_4); // LED灯亮一下
					GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				}
				else if (opt == 2)
				{
					GPIO_ResetBits(GPIOA, GPIO_Pin_4); // LED灯亮一下
					GPIO_ResetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
				}
				else if (opt == 3)
				{
					MenuStatu = 0;
					MenuIdx = 2;
				}
			}
		}
		optRecord = opt;
	}
}
