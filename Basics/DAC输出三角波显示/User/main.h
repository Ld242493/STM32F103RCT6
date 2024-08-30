#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "Systick.h"
#include "usart.h"

typedef struct Curve{
  uint8_t yoffset;  //时间轴偏移量,也是数据最低点位置，即最小值，对应LCD最大y坐标值
  uint8_t ymax;     //最大值，即数据峰值，对应LCD最小y坐标值
  uint8_t xStart;   //折线图起始位置
  uint8_t xEnd;     //折线图终点位置
  uint16_t color;   //折线颜色
}CurvePara;

#endif 
