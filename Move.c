/*
  移動用API
  MoveTskもここにあります
*/

#include "kernel_id.h" //タスクIDが必要
#include "Move.h"
#include "monoatume_cfg.h"
#include "Log.h"

//グローバル変数
static int g_power,g_turn,g_length;
static double g_pgain=5,g_igain=5,g_dgain=5;
// グローバル変数のゲッターです
int GetPower(){
  return g_power;
}
int GetTurn(){
  return g_turn;
}
int GetLength(){
  return g_length;
}
int GetPgain(){
  return g_pgain;
}
int GetIgain(){
  return g_igain;
}
int GetDgain(){
  return g_dgain;
}

// ゲインのセッターです
void SetPgain(int pg){
  g_pgain=pg;
}
void SetIgain(int ig){
  g_igain=ig;
}
void SetDgain(int dg){
  g_dgain=dg;
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
  int turn=GetTurn(),power=GetPower(),tmp;
  int cur_spow;//current slave power
  double pgain=GetPgain(),dgain=GetDgain(),igain=GetIgain();
  double val,error=0,error_d=0,error_i=0;
  int ireset=0;
  if(turn<0)
    turn=-turn;
  //turnの値は「外側のタイヤに対し内側のタイヤは(100-turn)%回る」という意味
  cur_spow=(100-turn)*power/100.0;
  GetMasterSlave(&master,&slave);
  for(;;){
    mrot=nxt_motor_get_count(master);
    srot=nxt_motor_get_count(slave);
    dly_tsk(MOVETSK_WAIT);
    mrot=nxt_motor_get_count(master)-mrot;
    srot=nxt_motor_get_count(slave)-srot;
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

    //エラーの方向補正
    if(power<0 && turn<100)
      error=-error;
    else if(power>=0 && turn>=100)
      error=-error;

    error_i+=error;
    error_d=error-error_d;
    
    tmp=0;    
    tmp+=pgain*error;
    tmp+=igain*error_i;
    tmp+=dgain*error_d;
    cur_spow+=tmp/100;
    // 逆走防止
    if(isReverse==1 && cur_spow>0)
      cur_spow=0;
    if(isReverse==0 && cur_spow<0)
      cur_spow=0;
    
    motor_set_speed(slave,cur_spow, 1);
    LogInt(error);
    ireset+=MOVETSK_WAIT;
    if(ireset>2000){
      error_i=0;
      ireset=0;
    }
  }
}

void GetMasterSlave(DeviceConstants* master,DeviceConstants* slave){
  //マスター(主モータ)：外側のモータ
  //スレーブ(従モータ)：内側のモータ を取得する
  
  if(g_turn<0){
    *master=Rmotor;
    *slave=Lmotor;
  }
  else{
    *master=Lmotor;
    *slave=Rmotor;
  }
}

void MoveSetPower(int pow){
  //パワーの登録…とはいえ、g_powに代入するだけです
  g_power=pow;
  motor_set_speed(Rmotor,pow/4,1);
  motor_set_speed(Lmotor,pow/4,1);
  dly_tsk(50);
  motor_set_speed(Rmotor,pow/2,1);
  motor_set_speed(Lmotor,pow/2,1);
  dly_tsk(50);
  motor_set_speed(Rmotor,pow,1);
  motor_set_speed(Lmotor,pow,1);
}

void MoveSetSteer(int turn){
  //ステアリングの登録…とはいえ、g_turnに代入するだけです
  DeviceConstants master,slave;
  int power;
  g_turn=turn;
  GetMasterSlave(&master,&slave);
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

void CheckLength(int length){
  //移動距離の登録…とはいえ、g_lengthに代入してCheckTskを起動するだけです
  //ついでにここでモータの回転角度を0に初期化します
  g_length=length;
  nxt_motor_set_count(Rmotor,0);
  nxt_motor_set_count(Lmotor,0);
  act_tsk(Tcheck);
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

FLGPTN MLIgnoreTouch(int pow,int turn,int length){
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
  sensor=WaitForOR(efEndMove | efTOMove);
  MoveTerminate();
  return sensor;
}

FLGPTN MoveTurn(int pow,int turn,DeviceConstants slave){
  //パワー・旋回角度・内側のモータを指定して信地旋回します
  
  //外側のモータが進まないと行けない距離 タイヤ間距離を半径とする円周
  int length=SHAFT_LENGTH*2*3.14*turn/360;
  if(slave==Lmotor)//右回転
    return MoveLength(pow,100,length);
  else //左回転
    return MoveLength(pow,-100,length);
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



