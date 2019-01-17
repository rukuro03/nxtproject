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
	act_tsk(Tfunc);
	act_tsk(Tquit);
	break;
      }
      else{
	//サブメニュー
	MenuList[menu].func();
	if(MenuList[menu].sub==1){
	  //サブメニュー
	  if(canceled==0)
	    break;
	  else{
	    canceled=0;
	    continue;
	  }
	}
	else if(MenuList[menu].sub==2){ //直接実行
	  continue;
	}
      }
      break;
    case Cbtn:	// グレーボタン == キャンセル
      if(level==1){
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
  level--;
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
    {"WHEEL",SetWheelRadius,GetWheelRadius()},
    {"SHAFT",SetShaftLength,GetShaftLength()}
  };
  SetMenu(SettingMenu,ARRAYSIZE(SettingMenu));
}

void SetNormal(){
  g_strategy=Strategy;
}

void SetBlock(){
  g_strategy=Block;
}

void SetTire(){
  g_strategy=Tire;
}

void SetReNormal(){
  g_strategy=ReNormal;
}
void SetReBlock(){
  g_strategy=ReBlock;
}
void SetReTire(){
  g_strategy=ReTire;
}

void ChangeReverse(){
  NameFunc ReverseMenu[] = {
    {"RE_Normal",SetReNormal},
    {"RE_Block",SetReBlock},
    {"RE_Tire",SetReTire},
  };
  NormalMenu(ReverseMenu,ARRAYSIZE(ReverseMenu));
}

void ChangeStrategy(){
  NameFunc StrategyMenu[] = {
    {"Normal",SetNormal},
    {"Block",SetBlock},
    {"Tire",SetTire},
    {"Reverse",ChangeReverse,1},
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
  ter_tsk(Tquit);
}

//テスト関数群

void Credit(){
  wai_sem(Sdisp);
  display_clear(0);
  sig_sem(Sdisp);
  LogString("2018");
  LogString("cirusmarine1505");
  dly_tsk(2000);
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

void SyncMotor(){
  /*
    左右のモータのシンクロ率を測ります
    パワーを0から90まで上げながら、
    左右のモータの回転速度(度/10ミリ秒)を表示します
    0.5秒計測する→表示する→オレンジ押す→パワー上がる…
    のループです　

    最終的な出力が小さいほどいいモータのペアです
  */
  int pow=0,sum=0;
  int i;
  int ldat,rdat;
  nxtButton btn;
  wai_sem(Sdisp);
  display_clear(0);
  display_goto_xy(0,1);
  display_string("Testing...");
  display_update();
  sig_sem(Sdisp);
  for(i=0;i<11;i++){
    //パワーを上げながら値(度)を取得していきます
    pow = i*10;
    motor_set_speed(Rmotor, pow, 1);//パワーを与え
    motor_set_speed(Lmotor, pow, 1);
    nxt_motor_set_count(Rmotor, 0);//回転角初期化
    nxt_motor_set_count(Lmotor, 0);
    dly_tsk(1000);//1s待機
    rdat=nxt_motor_get_count(Rmotor);//回転角度取得
    ldat=nxt_motor_get_count(Lmotor);
    sum+=(rdat-ldat)*(rdat-ldat);
  }
  wai_sem(Sdisp);
  display_clear(0);
  display_goto_xy(0,1);
  display_string("Result:");
  display_int(sum,4);
  display_goto_xy(0,2);
  display_string("Press Orange");
  display_update();
  sig_sem(Sdisp);
  motor_set_speed(Rmotor, 0, 1);
  motor_set_speed(Lmotor, 0, 1);
  /*この間は
    Result:1000
    Press Orange
    　みたいな表示が出ている
  */
  do{
    btn = get_btn();
  }while(btn != Obtn);
}

void Gentle(){
  SetArmUp(ARM_T_UP);
  ArmDown(30);
  MoveLength(20,0,200);
  ArmUp(30);
  dly_tsk(1000);
  MoveLength(-20,0,200);
  ArmDown(10);
}

void TestArm(){
  for(int i=0;i<100;i++){
    LogString("Arm Up");
    ArmUp(40);
    LogString("Arm Down");
    ArmDown(40);
  }
}

void CountArm(){
  int rot;
  nxt_motor_set_count(Arm,0);//一番下に下げた状態でカウントを0にする
  for(;;){
    rot=nxt_motor_get_count(Arm);
    LogInt(rot);
    dly_tsk(10);
  }
}

void TurnTest1(){
  MoveTurn(70,90,Lmotor);
}

void TurnTest2(){
  //紫の四角に向かう
  LogString("Go Purple");
  MoveCurve(-70,50,150);
  MoveLength(-70,0,100);
  ArmUp(30);
  MLIgnoreTouch(50, 10, 800);
  ArmDown(100);
  MoveLength(-50, 0, 100);
  MoveLength(50, 0, 100);
  dly_tsk(1000);
  ArmUp(100);
  //帰る
  LogString("Return");
  MoveLength(-70,0,2700);
}

void LongRun(){
  MLIgnoreTouch(70,0,1100);
}

void Test2(){
  NameFunc TestMenu[]={
    {"TestArm",TestArm,0},
    {"CountArm",CountArm,0},
    {"Turn test1",TurnTest1,2},
    {"RunSquare",RunSquare,2},
    {"",Credit,2},
  };
  NormalMenu(TestMenu,ARRAYSIZE(TestMenu));
}

void Test(){
  NameFunc TestMenu[]={
    {"LastTest",TurnTest2,2},
    {"LongRun",LongRun,2},
    {"SyncMotor",SyncMotor,0},
    {"Gentle",Gentle,2},
    {"Test2",Test2,1},
  };
  NormalMenu(TestMenu,ARRAYSIZE(TestMenu));
}

