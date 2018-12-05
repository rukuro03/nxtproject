/*
  移動用API
*/

#include "kernel_id.h" //タスクIDが必要
#include "monoatume_cfg.h"

//グローバル変数
static int g_power,g_turn,g_length;
static double g_pgain=1,g_igain=1,g_dgain=1;

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

FLGPTN MoveTurn(int pow,int turn,DeviceConstants slave){
  //パワー・旋回角度・内側のモータを指定して信地旋回します
  
  //外側のモータが進まないと行けない距離 タイヤ間距離を半径とする円周
  int length=SHAFT_LENGTH*2*3.14*turn/360;
  if(slave==Rmotor)//右回転
    return MoveLegth(pow,100,length);
  else //左回転
    return MoveLegth(pow,-100,length);
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
