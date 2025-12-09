/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-29     LCKFB-LP    first version
 */
#ifndef _BSP_ENCODER_H_
#define _BSP_ENCODER_H_

#include "stm32f10x.h"

#define RCC_GPIO RCC_APB2Periph_GPIOA
#define PORT_GPIO GPIOA

// SW引脚 对应key
#define GPIO_ENCODER_SW GPIO_Pin_10

// CLK引脚 对应S2
#define GPIO_ENCODER_LCK GPIO_Pin_9

// DT引脚 对应S1
#define GPIO_ENCODER_DT GPIO_Pin_8

// 获取CLK引脚的状态
#define GET_CLK_STATE GPIO_ReadInputDataBit(PORT_GPIO, GPIO_ENCODER_LCK)
// 获取DT引脚的状态
#define GET_DT_STATE GPIO_ReadInputDataBit(PORT_GPIO, GPIO_ENCODER_DT)
// 获取SW引脚的状态
#define GET_SW_STATE GPIO_ReadInputDataBit(PORT_GPIO, GPIO_ENCODER_SW)

// 定时器扫描
#define BSP_TIMER_RCC RCC_APB1Periph_TIM3    // 定时器时钟
#define BSP_TIMER TIM3                       // 定时器
#define BSP_TIMER_IRQ TIM3_IRQn              // 定时器中断
#define BSP_TIMER_IRQHANDLER TIM3_IRQHandler // 定时器中断服务函数

void Encoder_GPIO_Init(void);        // 旋转编码器初始化
unsigned char Encoder_Sw_Down(void); // 编码器是否按下
char Encoder_Scanf(void);
int Encoder_Rotation_left(void);  // 左转服务函数
int Encoder_Rotation_right(void); // 右转服务函数
int16_t Encoder_Get(void);

#endif