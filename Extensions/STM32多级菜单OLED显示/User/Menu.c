#include "Menu.h"
#include "OLED/OLED.h"
#include "KEY/key.h"

//������ͱ�����Ĭ�Ϸ���
CursorType CurType = CursorReserver; 

int8_t RollSpeed = 4, AnimSpeed = 6;
uint16_t FPS = 0, FPS_Count = 0;

uint8_t Get_FPS(void)	//��ȡ֡��
{
	FPS_Count++;
	return FPS;
}

int8_t menu_roll_event(void)
{
  if (Key_Val == Key_Up)
  {
    Key_Val = Key_None;
    return -1;
  }
  else if (Key_Val == Key_Down)
  {
    Key_Val = Key_None;
    return 1;
  }
  return 0;
}

int8_t menu_enter_event(void)
{
  if (Key_Val == Key_Enter)
  {
    Key_Val = Key_None;
    return 1;
  }
  return 0;	
}

void main_menu(void)
{
  opt main_list[] = {
    {"�ػ�"},
    {"����", 0, Tools_menu},
    {"����", 0, Setting_menu},
    {"̫����", 0, SpaceMan},
    {"��Ϣ", 0, Information},
    {"..."}
  };
  running_menu(main_list);
}

void running_menu(opt *opt_list)
{
  int8_t opt_index = 1, cur_index = 0;    //ѡ������������������    
  /*  ��궯������  ��̬�������Ա����ϸ��б���λ��*/
  static float  curSpos = 0, curEpos = 0; //���λ��������յ�
  static float  curSlen = 0, curElen = 0; //��곤��������յ�
  static float curpos_temp = 0, curlen_temp = 0;  //���λ���볤�ȱ仯��ʱ����

  int8_t opt_num = 0; //ѡ����
  int8_t rollVal = 0; 
  /*  ����ѡ�����  */
  while(opt_list[++opt_num].opt_name[0] != '.');
  opt_num --; //����ӡ���һ��ѡ��
  /*  ����ѡ���  */
  for(int i = 0; i <= opt_num; i++)
    opt_list[i].opt_name_len = strlen(opt_list[i].opt_name);  
  /*  ѡ��ƫ�Ʊ����������б������ʾ����  */
  int8_t offset = 0, offset_last = opt_num; //��ǰƫ�������ϴ�ƫ����
  int8_t offset_temp = 0; //ƫ�Ʊ仯��ʱ����

  while (1)
  {
    OLED_Clear();
    rollVal = menu_roll_event();
    if(rollVal)
    {
      opt_index += rollVal; //���� +1������ -1
      cur_index += rollVal;

      if(opt_index < 0)               opt_index = 0;
      else if(opt_index > opt_num)    opt_index = opt_num;
     
      if(cur_index < 0)               cur_index = 0;
      else if(cur_index > opt_num)    cur_index = opt_num;
      else if(cur_index > 3)          cur_index = 3;  //������ƣ���ֹ������Ļ��
    }
    /*  ����ƫ������Ĭ��Ϊ 1����ѡ���������� 3 �� ��һ�����ƣ�ƫ�����仯  */
    offset = opt_index - cur_index;
    /*  ��һ�� �� ƫ�����仯������Ϊ�棬����Ϊ��  */
    if(offset - offset_last)
    {
      offset_temp = (offset - offset_last) * LineHeight;  //����ƫ�Ʊ仯��ʱ����
      offset_last = offset;
    }
    /*  ʹƫ����ʱ�仯����˥��Ϊ 0 */
    if(offset_temp) offset_temp /= RollSpeed;
    /*  ����ѡ�ͨ��ƫ������ȷ����ʾ���ѡ�� */
    for (int i = 0; i < 5; i++) 
    {
      if(offset + i > opt_num)  break;
      if(opt_list[i+offset].opt_name[0] > '~')  //�жϵ�һ���ַ��Ƿ�Ϊ�����ַ�
        OLED_ShowChinese(2, Line(i)+offset_temp, opt_list[i+offset].opt_name);
      else
        OLED_ShowString(2, Line(i)+offset_temp, opt_list[i+offset].opt_name, OLED_8X16);
    }
    /*
      * ����ʵ��ԭ��
      * ͨ�����λ�õ�Ŀ��λ��֮��������������ͨ��OLED������ˢ��ʵ�ֶ���Ч��
      */
    /* ����ѡ���յ�λ����ѡ���յ㳤�� */
    curEpos = Line(cur_index);
    curElen = 8 * opt_list[opt_index].opt_name_len + 4;
    /*  �������  */
    curpos_temp = curEpos - curSpos;
    curlen_temp = curElen - curSlen;
    /*  ��������  */
    if(curpos_temp > 1)         curSpos += curpos_temp/AnimSpeed + 1;
    else if(curpos_temp < -1)   curSpos += curpos_temp/AnimSpeed - 1;
    else  curSpos = curEpos;
    if(curlen_temp > 1)         curSlen += curlen_temp/AnimSpeed + 1;
    else if(curlen_temp < -1)   curSlen += curlen_temp/AnimSpeed - 1;
    else  curSlen = curElen;
    /*  ѡ����ʽ����  */  
    if(CurType == CursorReserver)      
      OLED_ReverseArea(0, curSpos, curSlen, LineHeight);
    else if(CurType == CursorRect)
      OLED_DrawRectangle(0, curSpos, curSlen, LineHeight, OLED_UNFILLED);
    else if(CurType == CursorTail)
      OLED_ShowString(curSlen+2, curSpos+6, "<=", OLED_6X8); 
    /*  ��ʾ��ǰѡ������  */    
    OLED_ShowNum(110, 55, opt_index, 2, OLED_6X8);
    /*  ��ʾ֡��  */
    OLED_ShowNum(110, 5, Get_FPS(), 2, OLED_6X8);
    OLED_Update();

    if(menu_enter_event())
    {
      if(opt_list[opt_index].fun != NULL)
        opt_list[opt_index].fun();
      else return;       
    }          
  }
}

