#ifndef __KEY_H
#define __KEY_H

#include "main.h"

typedef enum {
  Key_None  = 0,
  Key_Enter,
  Key_Up,
  Key_Down,
  Key_Enter_long
}key_type;


extern key_type Key_Val;

void key_init(void);
void key_scan(void);
#endif
