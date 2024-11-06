/*******************************************************************************
* 文件名          : main.c
* 作者            : Losingamong
* 时间			  : 08/08/2008
* 文件描述        : 主函数
********************************************************************************/

/* 头文件    ------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
									   
/* 自定义同义关键字    --------------------------------------------------------*/
typedef enum
{
    KeyScanState_0 = 0x00,
    KeyScanState_1 = 0x01,
    KeyScanState_2 = 0x02,
}KeyScanState_Typedef;

/* 自定义参数宏        --------------------------------------------------------*/
#define KEYPORT		GPIOA
#define KEY0PIN		GPIO_Pin_0
#define KEY1PIN		GPIO_Pin_1
#define KEY2PIN		GPIO_Pin_2
#define KEY3PIN		GPIO_Pin_3

#define LEDPORT		GPIOA
#define LED0PIN		GPIO_Pin_4
#define LED1PIN		GPIO_Pin_5
#define LED2PIN		GPIO_Pin_6
#define LED3PIN		GPIO_Pin_7

/* 自定义函数宏        --------------------------------------------------------*/

/* 自定义变量          --------------------------------------------------------*/
vu32 flag = 0;

/* 自定义函数声明      --------------------------------------------------------*/

void RccInitialisation(void);
void GpioInitialisation(void);
void SystickInitialisation(void);

/*******************************************************************************
* 函数名		 : main
* 函数描述       : Main 函数
* 输入参数       : 无
* 输出结果       : 无
* 返回值         : 无
*******************************************************************************/
int main (void)
{
    vu16 KeyPortStatus = 0;
    /* 定义按键扫描状态枚举变量 */
    KeyScanState_Typedef KeyScanState = KeyScanState_0;
    
    /* 设置系统时钟 */
    RccInitialisation();			
    /* 设置GPIO端口 */
    GpioInitialisation();
    /* 设置Systick定时器 */
    SystickInitialisation();
    
    while(1)
    {	
        /* 查询20ms到？ */
		if (flag == 1)
        {
			flag = 0;
            /* 读取IO电平 */
            KeyPortStatus = GPIO_ReadInputData(KEYPORT) & 0x000f;
            /* 进入状态机流程 */
            switch(KeyScanState)
            {
                /* 状态1：判断有否按键按下 */	
            case KeyScanState_0:
                {
                    if(KeyPortStatus != 0x000f)
                    {
                        /* 有按键按下，更新状态标志 */
                    	KeyScanState = KeyScanState_1;
                    }
                    break;
                }
                /* 状态2：判断是否抖动 */
            case KeyScanState_1:
                {
                    if(KeyPortStatus)
                    {
                        /* 非抖动，确认按键按下，执行相应操作 */
                        if(GPIO_ReadInputDataBit(KEYPORT, KEY0PIN) == 0)
                        {
                            GPIO_WriteBit(LEDPORT, 
                                          LED0PIN, 
                                          (BitAction)(1 - GPIO_ReadOutputDataBit(LEDPORT, LED0PIN)));
                        }
                        else if(GPIO_ReadInputDataBit(KEYPORT, KEY1PIN) == 0)
                        {
                            GPIO_WriteBit(LEDPORT, 
                                          LED1PIN, 						  
                                          (BitAction)(1 - GPIO_ReadOutputDataBit(LEDPORT, LED1PIN)));
                        }
                        else if(GPIO_ReadInputDataBit(KEYPORT, KEY2PIN) == 0)
                        {
                            GPIO_WriteBit(LEDPORT, 
                                          LED2PIN, 
                                          (BitAction)(1 - GPIO_ReadOutputDataBit(LEDPORT, LED2PIN)));
                        }
                        else if(GPIO_ReadInputDataBit(KEYPORT, KEY3PIN) == 0)
                        {
                            GPIO_WriteBit(LEDPORT, 
                                          LED3PIN, 
                                          (BitAction)(1 - GPIO_ReadOutputDataBit(LEDPORT, LED3PIN)));
                        }
                        /* 更新状态标志 */
                        KeyScanState = KeyScanState_2;
                    }
                    else
                    {
                        /* 抖动，确认按键未按下，更新状态标志*/
                        KeyScanState = KeyScanState_0;
                    }
                    break;
                }
                /* 状态3：松手检测 */
            case KeyScanState_2:
                {
                    if(KeyPortStatus == 0x000f)
                    {
			/* 松手，更新状态标志 */
			KeyScanState = KeyScanState_0;
                    }
                    break;
                }
            }
        }
    }
}

/*******************************************************************************
* 函数名	: RCC_Configuration
* 函数描述  : 设置系统各部分时钟
* 输入参数  : 无
* 输出结果  : 无
* 返回值    : 无
*******************************************************************************/
void RccInitialisation(void)
{
    /* 打开APB2总线上的GPIOA时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}



/*******************************************************************************
* 函数名  		: GPIO_Configuration
* 函数描述    	: 设置各GPIO端口功能
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void GpioInitialisation(void)
{
    /* 定义GPIO初始化结构体 GPIO_InitStructure*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 设置 GPIOA0 , GPIOA1 为上拉输入 */
    GPIO_InitStructure.GPIO_Pin = KEY0PIN | KEY1PIN | KEY2PIN | KEY3PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEYPORT, &GPIO_InitStructure);
    
    /* 设置 GPIOA2 ,GPIOA3 为推挽输出，最大翻转频率为50MHz*/
    GPIO_InitStructure.GPIO_Pin = LED0PIN | LED1PIN | LED2PIN | LED3PIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(LEDPORT, &GPIO_InitStructure); 
}


/**************************************************************************
* 函数名  	: SystickInitialisation
* 函数描述    	: 设置Systick定时器,重装载时间为20ms
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
**************************************************************************/
void SystickInitialisation(void)
{
  	/* 主频为72MHz，配置计数值72000000除以50可以得到20ms定时间隔 */
	SysTick_Config(SystemCoreClock / 50);		
}

