void Tools_menu(void)
{
  opt Tools_list[] ={
    {"<<<"},
    {"LED TOGGLE"},
    {"TIMER COUNT"},
    {"PWM OUT"},
    {"PWM IN"},
    {"ADC SAMPLE"},
    {"RTC SHOW"},
    {"DAC OUT"},
    {"SPI FLASH"},
    {"SDIO TFCARD"},
    {"USART PRINTF"},      
    {"USB PRINTF"},
    {"..."}
  };
  running_menu(Tools_list);
}

opt set_list[] = {
  {"<<<"},
  {"Anim: [ON]",0,SetAnim},
  {"Style: [Resver]",0,SetCurStyle},
  {"..."}      
};

void SetCurStyle(void)
{
  memset(set_list[2].opt_name, 0, set_list[2].opt_name_len);
  if(++CurType > CursorTail)
    CurType = CursorReserver;
  if(CurType == CursorReserver)
    sprintf(set_list[2].opt_name,"Style: [Resver]");
  else if(CurType == CursorRect)
    sprintf(set_list[2].opt_name,"Style: [Frame]");
  else if(CurType == CursorTail)
    sprintf(set_list[2].opt_name,"Style: [Tail]");
  set_list[2].opt_name_len = strlen(set_list[2].opt_name);  
}
void SetAnim(void)
{
  memset(set_list[1].opt_name, 0, set_list[1].opt_name_len);
 if(AnimSpeed == 6) 
 {
   AnimSpeed = 1;
   sprintf(set_list[1].opt_name,"Anim: [OFF]");
 }
 else if(AnimSpeed == 1)
 {
   AnimSpeed = 6;
   sprintf(set_list[1].opt_name,"Anim: [ON]");
 }
  set_list[1].opt_name_len = strlen(set_list[1].opt_name);
}

void Setting_menu(void)
{ 
  running_menu(set_list);
}

void SpaceMan(void)
{
  while (1)
  {
    for(int i = 0; i < 10; i++)
    {
      OLED_Clear();
      OLED_ShowImage(35,10,52,48,gImage[i]);
      OLED_ShowNum(110, 5, Get_FPS(), 2, OLED_6X8);
      OLED_Update();
      delay_ms(30);
    }
    if (menu_enter_event())   return;   
  }
}

void Information(void)
{
  char temp[20];
  
  OLED_Clear();  
  OLED_ShowString(2, 0, "Menu V1.0", OLED_8X16);	
  OLED_ShowString(2, 23, "IC: STM32F103RCT6", OLED_6X8);  
  sprintf(temp, "Rom: %dKB", *((__IO uint16_t *)(0x1FFFF7E0)));
  OLED_ShowString(2, 35, temp, OLED_6X8);
  sprintf(temp, "UID: 0x%x", *((__IO uint32_t *)(0x1FFFF7E8 + 0x04)));
  OLED_ShowString(2, 45, temp, OLED_6X8);
  OLED_Update();  
	while(1)
	{
		if(menu_enter_event()) {return;}	
	}
}

