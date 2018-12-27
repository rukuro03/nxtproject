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
  ArmUp(30);
  MoveLength(50, 0, 300);
  MoveTurn(50, 25, Lmotor);
  MLIgnoreTouch(80, 0, 500);
  dly_tsk(1000);
  ArmDown(30);

 //赤をとる
  MoveLength(-50, 0, 300);
  MoveTurn(50, 70, Rmotor);
  MLIgnoreTouch(100, 0, 1000);
  //MoveTurn(50, 60, Rmotor);
  //MoveLength(70, 0, 600);
  dly_tsk(1000);

  //下の壁に向かう
  MoveLength(-70, 0, 200);
  MoveTurn(70, 60, Rmotor);
  MLIgnoreTouch(70, 0, 2500);

  //赤をとる
  MoveLength(-50, 0, 500);
  MoveTurn(50, 90, Lmotor);
  MoveLength(50, 0, 600);
  MoveLength(50, 30, 400);
  dly_tsk(1000);

  //紫の四角に向かう
  MoveLength(-70, 0, 400);
  MoveTurn(70, 50, Rmotor);
  MoveLength(50, 0, 800);
  ArmUp(30);
  MoveLength(50, 0, 50);
  ArmDown(30);
  MoveLength(-50, 0, 50);
  dly_tsk(1000);
  ArmUp(30);

  //帰る
  MoveLength(-50, 0, 700);
  MoveLength(-70, -20, 2500);
}

void RedBall(){
  //T字ブロックをとる
  ArmDown(30);
  MoveLength(50, 0, 1200);
  MoveTurn(50, 90, Lmotor);
  MoveLength(50, 0, 900);
  ArmUp(30);
  MoveLength(-50, 0, 300);
  MoveTurn(50, 60, Rmotor);
  MoveLength(50, 0, 900);
  ArmDown(30);
  MoveLength(-70, 0, 2500);
}

void Tire(){
  //タイヤを大きな四角にもっていく
  ArmUp(30);
  MoveLength(70, 0, 1100);
  MoveTurn(50, 90, Lmotor);
  MoveLength(70, 0, 500);
  MoveTurn(50, 100, Rmotor);
  MoveLength(70, 0, 1300);
  MoveLength(-70, 0, 2500);
}

void Re(){
  //逆

  //青をとる
  ArmUp(30);
  MoveLength(50, 0, 300);
  MoveTurn(50, 25, Rmotor);
  MLIgnoreTouch(80, 0, 500);
  dly_tsk(1000);
  ArmDown(30);

 //赤をとる
  MoveLength(-50, 0, 300);
  MoveTurn(50, 70, Lmotor);
  MLIgnoreTouch(100, 0, 1000);
  //MoveTurn(50, 60, Rmotor);
  //MoveLength(70, 0, 600);
  dly_tsk(1000);

  //下の壁に向かう
  MoveLength(-70, 0, 200);
  MoveTurn(70, 60, Lmotor);
  MLIgnoreTouch(70, 0, 2500);

  //赤をとる
  MoveLength(-50, 0, 500);
  MoveTurn(50, 90, Rmotor);
  MoveLength(50, 0, 600);
  MoveTurn(50, 30, 400);
  dly_tsk(1000);

  //紫の四角に向かう
  MoveLength(-70, 0, 400);
  MoveTurn(70, 50, Rmotor);
  MoveLength(50, 0, 800);
  ArmUp(30);
  MoveLength(50, 0, 50);
  ArmDown(30);
  MoveLength(-50, 0, 50);
  dly_tsk(1000);
  ArmUp(30);

  //帰る
  MoveLength(-50, 0, 700);
  MoveLength(-70, -20, 2500);


}
