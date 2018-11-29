#include "display.h"

#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "ecrobot_base.h"

#include "monoatume.h"
#include "monoatume_cfg.h"
#include "music.h"
#include "button.h"
#include "graphics.h"

//定義・宣言群
#define ARRAYSIZE(A)	(sizeof((A)) / sizeof((A)[0]))
#define MACHINE_NAME "GOHAN"
#define TIME_LEFT 180
#define WHEEL_RADIUS 30 
#define MOVETSK_WAIT 200
#define ARM_POWER 30
//ヘッダー・フッター位置 
#define HEADER 0
#define FOOTER 7

typedef void (*MFunc)(void);
typedef struct _NameFunc {
  char *name;
  MFunc func;
} NameFunc;

int get_touch();
void get_master_slave(DeviceConstants*,DeviceConstants*);
void func_calib();
void strategy();
void func_menu();
void MoveSetPower(int);
void MoveSetSteer(int);
void MoveActivate();
void MoveTerminate();
void ClearLog();
void LogString(char*);
void LogInt(int);
void CheckLength(int);
void SetTimeOut(int);
FLGPTN WaitForOR(FLGPTN);
FLGPTN WaitForAND(FLGPTN);
FLGPTN MoveLength(int,int,int);
void MoveArm(int);

//グローバル変数群　できれば使いたくないが組み込みだからね
//グローバル変数には g_ のプレフィックスをつけてください
int g_timer,g_power,g_turn,g_length;
int g_armdown;
int g_timeout;
int g_dbg1=0,g_dbg2=0;
double g_pgain=1,g_igain=1,g_dgain=1;
int g_log=HEADER+1;
int g_white[3]={0,0,0};
int g_black[3]={0,0,0};
void (*g_function)(void)=strategy;

//ユーティリティ群

int get_touch(){
  //RtouchかLtouchか0を返す
  if(ecrobot_get_touch_sensor(Rtouch))
    return (int)Rtouch;
  if(ecrobot_get_touch_sensor(Ltouch))
    return (int)Ltouch;
  return 0;
}

void get_master_slave(DeviceConstants* master,DeviceConstants* slave){
  //マスター(主モータ)：外側のモータ
  //スレーブ(従モータ)：内側のモータ を取得する
  //うーん　あくまで外側のモーターってだけだからマスター/スレーブってどうよ
  //たしかに外側のモータが主として機能するけどさ
  
  if(g_turn<0){
    *master=Rmotor;
    *slave=Lmotor;
  }
  else{
    *master=Lmotor;
    *slave=Rmotor;
  }
}

void func_calib(){
  int rot=0;
  //色のカリブレーション
  //アームのカリブレーション
  //しくじった　どっちが上かわからん
  motor_set_speed(Arm,-ARM_POWER,1);
  LogString("Arm Up");
  for(;;){
    rot=nxt_motor_get_count(Arm);
    dly_tsk(1000);
    rot-=nxt_motor_get_count(Arm);
    if(rot<0)
      rot=-rot;
    if(rot<1){
      nxt_motor_set_count(Arm,0);//一番上に上げた状態でカウントを0にする
      break;
    }
  }
  motor_set_speed(Arm,ARM_POWER,1);
  LogString("Arm Down");
  for(;;){
    rot=nxt_motor_get_count(Arm);
    dly_tsk(1000);
    rot-=nxt_motor_get_count(Arm);
    if(rot<0)
      rot=-rot;
    if(rot<1){
      g_armdown=nxt_motor_get_count(Arm);
      break;
    }
  }
  LogString("End");
  motor_set_speed(Arm,0,1);
}

void strategy(){
  //機能テスト用に適当に動作を指定しました
  //しばらくまっすぐすすんで
  act_tsk(Ttimer);
  act_tsk(Tmusc);
  LogString("Start");
  MoveLength(40,0,100);
  //右に回って
  LogString("Turn right");
  MoveLength(50,200,100);
  //左に回って
  LogString("Turn left");
  MoveLength(50,-200,100);
  //前後にぷるぷる
  LogString("Go back");
  MoveLength(-20,0,100);
  LogString("and forth");
  MoveLength(20,0,100);
  //アームを下ろす
  MoveArm(g_armdown);
}

