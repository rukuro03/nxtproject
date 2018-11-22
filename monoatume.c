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

typedef void (*MFunc)(void);
typedef struct _NameFunc {
  char *name;
  MFunc func;
} NameFunc;

void strategy();
int get_touch();
void get_master_slave(DeviceConstants*,DeviceConstants*);
void func_calib();
void strategy();
void func_menu();
void MovePower(int);
void MoveSteer(int);
void CheckLenght(int);
void SetTimeOut(int);
FLGPTN MoveLength(int,int,int);
void MoveArm(int);

//グローバル変数群　できれば使いたくないが組み込みだからね
//グローバル変数には g_ のプレフィックスをつけてください
int g_timer,g_power,g_turn,g_length;
int g_armdown;
int g_timeout;
int g_pgain=5,g_igain=1,g_dgain=2;
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
  //スレーブ(従モータ)；内側のモータ を取得する
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
  int prev_rot=0,rot=0;
  //色のカリブレーション
  //アームのカリブレーション
  //しくじった　どっちが上かわからん
  motor_set_speed(Arm,10,0);
  for(;;){
    dly_tsk(10);
    rot=nxt_motor_get_count();
    if(rot-prev_rot<1){
      nxt_motor_set_count(Arm,0);//一番上に上げた状態でカウントを0にする
      break;
    }
    prev_rot=rot;
  }
  motor_set_speed(Arm,-10,0);
  for(;;){
    dly_tsk(10);
    rot=nxt_motor_get_count();
    if(rot-prev_rot<1){
      g_armdown=rot;
      break;
    }
    prev_rot=rot;
  }
}

void strategy(){
  act_tsk(Tmove);
  MoveLength(100,200,1000);
  //右に回って
  MoveLength(100,-200,1000);
  //左に回って
  MoveLength(20,0,100);
  MoveLength(-20,0,100);
  //前後にぷるぷる
  MoveArm(g_armdown);
}

void func_menu(){
  NameFunc MainMenu[] = {
    {"Calibration", func_calib},	// センサーのキャリブレーション
    {"SetStrategy",strategy},
    {"Start", NULL},			// ライントレースの開始
    {"Exit", ecrobot_restart_NXT},	// OSの制御に戻る
    {"Power Off", ecrobot_shutdown_NXT},	// 電源を切る
  };
  int cnt=ARRAYSIZE(MainMenu);
  int i;
  nxtButton btn;
  static int menu = 0;
  for (;;) {
    display_clear(0);
    for (i = 0; i < cnt; i++) {
      if (i == menu) {
        nxt_display_dish(nxt_display_set, 6, i * 8 + 3, 3);
      }
      display_goto_xy(2, i+1);
      display_string(MainMenu[i].name);
    }
    display_update();
    btn = get_btn();
    switch (btn) {
    case Obtn:	// オレンジボタン == 選択
      if (MainMenu[menu].func == NULL)
	return;
      g_function=MainMenu[menu].func;
      continue;
    case Cbtn:	// グレーボタン == キャンセル
      continue;
    case Rbtn:	// 右ボタン == 次へ
      menu++;
      if (menu >= cnt) menu = 1;
      continue;
    case Lbtn:	// 左ボタン == 前へ
      --menu;
      if (menu <= 0) menu = cnt - 1;
      continue;
    default:	// 複数が押されている場合
      continue;
    }
    break;
  }
}

//移動用関数群
void MovePower(int pow){
  //パワーの登録…とはいえ、g_powに代入するだけです
  g_power=pow;
  motor_set_speed(Rmotor,pow);
  motor_set_speed(Lmotor,pow);
}
void MoveSteer(int turn){
  //ステアリングの登録…とはいえ、g_turnに代入するだけです
  DeviceConstants master,slave;
  int power;
  g_turn=turn;
  get_master_slave(&master,&slave);
  if(turn<0)
    turn=-turn;
  if(turn>100)//信地旋回以上
    power=(double)(100-turn)*g_power/100;
  else
    power=(double)(turn-100)*g_power/100;
  motor_set_speed(slave,power,0);
}

void CheckLength(int length){
  //移動距離の登録…とはいえ、g_lengthに代入してCheckTskを起動するだけです
  g_length=length;
  act_tsk(Tcheck);
}
void SetTimeOut(int time){
  //時間切れの登録…とはいえ、g_timeoutに代入してTimeOutTskを起動するだけです
  g_timeout=time;
  act_tsk(Ttimeout);
}

