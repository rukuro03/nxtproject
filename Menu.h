#ifndef MENU_H
#define MENU_H
typedef void (*MFunc)(void); //メニュー関数
typedef void (*SFunc)(int); // メニュー関数(引数あり)

typedef struct _NameFunc {
  char *name;
  MFunc func;
  int sub;// サブメニューを開くか否か
} NameFunc;

typedef struct _SetFunc{
  char *name;
  SFunc func;
  int def;// 引数のデフォルト値
} SetFunc;

void NormalMenu(NameFunc*,int);
void SetMenu(SetFunc*,int);
//メインメニューの項目
void Calibration();
void Run();
void Setting();
void ChangeStrategy();
void Test();
#endif
