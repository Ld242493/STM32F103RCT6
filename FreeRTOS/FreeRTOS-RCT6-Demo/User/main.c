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

TaskHandle_t StartTask_t, Task1_t, Task2_t;   // ������
char TaskInfoBuf[50];      //������Ϣ�������

int main(void)
{  
  Systick_Init();
  USART1_Config();
  OLED_Init();
  OLED_ShowString(10, 0, "FreeRTOS Demo", OLED_8X16);
  OLED_ShowChinese(0, 20, "��ã����硣");
  OLED_Update();
  //�����õ��ж����ȼ���Χ(0~15, MAX = 0, MIN = 15)
  //FreeRTOS�ɹ�����жϷ�Χ(5~15)
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  xTaskCreate((TaskFunction_t )StartTask,       /* ������ */
              (const char*    )"StartTask",     /* �������� */
              (uint16_t       )128,             /* �����ջ��С */
              (void*          )NULL,            /* ������������Ĳ��� */
              (UBaseType_t    )1,               /* �������ȼ� */
              (TaskHandle_t*  )&StartTask_t);   /* ������ */  
  vTaskStartScheduler();
}

void StartTask(void * pvParameters)
{
  taskENTER_CRITICAL();           /* �����ٽ���(��ֹ�������񱻴�ϣ�ֹͣ�������) */
  /* ��������1 */
  xTaskCreate((TaskFunction_t )Task1,           /* ������ */
              (const char*    )"Task1",         /* �������� */
              (uint16_t       )128,             /* �����ջ��С */
              (void*          )NULL,            /* ������������Ĳ��� */
              (UBaseType_t    )2,               /* �������ȼ� */
              (TaskHandle_t*  )&Task1_t);       /* ������ */

  /* ��������2 */
  xTaskCreate((TaskFunction_t )Task2,           /* ������ */
              (const char*    )"Task2",         /* �������� */
              (uint16_t       )128,             /* �����ջ��С */
              (void*          )NULL,            /* ������������Ĳ��� */
              (UBaseType_t    )3,               /* �������ȼ� */
              (TaskHandle_t*  )&Task2_t);       /* ������ */  
  
  vTaskDelete(StartTask_t);       /* ɾ����ʼ���� */
  taskEXIT_CRITICAL();            /* �˳��ٽ���(����ʼ����) */    
}

void Task1(void *pvParameters)
{
  led_init();
  vTaskList(TaskInfoBuf);
  printf("\r\n������\t\t ״̬\t ���ȼ�\t ʣ��ջ\t �������\r\n");
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
