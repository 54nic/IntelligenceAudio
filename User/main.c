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

uint8_t List[15][20] = {0};
uint8_t ring[15][3] = {{1, 30, 6}}; //{h,m,sid}
uint8_t ListNum[15] = {0};

void OLED_ShowStringL(int line, int col, char* str){
	OLED_ShowString((col-1) * 8, (line-1) * 16, str, OLED_8X16);
}
void OLED_ShowNumL(int line, int col, int num, int bt){
	OLED_ShowNum((col-1) * 8, (line-1) * 16, num, bt, OLED_8X16);
}

int MenuLoop(char **Menu, int max){
	OLED_Clear();
	int optRecord, opt = 1;
	while (1)
	{
		
		int tmp = Encoder_Get() / 4;
		if (tmp > 0)
			opt = optRecord + 1;
		if (tmp < 0)
			opt = optRecord - 1;

		if (opt < 1)
			opt = 1;
		if (opt > max)
			opt = max;
		if (opt != optRecord)
		
		OLED_Clear();
		OLED_ShowImage(80,28,40,36,Xxyy);

		OLED_ShowStringL(1, 1, "欢迎使用智能音频");

		OLED_ShowStringL(2, 1, Menu[opt - 1]);
		OLED_ShowStringL(3, 1, ">");
		OLED_ShowStringL(3, 2, Menu[opt]);
		OLED_ShowStringL(4, 1, Menu[opt + 1]);
		OLED_Update();
		optRecord = opt;
		if (Encoder_Sw_Down() == 1)
		{

			OLED_Clear();
			return opt;
		}
	}
}


void sys_Init(){
	/*模块初始化*/
	OLED_Init();   // OLED初始化
	Serial_Init(); // 串口初始化
	ds1302_init(); // 1302初始化
	DHT11_Init();
	Encoder_GPIO_Init();
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	// 定义结构体变量
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // GPIO速度，赋值为50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_4; // GPIO引脚，赋值为第0号引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	Serial_TxPacket[0] = 0x03; // 暂停
	Serial_TxPacket[1] = 0x02;
	Serial_TxPacket[2] = 0x01;
	Serial_SendPacket();
	
	for(int i = 0; i > -64; i--){
		OLED_ShowImage(0,i,128,63,Xyy);
		//Delay_us(1+(int)(((i+32)>0)?(i+32):(-i-32)));
		
		OLED_ShowString(20, 88+i, "Welcome", OLED_8X16);
		OLED_Update();
	}
	for(int i = 20; i < 128; i++){
		OLED_Clear();
		OLED_ShowString(i, 25, "Welcome", OLED_8X16);
		OLED_Update();
	}
}
	

