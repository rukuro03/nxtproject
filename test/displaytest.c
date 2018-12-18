/* 
   NXTのディスプレイ制御を適当にシミュレートしたプログラムです
   かなりの機能は適当ですので、あくまでコンパイルが通るかどうか、
   最低限の機構は機能するかくらいのテストしか出来ません。
   オレンジボタン=y
   キャンセルボタン=q
   右ボタン=右矢印
   左ボタン=左矢印
   コンパイルコマンド： gcc displaytest.c display.c -lncurses
   curses.hを使用するので何がしかのcursesライブラリが必要です。
   Mac・Linuxなら大体ncursesが入っていますが、Windowsの場合
   余りサポートされていません。CygWinかMinGWを使っていれば
   コンパイルできるかもしれません。
*/
#include "display.h"
#include "../Menu.h" 
#include <unistd.h>

static MFunc g_strategy;

void NormalMenu(NameFunc* MenuList,int cnt){
  //変数を渡す必要の無いメニュー オブジェクト指向が使えないのが不便
  int i;
  nxtButton btn;
  int menu = 0;
  MFunc retval;
  static int level=0,canceled=0;
  level++;
  for (;;) {
    display_clear(0);
    for (i = 0; i < cnt; i++) {
      if (i == menu) {
	display_goto_xy(1,i+HEADER+1);
	if(MenuList[menu].sub==0)
	  display_string(">");
	else
	  display_string("*");
      }
      display_goto_xy(2, i+1);
      display_string(MenuList[i].name);
    }
    display_update();
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if(MenuList[menu].sub==0){
	//実際はタスク起動
	End();
	MenuList[menu].func();
	Init();
      }
      else{
	//サブメニュー
	MenuList[menu].func();
	if(MenuList[menu].sub==1){
	  if(canceled==0)
	    break;
	  else//サブメニュー(変数選択)
	    continue;
	}
	else
	  continue;
      }
      break;
    case Cbtn:	// グレーボタン == キャンセル
      if(level==0){
	ecrobot_shutdown_NXT();
	break;
      }
      else{
	canceled=1;
	break;
      }
    case Rbtn:	// 右ボタン == 次へ
      menu++;
      if (menu >= cnt) menu = 0;
      continue;
    case Lbtn:	// 左ボタン == 前へ
      --menu;
      if (menu < 0) menu = cnt-1;
      continue;
    default:	// 複数が押されている場合
      continue;
    }
    break;
  }
}

void SetMenu(SetFunc* MenuList,int cnt){
  //変数を渡す必要のあるメニュー 値自体はintになる
  int i,state=0;
  nxtButton btn;
  int nums[cnt],defs[cnt];//変数要素数の配列が使用可能であること前提でいきます
  //nums:変数の値 defs:変数のデフォルト値
  int menu = 0;
  for(i=0;i<cnt;i++){
    nums[i]=MenuList[i].def;
    defs[i]=MenuList[i].def;
  }
  for (;;) {
    display_clear(0);
    for (i = 0; i < cnt; i++) {
      if (i == menu) {
	display_goto_xy(1,i+HEADER+1);
	if(state==0)
	  display_string(">");
	else
	  display_string("*");
      }
      display_goto_xy(2, i+1);
      display_string(MenuList[i].name);
      display_string(":");
      display_int(nums[i],4);
    }
    display_update();
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if(state==0){
	state=1;
      }
      else{
	(*MenuList[menu].func)(nums[menu]);
	state=0;
      }
      break;
    case Cbtn:	// グレーボタン == キャンセル
      if(state==1){
	nums[menu]=defs[menu];
	state=0;
      }
      else
	return;
      break;
    case Rbtn:	// 右ボタン == 次へ
      if(state==0){
	menu++;
	if (menu >= cnt) menu = 0;
      }
      else{
	nums[menu]++;
      }
      break;
    case Lbtn:	// 左ボタン == 前へ
      if(state==0){
	--menu;
	if (menu < 0) menu = cnt-1;
      }
      else{
	nums[menu]--;
      }
      break;
    default:	// 複数が押されている場合
      break;
    }
  }
}

void Setting(){
  SetFunc SettingMenu[]={
    {"P_GAIN",DummyInt,ReturnInt()},
    {"I_GAIN",DummyInt,ReturnInt()},
    {"D_GAIN",DummyInt,ReturnInt()},
    {"WAIT",DummyInt,ReturnInt()}
  };
  SetMenu(SettingMenu,ARRAYSIZE(SettingMenu));
}

void ChangeStrategy(){
  NameFunc StrategyMenu[]={
    {"Normal",Dummy},
    {"RedBall",Dummy},
    {"Tire",Dummy},
  };
  NormalMenu(StrategyMenu,ARRAYSIZE(StrategyMenu));
}

int main(){
  NameFunc MainMenu[] = {
    {"Calibration", Dummy,0},// アームのキャリブレーション
    {"Start",Dummy,0},
    {"Setting",Setting,2},
    {"ChangeStrategy",ChangeStrategy,1},
  };
  Init();
  NormalMenu(MainMenu,ARRAYSIZE(MainMenu));
  End();
}
