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
    {"WHEEL",SetWheelRadius,GetWheelRadius()},
    {"SHAFT",SetShaftLength,GetShaftLength()}
  };
  SetMenu(SettingMenu,ARRAYSIZE(SettingMenu));
}

void SetNormal(){
  g_strategy=Strategy;
}

void SetRedBall(){
  g_strategy=RedBall;
}

void SetTire(){
  g_strategy=Tire;
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
  */

  int pow=0,sum=0;
  int i;
  int ldat[10]={0};
  int rdat[10]={0};
  nxtButton btn;
  for(i=0;i<10;i++){
    //パワーを上げながら値(度)を取得していきます
    pow = i*10;
    //わかってる値を表示
    display_clear(0);
    display_goto_xy(0,1);
    display_string("Testing...");
    display_goto_xy(0,2);
    display_string("Pow  L  R");
    display_goto_xy(0,3);
    display_int(pow,4);
    display_update();
    //モータ回転開始
    motor_set_speed(Rmotor, pow, 1);//パワーを与え
    motor_set_speed(Lmotor, pow, 1);
    nxt_motor_set_count(Rmotor, 0);//回転角初期化？
    nxt_motor_set_count(Lmotor, 0);
    dly_tsk(1000);//1s待機
    rdat[i]=nxt_motor_get_count(Rmotor);//回転角度取得
    ldat[i]=nxt_motor_get_count(Lmotor);
    //回転速度表示
    display_string(" ");
    display_int(ldat[i],4);
    display_string(" ");
    display_int(rdat[i],4);
    display_update();
  }
  //一応ひと目でわかるように適当な誤差値を出すようにする
  //とりあえず小さいほどいい感じ
  for(i=0;i<10;i++){
    sum += (rdat[i] - ldat[i]) * (rdat[i] - ldat[i]);
  }
  display_clear(0);
  display_goto_xy(0,1);
  display_string("Result:");
  display_int(sum,4);
  display_goto_xy(0,2);
  display_string("Press Orange");
  display_update();
  /*この間は
    Result:1000
    Press Orange
    　みたいな表示が出ている
  */
  do{
    btn = get_btn();
  }while(btn != Obtn);
  motor_set_speed(Rmotor, 0, 1);
  motor_set_speed(Lmotor, 0, 1);
}

void Gentle(){
  MoveArm(ARM_DOWN,30);
  MoveLength(20,0,200);
  MoveArm(ARM_UP,20);
  dly_tsk(1000);
  MoveLength(-20,0,200);
  MoveArm(ARM_DOWN,10);
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

void Test2(){
  NameFunc TestMenu[]={
    {"CountArm",CountArm,0},
  };
  NormalMenu(TestMenu,ARRAYSIZE(TestMenu));
}

void Test(){
  NameFunc TestMenu[]={
    {"RunSquare",RunSquare,2},
    {"Backn'Force",BackForce,2},
    {"SyncMotor",SyncMotor,0},
    {"Gentle",Gentle,2},
    {"Test2",Test2,1},
    {"",Credit,2}
  };
  NormalMenu(TestMenu,ARRAYSIZE(TestMenu));
}

