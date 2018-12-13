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
  //しばらくまっすぐすすんで
  act_tsk(Ttimer);
  act_tsk(Tmusc);

  //サンプル
  //緑出てはいるだけ
  /*LogString("Start");
  flg = MoveLength(40,0,100);
  LogString("Turn right");
  flg = MoveLength(50, 200, 100);
  LogString("Turn left");
  flg = MoveLength(50,-200,100);
  LogString("Go back");
  flg = MoveLength(70,0,250);
  LogString("and forth");
  flg = MoveLength(-70,0,250);
  //アームを上げる
  MoveArm(ARM_UP);*/

  //ちゃんと集めるやつ
  //赤ボールを取る
  /* LogString("Red boll");
  flg = MLIgnoreTouch(70, 0, 900);
  //バックする
  flg = MoveLength(-70, 40, 250);
  //タイヤを取りに行く
  flg = MoveLength(50, 0, 400);
  //タイヤの前で 3秒停止　アームを上げる　ゆっくり前に進む
  dly_tsk(2000);
  MoveArm(ARM_UP);
  flg = MoveLength(20, 0, 100);
  //方向転換
  MoveArm(ARM_DOWN);
  flg = MoveLength(70, -200, 130);
  //スタート位置に戻る
  flg = MLIgnoreTouch(70, 0, 700);
  flg = MoveLength(-70, 0, 20);*/

  //タイヤを紫までもっていってあわよくば青も落とす
  MoveArm(ARM_UP);
  flg = MLIgnoreTouch(70,0, 1300);
  flg = MLIgnoreTouch(70,6, 1350);
  MoveArm(ARM_DOWN);
  flg = MLIgnoreTouch(-70, 0, 50);
  MoveArm(ARM_UP);
  flg = MLIgnoreTouch(-70, -2, 3000);
  MoveArm(ARM_DOWN);
}
