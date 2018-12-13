#ifndef MENU_H
#define MENU_H
typedef void (*MFunc)(void);
typedef void (*SFunc)(int);
typedef struct _NameFunc {
  char *name;
  MFunc func;
} NameFunc;

typedef struct _SetFunc{
  char *name;
  SFunc func;
} SetFunc;

void Calibration();
void TestStrategy();
void Setting();
MFunc MenuFunc();
#endif
