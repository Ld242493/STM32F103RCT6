#ifndef __MENU_H
#define __MENU_H

#include "main.h"

typedef struct {
  uint8_t opt_name[20];
  int8_t opt_name_len;
  void (*fun)(void);
}opt;

typedef enum {
  CursorReserver = 0x01,
  CursorRect,
  CursorTail,  
}CursorType;

#define LineHeight    16
#define Line(x)       (x*LineHeight)

void main_menu(void);
void running_menu(opt *opt_list);
void Setting_menu(void);
void SetCurStyle(void);
void SetAnim(void);
void Tools_menu(void);
void Information(void);
void SpaceMan(void);
#endif
