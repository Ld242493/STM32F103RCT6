#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "Systick.h"
#include "usart.h"

void StartTask(void * pvParameters);          //��������
void Task1(void * pvParameters);              //��������
void Task2(void * pvParameters);              //��������

#endif 
