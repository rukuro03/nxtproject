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

void Strategy(RotateDirections dir){
  //青をとる
  LogString("Get Blue");
  ArmUp(30);
  MoveLength(50, 0, 300);
  MoveTurn(50, 25*dir);
  MLIgnoreTouch(80, 0, 500);
  dly_tsk(1000);
  ArmDown(100);

 //赤をとる
  LogString("Get Red");
  MoveCurve(-50, -40*dir, 45);
  MoveTurn(50, -90*dir);
  MLIgnoreTouch(100, 0, 1000);
  dly_tsk(1000);

  //下の壁に向かう
  LogString("Go Bottom");
  MoveLength(-70, 0, 200);
  MoveTurn(70, -60*dir);
  MLIgnoreTouch(70, 0, 2300);

  //赤をとる
  LogString("Get Red");
  MLIgnoreTouch(-50, 0, 500);
  MoveCurve(70, 40*dir, 150);
  MLIgnoreTouch(100, 0, 800);
  dly_tsk(1000);

  //紫の四角に向かう
  LogString("Go Purple");
  MoveCurve(-70,50*dir,180);
  MoveLength(-70,0,200);
  MoveTurn(70,45*dir);
  ArmUp(50);
  MLIgnoreTouch(100, 0, 1220);
  ArmDown(100);
  MoveLength(-50, 0, 100);
  ArmUp(70);
  dly_tsk(2000);
  MoveLength(30, 0, 100);
  dly_tsk(1000);
  //帰る
  LogString("Return");
  MoveLength(-70,-10*dir,1200);
  MoveLength(-70,0,1400);
}

void Block(RotateDirections dir){
  //T字ブロックをとる
  LogString("Get Block");
  SetArmUp(-30);
  ArmDown(30);
  MoveLength(100, 0, 1250);
  dly_tsk(500);
  MoveTurn(70, 91*dir);
  MoveLength(50, 0, 730);
  MoveLength(-50,0,5);
  dly_tsk(500);
  ArmUp(20);
  MoveLength(-50, 0, 400);

  //MoveTurn(50, -60*dir);
  MoveTurn(50, 70*dir);

  MoveLength(50, 0, 1150);
  ArmDown(20);

  // MoveLength(-70,-10*dir,1200);
  //MoveLength(-70,0,1200);
  MoveLength(-40,0,100);
  MoveCurve(-70,-60*dir,60);
  MoveLength(70,0,700);
  SetArmUp(ARM_DEFAULT_UP);
}

void Tire(RotateDirections dir){
  //タイヤを大きな四角にもっていく
  ArmUp(30);
  MoveLength(100, 0, 1150);
  MoveTurn(50, 90*dir);
  MoveLength(100, 0, 500);
  MoveTurn(50, -120*dir);
  MoveLength(100, 0, 1300);
  MoveLength(-100, -10*dir, 2500);
}
