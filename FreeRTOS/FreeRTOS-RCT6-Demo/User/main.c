/**
  ******************************************************************************
  * @file    main.c 
  * @brief   FreeRTOS Project Template
  *
  *
  *    
  ******************************************************************************
  */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"

TaskHandle_t StartTask_t, Task1_t, Task2_t;   // 任务句柄
char TaskInfoBuf[50];      //任务信息缓存变量

int main(void)
{  
  Systick_Init();
  USART1_Config();
  OLED_Init();
  OLED_ShowString(10, 0, "FreeRTOS Demo", OLED_8X16);
  OLED_ShowChinese(0, 20, "你好，世界。");
  OLED_Update();
  //可配置的中断优先级范围(0~15, MAX = 0, MIN = 15)
  //FreeRTOS可管理的中断范围(5~15)
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  xTaskCreate((TaskFunction_t )StartTask,       /* 任务函数 */
              (const char*    )"StartTask",     /* 任务名称 */
              (uint16_t       )128,             /* 任务堆栈大小 */
              (void*          )NULL,            /* 传入给任务函数的参数 */
              (UBaseType_t    )1,               /* 任务优先级 */
              (TaskHandle_t*  )&StartTask_t);   /* 任务句柄 */  
  vTaskStartScheduler();
}

void StartTask(void * pvParameters)
{
  taskENTER_CRITICAL();           /* 进入临界区(防止创建任务被打断，停止任务调度) */
  /* 创建任务1 */
  xTaskCreate((TaskFunction_t )Task1,           /* 任务函数 */
              (const char*    )"Task1",         /* 任务名称 */
              (uint16_t       )128,             /* 任务堆栈大小 */
              (void*          )NULL,            /* 传入给任务函数的参数 */
              (UBaseType_t    )2,               /* 任务优先级 */
              (TaskHandle_t*  )&Task1_t);       /* 任务句柄 */

  /* 创建任务2 */
  xTaskCreate((TaskFunction_t )Task2,           /* 任务函数 */
              (const char*    )"Task2",         /* 任务名称 */
              (uint16_t       )128,             /* 任务堆栈大小 */
              (void*          )NULL,            /* 传入给任务函数的参数 */
              (UBaseType_t    )3,               /* 任务优先级 */
              (TaskHandle_t*  )&Task2_t);       /* 任务句柄 */  
  
  vTaskDelete(StartTask_t);       /* 删除开始任务 */
  taskEXIT_CRITICAL();            /* 退出临界区(任务开始调度) */    
}

void Task1(void *pvParameters)
{
  led_init();
  vTaskList(TaskInfoBuf);
  printf("\r\n任务名\t\t 状态\t 优先级\t 剩余栈\t 任务序号\r\n");
  printf("%s\r\n", TaskInfoBuf);  
  while(1)
  {
    led_toggle();
    vTaskDelay(200);
  }  
}

void Task2(void *pvParameters)
{
  key_init();
  while(1)
  {
    key_scan();
    if(Key_Val == Key_Enter)
      vTaskSuspend(Task1_t);
    if(Key_Val == Key_Up)
      vTaskResume(Task1_t);    
    vTaskDelay(20);
  }  
}
