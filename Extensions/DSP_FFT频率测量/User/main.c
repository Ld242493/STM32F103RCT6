/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  * @warning  该工程中的core_cm3.h文件与其他工程中的core_cm3.h文件不同，移植DSP
  *           必须使用该工程的core_cm3.h文件编译才不会报错，此外在宏定义中添加
  *           ARM_MATH_CM3
  * @attention  移植DSP时可以直接在MDK中添加DSP库，该工程目录中的DSP文件内容可以
  *             不用添加到工程中，放在这里方便查看
  *    
  ******************************************************************************
  */
 
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "ADC/adc.h"
#include "PWM/pwm.h"
#include "arm_math.h"
#include "arm_const_structs.h"

/*  正弦波直接给定采样率，方波信号由定时器计算  */
#define SampleFreq  10000

uint8_t text[20];

float max = 0;
uint8_t maxpos;
float data_in[SampleNber * 2] = {0};    //输入信号
float data_out[SampleNber / 2];         //输出信号
float data[SampleNber] =                //正弦序列
{
   747,  443,  630,  679,  741,  735,  607,  476,  298,   95,  -63, -246, -352, -472, -526, -548, 
  -612, -601, -584, -563, -471, -383, -225, -127,   63,  266,  429,  591,  710,  740,  717,  622,
   457,  303,  112,  -76, -227, -345, -435, -528, -585, -607, -609, -563, -530, -439, -386, -271,
   -94,   38,  293,  437,  594,  688,  755,  722,  623,  516,  290,  106, -62 , -226, -349, -449,
  -503, -573, -596, -615, -596, -540, -496, -407, -281, -99 ,   53,  226,  408,  585,  725,  723,
   719,  645,  486,  319,  123,  -69, -218, -346, -452, -508, -563, -585, -613, -572, -549, -481,
  -359, -279, -127,   67,  227,  437,  577,  681,  758,  742,  627,  494,  322,  146, -51 , -178,
  -350, -449, -499, -608, -596, -608, -600, -538, -479, -388, -267, -130,   41,  248,  415,  588,
   688,  734,  718,  636,  500,  320,  135,  -23, -210, -343, -434, -511, -566, -590, -604, -581, 
  -544, -499, -395, -281, -136,   38,  226,  407,  568,  683,  769,  722,  637,  509,  336,  141,
   -38, -217, -335, -436, -546, -563, -594, -620, -583, -544, -488, -395, -288, -144,   42,  215,
   404,  561,  684,  738,  722,  626,  498,  331,  140, -43,-  199, -331, -440, -515, -561, -574,
  -604, -584, -516, -490, -403, -290, -133,   43,  211,  398,  557,  715,  733,  718,  629,  507,
   336,  176,  -36, -194, -329, -435, -510, -554, -550, -606, -584, -540, -492, -408, -291, -144,
    18,  208,  401,  551,  672,  757,  720,  643,  547,  342,  154,  -24, -181, -322, -428, -485,
  -559, -593, -599, -588, -548, -489, -411, -296, -152,   15,  195,  383,  551,  667,  708,  724
};

int main(void)
{
  Systick_Init();
  LED_Init();
  KEY_Init();
  TIM6_Init();

  USART_Configuration();
  PWM_Output_Init();
  ADC_DMA_Init();
  OLED_Init();

  OLED_ShowString(10, 0, "FFT demo", OLED_8X16);

  for (uint8_t i = 0; i < 2; i++)
  {
    /*  等待DMA采集完一次数据  */
    while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);

    for(uint16_t j = 0; j < SampleNber; j++)  //纯实数信号输入
    {
      data_in[j*2]=(float)adcBuffer[j];  //设置adc采样方波信号输入
      // data_in[j*2] = data[j];   //设置自定义正弦波信号输入
      data_in[j*2+1] = 0;
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len256, data_in, 0, 1);
    arm_cmplx_mag_f32(data_in, data_out, SampleNber/2);    
  }
  
  for(uint8_t i = 0; i < 128; i++)
  {
    data_out[i] /= 128;   //归一化处理
    if(data_out[i] > max && i != 0) //找出幅值与幅值所在点位
    {
      max = data_out[i];
      maxpos = i - 1; //忽略第一个点的值
    }
    // printf("%f, %f\r\n",data[i], data_out[i]);
    printf("%d , %f\r\n",adcBuffer[i], data_out[i]);
  }
  sprintf(text, "Pos: %d", maxpos);
  OLED_ShowString(10, 20, text, OLED_8X16);
  /*  正弦波输入信号频率计算，采样率/采样点位  */
  // sprintf(text, "Freq: %dHz", SampleFreq/maxpos);

  /*  方波输入信号频率计算，频率分辨率*幅值点位，频率分辨率 = 采样率/样本数量
      其中第一点为直流分量，忽略不计  */
  sprintf(text, "Freq: %d Hz", SampleFreq*maxpos/SampleNber);
  OLED_ShowString(10, 40, text, OLED_8X16);
  OLED_Update();

  while(1)
  {
    delay_ms(100);
  }  
}