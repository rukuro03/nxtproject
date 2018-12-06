/*
  monoatume_cfg.h内で宣言されるタスク用関数の定義
*/
#include "display.h"
#include "kernel_id.h"
#include "monoatume_cfg.h"
#include "ecrobot_base.h"
#include "button.h"
#include "music.h"
#include "graphics.h"
#include "Move.h"

//外部で定義される関数の宣言
void func_menu();//monoatume.cで定義される

//グローバル変数
static int g_timer,g_timeout;
extern void (*g_function)(void);
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
      ter_tsk(Tcheck);
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
  GetMasterSlave(&master,&slave);
  int rot_p=nxt_motor_get_count(master);
  
  for(;;){
    rot=nxt_motor_get_count(master)-rot_p;
    if(rot<0)
      rot=-rot;
    if((double)WHEEL_RADIUS*rot/360>GetLength()){
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

void SetTimeOut(int time){
  //TimeOutTskを呼ぶためのラッパーです
  g_timeout=time;
  act_tsk(Ttimeout);
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
  //タッチセンサが押されたら、次離されるまでフラグは立たない
  int rrel=0,lrel=0;
  for (;;) {
    ecrobot_process_bg_nxtcolorsensor();
    if(ecrobot_get_touch_sensor(Rtouch)){
      if(rrel==0){
	set_flg(Fsens,efRtouch);
	rrel=1;
      }
    }
    else{
      clr_flg(Fsens,!efRtouch);
      rrel=0;
    }
    if(ecrobot_get_touch_sensor(Ltouch)){
      if(rrel==0){
	set_flg(Fsens,efLtouch);
	lrel=1;
      }
    }
    else{
      clr_flg(Fsens,!efLtouch);
      lrel=0;
    }
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
  nxt_motor_set_speed(Arm, 0, 1);
  ecrobot_init_nxtcolorsensor(Color, NXT_COLORSENSOR);
  
}

//システム停止時に呼ばれるもの
void ecrobot_device_terminate(void)
{
  nxt_motor_set_speed(Rmotor, 0, 1);
  nxt_motor_set_speed(Lmotor, 0, 1);
  nxt_motor_set_speed(Arm, 0, 1);
  ecrobot_term_nxtcolorsensor(Color);
}
