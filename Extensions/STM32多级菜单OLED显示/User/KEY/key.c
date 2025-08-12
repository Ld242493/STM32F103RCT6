#include "key.h"

key_type keyVal = Key_None;
uint8_t keyTime, keyMode = 1;   //分别用来按键延时消抖与区分按键按下和松开
uint8_t key1Flag, keyWakeUpFlag, key2Flag;  //用来区别哪个按键松开

void KEY_TIM6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = Key2_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(Key2_GPIO_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = WakeUp_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(WakeUp_GPIO_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = Key1_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(Key1_GPIO_Port, &GPIO_InitStructure);     

  NVIC_InitTypeDef NVIC_InitStructure;  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM6, 71, TIM_PSCReloadMode_Immediate);  
  
  /* TIM IT enable */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM6, ENABLE);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void KeyScan(void)
{
  if(keyMode)
	{
		if(!ReadKey1)  //检测按键按下
		{
			if(keyTime == 0)
			{
				keyMode = 0;
        key1Flag = 1;
				keyTime = KeyTimeVal;
				keyVal = Key_Up;
			}
		}   
		else if(ReadKeyWakeUp)  //检测按键按下
		{
			if(keyTime == 0)
			{
				keyMode = 0;
        keyWakeUpFlag = 1;
				keyTime = KeyTimeVal;
				keyVal = Key_Enter;
			}
		}
		else if(!ReadKey2)  //检测按键按下
		{
			if(keyTime == 0)
			{
				keyMode = 0;
        key2Flag = 1;
				keyTime = KeyTimeVal;
				keyVal = Key_Down;
			}
		}        
		else  keyTime = KeyTimeVal;
	}
	else
	{ /*  检测按键松手  */
		if((ReadKey1 && key1Flag) || (!ReadKeyWakeUp && keyWakeUpFlag) || (ReadKey2 && key2Flag))
		{
			if(keyTime == 0)
			{
				keyMode = 1;
        key1Flag = 0;
        keyWakeUpFlag = 0;
        key2Flag = 0;
				keyTime = KeyTimeVal;
			}
		}
		else  keyTime = KeyTimeVal;
	}  
}

extern uint16_t FPS, FPS_Count;

void TIM6_IRQHandler(void)
{
  static uint16_t Cnt1, Cnt2;
  static uint8_t flag = 0;
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    /*  pwm led breathe */
    if(flag)  TIM_SetCompare1(TIM1, Cnt2);
    else      TIM_SetCompare1(TIM1, 1000 - Cnt2);
    
    if(keyTime)   keyTime--;
    
    if(++Cnt1 == 20)
    {
      Cnt1 = 0;
      KeyScan();
    }
    if(++Cnt2 == 1000)
    {
      Cnt2 = 0;
      flag = !flag;
      FPS = FPS_Count;
      FPS_Count = 0;
    }
  }
}
