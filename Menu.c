#include "kernel_id.h"
#include "button.h"
#include "Menu.h"
#include "Move.h"
#include "Arm.h"
#include "Log.h"
#include "monoatume.h"

//monoatume.c 内で定義される関数
void Strategy(void);

void Calibration(){
  //色のカリブレーション
  //アームのカリブレーション
  CalibArm();
}

void Setting(){
  SetFunc SettingMenu[]={
    {"P_GAIN",SetPgain},
    {"I_GAIN",SetIgain},
    {"D_GAIN",SetDgain}
  };
  int nums[3]={
    GetPgain(),
    GetIgain(),
    GetDgain()
  };
  int cnt=3;
  int i,state=0;
  nxtButton btn;
  static int menu = 0;
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
      display_string(SettingMenu[i].name);
      display_string(":");
      display_int(nums[i],4);
    }
    display_update();
    sig_sem(Sdisp);//ディスプレイの占有権返却
    
    btn = get_btn();
    
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if(state==0){
	state=1;
      }
      else{
	(*SettingMenu[menu].func)(nums[menu]);
	state=0;
      }
      break;
    case Cbtn:	// グレーボタン == キャンセル
      if(state==1)
	state=0;
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

void TestStrategy2(){
  //青を落とす
  MoveArm(ARM_UP);
  MLIgnoreTouch(70, 2, 700);
  MoveArm(ARM_DOWN);
  MLIgnoreTouch(-70, 0, 50);
  MoveArm(ARM_UP);
  MLIgnoreTouch(-70, 2, 700);

}

void TestStrategy(){
  /* LogString("Arm up");
  MoveArm(ARM_UP);
  LogString("Forward");
  MoveLength(BASE_POWER,0,900);
  LogString("Arm down");
  MoveArm(ARM_DOWN);
  MoveLength(-BASE_POWER,0,300);
  MoveTurn(BASE_POWER,90,Rmotor);
  MoveArm(ARM_UP);
  MoveLength(BASE_POWER,0,400);
  MoveArm(ARM_DOWN);
  MoveLength(-BASE_POWER,0,200);
  */

  /*  //赤ボールを取る */
  /* LogString("Red bull"); */
  /* MLIgnoreTouch(70, 0, 1100); */
  /* //バックする */
  /* MoveLength(-70, 0, 1000); */
  MLIgnoreTouch(70,0,2000);
  MLIgnoreTouch(-70,0,2000);
}

MFunc MenuFunc(){
  //メニュー起動

  NameFunc MainMenu[] = {
    {"Calibration", Calibration},// アームのキャリブレーション
    {"Start",Strategy},
    {"Test1",TestStrategy},
    {"Test2",TestStrategy2},
    {"Setting",Setting},
  };
  
  int cnt=ARRAYSIZE(MainMenu);
  int i;
  nxtButton btn;
  static int menu = 0;
  for (;;) {
    wait_for_release();
    wai_sem(Sdisp);//ディスプレイの占有権待ち
    display_clear(0);
    for (i = 0; i < cnt; i++) {
      if (i == menu) {
	display_goto_xy(1,i+HEADER+1);
	display_string(">");
      }
      display_goto_xy(2, i+1);
      display_string(MainMenu[i].name);
    }
    display_update();
    sig_sem(Sdisp);//ディスプレイの占有権返却
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      return MainMenu[menu].func;
    case Cbtn:	// グレーボタン == キャンセル
      ecrobot_shutdown_NXT();
      break;
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
  return Strategy;
}
