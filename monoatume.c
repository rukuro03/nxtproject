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
  MoveArm(ARM_UP,30);
  MoveLength(70, 0, 500);
  MoveTurn(70, 30, Lmotor);
  MoveLength(70, 0, 500);
  MoveArm(ARM_DOWN,30);
  MoveLength(-50, 0, 30);
  dly_tsk(1000);
  MoveArm(ARM_UP,30);

  //赤をとる
  MoveLength(-70, 0, 300);
  MoveTurn(70, 50, Rmotor);
  MoveArm(ARM_DOWN,30);
  MoveLength(50, 0, 500);

  //下の壁に向かう
  MoveLength(-70, 0, 300);
  MoveTurn(70, 90, Rmotor);
  MoveLength(70, 0, 2500);

  //赤をとる
  MoveLength(-70, 0, 800);
  MoveTurn(70, 90, Lmotor);
  MoveLength(70, 0, 600);
  MoveLength(50, 0, 400);

  //紫の四角に向かう
  MoveLength(-70, 0, 400);
  MoveTurn(70, 50, Rmotor);
  MoveLength(50, 0, 800);
  MoveArm(ARM_UP,30);
  MoveLength(50, 0, 50);
  MoveArm(ARM_DOWN,30);
  MoveLength(-50, 0, 50);
  dly_tsk(1000);
  MoveArm(ARM_UP,30);

  //帰る
  MoveLength(-50, 0, 700);
  MoveLength(-70, -20, 2500);
}

void RedBall(){
}

void Tire(){
}
