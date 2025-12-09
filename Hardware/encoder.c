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

#include "encoder.h"
#include "Delay.h"
#include "stdio.h"
#include "OLED.h"

uint8_t MenuIdx = 2, MenuStatu = 0; // 0:主菜单 1:Music Statu 2:.Music List 3:Playing Mode 4:Alarm 5:Environment 6:List 7:Alarm
/******************************************************************
 * 函 数 名 称：Encoder_GPIO_Init
 * 函 数 说 明：旋转编码器引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：使用定时器每10Ms扫描一次是否有旋转，即通过定时器进行消抖
 ******************************************************************/
void Encoder_GPIO_Init(void)
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1; // ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;  // PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM1, ENABLE);

    TIM_SetCounter(TIM1, 2);
}

int16_t Encoder_Get(void)
{
    int16_t temp = TIM_GetCounter(TIM1);
    TIM_SetCounter(TIM1, 0);
    return temp;
}

/******************************************************************
 * 函 数 名 称：Encoder_Scanf
 * 函 数 说 明：判断旋转编码器是否有往哪一个方向旋转
 * 函 数 形 参：无
 * 函 数 返 回：1=正转 2=反转
 * 作       者：LC
 * 备       注：哪一边正转哪一边反转不需要太在意，你说的算
 ******************************************************************/
char Encoder_Scanf(void)
{
    static FlagStatus EC11_CLK_Last = RESET; // EC11的LCK引脚上一次的状态 （A相）
    static FlagStatus EC11_DT_Last = RESET;  // EC11的DT引脚上一次的状态（B相）
    char ScanResult = 0;
    // 当A发生跳变时采集B当前的状态，并将B与上一次的状态进行对比。
    if (GET_CLK_STATE != EC11_CLK_Last)
    {                           // 若A 0->1 时，B 1->0 正转；若A 1->0 时，B 0->1 正转；
                                // 若A 0->1 时，B 0->1 反转；若A 1->0 时，B 1->0 反转
        if (GET_CLK_STATE == 1) // EC11_A和上一次状态相比，为上升沿
        {
            // EC11_B和上一次状态相比，为下降沿
            if ((EC11_DT_Last == 1) && (GET_DT_STATE == 0))
                ScanResult = 1; // 正转
            // EC11_B和上一次状态相比，为上升沿
            if ((EC11_DT_Last == 0) && (GET_DT_STATE == 1))
                ScanResult = 2; // 反转

            //>>>>>>>>>>>>>>>>下面为正转一次再反转或反转一次再正转处理<<<<<<<<<<<<<<<<//
            // A上升沿时，采集的B不变且为0
            if ((EC11_DT_Last == GET_DT_STATE) && (GET_DT_STATE == 0))
                ScanResult = 1; // 正转
            // A上升沿时，采集的B不变且为1
            if ((EC11_DT_Last == GET_DT_STATE) && (GET_DT_STATE == 1))
                ScanResult = 2; // 反转
        }
        else // EC11_A和上一次状态相比，为下降沿
        {
            // EC11_B和上一次状态相比，为下降沿
            if ((EC11_DT_Last == 1) && (GET_DT_STATE == 0))
                ScanResult = 2; // 反转
            // EC11_B和上一次状态相比，为上升沿
            if ((EC11_DT_Last == 0) && (GET_DT_STATE == 1))
                ScanResult = 1; // 正转

            //>>>>>>>>>>>>>>>>下面为正转一次再反转或反转一次再正转处理<<<<<<<<<<<<<<<<//
            // A上升沿时，采集的B不变且为0
            if ((EC11_DT_Last == GET_DT_STATE) && (GET_DT_STATE == 0))
                ScanResult = 2; // 反转
            // A上升沿时，采集的B不变且为1
            if ((EC11_DT_Last == GET_DT_STATE) && (GET_DT_STATE == 1))
                ScanResult = 1; // 正转
        }
        EC11_CLK_Last = GET_CLK_STATE; // 更新编码器上一个状态暂存变量
        EC11_DT_Last = GET_DT_STATE;   // 更新编码器上一个状态暂存变量
        return ScanResult;             // 返回值的取值：   0：无动作； 1：正转；  2：反转；
    }
    return 0;
}

/******************************************************************
 * 函 数 名 称：Encoder_Sw_Down
 * 函 数 说 明：判断编码器是否被按下
 * 函 数 形 参：无
 * 函 数 返 回：0=没有被按下  1=被按下
 * 作       者：LC
 * 备       注：请注意消抖
 ******************************************************************/
unsigned char Encoder_Sw_Down(void)
{
    // 没有按下
    if (GET_SW_STATE == Bit_SET)
    {
        Delay_ms(100); // 消抖
        return 0;
    }
    else // 按下
    {
        Delay_ms(100); // 消抖
        //        printf("down\r\n");
        return 1;
    }
}

/******************************************************************
 * 函 数 名 称：Encoder_Rotation_left
 * 函 数 说 明：左旋转服务函数。当编码器左转时，需要执行的操作
 * 函 数 形 参：无
 * 函 数 返 回：向左旋转次数
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
int Encoder_Rotation_left(void)
{
    static int left_num = 0; // 左转次数
    left_num++;
    /*  你的代码写在此处  */
    GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
    /*  你的代码写在此处  */
    return left_num;
}

/******************************************************************
 * 函 数 名 称：Encoder_Rotation_left
 * 函 数 说 明：右旋转服务函数。当编码器右转时，需要执行的操作
 * 函 数 形 参：无
 * 函 数 返 回：向右旋转次数
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
int Encoder_Rotation_right(void)
{
    static int right_num = 0; // 右转次数
    right_num++;
    /*  你的代码写在此处  */
    GPIO_SetBits(GPIOA, GPIO_Pin_7); // LED灯亮一下
    /*  你的代码写在此处  */

    return right_num;
}

/************************************************
函数名称 ： BSP_TIMER_IRQHandler
功    能 ： 基本定时器中断服务函数
参    数 ： 无
返 回 值 ： 无
作    者 ： LC
*************************************************/
void BSP_TIMER_IRQHANDLER(void)
{
    static char dat = 0;
    if (TIM_GetITStatus(BSP_TIMER, TIM_IT_Update) == SET)
    {
        dat = Encoder_Scanf(); // 扫描编码器是否扭动
        if (dat != 0)          // 如果有转动
        {
            if (dat == 2)
            {
                Encoder_Rotation_left();
            }
            if (dat == 1)
            {
                Encoder_Rotation_right();
            }
        }
    }
    TIM_ClearITPendingBit(BSP_TIMER, TIM_IT_Update);
}