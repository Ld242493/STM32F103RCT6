#include "hx711.h"


/*  校准参数，当测试重量偏大时，增加该数值，重量偏小就减小该值  */
#define GapValue 207.00f

uint32_t  WeightMaoPi = 0;  //第一次获取的毛皮质量，后面测试的物品质量都以这个为基准
uint32_t WeightTotal  = 0;  //总质量，毛皮重量加物品实际质量
int32_t  WeightQuPi  = 0;   //物品实际质量，物品去皮后的质量

void HX711_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOB Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = HX711_DT_PIN | HX711_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(HX711_PORT, &GPIO_InitStructure);  
  
  HX711_GetMaoPi();
}

static void DT_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = HX711_DT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  

  GPIO_Init(HX711_PORT, &GPIO_InitStructure);
}


static void DT_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = HX711_DT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  

  GPIO_Init(HX711_PORT, &GPIO_InitStructure);
}

static uint32_t HX711_ReadData(void)
{
  uint32_t count;

  DT_OUT();
  delay_us(5);

  HX711_DT_OUT(1);
  delay_us(1);
  HX711_SCK_OUT(0);
  count = 0;

  DT_IN();
  delay_us(5);

  // while(HX711_DT_IN())

  for (uint8_t i = 0; i < 24; i++)
  {
    HX711_SCK_OUT(1);
    count <<= 1;
    delay_us(1);
    HX711_SCK_OUT(0);
    if(HX711_DT_IN())   count++;
    delay_us(1);
  }
  HX711_SCK_OUT(1);
  count = count ^ 0x800000;
  delay_us(1);
  HX711_SCK_OUT(0);

  return count;
}

void HX711_GetMaoPi(void)
{
  WeightMaoPi = HX711_ReadData();
}

float HX711_GetWeight(void)
{
  float weight = 0;

  WeightTotal = HX711_ReadData();
  if (WeightTotal > WeightMaoPi)
  {
    WeightQuPi = WeightTotal - WeightMaoPi; //计算物品去皮后质量
    weight = (float)WeightQuPi / GapValue;  //转换为物品实际重量
  }
  // if(weight < 0)  weight = 0;
  return weight;
}