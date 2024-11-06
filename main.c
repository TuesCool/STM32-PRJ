/*******************************************************************************
* �ļ���          : main.c
* ����            : Losingamong
* ʱ��			  : 08/08/2008
* �ļ�����        : ������
********************************************************************************/

/* ͷ�ļ�    ------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
									   
/* �Զ���ͬ��ؼ���    --------------------------------------------------------*/
typedef enum
{
    KeyScanState_0 = 0x00,
    KeyScanState_1 = 0x01,
    KeyScanState_2 = 0x02,
}KeyScanState_Typedef;

/* �Զ��������        --------------------------------------------------------*/
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

/* �Զ��庯����        --------------------------------------------------------*/

/* �Զ������          --------------------------------------------------------*/
vu32 flag = 0;

/* �Զ��庯������      --------------------------------------------------------*/

void RccInitialisation(void);
void GpioInitialisation(void);
void SystickInitialisation(void);

/*******************************************************************************
* ������		 : main
* ��������       : Main ����
* �������       : ��
* ������       : ��
* ����ֵ         : ��
*******************************************************************************/
int main (void)
{
    vu16 KeyPortStatus = 0;
    /* ���尴��ɨ��״̬ö�ٱ��� */
    KeyScanState_Typedef KeyScanState = KeyScanState_0;
    
    /* ����ϵͳʱ�� */
    RccInitialisation();			
    /* ����GPIO�˿� */
    GpioInitialisation();
    /* ����Systick��ʱ�� */
    SystickInitialisation();
    
    while(1)
    {	
        /* ��ѯ20ms���� */
		if (flag == 1)
        {
			flag = 0;
            /* ��ȡIO��ƽ */
            KeyPortStatus = GPIO_ReadInputData(KEYPORT) & 0x000f;
            /* ����״̬������ */
            switch(KeyScanState)
            {
                /* ״̬1���ж��з񰴼����� */	
            case KeyScanState_0:
                {
                    if(KeyPortStatus != 0x000f)
                    {
                        /* �а������£�����״̬��־ */
                    	KeyScanState = KeyScanState_1;
                    }
                    break;
                }
                /* ״̬2���ж��Ƿ񶶶� */
            case KeyScanState_1:
                {
                    if(KeyPortStatus)
                    {
                        /* �Ƕ�����ȷ�ϰ������£�ִ����Ӧ���� */
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
                        /* ����״̬��־ */
                        KeyScanState = KeyScanState_2;
                    }
                    else
                    {
                        /* ������ȷ�ϰ���δ���£�����״̬��־*/
                        KeyScanState = KeyScanState_0;
                    }
                    break;
                }
                /* ״̬3�����ּ�� */
            case KeyScanState_2:
                {
                    if(KeyPortStatus == 0x000f)
                    {
			/* ���֣�����״̬��־ */
			KeyScanState = KeyScanState_0;
                    }
                    break;
                }
            }
        }
    }
}

/*******************************************************************************
* ������	: RCC_Configuration
* ��������  : ����ϵͳ������ʱ��
* �������  : ��
* ������  : ��
* ����ֵ    : ��
*******************************************************************************/
void RccInitialisation(void)
{
    /* ��APB2�����ϵ�GPIOAʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}



/*******************************************************************************
* ������  		: GPIO_Configuration
* ��������    	: ���ø�GPIO�˿ڹ���
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GpioInitialisation(void)
{
    /* ����GPIO��ʼ���ṹ�� GPIO_InitStructure*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* ���� GPIOA0 , GPIOA1 Ϊ�������� */
    GPIO_InitStructure.GPIO_Pin = KEY0PIN | KEY1PIN | KEY2PIN | KEY3PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEYPORT, &GPIO_InitStructure);
    
    /* ���� GPIOA2 ,GPIOA3 Ϊ������������תƵ��Ϊ50MHz*/
    GPIO_InitStructure.GPIO_Pin = LED0PIN | LED1PIN | LED2PIN | LED3PIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(LEDPORT, &GPIO_InitStructure); 
}


/**************************************************************************
* ������  	: SystickInitialisation
* ��������    	: ����Systick��ʱ��,��װ��ʱ��Ϊ20ms
* �������      : ��
* ������      : ��
* ����ֵ        : ��
**************************************************************************/
void SystickInitialisation(void)
{
  	/* ��ƵΪ72MHz�����ü���ֵ72000000����50���Եõ�20ms��ʱ��� */
	SysTick_Config(SystemCoreClock / 50);		
}

