FLGPTN MoveLength(int pow,int turn,int length){
  FLGPTN sensor;
  //turn:-200~200
  //turnがマイナスだと右がマスター左がスレーブ
  //turnがプラスだと左がマスター右がスレーブ
  MovePower(power);
  MoveSteer(turn);
  CheckLength(length);
  /*
    完了/時間切れ/左右どちらかのタッチセンサが押される
    のいずれかまで待つ
  */
  wai_flg(Fsens,efEndMove | efTOMove | efRtouch | efLtouch, TWF_ORW, &sensor);
  g_power=0;
  g_turn=0;
  return sensor;
}

void MoveArm(int deg){
  //最も上に上げたアームの状態から何度曲げるかを指定
  
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
  act_tsk(Tbutton);
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
}

/*
  QuitTsk
  メニュー項目強制終了用タスク
  ボタン押したら強制終了させ、初期化処理を行う
  なんか終わらせるタスクがやたらと多いんですが
*/
void QuitTsk(VP_INT exinf){
  nxtButton btn;
  btn=get_btn();
  if(btn==Cbtn){
    //うーん　起動されるかされないかわからないタスクが多いんですが、
    //それも無理やりter_tskしてます。果たしてうまくいくかどうか
    ter_tsk(Tfunc);
    ter_tsk(Tmove);
    ter_tsk(Ttimer);
    ter_tsk(Ttimeout);
    act_tsk(Tmain);
  }
}

/*
  FuncTsk
  メニュー実行用タスク
  元々のMoveTskに近い
*/
void FuncTsk(VP_INT exinf){
  (*g_function)();
  for(;;){
    display_goto_xy(2, 3);
    display_string("Press Cancel Button");
    //QuitTskで終了されるのを待つ
  }
}

/*
  MoveTsk
  モータの回転数左右比をPID制御する
  移動用関数から呼ばれ起動し、指定されたパワーと旋回値を元に制御する
  slaveの値のみを変化させる
*/
void MoveTsk(VP_INT exinf){
  DeviceConstants master,slave;
  int mrot,srot;
  int turn,power;
  double val,error=0,error_d=0,error_i=0;
  for(;;){
    power=g_power;
    turn=g_turn;
    if(turn<0)
      turn=-turn;
    //turnは"外側の車輪の回転角が内側の車輪の回転角のturn％に"を表す数字
    get_master_slave(&master,&slave);

    mrot=nxt_motor_get_count(master);
    srot=nxt_motor_get_count(slave);
    //mrotとsrotの比(%)を取る 100*450/900=50
    val=(double)100*srot/mrot;
    error_d=error;
    if(turn>100){//信地旋回以上
      //turnが200ならvalが100になったときにerror=0
      //turnが170ならvalが70になったときにerror=0
      error=val-(turn-100);
    }
    else{
      //turnが30ならvalが70になったときにerror=0
      //turnが90ならvalが10になったときにerror=0
      error=val-(100-turn);
    }
    error_i+=error;
    error_d-=error;
    
    power=0;
    power+=g_pgain*error;// P
    power+=g_igain*error_i;// I
    power+=g_dgain*error_d;// D
    motor_set_speed(slave,g_power+power, 0);
    dly_tsk(10);
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
  int move=0,rot=0;
  get_master_slave(&master,&slave);
  int rot_p=nxt_motor_get_count(master);
  
  for(;;){
    rot=nxt_motor_get_count(master)-rot_p;
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
    display_goto_xy(0,0);
    display_string(MACHINE_NAME);
    display_goto_xy(10,0);
    display_int(g_timer,4);
    display_goto_xy(0,6);
    display_string("FOOTER");
    display_update();  
    dly_tsk(5);
  }
}

/*
  SensTsk
  センサ制御用タスク
  カラーセンサの調整・タッチセンサ用フラグ
*/
void
ColsTsk(VP_INT exinf)
{
  int touch;
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
    play_notes(TIMING_chiba_univ, 8, chiba_univ);
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
  nxt_motor_set_speed(Rmotor, 0, 0);
  nxt_motor_set_speed(Lmotor, 0, 0);
  ecrobot_init_nxtcolorsensor(Color, NXT_COLORSENSOR);
  
}

//システム停止時に呼ばれるもの
void ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(Rmotor, 0, 1);
  nxt_motor_set_speed(Lmotor, 0, 1);
  ecrobot_term_nxtcolorsensor(Color);
}
