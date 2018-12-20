#include "kernel_id.h"
#include "button.h"
#include "Menu.h"
#include "Move.h"
#include "Arm.h"
#include "Log.h"
#include "Task.h"
#include "monoatume.h"

static MFunc g_strategy=Strategy;
static MFunc g_function=Strategy;

void NormalMenu(NameFunc* MenuList,int cnt){
  //変数を渡す必要の無いメニュー オブジェクト指向が使えないのが不便
  int i;
  nxtButton btn;
  int menu = 0;
  static int level=0,canceled=0;
  level++;
  for (;;) {
    wait_for_release();
    wai_sem(Sdisp);//ディスプレイの占有権待ち
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
    sig_sem(Sdisp);
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if(MenuList[menu].sub==0){
	g_function=MenuList[menu].func;
	Start();
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
    wait_for_release();
    wai_sem(Sdisp);//ディスプレイの占有権待ち
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
    sig_sem(Sdisp);
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

void Calibration(){
  //色のカリブレーション
  //アームのカリブレーション
  CalibArm();
}

void Run(){
  act_tsk(Ttimer);
  act_tsk(Tmusc);
  (*g_strategy)();  
}

void Setting(){
  SetFunc SettingMenu[]={
    {"P_GAIN",SetPgain,GetPgain()},
    {"I_GAIN",SetIgain,GetIgain()},
    {"D_GAIN",SetDgain,GetDgain()},
  };
  SetMenu(SettingMenu,ARRAYSIZE(SettingMenu));
}

void SetNormal(){
  g_function=Strategy;
}

void SetRedBall(){
  g_function=RedBall;
}

void SetTire(){
  g_function=Tire;
}


void ChangeStrategy(){
  NameFunc StrategyMenu[] = {
    {"Normal",SetNormal},
    {"RedBall",SetRedBall},
    {"Tire",SetTire},
   };
  NormalMenu(StrategyMenu,ARRAYSIZE(StrategyMenu));
}

/*
  FuncTsk
  メニュー実行用タスク
*/
void FuncTsk(VP_INT exinf){
  wai_sem(Sdisp);
  display_clear(0);
  sig_sem(Sdisp);
  (*g_function)();
  Quit();
}

//テスト関数群

void Credit(){
  LogString("2018");
  LogString("cirusmarine1505");
  dly_tsk(100);
}

void RunSquare(){
  int i;
  for(i=0;i<4;i++){
    MoveLength(70,0,1000);
    MoveTurn(70,90,Rmotor);
  }
}

void BackForce(){
  MoveLength(70,0,1000);
  MoveLength(-70,0,1000);
}

void Test(){
  NameFunc TestMenu[]={
    {"RunSquare",RunSquare},
    {"Backn'Force",BackForce},
    {"",Credit,2}
  };
  NormalMenu(TestMenu,ARRAYSIZE(TestMenu));
}

