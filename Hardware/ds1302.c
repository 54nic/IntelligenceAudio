#include "stm32f10x.h" // Device header

#include "ds1302.h"
#include <math.h>
#include "delay.h"

uint8_t read[] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};  // 读秒、分、时、日、月、周、年的寄存器地址
uint8_t write[] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c}; // 写秒、分、时、日、月、周、年的寄存器地址

uint8_t TIME[7] = {0x40, 0x59, 0x23, 0x24, 0x09, 0x02, 0x24}; // 64 89 35 36 9 2 35

// DS1302时钟端口定义
void ds1302_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			  // 开启GPIO时钟
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_0; // 设置引脚号
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;					  // 设置GPIO的模式

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 设置IO口的速度
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void DS1302_DATAOUT_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;		   // 设置引脚号
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置GPIO的模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 设置IO口的速度
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

void DS1302_DATAINPUT_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;	   // 设置引脚号
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // 设置GPIO的模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/**********************************DS1302时钟子函数如下*****************************************/

// 写一个字节的数据sck上升沿写数据
void write_1302byte(uint8_t data)
{
	uint8_t count = 0;
	DS1302_DATAOUT_Config();

	DS_CLK = 0;
	for (count = 0; count < 8; count++)
	{
		DS_CLK = 0;
		if (data & 0x01)
		{
			DS_DIO = 1;
		}
		else
		{
			DS_DIO = 0;
		}
		DS_CLK = 1;
		data >>= 1;
	}
}

// 向DS1302指定寄存器写入一个字节的数据
void write_1302(uint8_t address, uint8_t data)
{
#if 1
	uint8_t temp1 = address;
	uint8_t temp2 = data;
	DS_RST = 0;
	DS_CLK = 0;
	Delay_us(5);
	DS_RST = 1;
	Delay_us(5);
	write_1302byte(temp1);
	write_1302byte(temp2);
	DS_RST = 0;
	DS_CLK = 0;
	Delay_us(2);
#endif
}

// 从DS1302指定寄存器读数据
uint8_t read_1302(uint8_t address)
{
	uint8_t temp3 = address;
	uint8_t count = 0;
	uint8_t return_data = 0x00;

	DS_RST = 0;
	DS_CLK = 0;
	Delay_us(5);
	DS_RST = 1;
	Delay_us(5);
	write_1302byte(temp3);
	DS1302_DATAINPUT_Config(); // 配置I/O口为输入
	Delay_us(5);
	for (count = 0; count < 8; count++)
	{
		Delay_us(2); // 使电平持续一段时间
		return_data >>= 1;
		DS_CLK = 1;
		Delay_us(5); // 使高电平持续一段时间
		DS_CLK = 0;
		Delay_us(5); // 延时14us后再去读取电压，更加准确

		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))
		{
			return_data = return_data | 0x80;
		}
	}
	Delay_us(2);
	DS_RST = 0;
	DS_DIO = 0;
	return return_data;
}

// 初始化1302
void Ds1302Set(void)
{
	uint8_t i = 0;
	write_1302(0x8e, 0x00); // 关闭写保护
	for (i = 0; i < 7; i++) // 进行对时
	{
		write_1302(write[i], TIME[i]); // 在对应寄存器上写入对应的十六进制数据
	}
	write_1302(0x8e, 0x80); // 打开写保护
}

void Ds1302ReadTime(uint8_t *Read_Time)
{
	uint8_t n;
	for (n = 0; n < 7; n++) // 读取7个字节的时钟信号：秒时分日月周年
	{
		Read_Time[n] = read_1302(read[n]);
	}
}

uint8_t Z10BCD(uint8_t date) // 十进制转BCD码
{
	uint8_t i, j;
	i = date / 10;
	j = date - (i * 10);
	i = (i << 4) | j;
	return (i);
}
uint8_t BCDTO10(uint8_t date) // BCD码转10进制
{
	uint8_t i, j;
	i = (date & 0xf0) >> 4;
	j = date & 0x0f;
	i = i * 10 + j;
	return i;
}

/*-----------------------------------------------------------------------
Judge_Week          			  	  : 判断星期

输入参数                   			：年月日
返回值                         	: 返回星期 （0：星期日 1：星期一 1~ 6：星期六）---*/
uint8_t Judge_Week(uint16_t y, uint8_t m, uint8_t d)
{
	int c, n;
	int w;
	c = y / 100;
	n = y % 100;
	w = n + floor(n * 1.0 / 4) + floor(c * 1.0 / 4) - 2 * c + floor(26 * (m + 1) * 1.0 / 10) + d - 1;

	if (w % 7 == 0)
		return 7;
	else
		return (w % 7);
}

/*******************************************************************************
 * 函 数 名         : Ds1302Write
 * 函数功能		   : 向DS1302命令（地址+数据）
 * 输    入         : addr,dat
 * 输    出         : 无
 *******************************************************************************/
#if 0
void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;                                                         // 先将SCLK置低电平。
	_nop_();
	RST = 1;                                                          // 然后将RST(CE)置高电平。
	_nop_();

	for (n=0; n<8; n++)                                               // 开始传送八位地址命令
	{
		DSIO = addr & 0x01;                                           // 数据从低位开始传送
		addr >>= 1;
		SCLK = 1;                                                     // 数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)                                               // 写入8位数据
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;                                                     // 数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;                                                          // 传送数据结束
	_nop_();
}

/*******************************************************************************
* 函 数 名         : Ds1302Read
* 函数功能		   : 读取一个地址的数据
* 输    入         : addr
* 输    出         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;                                                         // 先将SCLK置低电平。
	_nop_();
	RST = 1;                                                          // 然后将RST(CE)置高电平。
	_nop_();

	for(n=0; n<8; n++)                                                // 开始传送八位地址命令
	{
		DSIO = addr & 0x01;                                           // 数据从低位开始传送
		addr >>= 1;
		SCLK = 1;                                                     // 数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;                                                     // DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)                                                // 读取8位数据
	{
		dat1 = DSIO;                                                  // 从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;                                                     // DS1302下降沿时，放置数据
		_nop_();
	}

	RST = 0;
	_nop_();	                                                      // 以下为DS1302复位的稳定时间,必须的。
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* 函 数 名         : Ds1302Init
* 函数功能		   : 初始化DS1302.
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

//void Ds1302Init()
//{
//	uchar n;
//	Ds1302Write(0x8E,0X00);		                                      // 禁止写保护，就是关闭写保护功能
//	for (n=0; n<7; n++)                                               // 写入7个字节的时钟信号：分秒时日月周年
//	{
//		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
//	}
//	Ds1302Write(0x8E,0x80);		                                      // 打开写保护功能
//}

/*******************************************************************************
* 函 数 名         : Ds1302ReadTime
* 函数功能		   : 读取时钟信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)                                               // 读取7个字节的时钟信号：秒时分日月周年
	{
		Read_Time[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}


uchar Z10BCD( uchar date)  //十进制转BCD码
{
   uchar i,j;
   i=date/10;
   j=date-(i*10);
    i=(i<<4)|j;  
   return (i);
}
uchar BCDTO10( uchar date )   //BCD码转10进制
{
 uchar i,j;
 i=(date & 0xf0)>>4 ;
 j=date & 0x0f ;
 i=i*10+j;
 return i;
}
#endif