void func_menu(){
  NameFunc MainMenu[] = {
    {"countdbg",DEBUG_count},
    {"Start",strategy},
    {"Calibration", func_calib},// センサーのキャリブレーション
    {"Power Off", ecrobot_shutdown_NXT},// 電源を切る
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
      g_function=MainMenu[menu].func;
      return;
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
}

//移動用関数群
void MoveSetPower(int pow){
  //パワーの登録…とはいえ、g_powに代入するだけです
  g_power=pow;
  motor_set_speed(Rmotor,pow,1);
  motor_set_speed(Lmotor,pow,1);
}
void MoveSetSteer(int turn){
  //ステアリングの登録…とはいえ、g_turnに代入するだけです
  DeviceConstants master,slave;
  int power;
  g_turn=turn;
  get_master_slave(&master,&slave);
  if(turn<0)
    turn=-turn;
  //turnの値は「外側のタイヤに対し内側のタイヤは(100-turn%)回る」という意味
  power=(100-turn)*g_power/100.0;
  motor_set_speed(slave,power,1);
}
void MoveActivate(){
  //  act_tsk(Tmove)のラッパーです
  act_tsk(Tmove);
}
void MoveTerminate(){
  //  ter_tsk(Tmove)のラッパーです
  ter_tsk(Tmove);
  MoveSetPower(0);
  MoveSetSteer(0);  
}

void ClearLog(){
  g_log=HEADER+1;
  wai_sem(Sdisp);
  display_clear(0);
  sig_sem(Sdisp);
}

void LogString(char* str){
  wai_sem(Sdisp);
  display_goto_xy(2,g_log);
  display_string(str);
  sig_sem(Sdisp);
  g_log++;
  if(g_log>=FOOTER)
    g_log=HEADER+1;
}

void LogInt(int dat){
  wai_sem(Sdisp);
  display_goto_xy(2,g_log);
  display_int(dat,4);
  sig_sem(Sdisp);
  g_log++;
  if(g_log>=FOOTER)
    g_log=HEADER+1;
}

void CheckLength(int length){
  //移動距離の登録…とはいえ、g_lengthに代入してCheckTskを起動するだけです
  //ついでにここでモータの回転角度を0に初期化します
  g_length=length;
  nxt_motor_set_count(Rmotor,0);
  nxt_motor_set_count(Lmotor,0);
  act_tsk(Tcheck);
}
void SetTimeOut(int time){
  //時間切れの登録…とはいえ、g_timeoutに代入してTimeOutTskを起動するだけです
  g_timeout=time;
  act_tsk(Ttimeout);
}

FLGPTN WaitForOR(FLGPTN flg){
  //wai_flg(Fsens,flg,TWF_ORW,&sensor)のラッパー関数です
  //先生のパクリ
  FLGPTN sensor;
  wai_flg(Fsens,flg,TWF_ORW,&sensor);
  return sensor;
}

FLGPTN WaitForAND(FLGPTN flg){
  //wai_flg(Fsens,flg,TWF_ANDW,&sensor)のラッパー関数です
  //先生のパクリ
  FLGPTN sensor;
  wai_flg(Fsens,flg,TWF_ANDW,&sensor);
  return sensor;
}

FLGPTN MoveLength(int pow,int turn,int length){
  FLGPTN sensor;
  //turn:-200~200
  //turnの値は「外側のタイヤに対し内側のタイヤは(100-turn%)回る」という意味
  //turnがマイナスだと右が外側左が内側
  //turnがプラスだと左が外側右が内側
  MoveSetPower(pow);
  MoveSetSteer(turn);
  CheckLength(length);
  MoveActivate();
  /*
    完了/時間切れ/左右どちらかのタッチセンサが押される
    のいずれかまで待つ
  */
  sensor=WaitForOR(efEndMove | efTOMove | efRtouch | efLtouch);
  MoveTerminate();
  return sensor;
}

void MoveArm(int deg){
  //現在のアームの状態から何度曲げるかを指定
  /* int ini=nxt_motor_get_count(Arm); */
  /* int  */
  /* for(;;){ */
  /*   motor_set_power(Arm,ARM_POWER,1); */
  /*   if() */
  /*   dly_tsk(10); */
  /* } */
  /* motor_set_power(Arm,0,1); */
}

//タスク用関数群

/*
  InitTsk
  初期化
*/
void InitTsk(VP_INT exinf){
  g_timer=TIME_LEFT;
  display_clear(0);
  display_goto_xy(2, 3);
  display_string("Initializing");
  display_update();
  act_tsk(Tmain);
  act_tsk(Tdisp);
}

/*
  ButtonTsk
  ボタン更新などなど
*/
//やはり意味がないと判断し削除しました
//タッチセンサのフラグはSensTskで制御します

/*
  MainTsk
  メニュー起動+選択された関数をタスクとして起動
*/
void MainTsk(VP_INT exinf){
  func_menu();
  act_tsk(Tfunc);
  act_tsk(Tquit);
}

/*
  QuitTsk
  メニュー項目強制終了用タスク
  ボタン押したら強制終了させ、初期化処理を行う
  なんか終わらせるタスクがやたらと多いんですが
*/
void QuitTsk(VP_INT exinf){
  nxtButton btn;
  for(;;){
    btn=get_btn();
    if(btn==Cbtn){
      //うーん　起動されるかされないかわからないタスクが多いんですが、
      //それも無理やりter_tskしてます。果たしてうまくいくかどうか
      ter_tsk(Tfunc);
      ter_tsk(Ttimer);
      ter_tsk(Ttimeout);
      ter_tsk(Tmusc);
      MoveTerminate();
      act_tsk(Tmain);
    }
    dly_tsk(20);
  }
}

/*
  FuncTsk
  メニュー実行用タスク
  元々のMoveTskに近い
*/
void FuncTsk(VP_INT exinf){
  wai_sem(Sdisp);
  display_clear(0);
  sig_sem(Sdisp);
  (*g_function)();
  wai_sem(Sdisp);
  display_clear(0);
  display_goto_xy(2,3);
  display_string("Press cancel");
  sig_sem(Sdisp);
  for(;;){
    //QuitTskで終了されるのを待つ
    dly_tsk(20000);
  }
}

/*
  MoveTsk
  モータの回転数左右比をPID制御する
  移動用関数から呼ばれ起動し、指定されたパワーと旋回値を元に制御する
  内側のモータのパワーのみを変化させる
*/
void MoveTsk(VP_INT exinf){
  DeviceConstants master,slave;
  int mrot,srot;
  int turn,power;
  int cur_spow;//current slave power
  double val,error=0,error_d=0,error_i=0;
  power=g_power;
  turn=g_turn;
  if(turn<0)
    turn=-turn;
  //turnの値は「外側のタイヤに対し内側のタイヤは(100-turn)%回る」という意味
  cur_spow=(100-turn)*g_power/100.0;
  get_master_slave(&master,&slave);
  for(;;){
    mrot=nxt_motor_get_count(master);
    srot=nxt_motor_get_count(slave);
    dly_tsk(MOVETSK_WAIT);
    mrot=nxt_motor_get_count(master)-mrot;
    srot=nxt_motor_get_count(slave)-srot;
    if(mrot<0)
      mrot=-mrot;
    if(srot<0)
      srot=-srot;
    //mrotとsrotの比(%)を取る 100*450/900=50
    if(mrot==0)
      mrot=1;
    val=(double)100*srot/mrot;
    error_d=error;
    if(turn>100){//信地旋回以上
      //turnが200ならvalが100になったときにerror=0
      //turnが170ならvalが70になったときにerror=0
      error=(turn-100)-val;
    }
    else{
      //turnが30ならvalが70になったときにerror=0
      //turnが90ならvalが10になったときにerror=0
      error=(100-turn)-val;
    }
    error_i+=error;
    error_d-=error;
    
    power=0;

    power+=g_pgain*error;
    power+=g_igain*error_i;
    power+=g_dgain*error_d;
    cur_spow+=power/100;
    motor_set_speed(slave,cur_spow, 1);
    g_dbg1=g_power;
    g_dbg2=cur_spow;
  }
}

/*
  TimerTsk
  残り時間を更新する
*/
void TimerTsk(VP_INT exinf){
  for(;;){
    dly_tsk(1000);
    if(g_timer>0)
      g_timer--;
    else
      break;
  }
}

/*
  CheckTsk
  進んだ距離を計測する
*/
void CheckTsk(VP_INT exinf){
  DeviceConstants master,slave;
  int rot=0;
  get_master_slave(&master,&slave);
  int rot_p=nxt_motor_get_count(master);
  
  for(;;){
    rot=nxt_motor_get_count(master)-rot_p;
    if(rot<0)
      rot=-rot;
    if((double)WHEEL_RADIUS*rot/360>g_length){
      set_flg(Fsens,efEndMove);
      break;
    }
    dly_tsk(5);
  }
}

/*
  TimeOutTsk
  移動等にかかった時間を計測
  何らかの手段で予測完了時間を渡すこと
*/
void TimeOutTsk(VP_INT exinf)
{
  int i=0;
  for(;;){
    dly_tsk(5);
    i+=5;
    if(i>=g_timeout){
      set_flg(Fsens,efTOMove);
      break;
    }
  }
}


/*
  DispTsk
  ディスプレイ制御タスク
  常にヘッダとフッタを表示し続ける
*/
void DispTsk(VP_INT exinf){
  for(;;){
    wai_sem(Sdisp);
    display_goto_xy(0,HEADER);
    display_string(MACHINE_NAME);
    display_goto_xy(10,HEADER);
    display_int(g_timer,4);
    display_goto_xy(0,FOOTER);
    display_string("---");
    display_int(g_dbg1,4);
    display_string(",");
    display_int(g_dbg2,4);
    display_update();
    sig_sem(Sdisp);
    dly_tsk(5);
  }
}

/*
  SensTsk
  センサ制御用タスク
  カラーセンサの調整・タッチセンサ用フラグ
*/
void
SensTsk(VP_INT exinf)
{
  for (;;) {
    ecrobot_process_bg_nxtcolorsensor();
    if(ecrobot_get_touch_sensor(Rtouch))
      set_flg(Fsens,efRtouch);
    else
      clr_flg(Fsens,!efRtouch);
    if(ecrobot_get_touch_sensor(Ltouch))
      set_flg(Fsens,efLtouch);
    else
      clr_flg(Fsens,!efLtouch);
    dly_tsk(2);
  }
}

/*
  MuscTsk
  音楽用タスク　改造はしない
*/
void MuscTsk(VP_INT exinf){
  for (;;) {
    play_notes(TIMING_chiba_univ, 8, i_j);
  }
}


//割り込みハンドラ　システム時間のインクリメント
void jsp_systick_low_priority(void)
{
  if (get_OS_flag()) {
    isig_tim();		// 今回はタイマを使っているのでこの呼び出しが必要
  }
}

//ECrobotのフック関数　勝手に初期化時と終了時に呼ばれるものたち

//システムの初期化時に呼ばれるもの
void ecrobot_device_initialize(void)
{
  nxt_motor_set_speed(Rmotor, 0, 1);
  nxt_motor_set_speed(Lmotor, 0, 1);
  ecrobot_init_nxtcolorsensor(Color, NXT_COLORSENSOR);
  
}

//システム停止時に呼ばれるもの
void ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(Rmotor, 0, 1);
  nxt_motor_set_speed(Lmotor, 0, 1);
  ecrobot_term_nxtcolorsensor(Color);
}
