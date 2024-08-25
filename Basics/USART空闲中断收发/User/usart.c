#include "usart.h"

uint8_t RxBuffer[50];
uint8_t RxCount;
uint8_t RxFlag;

void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure USARTy Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USARTy */
  USART_Init(USART1, &USART_InitStructure);
  /* Enable USARTy Receive interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable USARTy Receive Idle interrupts */
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
  /* Enable the USARTy */
  USART_Cmd(USART1, ENABLE);
}

int fputc(int ch, FILE *f)
{
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  return ch;  
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer[RxCount++] = USART_ReceiveData(USART1);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
  else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
  {
    /*  读数据寄存器，清除空闲中断标志位  */
    USART_ReceiveData(USART1);
    RxCount = 0;
    RxFlag = 1;
  }
}