int main(void)
{

	uint8_t optRecord;
	char *MainMenu[20] = {" ",	"播放状态",		  "歌单",	  "闹钟",	 "环境", "设置", "     ",						  "     "};
	char *MusicStatu[20] = {" ", "播放", "下一首", "上一首", "音量", "所有歌曲", "循环模式", "返回", "    "};
	char *MusicList[20] = {" ", "List1", "List2", "List3", "List4", "List5", "List6", "List7", "List8", "List9", "返回", " "};
	char *PlayingMode[20] = {" ", "Earphone", "Speaker", "Back", "   ", "    "};
	char *Alarm[20] = {" ", "Alarm1", "Alarm2", "Alarm3", "Alarm4", "Alarm5", "返回", "    "};
	char *Settings[20] = {" ", "Time", "Audio Device", "返回", " ", " ", " ", "    "};
	char *Songs[30] = {" ", "9420", "Black Magic", "Bleeding Love", "Bones", "Drown", "Flower dance", "In The Shadow Of The Sun", "Like I Love You", "Komorebi",
					   "Letting Go", "Star Unkind", "Stay", "Take a While", "Take Me Back", "Take Me to Infinity", "There For You", "Tonight", "Umbrella", "Viva La Vida", "Wait Another Day", "Watch Me Work", "What Do You Mean", "返回", "    "};
	int8_t playing = 0;

	sys_Init();
	// Ds1302ReadTime(ReadTime);
	Delay_ms(200);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_SetBits(GPIOA, GPIO_Pin_7);

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

		// 主菜单
		if (MenuStatu == 0)
		{
			int opt = MenuLoop(MainMenu, 5);

			MenuStatu = opt;
			MenuIdx = 2;
			Delay_ms(50);
			OLED_Clear();
			OLED_Update();
		}

		// 音乐播放状态菜单
		if (MenuStatu == 1)
		{

			int opt = MenuLoop(MusicStatu, 7);

			// LED灯亮一下
			Delay_ms(50);
			if (opt == 1)
			{
				if (!playing)
				{

					Serial_SendByte(0x7e);
					Serial_SendByte(0x03);
					Serial_SendByte(0x01);
					Serial_SendByte(0x02);
					Serial_SendByte(0xEF);
					playing = 1;
				}
				else
				{
					Serial_SendByte(0x7e);
					Serial_SendByte(0x03);
					Serial_SendByte(0x02);
					Serial_SendByte(0x01);
					Serial_SendByte(0xEF);
					playing = 0;
				}
			}
			else if (opt == 2)
			{
				Serial_TxPacket[0] = 0x03;
				Serial_TxPacket[1] = 0x03;
				Serial_TxPacket[2] = 0x00;
				Serial_SendPacket();
			}
			else if (opt == 3)
			{
				Serial_TxPacket[0] = 0x03;
				Serial_TxPacket[1] = 0x04;
				Serial_TxPacket[2] = 0x07;
				Serial_SendPacket();
			}
			else if (opt == 4)
			{
				while (1)
				{
					OLED_Clear();
					OLED_ShowStringL(1, 1, "欢迎使用智能音频");
					OLED_ShowStringL(2,1, "Rotate Encoder");
					OLED_ShowStringL(3, 1, "音量");
					OLED_ShowStringL(4, 1, ">返回");
					OLED_Update();

					int tmp = Encoder_Get() / 4;
					if (tmp < 0)
					{
						Serial_TxPacket[0] = 0x03;
						Serial_TxPacket[1] = 0x05;
						Serial_TxPacket[2] = 0x06;
						Serial_SendPacket();
						OLED_ShowStringL(3, 9, "+");Delay_ms(100);
						OLED_Update();
						OLED_ShowStringL(3, 9, "++");Delay_ms(100);
						OLED_Update();
						OLED_ShowStringL(3, 9, "+++");Delay_ms(100);
						OLED_Update();
						OLED_Clear();
					}
					if (tmp > 0)
					{
						Serial_TxPacket[0] = 0x03;
						Serial_TxPacket[1] = 0x06;
						Serial_TxPacket[2] = 0x05;
						Serial_SendPacket();
						OLED_ShowStringL(3, 9, "-");Delay_ms(100);
						OLED_Update();Delay_ms(10);
						OLED_ShowStringL(3, 9, "--");Delay_ms(100);
						OLED_Update();Delay_ms(10);
						OLED_ShowStringL(3, 9, "---");Delay_ms(100);
						OLED_Update();Delay_ms(10);
						OLED_Clear();
					}
					Delay_ms(10);

					if (Encoder_Sw_Down() == 1)
					{
						break;
					}
				}
			}
			else if (opt == 5)
			{
				int play = MenuLoop(Songs, 23);
				while (play < 23)
				{
					Serial_SendByte(0x7e);
					Serial_SendByte(0x05);
					Serial_SendByte(0x41);
					Serial_SendByte(0x00);
					Serial_SendByte(play);
					Serial_SendByte(05 ^ (0x41) ^ 00 ^ play);
					Serial_SendByte(0xef);
					playing = 1;
					play = MenuLoop(Songs, 23);
				}
			}
			else if (opt == 6)
			{
				char *CycleMode[8] = {" ", "Ordered", "Only", "Random", "No Cycle", "Back", " "};
				int mode = MenuLoop(CycleMode, 5);
				if (mode < 5)
				{
					Serial_SendByte(0x7e);
					Serial_SendByte(0x04);
					Serial_SendByte(0x33);
					Serial_SendByte(mode);
					Serial_SendByte(04 ^ (0x33) ^ mode);
					Serial_SendByte(0xef);
				}
			}
			else if (opt == 7)
			{
				MenuStatu = 0;
			}
			Delay_ms(100);
			OLED_Clear();
		}

		// 歌单菜单
		if (MenuStatu == 2)
		{
			int lid = MenuLoop(MusicList, 10);
			OLED_Clear();
			if (lid < 10)
			{
				int opt = 1;
				while (1)
				{
					int tmp = Encoder_Get() / 4;
					if (tmp > 0)
						opt = optRecord + 1;
					if (tmp < 0)
						opt = optRecord - 1;
					if (opt != optRecord)
						OLED_Clear();
					OLED_ShowStringL(1, 1, "Edit Your Songs");
					if (opt <= 0)
						opt = 1;
					uint8_t id = lid;
					if (opt > ListNum[id] + 2)
						opt = ListNum[id] + 2;
					if (opt < ListNum[id])
					{
						OLED_ShowStringL(2, 1, Songs[List[id][opt - 1]]);
						OLED_ShowStringL(3, 1, ">");
						OLED_ShowStringL(3, 2, Songs[List[id][opt]]);
						OLED_ShowStringL(4, 1, Songs[List[id][opt + 1]]);
						OLED_Update();
					}
					else if (opt == ListNum[id])
					{
						OLED_ShowStringL(2, 1, Songs[List[id][opt - 1]]);
						OLED_ShowStringL(3, 1, ">");
						OLED_ShowStringL(3, 2, Songs[List[id][opt]]);
						OLED_ShowStringL(4, 1, "Add");
						OLED_Update();
					}
					else if (opt == ListNum[id] + 1)
					{

						OLED_ShowStringL(2, 1, Songs[List[id][opt - 1]]);
						OLED_ShowStringL(3, 1, ">");
						OLED_ShowStringL(3, 2, "Add");
						OLED_ShowStringL(4, 1, "Back");
						OLED_Update();
					}
					else if (opt == ListNum[id] + 2)
					{
						OLED_ShowStringL(2, 1, "Add");
						OLED_ShowStringL(3, 1, ">");
						OLED_ShowStringL(3, 2, "Back");
						OLED_Update();
					}
					if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
					{
						// LED灯亮一下
						Delay_ms(50);
						OLED_Clear();
						if (opt == ListNum[id] + 1)
						{
							int NSong = MenuLoop(Songs, 23);
							OLED_Clear();
							List[id][opt] = NSong;
							ListNum[id]++;
						}
						else if (opt == ListNum[id] + 2)
							break;
						else if (opt <= ListNum[id])
						{
							for (int i = opt; i < ListNum[id]; i++)
							{
								List[id][i] = List[id][i + 1];
							}
							ListNum[id]--;
						}
					}
					optRecord = opt;
				}
			}
			else if (lid == 10)
			{
				MenuStatu = 0;
			}
		}
		// 闹钟菜单
		if (MenuStatu == 3)
		{
			int aid = MenuLoop(Alarm, 6);
			// LED灯亮一下
			Delay_ms(50);
			OLED_Clear();
			if (aid < 6)
			{

				int optRecord = 1;
				int t = 1;
				int8_t timesetting[3] = {current_hour, current_minute, -1};
				while (1)
				{
					int tmp = Encoder_Get() / 4;
					if (tmp > 0)
						t = optRecord + 1;
					if (tmp < 0)
						t = optRecord - 1;

					if (t != optRecord)
						OLED_Clear();

					if (t <= 0)
						t = 1;
					if (t > 4)
						t = 4;

					OLED_ShowStringL(1, 2, "Hour:");
					OLED_ShowNumL(1, 9, timesetting[0], 2);
					OLED_ShowStringL(2, 2, "Minute:");
					OLED_ShowNumL(2, 9, timesetting[1], 2);
					OLED_ShowStringL(3, 2, "Ring:");
					if (timesetting[2] > 0)
						OLED_ShowNumL(3, 9, timesetting[2], 2);
					else
						OLED_ShowStringL(3, 9, "Null");
					OLED_ShowStringL(4, 2, "Back");

					OLED_ShowStringL(t, 1, ">");
					OLED_Update();

					if (Encoder_Sw_Down() == 1)
					{
						if (t < 3)
						{
							timesetting[t - 1]++;
							if (timesetting[0] >= 24)
								timesetting[0] -= 24;
							if (timesetting[1] >= 60)
								timesetting[1] -= 60;
						}
						else if (t == 3)
						{
							int rid = MenuLoop(Songs, 23);
							if (rid < 23)
								timesetting[2] = rid;
							else
								timesetting[2] = -1;
						}
						else if (t == 4)
						{
							ring[aid][0] = timesetting[0];
							ring[aid][1] = timesetting[1];
							ring[aid][2] = timesetting[2];
							break;
						}
					}
					optRecord = t;
				}
			}
			else if (aid == 6)
				MenuStatu = 0;
		}
		// 环境菜单
		if (MenuStatu == 4)
		{
			
			OLED_ShowNumL(1, 1, current_hour, 2);
			OLED_ShowStringL(1, 3, ":");
			OLED_ShowNumL(1, 4, current_minute, 2);
			OLED_ShowStringL(1, 6, ":");
			OLED_ShowNumL(1, 7, BCDTO10(ReadTime[0]), 2); // 秒

			OLED_ShowStringL(2, 1, "Temperature");
			OLED_ShowStringL(3, 1, "Humidity");
			OLED_ShowStringL(4, 1, ">Back");

						
			uint8_t temph, templ, humih, humil, Val;
			uint8_t result = DHT11_Read_Data(&temph, &templ, &humih, &humil, &Val);
			if (result == 0) // 如果读取成功
			{

				OLED_ShowNumL(2, 13, temph, 2); // 显示接收的数据包
				OLED_ShowNumL(3, 10, humih, 2);
			}

			OLED_Update();
			if (Encoder_Sw_Down() == 1) // 旋转编码器被按下
			{
				OLED_Clear();
				MenuStatu = 0;
			}
		}

		// 设置菜单
		if (MenuStatu == 5)
		{

			int opt = MenuLoop(Settings, 3);
			OLED_Clear();
			if (opt == 1)
			{
				int optRecord = 1;
				int t = 1;
				while (1)
				{
					int tmp = Encoder_Get() / 4;
					if (tmp > 0)
						t = optRecord + 1;
					if (tmp < 0)
						t = optRecord - 1;

					if (t != optRecord)
						OLED_Clear();

					if (t <= 0)
						t = 1;
					if (t > 4)
						t = 4;
					uint8_t timesetting[3] = {BCDTO10(TIME[0]), BCDTO10(TIME[1]), BCDTO10(TIME[2])};

					OLED_ShowStringL(1, 2, "hour:");
					OLED_ShowNumL(1, 9, timesetting[2], 2);
					OLED_ShowStringL(2, 2, "Minute:");
					OLED_ShowNumL(2, 9, timesetting[1], 2);
					OLED_ShowStringL(3, 2, "Second:");
					OLED_ShowNumL(3, 9, timesetting[0], 2);
					OLED_ShowStringL(4, 2, "Back");

					OLED_ShowStringL(t, 1, ">");
					OLED_Update();

					if (Encoder_Sw_Down() == 1)
					{
						if (t < 4)
						{
							timesetting[3 - t]++;
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
						else if (t == 4)
						{
							Ds1302Set();
							break;
						}
					}
					optRecord = t;
				}
			}
			else if (opt == 2)
			{
				while (1)
				{
					int mode = MenuLoop(PlayingMode, 3);
					Delay_ms(50);
					OLED_Clear();
					if (mode == 2)
					{
						GPIO_SetBits(GPIOA, GPIO_Pin_4);
						GPIO_SetBits(GPIOA, GPIO_Pin_7);
					}
					else if (mode == 1)
					{
						GPIO_ResetBits(GPIOA, GPIO_Pin_4);
						GPIO_ResetBits(GPIOA, GPIO_Pin_7);
					}
					else if (mode == 3)
					{
						break;
					}
				}
			}

			else if (opt == 3)
			{
				MenuStatu = 0;
			}
		}
	}
}
