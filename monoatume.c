#include "display.h"
#include "monoatume_cfg.h" //t_service+monoatume
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include "ecrobot_base.h"
#include "button.h"
#include "graphics.h"
//自作API用ヘッダファイル
#include "Log.h"
#include "Move.h"
#include "Arm.h"
#include "Menu.h"

//グローバル変数には g_ のプレフィックスをつけてください

void Strategy(){
  //青をとる
  LogString("Get Blue");
  ArmUp(30);
  MoveLength(50, 0, 300);
  MoveTurn(50, 25, Lmotor);
  MLIgnoreTouch(80, 0, 500);
  dly_tsk(1000);
  ArmDown(100);

 //赤をとる
  LogString("Get Red");
  MoveCurve(-50, -40, 45);
  MoveTurn(50, 90, Rmotor);
  MLIgnoreTouch(100, 0, 1000);
  dly_tsk(1000);

  //下の壁に向かう
  LogString("Go Bottom");
  MoveLength(-70, 0, 200);
  MoveTurn(70, 60, Rmotor);
  MLIgnoreTouch(70, 0, 2300);

  //赤をとる
  LogString("Get Red");
  MLIgnoreTouch(-50, 0, 500);
  MoveCurve(70, 40, 150);
  MLIgnoreTouch(100, 0, 800);
  dly_tsk(1000);

  //紫の四角に向かう
  LogString("Go Purple");
  MoveCurve(-70,50,180);
  MoveLength(-70,0,200);
  MoveTurn(70,45,Lmotor);
  ArmUp(30);
  MLIgnoreTouch(50, 0, 1000);
  ArmDown(100);
  MoveLength(-50, 0, 100);
  MoveLength(50, 0, 100);
  dly_tsk(1000);
  ArmUp(30);
  //帰る
  LogString("Return");
  MoveLength(-70,-10,1200);
  MoveLength(-70,0,1200);
}

void Block(){
  //T字ブロックをとる
  SetArmUp(-30);
  ArmDown(30);
  MoveLength(50, 0, 1250);
  MoveTurn(50, 95, Lmotor);
  MoveLength(50, 0, 900);
  MoveLength(-30,0,20);
  ArmUp(30);
  MoveLength(-50, 0, 300);
  MoveTurn(50, 60, Rmotor);
  MoveLength(50, 0, 1100);
  ArmDown(10);
  MoveLength(-70,-10,1200);
  MoveLength(-70,0,1200);
  SetArmUp(ARM_DEFAULT_UP);
}

void Tire(){
  //タイヤを大きな四角にもっていく
  ArmUp(30);
  MoveLength(70, 0, 1150);
  MoveTurn(50, 95, Lmotor);
  MoveLength(70, 0, 500);
  MoveTurn(50, 120, Rmotor);
  MoveLength(70, 0, 1300);
  MoveLength(-70, -10, 2500);
}

void ReNormal(){
  //逆 通常
}
void ReBlock(){
  //逆 ブロック
}
void ReTire(){
  //逆 タイヤ
}
