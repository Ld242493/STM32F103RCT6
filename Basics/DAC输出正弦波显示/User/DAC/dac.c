#include "dac.h"

/*  正弦波序列，12bit，1个周期128个点, 0-4095之间变化  */
const uint32_t SineWave[128] = 
{
  2047,   2147,	2248, 2347,	2446,	2544,	2641,	2737,
  2830,   2922,	3012,	3099,	3184,	3266,	3346,	3422,
  3494,   3564,	3629,	3691,	3749,	3803,	3852,	3897,
  3938,   3974,	4006,	4033,	4055,	4072,	4084,	4092,
  4094,   4092,	4084,	4072,	4055,	4033,	4006,	3974,
  3938,   3897,	3852,	3803,	3749,	3691,	3629,	3564,
  3494,   3422,	3346,	3266,	3184,	3099,	3012,	2922,
  2830,   2737,	2641,	2544,	2446,	2347,	2248,	2147,
  2047,   1947,	1846,	1747,	1648,	1550,	1453,	1357,
  1264,   1172,	1082,	995 ,	910 ,	828 ,	748 ,	672 ,
  600 ,   530 ,	465 ,	403 ,	345 ,	291 ,	242 ,	197 ,
  156 ,   120 ,	88  ,	61  ,	39  ,	22  ,	10  ,	2   ,
  0   ,   2   ,	10  ,	22  ,	39  ,	61  ,	88  ,	120 ,
  156 ,   197 ,	242 ,	291 ,	345 ,	403 , 465,	530 ,
  600 ,   672 ,	748 ,	828 ,	910 ,	995 ,	1082,	1172,
  1264,   1357,	1453, 1550,	1648,	1747,	1846,	1947
};

void DAC_Configuration(void)
{
  DAC_InitTypeDef   DAC_InitStructure;
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);  
  /* TIM7 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  /* TIM7 Configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  /*  建议触发频率设低些，太快的话波形显示有问题,当前频率为200hz  */
  TIM_TimeBaseStructure.TIM_Period = 4999;             
  TIM_TimeBaseStructure.TIM_Prescaler = 71;        
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  /* TIM7 TRGO selection */
  TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);  

  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  DMA_InitTypeDef   DMA_InitStructure;
  /* DMA2 channel3 configuration */
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&DAC->DHR12R1;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&SineWave;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 128;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA2_Channel3, &DMA_InitStructure);

  /* Enable DMA2 Channel4 */
  DMA_Cmd(DMA2_Channel3, ENABLE);

  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
  /* TIM7 enable counter */
  TIM_Cmd(TIM7, ENABLE);   
}


