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
  FLGPTN flg;
  //機能テスト用に適当に動作を指定しました
  //タイヤを紫までもっていってあわよくば青も落とす
  MoveArm(ARM_UP,30);
  flg = MLIgnoreTouch(70,0, 1300);
  flg = MLIgnoreTouch(70,6, 1350);
  MoveArm(ARM_DOWN,30);
  flg = MLIgnoreTouch(-70, 0, 50);
  MoveArm(ARM_UP,20);
  flg = MLIgnoreTouch(-70, -2, 3000);
  MoveArm(ARM_DOWN,20);
}

void RedBall(){
}

void Tire(){
}
